// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>
//  Created: 27 Mar 2009
//
//  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "Decay0Interface.h"

#include "DetectorConstruction.h"
#include "BaseGeometry.h"

#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include "decay0.h"
#include <iostream>
using namespace nexus;




Decay0Interface::Decay0Interface(): 
  G4VPrimaryGenerator(), _msg(0), _opened(false), _geom(0)
{

  _msg = new G4GenericMessenger(this, "/Generator/Decay0Interface/",
    "Control commands of the Decay0 interface.");

  _msg->DeclareMethod("inputFile", &Decay0Interface::OpenInputFile, "");
  _msg->DeclareProperty("region", _region, "");
  
  _msg->DeclareMethod("EnergyThreshold", &Decay0Interface::SetEnergyThreshold, ""); // for electrons only. 
  _msg->DeclareMethod("Xe136DecayMode", &Decay0Interface::SetXe136DecayMode, "");
  _msg->DeclareMethod("Ba136FinalState", &Decay0Interface::SetBa136FinalState, "");

  DetectorConstruction* detConst = (DetectorConstruction*)
  G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  _geom = detConst->GetGeometry();
  
  _decay0 = 0;
  _myEventCounter = 0;
  _energyThreshold = 0.;
}



Decay0Interface::~Decay0Interface()
{
  if (_file.is_open()) _file.close();
  if (_fOutDebug.is_open()) _fOutDebug.close(); 
  if (_decay0 != 0) delete _decay0;
}



void Decay0Interface::OpenInputFile(G4String filename)
{
   if (filename.find("none") != std::string::npos) {
      std::cerr << " We will use the new interface to Decay00 " << std::endl;
     _opened = false;
     return;
   }
     
  _file.open(filename.data());

  if (_file.good()) {
    _opened = true;
    ProcessHeader();
  }
  else {
    G4Exception("[Decay0Interface]", "SetInputFile()", JustWarning,
      "Cannot open Decay0 input file.");
  }
}



/// Read an event from file and create primary particles and
/// vertices accordingly
void Decay0Interface::GeneratePrimaryVertex(G4Event* event)
{
  const bool runG4 = true;
//  const bool runG4 = false;
  if (!_opened) {
     if (_decay0 == 0) {
       const std::string XeName("Xe136");
       _decay0 = new decay0(XeName, _Ba136FinalState, _Xe136DecayMode);
      // Temporary debugging file, just generate particle and dump them on a file 
//      std::ostringstream fOutStrStr; fOutStrStr << "./Decay0Out_" << _Ba136FinalState << "_" << _Xe136DecayMode << "_V1.txt";
//      std::string fOutStr(fOutStrStr.str());
//      _fOutDebug.open(fOutStr.c_str());
       if (_fOutDebug.is_open()) _fOutDebug << " evt tr pdg px py pz e t  " << std::endl;
     }
     
     std::vector<decay0Part> theParts;
     _decay0->decay0DoIt(theParts);
     //
     // Keep the event only if the sum of the electron energies are above the threshold. 
     //
     double eTotKin = 0.;
     for(std::vector<decay0Part>::const_iterator itp = theParts.begin(); itp != theParts.end(); itp++) {
       
       if (std::abs(itp->_pdgCode) == 11) eTotKin += itp->_energy; 
     }
     const bool keepEvt = eTotKin > _energyThreshold;
     _myEventCounter++;
     if (_fOutDebug.is_open() && keepEvt ) {
       int k = 0;
       for (std::vector<decay0Part>::const_iterator itp = theParts.begin(); itp != theParts.end(); itp++, k++) {
         _fOutDebug << " " << _myEventCounter << " " << k << " " << itp->_pdgCode << " " 
	           << itp->_pmom[0] << " " <<  itp->_pmom[1] << " " << itp->_pmom[2] << " "
		   << itp->_energy << " " << itp->_time << std::endl;
        }
     }
     if (runG4 && keepEvt) {
        particle_position = _geom->GenerateVertex(_region);
        for (std::vector<decay0Part>::const_iterator itp = theParts.begin(); itp != theParts.end(); itp++) {
          G4ParticleDefinition* g4code = 
             G4ParticleTable::GetParticleTable()->FindParticle(itp->_pdgCode);
          G4PrimaryParticle* particle =
	     new G4PrimaryParticle(g4code, MeV*itp->_pmom[0], MeV*itp->_pmom[1], MeV*itp->_pmom[2]);
         // create a primary vertex for the particle
          G4PrimaryVertex* vertex =
              new G4PrimaryVertex(particle_position, particle_time*second);
         vertex->SetPrimary(particle);
         event->AddPrimaryVertex(vertex);
        } 
    }
     return;
   }

  //G4cout << "GeneratePrimaryVertex()" << G4endl;
 
  // reading event-related information    
  G4int entries;     // number of particles in the event
  G4long evt_no;     // event number
  G4double evt_time; // initial time in seconds

  _file >> evt_no >> evt_time >> entries;


  // abort if end-of-file was reached in last operation
  if (_file.eof()) {
    G4cout  << "[Decay0Interface] End-of-File reached. "
            << "Aborting the run..." << G4endl;
    G4RunManager::GetRunManager()->AbortRun();
    return;
  }

  //G4cout << "entries: " << entries << G4endl;

  // generate a position in the detector 
  // (all primary particles will be generated there)
  particle_position = _geom->GenerateVertex(_region);


  // reading info for each particle in the event
  for (G4int i=0; i<entries; i++) {
    //G4cout << i << G4endl;

    G4int g3code;           // GEANT3 particle code
    G4double px, py, pz;    // Momentum components in MeV

    _file >> g3code >> px >> py >> pz >> particle_time;

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



void Decay0Interface::ProcessHeader()
{
  G4String line;

  while (!line.contains("First event")) getline(_file, line);

  getline(_file, line);
  getline(_file, line);
}



G4int Decay0Interface::G3toPDG(const G4int G3code)
{
  int pdg_code = 0;
  if      (G3code == 1) pdg_code =  22;         // gamma
  else if (G3code == 2)  pdg_code = -11;         // e+
  else if (G3code == 3) pdg_code =   11;         // e-
  else if (G3code == 5) pdg_code =  -13;         // mu+
  else if (G3code == 6) pdg_code =  13;          // mu-
  else if (G3code == 13) pdg_code =  2112;       // neutron
  else if (G3code == 14) pdg_code =  2212;       // proton
  else if (G3code == 47) pdg_code =  1000020040; // alpha
  else {
    G4cerr << "[Decay0Interface] ERROR: Particle with unknown GEANT3 code: "
    << G3code << G4endl;
     G4Exception("[Decay0Interface]", "G3toPDG()", FatalException,
		 "Unknown particle GEANT3 code!");     
  //G4Exception("Aborting run...");
  }
  return pdg_code;
}




