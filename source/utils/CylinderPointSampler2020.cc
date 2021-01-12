// ----------------------------------------------------------------------------
// nexus | CylinderPointSampler2020.cc
//
// This class is a sampler of random uniform points in a cylinder.
// It follows the GEANT4 cylinder geometry parameter convention.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

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
    minRad_(minRad),
    maxRad_(maxRad),
    halfLength_(halfLength),
    iniPhi_(iniPhi),
    deltaPhi_(deltaPhi),
    rotation_(rotation),
    origin_(origin)
  {
  }


  // Constructor via Geant4 Physical Volume
  CylinderPointSampler2020::CylinderPointSampler2020 (G4VPhysicalVolume* physVolume):
    rotation_(physVolume->GetObjectRotation()),
    origin_(physVolume->GetObjectTranslation())
  {
    G4Tubs* solidVolume = dynamic_cast<G4Tubs*> (physVolume->GetLogicalVolume()->GetSolid());
    minRad_     = solidVolume->GetRMin();
    maxRad_     = solidVolume->GetRMax();
    halfLength_ = solidVolume->GetDz();
    iniPhi_     = solidVolume->GetSPhi();
    deltaPhi_   = solidVolume->GetDPhi();
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
      G4double rad = GetRadius(minRad_, maxRad_);
      x = rad * cos(phi);
      y = rad * sin(phi);
      z = GetLength(halfLength_);
    }

    // Generating from the INNER surface
    else if (region == "INNER_SURFACE") {
      G4double phi = GetPhi();
      G4double rad = minRad_;
      x = rad * cos(phi);
      y = rad * sin(phi);
      z = GetLength(halfLength_);
    }

    // Generating from the OUTER surface
    else if (region == "OUTER_SURFACE") {
      G4double phi = GetPhi();
      G4double rad = maxRad_;
      x = rad * cos(phi);
      y = rad * sin(phi);
      z = GetLength(halfLength_);
    }

    // Unknown region
    else {
      G4Exception("[CylinderPointSampler2020]", "GenerateVertex()", FatalException,
                  "Unknown Region!");
    }

    return RotateAndTranslate(G4ThreeVector(x, y, z));
  }


  G4ThreeVector
  CylinderPointSampler2020::GetIntersect(const G4ThreeVector& point,
					 const G4ThreeVector& dir)
  {
    // Projects for +ve direction (for backwards send -ve dir)
    // To find the intersection with the Cylinder.
    // First we want to solve for the ray intersection with
    // an infinite barrel, so where point + tdir satisfies
    // x^2 + y^2 - r^2 = 0 (valid for all z before rotation).
    // Solve the quadratic equation and take the +ve t
    G4double a = dir.x()*dir.x() + dir.y()*dir.y();
    G4double b = 2 * (point.x()*dir.x() + point.y()*dir.y());
    G4double c = point.x()*point.x() + point.y()*point.y() - minRad_*minRad_;
    G4double determinant = b*b - 4*a*c;
    if (determinant < 0)
      // No intersection return origin.
      return RotateAndTranslate(G4ThreeVector(0., 0., 0.));

    G4double t = -b + std::sqrt(determinant) / (2 * a);
    if (t < 0) t = -b - std::sqrt(determinant) / (2 * a);
    // If still -ve we're outside the volume giv origin
    if (t < 0)
      return RotateAndTranslate(G4ThreeVector(0., 0., 0.));

    G4ThreeVector barrel_intersect = point + t * dir;

    // If the z of the prediction is within the barrel limits we're done.
    if (std::abs(barrel_intersect.z()) <= halfLength_)
      return RotateAndTranslate(barrel_intersect);

    // If not, we need to check the endcaps.
    if (barrel_intersect.z() < 0)
      // We're beyond the -ve endcap.
      t = (-halfLength_ - point.z()) / dir.z();
    else
      t = (halfLength_ - point.z()) / dir.z();
    return RotateAndTranslate(point + t * dir);
  }



  G4double CylinderPointSampler2020::GetRadius(G4double innerRad, G4double outerRad)
  {
    G4double rand = G4UniformRand();
    G4double r = sqrt((1.-rand) * innerRad*innerRad + rand * outerRad*outerRad);
    return r;
  }



  G4double CylinderPointSampler2020::GetPhi()
  {
    return (iniPhi_ + (G4UniformRand() * deltaPhi_));
  }



  G4double CylinderPointSampler2020::GetLength(G4double halfLength)
  {
    return ((G4UniformRand() * 2.0 - 1.0) * halfLength);
  }



  G4ThreeVector CylinderPointSampler2020::RotateAndTranslate(G4ThreeVector position)
  {
    // Rotating if needed
    if (rotation_) position *= *rotation_;
    // Translating
    position += origin_;

    return position;
  }


} // end namespace nexus
