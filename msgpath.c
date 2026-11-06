/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Generates A FIFO's Path And File Name From An Address
 *
 *	This file defines the following functions.
 *		MsgPath( );
 ******************************************************************************/
#include	"msglocal.h"

static	char	*Path( u32 adr, char *buf, char * fmt, u32 tim )
{
static	char	buffer[ MAX_PATH ];
/*
 *	If Routine Is Not Passed A Pointer To A Buffer
 *		Then It Supplies A Static Buffer
 */
	if( buf == (char *)0 ) buf = buffer;
/*
 *	Compose The Path Plus The Name
 */
	return( File_Path( buf, MAX_PATH, "PIPE_PATH", fmt, adr, tim ) );
}

/* FUNCTIONS:	MsgPath MsgFile
 *
 *	DESCRIPTION:
 *		This subroutine will return a pointer to a character array
 *		which will contain a full path to a named pipe.  The name
 *		is calculated from the address passed to this subroutine.
 *
 *	INPUTS:
 *		adr =	Named FIFO/Pipe Address
 *		buf =	Buffer to place the path into
 *
 *	RETURNS:
 *		Pointer to path buffer.
 *
 *	ON ERROR:
 *		returns a NULL pointer "(char *)0"
 */

char *	MsgPath( u32 adr, char *buf )
{
	return( Path( adr, buf, "%08.8X.que", 0 ) );
}

char *	MsgFile( u32 adr, char *buf )
{
	return( Path( adr, buf, "%08.8X.mem", 0 ) );
}
