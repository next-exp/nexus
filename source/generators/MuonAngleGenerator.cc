// ----------------------------------------------------------------------------
// nexus | MuonAngleGenerator.cc
//
// This class is the primary generator of muons following an angular
// distribution measured in the LSC.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "MuonAngleGenerator.h"
#include "DetectorConstruction.h"
#include "GeometryBase.h"
#include "MuonsPointSampler.h"
#include "AddUserInfoToPV.h"
#include "FactoryBase.h"

#include <G4Event.hh>
#include <G4GenericMessenger.hh>
#include <G4ParticleDefinition.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>
#include <G4OpticalPhoton.hh>

#include <TMath.h>
#include <TFile.h>
#include <TH2F.h>

#include "CLHEP/Units/SystemOfUnits.h"

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(MuonAngleGenerator, G4VPrimaryGenerator)

MuonAngleGenerator::MuonAngleGenerator():
  G4VPrimaryGenerator(), msg_(0), particle_definition_(0),
  angular_generation_(true), rPhi_(NULL), energy_min_(0.),
  energy_max_(0.), distribution_(0), geom_(0), geom_solid_(0)
{
  msg_ = new G4GenericMessenger(this, "/Generator/MuonAngleGenerator/",
				"Control commands of muongenerator.");

  G4GenericMessenger::Command& min_energy =
    msg_->DeclareProperty("min_energy", energy_min_, "Set minimum kinetic energy of the particle.");
  min_energy.SetUnitCategory("Energy");
  min_energy.SetParameterName("min_energy", false);
  min_energy.SetRange("min_energy>0.");

  G4GenericMessenger::Command& max_energy =
    msg_->DeclareProperty("max_energy", energy_max_, "Set maximum kinetic energy of the particle");
  max_energy.SetUnitCategory("Energy");
  max_energy.SetParameterName("max_energy", false);
  max_energy.SetRange("max_energy>0.");

  msg_->DeclareProperty("region", region_,
			"Set the region of the geometry where the vertex will be generated.");

  msg_->DeclareProperty("angles_on", angular_generation_,
			"Distribute muon directions according to file?");

  msg_->DeclareProperty("angle_file", ang_file_,
			"Name of the file containing angular distribution.");
  msg_->DeclareProperty("angle_dist", dist_name_,
			"Name of the angular distribution histogram.");

  G4GenericMessenger::Command& rotation =
    msg_->DeclareProperty("azimuth_rotation", axis_rotation_,
			  "Angle between north and nexus z in anticlockwise");
  rotation.SetUnitCategory("Angle");
  rotation.SetParameterName("azimuth", false);
  rotation.SetRange("azimuth>0.");

  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();

}


MuonAngleGenerator::~MuonAngleGenerator()
{

  delete msg_;
}


void MuonAngleGenerator::SetupAngles()
{
  // Rotation from the axes used in file.
  // Rotates anticlockwise about Y.
  rPhi_ = new G4RotationMatrix();
  rPhi_->rotateY(-axis_rotation_);

  // Get the Angular distribution from file.
  TFile angle_file(ang_file_);
  angle_file.GetObject(dist_name_, distribution_);
  distribution_->SetDirectory(0);
  angle_file.Close();

  // Get the solid to check overlap
  geom_solid_ =
    geom_->GetLogicalVolume()->GetDaughter(0)->GetLogicalVolume()->GetSolid();

}


void MuonAngleGenerator::GeneratePrimaryVertex(G4Event* event)
{

  if (angular_generation_ && rPhi_ == NULL)
    SetupAngles();

  particle_definition_ =
    G4ParticleTable::GetParticleTable()->FindParticle(MuonCharge());
  if (!particle_definition_)
    G4Exception("[MuonAngleGenerator]", "SetParticleDefinition()",
                FatalException, " can not create a muon ");

  // Generate uniform random energy in [E_min, E_max]
  G4double kinetic_energy = RandomEnergy();

  // Particle propierties
  G4double mass   = particle_definition_->GetPDGMass();
  G4double energy = kinetic_energy + mass;
  G4double pmod   = std::sqrt(energy*energy - mass*mass);

  G4ThreeVector position = geom_->GenerateVertex(region_);
  G4ThreeVector p_dir(0., -1., 0.);
  if (angular_generation_){
    GetDirection(p_dir);
    while ( !CheckOverlap(position, p_dir) )
      position = geom_->GenerateVertex(region_);
  }

  G4double px = pmod * p_dir.x();
  G4double py = pmod * p_dir.y();
  G4double pz = pmod * p_dir.z();

  // Particle generated at start-of-event
  G4double time = 0.;
  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);

  // Create the new primary particle and set it some properties
  G4PrimaryParticle* particle =
    new G4PrimaryParticle(particle_definition_, px, py, pz);

  // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle);
  event->AddPrimaryVertex(vertex);
}


G4double MuonAngleGenerator::RandomEnergy() const
{
  if (energy_max_ == energy_min_)
    return energy_min_;
  else
    return (G4UniformRand()*(energy_max_ - energy_min_) + energy_min_);
}


G4String MuonAngleGenerator::MuonCharge() const
{
  G4double rndCh = 2.3 *G4UniformRand(); //From PDG cosmic muons  mu+/mu- = 1.3
  if (rndCh <1.3)
    return "mu+";
  else
    return "mu-";
}


void MuonAngleGenerator::GetDirection(G4ThreeVector& dir)
{
  // GetAngles from file?? Azimuth defined anticlockwise
  // From north
  G4double zenith  = 0.;
  G4double azimuth = 0.;
  distribution_->GetRandom2(azimuth, zenith);
  // !! Current distribution in units of pi
  zenith  *= pi;
  azimuth *= pi;

  dir.setX(sin(zenith) * sin(azimuth));
  dir.setY(-cos(zenith));
  dir.setZ(-sin(zenith) * cos(azimuth));

  dir *= *rPhi_;
}


G4bool MuonAngleGenerator::CheckOverlap(const G4ThreeVector& vtx,
					const G4ThreeVector& dir)
{
  // Check for overlap between generated vertex+direction
  // and the geometry.

  if (geom_solid_->DistanceToIn(vtx, dir) == kInfinity)
    return false;

  return true;
}
