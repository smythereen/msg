/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Read Message From Named Fifo(Pipe)!
 *
 *	This file defines the following puplic functions.
 *		MsgInternal( );
 *
 *	This file defines the following private functions.
 *		reopen( );
 ******************************************************************************/
#include	"msglocal.h"

	int	MsgBypass = 0;

static	void	reopen( int fd, char * file )
{
	int	new;

	File_Close( fd );
	new = File_Open( file, O_WRONLY|O_CREAT|O_APPEND, 0666 );
	if( new == -1 ) {
		new = File_Open( "/dev/null", O_WRONLY );
	}
	if( new != fd ) {
		FatalError(
			"MsgInternal: Re-opened wrong FD%u, should be FD%u!",
			new,
			fd
		);
	}
	return;
}

/* FUNCTION:	MsgInternal
 *
 *	DESCRIPTION:
 *		This subroutine processes internal system messages
 *
 *	INPUTS:
 *		m =	message buffer pointer
 *
 *	RETURNS:
 *		Nothing
 *
 *	ON ERROR:
 *		This subroutine will log an error and then it will wait 30
 *		seconds after that it terminates the current process.
 */

Msg_t	*	MsgInternal( Msg_t * x )
{
MsgIn_t	*	m;
	int	i;

	m = (MsgIn_t *)MsgDumper( x );

	if( MsgRelay && x->head.target != my_adr ) return( x );

	if( x->head.function >= 0 ) return( (MsgBypass ? (Msg_t *)0 : x) );

	switch( m->h.function ) {

	case	_Timeout:	return( x );

	case	_Trace:
		if( TracePointer != NULL ) {
			if( *TracePointer != m->b.t.newlevel ) {
				*TracePointer = m->b.t.newlevel;
				Trace(	MSG_TRC_LVL,
					"MsgInternal: New Trace Level=0x%08X!",
					*TracePointer
				);
			}
		} else if( TraceLevel != m->b.t.newlevel ) {
			TraceLevel = m->b.t.newlevel;
			Trace(	MSG_TRC_LVL,
				"MsgInternal: New Trace Level=0x%08X!",
				TraceLevel
			);
		}
		if( m->b.t.file[ 0 ] == 0 ) break;
		reopen( 2, m->b.t.file );
		Trace(	MSG_TRC_LVL,
			"MsgInternal: New Trace File=\"%s\"!",
			m->b.t.file
		);
		break;

	case	_Quit:
		EventLog(
			"MsgInternal: Received Quit Request From pid=%u",
			m->h.pid
		);
		if( !MsgBypass && my_quit != (void (*)( Msg_p ))0 ) my_quit( &m->h );
		exit( m->b.q.exit );
		break;
	
	case	_Stub:
		MsgBypass = ( m->b.s.pass  == '1' );
		MsgDumpIt = ( m->b.s.print == '1' );
		Trace(	MSG_TRC_LVL,
			"MsgInternal: Msg Bypass = %u, Msg Dump = %u.",
			MsgBypass,
			MsgDumpIt
		);
		if( m->b.s.file[ 0 ] == 0 ) break;
		reopen( 2, m->b.s.file );
		Trace(	MSG_TRC_LVL,
			"MsgInternal: New Trace File=\"%s\"!",
			m->b.t.file
		);
		break;

	case	_Peer:
		if( ( i = m->b.p.peers ) < 1 ) break;	/* Do Nothing! */
		if( i > 4 ) i = 4;			/* Max Of 4 Peers At One Time */
		for( ; i > 0; i-- ) {
			if( fork( ) != 0 ) continue;
			my_pid = getpid( );
			Trace(	MSG_TRC_LVL,
				"MsgInternal: Created New Peer Process pid=%u.",
				my_pid
			);
			break;
		}
		break;

	default:
		Trace(	MSG_TRC_LVL,
			"MsgInternal: Unknown message type %d from pid=%u!",
			m->h.function,
			m->h.pid );
		break;
	}
	return( (Msg_t *)0 );
}
