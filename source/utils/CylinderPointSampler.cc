// ----------------------------------------------------------------------------
// nexus | CylinderPointSampler.cc
//
// This class is a sampler of random uniform points in a cylinder.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "CylinderPointSampler.h"

#include <Randomize.hh>

#include "CLHEP/Units/PhysicalConstants.h"


namespace nexus {

  using namespace CLHEP;


  CylinderPointSampler::CylinderPointSampler(G4double inner_radius,
                                             G4double inner_length,
                                             G4double body_thickness,
                                             G4double endcaps_thickness,
                                             G4ThreeVector origin,
                                             G4RotationMatrix* rotation):
    inner_length_(inner_length),
    inner_radius_(inner_radius),
    body_thickness_(body_thickness),
    endcaps_thickness_(endcaps_thickness),
    outer_radius_(inner_radius_ + body_thickness_),
    origin_(origin),
    rotation_(rotation)
  {
    // Calculate surface ratio between body and endcaps
    G4double body_surf = twopi * inner_radius_ * inner_length_;
    G4double endcaps_surf = 2. * pi * inner_radius_ * inner_radius_;
    perc_body_surf_ = body_surf / (body_surf + endcaps_surf);

    // Calculate volume ratio between body and endcaps
    G4double body_vol = pi * inner_length_ *
      (outer_radius_*outer_radius_ - inner_radius_*inner_radius_);
    G4double endcaps_vol =
      2. * (pi * outer_radius_*outer_radius_) * endcaps_thickness_;

    perc_body_vol_ = body_vol / (body_vol + endcaps_vol);
  }



  CylinderPointSampler::~CylinderPointSampler()
  {
  }



  G4ThreeVector CylinderPointSampler::GenerateVertex(const G4String& region)
  {
    G4double x, y, z, origin;
    G4ThreeVector point;

    // Center of the chamber
    if (region == "CENTER") {
      point =  RotateAndTranslate(G4ThreeVector(0., 0., 0.));
    }

    // Generating in the endcap volume
    else if (region == "ENDCAP_VOL") {
      G4double rand = G4UniformRand();
      G4double phi = GetPhi();
      G4double rad = GetRadius(0., outer_radius_);
      x = rad * cos(phi);
      y = rad * sin(phi);
      // Selecting between -Z and +Z
      if (rand < 0.5) origin = -0.5 * (inner_length_ + endcaps_thickness_);
      else            origin =  0.5 * (inner_length_ + endcaps_thickness_);
      z = GetLength(origin, endcaps_thickness_);
      point =  RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating in the body volume
    else if (region == "BODY_VOL") {
      G4double phi = GetPhi();
      G4double rad = GetRadius(inner_radius_, outer_radius_);
      x = rad * cos(phi);
      y = rad * sin(phi);
      origin = 0.;
      z = GetLength(origin, inner_length_);
      point = RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating in the whole volume
    else if (region == "WHOLE_VOL") {
      G4double rand = G4UniformRand();
      G4double rand2 = G4UniformRand();

      if (rand < perc_body_vol_) {
        // Generating in the body volume
        G4double phi = GetPhi();
        G4double rad = GetRadius(inner_radius_, outer_radius_);
        x = rad * cos(phi);
        y = rad * sin(phi);
        origin = 0.;
        z = GetLength(origin, inner_length_);
      }
      else {
        // Generating in the endcaps volume
        G4double phi = GetPhi();
        G4double rad = GetRadius(0., outer_radius_);
        x = rad * cos(phi);
        y = rad * sin(phi);
        if (rand2 < 0.5) origin = -0.5 * (inner_length_ + endcaps_thickness_);
        else origin             =  0.5 * (inner_length_ + endcaps_thickness_);
        z = GetLength(origin, endcaps_thickness_);
      }

      point =  RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating in the volume inside
    else if (region == "INSIDE") {
      G4double phi = GetPhi();
      G4double rad = GetRadius(0., inner_radius_);
      x = rad * cos(phi);
      y = rad * sin(phi);
      origin = 0.;
      z = GetLength(origin, inner_length_);
      point =  RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating in the endcap surface
    else if (region == "ENDCAP_SURF") {
      G4double rand = G4UniformRand();
      G4double phi = GetPhi();
      G4double rad = GetRadius(0., inner_radius_);
      x = rad * cos(phi);
      y = rad * sin(phi);
      // Selecting between -Z and +Z
      if (rand < 0.5) z = -0.5 * inner_length_;
      else z            =  0.5 * inner_length_;
      point = RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating in the body surface
    else if (region == "BODY_SURF") {
      G4double phi = GetPhi();
      x = inner_radius_ * cos(phi);
      y = inner_radius_ * sin(phi);
      origin = 0.;
      z = GetLength(origin, inner_length_);
      point = RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating in the whole surface
    else if (region == "WHOLE_SURF") {
      G4double rand = G4UniformRand();
      G4double rand2 = G4UniformRand();

      if (rand < perc_body_surf_) {
        // Generating in the body surface
        G4double phi = GetPhi();
        x = inner_radius_ * cos(phi);
        y = inner_radius_ * sin(phi);
        origin = 0.;
        z = GetLength(origin, inner_length_);
      }
      else {
        // Generating in the endcaps surface
        G4double phi = GetPhi();
        G4double rad = GetRadius(0., inner_radius_);
        x = rad * cos(phi);
        y = rad * sin(phi);
        if (rand2 < 0.5) z = -0.5 * inner_length_;
        else             z =  0.5 * inner_length_;
      }

      point =  RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Unknown region
    else {
      G4Exception("[CylinderPointSampler]", "GenerateVertex()", FatalException,
		  "Unknown Region!");
    }

    return point;
  }



  G4double CylinderPointSampler::GetRadius(G4double inner, G4double outer)
  {
    G4double rand = G4UniformRand();
    G4double r = sqrt( (1.-rand) * inner*inner + rand * outer*outer);
    return r;
  }



  G4double CylinderPointSampler::GetPhi()
  {
    return (G4UniformRand() * twopi);
  }



  G4double CylinderPointSampler::GetLength(G4double origin,
					      G4double max_length)
  {
    return (origin + (G4UniformRand() - 0.5) * max_length);
  }



  G4ThreeVector
  CylinderPointSampler::RotateAndTranslate(G4ThreeVector position)
  {
    G4ThreeVector real_pos = position;

    // Rotating if needed
    if (rotation_) real_pos *= *rotation_;
    // Translating
    real_pos += origin_;

    return real_pos;
  }

} // end namespace nexus
