/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Declare Message System Global Data Area
 *
 *	This file defines the following functions.
 *		N/A
 ******************************************************************************/
#include	"msglocal.h"

	u32	my_pid =	 0;
	u32	my_adr =	-1;
	u32	my_buf =	-1;

	int	my_mch =	-1;
	int	my_gid =	-1;
	int	my_typ =	-1;
	int	my_sub =	-1;
	int	my_dir_fd =	-1;
	int	MsgAudit =	 1;
	int	MsgProxy =	 0;	// Don't Change Target Address In Sent Message
	int	MsgRelay =	 0;	// Don't Change Source Address In Sent Message

	u16	rcv_ok = 	 0;
	u16	snd_ok = 	 0;

	char	my_pipe[ MAX_PATH ];
	char	my_directory[ MAX_PATH ];

	void	(*my_quit)( Msg_p );
	void	(*my_print)( Msg_p );
