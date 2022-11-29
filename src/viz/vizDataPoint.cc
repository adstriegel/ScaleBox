/*
 * vizDataPoint.cc
 *
 *  Created on: Jul 13, 2014
 *      Author: striegel
 */

#include <iostream>
using namespace std;

#include "vizDataPoint.h"
#include "json.h"

VizDataPoint2D::VizDataPoint2D ()
{
	m_fX = 0.0;
	m_fY = 0.0;
}

VizDataPoint2D::VizDataPoint2D (double fX, double fY)
{
	m_fX = fX;
	m_fY = fY;
}

VizDataPoint2D::~VizDataPoint2D()
{

}

string 	VizDataPoint2D::extractJSON()
{
	string 	sJSON;

	sJSON = "{";
	sJSON += outputJSON("x",m_fX);
	sJSON += ",";
	sJSON += outputJSON("y",m_fY);
	sJSON += "}";

	return sJSON;
}

double 	VizDataPoint2D::getX ()
{
	return m_fX;
}

void	VizDataPoint2D::setX (double fX)
{
	m_fX = fX;
}

double  VizDataPoint2D::getY ()
{
	return m_fY;
}

void	VizDataPoint2D::setY (double fY)
{
	m_fY = fY;
}


