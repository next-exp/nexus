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

#include "CLHEP/Units/SystemOfUnits.h"

using namespace nexus;

REGISTER_CLASS(MuonAngleGenerator, G4VPrimaryGenerator)

MuonAngleGenerator::MuonAngleGenerator():
  G4VPrimaryGenerator(), msg_(0), particle_definition_(0),
  angular_generation_(true), rPhi_(NULL), energy_min_(0.),
  energy_max_(0.), dist_name_("za"), bInitialize_(false), geom_(0), geom_solid_(0)
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

void MuonAngleGenerator::LoadMuonDistribution()
{

  // File pointer
  std::ifstream fin(ang_file_);

  // Check if file has opened properly
  if (!fin.is_open() && angular_generation_){
    G4Exception("[MuonAngleGenerator]", "LoadMuonDistribution()",
                FatalException, " could not read in the input muon distributions from CSV file ");
  }

  // Check the input filename for the keyword Energy
  size_t found = ang_file_.find("Energy");
  if (found!=std::string::npos){

    // Only angle option, but energy specified
    if (dist_name_ == "za")
      G4Exception("[MuonAngleGenerator]", "LoadMuonDistribution()",
                FatalException, " Angular + Energy file specified with angle_dist=za option selected, use angle_dist=zae ");
  }
  // File name does not contain the word Energy
  else {
    if (dist_name_ == "zae")
      G4Exception("[MuonAngleGenerator]", "LoadMuonDistribution()",
                FatalException, " Angular file specified with angle_dist=zae option selected, use angle_dist=za ");
  }

  // Read the Data from the file as strings
  std::string s_header, s_flux, s_azimuth, s_zenith, s_energy;
  std::string s_azimuth_smear, s_zenith_smear, s_energy_smear;

  // Loop over the lines in the file and add the values to a vector
  while (fin.peek()!=EOF) {

    std::getline(fin, s_header, ',');

    // Angle input only
    if (s_header == "value" && dist_name_ == "za"){
      std::getline(fin, s_flux, ',');
      std::getline(fin, s_azimuth, ',');
      std::getline(fin, s_zenith, ',');
      std::getline(fin, s_azimuth_smear, ',');
      std::getline(fin, s_zenith_smear, '\n');

      flux_.push_back(stod(s_flux));
      azimuths_.push_back(stod(s_azimuth));
      zeniths_.push_back(stod(s_zenith));
      azimuth_smear_.push_back(stod(s_azimuth_smear));
      zenith_smear_.push_back(stod(s_zenith_smear));
    }
    // Angle + Energy input
    if (s_header == "value" && dist_name_ == "zae"){
      std::getline(fin, s_flux, ',');
      std::getline(fin, s_azimuth, ',');
      std::getline(fin, s_zenith, ',');
      std::getline(fin, s_energy, ',');
      std::getline(fin, s_azimuth_smear, ',');
      std::getline(fin, s_zenith_smear, ',');
      std::getline(fin, s_energy_smear, '\n');

      flux_.push_back(stod(s_flux));
      azimuths_.push_back(stod(s_azimuth));
      zeniths_.push_back(stod(s_zenith));
      energies_.push_back(stod(s_energy));
      azimuth_smear_.push_back(stod(s_azimuth_smear));
      zenith_smear_.push_back(stod(s_zenith_smear));
      energy_smear_.push_back(stod(s_energy_smear));
    }

  }


  // Convert flux vector to arr
  G4double arr_flux[flux_.size()];
  std::copy(flux_.begin(), flux_.end(), arr_flux);

  // Initialise the Random Number Generator based on the flux distribution (in bin index)
  fRandomGeneral_ = new G4RandGeneral( arr_flux, flux_.size() );

}

void MuonAngleGenerator::SetupAngles()
{
  // Rotation from the axes used in file.
  // Rotates anticlockwise about Y.
  rPhi_ = new G4RotationMatrix();
  rPhi_->rotateY(-axis_rotation_);
}


void MuonAngleGenerator::GeneratePrimaryVertex(G4Event* event)
{

  // Initalise RNG seeds and load file on the first event
  if (!bInitialize_){

    // Load in the Muon angular distribution from file
    if (angular_generation_)
      LoadMuonDistribution();

    // Set Initialisation
    bInitialize_ = true;

  }

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

  G4ThreeVector position = geom_->GenerateVertex(region_);

  // Set default momentum and angular variables
  G4ThreeVector p_dir(0., -1., 0.);
  G4double zenith  = p_dir.getTheta();
  G4double azimuth = p_dir.getPhi() + pi; // factor pi to ensure range from 0.->2pi

  // Overwrite default p_dir, zenith and azimuth from angular distribution file
  if (angular_generation_){
    GetDirection(p_dir, zenith, azimuth, energy, kinetic_energy, mass);
    position = geom_->GenerateVertex(region_);
  }

  G4double pmod   = std::sqrt(energy*energy - mass*mass);
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

  // Add info to PrimaryVertex to be accessed from EventAction type class to make histos of variables generated here.
  AddUserInfoToPV *info = new AddUserInfoToPV(zenith, azimuth);

  vertex->SetUserInformation(info);

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


void MuonAngleGenerator::GetDirection(G4ThreeVector& dir, G4double& zenith, G4double& azimuth,
                      G4double& energy, G4double& kinetic_energy, G4double mass)
{

  // Bool to check if zenith has a valid value. If not then resample
  G4bool invalid_evt = true;

  while(invalid_evt){

    // Generate random index weighted by the bin contents
    // Scale by flux vec size, then round to nearest integer to get an index
    G4int RN_indx = round(fRandomGeneral_->fire()*flux_.size());

    // Correct sampled values by Gaussian smearing
    azimuth  = azimuths_[RN_indx] + G4RandGauss::shoot( 0., azimuth_smear_[RN_indx]);
    zenith   = zeniths_[RN_indx]  + G4RandGauss::shoot( 0., zenith_smear_[RN_indx]);

    // Sample and update the energy if angle + energy option specified
    if (dist_name_ == "zae"){
      energy   = (energies_[RN_indx]*GeV  + G4RandGauss::shoot( 0., energy_smear_[RN_indx]*GeV));
      kinetic_energy = energy - mass;

      // Resample if the energy is not in the specified range
      if (energy < energy_min_ || energy > energy_max_){
        invalid_evt = true;
        continue;
      }
    }

    // Catch negative value and resample if so
    if (zenith < 0.0)
        invalid_evt = true;
    else
        invalid_evt = false;

    // Calculate the vector components of the muon
    dir.setX(sin(zenith) * sin(azimuth));
    dir.setY(-cos(zenith));
    dir.setZ(-sin(zenith) * cos(azimuth));

    // Rotate about the Y-Axis
    dir *= *rPhi_;

  }

}
