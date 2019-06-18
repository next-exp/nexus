// ----------------------------------------------------------------------------
///  \file   MuonAngleGenerator.h
///  \brief  Point Sampler for muons generation to centre of Geom.
///
///  \author   Andrew Laing <andrew.laing@uta.edu>
///  \date     30 Jan 2014
///
///
///  Copyright (c) 2019 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __MUON_ANGLE_GENERATOR__
#define __MUON_ANGLE_GENERATOR__

#include <G4VPrimaryGenerator.hh>
#include <G4RotationMatrix.hh>

#include "TH2F.h"

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;
class G4VSolid;


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

    G4bool CheckOverlap(const G4ThreeVector& vtx,
    			const G4ThreeVector& dir);

  private:
    G4GenericMessenger* _msg;

    G4ParticleDefinition* _particle_definition;

    G4double _axis_rotation; ///< Angle between North and +z
    G4RotationMatrix *_rPhi; ///< Rotation to adjust axes

    G4double _energy_min; ///< Minimum kinetic energy
    G4double _energy_max; ///< Maximum kinetic energy

    G4String _region; ///< Name of generator region
    G4String _ang_file; ///< Name of file with distributions
    G4String _dist_name; ///< Name of distribution in file

    TH2F * _distribution; ///< Anglular distribution

    const BaseGeometry* _geom; ///< Pointer to the detector geometry

    G4VSolid * _geom_solid;

  };

} // end namespace nexus

#endif
