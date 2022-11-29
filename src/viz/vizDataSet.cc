/*
 * vizDataSet.cc
 *
 *  Created on: Jul 13, 2014
 *      Author: striegel
 */

#include <iostream>
using namespace std;

#include <fstream>
using namespace std;

#include "vizDataSet.h"
#include "json.h"

VizDataSet::VizDataSet ()
{

}

VizDataSet::~VizDataSet ()
{
	int		j;

	for(j=0; j<m_DataPoints.size(); j++)
	{
		delete m_DataPoints[j];

	}

	m_DataPoints.clear();
}

void VizDataSet::addDataPoint (VizDataPoint * pPoint)
{
	m_DataPoints.push_back(pPoint);
}

int	 VizDataSet::getCount ()
{
	return m_DataPoints.size();
}

void VizDataSet::clear 	  ()
{
	for(int j=0; j<m_DataPoints.size(); j++)
	{
		delete m_DataPoints[j];
		m_DataPoints[j] = NULL;
	}

	m_DataPoints.clear();
}

string  VizDataSet::extractScript_Var (string sVarName)
{
	string 	sScript;

	sScript = "var " + sVarName + " = ";
	sScript += extractJSON();
	sScript += ";";

	return sScript;
}


string 	VizDataSet::extractJSON()
{
	int		j;
	string 	sJSON;

	sJSON = "[";

	for(j=0; j<m_DataPoints.size(); j++)
	{
		sJSON += m_DataPoints[j]->extractJSON();

		if(j<m_DataPoints.size()-1)
		{
			sJSON += ",";
		}
	}

	sJSON += "]";
	return sJSON;
}


// Should give a "field": [ { ... }, {....},

string 	VizDataSet::extractJSON(string sName)
{
	int		j;
	string 	sJSON;

	sJSON = wrapQuotes(sName);
	sJSON += ":";
	sJSON += extractJSON();
	return sJSON;
}


VizDataPoint *	VizDataSet::get (int nIndex)
{
	if(nIndex < 0 || nIndex >= m_DataPoints.size())
	{
		cerr << "* Error: Out of bounds index request (index = " << nIndex << "), bound of " << m_DataPoints.size();
		cerr << endl;
		return NULL;
	}

	return m_DataPoints[nIndex];
}

