// ----------------------------------------------------------------------------
///  \file   NexusFactoryMessenger.h
///  \brief  
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     24 Feb 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __NEXUS_FACTORY_MESSENGER__
#define __NEXUS_FACTORY_MESSENGER__

#include <G4UImessenger.hh>
#include <globals.hh>

class G4UIdirectory;
class G4UIcmdWithABool;



namespace nexus {

	class NexusFactory;


	/// FIXME. CLASS DESCRIPTION

	class NexusFactoryMessenger: public G4UImessenger
	{
	public:
		/// Constructor
		NexusFactoryMessenger(NexusFactory*);
		/// Destructor
		virtual ~NexusFactoryMessenger();

		void SetNewValue(G4UIcommand*, G4String);
		G4String GetCurrentValue(G4UIcommand*);

	private:
		NexusFactory* _factory;

		G4UIdirectory* _directory;
		G4UIcmdWithAString* _geometry_name;

	};

}

  #endif