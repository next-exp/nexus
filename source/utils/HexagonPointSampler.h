// ----------------------------------------------------------------------------
// nexus | HexagonPointSampler.h
//
// This class is a sampler of random uniform points in a hexagon-shaped
// tube.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------


#ifndef HEXAGON_POINT_SAMPLER
#define HEXAGON_POINT_SAMPLER

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>

#include <vector>

namespace nexus {

  enum HexagonRegion { INSIDE, PLANE };


  /// FIXME.

  class HexagonPointSampler
  {
  public:

    /// Constructor
    HexagonPointSampler(G4double apothem, G4double length, G4double thickness,
			G4ThreeVector origin=G4ThreeVector(0.,0.,0.),
			G4RotationMatrix* rotation=0);

    /// Destructor
    ~HexagonPointSampler();

    /// Returns vertex within a given region of the chamber
    G4ThreeVector GenerateVertex(HexagonRegion);

    /// Calculates the position of hexagonal cells of a given pitch
    /// and stores them in a vector (notice that the vector will be
    /// cleared before filling it)
    void TesselateWithFixedPitch(G4double pitch,
				 std::vector<G4ThreeVector>& vpos);


  private:
    /// Calculates the position of cells in the honeycomb and stores
    /// them in a vector
    void PlaceCells(std::vector<G4ThreeVector>&, G4int, G4double);

    /// Fills a vector with the positions of all points needed
    /// to create a look-up table
    void TriangleWalker(G4double, G4double, G4double);

    G4ThreeVector RandomPointInTriangle();

    G4double RandomRadius(G4double inner, G4double outer);
    G4double RandomPhi();
    G4double RandomLength(G4double origin, G4double max_length);
    G4ThreeVector RotateAndTranslate(const G4ThreeVector&);

  private:
    /// Default constructor is hidden
    HexagonPointSampler();

  private:

    G4double length_, radius_, apothem_; ///< internal dimensions
    G4double thickness_;

    G4ThreeVector origin_;
    G4RotationMatrix* rotation_;

    G4double binning_;
    std::vector<G4ThreeVector> table_vertices_;

    G4int number_events_;
  };

  // inline methods ..................................................

  inline HexagonPointSampler::~HexagonPointSampler() {}

} // namespace nexus

#endif
