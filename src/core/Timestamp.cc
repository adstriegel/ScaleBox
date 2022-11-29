/* Timestamp.cc : Implementation code for the Timestamp object 
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://netscale.cse.nd.edu/
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 **********************************************************
 * Development and equipment support has been provided in
 *  part by the following sponsors:
 * 
 *    National Science Foundation    CNS03-47392
 *    Sun Microsystems Academic Excellence Grant
 *    Intel IXP Development Grant 
 *    University of Notre Dame Faculty Grant
 * 
 **********************************************************
 */

#include <sys/time.h>

#include "Timestamp.h"

/////////////////////////////////////////////////////////////////////

Timestamp::Timestamp () {
	m_Time.tv_sec = 0;
	m_Time.tv_usec = 0;
}

/////////////////////////////////////////////////////////////////////

Timestamp::~Timestamp () {
	
}

/////////////////////////////////////////////////////////////////////

void Timestamp::clearTime ()
{
	m_Time.tv_sec = 0;
	m_Time.tv_usec = 0;
}

struct timeval * 	Timestamp::getCoreTime ()
{
	return &m_Time;
}

bool Timestamp::isClear()
{
	if(m_Time.tv_sec == 0 && m_Time.tv_usec == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////

void Timestamp::setToCurrentTime () {
	gettimeofday(&m_Time, NULL);		
}

/////////////////////////////////////////////////////////////////////

void Timestamp::setTime (timeval theTime) {
	m_Time = theTime;
}

/////////////////////////////////////////////////////////////////////

long int	Timestamp::getSeconds () {
	return m_Time.tv_sec;
}

/////////////////////////////////////////////////////////////////////

long int	Timestamp::getMicroseconds () {
	return m_Time.tv_usec;	
}

/////////////////////////////////////////////////////////////////////

Timestamp Timestamp::operator + (Timestamp tsComp) {
	Timestamp	temp;
	
	temp.m_Time.tv_sec = getSeconds() + tsComp.getSeconds();
	temp.m_Time.tv_usec = getMicroseconds() + tsComp.getMicroseconds();

	if(temp.m_Time.tv_usec > 1000000) {
		temp.m_Time.tv_sec += 1;
		temp.m_Time.tv_usec -= 1000000;
	}
	
	return temp;
}

/////////////////////////////////////////////////////////////////////

Timestamp Timestamp::operator - (Timestamp tsComp) {
	Timestamp	temp;
	
	temp.m_Time.tv_sec = getSeconds() - tsComp.getSeconds();
	temp.m_Time.tv_usec = getMicroseconds() - tsComp.getMicroseconds();
	
	if(temp.m_Time.tv_usec < 0) {
		temp.m_Time.tv_sec -= 1;
		temp.m_Time.tv_usec += 1000000;
	}
	
	return temp;
}

/////////////////////////////////////////////////////////////////////

bool Timestamp::operator < (Timestamp tsComp) {
	if(getSeconds() < tsComp.getSeconds()) {
		return true;
	} else if (getSeconds() > tsComp.getSeconds()) {
		return false;
	} else {
		// If it is not less and not greater, it must be equal
		
		if(getMicroseconds() < tsComp.getMicroseconds()) {
			return true;
		} else if (getMicroseconds() > tsComp.getMicroseconds()) {
			return false;
		} else {
			// Equal means that we are not less than
			return false;
		}
	}
}

/////////////////////////////////////////////////////////////////////

bool Timestamp::operator <= (Timestamp tsComp) {
	if(getSeconds() < tsComp.getSeconds()) {
		return true;
	} else if (getSeconds() > tsComp.getSeconds()) {
		return false;
	} else {
		// If it is not less and not greater, it must be equal
		
		if(getMicroseconds() < tsComp.getMicroseconds()) {
			return true;
		} else if (getMicroseconds() > tsComp.getMicroseconds()) {
			return false;
		} else {
			return true;
		}
	}
}

/////////////////////////////////////////////////////////////////////

bool Timestamp::operator > (Timestamp tsComp) {
	if(getSeconds() > tsComp.getSeconds()) {
		return true;
	} else if (getSeconds() < tsComp.getSeconds()) {
		return false;
	} else {
		// If it is not less and not greater, it must be equal
		
		if(getMicroseconds() > tsComp.getMicroseconds()) {
			return true;
		} else if (getMicroseconds() < tsComp.getMicroseconds()) {
			return false;
		} else {
			// Equal means that we are not less than
			return false;
		}
	}
}

/////////////////////////////////////////////////////////////////////

bool Timestamp::operator >= (Timestamp tsComp) {
	if(getSeconds() > tsComp.getSeconds()) {
		return true;
	} else if (getSeconds() < tsComp.getSeconds()) {
		return false;
	} else {
		// If it is not less and not greater, it must be equal
		
		if(getMicroseconds() > tsComp.getMicroseconds()) {
			return true;
		} else if (getMicroseconds() < tsComp.getMicroseconds()) {
			return false;
		} else {
			return true;
		}
	}
}

/////////////////////////////////////////////////////////////////////

bool Timestamp::operator == (Timestamp tsComp) {
	// If it is not less than nor greater than, it must be the same
	if(*this > tsComp) {
		return false;
	} else if (*this < tsComp) {
		return false;
	} else {
		return true;
	}
}

/////////////////////////////////////////////////////////////////////

Timestamp Timestamp::operator = (Timestamp op2) {
	m_Time = op2.m_Time;
	return *this;
}

/////////////////////////////////////////////////////////////////////

