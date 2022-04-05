// ----------------------------------------------------------------------------
// nexus | MuonGenerator.cc
//
// This class is the primary generator of muons following the angular
// distribution at sea level. Angles are saved to be plotted later.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "MuonGenerator.h"
#include "DetectorConstruction.h"
#include "GeometryBase.h"
#include "MuonsPointSampler.h"
#include "AddUserInfoToPV.h"
#include "FactoryBase.h"

#include <G4GenericMessenger.hh>
#include <G4ParticleDefinition.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <G4RandomDirection.hh>


using namespace nexus;

REGISTER_CLASS(MuonGenerator, G4VPrimaryGenerator)


MuonGenerator::MuonGenerator():
  G4VPrimaryGenerator(), msg_(0), particle_definition_(0),
  energy_min_(0.), energy_max_(0.), geom_(0), momentum_{}
{
  msg_ = new G4GenericMessenger(this, "/Generator/MuonGenerator/",
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

  msg_->DeclarePropertyWithUnit("momentum", "mm",  momentum_,
    "Set particle 3-momentum.");


  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();

  // Create a vector of values finely spaced from 0 -> pi/2
  // Then take cos(x)*cos(x) to make a dist to sample from
  std::vector<G4double> v_angles;

  for (G4double i = 0; i <= pi/2; i+=0.0001){
      v_angles.push_back(std::cos(i)*std::cos(i));
  }

  // Convert vector to arr
  G4double arr_ang[v_angles.size()];
  std::copy(v_angles.begin(), v_angles.end(), arr_ang);

  // Initialise the Random Number Generator based on cos(x)*cos(x) distribution
  fRandomGeneral_ = new G4RandGeneral( arr_ang, v_angles.size() );

}


MuonGenerator::~MuonGenerator()
{

  delete msg_;
}

void MuonGenerator::GeneratePrimaryVertex(G4Event* event)
{
  particle_definition_ = G4ParticleTable::GetParticleTable()->FindParticle(MuonCharge());
  if (!particle_definition_)
    G4Exception("[MuonGenerator]", "SetParticleDefinition()",
                FatalException, " can not create a muon ");

  // Generate an initial position for the particle using the geometry
  G4ThreeVector position = geom_->GenerateVertex(region_);
  // Particle generated at start-of-event
  G4double time = 0.;
  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);
  // Generate uniform random energy in [E_min, E_max]
  G4double kinetic_energy = RandomEnergy();

  // Particle propierties
  G4double mass   = particle_definition_->GetPDGMass();
  G4double energy = kinetic_energy + mass;
  G4double pmod = std::sqrt(energy*energy - mass*mass);

  // Generate momentum direction in spherical coordinates
  G4double theta = GetTheta();
  G4double phi   = GetPhi();

  // NEXT axis convention (z<->y) and generate with -y! towards the detector
  G4double x = sin(theta) * cos(phi);
  G4double y = -cos(theta);
  G4double z = sin(theta) * sin(phi);

  G4ThreeVector p_dir(x,y,z);

  bool fixed_momentum = momentum_ != G4ThreeVector{};
  // If user provides a momentum direction, this one is used
  if (fixed_momentum) {
    p_dir = momentum_.unit();
    phi   = p_dir.getPhi() + pi; // change phi interval to be between 0, twopi
    theta = p_dir.getTheta();
  }

  G4ThreeVector p = pmod * p_dir;

  // Create the new primary particle and set it some properties
  G4PrimaryParticle* particle =
    new G4PrimaryParticle(particle_definition_, p.x(), p.y(), p.z());

  // Add info to PrimaryVertex to be accessed from EventAction type class to make histos of variables generated here.
  AddUserInfoToPV *info = new AddUserInfoToPV(theta, phi);

  vertex->SetUserInformation(info);

  // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle);
  event->AddPrimaryVertex(vertex);

}

G4double MuonGenerator::RandomEnergy() const
{
  if (energy_max_ == energy_min_)
    return energy_min_;
  else
    return G4UniformRand()*(energy_max_ - energy_min_) + energy_min_;
}

G4String MuonGenerator::MuonCharge() const
{
  G4double rndCh = 2.3 *G4UniformRand(); //From PDG cosmic muons  mu+/mu- = 1.3
  if (rndCh <1.3)
    return "mu+";
  else
    return "mu-";
}


G4double MuonGenerator::GetTheta() const
{
  G4double theta;
  theta = fRandomGeneral_->fire()*pi/2;
  return theta;
}


G4double MuonGenerator::GetPhi() const
{
  return twopi*G4UniformRand();
}
