/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Stub Process To Read Messages From Named Fifo(Pipe)!
 *
 *	This file defines the following private functions.
 *		main( );
 ******************************************************************************/
#include	"msglocal.h"

/* FUNCTION:	main
 *
 *	DESCRIPTION:
 *		This routine reads messages from FIFO and writes them
 *		to the stderr output
 *
 *	INPUTS:
 *		ac =	pointer to format string
 *		av =	variable list of arguments
 *
 *	RETURNS:
 *		Nothing
 *
 *	ON ERROR:
 *		Exits
 */

int	main( int ac, char * av[ ] )
{
	s32	func;
	s32	tout;
	s32	secs;
	Msg_t	msg;
	char	buf[ 64 ];

	tout = -1;
	switch( ac ) {
	case 4:		MsgRelay = 1;
	case 3:		tout = strtol( av[ 2 ], 0, 10 );
	case 2:		break;
	default:	SysPrt(	"Usage: %s my.msg.sys.adr [timeout]\n",
				av[ 0 ]
			);
			exit( 1 );
	}
//	TraceLevel = MSG_TRC_LVL;
	TraceLevel = 0;
	MsgInit( MsgN2A( av[ 1 ] ), 0, 0 );
	SysPrt( "Local Address = %s(0x%08X)\n", my_name, my_adr );
	for( secs = 0; ; ) {
		switch( ( func = MsgRead( &msg, tout ) ) ) {
		case MSG_TIMEOUT:
			secs += tout;
			SysPrt( "Message Read Timeout! %d", tout );
			if( tout != 0 ) break;
			SysPrt( "Sleeping! %d", tout );
			sleep( 1 );
			break;
		default:
			SysPrt( "Recieved message #%d from %s(0x%08X)",
				func,
				MsgA2N( msg.head.source, buf ),
				msg.head.source
			);
			break;
		}
	}
	exit( 0 );
}
