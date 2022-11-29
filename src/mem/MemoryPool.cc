

#include "MemoryPool.h"

#include <iostream>
using namespace std;

// The global definition of the memory pool is here
MemoryPool		g_MemPool;


MemoryPool::MemoryPool () {
	int			j;
	
	for(j=0; j<MEMPOOL_OBJ_CUSTLAST; j++) {
		// Initialize the mutexes
		pthread_mutex_init(m_PoolMutexes+j, NULL);
		
		// Set the growth curve to 5 for now
		m_nPoolGrowth[j] = 5;
		
		// Allow up to 50000 of any object type simultaneously to exist
		//  in the system
		m_nMaxPoolSize[j] = MEMPOOL_DEFAULT_MAXOBJECTS;
		
		// How many have we allocated, zero as of right now
		m_nAllocated[j] = 0;
	}	

	// We have a special case at the boundary between base and custom that should
	//  its growth and maximum pool size tweaked.  We might purge this in the
	//  future but for now we will leave it in
	m_nMaxPoolSize[MEMPOOL_OBJ_LAST] = 0;
	m_nPoolGrowth[MEMPOOL_OBJ_LAST] = 0;
	
}

MemoryPool::~MemoryPool () {
	// Iterate through each array and free the relevant objects
	
	// This is kind of a moot point anyway since the only time the memory pool
	//  disappears is upon shutdown	
}
	
MemPoolObject* MemoryPool::getObject (int nType) {
	MemPoolObject*	pMemObj;
	vector<MemPoolObject*>::iterator		pElem; 	
	
	// Check the sanity of the type value, i.e. will it ever hit
	//  an actual object that we are pooling
	if(!checkValidMemObjectType(nType)) {
		return NULL;
	}
			
	//  Lock the mutex for that pool
	pthread_mutex_lock(m_PoolMutexes+nType);
	
	// OK, does that pool have anything in it?
	if(m_PoolArray[nType].size() > 0) {		
		// Yes, it does, pull it out and return it
		pMemObj = m_PoolArray[nType].front();
		
		// Erase the first one (why we need to lock the mutex)
		//pElem = *(m_MonPackets.begin());
		//m_PoolArray[nType].erase(m_PoolArray[nType].begin());
		m_PoolArray[nType].pop_front();

		//  Unlock the mutex for that pool
		pthread_mutex_unlock(m_PoolMutexes+nType);
		
		// The clean burden is on adding it to the pool, not pulling it out
		//  of the pool
		//pMemObj->clean();
		
		pMemObj->markAllocated();
		return pMemObj;
	}
	
	// All gone, need to allocate more
	if(!increasePool(nType)) { 
		// Failed, just return NULL
		cerr << "  Failed to allocate additional objs of type " << nType << " into memory pool" << endl;
		return NULL;
	} else {	
		// Now we have a pool, pull it out and return it
		pMemObj = m_PoolArray[nType].front();
		
		// Erase the first one (why we need to lock the mutex)
		//pElem = *(m_MonPackets.begin());
		//m_PoolArray[nType].erase(m_PoolArray[nType].begin());
		m_PoolArray[nType].pop_front();
		
		//  Unlock the mutex for that pool
		pthread_mutex_unlock(m_PoolMutexes+nType);
		
		// The clean burden is on adding it to the pool, not pulling it out
		//  of the pool
		//pMemObj->clean();
		
        pMemObj->markAllocated(); // Don't forget to mark it as allocated
		return pMemObj;
	}
}

bool MemoryPool::releaseObject (MemPoolObject * pMemObj) {
	if(pMemObj == NULL) {
		cerr << "Warning: Attempted to release a NULL object to the memory pool" << endl;
		return false;
	}
	
	if(!checkValidMemObjectType(pMemObj->getType())) {
		cerr << "Error: Attempted to release an object to the memory pool with a bad type" << endl;
		cerr << "    Object ignored and not committed to the pool" << endl;
		cerr << "    Type of object: " << pMemObj->getType() << endl;
		return false;
	}	

	// Mark it as being in the pool from here on out
	pMemObj->markInPool();
	pMemObj->setOwner(this);
	

	// For now, we will put the onus on cleaning on the one putting it
	//  in the pool
	pMemObj->cleanBase();
	pMemObj->clean();
		
	//  Lock the mutex for that pool
	pthread_mutex_lock(m_PoolMutexes+pMemObj->getType());
	
	//  Add this to the vector pool for that particular object
	m_PoolArray[pMemObj->getType()].push_back(pMemObj);

	//  Unlock the mutex for that pool
	pthread_mutex_unlock(m_PoolMutexes+pMemObj->getType());	
	
	return true;
}

bool MemoryPool::changeMaxPoolSize (int nType, int nNewMax)
{
	if(nType < 0 || nType > MEMPOOL_OBJ_CUSTLAST)
	{
		cerr << "Illegal object requested to change the maximum pool size" << endl;
		cerr << "  Type = " << nType << " New Max = " << nNewMax << endl;
		return false;
	}

	if(nNewMax <= 0)
	{
		cerr << "Illegal size (less than zero) requested to change the maximum pool size" << endl;
		cerr << "  Type = " << nType << " New Max = " << nNewMax << endl;
		return false;
	}

	//  Lock the mutex for that pool
	pthread_mutex_lock(m_PoolMutexes+nType);

	//  Add this to the vector pool for that particular object
	m_nMaxPoolSize[nType] = nNewMax;

	// TODO: What happens if we tweak it but we have already allocated that amount,
	//       i.e. we reduce it but are bigger?

	//  Unlock the mutex for that pool
	pthread_mutex_unlock(m_PoolMutexes+nType);
	return true;
}


char MemoryPool::increasePool (int nType) {
	int		j;
	
	// Assume that thread safety is already done elsewhere
	if(!checkValidMemObjectType(nType)) {
		return 0;
	}
	
	for(j=0; j<m_nPoolGrowth[nType]; j++) {
		MemPoolObject *	pMemObj;
		
		if(m_nAllocated[nType] >= m_nMaxPoolSize[nType]) {
			cerr << "Allocation failed - maximum pool size of " << m_nMaxPoolSize[nType] << " reached" << endl;
			cerr << "   Object type is " << nType << endl;
			return 0;
		}
		
		// Call out to the MemPoolObject code to correctly translate
		//  the type to an actual class and newly allocated object
		pMemObj = allocateManagedMemObject(nType);		
		pMemObj->clean();
	
		if(pMemObj == NULL) {
			return 0;
		} else {
			// Add it to the vector 
			m_PoolArray[nType].push_back(pMemObj);
			// Update the allocation numbers
			m_nAllocated[nType]++;
		}
	}
	
	// Success!
	return 1;
}

void MemoryPool::dumpStatus () {
	int			j;
	float			fVal;
	
	for(j=0; j<MEMPOOL_OBJ_CUSTLAST; j++) {
		//  Lock the mutex for that pool
		pthread_mutex_lock(m_PoolMutexes+j);
		
		if(m_PoolArray[j].size() > 0)
		{
			cout << "  Object " << j << "  " << MemPoolObject::mapIDtoClassType(j) << "  P: ";
			cout << m_PoolArray[j].size() << " A: " << m_nAllocated[j] << "    ";
	
			if(m_nAllocated[j] > 0) {
				printf("%5.2f%%\n", (float) m_PoolArray[j].size() / (float) m_nAllocated[j] * (float) 100);
			} else {
				printf("\n");
			}
		}
			
		//  Unlock the mutex for that pool
		pthread_mutex_unlock(m_PoolMutexes+j);		
	}	
}


CommandResult	MemoryPool::processCommand (const vector<string> & theCommands, int nOffset) {	
	if(theCommands.size() <= nOffset) {
		return PROCESS_CMD_SYNTAX;
	}
	
	if(theCommands[nOffset] == "status") {
		dumpStatus();
		return PROCESS_CMD_SUCCESS;
	}
	
	return PROCESS_CMD_UNKNOWN;	
}
