/*
 * fmnc_measurement_pair.cc
 *
 */

#include "fmnc_measurement_packet.h"
#include "fmnc_measurement_pair.h"


FMNC_Measurement_Pair::FMNC_Measurement_Pair ()
{
	m_pMeasurement_Sent = NULL;
	m_pMeasurement_Rcvd = NULL;
	m_fRTT = 0;
}

FMNC_Measurement_Pair::~FMNC_Measurement_Pair ()
{
	m_pMeasurement_Sent = NULL;
	m_pMeasurement_Rcvd = NULL;
	m_fRTT = 0;
}

void	FMNC_Measurement_Pair::setMeasurement_Sent (FMNC_Measurement_Packet_TCP * pSent)
{
	m_pMeasurement_Sent = pSent;
	computeRTT();
}

FMNC_Measurement_Packet_TCP * FMNC_Measurement_Pair::getMeasurement_Sent ()
{
	return m_pMeasurement_Sent;
}

void 	FMNC_Measurement_Pair::setMeasurement_Rcvd (FMNC_Measurement_Packet_TCP * pRcvd)
{
	m_pMeasurement_Rcvd = pRcvd;
	computeRTT();
}

FMNC_Measurement_Packet_TCP * FMNC_Measurement_Pair::getMeasurement_Rcvd ()
{
	return m_pMeasurement_Rcvd;
}

double	FMNC_Measurement_Pair::getRTT ()
{
	return m_fRTT;
}

bool	FMNC_Measurement_Pair::computeRTT ()
{
	if(m_pMeasurement_Sent == NULL || m_pMeasurement_Rcvd == NULL)
	{
		m_fRTT = 0;
		return false;
	}

	// Compute the difference in seconds
	m_fRTT = m_pMeasurement_Rcvd->getTime()->tv_sec - m_pMeasurement_Sent->getTime()->tv_sec;
	// Convert from seconds to microseconds
	m_fRTT = m_fRTT * 1000000.0;

	m_fRTT += (m_pMeasurement_Rcvd->getTime()->tv_usec - m_pMeasurement_Sent->getTime()->tv_usec);

	m_fRTT /= 1000.0;
	return true;
}

FMNC_List_Measurement_Pairs::FMNC_List_Measurement_Pairs ()
{

}

FMNC_List_Measurement_Pairs::~FMNC_List_Measurement_Pairs ()
{

}

bool FMNC_List_Measurement_Pairs::addMeasurementPair (FMNC_Measurement_Pair * pPair)
{
	if(pPair == NULL)
	{
		return false;
	}

	m_Pairs.push_back(pPair);
	return true;
}

FMNC_Measurement_Pair * FMNC_List_Measurement_Pairs::getMeasurementPair (int nIndex)
{
	if(nIndex < 0 || nIndex >= m_Pairs.size())
	{
		return NULL;
	}

	return m_Pairs[nIndex];
}

int		FMNC_List_Measurement_Pairs::getCount ()
{
	return m_Pairs.size();
}

bool sortPairByRTT(FMNC_Measurement_Pair *pLeft, FMNC_Measurement_Pair * pRight)
{ return pLeft->getRTT() < pRight->getRTT(); }

bool sortPairByTime (FMNC_Measurement_Pair * pLeft, FMNC_Measurement_Pair * pRight)
{
	// Is the seconds value the same?
	if(pLeft->getMeasurement_Sent()->getTime()->tv_sec == pRight->getMeasurement_Sent()->getTime()->tv_sec)
	{
		return pLeft->getMeasurement_Sent()->getTime()->tv_usec < pRight->getMeasurement_Sent()->getTime()->tv_usec;
	}
	else if(pLeft->getMeasurement_Sent()->getTime()->tv_sec < pRight->getMeasurement_Sent()->getTime()->tv_sec)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void	FMNC_List_Measurement_Pairs::sortByTime ()
{
	sort(m_Pairs.begin(), m_Pairs.end(), sortPairByTime);
}

void	FMNC_List_Measurement_Pairs::sortByRTT ()
{
	sort(m_Pairs.begin(), m_Pairs.end(), sortPairByRTT);
}

void	FMNC_List_Measurement_Pairs::clear ()
{
	int		j;

	for(j=0; j<m_Pairs.size(); j++)
	{
		delete m_Pairs[j];
		m_Pairs[j] = NULL;
	}

	m_Pairs.clear();
}

