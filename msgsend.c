/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 *
 *	Sends A Message To A Named Fifo(Pipe)!
 *
 *	This file defines the following functions.
 *		MsgSend( );
 ******************************************************************************/
#include	"msglocal.h"

static	char	func[ ]	= "MsgSend";
static	int	flags	= O_CREAT | O_WRONLY | O_APPEND | O_NDELAY;
static	int	pmode	= FAR_FIFO | 0666;

static	int	chk_fd( u32 adr )
{
	fard_t	*	ft;
	struct	stat	s;

	if( stat(  MsgPath( adr, 0 ), &s ) == -1 ) return( -1 );
	for( ft = File_Table; ft < &File_Table[ MAX_FD ]; ft++ ) {
		if( s.st_ino != ft->s.st_ino ) continue;
		return( ft->fd );
	}
	return( -1 );
}

static	u32	get_adr( u32 adr )
{
	int		grp;
	int		mch;
	u32		alt;
	struct	stat	s;

	if( stat(  MsgPath( adr, 0 ), &s ) != -1 &&
	    stat(  MsgFile( adr, 0 ), &s ) != -1 ) return( adr );

	if( ( mch = MsgMachine( adr ) ) == 0 ) mch = my_mch;
	if( my_mch != mch ) {
		return( MsgFullAddress( mch, 255, 255, 255 ) );
		return( alt );
	}

	grp = MsgGroup( adr );
	if( (uchar)grp == GRP_PID ) return( adr );

	if( my_gid != grp && grp == 0 ) grp = my_gid;
	if( my_gid != grp ) {
		alt = MsgFullAddress( mch, grp, 255, 255 );
		return( alt );
	}
	return( adr );
}

static	int	get_fd( u32 alt )
{
	int		fd;

	fd  = chk_fd(  alt );

	if( fd != -1 ) {
		Trace(	MSG_TRC_LVL,
			"MsgSend: Found FD%u For Address %08X",
			fd,
			alt
		);
		return( fd );
	}

	if( ( fd = File_Open( MsgPath( alt, 0 ), flags, pmode ) ) != -1 ) {
		Trace(	MSG_TRC_LVL,
			"MsgSend: Opened FD%u For Address %08X",
			fd,
			alt
		);
		return( fd );
	}

	switch( errno ) {
	case ENXIO:	return( -1 );
	default:	break;
	}	

	SystemError(	func,
			"Error Opening Output Address 0x%08X",
			alt
	);
	return( -1 );
}

/* FUNCTION:	MsgSend
 *
 *	DESCRIPTION:
 *		This subroutine will transmit a message to the addressed
 *		fifo.
 *
 *	INPUTS:
 *		adr =	target process address
 *		f =	function index number
 *		m =	message buffer pointer
 *		l =	message length
 *		e =	seconds until message expires
 *
 *	RETURNS:
 *		nothing
 *
 *	ON ERROR:
 *		This subroutine will log the error to the printer then it
 *		will wait 30 seconds after that it terminates the current
 *		process.
 */

void	 MsgSend( u32 adr, s32 f, void * m, u32 l, u32 e )
{
static	u16	buf;
	int	i;
	int	len;
	int	retry;
	int	fd;

	if( !snd_ok ) MsgInit( 0, 0, 0 );

	if( !MsgProxy ) ( (Msg_t *)m )->head.target	= adr;
	( (Msg_t *)m )->head.function	= f;
	if( l > sizeof( Msg_t ) ) l = sizeof( Msg_t );
	( (Msg_t *)m )->head.length	= l;

	adr = get_adr( adr );

	if( ( i = MsgSetBuffer( adr, (Msg_t *)m ) ) == -1 ) {
		Trace(	0,
			"MsgSend: Zero buffers for %s FIFO",
			MsgA2N( adr, 0 )
		);
		if( e != 0 ) {
			FatalError(
				"MsgSend: Zero buffers for %s FIFO",
				MsgA2N( adr, 0 )
			);
		}
		EventLog(
			"MsgSend: Zero buffers for %s FIFO",
			MsgA2N( adr, 0 )
		);
		return;
	}

	buf = (u16)i;

	/*
	 *	Send The Message via FIFO
	 */

	for( len = -1, retry = 0; retry < 3; retry++, len = -1, sched_yield( ) ) {

		if( ( fd = get_fd( adr ) ) == -1 ) continue;

		errno = 0;
		if( ( len = File_Write( fd, (char *)&buf, sizeof( buf ) ) ) == 2 ) {
			Trace(	MSG_TRC_LVL,
				"MsgSend: Sent To %s, func %u, size %u",
				MsgA2N( adr, 0 ),
				f,
				l
			);
			return;
		}
		if( len == -1 ) {
			switch( errno ) {
			case EPIPE:	File_Close( fd );
			case EAGAIN:	continue;
			default:
					MsgFreeBuffer( adr, buf );
					SystemError(	func,
							"Cannot write FIFO %s",
							File_Table[ fd ].name
					);
					return;
			}
		}
	}
	if( len == 0 ) {
		MsgFreeBuffer( adr, buf );
		Trace(	0,
			"MsgSend: Zero write on FIFO %s",
			File_Table[ fd ].name
		);
		if( e != 0 ) {
			FatalError(
				"MsgSend: Zero write on FIFO %s",
				File_Table[ fd ].name
			);
		} else {
			EventLog(
				"MsgSend: Zero write on FIFO %s",
				File_Table[ fd ].name
			);
		}
		return;
	}
	if( len != -1 ) {
		MsgFreeBuffer( adr, buf );
		EventLog(
			"MsgSend: Short write to FIFO %s, want %u, got %u",
			File_Table[ fd ].name,
			sizeof( buf ),
			len
		);
		return;
	}
	MsgFreeBuffer( adr, buf );
	return;
}
