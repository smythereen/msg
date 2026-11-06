/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2010-2026 John R. Smythe Jr. */
/*******************************************************************************
 *
 *	Message Support Header File
 *
 *		This file defines common structures, variables, and routines
 *		need for messaging.
 ******************************************************************************/
#ifndef	MSG_H
#define	MSG_H

#if	defined( __cplusplus )
extern	"C"	{
#endif

#include	<common.h>
#include	<addr.h>		/* Addresses: names, numbers & the old Msg* names */

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define	SYNC_PATTERN	0xA4B3C2D1
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
#define	SYNC_PATTERN	0xA1B2C3D4
#endif

/*
 *	Externals
 */

#define	MAX_MSG_LEN		1024	/* Maximum Message Length In Bytes */
#define	MSG_TRC_LVL		0x10000000
#define	MSG_TIMEOUT		-1

/*
 *	General Message Format Structures
 */

typedef	struct	MsgHead_s	{
	u32	sync;		/* Message Identifier Tag */
	u32	target;		/* Receiver's Address */
	u32	source;		/* Sender's Address */
	u32	sent;		/* Message's Expiration Date & Time */
	u32	expire;		/* Buffered Message's Expiration Date & Time */
	u32	pid;		/* Sender's Process ID */
	s32	function;	/* Message Type/Function */
	u32	length;		/* Message Length */
}	MsgHead_t;

typedef	struct	MsgBody_s	{
	char	data[ MAX_MSG_LEN - sizeof( MsgHead_t ) ];
}	MsgBody_t;

typedef	struct	Msg_s	{
	MsgHead_t	head;
	MsgBody_t	body;
}	Msg_t;

typedef	union	__attribute__((transparent_union))	{
	Msg_t		*	msg;
	MsgHead_t	*	head;
}	Msg_p;

/*
 *	Externals Global Variables
 */

extern	u32	my_pid;		/* This program's Process   ID Number */
extern	u32	my_adr;		/* This program's Address   ID Number */
extern	u32	my_buf;		/* This program's Address   ID Number */

extern	int	my_mch;		/* This program's Machine   ID Number */
extern	int	my_gid;		/* This program's Msg Group ID Number */
extern	int	my_typ;		/* This program's Type      ID Number */
extern	int	my_sub;		/* This program's SubProgram   Number */

extern	void	(*my_print)( Msg_p );	/* This program's Message Print Routine */
extern	void	(*my_quit )( Msg_p );	/* This program's Message Quit  Routine */

extern	char *	ProgramTypes[];	/* Message System Program Type Table */

/*
 *	General Message Prototype Macro
 */

#define	MsgProto( name )		\
	struct	{			\
		MsgHead_t	h;	\
		struct	name	m;	\
	}

#define	BodyFieldValue( typename, field ) (((typename *)&msg)->m.field )
#define	HeadFieldValue( typename, field ) (((typename *)&msg)->h.field )

/*
 *	General Message Subroutine Declarations
 */

extern	Msg_t	*	MsgCopy( 	Msg_t *	target,
					Msg_t *	source
			);

#ifdef	JUNK
extern	char	*	MsgDir( 	u32	address,
					char *	buffer
			);

extern	char	*	MsgDirFile(	u32	address,
					char *	buffer,
					u32	time
			);
#endif

extern	char	*	MsgFile(	u32	address,
					char *	buffer
			);

extern	void		MsgInit(	u32	address,
					void	(*quit)( Msg_p ),
					void	(*MsgPrint)( Msg_p )
			);

extern	void		MsgInitFake(	u32	address
			);

extern	void		MsgSend(	u32	address,
					s32	function,
					void *	msg,
					u32	size,
					u32	expire
			);

extern	s32		Msg_Statistics(	u32	address
			);

extern	s32		MsgRead(	Msg_t *	msg,
					s32	timeout_in_seconds
			);

extern	char	*	MsgPath(	u32	address,
					char *	buffer
			);

extern	int		CreateKillScript(	char *	name
			);

extern	int		AddToKillScript(	u16	pid,
						char *	name
			);

#if	defined( __cplusplus )
}
#endif

#endif
