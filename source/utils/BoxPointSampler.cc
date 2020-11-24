// ----------------------------------------------------------------------------
// nexus | BoxPointSampler.cc
//
// This class is a sampler of random uniform points in a box-shaped volume.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "BoxPointSampler.h"

#include <Randomize.hh>


namespace nexus {

  BoxPointSampler::BoxPointSampler(G4double inner_x,
                                   G4double inner_y,
                                   G4double inner_z,
                                   G4double thickness,
                                   G4ThreeVector origin,
                                   G4RotationMatrix* rotation):
    inner_x_(inner_x), inner_y_(inner_y), inner_z_(inner_z),
    thickness_(thickness), origin_(origin), rotation_(rotation)
  {
    outer_x_ = inner_x_ + 2.*thickness_;
    outer_y_ = inner_y_ + 2.*thickness_;
    outer_z_ = inner_z_ + 2.*thickness_;

    // The Z face totally covers X and Y faces
    G4double Z_volume = outer_x_ * outer_y_ * thickness_;
    // The Y face totally covers X faces
    G4double Y_volume = outer_x_ * thickness_ * inner_z_;
    // The X face does not cover any face
    G4double X_volume = thickness_ * inner_y_ * inner_z_;

    G4double total_volume = Z_volume + Y_volume + X_volume;

    perc_Zvol_ = Z_volume / total_volume;
    perc_Yvol_ = Y_volume / total_volume;

    // Internal surfaces
    G4double X_surface = inner_x_ * inner_x_;
    G4double Y_surface = inner_y_ * inner_y_;
    G4double Z_surface = inner_z_ * inner_z_;
    G4double total_surface = Z_surface + Y_surface + X_surface;

    perc_Zsurf_ = Z_surface / total_surface;
    perc_Ysurf_ = Y_surface / total_surface;
  }



  BoxPointSampler::~BoxPointSampler()
  {
  }



  G4ThreeVector BoxPointSampler::GenerateVertex(const G4String& region)
  {
    G4double x, y, z, origin;
    G4ThreeVector point;

    // Default vertex
    if (region == "CENTER") {
      point = G4ThreeVector(0., 0., 0.);
    }


    // Generating in the endcap volume
    else if (region == "Z_VOL") {
      G4double rand = G4UniformRand();
      x = GetLength(0., outer_x_);
      y = GetLength(0., outer_y_);
      // Selecting between -Z and +Z
      if (rand < 0.5) origin = -0.5 * (inner_z_ + thickness_);
      else            origin =  0.5 * (inner_z_ + thickness_);
      z = GetLength(origin, thickness_);
      point = RotateAndTranslate(G4ThreeVector(x, y, z));
    }


    // Generating in the whole volume
    else if (region == "WHOLE_VOL") {
      G4double rand = G4UniformRand();
      G4double rand2 = G4UniformRand();

      // Z walls volume
      if (rand < perc_Zvol_) {
        x = GetLength(0., outer_x_);
        y = GetLength(0., outer_y_);
        // Selecting between -Z and +Z
        if (rand2 < 0.5) origin = -0.5 * (inner_z_ + thickness_);
        else             origin =  0.5 * (inner_z_ + thickness_);
        z = GetLength(origin, thickness_);
      }

      // Y walls volume
      else if ( (perc_Zvol_ < rand) && (rand < (perc_Zvol_+perc_Yvol_))) {
        x = GetLength(0., outer_x_);
        // Selecting between -Y and +Y
        if (rand2 < 0.5) origin = -0.5 * (inner_y_ + thickness_);
        else  origin            =  0.5 * (inner_y_ + thickness_);
        y = GetLength(origin, thickness_);
        z = GetLength(0., inner_z_);
      }

      // X walls volume
      else {
        // Selecting between -X and +X
        if (rand2 < 0.5) origin = -0.5 * (inner_x_ + thickness_);
        else             origin =  0.5 * (inner_x_ + thickness_);
        x = GetLength(origin, thickness_);
        y = GetLength(0., inner_y_);
        z = GetLength(0., inner_z_);
      }
      point = RotateAndTranslate(G4ThreeVector(x, y, z));
    }


    // Generating in the volume inside
    else if (region == "INSIDE") {
      x = GetLength(0., inner_x_);
      y = GetLength(0., inner_y_);
      z = GetLength(0., inner_z_);
      point = RotateAndTranslate(G4ThreeVector(x, y, z));
    }


    // Generating in the endcap surface
    else if (region == "Z_SURF") {
      G4double rand = G4UniformRand();
      x = GetLength(0., inner_x_);
      y = GetLength(0., inner_y_);
      if (rand < 0.5) z = -0.5 * inner_z_;
      else            z =  0.5 * inner_z_;
      point = RotateAndTranslate(G4ThreeVector(x, y, z));
    }


    // Generating in the whole surface
    else if (region == "WHOLE_SURF") {
      G4double rand = G4UniformRand();
      G4double rand2 = G4UniformRand();

      // Z walls surface
      if (rand < perc_Zsurf_) {
        x = GetLength(0., inner_x_);
        y = GetLength(0., inner_y_);
        // Selecting between -Z and +Z
        if (rand2 < 0.5) z = -0.5 * inner_z_;
        else             z =  0.5 * inner_z_;
      }

      // Y walls surface
      else if ((perc_Zsurf_ < rand) && (rand < (perc_Zsurf_+perc_Ysurf_))) {
        x = GetLength(0., inner_x_);
        // Selecting between -Y and +Y
        if (rand2 < 0.5) y = -0.5 * inner_y_;
        else             y =  0.5 * inner_y_;
        z = GetLength(0., inner_z_);
      }

      // X walls surface
      else {
        // Selecting between -X and +X
        if (rand2 < 0.5) x = -0.5 * inner_x_;
        else             x =  0.5 * inner_x_;
        y = GetLength(0., inner_y_);
        z = GetLength(0., inner_z_);
      }

      point =  RotateAndTranslate(G4ThreeVector(x, y, z));
    }


    // Unknown region
    else {
      G4String err = "Unknown generation region: " + region;
      G4Exception("[BoxPointSampler]", "GenerateVertex()",
        FatalErrorInArgument, err);
    }

    return point;

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
    // Need to rotate into the Sampler frame to get the intersection
    // then rotate back into lab frame.
    G4ThreeVector local_point = point;
    InvertRotationAndTranslation(local_point);
    G4ThreeVector local_dir = dir;
    InvertRotationAndTranslation(local_dir, false);

    // Get the +ve movements to intersect with the faces of the box.
    G4double tx = (-inner_x_ - local_point.x())  / local_dir.x();
    if (tx < 0) tx = (inner_x_ - local_point.x()) / local_dir.x();

    G4double ty = (-inner_y_ - local_point.y()) / local_dir.y();
    if (ty < 0) ty = (inner_y_ - local_point.y()) / local_dir.y();

    G4double tz = (-inner_z_ - local_point.z()) / local_dir.z();
    if (tz < 0) tz = (inner_z_ - local_point.z()) / local_dir.z();

    // Protect against outside the region.
    if (tx < 0 || ty < 0 || tz < 0)
      G4Exception("[BoxPointSampler]", "GetIntersect()", FatalException,
		  "Point outside region, projection fail!");
    // The minimum of the tx, ty, tz gives the intersection point.
    G4double tmin = std::min({tx, ty, tz});

    return RotateAndTranslate(local_point + tmin * local_dir);
  }

} // end namespace nexus
