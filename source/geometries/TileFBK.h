// ----------------------------------------------------------------------------
// nexus | TileFBK.h
//
// Geometry of a FBK sensor tile.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef TILE_FBK_H
#define TILE_FBK_H

#include "TileBaseGeometry.h"

#include <G4ThreeVector.hh>

class G4GenericMessenger;
namespace nexus {class SiPMFBKVUV;}

namespace nexus {

  class TileFBK: public TileBaseGeometry
  {
  public:
    /// Constructor
    TileFBK();
    /// Destructor
    ~TileFBK();

    /// Return dimensions of the SiPM
    //G4ThreeVector GetDimensions() const;

    /// Invoke this method to build the volumes of the geometry
    void Construct();

  private:

    // Tile dimensions
    G4double tile_x_, tile_y_, tile_z_;

    // SiPM pitch
    G4double sipm_pitch_;

    // Rows and columns of SiPMs
    G4int n_rows_, n_columns_;

    SiPMFBKVUV* sipm_;

  };

} // end namespace nexus

#endif
