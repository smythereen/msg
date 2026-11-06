/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Message Header Format Data
 ******************************************************************************/
#include	"msglocal.h"
#include	<mem_io.h>

#define	STRUCT	MsgHead_t
	Mitem		MsgHead_l[ ] =	{
	X32(		sync,		"Synchronizing Bytes"		),
	MsgAddr(	target,		"Message Target Address"	),
	MsgAddr(	source,		"Message Source Address"	),
	Stamp32(	sent,		"Sent Date & Time"		),
	Stamp32(	expire,		"Expire Date & Time"		),
	U32(		pid,		"Sender's Process ID"		),
	S32(		function,	"Message Type"			),
	U32(		length,		"Message Length"		),
	{ END_OF_LIST }
};
#undef	STRUCT

