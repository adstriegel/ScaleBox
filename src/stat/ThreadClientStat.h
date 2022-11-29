/* ThreadClientStat.h
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
 * $Revision: 1.2 $  $Date: 2008/04/04 02:34:01 $
 **********************************************************
 */

#ifndef __THREADCLIENTSTAT_H
#define __THREADCLIENTSTAT_H

/** Thread to handle clients receiving streaming statistics from this instance
 * of ScaleBox 
 */
void * Thread_ClientStat (void * pArg);

/** Thread to listen for incoming client statistic connections 
 */
void * Thread_ClientStatListen (void * pArg);

#endif

