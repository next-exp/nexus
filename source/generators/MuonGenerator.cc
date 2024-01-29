// ----------------------------------------------------------------------------
// nexus | MuonGenerator.cc
//
// This class is the primary generator of muons
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "MuonGenerator.h"
#include "DetectorConstruction.h"
#include "GeometryBase.h"
#include "AddUserInfoToPV.h"
#include "FactoryBase.h"
#include "RandomUtils.h"
#include "IOUtils.h"

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

REGISTER_CLASS(MuonGenerator, G4VPrimaryGenerator)

MuonGenerator::MuonGenerator():
  G4VPrimaryGenerator(), msg_(0), particle_definition_(0),
  use_lsc_dist_(true), axis_rotation_(150), rPhi_(NULL), user_dir_{},
  energy_min_(0.), energy_max_(0.), dist_name_("za"), bInitialize_(false),
  geom_(0), geom_solid_(0), gen_rad_(223.33*cm)
{
  msg_ = new G4GenericMessenger(this, "/Generator/MuonGenerator/",
				"Control commands of muongenerator.");

  G4GenericMessenger::Command& min_energy =
    msg_->DeclareProperty("min_energy", energy_min_, "Minimum kinetic energy of the particle.");
  min_energy.SetUnitCategory("Energy");
  min_energy.SetParameterName("min_energy", false);
  min_energy.SetRange("min_energy>0.");

  G4GenericMessenger::Command& max_energy =
    msg_->DeclareProperty("max_energy", energy_max_, "Maximum kinetic energy of the particle");
  max_energy.SetUnitCategory("Energy");
  max_energy.SetParameterName("max_energy", false);
  max_energy.SetRange("max_energy>0.");

  msg_->DeclareProperty("region", region_,
                        "Region of the geometry where the vertex will be generated.");

  msg_->DeclareProperty("use_lsc_dist", use_lsc_dist_,
			"Distribute muon directions according to file?");

  msg_->DeclareProperty("angle_file", ang_file_,
			"Name of the file containing angular distribution.");
  msg_->DeclareProperty("angle_dist", dist_name_,
			"Name of the angular distribution histogram.");

  msg_->DeclareProperty("user_dir",  user_dir_, "Set fixed muon direction.");

  G4GenericMessenger::Command& rotation =
    msg_->DeclareProperty("azimuth_rotation", axis_rotation_,
			  "Angle between north and nexus z in anticlockwise");
  rotation.SetUnitCategory("Angle");
  rotation.SetParameterName("azimuth", false);
  rotation.SetRange("azimuth>0.");

  // defaults to length of the NEXT100 shielding diagonal if not present.
  G4GenericMessenger::Command& generation_radius =
    msg_->DeclareProperty("gen_rad", gen_rad_, "Set radius for generation disc");
  generation_radius.SetUnitCategory("Length");
  generation_radius.SetParameterName("gen_rad", false);
  generation_radius.SetRange("gen_rad>0.");

  DetectorConstruction* detconst =
    (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();

}


MuonGenerator::~MuonGenerator()
{
  delete msg_;
}

void MuonGenerator::LoadMuonDistribution()
{
  

  // Check the input filename for the keyword Energy
  size_t found = ang_file_.find("Energy");
  if (found!=std::string::npos){

    // Only angle option, but energy specified
    if (dist_name_ == "za")
      G4Exception("[MuonGenerator]", "LoadMuonDistribution()",
                  FatalException, " Angular + Energy file specified "
                  "with angle_dist=za option selected, use angle_dist=zae ");
  }
  // File name does not contain the word Energy
  else {
    if (dist_name_ == "zae")
      G4Exception("[MuonGenerator]", "LoadMuonDistribution()",
                  FatalException, " Angular file specified "
                  "with angle_dist=zae option selected, use angle_dist=za ");
  }

  // Load in the data from csv file depending on 2D histogram sampling or 3D
  if (dist_name_ == "za")
    LoadHistData2D(ang_file_, flux_, azimuths_, zeniths_, azimuth_smear_, zenith_smear_);

  if (dist_name_ == "zae"){
    LoadHistData3D(ang_file_, flux_, azimuths_, zeniths_, energies_,
                   azimuth_smear_, zenith_smear_, energy_smear_);
    
    // Check if the energy is in the desired range permitted
    // by the binning range in the data file
    CheckVarBounds(ang_file_, energy_min_/GeV, energy_max_/GeV, "energy"); 

  }

  // Convert flux vector to arr
  auto arr_flux = flux_.data();

  // Initialise the Random Number Generator based on the flux distribution
  // (in bin index)
  fRandomGeneral_ = new G4RandGeneral( arr_flux, flux_.size() );

}

void MuonGenerator::InitMuonZenithDist()
{

  // Create a vector of values finely spaced from 0 -> pi/2
  // Then take cos(x)*cos(x) to make a dist to sample from
  std::vector<G4double> v_angles;

  for (G4double i = 0; i <= pi/2; i+=0.0001){
      v_angles.push_back(std::cos(i)*std::cos(i));
  }

  // Convert vector to arr
  auto arr_ang = v_angles.data();

  // Initialise the Random Number Generator based on cos(x)*cos(x) distribution
  fRandomGeneral_ = new G4RandGeneral( arr_ang, v_angles.size() );

}

void MuonGenerator::SetupAngles()
{
  // Rotation from the axes used in file.
  // Rotates anticlockwise about Y.
  rPhi_ = new G4RotationMatrix();
  rPhi_->rotateY(-axis_rotation_);
}


void MuonGenerator::GeneratePrimaryVertex(G4Event* event)
{

  // Initalise RNG seeds and load file on the first event
  if (!bInitialize_){

    // Load in the Muon angular distribution from file
    if (use_lsc_dist_){
      std::cout << "[MuonGenerator]: Generating muons using lsc distribution loaded from file" << std::endl;
      LoadMuonDistribution();

      // Throw exception if a user dir given
      // and also the use_lsc_dist_ is set to true
      if (user_dir_ != G4ThreeVector{})
        G4Exception("[MuonGenerator]", "GeneratePrimaryVertex()",
                    FatalException, " Fixed user direction specified with "
                    "use_lsc_dist set to true. Set use_lsc_dist to "
                    "false or remove user_dir in the config");

    }
    
    // Initalise a cos^2 distribution to sample the zenith
    if (!use_lsc_dist_ && (user_dir_ == G4ThreeVector{})){
      std::cout << "[MuonGenerator]: Generating muons with uniform azimuth "
                   "and cos^2 distribution for zenith " << std::endl;
      InitMuonZenithDist();
    }

    // User specified muon direction
    if (!use_lsc_dist_ && (user_dir_ != G4ThreeVector{})){
      std::cout << "[MuonGenerator]: Generating muons with user specified "
                   "direction " << std::endl;
    }

    // Set Initialisation
    bInitialize_ = true;

  }

  // Init the rotation matrix
  if (rPhi_ == NULL)
    SetupAngles();

  particle_definition_ =
    G4ParticleTable::GetParticleTable()->FindParticle(MuonCharge());

  if (!particle_definition_)
    G4Exception("[MuonGenerator]", "SetParticleDefinition()",
                FatalException, " can not create a muon ");

  // Generate uniform random energy in [E_min, E_max]
  G4double kinetic_energy = UniformRandomInRange(energy_max_, energy_min_);

  // Particle properties
  G4double mass          = particle_definition_->GetPDGMass();
  G4double energy        = kinetic_energy + mass;
  //G4ThreeVector position = geom_->GenerateVertex(region_);

  // Set default momentum and angular variables
  G4ThreeVector p_dir;
  G4double zenith;
  G4double azimuth;

  // Momentum, zenith, azimuth (and energy) from angular distribution file
  if (use_lsc_dist_){
    GetDirection(p_dir, zenith, azimuth, energy, kinetic_energy, mass);
    //    position = geom_->GenerateVertex(region_);
  }
  else {

    // User specified muon direction in some fixed direction
    if ( user_dir_ != G4ThreeVector{}) {
      p_dir   = user_dir_.unit();
      zenith  = p_dir.getTheta();
      azimuth = p_dir.getPhi() + pi; // change azimuth interval to be between 0, twopi
    }

    // Sample direction via cos^2 distribution for zenith, uniform azimuth
    else {
      zenith  = GetZenith();
      azimuth = GetAzimuth(); // Returns from 0 to 2pi

      // Calculate the vector components of the muon
      p_dir.setX(sin(zenith) * sin(azimuth));
      p_dir.setY(-cos(zenith));
      p_dir.setZ(-sin(zenith) * cos(azimuth));

      // Rotate about the Y-Axis
      p_dir *= *rPhi_;

    }
  }

  G4ThreeVector position;
  if ((region_ == "HALLA_INNER") || (region_ == "HALLA_OUTER")) {
    position = ProjectToVertex(p_dir);
  } else {
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

  // Add info to PrimaryVertex to be accessed from EventAction type class
  // to make histos of variables generated here.
  AddUserInfoToPV *info = new AddUserInfoToPV(zenith, azimuth);

  vertex->SetUserInformation(info);

  // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle);
  event->AddPrimaryVertex(vertex);
}


G4String MuonGenerator::MuonCharge() const
{

  // Ratio of Mu+/Mu- is energy dependent, ranges from 1.3 to 1.5:
  // https://arxiv.org/pdf/1111.6675.pdf
  // Assume (approx) flat ratio up to energy range of interest ~6 TeV 
  // mu+/mu- ~1.3

  // Sample random number to give 1.3 to 1 ratio of Mu+/Mu-
  G4double rndCh = 2.3 * G4UniformRand(); 
  if (rndCh < 1.3)
    return "mu+";
  else
    return "mu-";
}


void MuonGenerator::GetDirection(G4ThreeVector& dir, G4double& zenith,
                                 G4double& azimuth, G4double& energy,
                                 G4double& kinetic_energy, G4double mass)
{

  // Bool to check if zenith has a valid value. If not then resample
  G4bool invalid_evt = true;

  while(invalid_evt){

    // Generate random index weighted by the bin contents
    // Scale by flux vec size, then round to nearest integer to get an index
    G4int RN_indx = GetRandBinIndex(fRandomGeneral_, flux_);

    // Correct sampled values by Gaussian smearing
    azimuth = Sample(azimuths_[RN_indx], true, azimuth_smear_[RN_indx]);
    zenith  = Sample(zeniths_[RN_indx],  true, zenith_smear_[RN_indx]);

    // Sample and update the energy if angle + energy option specified
    if (dist_name_ == "zae"){
      energy = Sample(energies_[RN_indx]*GeV, true, energy_smear_[RN_indx]*GeV);
      kinetic_energy = energy - mass;

    }

    // Catch negative value and resample if so
    if (zenith < 0.0){
        invalid_evt = true;
        continue;
    }
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


G4ThreeVector MuonGenerator::ProjectToVertex(const G4ThreeVector& dir)
{
  /////////////////////////////////////////////////////////////////////////
  // This method of vertex generation decides the starting vertex
  // of the primary particle in three steps:
  // 1) A random point is generated on a disc centred on the main
  //    volumes.
  // 2) This disc is rotated so that it is perpendicular to the
  //    direction vector which is the only argument of the function.
  //    The new point is a point through which the vector passes.
  // 3) The vertex is found by projecting backwards along the direction
  //    vector from that point to find the point where the ray
  //    (point - t*dir) intersects with the region configured as the
  //    starting point for all vertices.
  /////////////////////////////////////////////////////////////////////////
  // Postion in disc
  G4double radius = gen_rad_ * std::sqrt(G4UniformRand());
  G4double ang = 2 * G4UniformRand() * pi;

  // Continue assuming that Y is vertical and z drift,
  // valid for NEW and NEXT-100 (at least).
  // Rotate the disc (origin-point vector) to be perpendicular to dir.
  G4ThreeVector point(radius * std::cos(ang), 0., radius * std::sin(ang));
  point.rotate(pi / 2 - dir.angle(point), dir.cross(point));

  // Now project back to the requested region intersection.
  return geom_->ProjectToRegion(region_, point, -dir);
}


G4double MuonGenerator::GetZenith() const
{
  return fRandomGeneral_->fire()*pi/2;
}


G4double MuonGenerator::GetAzimuth() const
{
  return twopi*G4UniformRand();
}
