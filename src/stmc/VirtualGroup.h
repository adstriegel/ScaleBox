#ifndef VIRTUALGROUP_H_
#define VIRTUALGROUP_H_

/** A virtual group is a collection of packets gathered over a
 * short timeframe for transmission to a group of clients. The
 * virtual group is translated into an covering set of physical
 * groups or unicast packets by the transport protocol
 */
class VirtualGroup {
	public:
		VirtualGroup ();
		~VirtualGroup ();	
	
	private:
	
};

#endif /*VIRTUALGROUP_H_*/
