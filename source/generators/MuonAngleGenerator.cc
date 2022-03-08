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

#include "CLHEP/Units/SystemOfUnits.h"

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(MuonAngleGenerator, G4VPrimaryGenerator)

MuonAngleGenerator::MuonAngleGenerator():
  G4VPrimaryGenerator(), msg_(0), particle_definition_(0),
  angular_generation_(true), rPhi_(NULL), energy_min_(0.),
  energy_max_(0.), geom_(0), geom_solid_(0)
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

  // Read the Data from the file as strings
  std::string s_flux, s_azimuth, s_zenith;

  // Loop over the lines in the file and add the values to a vector
  while (fin.peek()!=EOF) {

    std::getline(fin, s_flux, ',');

    // Load in zenith bin edges
    if (s_flux == "zenith"){
      std::getline(fin, s_zenith, '\n');
      zenith_bins_.push_back(stod(s_zenith));
    }
    // Load in azimuth bin edges
    else if (s_flux == "azimuth"){
      std::getline(fin, s_azimuth, '\n');
      azimuth_bins_.push_back(stod(s_azimuth));
    }
    // Load in the flux values and positions
    else {
      std::getline(fin, s_azimuth, ',');
      std::getline(fin, s_zenith, '\n');

      flux_.push_back(stod(s_flux));
      azimuths_.push_back(stod(s_azimuth));
      zeniths_.push_back(stod(s_zenith));
    }

  }

  // Calculate and store the bin widths
  az_BW_  = GetBinWidths(azimuth_bins_);
  zen_BW_ = GetBinWidths(zenith_bins_);

  // Discrete distribution with bin index and intensity to sample from
  discr_dist_ = std::discrete_distribution<G4int>(std::begin(flux_), std::end(flux_));

}

std::vector<G4double> MuonAngleGenerator::GetBinWidths(std::vector<G4double> bins)
{

  // Vector of Bin Widths
  std::vector<G4double> BW = {};

  // Loop over and store the bin differences
  for (G4int i = 0; i < bins.size(); i++){
      BW.push_back((bins[i+1] - bins[i]));
  }

  return BW;

}

void MuonAngleGenerator::SetupAngles()
{
  // Rotation from the axes used in file.
  // Rotates anticlockwise about Y.
  rPhi_ = new G4RotationMatrix();
  rPhi_->rotateY(-axis_rotation_);

  // Get the solid to check overlap
  geom_solid_ =
    geom_->GetLogicalVolume()->GetDaughter(0)->GetLogicalVolume()->GetSolid();

}


void MuonAngleGenerator::GeneratePrimaryVertex(G4Event* event)
{

  // Initalise RNG seeds and load file on the first event
  if (event->GetEventID() == 0){ 
    RN_engine_.seed(CLHEP::HepRandom::getTheSeed());
    RN_engine_az_.seed(CLHEP::HepRandom::getTheSeed()+1);   // +1 to keep unique seeds
    RN_engine_zen_.seed(CLHEP::HepRandom::getTheSeed()+2);  // +2 to keep unique seeds

    // Load in the Muon angular distribution from file
    if (angular_generation_)
      LoadMuonDistribution();

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

  // Bool to check if zenith has a valid value. If not then resample
  G4bool invalid_evt = true;

  while(invalid_evt){
  
    // Amount to smear the randomly sampled zenith/azimuth values by
    G4double zen_BW_smear{1.0e6}; 
    G4double az_BW_smear{1.0e6};

    // Generate random index weighted by the bin contents
    G4int RN_indx = discr_dist_(RN_engine_);

    // Loop over the zenith values and find the corresponding bin width to smear
    for (G4int i = 0; i < zenith_bins_.size()-1; i++){
        
        // Catch very last bin
        if (zen_BW_smear == 1e6 && i == zenith_bins_.size()-2){
          if (zeniths_[RN_indx] >= zenith_bins_[i] 
              && zeniths_[RN_indx] <= zenith_bins_[i+1]){
            
              zen_BW_smear = zen_BW_[i];
            }
        }
        else {
          if (zeniths_[RN_indx] >= zenith_bins_[i] 
                && zeniths_[RN_indx] < zenith_bins_[i+1]){
              
              zen_BW_smear = zen_BW_[i];

          }
        }
    
    }

    // Loop over the azimuth values and find the corresponding bin width to smear
    for (G4int i = 0; i < azimuth_bins_.size()-1; i++){
        
        // Include last bin in check
        if (az_BW_smear == 1e6 && i == azimuth_bins_.size()-2){
          if (azimuths_[RN_indx] >= azimuth_bins_[i] 
              && azimuths_[RN_indx] <= azimuth_bins_[i+1]){
            
              az_BW_smear = az_BW_[i];

          }
        }
        else {

          if (azimuths_[RN_indx] >= azimuth_bins_[i] 
                && azimuths_[RN_indx] < azimuth_bins_[i+1]){
              
              az_BW_smear = az_BW_[i];

          }
        }

    }

    // Check if the smear values are set properly
    if (az_BW_smear == 1.0e6 || zen_BW_smear == 1.0e6 )
      G4Exception("[MuonAngleGenerator]", "GetDirection()",
                FatalException, " bin smear value was not set correctly ");

    // Gaussian dist to smear by bin widths in azimuth and zenith
    std::normal_distribution<G4double> Gauss_az(0, az_BW_smear);
    std::normal_distribution<G4double> Gauss_zen(0, zen_BW_smear);
    
    // Get the Gaussian smear values 
    G4double az_smear  = Gauss_az(RN_engine_az_);
    G4double zen_smear = Gauss_zen(RN_engine_zen_);

    // Correct sampled values by smearing
    G4double az_samp  = azimuths_[RN_indx] + az_smear;
    G4double zen_samp = zeniths_[RN_indx]  + zen_smear;

    // Catch negative value and resample if so
    if (zen_samp < 0.0)
        invalid_evt = true;
    else
        invalid_evt = false;

    // Calculate the vector components of the muon
    dir.setX(sin(zen_samp) * sin(az_samp));
    dir.setY(-cos(zen_samp));
    dir.setZ(-sin(zen_samp) * cos(az_samp));

    // Rotate about the Y-Axis
    dir *= *rPhi_;
  
  }

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
