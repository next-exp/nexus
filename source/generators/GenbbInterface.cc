// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 27 Mar 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "GenbbInterface.h"

#include "ConfigService.h"
#include "DetectorConstruction.h"

#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>

#include <bhep/bhep_svc.h>


namespace nexus {


  GenbbInterface::GenbbInterface(): _detConst(0)
  {
    // get genbb filename from config service
    G4String filename = 
      ConfigService::Instance().Generation().GetSParam("genbb_filename");

    // open ifstream associated to the genbb file.
    // if file is ok, start processing the header.
    // abort otherwise.
    _genbb.open(filename.data());
    if (_genbb.good())
      ProcessHeader();
    // else
    //   G4Exception("[GenbbInterface] ERROR: Cannot read input file!");

    // get generation region from config service
    _region = ConfigService::Instance().Generation().GetSParam("region");
  }



  GenbbInterface::~GenbbInterface()
  {
    _genbb.close();
  }

  
  
  /// Read an event from file and create primary particles and
  /// vertices accordingly
  void GenbbInterface::GeneratePrimaryVertex(G4Event* event)
  {
    // retrieve user detector construction only once
    if (!_detConst)
      _detConst = (DetectorConstruction*)
	G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    
    
    // reading event-related information    
    
    G4int entries;     // number of particles in the event
    G4long evt_no;     // event number
    G4double evt_time; // initial time in seconds
    
    _genbb >> evt_no >> evt_time >> entries;
    
    
    // abort if end-of-file was reached in last operation
    if (_genbb.eof()) {
      G4cout << "[GenbbInterface] End-of-File reached. "
	     << "Aborting the run..." << G4endl;
      G4RunManager::GetRunManager()->AbortRun();
      return;
    }

    
    // generate a position in the detector 
    // (all primary particles will be generated there)
    particle_position = _detConst->GetGeometry()->GenerateVertex(_region);
    
    
    // reading info for each particle in the event
    for (G4int i=0; i<entries; i++) {
      
      G4int g3code;           // GEANT3 particle code
      G4double px, py, pz;    // Momentum components in MeV
      
      _genbb >> g3code >> px >> py >> pz >> particle_time;
      
      G4ParticleDefinition* g4code = 
	G4ParticleTable::GetParticleTable()->FindParticle(G3toPDG(g3code));
      
      // create a primary particle
      G4PrimaryParticle* particle =
	new G4PrimaryParticle(g4code, px*MeV, py*MeV, pz*MeV);
      
      particle->SetMass(g4code->GetPDGMass());
      particle->SetCharge(g4code->GetPDGCharge());
      
      // create a primary vertex for the particle
      G4PrimaryVertex* vertex =
	new G4PrimaryVertex(particle_position, particle_time*second);
      
      vertex->SetPrimary(particle);
      
      // add vertex to the event
      event->AddPrimaryVertex(vertex);
    }
  }
  
  
  
  void GenbbInterface::ProcessHeader()
  {
    G4String line;

    // The first five lines of the file contain the filename, date
    // and time of generation, and the random seed. We skip them.
    for (int i=0; i<5; i++) { getline(_genbb, line); }

    // The following block in the header contains information about
    // the generation mode. The number of lines varies from mode to mode.
    // We'll process them one by one.
    //
    do {
      getline(_genbb, line);
      
      // event type
      if (line.contains("event type")) {
	
	// remove characters in line till the colon (':')
	line = line.erase(0, line.find(":")+1);
	// remove white spaces, if any
	// ('2' means that strips from both start and end)
	line = line.strip(2, ' ');
	// add to run info properties
	bhep::bhep_svc::instance()->get_run_info()
	  .add_property("GENERATION_event_type", line);

	// if event type is a bb mode, there are three more lines
	// with relevant information.
	getline(_genbb, line);
	if (line.contains("0+ ->")) {
	  
	  // bb mode
	  line = line.strip(2, ' ');
	  bhep::bhep_svc::instance()->get_run_info().
	    add_property("GENERATION_bb_mode", line);
	  
	  // energy level of daughter nucleus
	  getline(_genbb, line);
	  
	  // Qbb (bb energy release)
	  G4double Qbb;
	  _genbb.ignore(256, '=');
	  _genbb >> Qbb;
	  bhep::bhep_svc::instance()->get_run_info().
	    add_property("GENERATION_Qbb", Qbb*MeV);
	}
      }
      
      if (line.contains("range for sum")) {
	
	// range for sum of energies of emitted e- in a bb decay
	G4double min, max;
	_genbb >> min >> max;
	bhep::bhep_svc::instance()->get_run_info().
	  add_property("GENERATION_range_min", min*MeV);
	bhep::bhep_svc::instance()->get_run_info().
	  add_property("GENERATION_range_max", max*MeV);

	// 'toallevents' normalization
	_genbb.ignore(256, '\n');
	getline(_genbb, line);
	G4cout << line << G4endl;
	G4double toallevents;
	_genbb >> toallevents;
	bhep::bhep_svc::instance()->get_run_info().
	  add_property("GENERATION_toallevents", toallevents);
      }
      
    } while (!line.contains("Format of data"));
    
    for (int i=0; i<10; i++) { getline(_genbb, line); }


    G4int foo;
    G4long num_events_file, num_events_job;

    // last line of header contains number of events in file
    _genbb >> foo >> num_events_file;
    
    // get from config service the number of events requested by user
    num_events_job = 
      ConfigService::Instance().Job().GetIParam("number_events");
    
    if (num_events_job > num_events_file) {
      G4cout << "[GenbbInterface] WARNING: "
	     << "GENBB file contains only " << num_events_file 
	     << ". Won't be possible to complete the requested job."
	     << G4endl;
    }
  }
  
  
  
  G4int GenbbInterface::G3toPDG(const G4int G3code)
  {
    if      (G3code == 1) return  22;         // gamma
    else if (G3code == 2) return -11;         // e+
    else if (G3code == 3) return  11;         // e-
    else if (G3code == 5) return -13;         // mu+
    else if (G3code == 6) return 13;          // mu-
    else if (G3code == 13) return 2112;       // neutron
    else if (G3code == 14) return 2212;       // proton
    else if (G3code == 47) return 1000020040; // alpha
    else {
      G4cerr << "[GenbbInterface] ERROR: Particle with unknown GEANT3 code: "
	     << G3code << G4endl;
      //G4Exception("Aborting run...");
    }
  }
  
  
} // end namespace nexus


