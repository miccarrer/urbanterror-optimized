/*
 * libFuzzer target for the userinfo parsers in q_shared.c — a direct network
 * attack surface (clients send userinfo strings the server parses).
 *
 *   make -C tests fuzz CC=clang
 *   ./tests/build/fuzz_info -runs=200000 tests/fuzz/corpus/info
 */
#include "q_shared.h"

#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

extern jmp_buf com_error_jmp;
extern int com_error_ready;

int LLVMFuzzerTestOneInput( const uint8_t *data, size_t size ) {
	char info[MAX_INFO_STRING];
	size_t n = size < sizeof( info ) - 1 ? size : sizeof( info ) - 1;

	memcpy( info, data, n );
	info[n] = '\0';

	com_error_ready = 1;
	if ( setjmp( com_error_jmp ) == 0 ) {
		if ( Info_Validate( info ) ) {
			(void)Info_ValueForKey( info, "name" );
			(void)Info_ValueForKey( info, "cl_guid" );

			char key[MAX_INFO_STRING];
			char value[MAX_INFO_STRING];
			const char *p = info;
			int guard = 0;

			for ( ;; ) {
				p = Info_NextPair( p, key, value );
				if ( key[0] == '\0' || ++guard > 512 ) {
					break;
				}
			}

			/* destructive parser, operates in place on the writable copy */
			Info_RemoveKey( info, "name" );
		}
	}
	com_error_ready = 0;
	return 0;
}
