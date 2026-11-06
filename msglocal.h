/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2009-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Local Message Support Header File
 *
 *		This file defines private structures, variables, and routines
 *		need for messaging.
 ******************************************************************************/
#ifndef	MSGLOCAL_H
#define	MSGLOCAL_H

#if	defined( __cplusplus )
extern	"C"	{
#endif

#include	<fcntl.h>
#include	<sched.h>
#include	<sys/fcntl.h>
#include	<sys/stat.h>
#include	<limits.h>

#include	<far.h>
#include	<map.h>
#include	<addr.h>
#include	<msg.h>

#ifndef		linux
//	#define		Big_Endian	1
#include	<sys/inode.h>
#endif

#ifndef	MSG_MAX_BUF
#define	MSG_MAX_BUF	2100
#endif

#ifndef	PIPSIZ
#define	PIPSIZ	5120
#endif

typedef	enum	{
	_Timeout	= MSG_TIMEOUT,	/* Pseudo-Command MsgRead Timed Out */
	_Trace		= -2,		/* Set Program Trace Level Command */
	_Quit		= -3,		/* Program Exit Command */
	_Stub		= -4,		/* Print/NoPrint & Pass/NoPass Msg */
	_Peer		= -5,		/* Spawn Peer Processes */
	_Internal	= 0x80000000	/* Internal Functions Indicator */
}	MsgFunc_e;

/*
 *	Internal Message Functions
 */

extern	void		MsgDelete( u32 time_stamp );
extern	Msg_t	*	MsgDumper( Msg_t * msg );
extern	int		MsgFreeBuffer( u32 tgt_adr, int i );
extern	void		MsgFreePipeBuffer( void );
extern	int		MsgGetBuffer( Msg_t * m, int i );
extern	Msg_t	*	MsgInternal( Msg_t * msg );
extern	Msg_t	*	Msg_Map( u32 Address );
extern	Msg_t	*	MsgPipe( s32 timeout );
extern	int		MsgSetBuffer( u32 tgt_adr, Msg_t * m );

/*
 *	Internal Message Variables
 */

extern	u16	rcv_ok;
extern	u16	snd_ok;
extern	int	MsgAudit;	/* Message System Audit Input Buffer Flag */
extern	int	MsgProxy;	/* Message System In Proxy Delivery Mode Flag */
extern	int	MsgRelay;	/* Message System In Relay Mode Flag */
extern	int	MsgBypass;	/* Message System In Bypass Mode Flag */
extern	int	MsgDumpIt;	/* Print Message To stderr Flag */
extern	int	my_dir_fd;	/* Message System's Message Directory FD */
extern	int	msg_prg_num;	/* Last Decoded Prg Number */
extern	char	my_directory[ ];
extern	char	my_pipe[ ];
extern	char	msg_prg_name[ ];/* Last Decoded Prg Name */
extern	fard_t	File_Table[ ];

/*
 *	Internal Message Structures
 */

struct	_Trace	{
	u32	newlevel;		/* New Trace Level Msg Body */
	char	file[ MAX_PATH ];	/* New File Or Device For Output */
};

typedef	MsgProto( _Trace )	MsgTrace_t;

struct	_Quit	{
	u32	exit;			/* Exit Value */
};

typedef	MsgProto( _Quit )	MsgQuit_t;

struct	_Stub	{
	uchar	pass;			/* Pass Msg To Program Boolean */
					/*   "0"=NoPass, "1"=Pass */
	uchar	print;			/* Print Msg Boolean */
					/*   "0"=NoPrint, "1"=Print */
	char	file[ MAX_PATH ];	/* New File Or Device For Msg Output */
};

typedef	MsgProto( _Stub )	MsgStub_t;

struct	_Peer	{
	uchar	peers;			/* Number Of Peer Processes To Create */
};

typedef	MsgProto( _Peer )	MsgPeer_t;

typedef	struct	MsgIn_s	{
	MsgHead_t	h;
	union	{
		struct	_Trace	t;
		struct	_Quit	q;
		struct	_Stub	s;
		struct	_Peer	p;
	}	b;
}	MsgIn_t;

#if	defined( __cplusplus )
}
#endif

#endif
