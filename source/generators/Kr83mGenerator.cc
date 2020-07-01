// ----------------------------------------------------------------------------
// nexus | Kr83mGenerator.cc
//
// This class is the primary generator for the decay chain
// of the isomeric state krypton 83.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Kr83mGenerator.h"

#include "G4Event.hh"
#include "DetectorConstruction.h"
#include "BaseGeometry.h"

#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
//
// in Global space... temporary hack. 
//
// #include <fstream>
// std::ofstream fOutCheckKr83mTmp;

namespace nexus {

  using namespace CLHEP;

  Kr83mGenerator::Kr83mGenerator() : geom_(0),energy_32_(32.1473*keV), energy_9_(9.396*keV),
                                     probGamma_9_(0.0490), lifetime_9_(154.*ns)
  {
  // From the TORI /ENSDF data tables. 
  
   energy_Xrays_.push_back(1.383*keV); 
   probability_Xrays_.push_back(0.01*0.099);    
   energy_Xrays_.push_back(1.435*keV);  
   probability_Xrays_.push_back(0.01*0.060 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(1.580*keV);   	
   probability_Xrays_.push_back(0.01*0.21 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(1.581*keV);   	
   probability_Xrays_.push_back(0.01*1.9 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(1.632*keV);   	
   probability_Xrays_.push_back(0.01*1.1 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(1.647*keV);   	
   probability_Xrays_.push_back(0.01*0.0094 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(1.699*keV);   	
   probability_Xrays_.push_back(0.01*0.09  + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(1.707*keV);   	
   probability_Xrays_.push_back(0.01*0.14 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(1.906*keV);   	
   probability_Xrays_.push_back(0.01*0.008 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(1.907*keV);   	
   probability_Xrays_.push_back(0.01*0.025 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(12.405*keV);   	
   probability_Xrays_.push_back(0.01*3.90E-05 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(12.598*keV);   	
   probability_Xrays_.push_back(0.01*5.05 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(12.651*keV);   	
   probability_Xrays_.push_back(0.01*9.8 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(14.104*keV);   	
   probability_Xrays_.push_back(0.01*0.70 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(14.111*keV);   	
   probability_Xrays_.push_back(0.01*1.36  + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(14.231*keV);   	
   probability_Xrays_.push_back(0.01*0.00429 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(14.311*keV);   	
   probability_Xrays_.push_back(0.01*0.179 + probability_Xrays_[probability_Xrays_.size() -1]);
   energy_Xrays_.push_back(14.326*keV);   	
   probability_Xrays_.push_back(0.01*0.0064 + probability_Xrays_[probability_Xrays_.size() -1]);
   
   std::cerr << " Kr83Generator::Kr83Generator, probability to emit an X-ray " 
             << probability_Xrays_[probability_Xrays_.size() -1]*100. << " percent " << std::endl;
    /// For the moment, only random direction are allowed. 
    // Since the transion are either E3, M4 (32 keV), E2, M1, (9 keV), 
    // no strong asymmetry to start with.. 
    //
     msg_ = new G4GenericMessenger(this, "/Generator/Kr83mGenerator/",
    "Control commands of Kr83 generator.");

     msg_->DeclareProperty("region", region_, 
			   "Set the region of the geometry where the vertex will be generated.");

     // Set particle type searching in particle table by name
    particle_defgamma_ = G4ParticleTable::GetParticleTable()->
      FindParticle("gamma");
    particle_defelectron_ = G4ParticleTable::GetParticleTable()->
      FindParticle("e-");

    DetectorConstruction* detconst = (DetectorConstruction*)
      G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    geom_ = detconst->GetGeometry();
    //
    // to debug possible problem with paucity of X-ray from the 32 kEV line..
    // May 2 
//    fOutCheckKr83mTmp.open("./testK484m_1.txt");
//    fOutCheckKr83mTmp << " Evt key ID e " << std::endl;
// bug in selecting the ad-hoc decay mode.. 
//
  }

  Kr83mGenerator::~Kr83mGenerator()
  {
  }

  void Kr83mGenerator::GeneratePrimaryVertex(G4Event* evt)
  {
    // Add an Ascci ntuple to debug.. 
   // const int evtNum = evt->GetEventID();
    
    // Ask the geometry to generate a position for the particle
    G4ThreeVector position = geom_->GenerateVertex(region_);
   //
   // First transition (32 kEv) Always one electron. Set it's kinetic energy. 
   // Decide if we emit an X-ray.. 
   //
   
    const double probXRay = G4UniformRand();
    double eKin32 = energy_32_;
    double eXray = 0.;
    if (probXRay < probability_Xrays_[probability_Xrays_.size() -1]) {
      size_t kSel = probability_Xrays_.size() - 1;
      for (size_t k=1; k!= probability_Xrays_.size(); k++) {
        if ((probXRay >= probability_Xrays_[k-1]) && (probXRay < probability_Xrays_[k])) {
          kSel = k-1;
	  break;
        } 
      }
      eKin32 = energy_32_ - energy_Xrays_[kSel];
      eXray = energy_Xrays_[kSel];
    }
   
    G4double mass = particle_defelectron_->GetPDGMass();
   // Calculate cartesian components of momentum for the most energetic EC 
    G4double energy = eKin32 + mass;
    G4double pmod = std::sqrt(energy*energy - mass*mass);
    G4ThreeVector momentum_dir32 = G4RandomDirection();
    G4double px = pmod * momentum_dir32.x();
    G4double py = pmod * momentum_dir32.y();
    G4double pz = pmod * momentum_dir32.z();
    // Set starting time of generation
    G4double time = 0;

    G4PrimaryVertex* vertex = 
      new G4PrimaryVertex(position, time);

    // create new primaries and set them to the vertex
    
    G4PrimaryParticle* particle1 =
      new G4PrimaryParticle(particle_defelectron_);
    particle1->SetMomentum(px, py, pz);
    particle1->SetPolarization(0.,0.,0.);
    particle1->SetProperTime(time);
    vertex->SetPrimary(particle1);
//    fOutCheckKr83mTmp << " " << evtNum << " 32  11 " << eKin32 << std::endl;  
    if (eXray > (0.0001*keV)) {
      pmod = eXray;
      G4ThreeVector momentum_dirXr = G4RandomDirection();
      px = pmod * momentum_dirXr.x();
      py = pmod * momentum_dirXr.y();
      pz = pmod * momentum_dirXr.z();
      G4PrimaryParticle* particle2 = new G4PrimaryParticle(particle_defgamma_);
      particle2->SetMomentum(px, py, pz);
      particle2->SetPolarization(0.,0.,0.);
      particle2->SetProperTime(time);
      vertex->SetPrimary(particle2);
//      fOutCheckKr83mTmp << " " << evtNum << " 320  22 " << eXray << std::endl;  
    }
    // 
    // set the second gamma. Decide first if we have a 9.4 keV gamma, 
    // on an EC 
    //
    const double probGam9 = G4UniformRand();
    G4ThreeVector momentum_dir9 = G4RandomDirection();
    //
    // finite lifetime for the 9 keV line
    //
    const double time9 = lifetime_9_ * G4RandExponential::shoot();
    //   
    if (probGam9 < probGamma_9_) { // a soft gamma (i.e., an X-ray.. )
      energy = energy_9_;
      pmod = energy;
      px = pmod * momentum_dir9.x();
      py = pmod * momentum_dir9.y();
      pz = pmod * momentum_dir9.z();
      G4PrimaryParticle* particle3 = new G4PrimaryParticle(particle_defgamma_);
      particle3->SetMomentum(px, py, pz);
      particle3->SetPolarization(0.,0.,0.);
      particle3->SetProperTime(time9);
      vertex->SetPrimary(particle3);
//      fOutCheckKr83mTmp << " " << evtNum << " 9  22 " << energy_9_ << std::endl;  
    } else { // a soft electron. (Electron Conversion )    
     // Calculate cartesian components of momentum for the most energetic EC 
      energy = energy_9_ + mass;
      pmod = std::sqrt(energy*energy - mass*mass);
      px = pmod * momentum_dir9.x();
      py = pmod * momentum_dir9.y();
      pz = pmod * momentum_dir9.z();
      G4PrimaryParticle* particle3 =
       new G4PrimaryParticle(particle_defelectron_);
      particle3->SetMomentum(px, py, pz);
      particle3->SetProperTime(time9);
      vertex->SetPrimary(particle3);
//      fOutCheckKr83mTmp << " " << evtNum << " 9  11 " << energy_9_ << std::endl;  
   }   
   evt->AddPrimaryVertex(vertex);
  }
} // Name space nexus 
