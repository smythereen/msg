/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Initialize Process's Fifo Message Functions
 *
 *	This file defines the following functions.
 *		MsgInitFake( );
 ******************************************************************************/
#include	"msglocal.h"

void	MsgInitFake( u32 adr )
{
	pid_t	pid;
	char *	cp;

	if( my_pid == ( pid = getpid( ) ) ) return;	/* Already Initialized! */
	if( ( cp = getenv( "TraceLevel" ) ) != (char *)0 ) {
		TraceLevel = strtoul( cp, 0, 16 );
	}
/*
 *	Set Message Address Information
 */
	my_adr = adr;
	my_mch = MsgMachine(	adr );
	my_gid = MsgGroup(	adr );
	my_typ = MsgProgram(	adr );
	my_sub = MsgSubTask(	adr );
	my_pid = getpid( );
	MsgA2N( my_adr, my_name );
/*
 *	Initialize Message Send Tables
 */
	rcv_ok = 0;
	snd_ok = 1;
	return;
}
