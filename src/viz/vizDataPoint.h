/*
 * vizDataPoint.h
 *
 *  Created on: Jul 13, 2014
 *      Author: striegel
 */

#ifndef VIZDATAPOINT_H_
#define VIZDATAPOINT_H_

#include <string>
using namespace std;

class VizDataPoint
{
	public:

		virtual ~VizDataPoint() {}

		virtual string 	extractJSON() = 0;

};

/** One individual data point
 */
class VizDataPoint2D: public VizDataPoint
{
	public:
		/// Default constructor - initializes the data points to 0
		VizDataPoint2D ();

		/// Initializes the data points to the values specified
		VizDataPoint2D (double fX, double fY);

		virtual ~VizDataPoint2D();

		/// Retrieve the x-point
		double 	getX ();

		/// Set the x value
		void	setX (double fX);

		/// Retrieve the y value
		double  getY ();

		/// Set the y value
		void	setY (double fY);

		/// Extract a JSON representation of this object
		virtual string 	extractJSON();

	private:
		double		m_fX;
		double		m_fY;
};


#endif /* VIZDATAPOINT_H_ */
