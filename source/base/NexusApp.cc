// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 8 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "NexusApp.h"

#include <G4GenericMessenger.hh>


using namespace nexus;



NexusApp::NexusApp(G4String)
{

}



NexusApp::~NexusApp()
{
	
	delete _msg;
}
