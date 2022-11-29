/*
 * fmnc_measurement_pair.h
 *
 *  Created on: Jul 16, 2014
 *      Author: striegel
 */

#ifndef FMNC_MEASUREMENT_PAIR_H_
#define FMNC_MEASUREMENT_PAIR_H_

#include <vector>
using namespace std;

#include <algorithm>
using namespace std;

/** One particular measurement pair that consists of a measurement packet that was sent as
 * well as a packet that was received at the server.
 */
class FMNC_Measurement_Pair
{
	public:
		FMNC_Measurement_Pair ();
		~FMNC_Measurement_Pair ();

		void	setMeasurement_Sent (FMNC_Measurement_Packet_TCP * pSent);
		FMNC_Measurement_Packet_TCP * getMeasurement_Sent ();

		void 	setMeasurement_Rcvd (FMNC_Measurement_Packet_TCP * pRcvd);
		FMNC_Measurement_Packet_TCP * getMeasurement_Rcvd ();

		/** Return the RTT (in milliseconds) as previously computed
		 * @returns 0 if there is not an appropriate sent / rcvd pair, the difference
		 * between the sent / receive pair in milliseconds otherwise
		 */
		double	getRTT ();

	/* protected: */
		/** Update / recompute the RTT
		 */
		bool	computeRTT ();

	private:
		FMNC_Measurement_Packet_TCP *	m_pMeasurement_Sent;
		FMNC_Measurement_Packet_TCP * 	m_pMeasurement_Rcvd;
		double							m_fRTT;
};

/** A list of measurement pairs as observed during a particular test run
 */
class FMNC_List_Measurement_Pairs
{
	public:
		FMNC_List_Measurement_Pairs ();
		~FMNC_List_Measurement_Pairs ();

		/** Add a measurement pair to the back of the list of measurement
		 * pairs.  Note that measurement pairs are not sorted by default
		 * and require a separate invocation of sorting if desired.
		 * @param pPair The measurement pair to add at the back of the list
		 */
		bool	addMeasurementPair (FMNC_Measurement_Pair * pPair);

		/** Retrieve a specific measurement pair from the list of
		 * measurement pairs
		 * @param nIndex The index of the measurement pair to retrieve
		 * @returns NULL if the index is out of bounds, non-NULL likely otherwise
		 */
		FMNC_Measurement_Pair * getMeasurementPair (int nIndex);

		/** Retrieve the number of measurement pairs present in this particular
		 * listing
		 */
		int		getCount ();

		/** Sort the list of measurement pairs by the arrival time of the
		 * sent packets in the measurement pairs to preserve order of arrival
		 */
		void	sortByTime ();

		/** Sort the list of measurement pairs by the RTT of the particular pair
		 */
		void	sortByRTT ();

		/** Clear the list of measurement pairs from this particular list, deleting
		 * any allocated measurement pairs
		 */
		void	clear ();

	private:
		vector<FMNC_Measurement_Pair *>	m_Pairs;
};


#endif /* FMNC_MEASUREMENT_PAIR_H_ */

