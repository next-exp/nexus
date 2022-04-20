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
#include <Randomize.hh>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;
class G4VSolid;


namespace nexus {

  class GeometryBase;

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

    // Sample the Muon Distribution loaded from file
    void GetDirection(G4ThreeVector& dir, G4double& zenith, G4double& azimuth,
                      G4double& energy, G4double& kinetic_energy, G4double mass);

    G4bool CheckOverlap(const G4ThreeVector& vtx, const G4ThreeVector& dir);

    /// Load in the Muon Angular/Energy Distribution from CSV file
    /// and initialise the discrete flux distribution
    void LoadMuonDistribution();


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

    G4bool bInitialize_;  ///< Check if initialisation is already done

    const GeometryBase* geom_; ///< Pointer to the detector geometry

    G4VSolid * geom_solid_;

    std::vector<G4double> flux_, azimuths_, zeniths_, energies_; ///< Values of flux, azimuth and zenith from file
    std::vector<G4double> azimuth_smear_; ///< List of Azimuth bin smear values
    std::vector<G4double> zenith_smear_;  ///< List of Zenith bin smear values
    std::vector<G4double> energy_smear_;  ///< List of Energy bin smear values
    G4RandGeneral *fRandomGeneral_; ///< Pointer to the RNG flux distribution
  };

} // end namespace nexus

#endif
