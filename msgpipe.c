/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Read Message From Named Fifo(Pipe)!
 *
 *	This file defines the following puplic functions.
 *		MsgPipe( );
 ******************************************************************************/
#include	"msglocal.h"

static	int	blocking	= O_RDWR | O_CREAT | O_APPEND;
static	int	nonblocking	= O_RDWR | O_CREAT | O_APPEND | O_NDELAY ;
static	Msg_t	msg;
static	s32	PipeBytes;
static	u16	PipeBuffer;

void	MsgFreePipeBuffer( void )
{
	MsgFreeBuffer( my_buf, PipeBuffer );
}

/* FUNCTION:	MsgPipe
 *
 *	DESCRIPTION:
 *		This subroutine will read the next message from the process's
 *		FIFO and place it into the buffer.
 *
 *	INPUTS:
 *		timeout < 0 :	Waits Forever For Message
 *		timeout = 0 :	No Wait For Message (Just Polls)
 *		timeout > 0 :	Waits Specified Seconds For Message
 *
 *	RETURNS:
 *		Pointer To Message
 *			or
 *		Zero If Timeout Occurs
 *
 *	ON ERROR:
 *		This subroutine will log an error and then it will wait 30
 *		seconds after that it terminates the current process.
 */

Msg_t	*	MsgPipe( s32 timeout )
{
	if( !rcv_ok ) {
		FatalError( "MsgPipe: Input pipe not initialized!" );
	}
	for( ; ; ) {
		for( PipeBytes = 0; PipeBytes < sizeof( PipeBuffer ); ) {

			fcntl( 0, F_SETFL, blocking );
			switch( File_Test_For_Read_Data( 0, timeout ) ) {

			case -2:	/* TIMEOUT */
					Trace(	MSG_TRC_LVL,
						"MsgPipe(%d) Timeout",
						timeout
					);
					return( (Msg_t *)0 );

			default:	/* DATA */
					break;

			case -1:	/* ERROR */
				SystemError(
					"MsgPipe",
					"ERROR Reading From Pipe."
				);
			}

			fcntl( 0, F_SETFL, nonblocking );

			PipeBytes = File_Read( 0, &PipeBuffer, sizeof( PipeBuffer ) );
		}
		Trace( MSG_TRC_LVL, "MsgPipe(%d) Data Buffer = %u", timeout, PipeBuffer );
		switch( MsgGetBuffer( &msg, PipeBuffer ) ) {
		case	-1:
				Trace(	0,
					"MsgPipe(%d) Get Buffer %u Error",
					timeout,
					PipeBuffer
				);
		case	0:
				continue;
		case	1:
				Trace(	MSG_TRC_LVL,
					"MsgPipe(%d) Buffer(%u) Data = %d",
					timeout,
					PipeBuffer,
					msg.head.function
				);
				return( &msg );
		}
	}
}
