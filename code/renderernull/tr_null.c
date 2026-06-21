/*
===========================================================================
Urban Terror Optimized — null renderer.

A do-nothing implementation of the refexport_t interface. It creates no window
and no GL/Vulkan context, so the client can boot and run fully headless (CI,
automated/agent-driven tests, demo parsing, server-side logic). Every entry
point is a no-op or returns a benign default; registration calls hand back a
non-zero handle so callers treat assets as "loaded".

Built as a dlopen renderer module (urbanterror-optimized_null_<arch>.so) and
selected with: +set cl_renderer null

This is engine-only, additive code (new directory) — it does not touch the
upstream-aligned renderer trees.
===========================================================================
*/

#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"
#include "../renderercommon/tr_public.h"

refimport_t ri;

// glconfig handed to the client; non-zero dimensions keep 2D scaling math sane.
static glconfig_t glConfig;

// The renderer .so is standalone under dlopen and links its own q_shared.o,
// whose Com_Error/Com_Printf must resolve here. Forward them to the imports.
void QDECL Com_Error( errorParm_t code, const char *fmt, ... ) {
	char buf[4096];
	va_list argptr;
	va_start( argptr, fmt );
	Q_vsnprintf( buf, sizeof( buf ), fmt, argptr );
	va_end( argptr );
	ri.Error( code, "%s", buf );
}

void QDECL Com_Printf( const char *fmt, ... ) {
	char buf[4096];
	va_list argptr;
	va_start( argptr, fmt );
	Q_vsnprintf( buf, sizeof( buf ), fmt, argptr );
	va_end( argptr );
	ri.Printf( PRINT_ALL, "%s", buf );
}

static void RE_Shutdown( refShutdownCode_t code ) {
}

static void RE_BeginRegistration( glconfig_t *config ) {
	Com_Memset( &glConfig, 0, sizeof( glConfig ) );

	glConfig.vidWidth = 640;
	glConfig.vidHeight = 480;
	glConfig.windowAspect = (float)glConfig.vidWidth / (float)glConfig.vidHeight;
	glConfig.displayFrequency = 60;
	glConfig.colorBits = 32;
	glConfig.depthBits = 24;
	glConfig.stencilBits = 8;
	glConfig.maxTextureSize = 2048;
	glConfig.numTextureUnits = 2;
	glConfig.deviceSupportsGamma = qfalse;
	glConfig.driverType = GLDRV_ICD;
	glConfig.hardwareType = GLHW_GENERIC;
	glConfig.textureCompression = TC_NONE;
	glConfig.textureEnvAddAvailable = qtrue;
	glConfig.isFullscreen = qfalse;
	glConfig.stereoEnabled = qfalse;
	glConfig.smpActive = qfalse;
	Q_strncpyz( glConfig.renderer_string, "null", sizeof( glConfig.renderer_string ) );
	Q_strncpyz( glConfig.vendor_string, "urbanterror-optimized", sizeof( glConfig.vendor_string ) );
	Q_strncpyz( glConfig.version_string, "headless", sizeof( glConfig.version_string ) );

	ri.Printf( PRINT_ALL, "----- null renderer (headless) -----\n" );

	if ( config ) {
		*config = glConfig;
	}
}

// Registration: return a non-zero handle so callers treat the asset as present.
static qhandle_t RE_RegisterModel( const char *name ) {
	return 1;
}
static qhandle_t RE_RegisterSkin( const char *name ) {
	return 1;
}
static qhandle_t RE_RegisterShader( const char *name ) {
	return 1;
}
static qhandle_t RE_RegisterShaderNoMip( const char *name ) {
	return 1;
}

static void RE_LoadWorldMap( const char *name ) {
}
static void RE_SetWorldVisData( const byte *vis ) {
}
static void RE_EndRegistration( void ) {
}

static void RE_ClearScene( void ) {
}
static void RE_AddRefEntityToScene( const refEntity_t *re, qboolean intShaderTime ) {
}
static void RE_AddPolyToScene( qhandle_t hShader, int numVerts, const polyVert_t *verts, int num ) {
}
static int R_LightForPoint( vec3_t point, vec3_t ambientLight, vec3_t directedLight, vec3_t lightDir ) {
	return 0;
}
static void RE_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b ) {
}
static void RE_AddAdditiveLightToScene( const vec3_t org, float intensity, float r, float g, float b ) {
}
static void RE_AddLinearLightToScene( const vec3_t start, const vec3_t end, float intensity, float r, float g, float b ) {
}
static void RE_RenderScene( const refdef_t *fd ) {
}

static void RE_SetColor( const float *rgba ) {
}
static void RE_StretchPic( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader ) {
}
static void RE_StretchRaw( int x, int y, int w, int h, int cols, int rows, byte *data, int client, qboolean dirty ) {
}
static void RE_UploadCinematic( int w, int h, int cols, int rows, byte *data, int client, qboolean dirty ) {
}

static void RE_BeginFrame( stereoFrame_t stereoFrame ) {
}
static void RE_EndFrame( int *frontEndMsec, int *backEndMsec ) {
	if ( frontEndMsec )
		*frontEndMsec = 0;
	if ( backEndMsec )
		*backEndMsec = 0;
}

static int R_MarkFragments( int numPoints, const vec3_t *points, const vec3_t projection,
                            int maxPoints, vec3_t pointBuffer, int maxFragments, markFragment_t *fragmentBuffer ) {
	return 0;
}

static int R_LerpTag( orientation_t *tag, qhandle_t model, int startFrame, int endFrame,
                      float frac, const char *tagName ) {
	if ( tag ) {
		AxisClear( tag->axis );
		VectorClear( tag->origin );
	}
	return qfalse;
}

static void R_ModelBounds( qhandle_t model, vec3_t mins, vec3_t maxs ) {
	if ( mins )
		VectorClear( mins );
	if ( maxs )
		VectorClear( maxs );
}

static void RE_RegisterFont( const char *fontName, int pointSize, fontInfo_t *font ) {
	if ( font ) {
		Com_Memset( font, 0, sizeof( *font ) );
	}
}

static void RE_RemapShader( const char *oldShader, const char *newShader, const char *offsetTime ) {
}
static qboolean RE_GetEntityToken( char *buffer, int size ) {
	if ( buffer && size > 0 )
		buffer[0] = '\0';
	return qfalse;
}
static qboolean R_inPVS( const vec3_t p1, const vec3_t p2 ) {
	return qtrue;
}

static void RE_TakeVideoFrame( int h, int w, byte *captureBuffer, byte *encodeBuffer, qboolean motionJpeg ) {
}
static void R_SetColorMappings( void ) {
}
static void RE_ThrottleBackend( void ) {
}
static void RE_FinishBloom( void ) {
}
static void RE_BlurConsoleBackground( float frac ) {
}
static qboolean RE_CanMinimize( void ) {
	return qfalse;
}
static const glconfig_t *RE_GetConfig( void ) {
	return &glConfig;
}
static void RE_VertexLighting( qboolean allowed ) {
}
static void RE_SyncRender( void ) {
}

/*
============
GetRefAPI
============
*/
Q_EXPORT refexport_t *QDECL GetRefAPI( int apiVersion, refimport_t *rimp ) {
	static refexport_t re;

	ri = *rimp;

	Com_Memset( &re, 0, sizeof( re ) );

	if ( apiVersion != REF_API_VERSION ) {
		ri.Printf( PRINT_ALL, "Mismatched REF_API_VERSION: expected %i, got %i\n",
		           REF_API_VERSION, apiVersion );
		return NULL;
	}

	re.Shutdown = RE_Shutdown;

	re.BeginRegistration = RE_BeginRegistration;
	re.RegisterModel = RE_RegisterModel;
	re.RegisterSkin = RE_RegisterSkin;
	re.RegisterShader = RE_RegisterShader;
	re.RegisterShaderNoMip = RE_RegisterShaderNoMip;
	re.LoadWorld = RE_LoadWorldMap;
	re.SetWorldVisData = RE_SetWorldVisData;
	re.EndRegistration = RE_EndRegistration;

	re.BeginFrame = RE_BeginFrame;
	re.EndFrame = RE_EndFrame;

	re.MarkFragments = R_MarkFragments;
	re.LerpTag = R_LerpTag;
	re.ModelBounds = R_ModelBounds;

	re.ClearScene = RE_ClearScene;
	re.AddRefEntityToScene = RE_AddRefEntityToScene;
	re.AddPolyToScene = RE_AddPolyToScene;
	re.LightForPoint = R_LightForPoint;
	re.AddLightToScene = RE_AddLightToScene;
	re.AddAdditiveLightToScene = RE_AddAdditiveLightToScene;
	re.AddLinearLightToScene = RE_AddLinearLightToScene;

	re.RenderScene = RE_RenderScene;

	re.SetColor = RE_SetColor;
	re.DrawStretchPic = RE_StretchPic;
	re.DrawStretchRaw = RE_StretchRaw;
	re.UploadCinematic = RE_UploadCinematic;

	re.RegisterFont = RE_RegisterFont;
	re.RemapShader = RE_RemapShader;
	re.GetEntityToken = RE_GetEntityToken;
	re.inPVS = R_inPVS;

	re.TakeVideoFrame = RE_TakeVideoFrame;
	re.SetColorMappings = R_SetColorMappings;

	re.ThrottleBackend = RE_ThrottleBackend;
	re.FinishBloom = RE_FinishBloom;
	re.BlurConsoleBackground = RE_BlurConsoleBackground;
	re.CanMinimize = RE_CanMinimize;
	re.GetConfig = RE_GetConfig;
	re.VertexLighting = RE_VertexLighting;
	re.SyncRender = RE_SyncRender;

	return &re;
}
