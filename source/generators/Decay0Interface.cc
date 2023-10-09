// ----------------------------------------------------------------------------
// nexus | Decay0Interface.cc
//
// This class is the primary generator of a Xe-136 double beta decay,
// interfacing the DECAY0 c++ code, translated from the original
// FORTRAN package, with nexus.
// It provides the primary vertex of a Xe-136 double beta decay.
// The possibility of reading a previously generated ascii file with the
// electron momenta is also allowed.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Decay0Interface.h"

#include "DetectorConstruction.h"
#include "GeometryBase.h"
#include "FactoryBase.h"

#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include "decay0.h"
#include <iostream>
using namespace nexus;

REGISTER_CLASS(Decay0Interface, G4VPrimaryGenerator)


Decay0Interface::Decay0Interface():
  G4VPrimaryGenerator(), msg_(0), decay_file_("th-e1-spectrum.dat"),
  opened_(false), geom_(0)
{

  msg_ = new G4GenericMessenger(this, "/Generator/Decay0Interface/",
    "Control commands of the Decay0 interface.");

  msg_->DeclareMethod("inputFile", &Decay0Interface::OpenInputFile, "");
  msg_->DeclareProperty("region", region_, "");
  msg_->DeclareProperty("decay_file", decay_file_,
                        "Name of the file with the decay info");

  msg_->DeclareMethod("EnergyThreshold", &Decay0Interface::SetEnergyThreshold, ""); // for electrons only.
  msg_->DeclareMethod("Xe136DecayMode", &Decay0Interface::SetXe136DecayMode, "");
  msg_->DeclareMethod("Ba136FinalState", &Decay0Interface::SetBa136FinalState, "");

  DetectorConstruction* detConst = (DetectorConstruction*)
  G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detConst->GetGeometry();

  decay0_ = 0;
  myEventCounter_ = 0;
  energyThreshold_ = 0.;
}



Decay0Interface::~Decay0Interface()
{
  if (file_.is_open()) file_.close();
  if (fOutDebug_.is_open()) fOutDebug_.close();
  if (decay0_ != 0) delete decay0_;
}



void Decay0Interface::OpenInputFile(G4String filename)
{
   if (filename.find("none") != std::string::npos) {
      std::cerr << " We will use the new interface to Decay00 " << std::endl;
     opened_ = false;
     return;
   }

  file_.open(filename.data());

  if (file_.good()) {
    opened_ = true;
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
  if (!opened_) {
     if (decay0_ == 0) {
       const std::string XeName("Xe136");
       decay0_ = new decay0(XeName, Ba136FinalState_, Xe136DecayMode_, decay_file_);
      // Temporary debugging file, just generate particle and dump them on a file
//      std::ostringstream fOutStrStr; fOutStrStr << "./Decay0Out_" << Ba136FinalState_ << "_" << Xe136DecayMode_ << "_V1.txt";
//      std::string fOutStr(fOutStrStr.str());
//      fOutDebug_.open(fOutStr.c_str());
       if (fOutDebug_.is_open()) fOutDebug_ << " evt tr pdg px py pz e t  " << std::endl;
     }

     std::vector<decay0Part> theParts;
     decay0_->decay0DoIt(theParts);
     //
     // Keep the event only if the sum of the electron energies are above the threshold.
     //
     double eTotKin = 0.;
     for(std::vector<decay0Part>::const_iterator itp = theParts.begin(); itp != theParts.end(); itp++) {

       if (std::abs(itp->pdgCode_) == 11) eTotKin += itp->energy_;
     }
     const bool keepEvt = eTotKin > energyThreshold_;
     myEventCounter_++;
     if (fOutDebug_.is_open() && keepEvt ) {
       int k = 0;
       for (std::vector<decay0Part>::const_iterator itp = theParts.begin(); itp != theParts.end(); itp++, k++) {
         fOutDebug_ << " " << myEventCounter_ << " " << k << " " << itp->pdgCode_ << " "
	           << itp->pmom_[0] << " " <<  itp->pmom_[1] << " " << itp->pmom_[2] << " "
		   << itp->energy_ << " " << itp->time_ << std::endl;
        }
     }
     if (runG4 && keepEvt) {
        particle_position = geom_->GenerateVertex(region_);
        for (std::vector<decay0Part>::const_iterator itp = theParts.begin(); itp != theParts.end(); itp++) {
          G4ParticleDefinition* g4code =
             G4ParticleTable::GetParticleTable()->FindParticle(itp->pdgCode_);
          G4PrimaryParticle* particle =
	     new G4PrimaryParticle(g4code, MeV*itp->pmom_[0], MeV*itp->pmom_[1], MeV*itp->pmom_[2]);
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

  file_ >> evt_no >> evt_time >> entries;


  // abort if end-of-file was reached in last operation
  if (file_.eof()) {
    G4cout  << "[Decay0Interface] End-of-File reached. "
            << "Aborting the run..." << G4endl;
    G4RunManager::GetRunManager()->AbortRun();
    return;
  }

  //G4cout << "entries: " << entries << G4endl;

  // generate a position in the detector
  // (all primary particles will be generated there)
  particle_position = geom_->GenerateVertex(region_);


  // reading info for each particle in the event
  for (G4int i=0; i<entries; i++) {
    //G4cout << i << G4endl;

    G4int g3code;           // GEANT3 particle code
    G4double px, py, pz;    // Momentum components in MeV

    file_ >> g3code >> px >> py >> pz >> particle_time;

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

  while (!G4StrUtil::contains(line, "First event")) getline(file_, line);

  getline(file_, line);
  getline(file_, line);
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
