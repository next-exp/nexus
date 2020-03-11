// -----------------------------------------------------------------------------
//  nexus | CylinderPointSampler2020.cc
//
//  * Author: <jmunoz@ific.uv.es>
//  * Creation date: 14 January 2020
// -----------------------------------------------------------------------------

#include "CylinderPointSampler2020.h"

#include <G4VPhysicalVolume.hh>
#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4VSolid.hh>
#include <G4PhysicalConstants.hh>
#include <Randomize.hh>


namespace nexus {

  // Constructor following Geant4 dimensions convention
  CylinderPointSampler2020::CylinderPointSampler2020 (
    G4double minRad, G4double maxRad, G4double halfLength,
    G4double iniPhi, G4double deltaPhi,
    G4RotationMatrix* rotation,
    G4ThreeVector origin ) :
    _minRad(minRad),
    _maxRad(maxRad),
    _halfLength(halfLength),
    _iniPhi(iniPhi),
    _deltaPhi(deltaPhi),
    _rotation(rotation),
    _origin(origin)
  {
  }


  // Constructor via Geant4 Physical Volume
  CylinderPointSampler2020::CylinderPointSampler2020 (G4VPhysicalVolume* physVolume):
    _rotation(physVolume->GetObjectRotation()),
    _origin(physVolume->GetObjectTranslation())
  {
    G4Tubs* solidVolume = dynamic_cast<G4Tubs*> (physVolume->GetLogicalVolume()->GetSolid());
    _minRad     = solidVolume->GetRMin();
    _maxRad     = solidVolume->GetRMax();
    _halfLength = solidVolume->GetDz();
    _iniPhi     = solidVolume->GetSPhi();
    _deltaPhi   = solidVolume->GetDPhi();
  }


  CylinderPointSampler2020::~CylinderPointSampler2020()
  {
  }



  G4ThreeVector CylinderPointSampler2020::GenerateVertex(const G4String& region)
  {
    G4double x = 0.;
    G4double y = 0.;
    G4double z = 0.;

    // Center of the chamber
    if (region == "CENTER") {
      x = y = z = 0.;
    }

    // Generating from inside the cylinder (between minRad and maxRad)
    else if (region == "VOLUME") {
      G4double phi = GetPhi();
      G4double rad = GetRadius(_minRad, _maxRad);
      x = rad * cos(phi);
      y = rad * sin(phi);
      z = GetLength(_halfLength);
    }

    // Generating from the INNER surface
    else if (region == "INNER_SURFACE") {
      G4double phi = GetPhi();
      G4double rad = _minRad;
      x = rad * cos(phi);
      y = rad * sin(phi);
      z = GetLength(_halfLength);
    }

    // Generating from the OUTER surface
    else if (region == "OUTER_SURFACE") {
      G4double phi = GetPhi();
      G4double rad = _maxRad;
      x = rad * cos(phi);
      y = rad * sin(phi);
      z = GetLength(_halfLength);
    }

    // Unknown region
    else {
      G4Exception("[CylinderPointSampler2020]", "GenerateVertex()", FatalException,
		  "Unknown Region!");
    }

    return RotateAndTranslate(G4ThreeVector(x, y, z));
  }



  G4double CylinderPointSampler2020::GetRadius(G4double innerRad, G4double outerRad)
  {
    G4double rand = G4UniformRand();
    G4double r = sqrt((1.-rand) * innerRad*innerRad + rand * outerRad*outerRad);
    return r;
  }



  G4double CylinderPointSampler2020::GetPhi()
  {
    return (_iniPhi + (G4UniformRand() * _deltaPhi));
  }



  G4double CylinderPointSampler2020::GetLength(G4double halfLength)
  {
    return ((G4UniformRand() * 2.0 - 1.0) * halfLength);
  }



  G4ThreeVector CylinderPointSampler2020::RotateAndTranslate(G4ThreeVector position)
  {
    // Rotating if needed
    if (_rotation) position *= *_rotation;
    // Translating
    position += _origin;

    return position;
  }


} // end namespace nexus
