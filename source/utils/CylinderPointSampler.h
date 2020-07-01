// ----------------------------------------------------------------------------
// nexus | CylinderPointSampler.h
//
// This class is a sampler of random uniform points in a cylinder.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef CYLINDER_POINT_SAMPLER_H
#define CYLINDER_POINT_SAMPLER_H

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>


namespace nexus {

  /// Generator of random positions in a cylinder

  class CylinderPointSampler
  {
  public:
    /// Constructor
    CylinderPointSampler(G4double inner_rad, G4double inner_length,
			    G4double body_thickness, G4double endcaps_thickness,
			    G4ThreeVector origin=G4ThreeVector(0,0,0),
			    G4RotationMatrix* rotation=0);
    /// Destructor
    ~CylinderPointSampler();

    /// Returns vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region);

  private:
    G4double GetRadius(G4double inner, G4double outer);
    G4double GetPhi();
    G4double GetLength(G4double origin, G4double max_length);
    G4ThreeVector RotateAndTranslate(G4ThreeVector position);

  private:
    /// Default constructor is hidden
    CylinderPointSampler();

  private:
    G4double inner_length_, inner_radius_;        ///< Internal dimensions
    G4double body_thickness_, endcaps_thickness_; ///< Thicknesses
    G4double outer_radius_;                       ///< External radius

    G4double perc_body_surf_, perc_body_vol_; ///< Cylinder body percentages

    G4ThreeVector origin_; ///< Origin of coordinates
    G4RotationMatrix* rotation_; ///< Rotation of the cylinder (if any)

  };

} // namespace nexus

#endif
