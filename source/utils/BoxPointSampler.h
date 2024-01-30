// ----------------------------------------------------------------------------
// nexus | BoxPointSampler.h
//
// This class is a sampler of random uniform points in a box-shaped volume.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef BOX_POINT_SAMPLER_H
#define BOX_POINT_SAMPLER_H

#include "RandomUtils.h"

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>

class G4VPhysicalVolume;


namespace nexus {
  
  class BoxPointSampler
  {
  public:
    /// Constructor
    BoxPointSampler(G4double half_inner_x, G4double half_inner_y,
                    G4double half_inner_z, G4double thickness=0,
                    G4ThreeVector origin=G4ThreeVector(0.,0.,0.),
                    G4RotationMatrix* rotation=0);

    BoxPointSampler (G4VPhysicalVolume* physVolume);

    /// Destructor
    ~BoxPointSampler();

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const vtx_region& region);

    /// Return the intersect point along dir
    G4ThreeVector GetIntersect(const G4ThreeVector& point,
			       const G4ThreeVector& dir);

  private:
    G4double GetLength(G4double origin, G4double max_length);
    G4ThreeVector RotateAndTranslate(G4ThreeVector position);
    void InvertRotationAndTranslation(G4ThreeVector& vec, bool translate=true);

  private:
    G4double half_inner_x_, half_inner_y_, half_inner_z_; ///< Internal dimensions
    G4double outer_x_, outer_y_, outer_z_; ///< External dimensions
    G4double thickness_; ///< Walls thickness

    G4double perc_Zvol_, perc_Yvol_;   ///< Faces volumes percentages
    G4double perc_Zsurf_, perc_Ysurf_; ///< Faces surfaces percentages

    G4ThreeVector origin_;
    G4RotationMatrix* rotation_;
  };

} // namespace nexus

#endif
