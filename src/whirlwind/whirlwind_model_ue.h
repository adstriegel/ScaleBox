/*
 * whirlwind_model_ue.h
 *
 * The model and code for the maintenance of user elements (UE) in the Whirlwind system.  A User Element
 * is any downstream user-owned device that is actively working with Whirlwind. The WhirlWind User Element
 * is effectively how we represent downstream UEs. We derive two classes from the base UE class for
 * the purposes of both the Gateway and the Rendezvous Point.
 */

#ifndef WHIRLWIND_UE_H_
#define WHIRLWIND_UE_H_

/** The base class for user elements that are covered / monitored by the Whirlwind system
 */
class Whirlwind_Model_UserElement
{
	public:

		/** Retrieve the Whirlwind-specific ID which is simply an enumeration of UEs as observed
		 * by Whirlwind
		 */
		long		getID_Whirlwind ();

		/** Set the Whirlwind-specific ID
		 */
		void 		setID_Whirlwind (long lID);

		/** Retrieve a pointer to the timeval structure noting when this model of a user element
		 * was created.
		 */
		struct timeval * 	getTime_Creation ();

		/** Set the time when this particular user element model was created
		 */
		void 				setTime_Creation(struct timeval * pTimeCreate);

		/** Retrieve a pointer to the timeval structure noting when this model of a user element was
		 * last meaningfully manipulated, a status was received, or the user element directly interacted
		 * with this gateway.
		 *
		 */
		struct timeval * 	getTime_LastAccessed ();

		/** Set the time when this particular user element model was created
		 */
		void 				setTime_LastAccessed (struct timeval * pTimeCreate);


	private:
		// When did we create this particular model entry?
		struct timeval		m_Time_Creation;

		// When did this user element last get accessed in a meaningful way?
		struct timeval		m_Time_LastAccess;

		// What is the Whirlwind-specific ID for this UE?
		long				m_lID;

};


/** A list of user elements
 */
class Whirlwind_List_UserElements
{
	public:
		/** Summarize a brief output to the console
		 */
		void	dumpConsole_Brief ();

		/** Summarize an extended version to the console
		 */
		void	dumpConsole ();

		/** Conduct a sanity check on this particular list of elements
		 * @returns True if everything is OK, false otherwise (more likely a crash)
		 */
		bool 	checkIntegrity ();

		/** Retrieve a count of the number of elements present
		 */
		int		getCount ();

		/** Remove a particular UE from the list
		 * @param pUE The particular user element to remove (should be non-NULL)
		 * @returns True if successful, false otherwise
		 */
		bool 	removeUE (Whirlwind_Model_UserElement * pUE);

		/** Add a new UE to the list
		 * @param pUE The new UE to add to the list (should be non-NULL)
		 * @returns True if successful, false otherwise
		 */
		bool	addUE (Whirlwind_Model_UserElement * pUE);



	private:

};


#endif /* WHIRLWIND_UE_H_ */
