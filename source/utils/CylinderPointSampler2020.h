// ----------------------------------------------------------------------------
///  \file   CylinderPointSampler2020.h
///  \brief  Generator of random points in a cylinder.
///
///  \author   Javier Muñoz Vidal <jmunoz@ific.uv.es>
///  \date     14 January 2020
///  \version  $Id$
///
///  Copyright (c) 2009-2020 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef CYLINDER_POINT_SAMPLER_2020_H
#define CYLINDER_POINT_SAMPLER_2020_H

#include <G4VPhysicalVolume.hh>

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>


namespace nexus {

  // Generator of random positions in a cylinder.
  // It follows the GEANT4 cylinder geometry parameter convention

  class CylinderPointSampler2020
  {
  public:
    // Constructor following Geant4 dimensions convention
    CylinderPointSampler2020(G4double          minRad,
                             G4double          maxRad,
                             G4double          halfLength,
                             G4double          iniPhi   = 0,
                             G4double          deltaPhi = 0,
                             G4RotationMatrix* rotation = 0,
                             G4ThreeVector     origin   = G4ThreeVector(0,0,0));

    // Constructor via Geant4 Physical Volume
    CylinderPointSampler2020(G4VPhysicalVolume* physVolume);

    // Destructor
    ~CylinderPointSampler2020();

    // Returns vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region);

  private:
    G4double      GetRadius(G4double innerRad, G4double outerRad);
    G4double      GetPhi();
    G4double      GetLength(G4double halfLength);
    G4ThreeVector RotateAndTranslate(G4ThreeVector position);

  private:
    G4double          _minRad, _maxRad, _halfLength;  // Solid Dimensions
    G4double          _iniPhi, _deltaPhi;             // Initial & delta Phi
    G4RotationMatrix* _rotation;                      // Rotation of the cylinder (if any)
    G4ThreeVector     _origin;                        // Origin of coordinates
  };

} // namespace nexus

#endif
