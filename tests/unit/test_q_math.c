/*
 * Unit tests for code/qcommon/q_math.c — pure, dependency-free math helpers.
 * q_math.c includes only q_shared.h and calls no Com_/Sys_ functions, so it
 * links standalone against Unity with no engine stubs.
 */
#include "unity.h"
#include "q_shared.h"

void setUp( void ) {}
void tearDown( void ) {}

static void test_Q_rsqrt_approximates_inverse_sqrt( void ) {
	TEST_ASSERT_FLOAT_WITHIN( 0.005f, 0.5f, Q_rsqrt( 4.0f ) );
	TEST_ASSERT_FLOAT_WITHIN( 0.005f, 0.25f, Q_rsqrt( 16.0f ) );
	TEST_ASSERT_FLOAT_WITHIN( 0.002f, 1.0f, Q_rsqrt( 1.0f ) );
}

static void test_VectorNormalize_returns_length_and_unit_vector( void ) {
	vec3_t v = { 3.0f, 0.0f, 4.0f };
	vec_t len = VectorNormalize( v );

	TEST_ASSERT_FLOAT_WITHIN( 1e-4f, 5.0f, len );
	TEST_ASSERT_FLOAT_WITHIN( 1e-4f, 1.0f, DotProduct( v, v ) );
	TEST_ASSERT_FLOAT_WITHIN( 1e-4f, 0.6f, v[0] );
	TEST_ASSERT_FLOAT_WITHIN( 1e-4f, 0.8f, v[2] );
}

static void test_VectorNormalize_zero_vector_is_safe( void ) {
	vec3_t v = { 0.0f, 0.0f, 0.0f };
	vec_t len = VectorNormalize( v );

	TEST_ASSERT_EQUAL_FLOAT( 0.0f, len );
	TEST_ASSERT_EQUAL_FLOAT( 0.0f, v[0] );
	TEST_ASSERT_EQUAL_FLOAT( 0.0f, v[1] );
	TEST_ASSERT_EQUAL_FLOAT( 0.0f, v[2] );
}

static void test_ClearBounds_then_AddPoint_collapses_to_point( void ) {
	vec3_t mins, maxs;
	vec3_t p = { 1.0f, -2.0f, 3.0f };

	ClearBounds( mins, maxs );
	AddPointToBounds( p, mins, maxs );

	TEST_ASSERT_EQUAL_FLOAT( 1.0f, mins[0] );
	TEST_ASSERT_EQUAL_FLOAT( -2.0f, mins[1] );
	TEST_ASSERT_EQUAL_FLOAT( 3.0f, maxs[2] );
	TEST_ASSERT_EQUAL_FLOAT( mins[0], maxs[0] );
}

static void test_RadiusFromBounds_unit_cube( void ) {
	vec3_t mins = { -1.0f, -1.0f, -1.0f };
	vec3_t maxs = { 1.0f, 1.0f, 1.0f };

	/* furthest corner from centre is at distance sqrt(3). */
	TEST_ASSERT_FLOAT_WITHIN( 1e-4f, 1.7320508f, RadiusFromBounds( mins, maxs ) );
}

static void test_AngleSubtract_wraps_into_signed_180( void ) {
	TEST_ASSERT_FLOAT_WITHIN( 1e-3f, 20.0f, AngleSubtract( 10.0f, 350.0f ) );
	TEST_ASSERT_FLOAT_WITHIN( 1e-3f, -20.0f, AngleSubtract( 350.0f, 10.0f ) );
	TEST_ASSERT_FLOAT_WITHIN( 1e-3f, 0.0f, AngleSubtract( 90.0f, 90.0f ) );
}

static void test_LerpAngle_midpoint( void ) {
	TEST_ASSERT_FLOAT_WITHIN( 1e-3f, 45.0f, LerpAngle( 0.0f, 90.0f, 0.5f ) );
	/* shortest path across the 0/360 wrap: 10->350 goes backwards through 0. */
	TEST_ASSERT_FLOAT_WITHIN( 1e-3f, 0.0f, LerpAngle( 10.0f, 350.0f, 0.5f ) );
}

static void test_Q_atof_parses_and_defaults_to_zero( void ) {
	TEST_ASSERT_FLOAT_WITHIN( 1e-4f, 3.14f, Q_atof( "3.14" ) );
	TEST_ASSERT_FLOAT_WITHIN( 1e-4f, -42.0f, Q_atof( "-42" ) );
	TEST_ASSERT_EQUAL_FLOAT( 0.0f, Q_atof( "not-a-number" ) );
}

int main( void ) {
	UNITY_BEGIN();
	RUN_TEST( test_Q_rsqrt_approximates_inverse_sqrt );
	RUN_TEST( test_VectorNormalize_returns_length_and_unit_vector );
	RUN_TEST( test_VectorNormalize_zero_vector_is_safe );
	RUN_TEST( test_ClearBounds_then_AddPoint_collapses_to_point );
	RUN_TEST( test_RadiusFromBounds_unit_cube );
	RUN_TEST( test_AngleSubtract_wraps_into_signed_180 );
	RUN_TEST( test_LerpAngle_midpoint );
	RUN_TEST( test_Q_atof_parses_and_defaults_to_zero );
	return UNITY_END();
}
