// ----------------------------------------------------------------------------
// nexus | MuonAngleGenerator.h
//
// This class is the primary generator of muons following an angular
// distribution measured in the LSC.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef MUON_ANGLE_GENERATOR_H
#define MUON_ANGLE_GENERATOR_H

#include <G4VPrimaryGenerator.hh>
#include <G4RotationMatrix.hh>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;

class TH2F;


namespace nexus {

  class BaseGeometry;

  class MuonAngleGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    MuonAngleGenerator();
    /// Destructor
    ~MuonAngleGenerator();

    /// This method is invoked at the beginning of the event. It sets
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);

  private:

    // Sets the rotation angle and the spectra to
    // be read for angle generation as well as
    // setting the overlap volume for filtering.
    void SetupAngles();

    /// Generate a random kinetic energy with flat probability in
    //  the interval [energy_min, energy_max].
    G4double RandomEnergy() const;
    G4String MuonCharge() const;

    void GetDirection(G4ThreeVector& dir);

    G4ThreeVector ProjectToVertex(const G4ThreeVector& dir);

  private:
    G4GenericMessenger* msg_;

    G4ParticleDefinition* particle_definition_;

    G4bool angular_generation_; ///< Distribution or all downwards
    G4double axis_rotation_; ///< Angle between North and +z
    G4RotationMatrix *rPhi_; ///< Rotation to adjust axes

    G4double energy_min_; ///< Minimum kinetic energy
    G4double energy_max_; ///< Maximum kinetic energy

    G4String region_; ///< Name of generator region
    G4String ang_file_; ///< Name of file with distributions
    G4String dist_name_; ///< Name of distribution in file
    G4double gen_rad_; ///< Radius of disc for generation

    TH2F * distribution_; ///< Anglular distribution

    const BaseGeometry* geom_; ///< Pointer to the detector geometry

  };

} // end namespace nexus

#endif
