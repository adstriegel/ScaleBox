/*
 * d3js.cc
 *
 *  Created on: Jul 13, 2014
 *      Author: striegel
 */

#include <iostream>
using namespace std;

#include <fstream>
using namespace std;

#include "d3js.h"



GraphD3JSLine::GraphD3JSLine ()
{
	m_nDisplay_Width = DEFAULT_D3JS_DISPLAY_WIDTH;
	m_nDisplay_Height = DEFAULT_D3JS_DISPLAY_HEIGHT;

	m_nMargin_Top = DEFAULT_D3JS_MARGIN_TOP;
	m_nMargin_Bottom = DEFAULT_D3JS_MARGIN_BOTTOM;
	m_nMargin_Left = DEFAULT_D3JS_MARGIN_LEFT;
	m_nMargin_Right = DEFAULT_D3JS_MARGIN_RIGHT;

	m_sVizArea = "";
}


GraphD3JSLine::~GraphD3JSLine ()
{
}

string 	GraphD3JSLine::extractScript ()
{
	string		sScript;

	sScript = "<script src=\"http://d3js.org/d3.v3.js\"></script>\n";
	sScript += "<script>\n";

	// Add the data

	sScript += getDataset(0)->extractScript_Var("lineData");
	sScript += "\n";

	// Core script
	sScript += "var vis = d3.select('#" + m_sVizArea + "'),\n";

	sScript += getSetting_DisplayWidth();
	sScript += getSetting_DisplayHeight();
	sScript += getSetting_Margins();


    sScript += "xRange = d3.scale.linear().range([MARGINS.left, WIDTH - MARGINS.right]).domain([d3.min(lineData, function(d) {\n";
    sScript += "  return d.x;\n";
    sScript += " }), d3.max(lineData, function(d) {\n";
    sScript += "  return d.x;\n";
    sScript += "})]),\n";
    sScript += "yRange = d3.scale.linear().range([HEIGHT - MARGINS.top, MARGINS.bottom]).domain([d3.min(lineData, function(d) {\n";
    sScript += "  return d.y;\n";
    sScript += "}), d3.max(lineData, function(d) {\n";
    sScript += "  return d.y;\n";
    sScript += "})]),\n";
    sScript += "xAxis = d3.svg.axis()\n";
    sScript += "  .scale(xRange)\n";
    sScript += "  .tickSize(5)\n";
    sScript += "  .tickSubdivide(true),\n";
    sScript += "yAxis = d3.svg.axis()\n";
    sScript += "  .scale(yRange)\n";
    sScript += "  .tickSize(1)\n";
    sScript += "  .orient('left')\n";
    sScript += "  .tickSubdivide(true);\n";


    sScript += "vis.append('svg:g')\n";
    sScript += "  .attr('class', 'x axis')\n";
    sScript += "  .attr('transform', 'translate(0,' + (HEIGHT - MARGINS.bottom) + ')')\n";
    sScript += "  .call(xAxis);\n";
    sScript += "  \n";
    sScript += "vis.append('svg:g')\n";
    sScript += "  .attr('class', 'y axis')\n";
    sScript += "  .attr('transform', 'translate(' + (MARGINS.left) + ',0)')\n";
    sScript += "  .call(yAxis);\n";
    sScript += "  \n";
    sScript += "  var lineFunc = d3.svg.line()\n";
    sScript += "  .x(function(d) {\n";
    sScript += "    return xRange(d.x);\n";
    sScript += "  })\n";
    sScript += "  .y(function(d) {\n";
    sScript += "    return yRange(d.y);\n";
    sScript += "  })\n";
    sScript += "  .interpolate('linear');\n";
    sScript += "  \n";
    sScript += "  \n";
    sScript += "vis.append('svg:path')\n";
    sScript += "  .attr('d', lineFunc(lineData))\n";
    sScript += "  .attr('stroke', 'blue')\n";
    sScript += "  .attr('stroke-width', 2)\n";
    sScript += "  .attr('fill', 'none');\n";

	sScript += "</script>";
	return sScript;
}



int		GraphD3JSLine::getCountDatasets ()
{
	return m_Datasets.size();
}

VizDataSet *	GraphD3JSLine::getDataset (int nIndex)
{
	if(nIndex < 0 || nIndex >= m_Datasets.size() )
	{
		cerr << "* Error: Out of bounds index request (index = " << nIndex << "), bound of " << m_Datasets.size() << endl;
		return NULL;
	}

	return m_Datasets[nIndex];
}

void		GraphD3JSLine::clear (bool bDelete)
{
	if(bDelete)
	{
		int		j;

		for(j=0; j<m_Datasets.size(); j++)
		{
			delete m_Datasets[j];
			m_Datasets[j] = NULL;
		}
	}

	m_Datasets.clear();
}

void		GraphD3JSLine::addDataset (VizDataSet * pDataset)
{
	m_Datasets.push_back(pDataset);
}

void		GraphD3JSLine::setAxisX_Limits (double fMin, double fMax)
{
	m_fAxisX_Min = fMin;
	m_fAxisX_Max = fMax;
}

void		GraphD3JSLine::setAxisY_Limits (double fMin, double fMax)
{
	m_fAxisY_Min = fMin;
	m_fAxisY_Max = fMax;
}

string		GraphD3JSLine::extractDataPoints ()
{
	return "";
}

uint16_t	GraphD3JSLine::getDisplay_Width ()
{
	return m_nDisplay_Width;
}

void		GraphD3JSLine::setDisplay_Width (uint16_t nWidth)
{
	m_nDisplay_Width = nWidth;
}

uint16_t	GraphD3JSLine::getDisplay_Height ()
{
	return m_nDisplay_Height;
}

void	GraphD3JSLine::setDisplay_Height (uint16_t nHeight)
{
	m_nDisplay_Height = nHeight;
}

void		GraphD3JSLine::setMargins (uint16_t nTop, uint16_t nBottom, uint16_t nLeft, uint16_t nRight)
{
	m_nMargin_Top = nTop;
	m_nMargin_Bottom = nBottom;
	m_nMargin_Left = nLeft;
	m_nMargin_Right = nRight;
}

uint16_t	GraphD3JSLine::getMargin_Top ()
{
	return m_nMargin_Top;
}

uint16_t	GraphD3JSLine::getMargin_Bottom ()
{
	return m_nMargin_Bottom;
}

uint16_t	GraphD3JSLine::getMargin_Left ()
{
	return m_nMargin_Left;
}

uint16_t	GraphD3JSLine::getMargin_Right()
{
	return m_nMargin_Right;
}

string		GraphD3JSLine::getSetting_DisplayWidth  ()
{
	string 	sSetting;
	char 	szTemp[20];

	sSetting = "WIDTH = ";

	sprintf(szTemp, "%d", m_nDisplay_Width);
	sSetting += szTemp;
	sSetting += ",\n";

	return sSetting;
}

string		GraphD3JSLine::getSetting_DisplayHeight ()
{
	string 	sSetting;
	char 	szTemp[20];

	sSetting = "HEIGHT = ";

	sprintf(szTemp, "%d", m_nDisplay_Height);
	sSetting += szTemp;
	sSetting += ",\n";

	return sSetting;

}

string		GraphD3JSLine::getSetting_Margins ()
{
	string 	sSetting;
	char 	szTemp[20];

	sSetting = "MARGINS = {\n";

	sSetting += "  top: ";
	sprintf(szTemp, "%d", m_nMargin_Top);
	sSetting += szTemp;
	sSetting += ",\n";

	sSetting += "  bottom: ";
	sprintf(szTemp, "%d", m_nMargin_Bottom);
	sSetting += szTemp;
	sSetting += ",\n";

	sSetting += "  left: ";
	sprintf(szTemp, "%d", m_nMargin_Left);
	sSetting += szTemp;
	sSetting += ",\n";

	sSetting += "  right: ";
	sprintf(szTemp, "%d", m_nMargin_Right);
	sSetting += szTemp;
	sSetting += "\n";

	sSetting += "},";

	return sSetting;
}

void		GraphD3JSLine::setVizArea (string sArea)
{
	m_sVizArea = sArea;
}

string		GraphD3JSLine::getVizArea ()
{
	return m_sVizArea;
}


