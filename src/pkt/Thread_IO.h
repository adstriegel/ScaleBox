/* Thread_IO.h
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://gipse.cse.nd.edu/NetScale
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 * Students: Dave Salyers (lead)   Yingxin Jiang
 *           Xiaolong Li           Jeff Smith
 * 
 **********************************************************
 * $Revision: 1.1.1.1 $  $Date: 2008/02/13 04:21:21 $
 **********************************************************
 */

#ifndef THREAD_IO_H_
#define THREAD_IO_H_

/** The input thread for the Adapter that simply does a simple
 * looped call to the readPacket function.
 */
void * Thread_Input (void * pArg);

/** The thread that consumes packets created by the input thread
 * by passing it along the input chain
 */
void * Thread_Process (void * pArg);

/** The output thread for the Adapter object that simply does a looped
 * inspection of items in the queue followed by an invocation of the
 * write functionality. 
 */
void * Thread_Output (void * pArg);

#endif /*THREAD_IO_H_*/
