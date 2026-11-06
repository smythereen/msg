/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Message Buffer Functions
 *
 *	This file defines the following functions.
 *		MsgFreeBuffer( );
 *		MsgGetBuffer( );
 *		MsgSetBuffer( );
 ******************************************************************************/
#include	<sched.h>

#include	<map.h>
#include	<cmpxchg.h>
#include	"msglocal.h"

static	Msg_t *	MsgBufferAddr;

Msg_t *	Msg_Map( u32 tgt_adr )
{
	Trace(	MSG_TRC_LVL, "Msg_Map( %08X )", tgt_adr );
	if( MsgBufferAddr != NULL )  {
		UnmapAddress( MsgBufferAddr, 0 );
		MsgBufferAddr = NULL;
	}
	return( ( MsgBufferAddr = MapFile2Mem(
			MsgFile( tgt_adr, NULL ),
			( sizeof( Msg_t ) * MSG_MAX_BUF ),
			PROT_WRITE
		)
	) );
}

/*
 *	Reclaim slots still claimed more than 2 seconds before "now".
 *	Called only when the buffer is full; slots normally free themselves.
 */
static	int	MsgAuditBuffer( Msg_t * base, time_t now )
{
	int	i;
	int	n;
	Msg_t *	x;

	for( n = 0, i = 0, x = base; i < MSG_MAX_BUF; i++, x++ ) {
		if( x->head.pid == 0 ) continue;
		if( ( x->head.sent + 2 ) >= now ) continue;
		x->head.sync = 0;
		x->head.pid = 0;	// Deallocate
		n += 1;
	}
	MsgAudit = 0;
	return( n );
}

/* FUNCTION:	MsgSetBuffer
 *
 *	DESCRIPTION:
 *		Will Allocate And Load A Message Buffer.
 *
 *	INPUTS:
 *		m =	message buffer pointer
 *
 *	RETURNS:
 *		Buffer Number ( >= 0 )
 *
 *	ON ERROR:
 *		Returns -1
 */

int	MsgSetBuffer( u32 tgt, Msg_t * m )
{
	int	i;
	int	pass;
	time_t	t;
	Msg_t *	x;
	Msg_t *	base;

	if( ( x = Msg_Map( tgt ) ) == NULL ) {
		Trace(	0, "MsgSetBuffer( %s ) = -1 [Map Failure]", MsgA2N( tgt, NULL ) );
		return( -1 );
	}

	/*
	 *	Allocate Buffer ( Watch Out For Simultaneous Access Via SMP )
	 *	If full, audit (reclaim stale slots) once and retry.
	 */
	base = x;
	for( pass = 0; pass < 2; pass++ ) {
	    for( i = 0, x = base; i < MSG_MAX_BUF; i++, x++ ) {

#ifndef	CompareAndExchange

		if( x->head.pid != 0 ) continue;
		x->head.pid = my_pid;
		sched_yield( );
		if( x->head.pid == my_pid ) break;

#else

		if( CompareAndExchange( (s32 *)&x->head.pid, 0, my_pid ) == 0 ) break; 

#endif

	    }
	    if( i < MSG_MAX_BUF ) break;
	    if( pass == 0 ) {
		time( &t );
		Trace(	0, "MsgSetBuffer( %s ) Full, Audit Reclaimed %d",
			MsgA2N( tgt, NULL ), MsgAuditBuffer( base, t ) );
	    }
	}
	if( i >= MSG_MAX_BUF ) {
		Trace(	0, "MsgSetBuffer( %s ) = -1 [No Empty Buffer]", MsgA2N( tgt, NULL ) );
		return( -1 );
	}

	m->head.pid = my_pid;
	m->head.sync = 0;
	if( m->head.length > sizeof( Msg_t ) ) m->head.length = sizeof( Msg_t );
	if( !MsgRelay ) {
		m->head.source = my_adr;
	}
	time( &t );
	m->head.sent = t;
	m->head.expire = 0;
	MsgCopy( x, m );
	x->head.sync = SYNC_PATTERN;	// Mark As Unprocessed
	Trace(	MSG_TRC_LVL, "MsgSetBuffer( %s ) = %u", MsgA2N( tgt, NULL ), i );
	return( i );
}

/* FUNCTION:	MsgGetBuffer
 *
 *	DESCRIPTION:
 *		Will Unload And Deallocate A Message Buffer.
 *
 *	INPUTS:
 *		m =	message buffer pointer
 *		i =	message buffer number
 *
 *	RETURNS:
 *		1	If Buffer Gotten
 *		0	If Buffer Empty
 *
 *	ON ERROR:
 *		-1	Cannot Get Buffer
 */

int	MsgGetBuffer( Msg_t * m, int i )
{
	Msg_t *	x;

	if( i < 0 || i >= MSG_MAX_BUF ) {
		Trace(	0, "MsgGetBuffer( %d ) = -1 [Bad Buffer Number]", i );
		return( -1 );
	}
	if( ( x = Msg_Map( my_buf ) ) == NULL ) {
		Trace(	0, "MsgGetBuffer( %d ) = -1 [NULL Map]", i );
		return( -1 );
	}
	x = &x[ i ];
	if( x->head.pid  == 0 ) {
		Trace(	0, "MsgGetBuffer( %d ) = 0 [Zero PID]", i );
		return( 0 );
	}

#ifndef	CompareAndExchange

	if( x->head.sync == 0 ) {
		if( x->head.pid  == 0 ) {
			Trace(	0, "MsgGetBuffer( %d ) = 0 [Zero PID]", i );
			return( 0 );
		}
		sched_yield( );
		if( x->head.sync == 0 ) {
			Trace(	0, "MsgGetBuffer( %d ) = 0 [Zero Sync]", i );
			sched_yield( );
			x->head.sync = 0;	// Deallocate
			x->head.pid  = 0;	// Deallocate
			return( 0 );
		}
	}
	x->head.sync = 0;	// Mark As Processed

#else

	if( CompareAndExchange( (s32 *)&x->head.sync, 0, 0 ) == 0 ) {
		if( x->head.pid  == 0 ) {
			Trace(	0, "MsgGetBuffer( %d ) = 0 [Zero PID]", i );
			return( 0 );
		}
		sched_yield( );
		if( CompareAndExchange( (s32 *)&x->head.sync, 0, 0 ) == 0 ) {
			Trace(	0, "MsgGetBuffer( %d ) = 0 [Zero Sync]", i );
			sched_yield( );
			x->head.sync = 0;	// Deallocate
			x->head.pid  = 0;	// Deallocate
			return( 0 );
		}
	}

#endif

	MsgCopy( m, x );
	x->head.pid = 0;	// Deallocate
	Trace(	MSG_TRC_LVL, "MsgGetBuffer( %d ) = 1", i );
	return( 1 );
}

/* FUNCTION:	MsgFreeBuffer
 *
 *	DESCRIPTION:
 *		Will Deallocate A Message Buffer.
 *
 *	INPUTS:
 *		i =	message buffer number
 *
 *	RETURNS:
 *		Buffer Number ( >= 0 )
 *
 *	ON ERROR:
 *		Returns -1
 */

int	MsgFreeBuffer( u32 tgt_adr, int i )
{
	Msg_t *	x;

	if( ( x = Msg_Map( tgt_adr ) ) == NULL ) return( -1 );
	x = &x[ i ];
	if( x->head.pid  == 0 ) return( 0 );
	if( x->head.sync != 0 ) x->head.sync = 0;
	x->head.pid = 0;	// Deallocate
	return( i );
}

/* FUNCTION:	MsgZeroAllBuffers
 *
 *	DESCRIPTION:
 *		Will Zero All Message Buffers.
 *
 *	INPUTS:
 *		i =	message buffer number
 *
 *	RETURNS:
 *		1 = Success
 *
 *	ON ERROR:
 *		Returns -1
 */

int	MsgZeroAllBuffers( u32 tgt_adr )
{
	if( Msg_Map( tgt_adr ) == NULL ) return( -1 );
	memset( MsgBufferAddr, 0, ( sizeof( Msg_t ) * MSG_MAX_BUF ) );
	MsgAudit = 0;
	return( 1 );
}
