/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// console.c

#include "client.h"

#define  DEFAULT_CONSOLE_WIDTH 78
#define  MAX_CONSOLE_WIDTH 120

#define NUM_CON_TIMES 8 // max notify lines (ring buffer size); con_notifyLines selects how many are shown

#define  CON_TEXTSIZE   65536

int bigchar_width;
int bigchar_height;
int smallchar_width;
int smallchar_height;

typedef struct {
	qboolean	initialized;

	short	text[CON_TEXTSIZE];
	int		current;		// line where next message will be printed
	int		x;				// offset in current line for next print
	int		display;		// bottom of console displays this line

	int 	linewidth;		// characters across screen
	int		totallines;		// total lines in console scrollback

	float	xadjust;		// for wide aspect screens

	float	displayFrac;	// aproaches finalFrac at scr_conspeed
	float	finalFrac;		// 0.0 to 1.0 lines of console to display

	int		vislines;		// in scanlines

	int		times[NUM_CON_TIMES];	// cls.realtime time the line was generated
								// for transparent notify lines
	vec4_t	color;

	int		viswidth;
	int		vispage;		

	qboolean newline;

} console_t;

extern  qboolean    chat_team;
extern qboolean chat_cmdMode;
extern  int         chat_playerNum;

// Tabbed console (UrT). CON_ALL receives every message and is identical to the
// classic single console; the other tabs receive a routed subset (iteration 2).
typedef enum {
	CON_ALL = 0, // every message — the default tab
	CON_GENERAL, // system / non-chat / non-frag messages
	CON_FRAG,    // kill (frag) messages
	CON_CHAT,    // chat / say messages
	NUM_CON
} conType_t;

static const char *const con_names[NUM_CON] = { "All", "General", "Frag", "Chat" };

console_t consoles[NUM_CON];
console_t *con = &consoles[CON_ALL]; // the active (visible) console
int con_iActive = CON_ALL;           // index of the active tab

cvar_t		*con_conspeed;
cvar_t		*con_autoclear;
cvar_t		*con_notifytime;
cvar_t		*con_scale;
cvar_t *con_tabs; // draw the tab bar / enable tabbed console
cvar_t *con_tabScale; // tab-title font size, multiple of the body font
cvar_t *con_height;   // fraction of the screen the open console covers
cvar_t *con_opacity;  // alpha of the console background
cvar_t *con_notifyLines; // number of notify lines drawn (<= NUM_CON_TIMES)
cvar_t *con_notifyY;     // initial vertical offset of the notify area, in pixels

int			g_console_field_width;

/*
================
Con_ToggleConsole_f
================
*/
void Con_ToggleConsole_f( void ) {
	// Can't toggle the console when it's the only thing available
    if ( cls.state == CA_DISCONNECTED && Key_GetCatcher() == KEYCATCH_CONSOLE ) {
		return;
	}

	if ( con_autoclear->integer ) {
		Field_Clear( &g_consoleField );
	}

	g_consoleField.widthInChars = g_console_field_width;

	Con_ClearNotify();
	Key_SetCatcher( Key_GetCatcher() ^ KEYCATCH_CONSOLE );
}


/*
================
Con_MessageMode_f
================
*/
static void Con_MessageMode_f( void ) {
	chat_playerNum = -1;
	chat_team = qfalse;
	Field_Clear( &chatField );
	chatField.widthInChars = 30;

	Key_SetCatcher( Key_GetCatcher() ^ KEYCATCH_MESSAGE );
}


/*
================
Con_MessageMode2_f
================
*/
static void Con_MessageMode2_f( void ) {
	chat_playerNum = -1;
	chat_team = qtrue;
	Field_Clear( &chatField );
	chatField.widthInChars = 25;
	Key_SetCatcher( Key_GetCatcher() ^ KEYCATCH_MESSAGE );
}


/*
================
Con_MessageMode3_f
================
*/
static void Con_MessageMode3_f( void ) {
	chat_playerNum = cgvm ? VM_Call( cgvm, 0, CG_CROSSHAIR_PLAYER ) : -1;
	if ( chat_playerNum < 0 || chat_playerNum >= MAX_CLIENTS ) {
		chat_playerNum = -1;
		return;
	}
	chat_team = qfalse;
	Field_Clear( &chatField );
	chatField.widthInChars = 30;
	Key_SetCatcher( Key_GetCatcher() ^ KEYCATCH_MESSAGE );
}


/*
================
Con_MessageMode4_f
================
*/
static void Con_MessageMode4_f( void ) {
	chat_playerNum = cgvm ? VM_Call( cgvm, 0, CG_LAST_ATTACKER ) : -1;
	if ( chat_playerNum < 0 || chat_playerNum >= MAX_CLIENTS ) {
		chat_playerNum = -1;
		return;
	}
	chat_team = qfalse;
	Field_Clear( &chatField );
	chatField.widthInChars = 30;
	Key_SetCatcher( Key_GetCatcher() ^ KEYCATCH_MESSAGE );
}

/*
================
Con_MessageMode5_f

Private message to ourselves ("tell me"): the text is sent to the server as a
tell to our own client, so it stays invisible to other players while still
being parsed server-side (e.g. mod/bot "!" commands).
================
*/
static void Con_MessageMode5_f( void ) {
	chat_playerNum = clc.clientNum;
	if ( chat_playerNum < 0 || chat_playerNum >= MAX_CLIENTS ) {
		chat_playerNum = -1;
		return;
	}
	chat_team = qfalse;
	chat_cmdMode = qtrue;
	Field_Clear( &chatField );
	chatField.widthInChars = 30;
	Key_SetCatcher( Key_GetCatcher() ^ KEYCATCH_MESSAGE );
}

/*
================
Con_Tellme_f

"tellme <text>": send <text> as a private tell to ourselves (see messagemode5),
for use in binds/scripts. The text is also recorded in the input history.
================
*/
static void Con_Tellme_f( void ) {
	char buffer[MAX_STRING_CHARS];

	if ( Cmd_Argc() < 2 ) {
		Com_Printf( "usage: tellme <text>\n" );
		return;
	}
	if ( clc.clientNum < 0 || clc.clientNum >= MAX_CLIENTS ) {
		return;
	}
	Com_sprintf( buffer, sizeof( buffer ), "tell %i \"%s\"\n", clc.clientNum, Cmd_ArgsFrom( 1 ) );
	CL_AddReliableCommand( buffer, qfalse );
}

/*
================
Con_SetActiveTab / Con_NextTab_f / Con_PrevTab_f

Tabbed console: change the visible tab. `con` always points at the active one.
================
*/
static void Con_SetActiveTab( int idx ) {
	con_iActive = ( idx + NUM_CON ) % NUM_CON;
	con = &consoles[con_iActive];
}

void Con_NextTab( void ) {
	Con_SetActiveTab( con_iActive + 1 );
}

void Con_PrevTab( void ) {
	Con_SetActiveTab( con_iActive - 1 );
}

static void Con_NextTab_f( void ) {
	Con_NextTab();
}

static void Con_PrevTab_f( void ) {
	Con_PrevTab();
}

/*
================
Con_Clear_f
================
*/
static void Con_Clear_f( void ) {
	int i, ci;

	for ( ci = 0; ci < NUM_CON; ci++ ) {
		console_t *c = &consoles[ci];
		for ( i = 0; i < c->linewidth; i++ ) {
			c->text[i] = ( ColorIndex( COLOR_WHITE ) << 8 ) | ' ';
		}
		c->x = 0;
		c->current = 0;
		c->newline = qtrue;
	}

	Con_Bottom(); // go to end (active console)
}

						
/*
================
Con_Dump_f

Save the console contents out to a file
================
*/
static void Con_Dump_f( void )
{
	int		l, x, i, n;
	short	*line;
	fileHandle_t	f;
	int		bufferlen;
	char	*buffer;
	char	filename[ MAX_OSPATH ];
	const char *ext;

	if ( Cmd_Argc() != 2 )
	{
		Com_Printf( "usage: condump <filename>\n" );
		return;
	}

	Q_strncpyz( filename, Cmd_Argv( 1 ), sizeof( filename ) );
	COM_DefaultExtension( filename, sizeof( filename ), ".txt" );

	if ( !FS_AllowedExtension( filename, qfalse, &ext ) ) {
		Com_Printf( "%s: Invalid filename extension '%s'.\n", __func__, ext );
		return;
	}

	f = FS_FOpenFileWrite( filename );
	if ( f == FS_INVALID_HANDLE )
	{
		Com_Printf( "ERROR: couldn't open %s.\n", filename );
		return;
	}

	Com_Printf( "Dumped console text to %s.\n", filename );

	if ( con->current >= con->totallines ) {
		n = con->totallines;
		l = con->current + 1;
	} else {
		n = con->current + 1;
		l = 0;
	}

	bufferlen = con->linewidth + ARRAY_LEN( Q_NEWLINE ) * sizeof( char );
	buffer = Hunk_AllocateTempMemory( bufferlen );

	// write the remaining lines
	buffer[ bufferlen - 1 ] = '\0';

	for ( i = 0; i < n ; i++, l++ ) 
	{
		line = con->text + ( l % con->totallines ) * con->linewidth;
		// store line
		for ( x = 0; x < con->linewidth; x++ )
			buffer[ x ] = line[ x ] & 0xff;
		buffer[con->linewidth] = '\0';
		// terminate on ending space characters
		for ( x = con->linewidth - 1; x >= 0; x-- ) {
			if ( buffer[ x ] == ' ' )
				buffer[ x ] = '\0';
			else
				break;
		}
		Q_strcat( buffer, bufferlen, Q_NEWLINE );
		FS_Write( buffer, strlen( buffer ), f );
	}

	Hunk_FreeTempMemory( buffer );
	FS_FCloseFile( f );
}

						
/*
================
Con_ClearNotify
================
*/
void Con_ClearNotify( void ) {
	int i, ci;

	for ( ci = 0; ci < NUM_CON; ci++ )
		for ( i = 0; i < NUM_CON_TIMES; i++ ) {
			consoles[ci].times[i] = 0;
		}
}


/*
================
Con_CheckResize

If the line width has changed, reformat the buffer.
================
*/
void Con_CheckResize( void )
{
	int		i, j, width, oldwidth, oldtotallines, oldcurrent, numlines, numchars;
	short	tbuf[CON_TEXTSIZE], *src, *dst;
	static int old_width, old_vispage;
	int		vispage;
	float	scale;
	int ci;
	console_t *con;

	if ( consoles[CON_ALL].viswidth == cls.glconfig.vidWidth && !con_scale->modified ) {
		return;
	}

	scale = con_scale->value;

	for ( ci = 0; ci < NUM_CON; ci++ )
		consoles[ci].viswidth = cls.glconfig.vidWidth;

	smallchar_width = SMALLCHAR_WIDTH * scale * cls.con_factor;
	smallchar_height = SMALLCHAR_HEIGHT * scale * cls.con_factor;
	bigchar_width = BIGCHAR_WIDTH * scale * cls.con_factor;
	bigchar_height = BIGCHAR_HEIGHT * scale * cls.con_factor;

	if ( cls.glconfig.vidWidth == 0 ) // video hasn't been initialized yet
	{
		g_console_field_width = DEFAULT_CONSOLE_WIDTH;
		width = DEFAULT_CONSOLE_WIDTH * scale;
		for ( ci = 0; ci < NUM_CON; ci++ ) {
			consoles[ci].linewidth = width;
			consoles[ci].totallines = CON_TEXTSIZE / width;
			consoles[ci].vispage = 4;
		}
		Con_Clear_f();
		con_scale->modified = qfalse;
		return;
	}

	width = ( ( cls.glconfig.vidWidth / smallchar_width ) - 2 );

	g_console_field_width = width;
	g_consoleField.widthInChars = g_console_field_width;

	if ( width > MAX_CONSOLE_WIDTH )
		width = MAX_CONSOLE_WIDTH;

	vispage = cls.glconfig.vidHeight / ( smallchar_height * 2 ) - 1;

	if ( old_vispage == vispage && old_width == width )
		return;

	old_vispage = vispage;
	old_width = width;

	// reformat every console buffer to the new width
	for ( ci = 0; ci < NUM_CON; ci++ ) {
		con = &consoles[ci];

		oldwidth = con->linewidth;
		oldtotallines = con->totallines;
		oldcurrent = con->current;

		con->linewidth = width;
		con->totallines = CON_TEXTSIZE / con->linewidth;
		con->vispage = vispage;

		numchars = oldwidth;
		if ( numchars > con->linewidth )
			numchars = con->linewidth;

		if ( oldcurrent > oldtotallines )
			numlines = oldtotallines;
		else
			numlines = oldcurrent + 1;

		if ( numlines > con->totallines )
			numlines = con->totallines;

		Com_Memcpy( tbuf, con->text, CON_TEXTSIZE * sizeof( short ) );

		for ( i = 0; i < CON_TEXTSIZE; i++ )
			con->text[i] = ( ColorIndex( COLOR_WHITE ) << 8 ) | ' ';

		if ( oldwidth > 0 && oldtotallines > 0 ) {
			for ( i = 0; i < numlines; i++ ) {
				src = &tbuf[( ( oldcurrent - i + oldtotallines ) % oldtotallines ) * oldwidth];
				dst = &con->text[( numlines - 1 - i ) * con->linewidth];
				for ( j = 0; j < numchars; j++ )
					*dst++ = *src++;
			}
		}

		con->current = numlines - 1;
		con->display = con->current;
	}

	Con_ClearNotify();

	con_scale->modified = qfalse;
}


/*
==================
Cmd_CompleteTxtName
==================
*/
static void Cmd_CompleteTxtName(const char *args, int argNum ) {
	if ( argNum == 2 ) {
		Field_CompleteFilename( "", "txt", qfalse, FS_MATCH_EXTERN | FS_MATCH_STICK );
	}
}


/*
================
Con_Init
================
*/
void Con_Init( void ) 
{
	con_notifytime = Cvar_Get( "con_notifytime", "3", 0 );
	Cvar_SetDescription( con_notifytime, "Defines how long messages (from players or the system) are on the screen (in seconds)." );
	con_conspeed = Cvar_Get( "scr_conspeed", "3", 0 );
	Cvar_SetDescription( con_conspeed, "Console opening/closing scroll speed." );
	con_autoclear = Cvar_Get("con_autoclear", "1", CVAR_ARCHIVE_ND);
	Cvar_SetDescription( con_autoclear, "Enable/disable clearing console input text when console is closed." );
	con_scale = Cvar_Get( "con_scale", "1", CVAR_ARCHIVE_ND );
	Cvar_CheckRange( con_scale, "0.5", "8", CV_FLOAT );
	Cvar_SetDescription( con_scale, "Console font size scale." );
	con_tabs = Cvar_Get( "con_tabs", "1", CVAR_ARCHIVE_ND );
	Cvar_SetDescription( con_tabs, "Show the tabbed-console tab bar (All/Chat); use con_nexttab/con_prevtab to switch." );
	con_tabScale = Cvar_Get( "con_tabScale", "1.25", CVAR_ARCHIVE_ND );
	Cvar_CheckRange( con_tabScale, "1.0", "3.0", CV_FLOAT );
	Cvar_SetDescription( con_tabScale, "Tab-title font size as a multiple of the console body font (1.0 = same size)." );
	con_height = Cvar_Get( "con_height", "0.5", CVAR_ARCHIVE_ND );
	Cvar_CheckRange( con_height, "0.1", "1.0", CV_FLOAT );
	Cvar_SetDescription( con_height, "Fraction of the screen the open console covers (0.5 = half screen)." );
	con_opacity = Cvar_Get( "con_opacity", "0.8", CVAR_ARCHIVE_ND );
	Cvar_CheckRange( con_opacity, "0", "1", CV_FLOAT );
	Cvar_SetDescription( con_opacity, "Opacity of the console background (1 = opaque, 0 = fully transparent)." );
	con_notifyLines = Cvar_Get( "con_notifyLines", "4", CVAR_ARCHIVE_ND );
	Cvar_CheckRange( con_notifyLines, "0", "8", CV_INTEGER );
	Cvar_SetDescription( con_notifyLines, "Number of notify lines shown over the game (0 disables the notify area)." );
	con_notifyY = Cvar_Get( "con_notifyY", "0", CVAR_ARCHIVE_ND );
	Cvar_CheckRange( con_notifyY, "0", "600", CV_INTEGER );
	Cvar_SetDescription( con_notifyY, "Vertical offset of the notify area, in pixels from the top of the screen." );

	Field_Clear( &g_consoleField );
	g_consoleField.widthInChars = g_console_field_width;

	Cmd_AddCommand( "con_nexttab", Con_NextTab_f );
	Cmd_AddCommand( "con_prevtab", Con_PrevTab_f );
	Cmd_AddCommand( "clear", Con_Clear_f );
	Cmd_AddCommand( "condump", Con_Dump_f );
	Cmd_SetCommandCompletionFunc( "condump", Cmd_CompleteTxtName );
	Cmd_AddCommand( "toggleconsole", Con_ToggleConsole_f );
	Cmd_AddCommand( "messagemode", Con_MessageMode_f );
	Cmd_AddCommand( "messagemode2", Con_MessageMode2_f );
	Cmd_AddCommand( "messagemode3", Con_MessageMode3_f );
	Cmd_AddCommand( "messagemode4", Con_MessageMode4_f );
	Cmd_AddCommand( "messagemode5", Con_MessageMode5_f );
	Cmd_AddCommand( "tellme", Con_Tellme_f );
}


/*
================
Con_Shutdown
================
*/
void Con_Shutdown( void )
{
	Cmd_RemoveCommand( "con_nexttab" );
	Cmd_RemoveCommand( "con_prevtab" );
	Cmd_RemoveCommand( "clear" );
	Cmd_RemoveCommand( "condump" );
	Cmd_RemoveCommand( "toggleconsole" );
	Cmd_RemoveCommand( "messagemode" );
	Cmd_RemoveCommand( "messagemode2" );
	Cmd_RemoveCommand( "messagemode3" );
	Cmd_RemoveCommand( "messagemode4" );
	Cmd_RemoveCommand( "messagemode5" );
	Cmd_RemoveCommand( "tellme" );
}


/*
===============
Con_Fixup
===============
*/
static void Con_Fixup( console_t *con ) {
	int filled;

	if ( con->current >= con->totallines ) {
		filled = con->totallines;
	} else {
		filled = con->current + 1;
	}

	if ( filled <= con->vispage ) {
		con->display = con->current;
	} else if ( con->current - con->display > filled - con->vispage ) {
		con->display = con->current - filled + con->vispage;
	} else if ( con->display > con->current ) {
		con->display = con->current;
	}
}

/*
===============
Con_Linefeed

Move to newline only when we _really_ need this
===============
*/
static void Con_NewLine( console_t *con ) {
	short *s;
	int i;

	// follow last line
	if ( con->display == con->current )
		con->display++;
	con->current++;

	s = &con->text[( con->current % con->totallines ) * con->linewidth];
	for ( i = 0; i < con->linewidth; i++ )
		*s++ = (ColorIndex(COLOR_WHITE)<<8) | ' ';

	con->x = 0;
}

/*
===============
Con_Linefeed
===============
*/
static void Con_Linefeed( console_t *con, qboolean skipnotify ) {
	// mark time for transparent overlay
	if ( con->current >= 0 ) {
		if ( skipnotify )
			con->times[con->current % NUM_CON_TIMES] = 0;
		else
			con->times[con->current % NUM_CON_TIMES] = cls.realtime;
	}

	if ( con->newline ) {
		Con_NewLine( con );
	} else {
		con->newline = qtrue;
		con->x = 0;
	}

	Con_Fixup( con );
}

/*
================
CL_ConsolePrint

Handles cursor positioning, line wrapping, etc
All console printing must go through this in order to be logged to disk
If no console is visible, the text will appear at the top of the game window
================
*/
static void Con_WriteText( console_t *con, const char *txt, qboolean skipnotify ) {
	int		y;
	int		c, l;
	int		colorIndex;
	int prev;

	colorIndex = ColorIndex( COLOR_WHITE );

	while ( (c = *txt) != 0 ) {
		if ( Q_IsColorString( txt ) && *(txt+1) != '\n' ) {
			colorIndex = ColorIndexFromChar( *(txt+1) );
			txt += 2;
			continue;
		}

		// count word length
		for ( l = 0; l < con->linewidth; l++ ) {
			if ( txt[l] <= ' ' ) {
				break;
			}
		}

		// word wrap
		if ( l != con->linewidth && ( con->x + l >= con->linewidth ) ) {
			Con_Linefeed( con, skipnotify );
		}

		txt++;

		switch( c )
		{
		case '\n':
			Con_Linefeed( con, skipnotify );
			break;
		case '\r':
			con->x = 0;
			break;
		default:
			if ( con->newline ) {
				Con_NewLine( con );
				Con_Fixup( con );
				con->newline = qfalse;
			}
			// display character and advance
			y = con->current % con->totallines;
			con->text[y * con->linewidth + con->x] = ( colorIndex << 8 ) | ( c & 255 );
			con->x++;
			if ( con->x >= con->linewidth ) {
				Con_Linefeed( con, skipnotify );
			}
			break;
		}
	}

	// mark time for transparent overlay
	if ( con->current >= 0 ) {
		if ( skipnotify ) {
			prev = con->current % NUM_CON_TIMES - 1;
			if ( prev < 0 )
				prev = NUM_CON_TIMES - 1;
			con->times[prev] = 0;
		} else {
			con->times[con->current % NUM_CON_TIMES] = cls.realtime;
		}
	}
}

/*
================
CL_ConsolePrint

Pick the category tab for a message. UrT's game mod marks frag (kill) lines
with a leading 0x11/0x12 byte and chat lines with 0x13; the marker is stripped
here. Unmarked text goes to the General tab. Every message is also logged to All.
================
*/
void CL_ConsolePrint( const char *txt ) {
	qboolean skipnotify = qfalse; // NERVE - SMF
	console_t *target;

	// TTimo - prefix for text that shows up in console but not in notify
	// backported from RTCW
	if ( !Q_strncmp( txt, "[skipnotify]", 12 ) ) {
		skipnotify = qtrue;
		txt += 12;
	}

	// for some demos we don't want to ever show anything on the console
	if ( cl_noprint && cl_noprint->integer ) {
		return;
	}

	if ( !consoles[CON_ALL].initialized ) {
		static cvar_t null_cvar = { 0 };
		int ci;
		for ( ci = 0; ci < NUM_CON; ci++ ) {
			consoles[ci].color[0] = consoles[ci].color[1] =
			    consoles[ci].color[2] = consoles[ci].color[3] = 1.0f;
			consoles[ci].viswidth = -9999;
			consoles[ci].initialized = qtrue;
		}
		cls.con_factor = 1.0f;
		con_scale = &null_cvar;
		con_scale->value = 1.0f;
		con_scale->modified = qtrue;
		Con_CheckResize();
	}

	// route to a category tab based on the game mod's leading marker byte
	if ( txt[0] == 17 || txt[0] == 18 ) {
		target = &consoles[CON_FRAG];
		txt++;
	} else if ( txt[0] == 19 ) {
		target = &consoles[CON_CHAT];
		txt++;
	} else {
		target = &consoles[CON_GENERAL];
	}

	Con_WriteText( &consoles[CON_ALL], txt, skipnotify );
	Con_WriteText( target, txt, skipnotify );
}

/*
==============================================================================

DRAWING

==============================================================================
*/


/*
================
Con_DrawInput

Draw the editline after a ] prompt
================
*/
static void Con_DrawInput( void ) {
	int		y;

	if ( cls.state != CA_DISCONNECTED && !(Key_GetCatcher( ) & KEYCATCH_CONSOLE ) ) {
		return;
	}

	y = con->vislines - smallchar_height - ( smallchar_height / 3 );

	re.SetColor( con->color );

	SCR_DrawSmallChar( con->xadjust + 1 * smallchar_width, y, ']' );

	Field_Draw( &g_consoleField, con->xadjust + 2 * smallchar_width, y,
	            SCREEN_WIDTH - 3 * smallchar_width, qtrue, qtrue );
}


/*
================
Con_DrawNotify

Draws the last few lines of output transparently over the game top
================
*/
static void Con_DrawNotify( void )
{
	int		x, v;
	short	*text;
	int		i;
	int		time;
	int		skip;
	int		currentColorIndex;
	int		colorIndex;
	int notifyLines;

	currentColorIndex = ColorIndex( COLOR_WHITE );
	re.SetColor( g_color_table[ currentColorIndex ] );

	// number of notify lines to show, clamped to the ring buffer size
	notifyLines = con_notifyLines->integer;
	if ( notifyLines <= 0 )
		return;
	if ( notifyLines > NUM_CON_TIMES )
		notifyLines = NUM_CON_TIMES;

	v = con_notifyY->integer;
	for ( i = con->current - notifyLines + 1; i <= con->current; i++ ) {
		if (i < 0)
			continue;
		time = con->times[i % NUM_CON_TIMES];
		if (time == 0)
			continue;
		time = cls.realtime - time;
		if ( time >= con_notifytime->value*1000 )
			continue;
		text = con->text + ( i % con->totallines ) * con->linewidth;

		if (cl.snap.ps.pm_type != PM_INTERMISSION && Key_GetCatcher( ) & (KEYCATCH_UI | KEYCATCH_CGAME) ) {
			continue;
		}

		for ( x = 0; x < con->linewidth; x++ ) {
			if ( ( text[x] & 0xff ) == ' ' ) {
				continue;
			}
			colorIndex = ( text[x] >> 8 ) & 63;
			if ( currentColorIndex != colorIndex ) {
				currentColorIndex = colorIndex;
				re.SetColor( g_color_table[ colorIndex ] );
			}
			SCR_DrawSmallChar( cl_conXOffset->integer + con->xadjust + ( x + 1 ) * smallchar_width, v, text[x] & 0xff );
		}

		v += smallchar_height;
	}

	re.SetColor( NULL );

	if ( Key_GetCatcher() & (KEYCATCH_UI | KEYCATCH_CGAME) ) {
		return;
	}

	// draw the chat line
	if ( Key_GetCatcher( ) & KEYCATCH_MESSAGE )
	{
		// rescale to virtual 640x480 space
		v /= cls.glconfig.vidHeight / 480.0;

		if (chat_team)
		{
			SCR_DrawBigString( SMALLCHAR_WIDTH, v, "say_team:", 1.0f, qfalse );
			skip = 10;
		}
		else
		{
			SCR_DrawBigString( SMALLCHAR_WIDTH, v, "say:", 1.0f, qfalse );
			skip = 5;
		}

		Field_BigDraw( &chatField, skip * BIGCHAR_WIDTH, v,
			SCREEN_WIDTH - ( skip + 1 ) * BIGCHAR_WIDTH, qtrue, qtrue );
	}
}

/*
================
Con_DrawScaledChar

Like SCR_DrawSmallChar but at an arbitrary pixel size, so the tab titles can
be drawn a bit larger than the body text. Drawn in native screen space.
================
*/
static void Con_DrawScaledChar( int x, int y, int w, int h, int ch ) {
	int row, col;
	float frow, fcol;
	const float size = 0.0625f;

	ch &= 255;
	if ( ch == ' ' ) {
		return;
	}

	row = ch >> 4;
	col = ch & 15;
	frow = row * size;
	fcol = col * size;

	re.DrawStretchPic( x, y, w, h, fcol, frow, fcol + size, frow + size, cls.charSetShader );
}

/*
================
Con_DrawSolidConsole

Draws the console with the solid background
================
*/
static void Con_DrawSolidConsole( float frac ) {

	static float conColorValue[4] = { 0.0, 0.0, 0.0, 0.0 };
	// for cvar value change tracking
	static char  conColorString[ MAX_CVAR_VALUE_STRING ] = { '\0' };

	int				i, x, y;
	int				rows;
	short			*text;
	int				row;
	int				lines;
	int				currentColorIndex;
	int				colorIndex;
	float			yf, wf;
	vec4_t conDrawColor;
	int tabX0 = 0, tabX1 = 0; // x-span covered by the tab bar (0 = none)
	char			buf[ MAX_CVAR_VALUE_STRING ], *v[4];

	lines = cls.glconfig.vidHeight * frac;
	if ( lines <= 0 )
		return;

	if ( re.FinishBloom )
		re.FinishBloom();

	if ( lines > cls.glconfig.vidHeight )
		lines = cls.glconfig.vidHeight;

	wf = SCREEN_WIDTH;

	// draw the background
	yf = frac * SCREEN_HEIGHT;

	// on wide screens, we will center the text
	con->xadjust = 0;
	SCR_AdjustFrom640( &con->xadjust, &yf, &wf, NULL );

	if ( yf < 1.0 ) {
		yf = 0;
	} else {
		// custom console background color
		if ( cl_conColor->string[0] ) {
			// track changes
			if ( strcmp( cl_conColor->string, conColorString ) ) 
			{
				Q_strncpyz( conColorString, cl_conColor->string, sizeof( conColorString ) );
				Q_strncpyz( buf, cl_conColor->string, sizeof( buf ) );
				Com_Split( buf, v, 4, ' ' );
				for ( i = 0; i < 4 ; i++ ) {
					conColorValue[ i ] = Q_atof( v[ i ] ) / 255.0f;
					if ( conColorValue[ i ] > 1.0f ) {
						conColorValue[ i ] = 1.0f;
					} else if ( conColorValue[ i ] < 0.0f ) {
						conColorValue[ i ] = 0.0f;
					}
				}
			}
			// apply con_opacity on a local copy so the cached value is preserved
			Vector4Copy( conColorValue, conDrawColor );
			conDrawColor[3] *= con_opacity->value;
			re.SetColor( conDrawColor );
			re.DrawStretchPic( 0, 0, wf, yf, 0, 0, 1, 1, cls.whiteShader );
		} else {
			Vector4Set( conDrawColor, 1.0f, 1.0f, 1.0f, con_opacity->value );
			re.SetColor( conDrawColor );
			re.DrawStretchPic( 0, 0, wf, yf, 0, 0, 1, 1, cls.consoleShader );
		}

	}

	//y = yf;

	// draw the version number
	SCR_DrawSmallString( cls.glconfig.vidWidth - ( ARRAY_LEN( Q3_VERSION ) ) * smallchar_width,
		lines - smallchar_height, Q3_VERSION, ARRAY_LEN( Q3_VERSION ) - 1 );

	// draw the tab bar (tabbed console) as real tabs: filled background per tab,
	// the active one highlighted, with a 1px border. Drawn in pixel space to line
	// up with the small-char text. Switch with shift+left/right or mouse buttons.
	if ( con_tabs->integer ) {
		// filled backgrounds + a red outline on the sides + bottom, so the red
		// border wraps the tabs as one shape with the console. Fills and border
		// are faded by con_opacity to match the (translucent) console background.
		static const vec4_t bgActive = { 0.20f, 0.20f, 0.24f, 1.00f };
		static const vec4_t bgInactive = { 0.07f, 0.07f, 0.09f, 1.00f };
		vec4_t bgA, bgI, red;
		float op = con_opacity->value;
		// tab titles are drawn a bit larger than the body text (con_tabScale)
		float tscale = con_tabScale->value;
		int cw, chh, th, ty, tx, tpad, t, k, tw;

		Vector4Copy( bgActive, bgA );
		Vector4Copy( bgInactive, bgI );
		Vector4Copy( g_color_table[ColorIndex( COLOR_RED )], red );
		bgA[3] *= op;
		bgI[3] *= op;
		red[3] *= op;

		// con_tabScale is range-checked at registration, but clamp the derived
		// pixel sizes to explicit bounds so the tab-layout arithmetic below is
		// provably finite (keeps CodeQL's uncontrolled-arithmetic check satisfied).
		if ( tscale < 1.0f )
			tscale = 1.0f;
		else if ( tscale > 3.0f )
			tscale = 3.0f;
		cw = (int)( smallchar_width * tscale );
		chh = (int)( smallchar_height * tscale );
		// clamp on both sides: a closed [1,256] range lets the analyzer prove the
		// width/position products below cannot overflow (an upper bound alone
		// leaves cw potentially negative, which still trips the multiply check).
		if ( cw < 1 )
			cw = 1;
		else if ( cw > 256 )
			cw = 256;
		if ( chh < 1 )
			chh = 1;
		else if ( chh > 256 )
			chh = 256;
		th = chh + 4;
		ty = lines; // hang the tabs just below the console panel
		tx = smallchar_width;
		tabX0 = tx;              // left edge of the tab strip (separator skips this span)
		tpad = ( th - chh ) / 2; // vertical centering of the title glyphs

		// keep them on-screen when the console is fully open (frac == 1)
		if ( ty + th > cls.glconfig.vidHeight )
			ty = cls.glconfig.vidHeight - th;
		for ( t = 0; t < NUM_CON; t++ ) {
			qboolean active = ( t == con_iActive );
			// bound the title length with a literal so the width/position
			// products below have both factors bounded (CodeQL-clean overflow).
			int nlen = (int)strlen( con_names[t] );
			if ( nlen > 32 )
				nlen = 32;
			tw = ( nlen + 2 ) * cw;

			re.SetColor( active ? bgA : bgI );
			re.DrawStretchPic( tx, ty, tw, th, 0, 0, 0, 0, cls.whiteShader );

			re.SetColor( red );
			re.DrawStretchPic( tx, ty, 1, th, 0, 0, 0, 0, cls.whiteShader );          // left
			re.DrawStretchPic( tx + tw - 1, ty, 1, th, 0, 0, 0, 0, cls.whiteShader ); // right
			re.DrawStretchPic( tx, ty + th - 2, tw, 2, 0, 0, 0, 0, cls.whiteShader ); // bottom

			re.SetColor( g_color_table[ColorIndex( active ? COLOR_YELLOW : COLOR_WHITE )] );
			for ( k = 0; k < nlen; k++ )
				Con_DrawScaledChar( tx + ( k + 1 ) * cw, ty + tpad, cw, chh, con_names[t][k] );

			tx += tw;
		}
		re.SetColor( NULL );
		tabX1 = tx; // right edge of the tab strip
	}

	// the panel's bottom red separator, faded with the background (con_opacity).
	// When the tab bar is shown, skip the span it covers so the panel and tabs
	// read as one shape (no red line bleeding through the translucent tab fills).
	Vector4Copy( g_color_table[ColorIndex( COLOR_RED )], conDrawColor );
	conDrawColor[3] *= con_opacity->value;
	re.SetColor( conDrawColor );
	if ( tabX1 > tabX0 ) {
		if ( tabX0 > 0 )
			re.DrawStretchPic( 0, yf, tabX0, 2, 0, 0, 1, 1, cls.whiteShader ); // left of tabs
		if ( tabX1 < wf )
			re.DrawStretchPic( tabX1, yf, wf - tabX1, 2, 0, 0, 1, 1, cls.whiteShader ); // right of tabs
	} else {
		re.DrawStretchPic( 0, yf, wf, 2, 0, 0, 1, 1, cls.whiteShader );
	}

	// draw the text
	con->vislines = lines;
	rows = lines / smallchar_width - 1;	// rows of text to draw

	y = lines - (smallchar_height * 3);

	row = con->display;

	// draw from the bottom up
	if ( con->display != con->current ) {
		// draw arrows to show the buffer is backscrolled
		re.SetColor( g_color_table[ ColorIndex( COLOR_RED ) ] );
		for ( x = 0; x < con->linewidth; x += 4 )
			SCR_DrawSmallChar( con->xadjust + ( x + 1 ) * smallchar_width, y, '^' );
		y -= smallchar_height;
		row--;
	}

#ifdef USE_CURL
	if ( download.progress[ 0 ] ) 
	{
		currentColorIndex = ColorIndex( COLOR_CYAN );
		re.SetColor( g_color_table[ currentColorIndex ] );

		i = strlen( download.progress );
		for ( x = 0 ; x < i ; x++ ) 
		{
			SCR_DrawSmallChar( ( x + 1 ) * smallchar_width,
				lines - smallchar_height, download.progress[x] );
		}
	}
#endif

	currentColorIndex = ColorIndex( COLOR_WHITE );
	re.SetColor( g_color_table[ currentColorIndex ] );

	for ( i = 0 ; i < rows ; i++, y -= smallchar_height, row-- )
	{
		if ( row < 0 )
			break;

		if ( con->current - row >= con->totallines ) {
			// past scrollback wrap point
			continue;
		}

		text = con->text + ( row % con->totallines ) * con->linewidth;

		for ( x = 0; x < con->linewidth; x++ ) {
			// skip rendering whitespace
			if ( ( text[x] & 0xff ) == ' ' ) {
				continue;
			}
			// track color changes
			colorIndex = ( text[ x ] >> 8 ) & 63;
			if ( currentColorIndex != colorIndex ) {
				currentColorIndex = colorIndex;
				re.SetColor( g_color_table[ colorIndex ] );
			}
			SCR_DrawSmallChar( con->xadjust + ( x + 1 ) * smallchar_width, y, text[x] & 0xff );
		}
	}

	// draw the input prompt, user text, and cursor if desired
	Con_DrawInput();

	re.SetColor( NULL );
}


/*
==================
Con_DrawConsole
==================
*/
void Con_DrawConsole( void ) {

	// check for console width changes from a vid mode change
	Con_CheckResize();

	// if disconnected, render console full screen
	if ( cls.state == CA_DISCONNECTED ) {
		if ( !( Key_GetCatcher( ) & (KEYCATCH_UI | KEYCATCH_CGAME)) ) {
			Con_DrawSolidConsole( 1.0 );
			return;
		}
	}

	if ( con->displayFrac ) {
		Con_DrawSolidConsole( con->displayFrac );
	} else {
		// draw notify lines
		if ( cls.state == CA_ACTIVE ) {
			Con_DrawNotify();
		}
	}
}

//================================================================

/*
==================
Con_RunConsole

Scroll it up or down
==================
*/
void Con_RunConsole( void ) 
{
	// decide on the destination height of the console
	if ( Key_GetCatcher( ) & KEYCATCH_CONSOLE )
		con->finalFrac = con_height->value; // range-checked 0.1-1.0 at registration
	else
		con->finalFrac = 0.0; // none visible

	// scroll towards the destination height
	if ( con->finalFrac < con->displayFrac ) {
		con->displayFrac -= con_conspeed->value * cls.realFrametime * 0.001;
		if ( con->finalFrac > con->displayFrac )
			con->displayFrac = con->finalFrac;

	} else if ( con->finalFrac > con->displayFrac ) {
		con->displayFrac += con_conspeed->value * cls.realFrametime * 0.001;
		if ( con->finalFrac < con->displayFrac )
			con->displayFrac = con->finalFrac;
	}
}


void Con_PageUp( int lines )
{
	if ( lines == 0 )
		lines = con->vispage - 2;

	con->display -= lines;

	Con_Fixup( con );
}


void Con_PageDown( int lines )
{
	if ( lines == 0 )
		lines = con->vispage - 2;

	con->display += lines;

	Con_Fixup( con );
}


void Con_Top( void )
{
	// this is generally incorrect but will be adjusted in Con_Fixup( con )
	con->display = con->current - con->totallines;

	Con_Fixup( con );
}


void Con_Bottom( void )
{
	con->display = con->current;

	Con_Fixup( con );
}


void Con_Close( void )
{
	if ( !com_cl_running->integer )
		return;

	Field_Clear( &g_consoleField );
	Con_ClearNotify();
	Key_SetCatcher( Key_GetCatcher( ) & ~KEYCATCH_CONSOLE );
	con->finalFrac = 0.0; // none visible
	con->displayFrac = 0.0;
}
