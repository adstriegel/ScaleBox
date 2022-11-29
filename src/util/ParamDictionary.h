/*
 * ParamReplace.h
 *
 *  Created on: Aug 1, 2014
 *      Author: striegel
 */

#ifndef PARAMREPLACE_H_
#define PARAMREPLACE_H_

#include <string>
using namespace std;

#include <vector>
using namespace std;

class ParamDictionary;

#define PARAM_DICTIONARY_MAXLENGTH_NAME				30
#define PARAM_DICTIONARY_DEFAULT_START_DELIM		"$#"
#define PARAM_DICTIONARY_DEFAULT_END_DELIM			"#$"

/** Parameterized replace identifying a variable name and replacing it with the specified
 * value
 */
class ParamDictionaryEntry
{
	public:
		ParamDictionaryEntry ();
		ParamDictionaryEntry (string sName, string sValue);

		string		getName ();
		void		setName (std::string sName);

		string		getValue ();
		void		setValue (string sValue);

		ParamDictionary * getOwner ();
		void setOwner (ParamDictionary * pOwner);

	private:
		string m_sName;
		string m_sValue;
		ParamDictionary * m_pOwner;
};

class ParamDictionary
{
	public:
		ParamDictionary ();
		~ParamDictionary ();

		/** Fuse the content from the other dictionary with the contents of this dictionary
		 * @param pDictionary The dictionary on which to fuse
		 * @param bOverwrite Denotes if overlapping entries should be overwritten
		 * @param bRefOnly Make references only but leave ownership elsewhere (dangerous)
		 * @returns Number of entries that were added
		 */
		int		fuseEntries (ParamDictionary * pDictionary, bool bOverwrite, bool bRefOnly);

		/** Add an entry to the list.  Note that this function will not update
		 * an existing entry, only add a new entry.
		 * @param sName The name for the parameter
		 * @param sValue The value that will be used to replace the parameter
		 * @returns True if it could be added, false if it already existed
		 */
		bool	addEntry (string sName, string sValue);

		/** Update a particular entry with a given name to the specified value.
		 * Note that this function will not create a new entry, only update.
		 * @param sName The name of the entry to update
		 * @param sNewValue The new value for the entry
		 * @returns True if successful, false otherwise
		 */
		bool 	updateEntry (string sName, string sNewValue);

		/** Set the entry associated with this name to the specified value.  If
		 * the entry does not exist, create it.  If the entry does exist, update
		 * it to reflect the new value.
		 * @param sName The parameter name
		 * @param sValue The value for the parameter
		 * @returns True if successful, false otherwise
		 */
		bool	setEntry (string sName, string sValue);

		/** Retrieve a count of how many entries are in the dictionary
		 * @returns Count of the number of entries in this dictionary
		 */
		int		getCount ();


		/** Replaces the given text with the values for any parameterized names.
		 * @param sString The string to conduct the replacement on
		 * @returns The resulting string
		 */
		string	replaceText (string sString);


		/** Get the maximum length for a paramter name.  Anything with a name in excess
		 * of the length will be ignored
		 */
		int		getMaxLength_Name ();

		/** Set the maximum length for a paramter name. Note that there are no restrictions
		 * on paramter values
		 * @param nLength The new maximum length
		 * @returns True if successful, false otherwise
		 */
		bool	setMaxLength_Name (int nLength);

		/** Get the delimiter that denotes the start of a parameter. An appropriate
		 * parameter should be sufficiently unique so as not to be accidentally
		 * found in the text.
		 * @returns The start delimiter
		 */
		string	getDelimiter_Start ();

		/** Set the delimiter that denotes the start of a parameter.  An appropriate
		 * parameter should be sufficiently unique so as not to be accidentally found in
		 * in the next.
		 * @param sDelim The new start delimiter
		 * @returns True if successful, false otherwise
		 */
		bool	setDelimiter_Start (string sDelim);

		/** Get the delimiter that denotes the end of a parameter. An appropriate
		 * parameter should be sufficiently unique so as not to be accidentally
		 * found in the text.
		 * @returns The end delimeter
		 */
		string	getDelimiter_End ();

		/** Set the delimiter that denotes the end of a parameter.  An appropriate
		 * parameter should be sufficiently unique so as not to be accidentally found in
		 * in the next.
		 * @param sDelim The new end delimiter
		 * @returns True if successful, false otherwise
		 */
		bool	setDelimiter_End (string sDelim);

		/** Analyze a prospective parameter name for validity
		 * @return True if valid, false otherwise
		 */
		bool	isValidParameterName (string sName);

		/** Dump the dictionary content to the console
		 *
		 */
		void	dumpConsole ();

	protected:

		/** Retrieve the dictionary entry associated with this parameter name
		 * @param sName The name of the parameter to search for
		 * @returns Non-NULL if the entry exists, NULL otherwise
		 */
		ParamDictionaryEntry *	getEntry (string sName);

		ParamDictionaryEntry * 	getEntry (int nIndex);

	private:

		// The list of all entries. Note that this may eventually be converted to a
		// hash table
		vector<ParamDictionaryEntry *>	m_Entries;

		// The maximum length for any parameter name
		int			m_nMaxLength_Name;

		// The start delimiter that denotes the beginning of a parameter
		string		m_sDelimiter_Start;

		// The end delimiter that denotes the end of a parameter
		string		m_sDelimiter_End;
};


#endif /* PARAMREPLACE_H_ */
