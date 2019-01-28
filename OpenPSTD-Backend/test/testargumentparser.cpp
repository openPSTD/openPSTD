#include <gtest/gtest.h>
#include "testcommand.h"
#include "../argumentparser.h"

/**
 * Verifies that the program exits with the correct error
 * message in case it is called without any arguments.
 */
TEST(ArgumentParser, NoArgs) {
	// Setup argc, argv to match "./OpenPSTD-cli"
	int argc = 1;
	char argv0[] = "./OpenPSTD-cli";
	char* argv[] = { argv0 };
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		ArgumentParser parser(argc, argv);
	}, "Usage: ./OpenPSTD-cli <command> \\[options\\]\n");
}

/**
 * Verifies that the program exits with the correct error
 * message in case the supplied command is invalid.
 */
TEST(ArgumentParser, InvalidCommand) {
	// Setup argc, argv to match "./OpenPSTD-cli one"
	int argc = 2;
	char argv0[] = "./OpenPSTD-cli";
	char argv1[] = "one";
	char* argv[] = { argv0, argv1 };
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		ArgumentParser parser(argc, argv);
	}, "\\[ArgumentParser\\] Invalid command \"one\"\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if an option is not prefixed with a dash.
 */
TEST(ArgumentParser, NoDashPrefix) {
	// Setup argc, argv to match "./OpenPSTD-cli create one"
	int argc = 3;
	char argv0[] = "./OpenPSTD-cli";
	char argv1[] = "create";
	char argv2[] = "one";
	char* argv[] = { argv0, argv1, argv2 };
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		ArgumentParser parser(argc, argv);
	}, "\\[ArgumentParser\\] Options must be prefixed with a dash \\(word 2\\)\n");
}

/**
 * Verifies that the parsed options are printed to stdout when
 * the debug flag is supplied.
 */
TEST(ArgumentParser, Debug) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Setup argc, argv to match "./OpenPSTD-cli create --debug -f <filename>"
	int argc = 5;
	char argv0[] = "./OpenPSTD-cli";
	char argv1[] = "create";
	char argv2[] = "--debug";
	char argv3[] = "-f";
	char* argv4 = &filename[0];
	char* argv[] = { argv0, argv1, argv2, argv3, argv4 };
	
	// Assert that the parsed commands are printed to stdout
	testing::internal::CaptureStdout();
	ArgumentParser parser(argc, argv);
	std::string output = testing::internal::GetCapturedStdout();
	
	// Assert that all parsed options are printed to stdout
	ASSERT_NE(output.find("ArgumentParser:"), std::string::npos);
	ASSERT_NE(output.find("\n  Command: "), std::string::npos);
	ASSERT_NE(output.find("\n  Option: \"--debug\" -> \"\""), std::string::npos);
	ASSERT_NE(output.find("\n  Option: \"-f\" -> "), std::string::npos);
}