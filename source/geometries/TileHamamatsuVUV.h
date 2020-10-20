// ----------------------------------------------------------------------------
// nexus | TileHamamatsuVUV.h
//
// Hamamatsu VUV sensor tile geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef TILE_HAMAMATSU_VUV_H
#define TILE_HAMAMATSU_VUV_H

#include "TileBaseGeometry.h"

#include <G4ThreeVector.hh>

class G4GenericMessenger;
namespace nexus {
    class SiPMHamamatsuVUV;}

namespace nexus {

  class TileHamamatsuVUV: public TileBaseGeometry
  {
  public:
    /// Constructor
    TileHamamatsuVUV();
    /// Destructor
    ~TileHamamatsuVUV();

    /// Invoke this method to build the volumes of the geometry
    void Construct();

    G4ThreeVector GetDimensions();

  private:

    // Tile dimensions
    G4double tile_x_, tile_y_, tile_z_;

    // SiPM pitch
    G4double sipm_pitch_;

    // Rows and columns of SiPMs
    G4int n_rows_, n_columns_;

    // Thickness of the LXe and quartz in front of the sensors
    G4double lxe_thick_;
    G4double quartz_thick_;
    G4double quartz_transparency_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    SiPMHamamatsuVUV* sipm_;

  };

} // end namespace nexus

#endif
