

#ifndef __VIRTUALHOST_H_
#define __VIRTUALHOST_H_

#include <vector>
using namespace std;

#include "VNetStack.h"

/** The virtual host entity in the network that represents an end client
 * or server with full network stack functionality. 
 */
class VirtualHost {
	public:
		VirtualHost ();
		~VirtualHost ();
	
		/** Add an adapter to the list of adapters connected to this host
		 * Note that this operation is not thread-safe if other threads are
		 * calling getAdapter
		 * @param pAdapter The adapter to add
		 * @returns True if successful, false otherwise
		 */
		bool	addAdapter (Adapter * pAdapter);
		
		/** Retrieve the number of adapters associated with this virtual
		 * host
		 * @returns The number of adapters registered at this virtual host
		 */
		int		getCountAdapters ();
		
		/** Retrieve a specific adapter associated with this virtual host.
		 * Note that this operation is not thread-safe if other threads are
		 * calling addAdapter
		 * @param nAdapter The adapter to retrieve
		 * @returns A pointer to a valid Adapter object if successful, false
		 *    otherwise
		 */
		Adapter *	getAdapter (int nAdapter);
		
	private:
		VNetStack *		m_pStack;
		
		
		vector<Adapter *> 	m_Adapters;
};


#endif
