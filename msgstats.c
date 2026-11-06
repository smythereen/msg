/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Message Buffer Statistics
 ******************************************************************************/
#include	<map.h>
#include	"msglocal.h"

s32	Msg_Statistics( u32 tgt )
{
	s32	Free;
	s32	Active;
	s32	Queued;
	s32	Loading;
	int	i;
	Msg_t *	x;


	x = MapFile( MsgFile( tgt, NULL ) );
	if( x == NULL ) {
		printf( "%20s - Unavailable\n", MsgA2N( tgt, NULL ) );
		return( -1 );
	}

	Free = 0;
	Active = 0;
	Queued = 0;
	Loading = 0;

	for( i = 0; i < MSG_MAX_BUF; i++, x++ ) {
		if( x->head.pid == 0 ) {
			Free += 1;
			continue;
		}
		Active += 1;
		if( x->head.sync == 0 ) {
			Loading += 1;
			continue;
		}
		Queued += 1;
	}
	printf( "%20s - Free:%5u, Used:%5u = Queued:%u / Loading:%u\n",
		MsgA2N( tgt, NULL ),
		Free,
		Active,
		Queued,
		Loading
	);
	return( Active );
}
