/* Helpers.h
 * 
 * Defines common helpers inside a namespace.
 * These, therefore, are global functions. */

#ifndef HELPERS_H
#define HELPERS_H

#include <sstream>
#include <string>

namespace Helpers
{
	// Returns the string format of an integer.
	// 
	// Parameters:
	// [int] input: the integer to be converted
	std::string toString(int input)
	{
		// Initialize a string stream.
		std::ostringstream sstream;
		std::string string;

		// Feed the integer into the stream.
		sstream << input;

		// Retrieve the integer in string format.
		string = sstream.str();

		return string;
	}
	
	// Returns the string format of a float.
	// 
	// Parameters:
	// [float] input: the integer to be converted
	std::string toString(float input)
	{
		// Initialize a string stream.
		std::ostringstream sstream;
		sstream.precision(6);
		std::string string;

		// Feed the integer into the stream.
		sstream << input;

		// Retrieve the integer in string format.
		string = sstream.str();

		return string;
	}

	// Returns the string format of a double.
	// 
	// Parameters:
	// [double] input: the integer to be converted
	std::string toString(double input)
	{
		// Initialize a string stream.
		std::ostringstream sstream;
		sstream.precision(17);
		std::string string;

		// Feed the integer into the stream.
		sstream << input;

		// Retrieve the integer in string format.
		string = sstream.str();

		return string;
	}
}

#endif // HELPERS_H