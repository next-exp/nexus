// ----------------------------------------------------------------------------
// nexus | PolygonPointSampler.h
//
// This class is a sampler of random uniform points in a polygon.
// This is designed for sampling inside the light tube with n sides
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef POLYGON_POINT_SAMPLER_H
#define POLYGON_POINT_SAMPLER_H

#include "RandomUtils.h"

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>

class G4VPhysicalVolume;


namespace nexus {

  using namespace CLHEP;

  class PolygonPointSampler
  {
  public:
    // Constructor following Geant4 dimensions convention
    PolygonPointSampler(G4double          min_radius,
                        G4double          max_radius,
                        G4double          half_length,
                        G4int             n_sides,
                        G4RotationMatrix* rotation = nullptr,
                        G4ThreeVector     origin   = G4ThreeVector(0,0,0));

    // Destructor
    ~PolygonPointSampler();

    // Set the coordinates of the polygons
    void InitalizePolygons();

    // Returns vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const vtx_region& region);

  private:
    G4double      GetRadius(G4double innerRad, G4double outerRad);
    G4double      GetLength(G4double half_length);
    G4ThreeVector RotateAndTranslate(G4ThreeVector position);
    void InvertRotationAndTranslation(G4ThreeVector& vec, G4bool translate=true);

    // Check if point is inside the polygon
    G4bool CheckXYBoundsPolygon(std::vector<G4double> point, std::vector<std::vector<G4double>> polygon);

    // Function to sample x and y positions on the edge of a regular polygon with given radius and number of sides
    std::vector<G4double> SampleXYonPolygonEdge(G4double radius);

  private:
    G4double          min_radius_, max_radius_, half_length_;   // Solid Dimensions
    G4int             n_sides_;                                 // Number of sides
    G4RotationMatrix* rotation_;                                // Rotation of the polygon (if any)
    G4ThreeVector     origin_;                                  // Origin of coordinates
    std::vector<std::vector<G4double>> polygon_inner_;          // Define the XY points in a inner polygon
    std::vector<std::vector<G4double>> polygon_outer_;          // Define the XY points in a outer polygon
  };

} // namespace nexus

#endif
