/*
 * vizDataSet.h
 *
 *  Created on: Jul 13, 2014
 *      Author: striegel
 */

#ifndef VIZDATASET_H_
#define VIZDATASET_H_

#include "vizDataPoint.h"

#include <vector>
using namespace std;

/** A group of data points that typically represent an individual test and / or run,
 * usually recorded across time
 */
class VizDataSet
{
	public:
		VizDataSet ();
		~VizDataSet ();

		void addDataPoint (VizDataPoint * pPoint);

		int	 getCount ();

		void clear 	  ();

		VizDataPoint *	get (int nIndex);

		/// Extract a JSON representation of this object
		/// @param sField The initial label for the dataset

		// Example output:
		//    Should give a "field": [ { ... }, {....},
		string 	extractJSON(string sField);
		string 	extractJSON();

		string  extractScript_Var (string sVarName);

	private:
		vector<VizDataPoint *>	m_DataPoints;
};

#endif /* VIZDATATSET_H_ */
