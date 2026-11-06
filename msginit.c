/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Initialize Process's Fifo Message Functions
 *
 *	This file defines the following functions.
 *		MsgInit( );
 ******************************************************************************/
#include	"msglocal.h"

static	char	open_err[] = "MsgInit: Cannot Open Input %s, errno=%u\n";
static	char	map_err[] = "MsgInit: Cannot Map Input %08X.mem Memory\n";

/* FUNCTION:	MsgInit
 *
 *	DESCRIPTION:
 *		This subroutine will initialize the message system for the
 *		current process.  If the "adr" (current address) argument is
 *		zero, then this subroutine assumes this process will only send
 *		messages and NOT receive them.  If "adr" is non-zero then this
 *		subroutine will create a named FIFO/pipe for this process, if
 *		one does not exist.  Also, the FIFO will be openned and attached
 *		to file descriptor zero (stdin).
 *
 *	INPUTS:
 *		adr =	The Current Process's Message FIFO Address
 *		quit =	Pointer To Process's Termination Function
 *		prnt =	Pointer To Process's Message Dump Subroutine
 *
 *	RETURNS:
 *		Nothing
 *
 *	ON ERROR:
 *		This subroutine will log the error to the printer then it
 *		will wait 30 seconds after that it terminates the current
 *		process.
 */

void	MsgInit( u32 adr, void (*quit)( Msg_p ), void (*prnt)( Msg_p ) )
{
	pid_t	pid;
	char *	cp;
	Msg_t *	m;

	
	if( my_pid == ( pid = getpid( ) ) ) return;	/* Already Initialized! */

	if( ( cp = getenv( "TraceLevel" ) ) != (char *)0 ) {
		TraceLevel = strtol( cp, 0, 16 );
	}
/*
 *	Set Message Address Information
 */
	if( adr != 0 ) {
		my_adr = adr;
		my_mch = MsgMachine(	adr );
		my_gid = MsgGroup(	adr );
		my_typ = MsgProgram(	adr );
		my_sub = MsgSubTask(	adr );
		MsgA2N( my_adr, my_name );
	}

	my_pid = pid;
/*
 *	Create Kill Script File
 */
	if( my_gid != GRP_PID ) CreateKillScript( 0 );
/*
 *	Initialize Message Send Tables
 */
	snd_ok = 1;
/*
 *	Initialize Message Receive Pipe
 */
	if( adr != 0 ) {
		File_Close( 0 );
		if( File_Open(	MsgPath( my_adr, my_pipe ),
				O_CREAT | O_APPEND | O_RDWR,
				FAR_FIFO | 0666 ) ) {
			FatalError(	open_err,
					my_pipe,
					errno );
		}
		if( ( m = Msg_Map( ( my_buf = my_adr ) ) ) == NULL ) {
			FatalError(	map_err,
					my_adr
			);
		}
#ifdef	JUNK
		int	i;
		for( i = 0; i < MSG_MAX_BUF; i++, m++ ) {
			m->head.pid = 0;
			m->head.sync = 0;
		}
#endif
		my_quit  = quit;
		my_print = prnt;
		rcv_ok = 1;
	}
	return;
}
