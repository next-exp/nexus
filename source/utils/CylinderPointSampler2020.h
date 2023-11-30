// ----------------------------------------------------------------------------
// nexus | CylinderPointSampler2020.h
//
// This class is a sampler of random uniform points in a cylinder.
// It follows the GEANT4 cylinder geometry parameter convention.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef CYLINDER_POINT_SAMPLER_2020_H
#define CYLINDER_POINT_SAMPLER_2020_H

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>

class G4VPhysicalVolume;


namespace nexus {

  using namespace CLHEP;

  class CylinderPointSampler2020
  {
  public:
    // Constructor following Geant4 dimensions convention
    CylinderPointSampler2020(G4double          minRad,
                             G4double          maxRad,
                             G4double          halfLength,
                             G4double          iniPhi   = 0,
                             G4double          deltaPhi = 0,
                             G4RotationMatrix* rotation = nullptr,
                             G4ThreeVector     origin   = G4ThreeVector(0,0,0));

    // Constructor via Geant4 Physical Volume
    CylinderPointSampler2020(G4VPhysicalVolume* physVolume);

    // Destructor
    ~CylinderPointSampler2020();

    // Returns vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region);

    /// Return the intersect point along dir
    G4ThreeVector GetIntersect(const G4ThreeVector& point,
    			       const G4ThreeVector& dir);

  private:
    G4double      GetRadius(G4double innerRad, G4double outerRad);
    G4double      GetPhi();
    G4double      GetLength(G4double halfLength);
    G4ThreeVector RotateAndTranslate(G4ThreeVector position);
    void InvertRotationAndTranslation(G4ThreeVector& vec, bool translate=true);

  private:
    G4double          minRad_, maxRad_, halfLength_;  // Solid Dimensions
    G4double          iniPhi_, deltaPhi_;             // Initial & delta Phi
    G4RotationMatrix* rotation_;                      // Rotation of the cylinder (if any)
    G4ThreeVector     origin_;                        // Origin of coordinates
  };

} // namespace nexus

#endif
