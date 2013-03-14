// ----------------------------------------------------------------------------
///  \file   NexusApp.h
///  \brief  
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     8 March 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __NEXUS_APP__
#define __NEXUS_APP__

#include <G4RunManager.hh>


class G4GenericMessenger;


namespace nexus {

	class GeometryFactory;
	class GeneratorFactory;
	

	/// TODO. CLASS DESCRIPTION

	class NexusApp: public G4RunManager
	{
	public:
	/// Constructor
		NexusApp(G4String config_macro);
	/// Destructor
		~NexusApp();

		virtual void Initialize();

	private:
		void AddMacro(G4String);
		void CreateDetectorConstruction(G4String name);
		void CreatePhysicsList(G4String name);
		void CreatePrimaryGeneration(G4String name);
		


	private:
		G4GenericMessenger* _msg;

		GeometryFactory* _geomfctr;
		GeneratorFactory* _genfctr;

		std::vector<G4String> _macros;

		G4String _geometry_name;
		G4String _physics_list_name;
		G4String _generator_name;
		G4String _run_action_name;
		G4String _event_action_name;
		G4String _tracking_action_name;
		G4String _stepping_action_name;
		G4String _params_macro;
	};

} // namespace nexus

#endif