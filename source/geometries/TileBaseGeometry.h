// ----------------------------------------------------------------------------
// nexus | TileBaseGeometry.h
//
// This is an abstract base class for encapsulation of SiPM board geometries.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef TILE_BASE_GEOMETRY_H
#define TILE_BASE_GEOMETRY_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>
//#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  class TileBaseGeometry : public BaseGeometry
  {
  public:

    /// Destructor
    virtual ~TileBaseGeometry();

    void SetBoxGeom (G4int box_geom);
    G4int GetBoxGeom() const;

    void SetTileVisibility (G4bool vis);
    G4bool GetTileVisibility() const;

    void SetTileReflectivity (G4double refl);
    G4double GetTileReflectivity() const;


  protected:
    /// Default constructor defined as protected so no instance of
    /// this base class can be created.
    TileBaseGeometry();

  private:
    G4int box_geom_;
    G4bool tile_vis_;
    G4double tile_refl_;

  };

  // Inline definitions ///////////////////////////////////

  inline TileBaseGeometry::TileBaseGeometry():  box_geom_(0) {}
  inline TileBaseGeometry::~TileBaseGeometry() {}

  inline void TileBaseGeometry::SetBoxGeom(G4int box_geom) { box_geom_ = box_geom; }
  inline G4int TileBaseGeometry::GetBoxGeom() const  { return box_geom_; }

  inline void TileBaseGeometry::SetTileVisibility(G4bool vis) { tile_vis_ = vis; }
  inline G4bool TileBaseGeometry::GetTileVisibility() const  { return tile_vis_; }

  inline void TileBaseGeometry::SetTileReflectivity(G4double refl) { tile_refl_ = refl; }
  inline G4double TileBaseGeometry::GetTileReflectivity() const  { return tile_refl_; }

 }

#endif
