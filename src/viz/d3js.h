/*
 * d3js.h
 *
 *  Created on: Jul 13, 2014
 *      Author: striegel
 */

#ifndef D3JS_H_
#define D3JS_H_

#include <vector>
using namespace std;

#include <string>
using namespace std;

#include "vizDataSet.h"
#include "../core/StdHeaders.h"


#define DEFAULT_D3JS_DISPLAY_WIDTH		1000
#define DEFAULT_D3JS_DISPLAY_HEIGHT		500

#define DEFAULT_D3JS_MARGIN_TOP			20
#define DEFAULT_D3JS_MARGIN_BOTTOM		20
#define DEFAULT_D3JS_MARGIN_LEFT		20
#define DEFAULT_D3JS_MARGIN_RIGHT		20

/* d3js is a graphical tool developed out of the academic visualization community.  It allows
 * for interesting visualization embedded inside of javascript. These files are part of the
 * ScaleBox distribution but can also be partitioned off / used separately.
 */

class GraphD3JSLine
{
	public:
		GraphD3JSLine ();
		~GraphD3JSLine ();

		/// Extract only the script portion of the D3JS line graph
		/// @returns D3JS javascript line graph based on datasets inside of this graph
		string 	extractScript ();

		/// Get the count of datasets associated with this particular graph
		/// @returns The number of datasets present in this graph
		int		getCountDatasets ();

		/// Get the dataset at a particular index
		///	@param nIndex The index of the dataset to retrieve
		/// @returns NULL if the dataset does not exist, otherwise a non-NULL object if valid
		VizDataSet *	getDataset (int nIndex);

		/// Set the limits for the x axis
		void		setAxisX_Limits (double fMin, double fMax);

		/// Set the limits for the y axis
		void		setAxisY_Limits (double fMin, double fMax);

		/// Clean out the datasets
		/// @param bDelete Denote whether or not the inner datasets should be deleted
		void		clear (bool bDelete);


		/// Add a dataset to the graph.  Note that adding the dataset does not necessarily
		/// imply that this dataset will be curated / owned by this graph object.  The clear
		/// function must be called with the deletion flag set later.
		/// @param pDataset The non-NULL dataset to add
		void		addDataset (VizDataSet * pDataset);

		/// Extract the data points
		string		extractDataPoints ();

		/// Retrieve the graph width (in pixels)
		uint16_t	getDisplay_Width ();

		/// Set the graph width in pixels
		void		setDisplay_Width (uint16_t nWidth);

		/// Retrieve the graph height (in pixels)
		uint16_t	getDisplay_Height ();

		/// Set the graph height in pixels
		void		setDisplay_Height (uint16_t nHeight);


		void		setMargins (uint16_t nTop, uint16_t nBottom, uint16_t nLeft, uint16_t nRight);
		uint16_t	getMargin_Top ();
		uint16_t	getMargin_Bottom ();
		uint16_t	getMargin_Left ();
		uint16_t	getMargin_Right();

		void		setVizArea (string sArea);
		string		getVizArea ();

	protected:

		string		getSetting_DisplayWidth  ();
		string		getSetting_DisplayHeight ();
		string		getSetting_Margins ();

	private:
		vector<VizDataSet *>	m_Datasets;

		double		m_fAxisY_Min;
		double		m_fAxisY_Max;

		double		m_fAxisX_Min;
		double		m_fAxisX_Max;

		uint16_t	m_nDisplay_Width;
		uint16_t	m_nDisplay_Height;

		uint16_t	m_nMargin_Top;
		uint16_t	m_nMargin_Bottom;
		uint16_t	m_nMargin_Left;
		uint16_t	m_nMargin_Right;


		string		m_sVizArea;

};


#endif /* D3JS_H_ */
