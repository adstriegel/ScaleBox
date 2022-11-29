/* mgmt.h
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame.
 *
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *
 *    http://netscale.cse.nd.edu/
 *
 * PI:       Prof. Aaron Striegel    striegel@nd.edu
 *
 */


#ifndef MGMT_H_
#define MGMT_H_

/** Give back the CPU to the OS or to a different thread within the
 * ScaleBox architecture. The function allows for OS dependent variations
 * as necessary */
void		yieldCPU ();

/** A sleep mechanism based intrinsically on yieldCPU with the eventual
 * option to tie time into the concept of a virtual system time. Note that
 * this function is just an approximation and is not terribly low power
 * friendly in that it is largely OS agnostic.  
 * @param nMicroSleep The amount of time to sleep in microseconds
 * @param pBreakSleep A pointer to a signal memory location for breaking out of the
 *   the sleep early due to a change necessitating an earlier response. If NULL,
 *   there is no signal.
 *                    
 */
void microsleep (unsigned int nMicroSleep, char * pBreakSleep=NULL);

#endif /*MGMT_H_*/
