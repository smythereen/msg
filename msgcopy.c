/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Message Buffer Functions
 *
 *	This file defines the following functions.
 *		MsgCopy( );
 ******************************************************************************/
#include	"msglocal.h"

/* FUNCTION:	MsgCopy
 *
 *	DESCRIPTION:
 *		Will Copy A Message
 *
 *	INPUTS:
 *		tgt =	target message buffer pointer
 *		src =	source message buffer pointer
 *
 *	RETURNS:
 *		tgt =	target message buffer pointer
 *
 *	ON ERROR:
 *		NULL message buffer pointer
 */

Msg_t *	MsgCopy( Msg_t * tgt, Msg_t * src )
{
	s32	len;

	if( src == NULL && tgt == NULL ) return( NULL );

	len = 0;
	if( tgt == NULL ) {
		tgt = src;
	} else if( src == NULL ) {
		src = tgt;
	} else {
		len = src->head.length;
		if( len > sizeof( Msg_t ) ) len = sizeof( Msg_t );
		memcpy( tgt, src, len );
	}
	if( len < 1 ) len = src->head.length;
	if( len < 1 || len >= sizeof( Msg_t ) ) return( tgt );
	memset( &((char *)tgt)[ len ], 0, ( sizeof( Msg_t ) - len ) );
	return( tgt );
}
