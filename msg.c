/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Process To Send Internal Messages To Another Process
 *
 *	This file defines the following private functions.
 *		main( );
 ******************************************************************************/
#include	<miomsg.h>
#include	<mem_io.h>
#include	"msglocal.h"


typedef	struct	list_s	{
	char *	cmd;
	int	(*func)( int ac, char * av[ ], ... );
}	list_t;

static	void	usage( char * av[ ] )
{
	fprintf( stderr,
		 "\n\tMessage System (MSG) - Verification Program\n\n"
		 "Usage:\n"
		 "\t%s\t a2n       hex_address\n"
		 "\t  \t file      machine.group.program.task\n"
		 "\t  \t g2i       [ group_name ]\n"
		 "\t  \t i2g       group_number\n"
		 "\t  \t i2m       machine_number\n"
		 "\t  \t i2p       program_number\n"
		 "\t  \t memory    machine.group.program.task [ C.Variable.Name [ Value ] ]\n"
		 "\t  \t m2i       [ machine_name ]\n"
		 "\t  \t n2a       machine.group.program.task\n"
		 "\t  \t p2i       [ program_name ]\n"
		 "\t  \t path      machine.group.program.task\n"
		 "\t  \t send      target.address.or.name function [data ...]\n"
		 "\t  \t stats     machine.group.program.task\n"
		 "\n",
		 av[ 0 ]
	);
	fflush( stderr );
	exit( 1 );
}

typedef	struct	buf_s	{
	Msg_t	buf[ MSG_MAX_BUF ];
}	buf_t;

extern  Mitem           msg_l[ ];
extern  Mitem           Msg_l[ ];

#define	STRUCT	buf_t
static	Mitem		buf_l[ ] =	{
	Struct( Msg_t, buf, msg_l ),
	{ END_OF_LIST }
};
#undef	STRUCT

/*
 *	Memory Command
 */

int	memory( int ac, char * av[ ], ... )
{
	u32	adr;
	buf_t *	x;
	char	prefix[ 128 ];

	switch( ac ) {
	case 3:
	case 2:
	case 1:		adr = MsgN2A( av[0] );
			break;
	default:	fprintf( stderr,
				 "\nUsage:\n"
					"\tmsg memory machine.group.program.task [ C.Variable.Name [ Value ] ]\n",
				 av[ 0 ]
			);
			fflush( stderr );
			exit( 1 );
	}
	LocalName = "msg memory";
	TraceLevel = 0x00000000;

	x = (buf_t *)Msg_Map( adr );
	if( x == NULL ) {
		usage( &av[ -2 ] );
		exit( 1 );
	}

	sprintf( prefix, "msg memory %s", av[ 0 ] );

	switch( ac ) {
	default:   Input_Memory_Item(  x, buf_l, av[ 1 ], av[ 2 ] );
		   UpdateMap( x, sizeof( buf_t ) );
	case  2:   Output_Memory_Item( x, buf_l, av[ 1 ], prefix  );
		   break;
	case  1:   Output_Memory_Item( x, buf_l,       0, prefix  );
		   break;
	}
	UnmapAddress( x, 1 );
	exit( 0 );
}

/*
 *	Send Command Functions
 */

static	void	sendusage( )
{
	fprintf(
	 stderr,
	 "\nSend Message Function Options:\n"
	 " msg send adr expire trace hex_level [new_trace_file]\n"
	 "  \"   \"    \"    \"    quit\n"
	 "  \"   \"    \"    \"    stub  print_on [pass_msg [print_file]]\n"
	 "  \"   \"    \"    \"    peer  number_of_peers\n"
	 "  \"   \"    \"    \"    func#\n"
	 "\n"
	);
	fflush( stderr );
	exit( 1 );
}

static	int	trace( int ac, char * av[ ], ... )
{
	va_list		ap;
	MsgTrace_t	*	m;
        
	va_start( ap, av );
	m = va_arg( ap, MsgTrace_t * );
	va_end( ap );

	switch( ac ) {
	default:	sendusage( );
	case	2:	strncpy( m->m.file, av[ 1 ], MAX_PATH );
	case	1:	m->m.newlevel = strtoul( av[ 0 ], 0, 0 );
			break;
	}
	return(	_Trace );
}

static	int	peer( int ac, char * av[ ], ... )
{
	va_list		ap;
	MsgPeer_t	*	m;
        
	va_start( ap, av );
	m = va_arg( ap, MsgPeer_t * );
	va_end( ap );

	switch( ac ) {
	default:	sendusage( );
	case	1:	m->m.peers = strtoul( av[ 0 ], 0, 0 );
			break;
	}
	return(	_Peer );
}

static	int	quit( int ac, char * av[ ], ... )
{
	switch( ac ) {
	default:	sendusage( );
	case	0:	break;
	}
	return(	_Quit );
}

static	int	stub( int ac, char * av[ ], ... )
{
	va_list			ap;
	MsgStub_t	*	m;
        
	va_start( ap, av );
	m = va_arg( ap, MsgStub_t * );
	va_end( ap );

	switch( ac ) {
	default:	sendusage( );
	case	3:	strncpy( m->m.file, av[ 2 ], MAX_PATH );
	case	2:	m->m.print = *av[ 1 ];
	case	1:	m->m.pass  = *av[ 0 ];
			break;
	}
	return(	_Stub );
}

static	list_t	sendlist[ ]	=	{
	{	"trace",		trace		},
	{	"peer",			peer		},
	{	"quit",			quit		},
	{	"stub",			stub		},
	{	(char *)0,		0		}
};

static	int	send( int ac, char * av[ ], ... )
{
	MsgIn_t		m;
	list_t	*	l;
	int		len;
	s32		fun;
	u32		adr;
	u32		exp;

	memset( &m, 0, sizeof( m ) );
	switch( ac ) {
	default:
		adr = MsgN2A( av[ 0 ] );
		exp = strtol( av[ 1 ], 0, 0 );
		len = strlen( av[ 2 ] );
		fun = 0;
		for( l = sendlist; l->cmd != (char *)0; l++ ) {
			if( strncmp( av[ 2 ], l->cmd, len ) ) continue;
			fun = l->func( ( ac - 3 ), &av[ 3 ], &m );
			break;
		}
		if( fun < 0 ) break;
		if( l->cmd == (char *)0 ) {
			fun = strtol( av[ 2 ], 0, 0 );
			break;
		}
	case  2:
	case  1:
	case  0:
		sendusage( );
	}
//	TraceLevel = MSG_TRC_LVL;
	MsgSend( adr, fun, &m, sizeof( MsgIn_t ), exp );
	return( 0 );
}

static	int	a2n( int ac, char * av[ ], ... )
{
	u32	adr;

	switch( ac ) {
	default:	usage( &av[-2] );
	case	1:	break;
	}
	adr = strtoul( av[0], 0, 16 );
	printf( "\t0x%08X = %s\n", adr, MsgA2N( adr, 0 ) );
	return( 0 );
}

static	int	file( int ac, char * av[ ], ... )
{
	u32	adr;

	switch( ac ) {
	default:	usage( &av[-2] );
	case	1:	break;
	}
	adr = MsgN2A( av[0] );
	printf( "\n\tMessage Buffer File = %s\n\n", MsgFile( adr, 0 ) );
	return( 0 );
}

static	int	g2i( int ac, char * av[ ], ... )
{
	printf( "Group Integer=%d\n", MsgGroupInteger( av[0] ) );
	return( 0 );
}

static	int	i2g( int ac, char * av[ ], ... )
{
	printf( "Group Name=%s\n", MsgGroupName( strtoul( av[0], 0, 10 ) ) );
	return( 0 );
}

static	int	i2m( int ac, char * av[ ], ... )
{
	printf( "Machine Name=%s\n", MsgMachineName( strtoul( av[0], 0, 10 ) ) );
	return( 0 );
}

static	int	i2p( int ac, char * av[ ], ... )
{
	printf( "Program Name=%s\n", MsgProgramName( strtoul( av[0], 0, 10 ) ) );
	return( 0 );
}

static	int	m2i( int ac, char * av[ ], ... )
{
	printf( "Machine Integer=%d\n", MsgMachineInteger( av[0] ) );
	return( 0 );
}

static	int	n2a( int ac, char * av[ ], ... )
{
	u32	adr;

	switch( ac ) {
	default:	usage( &av[-2] );
	case	1:	break;
	}
	adr = MsgN2A( av[0] );
	printf( "\t0x%08X = %s\n", adr, MsgA2N( adr, 0 ) );
	return( 0 );
}

static	int	p2i( int ac, char * av[ ], ... )
{
	printf( "Program Integer=%d\n", MsgProgramInteger( av[0] ) );
	return( 0 );
}

static	int	path( int ac, char * av[ ], ... )
{
	u32	adr;

	switch( ac ) {
	default:	usage( &av[-2] );
	case	1:	break;
	}
	adr = MsgN2A( av[0] );
	printf( "\n\tMessage Pipe = %s\n\n", MsgPath( adr, 0 ) );
	return( 0 );
}

static	int	stats( int ac, char * av[ ], ... )
{
	u32	adr;

	switch( ac ) {
	default:	usage( &av[-2] );
	case	1:	break;
	}
	adr = MsgN2A( av[0] );
	Msg_Statistics( adr );
	return( 0 );
}

static	int	zero( int ac, char * av[ ], ... )
{
extern	int	MsgZeroAllBuffers( u32 adr );
	u32	adr;

	switch( ac ) {
	default:	usage( &av[-2] );
	case	1:	break;
	}
	adr = MsgN2A( av[0] );
	MsgZeroAllBuffers( adr );
	Msg_Statistics( adr );
	return( 0 );
}

static	list_t	cmdlist[ ]	=	{
	{	"a2n",			a2n		},
	{	"file",			file		},
	{	"memory",		memory		},
	{	"g2i",			g2i		},
	{	"i2g",			i2g		},
	{	"i2m",			i2m		},
	{	"i2p",			i2p		},
	{	"m2i",			m2i		},
	{	"n2a",			n2a		},
	{	"p2i",			p2i		},
	{	"path",			path		},
	{	"send",			send		},
	{	"stats",		stats		},
	{	"zero",			zero		},
	{	(char *)0,		0		}
};

int	main( int ac, char * av[ ] )
{
struct	list_s	*	l;
	int		len;

	if( MioMsgInit( ) != 0 ) { perror( "MioMsgInit" ); return( 1 ); }

	LocalName = av[ 0 ];
//	MsgInitFake( MsgN2A( "msg.all" ) );
	switch( ac ) {
	default:	len = strlen( av[ 1 ] );
			for( l = cmdlist; l->cmd != (char *)0; l++ ) {
				if( strncmp( av[ 1 ], l->cmd, len ) ) continue;
				l->func( ( ac - 2 ), &av[ 2 ] );
				exit( 0 );
			}
	case  1:
	case  0:	usage( av );
	}
	exit( 0 );
}
