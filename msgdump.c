/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Read Message From Named Fifo(Pipe)!
 *
 *	This file defines the following puplic functions.
 *		MsgDumper( );
 ******************************************************************************/
#include	"msglocal.h"

	int	MsgDumpIt = 0;

/* FUNCTION:	MsgDumper
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

Msg_t	*	MsgDumper( Msg_t * x )
{
MsgIn_t	*	m;

	if( !MsgDumpIt ) return( x );

	m = (MsgIn_t *)x;
	SysPrt(	"Message from %9s, pid %6u, length = %3u",
			MsgA2N( m->h.source, 0 ),
			m->h.pid,
			m->h.length );
	if( !( m->h.function & _Internal ) ) {
		if( my_print != (void (*)(Msg_t *))0 ) my_print( &m->h );
		return( x );
	}

	switch( m->h.function ) {

	case _Trace:
		SysPrt( " Internal: New trace level is %d, old level was %d",
			m->b.t.newlevel,
			TraceLevel );
		if( m->b.t.file[0] != 0 ) {
			SysPrt( "\t   New output = %s", m->b.t.file );
		}
		return( x );

	case _Quit:
		SysPrt( " Internal: Request to quit( %u )",
			m->b.q.exit );
		return( x );

	case _Stub:
		SysPrt( " Internal: Turn Message Dump %s",
				( m->b.s.print != '1' ? "OFF" : "ON" ) );
		SysPrt( "\t   Turn Message Bypass %s",
				( m->b.s.pass != '1' ? "OFF" : "ON" ) );
		if( m->b.s.file[0] != 0 ) {
			SysPrt( "\t   New Output = %s", m->b.s.file );
		}
		return( x );

	default:
		SysPrt( " Internal: Unknown Message Function %u?",
			  m->h.function );
		return( x );
	}
}
