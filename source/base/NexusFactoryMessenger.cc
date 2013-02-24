// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 24 Feb 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "NexusFactoryMessenger.h"

#include "NexusFactory.h"

#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>


using namespace nexus;



NexusFactoryMessenger::NexusFactoryMessenger(NexusFactory* factory):
_factory(factory)
{
	_directory = new G4UIdirectory("/NexusFactory/");
	_directory->SetGuidance("Control commands for the NexusFactory.")

	_geometry_name = 
		new G4UIcmdWithAString("/NexusFactory/GeometryName", this);
	_geometry_name->SetGuidance("Name of the geometry to be used.");
}



NexusFactoryMessenger::~NexusFactoryMessenger()
{
	delete _geometry_name;
	delete _directory;
}



void NexusFactoryMessenger::SetNewValue(G4UIcommand* cmd, G4String value)
{
	if (cmd == _geometry_name) {
		_factory->SetGeometryName(value);
	}
}