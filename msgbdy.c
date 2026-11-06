/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Message Body Format Data
 ******************************************************************************/
#include	"msglocal.h"
#include	<mem_io.h>

#define	STRUCT	MsgBody_t
	Mitem	MsgBody_l[ ] =	{
	X8(	data,	"Message Payload"	),
	{ END_OF_LIST }
};
#undef	STRUCT

