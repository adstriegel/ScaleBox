//============================================================================
// Name        : ScaleBoxViz.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include <fstream>
using namespace std;

#include "../d3js.h"
#include "../vizDataPoint.h"
#include "../vizDataSet.h"

void	executeTest_1 ();


int main() {

	executeTest_1();
	return 0;
}


void executeTest_1 ()
{
	VizDataSet	 *	pDataset;
	VizDataPoint2D *	pDataPoint;
	GraphD3JSLine	theGraph;

	pDataset = new VizDataSet();

	for(int j=0; j<50; j++)
	{
		pDataPoint = new VizDataPoint2D((double) j, (double) (j*10));
		pDataset->addDataPoint(pDataPoint);
	}

	theGraph.addDataset(pDataset);

	cout << pDataset->extractJSON() << endl;

//	cout << theGraph.extractDataPoints() << endl;

	streampos size;
	char * memblock;
	string	sTemp;

	ifstream fileInStart ("index-start.html", ios::in|ios::binary|ios::ate);

	if (fileInStart.is_open())
	{
		size = fileInStart.tellg();
		memblock = new char [(int) size + 1];
		fileInStart.seekg (0, ios::beg);
		fileInStart.read (memblock, size);
		fileInStart.close();
		memblock[size]= '\0';

		sTemp = memblock;
		delete[] memblock;
	}
	else
	{
	}


	ifstream fileInStop ("index-end.html", ios::in|ios::binary|ios::ate);

	sTemp += pDataset->extractJSON();

	if (fileInStop.is_open())
	{
		size = fileInStop.tellg();
		memblock = new char [(int)size + 1];
		fileInStop.seekg (0, ios::beg);
		fileInStop.read (memblock, size);
		fileInStop.close();
		memblock[size]= '\0';

		sTemp += memblock;
		delete[] memblock;
	}

	ofstream fileOut ("test.html");

	fileOut << sTemp;
}

