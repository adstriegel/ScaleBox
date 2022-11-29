
/* Main.cc
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://netscale.cse.nd.edu
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 * Students: Dave Salyers (lead)   Yingxin Jiang
 *           Xiaolong Li           Jeff Smith 			 
 * 
 **********************************************************
 * $Revision: 1.10 $  $Date: 2008/04/08 21:52:01 $
 **********************************************************
 */
 
 #include <iostream>
 using namespace std;
 
 #include "../pkt/AdapterPCap.h"
 #include "../vnet/AdapterVirtual.h"
 #include "../pkt/AdapterFile.h"
 
 #include "../pkt/InputLog.h"
 #include "../stmc/InputIngressVGDM.h"
 #include "../pkt/InputSignature.h"
 #include "../xml/FileXML.h"
 #include "../xml/NodeDOM.h"
 #include "../xml/NodeElem.h"
 
 #include "../pkt/InputPurge.h"
 #include "../pkt/Filter.h"
 #include "../pkt/FilterRule.h"
 #include "../pkt/FilterLayer.h"
 
 #include "../mon/Thread_Timer.h"
 
 // Global monitor for placing loaded components for future reference
 #include "../mon/Monitor.h"
 
 #include "Console.h"
 
 void		MapXML (char * pFileName);
 
 
void showRunHelp () {
	cout << "** ScaleBox Command Line Help" << endl;
	cout << "  Startup syntax:   ScaleBox ConfigFile" << endl;	 
}
 
 int main (int argc, char * const argv[]) {
 	
 	if(argc < 2) {
 		cout << "Error: Insufficient arguments.  " << endl;
 		showRunHelp();
 		return -1; 
 	}
 	
 	// Help catches
 	
 	if(strcmp("-?", argv[1]) == 0) {
 		showRunHelp();
 		return 0;
 	}

 	if(strcmp("-help", argv[1]) == 0) {
 		showRunHelp();
 		return 0;
 	}

 	if(strcmp("help", argv[1]) == 0) {
 		showRunHelp();
 		return 0;
 	}

 	
 	g_Monitor.startup();
 	 	
	MapXML(argv[1]);
 	
 	//AdapterPCap *		pTestAdp;
 	// pTestAdp = new AdapterPCap();
 	// pTestAdp->setDevName("en0");
 	// pTestAdp->openDevice();
	// 	InputLog *		pLogTest;
 	//pLogTest = new InputLog();
	//pTestAdp->appendInputModule(pLogTest);
	//cout << "  Added the logging module" << endl;

	// Add a purge module to test
	
	//InputPurge	*	pPurgeTest;
	
	//pPurgeTest = new InputPurge();
	
	//Filter		*	pFilter;
	//FilterRule	*	pRule;
	//FilterL2Ethernet	*	pMACFilter;
	//char				byTypeLen[2];
	
	//pFilter = new Filter();
	//pRule = new FilterRule();
	//pMACFilter = new FilterL2Ethernet();
	
	//byTypeLen[0] = 0x08;
	//byTypeLen[1] = 0x00;
	
	//pRule->setExclude(true);
	
	//pMACFilter->enableFilterTypeLen(true);
	//pMACFilter->setFilterTypeLen(byTypeLen);
	
	//pRule->addFilterLayer(pMACFilter);
	//pFilter->addRule(pRule);
	
	//pPurgeTest->setFilter(pFilter);
	
	//pTestAdp->appendInputModule(pPurgeTest);

	// Add a signature module

	//InputSignature		*	pSig;
		
	//pSig = new InputSignature();
	//pTestAdp->appendInputModule(pSig);

	//SignatureRequest		theSig;
	
	//theSig.m_sigType = SIGNATURE_TYPE_XOR;
	//theSig.m_nOffset = 0;
	//theSig.m_nOffsetType = 0;

	//pSig->addSignatureType(theSig);
	
	//theSig.m_sigType = SIGNATURE_TYPE_MD5;
	//theSig.m_nOffset = 0;
	//theSig.m_nOffsetType = 0;

	//cout << "  Adding second signature" << endl;

	//pSig->addSignatureType(theSig);
	
	
	//cout << "  Establishing VGDM" << endl;	
	
	
	//InputIngressVGDM	*	pIngress;
	//pIngress = new InputIngressVGDM();

	//pTestAdp->appendInputModule(pIngress);
	
	
	
	//pTestAdp->setFlagEnabled(1);
	
	int j;
	
	g_TimerManager.startup();

	cout << "Starting up devices...." << endl;
	
	for(j=0; j<g_Monitor.getCountAdapters(); j++) {
		Adapter *	pTempAdp;
		
		pTempAdp = g_Monitor.getAdapter(j);
		pTempAdp->startDevice();
	}

	cout << "Starting up I/O chains on the adapters...." << endl;

	for(j=0; j<g_Monitor.getCountAdapters(); j++) {
		Adapter *	pTempAdp;
		
		pTempAdp = g_Monitor.getAdapter(j);	
		pTempAdp->startModules();
	}

	cout << "Starting up stand-alone modules...." << endl;
	
	for(j=0; j<g_Monitor.getCountModules(); j++) {
		IOModule *	pTempMod;
		
		pTempMod = g_Monitor.getModule(j);	
		pTempMod->startModule();
	}	

	cout << "Enabling devices...." << endl;
	
	for(j=0; j<g_Monitor.getCountAdapters(); j++) {
		Adapter *	pTempAdp;
		
		pTempAdp = g_Monitor.getAdapter(j);
		pTempAdp->setFlagEnabled(1);
		cout << " Enabled adapter " << j << endl;
	}	
			
	doConsole();	
 	
 	return 0;
 }
 
 
void mapScaleBox (NodeDOM * pScaleRoot);
 
void MapXML (char * pFileName) {
  	// Test the XML processing
 	FileXML		testFile;
 	int			j;
 	
 	testFile.setPath("");
 	testFile.setFile(pFileName);
 	
 	//testFile.setPath("../ex/cfg/");
 	//testFile.setFile("pktcache-test.xml");
 	
	if(testFile.readDOMfromFile()) {
		cout << "Successfully parsed DOM from XML" << endl;	
		testFile.getDOMRoot()->dumpConsole();
	} else {
		exit(-1);
	}
 	
 	NodeDOM		*		pRoot;
 	NodeDOM		*		pNode;
 	
 	pRoot = testFile.getDOMRoot();
 	
 	for(j=0; j<pRoot->getNumChildren(); j++) {
 		pNode = pRoot->getChild(j);
 		
 		cout << " Node " << j << " has tag of " << pNode->getTag() << endl;
 		
 		if(pNode->getTag() == "ScaleBox") {
 			break;	
 		}
 		
 		pNode = NULL;
 	}
 		
 	if(pNode != NULL) {
		// The children are the top tier items to create
		mapScaleBox(pNode);
 	}
 	
 }
 
void mapScaleBox (NodeDOM * pScaleRoot) {
	int j;
 	NodeDOM 	*		pNode;
 	NodeElem *		pElem;
 	
 	cout << "Children in DOM: " << pScaleRoot->getNumChildren() << endl;
 	pScaleRoot->dumpConsole();
 	 	
 	for(j=0; j<pScaleRoot->getNumChildren(); j++) {
 		pNode = pScaleRoot->getChild(j);
 		
 		cout << "Child " << j << " with tag of " << pNode->getTag() << endl;
 		 		
 		if(pNode->getTag() == "Adapter") {
 			// Got an adapter, figure out what type	
 						
 			pElem = pNode->findElement("type");
 			
 			if(pElem == NULL) {
 				cerr << "Warning: Unable to include adapter due to it not having a proper" << endl;
 				cerr << "  type (pcap, etc.) associated with it" << endl;
 			} else if (pElem->getValue() == "pcap") {
 				AdapterPCap *	pAdapter;
 				
 				pAdapter = new AdapterPCap();
 				
 				// Extract the base XML
 				
 				if(!pAdapter->extractBaseDOM(pNode)) {
 					cerr << "Error: Cannot process base information for adapter" << endl;
 					cerr << "   Base DOM processing failed, ignoring adapter" << endl;
 					delete pAdapter;
 					continue;
 				}
 				
 				// Extract the extended XML	

 				// ADS: This is already done inside the extractBaseDOM object
 				/*
 				if(!pAdapter->extractExtendedDOM(pNode)) {
 					cerr << "Error: Cannot process extended information for adapter" << endl;
 					cerr << "   Extended DOM processing failed, ignoring adapter" << endl;
 					delete pAdapter;
 					continue;
 				}
 				*/
 				
 				cout << "Adding an adapter to the monitor" << endl;
 				g_Monitor.addAdapter(pAdapter);
 			} else if (pElem->getValue() == "virtual") {
 				AdapterVirtual *	pAdapter;
 				
 				pAdapter = new AdapterVirtual();
 				
 				// Extract the base XML
 				
 				if(!pAdapter->extractBaseDOM(pNode)) {
 					cerr << "Error: Cannot process base information for adapter" << endl;
 					cerr << "   Base DOM processing failed, ignoring adapter" << endl;
 					delete pAdapter;
 					continue;
 				}
 				
 				// Extract the extended XML	
 				// ADS: This is already done inside the extractBaseDOM object
 				/*
 				if(!pAdapter->extractExtendedDOM(pNode)) {
 					cerr << "Error: Cannot process extended information for adapter" << endl;
 					cerr << "   Extended DOM processing failed, ignoring adapter" << endl;
 					delete pAdapter;
 					continue;
 				}
 				*/

 				cout << "Adding an adapter to the monitor" << endl;
 				g_Monitor.addAdapter(pAdapter);
 			}  else if (pElem->getValue() == "file") {
 				AdapterFile *	pAdapter;
 				
 				pAdapter = new AdapterFile();
 				
 				// Extract the base XML
 				
 				if(!pAdapter->extractBaseDOM(pNode)) {
 					cerr << "Error: Cannot process base information for adapter" << endl;
 					cerr << "   Base DOM processing failed, ignoring adapter" << endl;
 					delete pAdapter;
 					continue;
 				}
 				
 				// Extract the extended XML	
 				// ADS: This is already done inside the extractBaseDOM object
 				/*
 				if(!pAdapter->extractExtendedDOM(pNode)) {
 					cerr << "Error: Cannot process extended information for adapter" << endl;
 					cerr << "   Extended DOM processing failed, ignoring adapter" << endl;
 					delete pAdapter;
 					continue;
 				}
 				*/
 				cout << "Adding an adapter to the monitor" << endl;
 				g_Monitor.addAdapter(pAdapter);
 			}   					
 		} else if (pNode->getTag() == "Module") {
 			cout << " Processing module object" << endl;
 			IOModule * pModule;
 			
 			pElem = pNode->findElement("type");
 			
			if(pElem == NULL)
			{
			    cerr << " Element returned was NULL, ignoring module processing" << endl;
			    continue;
			}

			cout << pElem->getXML() << endl;

 			pModule = mapModuleFromName(pElem->getValue());
 			
 			if(pModule == NULL) {
 				cerr << "Error: Unable to map the module name (" << pElem->getValue() << ") to a" << endl;
 				cerr << " valid ScaleBox module type.  Check that the name listed is correct and" << endl;
 				cerr << " the module itself is enabled for compilation in the appropriate support file." << endl;
 				continue;
 			}  
 			
			// Extract the base XML
			if(!pModule->extractBaseDOM(pNode)) {
				cerr << "Error: Cannot process base information for module" << endl;
				cerr << "   Base DOM processing failed, ignoring module" << endl;
				delete pModule;
				continue;
			}
						
			// Extract the extended XML	
			if(!pModule->extractExtendedDOM(pNode)) {
				cerr << "Error: Cannot process extended information for module" << endl;
				cerr << "   Extended DOM processing failed, ignoring module" << endl;
				delete pModule;
				continue;
			}
 			
 			if(pModule != NULL) {
 				g_Monitor.addModule(pModule);
 				
 				if(!pModule->initialize()) {
 					cerr << "Warning: Initialization for module " << pElem->getValue() << " reported issues" << endl;
 					continue;
 				}
 			}
 		} else if (pNode->getTag() == "Command") {
 			string *	pData;
 			
 			pData = new string(pNode->getData());
 			 			
 			spawnCommandThread(pData);
 		}
 	}
 	
 	cout << "XML configuration file parsing complete...." << endl;
}



 
 
