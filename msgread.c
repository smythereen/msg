/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Read Message From Fifo(Pipe) or Directory!
 *
 *	This file defines the following puplic functions.
 *		MsgRead( );
 ******************************************************************************/
#include	"msglocal.h"

/* FUNCTION:	MsgRead
 *
 *	DESCRIPTION:
 *		This subroutine will read the next message from the process's
 *		FIFO and place it into the user supplied buffer.
 *
 *	INPUTS:
 *		msg =	message buffer pointer
 *		timeout = seconds for before read will timeout.
 *			 -1 waits forever for a message
 *			  0 no wait (poll for a message)
 *
 *	RETURNS:
 *		-1 on read timeout
 *		       or
 *		Message's function number
 *
 *	ON ERROR:
 *		This subroutine will log an error and then it will wait 30
 *		seconds after that it terminates the current process.
 */

s32	MsgRead( Msg_t * msg, s32 timeout )
{
static	Msg_t	*	m;

	if( !rcv_ok ) {
		FatalError( "MsgRead: System not initialized!" );
	}
	for( ; ; ) {
		m = MsgPipe( timeout );
		/*
		 *	Return On Timeout
		 */
		if( m == 0 ) return( _Timeout );
		/*
		 *	Process Internal Message.
		 */
		if( ( m = MsgInternal( m ) ) == 0 ) continue;

		if( m != msg ) MsgCopy( msg, m );

		return( msg->head.function );
	}
}
