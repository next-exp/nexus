// ----------------------------------------------------------------------------
// nexus | MuonGenerator.h
//
// This class is the primary generator of muons
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef MUON_GENERATOR_H
#define MUON_GENERATOR_H

#include <G4VPrimaryGenerator.hh>
#include <G4RotationMatrix.hh>
#include <Randomize.hh>


class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;
class G4VSolid;


namespace nexus {

  class GeometryBase;


  class MuonGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    MuonGenerator();
    /// Destructor
    ~MuonGenerator();

    /// This method is invoked at the beginning of the event. It sets
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);

  private:

    // Sets the rotation angle and the spectra to
    // be read for angle generation as well as
    // setting the overlap volume for filtering.
    void SetupAngles();

    G4String MuonCharge() const;

    // Sample the Muon Distribution loaded from file
    void GetDirection(G4ThreeVector& dir, G4double& zenith, G4double& azimuth,
                      G4double& energy, G4double& kinetic_energy, G4double mass);

    G4ThreeVector ProjectToVertex(const G4ThreeVector& dir);

    G4bool CheckOverlap(const G4ThreeVector& vtx, const G4ThreeVector& dir);

    /// Load in the Muon Angular/Energy Distribution from CSV file
    /// and initialise the discrete flux distribution
    void LoadMuonDistribution();

    // Initialise a cos(x)*cos(x) distribution to sample for zenith
    // Used when not sampling angles from a file
    void InitMuonZenithDist();

    
    G4double GetZenith()  const; // cos(x)*cos(x) distribution
    G4double GetAzimuth() const; // Uniform in from 0 to 2 pi
    

  private:
    G4GenericMessenger* msg_;

    G4ParticleDefinition* particle_definition_;

    G4bool use_lsc_dist_; ///< Use muon distribution according to input file
    G4double axis_rotation_; ///< Angle between North and +z
    G4RotationMatrix *rPhi_; ///< Rotation to adjust axes
    G4ThreeVector user_dir_; ///< User specified fixed dir (when angular generation mode is off)

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

    G4double gen_rad_; ///< Radius of disc for generation

  };

} // end namespace nexus

#endif
