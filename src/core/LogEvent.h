/* LogEvent.h
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

#ifndef __LOGEVENT_H
#define __LOGEVENT_H

#include <string>
using namespace std;

// Severity taxonomy
// 
//  Critical		The code is in really, really bad shape. A critical event
//					will force a complete sanity check as possible for the system
//					in a last ditch effort to save it
//
//	Failure			Bad things have happened (memory allocation issue, etc.) but
//					we still might be able to recover
//
//	Fault - High	Boundary case execeptios / potential security issues
//


enum LogEventSeverity {
	LOGEVENT_SEVERITY_CRITICAL,		// Very, very bad things
	LOGEVENT_SEVERITY_FAILURE,		// Things that are bad but still might be recoverable
	LOGEVENT_SEVERITY_FAULT_HIGH,	// Warning - high level (likely to lead to critical issues)
	LOGEVENT_SEVERITY_FAULT_MEDIUM,	// Warning - medium level
	LOGEVENT_SEVERITY_FAULT_LOW,	// Warning - not necessary always to log
	LOGEVENT_SEVERITY_INFORM,		// Informative output
	LOGEVENT_SEVERITY_VERBOSE,		// Verbose output (function flow as possible)
	
	LOGEVENT_SEVERITY_NONE			// Nothing to see here, move along
};

enum LogEventCategory {
	LOGEVENT_CATEGORY_SYSTEM,		// System issue
	LOGEVENT_CATEGORY_NONE
	
	
};


/** The LogEvent class captures a single loggable event that should
 * either be saved or displayed to the user. The LogEvent and its
 * underlying mechanisms should largely allow the programmer to avoid
 * using traditional cout mechanisms and to seamlessly output content
 * to files, stdout, or other avenues.
 */
class LogEvent {
	public:
		LogEvent ();
		~LogEvent ();
	
		void	setText (string sText);
		string  getText ();
		
	private:
		string	m_sText;

		unsigned int	m_nSeverity;		
		unsigned int	m_nCategory;
};

// Additional helper functions that do not necessitate the creation of a
//  LogEvent object

void		logCriticalEvent 		(string sText);
void 		logFailureEvent			(string sText);
void		logFaultHighEvent  		(string sText);
void		logFaultMediumEvent  	(string sText);
void		logFaultLowEvent  		(string sText);

void		logCriticalEvent		(string sText, string sClass, string sFunction);
void		logFailureEvent			(string sText, string sClass, string sFunction);
void		logFaultHighEvent		(string sText, string sClass, string sFunction);
void		logFaultMediumEvent		(string sText, string sClass, string sFunction);
void		logFaultLowEvent		(string sText, string sClass, string sFunction);

void		logInformEvent			(string sText, string sClass, string sFunction);

#endif

