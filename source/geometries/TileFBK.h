// ----------------------------------------------------------------------------
// nexus | TileFBK.cc
//
// Geometry of a FBK sensor tile.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef TILE_FBK_H
#define TILE_FBK_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;
namespace nexus {class SiPMFBKVUV;}

namespace nexus {

  class TileFBK: public BaseGeometry
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

    G4ThreeVector GetDimensions();

  private:
    //G4ThreeVector _dimensions; ///< external dimensions of the SiPMpet

    // Visibility of the tracking plane
    G4bool visibility_;

    // Reflectivity of the tile
    G4double reflectivity_;

    // Tile dimensions
    G4double tile_x_, tile_y_, tile_z_;

    // SiPM pitch
    G4double sipm_pitch_;

    // Rows and columns of SiPMs
    G4int n_rows_, n_columns_;

    G4int box_geom_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    SiPMFBKVUV* sipm_;

  };

} // end namespace nexus

#endif
