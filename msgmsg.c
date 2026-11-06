/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Message Format Data
 ******************************************************************************/
#include	"msglocal.h"
#include	<mem_io.h>

extern	Mitem		MsgHead_l[ ];
extern	Mitem		MsgBody_l[ ];

#define	STRUCT	Msg_t
	Mitem		Msg_l[ ] =	{
	Struct(		MsgHead_t,	head, MsgHead_l			),
	Struct(		MsgBody_t,	body, MsgBody_l			),
	{ END_OF_LIST }
};
#undef	STRUCT

#define	STRUCT	Msg_t
	Mitem		msg_l[ ] =	{
	Struct(		MsgHead_t,	head, MsgHead_l			),
	{ END_OF_LIST }
};
#undef	STRUCT

