/*
 * json.cc
 *
 * Helper operations for JSON interactions
 */

#include <iostream>

#include "json.h"

string 	wrapQuote  (char * pszVal)
{
	string 	sResult;

	sResult = "\"";
	sResult += pszVal;
	sResult += "\"";
	return sResult;
}

string  wrapQuotes (string sVal)
{
	string 	sResult;

	sResult = "\"" + sVal + "\"";
	return sResult;
}

string	outputJSON (string sField, string sValue)
{
	string 	sJSON;

	sJSON = wrapQuotes(sField) + ":";
	sJSON += wrapQuotes(sValue);

	return sJSON;
}

string 	outputJSON (string sField, double fValue)
{
	char 	szTemp[50];
	string 	sJSON;

	sprintf(szTemp, "%f", fValue);

//	sJSON = wrapQuotes(sField) + ":";
	sJSON = sField + " : ";
	sJSON += szTemp;
//	sJSON = wrapQuotes(szTemp);

	return sJSON;
}

string 	outputJSON (string sField, timeval * pVal)
{
	char 	szTemp[50];
	string 	sJSON;

	if(pVal == NULL)
	{
		cerr << "* Error: Invalid pointer for timeval JSON conversion, pointer was NULL" << endl;
		return "";
	}

	sprintf(szTemp, "%d.%d", pVal->tv_sec, pVal->tv_usec);

	sJSON = wrapQuotes(sField) + ":";
	sJSON = wrapQuotes(szTemp);

	return sJSON;
}



