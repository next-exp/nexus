// ----------------------------------------------------------------------------
// nexus | HexagonPointSampler.cc
//
// This class is a sampler of random uniform points in a hexagon-shaped
// tube.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "HexagonPointSampler.h"

#include <Randomize.hh>
#include <G4RunManager.hh>

#include "CLHEP/Units/PhysicalConstants.h"

#include <vector>


namespace nexus {

  using namespace CLHEP;


  HexagonPointSampler::HexagonPointSampler
  (G4double apothem, G4double length, G4double thickness,
   G4ThreeVector origin, G4RotationMatrix* rotation):
    length_(length), apothem_(apothem),  thickness_(thickness),
    origin_(origin), rotation_(rotation)
  {
    radius_ = apothem_ / cos(pi/6.);
  }



  void HexagonPointSampler::TesselateWithFixedPitch
  (G4double pitch, std::vector<G4ThreeVector>& vpos)
  {
    G4double cell_radius = pitch / sqrt(3.);
    G4double cell_apothem = sqrt(3.)/2. * cell_radius;

    G4int order = floor(2./3. * (apothem_/cell_radius - 1.));

    // Drop the contents of the vector, if any
    vpos.clear();

    PlaceCells(vpos, order, cell_apothem);
  }



  G4ThreeVector HexagonPointSampler::GenerateVertex(HexagonRegion region)
  {
    G4ThreeVector vertex;
    if (region == INSIDE) {

      // Get a random point in a triangular sector of the hexagonal section
      G4ThreeVector point = RandomPointInTriangle();

      // Pick a triangular sector of the hexagon and rotate the
      // previous point accordingly
      G4int face = floor(G4UniformRand() * 6.);
      point.rotateZ(face*pi/3.);

      // Get a random z coordinate
      point.setZ(RandomLength(-length_/2., length_/2));

      // Transform point to the user's system of reference and return
      vertex =  RotateAndTranslate(point);
    } else if (region == PLANE) {
       /// Get a random point in a triangular sector of the hexagonal section
      G4ThreeVector point = RandomPointInTriangle();

      /// Pick a triangular sector of the hexagon and rotate the
      /// previous point accordingly
      G4int face = floor(G4UniformRand() * 6.);
      point.rotateZ(face*pi/3.);

      //point.setZ(RandomLength(-length_/2., -length_/2+100.));
      point.setZ(RandomLength(length_/2.-20., length_/2.));
      vertex =  RotateAndTranslate(point);
    } // else if (region == TRIANGLE) {

    // }
    // // else if (region == TRIANGLE) {

    // //   static G4int index = 0;
    // //   if (index == 0) TriangleWalker();

    // //   return table_vertices_[index];
    // // }
     // Unknown region
    else {
      G4Exception("[HexagonPointSampler]", "GenerateVertex()", FatalException,
		  "Unknown Region!");
    }

    return vertex;
  }



  G4ThreeVector HexagonPointSampler::RandomPointInTriangle()
  {
    G4ThreeVector A(-radius_/2., radius_ * cos(pi/6.), 0);
    G4ThreeVector B( radius_/2., radius_ * cos(pi/6.), 0);

    G4double a = G4UniformRand();
    G4double b = G4UniformRand();

    if ((a+b) > 1.) {
      a = 1. - a;
      b = 1. - b;
    }

    G4ThreeVector P;
    P[0] = a * A[0] + b * B[0];
    P[1] = a * A[1] + b * B[1];

    return P;
  }



  void HexagonPointSampler::PlaceCells(std::vector<G4ThreeVector>& vp,
				       G4int order, G4double cell_apothem)
  {
    // Place the central cell
    G4ThreeVector position(0.,0.,0.);
    vp.push_back(RotateAndTranslate(position));

    // Place the rings
    // Notice that the loop starts at order 1 (first ring)
    // because we've placed already the central cell (order-0 element).
    for (G4int n=1; n<=order; n++) {

      // For a ring of order n, there are n independent cells, that is,
      // cells whose position cannot be obtained by rotating one of the
      // other cells in the ring.
      for (G4int i=0; i<n; i++) {

	position.setX(-n * cell_apothem + i * 2. * cell_apothem);
	position.setY(n* 2. * cell_apothem * cos(pi/6.));

	vp.push_back(RotateAndTranslate(position));

	// Rotating 60º this position, we fill the other sides of
	// the honeycomb.
	for (G4int j=0; j<5; j++)
	  vp.push_back(RotateAndTranslate(position.rotateZ(pi/3.)));
      }
    }
  }



  void HexagonPointSampler::TriangleWalker
  (G4double radius, G4double binning, G4double z)
  {
    G4double x = 0;
    G4double y = 0;

    while (x < radius/2.) {
      table_vertices_.push_back(G4ThreeVector(x,y,z));
      y += binning;
      if (y > apothem_) {
	x += binning;
	y = sqrt(3.) * x;
      }
    }
  }






  G4double HexagonPointSampler::RandomRadius(G4double inner, G4double outer)
  {
    G4double rnd = G4UniformRand();
    G4double r = sqrt((1.-rnd) * inner*inner + rnd * outer*outer);
    return r;
  }



  G4double HexagonPointSampler::RandomPhi()
  {
    return (G4UniformRand() * twopi);
  }



  G4double HexagonPointSampler::RandomLength(G4double min,
					     G4double max)
  {
    return (min + G4UniformRand() * (max-min));
  }



  G4ThreeVector
  HexagonPointSampler::RotateAndTranslate(const G4ThreeVector& position)
  {
    G4ThreeVector real_position(position);

    // Rotating if needed
    if (rotation_)
      real_position *= (*rotation_);
    // Translating
    real_position += origin_;

    return real_position;
  }








} // end namespace nexus
