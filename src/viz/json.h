/*
 * json.h
 *
 * Helper functions for outputting to JSON
 */

#ifndef JSON_H_
#define JSON_H_

#include "../core/StdHeaders.h"
#include <sys/time.h>					// timeval struct

#include <string>
using namespace std;

// JSON output for name / value pairings
//
//   Time = 125.6 --> "Time" : "125.6"

/// Wrap an arbitrary string in quotes
string 	wrapQuotes (string sVal);

string 	wrapQuote  (char * pszVal);

/// Output JSON for a string-based value
string	outputJSON (string sField, string sValue);

string 	outputJSON (string sField, double fValue);

string 	outputJSON (string sField, timeval * pVal);

#endif /* JSON_H_ */
