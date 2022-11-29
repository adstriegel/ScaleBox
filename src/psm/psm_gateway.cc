/*
 * psm_gateway.cc
 *
 *  Created on: Oct 26, 2012
 *      Author: striegel
 */


#include <iostream>
using namespace std;

#include "psm_gateway.h"

#include "../mon/Thread_Archive.h"
#include "../mon/Thread_Timer.h"

#include "../xml/NodeDOM.h"
#include "../xml/NodeElem.h"

#include "../util/ip-utils.h"

/////////////////////////////////////////////////////////////////////////

PSM_GatewayStats::PSM_GatewayStats () : Stats ()
{
  Allocate(PSM_GATEWAY_STAT_LAST);
}

/////////////////////////////////////////////////////////////////////////

void	PSM_GatewayStats::Get_Title	(int nStat, char * szTitle)
{
	switch(nStat)
	{
		case PSM_GATEWAY_WAN_PKTS:
			sprintf(szTitle, "Pkts(WAN)");
			break;
		default:
			sprintf(szTitle, "F%d", nStat);
			break;
	}
}

/////////////////////////////////////////////////////////////////////////

PSM_Gateway::PSM_Gateway ()
{

	m_nPort_SimpleTunnel = PSM_GATEWAY_SIMPLE_TUNNEL_PORT_DEFAULT;

	if(PSM_GATEWAY_FLAG_MONITORONLY_DEFAULT)
	{
		m_bMonitorOnly = true;
	}
	else
	{
		m_bMonitorOnly = false;
	}

	m_pClientMgr = new PSM_MobileClient_Manager();
}

PSM_Gateway::~PSM_Gateway ()
{
	delete m_pClientMgr;
}

char PSM_Gateway::initialize ()
{

	return 1;
}

bool PSM_Gateway::extractExtendedDOM (NodeDOM * pNode) {
	int		j;
	string	sTag;
	string	sData;

	for(j=0; j<pNode->getNumChildren(); j++) {



	}

	return true;
}

bool PSM_Gateway::processTimer (TimerEvent * pEvent)
{
/*	switch(pEvent->getTimerID())
	{



	}
*/

	return true;
}


NodeDOM * PSM_Gateway::getStats (NodeDOM * pRoot)
{
	return NULL;
}

CommandResult PSM_Gateway::processExtCommand (const vector<string> & theCommands, int nOffset)
{
	if(theCommands.size() <= nOffset) {
		return PROCESS_CMD_SYNTAX;
	}

//	cout << " Processing a command at the TWiCE module" << endl;

	if(theCommands[nOffset] == "startlog") {
		//return return PROCESS_CMD_SUCCESS;
	}


	return PROCESS_CMD_UNKNOWN;
}

bool PSM_Gateway::processWriteCallback (Packet * pPacket, void * pData) {
	return 1;
}


char PSM_Gateway::processTapPacket (Packet * pPacket, int nID)
{
  switch(nID)
    {
    case PSM_TAP_WAN:
//    	processPacket_WAN(pPacket);
      break;
    case PSM_TAP_VPN_TUN_SIMPLE:
      break;
    }
}

int PSM_Gateway::mapNameToID (string sName)
{

  if(sName == "WAN")
    {
      return PSM_TAP_WAN;
    }
  else if(sName == "TUN_SIMPLE")
    {
      return PSM_TAP_VPN_TUN_SIMPLE;
    }
  else
    {
      return PSM_TAP_NONE;
    }
}
