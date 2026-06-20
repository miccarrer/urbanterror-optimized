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
// cl_scrn.c -- master for refresh, status bar, console, chat, notify, etc

#include "client.h"

static qboolean	scr_initialized;		// ready to draw

cvar_t		*cl_timegraph;
static cvar_t		*cl_debuggraph;
static cvar_t		*cl_graphheight;
static cvar_t		*cl_graphscale;
static cvar_t		*cl_graphshift;

/*
================
SCR_DrawNamedPic

Coordinates are 640*480 virtual values
=================
*/
void SCR_DrawNamedPic( float x, float y, float width, float height, const char *picname ) {
	qhandle_t	hShader;

	assert( width != 0 );

	hShader = re.RegisterShader( picname );
	SCR_AdjustFrom640( &x, &y, &width, &height );
	re.DrawStretchPic( x, y, width, height, 0, 0, 1, 1, hShader );
}


/*
================
SCR_AdjustFrom640

Adjusted for resolution and screen aspect ratio
================
*/
void SCR_AdjustFrom640( float *x, float *y, float *w, float *h ) {
	float	xscale;
	float	yscale;

#if 0
		// adjust for wide screens
		if ( cls.glconfig.vidWidth * 480 > cls.glconfig.vidHeight * 640 ) {
			*x += 0.5 * ( cls.glconfig.vidWidth - ( cls.glconfig.vidHeight * 640 / 480 ) );
		}
#endif

	// scale for screen sizes
	xscale = cls.glconfig.vidWidth / 640.0;
	yscale = cls.glconfig.vidHeight / 480.0;
	if ( x ) {
		*x *= xscale;
	}
	if ( y ) {
		*y *= yscale;
	}
	if ( w ) {
		*w *= xscale;
	}
	if ( h ) {
		*h *= yscale;
	}
}

/*
================
SCR_FillRect

Coordinates are 640*480 virtual values
=================
*/
void SCR_FillRect( float x, float y, float width, float height, const float *color ) {
	re.SetColor( color );

	SCR_AdjustFrom640( &x, &y, &width, &height );
	re.DrawStretchPic( x, y, width, height, 0, 0, 0, 0, cls.whiteShader );

	re.SetColor( NULL );
}


/*
================
SCR_DrawPic

Coordinates are 640*480 virtual values
=================
*/
void SCR_DrawPic( float x, float y, float width, float height, qhandle_t hShader ) {
	SCR_AdjustFrom640( &x, &y, &width, &height );
	re.DrawStretchPic( x, y, width, height, 0, 0, 1, 1, hShader );
}


/*
** SCR_DrawChar
** chars are drawn at 640*480 virtual screen size
*/
static void SCR_DrawChar( int x, int y, float size, int ch ) {
	int row, col;
	float frow, fcol;
	float	ax, ay, aw, ah;

	ch &= 255;

	if ( ch == ' ' ) {
		return;
	}

	if ( y < -size ) {
		return;
	}

	ax = x;
	ay = y;
	aw = size;
	ah = size;
	SCR_AdjustFrom640( &ax, &ay, &aw, &ah );

	row = ch>>4;
	col = ch&15;

	frow = row*0.0625;
	fcol = col*0.0625;
	size = 0.0625;

	re.DrawStretchPic( ax, ay, aw, ah,
					   fcol, frow, 
					   fcol + size, frow + size, 
					   cls.charSetShader );
}


/*
** SCR_DrawSmallChar
** small chars are drawn at native screen resolution
*/
void SCR_DrawSmallChar( int x, int y, int ch ) {
	int row, col;
	float frow, fcol;
	float size;

	ch &= 255;

	if ( ch == ' ' ) {
		return;
	}

	if ( y < -smallchar_height ) {
		return;
	}

	row = ch>>4;
	col = ch&15;

	frow = row*0.0625;
	fcol = col*0.0625;
	size = 0.0625;

	re.DrawStretchPic( x, y, smallchar_width, smallchar_height,
					   fcol, frow, 
					   fcol + size, frow + size, 
					   cls.charSetShader );
}


/*
** SCR_DrawSmallString
** small string are drawn at native screen resolution
*/
void SCR_DrawSmallString( int x, int y, const char *s, int len ) {
	int row, col, ch, i;
	float frow, fcol;
	float size;

	if ( y < -smallchar_height ) {
		return;
	}

	size = 0.0625;

	for ( i = 0; i < len; i++ ) {
		ch = *s++ & 255;
		row = ch>>4;
		col = ch&15;

		frow = row*0.0625;
		fcol = col*0.0625;

		re.DrawStretchPic( x, y, smallchar_width, smallchar_height,
						   fcol, frow, fcol + size, frow + size, 
						   cls.charSetShader );

		x += smallchar_width;
	}
}


/*
==================
SCR_DrawBigString[Color]

Draws a multi-colored string with a drop shadow, optionally forcing
to a fixed color.

Coordinates are at 640 by 480 virtual resolution
==================
*/
void SCR_DrawStringExt( int x, int y, float size, const char *string, const float *setColor, qboolean forceColor,
		qboolean noColorEscape ) {
	vec4_t		color;
	const char	*s;
	int			xx;

	// draw the drop shadow
	color[0] = color[1] = color[2] = 0.0;
	color[3] = setColor[3];
	re.SetColor( color );
	s = string;
	xx = x;
	while ( *s ) {
		if ( !noColorEscape && Q_IsColorString( s ) ) {
			s += 2;
			continue;
		}
		SCR_DrawChar( xx+2, y+2, size, *s );
		xx += size;
		s++;
	}


	// draw the colored text
	s = string;
	xx = x;
	re.SetColor( setColor );
	while ( *s ) {
		if ( Q_IsColorString( s ) ) {
			if ( !forceColor ) {
				Com_Memcpy( color, g_color_table[ ColorIndexFromChar( *(s+1) ) ], sizeof( color ) );
				color[3] = setColor[3];
				re.SetColor( color );
			}
			if ( !noColorEscape ) {
				s += 2;
				continue;
			}
		}
		SCR_DrawChar( xx, y, size, *s );
		xx += size;
		s++;
	}
	re.SetColor( NULL );
}


/*
==================
SCR_DrawBigString
==================
*/
void SCR_DrawBigString( int x, int y, const char *s, float alpha, qboolean noColorEscape ) {
	float	color[4];

	color[0] = color[1] = color[2] = 1.0;
	color[3] = alpha;
	SCR_DrawStringExt( x, y, BIGCHAR_WIDTH, s, color, qfalse, noColorEscape );
}


/*
==================
SCR_DrawSmallString[Color]

Draws a multi-colored string with a drop shadow, optionally forcing
to a fixed color.
==================
*/
void SCR_DrawSmallStringExt( int x, int y, const char *string, const float *setColor, qboolean forceColor,
		qboolean noColorEscape ) {
	vec4_t		color;
	const char	*s;
	int			xx;

	// draw the colored text
	s = string;
	xx = x;
	re.SetColor( setColor );
	while ( *s ) {
		if ( Q_IsColorString( s ) ) {
			if ( !forceColor ) {
				Com_Memcpy( color, g_color_table[ ColorIndexFromChar( *(s+1) ) ], sizeof( color ) );
				color[3] = setColor[3];
				re.SetColor( color );
			}
			if ( !noColorEscape ) {
				s += 2;
				continue;
			}
		}
		SCR_DrawSmallChar( xx, y, *s );
		xx += smallchar_width;
		s++;
	}
	re.SetColor( NULL );
}


/*
** SCR_Strlen -- skips color escape codes
*/
static int SCR_Strlen( const char *str ) {
	const char *s = str;
	int count = 0;

	while ( *s ) {
		if ( Q_IsColorString( s ) ) {
			s += 2;
		} else {
			count++;
			s++;
		}
	}

	return count;
}


/*
** SCR_GetBigStringWidth
*/ 
int SCR_GetBigStringWidth( const char *str ) {
	return SCR_Strlen( str ) * BIGCHAR_WIDTH;
}


//===============================================================================

/*
=================
SCR_DrawDemoRecording
=================
*/
static void SCR_DrawDemoRecording( void ) {
	char	string[sizeof(clc.recordNameShort)+32];
	int		pos;

	if ( !clc.demorecording ) {
		return;
	}
	if ( clc.spDemoRecording ) {
		return;
	}

	pos = FS_FTell( clc.recordfile );

	if (cl_drawRecording->integer == 1) {
		sprintf(string, "RECORDING %s: %ik", clc.recordNameShort, pos / 1024);
		SCR_DrawStringExt(320 - strlen(string) * 4, 20, 8, string, g_color_table[ColorIndex(COLOR_WHITE)], qtrue, qfalse);
	} else if (cl_drawRecording->integer == 2) {
		sprintf(string, "RECORDING: %ik", pos / 1024);
		SCR_DrawStringExt(320 - strlen(string) * 4, 20, 8, string, g_color_table[ColorIndex(COLOR_WHITE)], qtrue, qfalse);
	}
}


#ifdef USE_VOIP
/*
=================
SCR_DrawVoipMeter
=================
*/
static void SCR_DrawVoipMeter( void ) {
	char	buffer[16];
	char	string[256];
	int limit, i;

	if (!cl_voipShowMeter->integer)
		return;  // player doesn't want to show meter at all.
	else if (!cl_voipSend->integer)
		return;  // not recording at the moment.
	else if (clc.state != CA_ACTIVE)
		return;  // not connected to a server.
	else if (!clc.voipEnabled)
		return;  // server doesn't support VoIP.
	else if (clc.demoplaying)
		return;  // playing back a demo.
	else if (!cl_voip->integer)
		return;  // client has VoIP support disabled.

	limit = (int) (clc.voipPower * 10.0f);
	if (limit > 10)
		limit = 10;

	for (i = 0; i < limit; i++)
		buffer[i] = '*';
	while (i < 10)
		buffer[i++] = ' ';
	buffer[i] = '\0';

	sprintf( string, "VoIP: [%s]", buffer );
	SCR_DrawStringExt( 320 - strlen( string ) * 4, 10, 8, string, g_color_table[ ColorIndex( COLOR_WHITE ) ], qtrue, qfalse );
}
#endif


/*
===============================================================================

DEBUG GRAPH

===============================================================================
*/

static	int			current;
static	float		values[1024];

/*
==============
SCR_DebugGraph
==============
*/
void SCR_DebugGraph( float value )
{
	values[current] = value;
	current = (current + 1) % ARRAY_LEN(values);
}


/*
==============
SCR_DrawDebugGraph
==============
*/
static void SCR_DrawDebugGraph( void )
{
	int		a, x, y, w, i, h;
	float	v;

	//
	// draw the graph
	//
	w = cls.glconfig.vidWidth;
	x = 0;
	y = cls.glconfig.vidHeight;
	re.SetColor( g_color_table[ ColorIndex( COLOR_BLACK ) ] );
	re.DrawStretchPic(x, y - cl_graphheight->integer, 
		w, cl_graphheight->integer, 0, 0, 0, 0, cls.whiteShader );
	re.SetColor( NULL );

	for (a=0 ; a<w ; a++)
	{
		i = (ARRAY_LEN(values)+current-1-(a % ARRAY_LEN(values))) % ARRAY_LEN(values);
		v = values[i];
		v = v * cl_graphscale->integer + cl_graphshift->integer;
		
		if (v < 0)
			v += cl_graphheight->integer * (1+(int)(-v / cl_graphheight->integer));
		h = (int)v % cl_graphheight->integer;
		re.DrawStretchPic( x+w-1-a, y - h, 1, h, 0, 0, 0, 0, cls.whiteShader );
	}
}

//=============================================================================

// remapShader is restricted to UI/2D asset namespaces so themes can restyle the
// menus / HUD / crosshair / font without enabling texture-based cheats: wallhack
// vectors live under textures/ and models/, which are deliberately NOT listed here.
// "ui_" covers menu line/decoration shaders (e.g. ui_lines1/ui_lines2).
static const char *const scr_remapSafePrefixes[] = {
    "ui/", "ui_", "menu/", "hud/", "gfx/2d/" };

// Specific menu-only assets that fall outside the safe prefixes but are still
// cheat-safe (drawn only in menus). Exact-match allowlist — keep this tight; do
// NOT add anything that can be rendered during live gameplay.
static const char *const scr_remapSafeNames[] = {
    "models/misc/circle_1" }; // the main-menu arc/swoosh decoration

// Registry of theme shader remaps issued via the remapShader command, so
// themesave can write them back out (the renderer itself keeps no list we can
// read). Deduped by source name; a self-remap (reset) drops the entry.
#define MAX_THEME_REMAPS 64
typedef struct {
	char oldName[MAX_QPATH];
	char newName[MAX_QPATH];
	char timeOffset[16];
} themeRemap_t;
static themeRemap_t scr_themeRemaps[MAX_THEME_REMAPS];
static int scr_numThemeRemaps;

static void SCR_RecordThemeRemap( const char *o, const char *n, const char *t ) {
	const qboolean reset = ( Q_stricmp( o, n ) == 0 ); // remap-to-self clears it
	int i;

	for ( i = 0; i < scr_numThemeRemaps; i++ ) {
		if ( Q_stricmp( scr_themeRemaps[i].oldName, o ) != 0 )
			continue;
		if ( reset ) {
			scr_numThemeRemaps--;
			memmove( &scr_themeRemaps[i], &scr_themeRemaps[i + 1],
			         ( scr_numThemeRemaps - i ) * sizeof( scr_themeRemaps[0] ) );
		} else {
			Q_strncpyz( scr_themeRemaps[i].newName, n, sizeof( scr_themeRemaps[i].newName ) );
			Q_strncpyz( scr_themeRemaps[i].timeOffset, t, sizeof( scr_themeRemaps[i].timeOffset ) );
		}
		return;
	}

	if ( reset || scr_numThemeRemaps >= MAX_THEME_REMAPS )
		return;

	Q_strncpyz( scr_themeRemaps[scr_numThemeRemaps].oldName, o, sizeof( scr_themeRemaps[0].oldName ) );
	Q_strncpyz( scr_themeRemaps[scr_numThemeRemaps].newName, n, sizeof( scr_themeRemaps[0].newName ) );
	Q_strncpyz( scr_themeRemaps[scr_numThemeRemaps].timeOffset, t, sizeof( scr_themeRemaps[0].timeOffset ) );
	scr_numThemeRemaps++;
}

/*
==================
SCR_WriteThemeRemaps

Write the active theme remaps as remapShader lines (used by themesave).
Returns the number written.
==================
*/
int SCR_WriteThemeRemaps( fileHandle_t f ) {
	int i;

	for ( i = 0; i < scr_numThemeRemaps; i++ ) {
		FS_Printf( f, "remapShader \"%s\" \"%s\" \"%s\"" Q_NEWLINE,
		           scr_themeRemaps[i].oldName, scr_themeRemaps[i].newName,
		           scr_themeRemaps[i].timeOffset );
	}
	return scr_numThemeRemaps;
}

/*
==================
SCR_RemapList_f

remaplist — print the active shader remaps issued via remapShader (theme state),
to confirm what a theme applied and spot failed remaps.
==================
*/
static void SCR_RemapList_f( void ) {
	int i;

	if ( scr_numThemeRemaps == 0 ) {
		Com_Printf( "No active shader remaps.\n" );
		return;
	}

	Com_Printf( "Active shader remaps (%i):\n", scr_numThemeRemaps );
	for ( i = 0; i < scr_numThemeRemaps; i++ ) {
		Com_Printf( "  %s " S_COLOR_CYAN "->" S_COLOR_WHITE " %s%s\n",
		            scr_themeRemaps[i].oldName, scr_themeRemaps[i].newName,
		            ( scr_themeRemaps[i].timeOffset[0] && strcmp( scr_themeRemaps[i].timeOffset, "0" ) != 0 )
		                ? va( " (t=%s)", scr_themeRemaps[i].timeOffset )
		                : "" );
	}
}

/*
==================
SCR_RemapShader_f

remapShader <old> <new> [timeOffset] — replace a UI/2D shader/image with another,
letting themes restyle the menus and 2D HUD without touching the game VM. The old
name must be in a safe UI/2D namespace (see scr_remapSafePrefixes).
==================
*/
static void SCR_RemapShader_f( void ) {
	const char *oldShader, *newShader, *offset;
	int i;
	qboolean allowed = qfalse;

	if ( Cmd_Argc() < 3 ) {
		Com_Printf( "Usage: remapShader <old> <new> [timeOffset]\n" );
		return;
	}

	if ( !cls.rendererStarted ) {
		Com_Printf( S_COLOR_YELLOW "remapShader: renderer not started.\n" );
		return;
	}

	oldShader = Cmd_Argv( 1 );
	newShader = Cmd_Argv( 2 );
	offset = ( Cmd_Argc() > 3 ) ? Cmd_Argv( 3 ) : "0";

	// anti-cheat: only UI/2D assets (by prefix) or specific menu-only names may be remapped
	for ( i = 0; i < ARRAY_LEN( scr_remapSafePrefixes ); i++ ) {
		if ( !Q_stricmpn( oldShader, scr_remapSafePrefixes[i], strlen( scr_remapSafePrefixes[i] ) ) ) {
			allowed = qtrue;
			break;
		}
	}
	for ( i = 0; !allowed && i < ARRAY_LEN( scr_remapSafeNames ); i++ ) {
		if ( !Q_stricmp( oldShader, scr_remapSafeNames[i] ) )
			allowed = qtrue;
	}
	if ( !allowed ) {
		Com_Printf( S_COLOR_YELLOW "remapShader: '%s' is not a remappable UI/2D asset (blocked for anti-cheat).\n",
		            oldShader );
		return;
	}

	re.RemapShader( oldShader, newShader, offset );
	SCR_RecordThemeRemap( oldShader, newShader, offset );
}

/*
==================
SCR_Init
==================
*/
void SCR_Init( void ) {
	cl_timegraph = Cvar_Get ("timegraph", "0", CVAR_CHEAT);
	cl_debuggraph = Cvar_Get ("debuggraph", "0", CVAR_CHEAT);
	cl_graphheight = Cvar_Get ("graphheight", "32", CVAR_CHEAT);
	cl_graphscale = Cvar_Get ("graphscale", "1", CVAR_CHEAT);
	cl_graphshift = Cvar_Get ("graphshift", "0", CVAR_CHEAT);

	Cmd_AddCommand( "remapShader", SCR_RemapShader_f );
	Cmd_AddCommand( "remaplist", SCR_RemapList_f );

	scr_initialized = qtrue;
}


/*
==================
SCR_Done
==================
*/
void SCR_Done( void ) {
	scr_initialized = qfalse;
}


//=======================================================

/*
==================
SCR_DrawScreenField

This will be called twice if rendering in stereo mode
==================
*/
static void SCR_DrawScreenField( stereoFrame_t stereoFrame ) {
	qboolean uiFullscreen;

	re.BeginFrame( stereoFrame );

	uiFullscreen = (uivm && VM_Call( uivm, 0, UI_IS_FULLSCREEN ));

	// wide aspect ratio screens need to have the sides cleared
	// unless they are displaying game renderings
	if ( uiFullscreen || cls.state < CA_LOADING ) {
		if ( cls.glconfig.vidWidth * 480 > cls.glconfig.vidHeight * 640 ) {
			// draw vertical bars on sides for legacy mods
			const int w = (cls.glconfig.vidWidth - ((cls.glconfig.vidHeight * 640) / 480)) /2;
			re.SetColor( g_color_table[ ColorIndex( COLOR_BLACK ) ] );
			re.DrawStretchPic( 0, 0, w, cls.glconfig.vidHeight, 0, 0, 0, 0, cls.whiteShader );
			re.DrawStretchPic( cls.glconfig.vidWidth - w, 0, w, cls.glconfig.vidHeight, 0, 0, 0, 0, cls.whiteShader );
			re.SetColor( NULL );
		}
	}

	// if the menu is going to cover the entire screen, we
	// don't need to render anything under it
	if ( uivm && !uiFullscreen ) {
		switch( cls.state ) {
		default:
			Com_Error( ERR_FATAL, "SCR_DrawScreenField: bad cls.state" );
			break;
		case CA_CINEMATIC:
			SCR_DrawCinematic();
			break;
		case CA_DISCONNECTED:
			// force menu up
			S_StopAllSounds();
			VM_Call( uivm, 1, UI_SET_ACTIVE_MENU, UIMENU_MAIN );
			break;
		case CA_CONNECTING:
		case CA_CHALLENGING:
		case CA_CONNECTED:
			// connecting clients will only show the connection dialog
			// refresh to update the time
			VM_Call( uivm, 1, UI_REFRESH, cls.realtime );
			VM_Call( uivm, 1, UI_DRAW_CONNECT_SCREEN, qfalse );
			break;
		case CA_LOADING:
		case CA_PRIMED:
			// draw the game information screen and loading progress
			if ( cgvm ) {
				CL_CGameRendering( stereoFrame );
			}
			// also draw the connection information, so it doesn't
			// flash away too briefly on local or lan games
			// refresh to update the time
			VM_Call( uivm, 1, UI_REFRESH, cls.realtime );
			VM_Call( uivm, 1, UI_DRAW_CONNECT_SCREEN, qtrue );
			break;
		case CA_ACTIVE:
			// always supply STEREO_CENTER as vieworg offset is now done by the engine.
			CL_CGameRendering( stereoFrame );
			SCR_DrawDemoRecording();
#ifdef USE_VOIP
			SCR_DrawVoipMeter();
#endif
			break;
		}
	}

	// the menu draws next
	if ( Key_GetCatcher( ) & KEYCATCH_UI && uivm ) {
		VM_Call( uivm, 1, UI_REFRESH, cls.realtime );
	}

	// console draws next
	Con_DrawConsole ();

	// debug graph can be drawn on top of anything
	if ( cl_debuggraph->integer || cl_timegraph->integer || cl_debugMove->integer ) {
		SCR_DrawDebugGraph ();
	}
}


/*
==================
SCR_UpdateScreen

This is called every frame, and can also be called explicitly to flush
text to the screen.
==================
*/
void SCR_UpdateScreen( void ) {
	static int recursive;
	static int framecount;
	static int next_frametime;

	if ( !scr_initialized )
		return; // not initialized yet

	if ( framecount == cls.framecount ) {
		int ms = Sys_Milliseconds();
		if ( next_frametime && ms - next_frametime < 0 ) {
			re.ThrottleBackend();
		} else {
			next_frametime = ms + 16; // limit to 60 FPS
		}
	} else {
		next_frametime = 0;
		framecount = cls.framecount;
	}

	if ( ++recursive > 2 ) {
		Com_Error( ERR_FATAL, "SCR_UpdateScreen: recursively called" );
	}
	recursive = 1;

	// If there is no VM, there are also no rendering commands issued. Stop the renderer in
	// that case.
	if ( uivm )
	{
		// XXX
		int in_anaglyphMode = Cvar_VariableIntegerValue("r_anaglyphMode");
		// if running in stereo, we need to draw the frame twice
		if ( cls.glconfig.stereoEnabled || in_anaglyphMode) {
			SCR_DrawScreenField( STEREO_LEFT );
			SCR_DrawScreenField( STEREO_RIGHT );
		} else {
			SCR_DrawScreenField( STEREO_CENTER );
		}

		if ( com_speeds->integer ) {
			re.EndFrame( &time_frontend, &time_backend );
		} else {
			re.EndFrame( NULL, NULL );
		}
	}

	recursive = 0;
}
