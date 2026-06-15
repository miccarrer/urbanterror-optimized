/*
 * Minimal engine stubs so dependency-light qcommon TUs (e.g. q_shared.c) link
 * standalone in the test/fuzz harness. Only Com_Error and Com_Printf are truly
 * external to q_shared.c.
 *
 * Com_Error is NORETURN in the engine. In tests we treat it as "input rejected":
 * a harness that has armed com_error_ready longjmps back instead of aborting, so
 * a legitimately-refused input is not reported as a fuzzer crash.
 */
#include "q_shared.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>

jmp_buf com_error_jmp;
int com_error_ready = 0;

void QDECL Com_Error( errorParm_t level, const char *fmt, ... ) {
	(void)level;
	(void)fmt;
	if ( com_error_ready ) {
		longjmp( com_error_jmp, 1 );
	}
	abort();
}

void QDECL Com_Printf( const char *fmt, ... ) {
	(void)fmt;
}
