

#ifndef __VSOCKETDATAGRAM_H_
#define __VSOCKETDATAGRAM_H_

#include "VirtSocket.h"

/** A wrapper class for the datagram (UDP) centric implementation of 
 * a virtual socket for the virtual host / network emulation.
 */
class VSocketDatagram : public VirtSocket {
	public:
		VSocketDatagram ();
		~VSocketDatagram ();
		
	private:
	
	
};

#endif
