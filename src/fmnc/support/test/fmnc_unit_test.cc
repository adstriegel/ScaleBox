/*
 * fmnc_unit_test.cc
 *
 * Unit testing via the Catch testing suite for the core FMNC operations
 */

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "../../fmnc_test_sequence.h"

TEST_CASE( "FMNC_Transmit_BufferBlock", "[FMNC_Transmit_BufferBlock]" ) {

	FMNC_Transmit_BufferBlock	buffer;

	// Check the initial conditions
	REQUIRE( buffer.getLength() == 0);
	REQUIRE( buffer.getData() == NULL);

	buffer.allocateBlock(3000);

	REQUIRE( buffer.getLength() == 3000);
	REQUIRE( buffer.getData() != NULL);
}


