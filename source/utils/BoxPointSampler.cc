// ----------------------------------------------------------------------------
// nexus | BoxPointSampler.cc
//
// This class is a sampler of random uniform points in a box-shaped volume.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "BoxPointSampler.h"

#include <Randomize.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4Box.hh>


namespace nexus {

  // Constructor following Geant4 dimensions convention
  BoxPointSampler::BoxPointSampler(G4double half_inner_x,
                                   G4double half_inner_y,
                                   G4double half_inner_z,
                                   G4double thickness,
                                   G4ThreeVector origin,
                                   G4RotationMatrix* rotation):
    half_inner_x_(half_inner_x), half_inner_y_(half_inner_y), half_inner_z_(half_inner_z),
    thickness_(thickness), origin_(origin), rotation_(rotation)
  {
    outer_x_ = 2. * (half_inner_x_ + thickness_);
    outer_y_ = 2. * (half_inner_y_ + thickness_);
    outer_z_ = 2. * (half_inner_z_ + thickness_);

    // The Z face totally covers X and Y faces
    G4double Z_volume = outer_x_ * outer_y_ * thickness_;
    // The Y face totally covers X faces
    G4double Y_volume = outer_x_ * thickness_ * 2*half_inner_z_;
    // The X face does not cover any face
    G4double X_volume = thickness_ * 2*half_inner_y_ * 2*half_inner_z_;

    G4double total_volume = Z_volume + Y_volume + X_volume;

    perc_Zvol_ = Z_volume / total_volume;
    perc_Yvol_ = Y_volume / total_volume;

    // Internal surfaces
    G4double X_surface = 4 * half_inner_x_ * half_inner_x_;
    G4double Y_surface = 4 * half_inner_y_ * half_inner_y_;
    G4double Z_surface = 4 * half_inner_z_ * half_inner_z_;
    G4double total_surface = Z_surface + Y_surface + X_surface;

    perc_Zsurf_ = Z_surface / total_surface;
    perc_Ysurf_ = Y_surface / total_surface;
  }


  // Constructor via Geant4 Physical Volume
  BoxPointSampler::BoxPointSampler (G4VPhysicalVolume* physVolume):
    thickness_(0.),
    origin_(physVolume->GetObjectTranslation()),
    rotation_(physVolume->GetObjectRotation())
  {
    G4Box* solidVolume = dynamic_cast<G4Box*> (physVolume->GetLogicalVolume()->GetSolid());
    half_inner_x_ = solidVolume->GetXHalfLength();
    half_inner_y_ = solidVolume->GetYHalfLength();
    half_inner_z_ = solidVolume->GetZHalfLength();

    outer_x_ = 2. * half_inner_x_;
    outer_y_ = 2. * half_inner_y_;
    outer_z_ = 2. * half_inner_z_;

    // Internal surfaces
    G4double X_surface = 4 * half_inner_x_ * half_inner_x_;
    G4double Y_surface = 4 * half_inner_y_ * half_inner_y_;
    G4double Z_surface = 4 * half_inner_z_ * half_inner_z_;
    G4double total_surface = Z_surface + Y_surface + X_surface;

    perc_Zsurf_ = Z_surface / total_surface;
    perc_Ysurf_ = Y_surface / total_surface;
  }


  BoxPointSampler::~BoxPointSampler()
  {
  }



  G4ThreeVector BoxPointSampler::GenerateVertex(const vtx_region& region)
  {
    G4double x = 0;
    G4double y = 0;
    G4double z = 0;
    G4double origin = 0;
    G4ThreeVector point;

    // Default vertex
    if (region == CENTER) {
      x = y = z = 0.;
    }

    // Generating in the whole volume
    if (region == VOLUME) {
      G4double rand = G4UniformRand();
      G4double rand2 = G4UniformRand();

      // Z walls volume
      if (rand < perc_Zvol_) {
        x = GetLength(0., outer_x_);
        y = GetLength(0., outer_y_);
        // Selecting between -Z and +Z
        if (rand2 < 0.5) origin = -(half_inner_z_  + 0.5*thickness_);
        else             origin =   half_inner_z_  + 0.5*thickness_;
        z = GetLength(origin, thickness_);
      }

      // Y walls volume
      else if ((perc_Zvol_ < rand) && (rand < (perc_Zvol_+perc_Yvol_))) {
        x = GetLength(0., outer_x_);
        // Selecting between -Y and +Y
        if (rand2 < 0.5) origin = -(half_inner_y_  + 0.5*thickness_);
        else  origin            =   half_inner_y_  + 0.5*thickness_;
        y = GetLength(origin, thickness_);
        z = GetLength(0., 2*half_inner_z_);
      }

      // X walls volume
      else {
        // Selecting between -X and +X
        if (rand2 < 0.5) origin = -(half_inner_x_  + 0.5*thickness_);
        else             origin =  (half_inner_x_  + 0.5*thickness_);
        x = GetLength(origin, thickness_);
        y = GetLength(0., 2*half_inner_y_);
        z = GetLength(0., 2*half_inner_z_);
      }
    }

    // Generating in the volume inside
    else if (region == INSIDE) {
      x = GetLength(0., 2*half_inner_x_);
      y = GetLength(0., 2*half_inner_y_);
      z = GetLength(0., 2*half_inner_z_);
    }

    // Generating in the whole surface
    else if (region == INNER_SURF) {
      G4double rand = G4UniformRand();
      G4double rand2 = G4UniformRand();

      // Z walls surface
      if (rand < perc_Zsurf_) {
        x = GetLength(0., 2*half_inner_x_);
        y = GetLength(0., 2*half_inner_y_);
        // Selecting between -Z and +Z
        if (rand2 < 0.5) z = -half_inner_z_;
        else             z =  half_inner_z_;
      }

      // Y walls surface
      else if ((perc_Zsurf_ < rand) && (rand < (perc_Zsurf_+perc_Ysurf_))) {
        x = GetLength(0., 2*half_inner_x_);
        // Selecting between -Y and +Y
        if (rand2 < 0.5) y = -half_inner_y_;
        else             y =  half_inner_y_;
        z = GetLength(0., 2*half_inner_z_);
      }

      // X walls surface
      else {
        // Selecting between -X and +X
        if (rand2 < 0.5) x = -half_inner_x_;
        else             x =  half_inner_x_;
        y = GetLength(0., 2*half_inner_y_);
        z = GetLength(0., 2*half_inner_z_);
      }
    }

    else if (region == OUTER_SURF) {
      G4double rand = G4UniformRand();
      G4double rand2 = G4UniformRand();

      // Z walls surface
      if (rand < perc_Zsurf_) {
        x = GetLength(0., outer_x_);
        y = GetLength(0., outer_y_);
        // Selecting between -Z and +Z
        if (rand2 < 0.5) z = -0.5*outer_z_;
        else             z =  0.5*outer_z_;
      }

      // Y walls surface
      else if ((perc_Zsurf_ < rand) && (rand < (perc_Zsurf_+perc_Ysurf_))) {
        x = GetLength(0., outer_x_);
        // Selecting between -Y and +Y
        if (rand2 < 0.5) y = -0.5*outer_y_;
        else             y =  0.5*outer_y_;
        z = GetLength(0., outer_z_);
      }

      // X walls surface
      else {
        // Selecting between -X and +X
        if (rand2 < 0.5) x = -0.5*outer_x_;
        else             x =  0.5*outer_x_;
        y = GetLength(0., outer_y_);
        z = GetLength(0., outer_z_);
      }
    }

    // Unknown region
    else {
      G4Exception("[BoxPointSampler]", "GenerateVertex()", FatalErrorInArgument,
                  "Unknown vertex region! Possible are: VOLUME, INSIDE, "
                  "INNER_SURF and OUTER_SURF");
    }

    return RotateAndTranslate(G4ThreeVector(x, y, z));

  }



  G4double BoxPointSampler::GetLength(G4double origin, G4double max_length)
  {
    G4double rand = G4UniformRand() - 0.5;
    return origin + (rand * max_length);
  }



  G4ThreeVector BoxPointSampler::RotateAndTranslate(G4ThreeVector position)
  {
    G4ThreeVector real_pos = position;

    // Rotating if needed
    if (rotation_) real_pos *= *rotation_;
    // Translating
    real_pos += origin_;
    return real_pos;
  }

  void BoxPointSampler::InvertRotationAndTranslation(G4ThreeVector& vec,
                                                     bool translate)
  {
    if (translate)
      vec -= origin_;
    if (rotation_)
      vec.rotate(-rotation_->delta(), rotation_->axis());
  }

  G4ThreeVector BoxPointSampler::GetIntersect(const G4ThreeVector& point,
                                              const G4ThreeVector& dir)
  {
    // The point and direction should be in the lab frame.
    // Need to rotate into the SamplerLegacy frame to get the intersection
    // then rotate back into lab frame.
    G4ThreeVector local_point = point;
    InvertRotationAndTranslation(local_point);
    G4ThreeVector local_dir = dir;
    InvertRotationAndTranslation(local_dir, false);

    // Get the +ve movements to intersect with the faces of the box.
    G4double tx = (-2*half_inner_x_ - local_point.x())  / local_dir.x();
    if (tx < 0) tx = (2*half_inner_x_ - local_point.x()) / local_dir.x();

    G4double ty = (-2*half_inner_y_ - local_point.y()) / local_dir.y();
    if (ty < 0) ty = (2*half_inner_y_ - local_point.y()) / local_dir.y();

    G4double tz = (-2*half_inner_z_ - local_point.z()) / local_dir.z();
    if (tz < 0) tz = (2*half_inner_z_ - local_point.z()) / local_dir.z();

    // Protect against outside the region.
    if (tx < 0 || ty < 0 || tz < 0)
      G4Exception("[BoxPointSampler]", "GetIntersect()", FatalException,
		  "Point outside region, projection fail!");
    // The minimum of the tx, ty, tz gives the intersection point.
    G4double tmin = std::min({tx, ty, tz});

    return RotateAndTranslate(local_point + tmin * local_dir);
  }

} // end namespace nexus
