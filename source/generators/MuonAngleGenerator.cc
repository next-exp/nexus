#include "MuonAngleGenerator.h"
#include "DetectorConstruction.h"
#include "BaseGeometry.h"
#include <G4GenericMessenger.hh>
#include <G4ParticleDefinition.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>
#include <G4OpticalPhoton.hh>
#include "MuonsPointSampler.h"
#include "AddUserInfoToPV.h"

#include <TMath.h>
#include "TFile.h"
#include "CLHEP/Units/SystemOfUnits.h"

using namespace nexus;
using namespace CLHEP;


MuonAngleGenerator::MuonAngleGenerator():
  G4VPrimaryGenerator(), _msg(0), _particle_definition(0),
  _angular_generation(true), _rPhi(NULL), _energy_min(0.),
  _energy_max(0.), _distribution(0), _geom(0), _geom_solid(0)
{
  _msg = new G4GenericMessenger(this, "/Generator/MuonAngleGenerator/",
				"Control commands of muongenerator.");

  G4GenericMessenger::Command& min_energy =
    _msg->DeclareProperty("min_energy", _energy_min, "Set minimum kinetic energy of the particle.");
  min_energy.SetUnitCategory("Energy");
  min_energy.SetParameterName("min_energy", false);
  min_energy.SetRange("min_energy>0.");

  G4GenericMessenger::Command& max_energy =
    _msg->DeclareProperty("max_energy", _energy_max, "Set maximum kinetic energy of the particle");
  max_energy.SetUnitCategory("Energy");
  max_energy.SetParameterName("max_energy", false);
  max_energy.SetRange("max_energy>0.");

  _msg->DeclareProperty("region", _region,
			"Set the region of the geometry where the vertex will be generated.");

  _msg->DeclareProperty("angles_on", _angular_generation,
			"Distribute muon directions according to file?");

  _msg->DeclareProperty("angle_file", _ang_file,
			"Name of the file containing angular distribution.");
  _msg->DeclareProperty("angle_dist", _dist_name,
			"Name of the angular distribution histogram.");
  
  G4GenericMessenger::Command& rotation =
    _msg->DeclareProperty("azimuth_rotation", _axis_rotation,
			  "Angle between north and nexus z in anticlockwise");
  rotation.SetUnitCategory("Angle");
  rotation.SetParameterName("azimuth", false);
  rotation.SetRange("azimuth>0.");

  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  _geom = detconst->GetGeometry();

}


MuonAngleGenerator::~MuonAngleGenerator()
{

  delete _msg;
}


void MuonAngleGenerator::SetupAngles()
{
  // Rotation from the axes used in file.
  // Rotates anticlockwise about Y.
  _rPhi = new G4RotationMatrix();
  _rPhi->rotateY(-_axis_rotation);

  // Get the Angular distribution from file.
  TFile angle_file(_ang_file);
  angle_file.GetObject(_dist_name, _distribution);
  _distribution->SetDirectory(0);
  angle_file.Close();

  // Get the solid to check overlap
  _geom_solid =
    _geom->GetLogicalVolume()->GetDaughter(0)->GetLogicalVolume()->GetSolid();
  
}


void MuonAngleGenerator::GeneratePrimaryVertex(G4Event* event)
{

  if (_angular_generation && _rPhi == NULL)
    SetupAngles();

  _particle_definition =
    G4ParticleTable::GetParticleTable()->FindParticle(MuonCharge());
  if (!_particle_definition)
    G4Exception("SetParticleDefinition()", "[MuonAngleGenerator]",
                FatalException, " can not create a muon ");

  // Generate uniform random energy in [E_min, E_max]
  G4double kinetic_energy = RandomEnergy();

  // Particle propierties
  G4double mass   = _particle_definition->GetPDGMass();
  G4double energy = kinetic_energy + mass;
  G4double pmod   = std::sqrt(energy*energy - mass*mass);

  G4ThreeVector position = _geom->GenerateVertex(_region);
  G4ThreeVector p_dir(0., -1., 0.);
  if (_angular_generation){
    GetDirection(p_dir);
    while ( !CheckOverlap(position, p_dir) )
      position = _geom->GenerateVertex(_region);
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
    new G4PrimaryParticle(_particle_definition, px, py, pz);

  // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle);
  event->AddPrimaryVertex(vertex);
}


G4double MuonAngleGenerator::RandomEnergy() const
{
  if (_energy_max == _energy_min)
    return _energy_min;
  else
    return (G4UniformRand()*(_energy_max - _energy_min) + _energy_min);
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
  _distribution->GetRandom2(azimuth, zenith);
  // !! Current distribution in units of pi
  zenith  *= pi;
  azimuth *= pi;

  dir.setX(sin(zenith) * sin(azimuth));
  dir.setY(-cos(zenith));
  dir.setZ(-sin(zenith) * cos(azimuth));

  dir *= *_rPhi;
}


G4bool MuonAngleGenerator::CheckOverlap(const G4ThreeVector& vtx,
					const G4ThreeVector& dir)
{
  // Check for overlap between generated vertex+direction
  // and the geometry.

  if (_geom_solid->DistanceToIn(vtx, dir) == kInfinity)
    return false;

  return true;
}
