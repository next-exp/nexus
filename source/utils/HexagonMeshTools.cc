// ----------------------------------------------------------------------------
// nexus | HexagonMeshTools.cc
//
//  Functions for helping with hexagonal mesh creation
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "HexagonMeshTools.h"
#include <G4SubtractionSolid.hh>
#include <G4Polyhedra.hh>
#include <G4PVPlacement.hh>


namespace nexus {

  using namespace CLHEP;



  G4ExtrudedSolid* CreateHexagon(G4double half_thickness, G4double circumradius){
    
    // Define a hexagonal prism
    const G4int nsect = 6;
    
    // Create the nodes of the hexagon
    std::vector<G4TwoVector> polygon(nsect);
    G4double ang = 2*pi/nsect;
    for (G4int i = 0; i < nsect; ++i) {
        G4double phi = i*ang;
        G4double cosphi = std::cos(phi);
        G4double sinphi = std::sin(phi);
        polygon[i].set(circumradius*cosphi, circumradius*sinphi);
    }
    
    // Define the hexagon
    G4TwoVector offsetA(0,0), offsetB(0,0);
    G4double scaleA = 1, scaleB = 1;
    return new G4ExtrudedSolid("Extruded", polygon, half_thickness, offsetA, scaleA, offsetB, scaleB);

  }

  // -----

  void PlaceHexagons(G4int n_hole, G4double in_radius, G4double thickness, G4LogicalVolume* disk_logical, G4LogicalVolume* hex_logical, G4double mesh_diam){
    
    // Logical Volume of SS Disk

    // Use cube coordinate system defined in:
    // https://www.redblobgames.com/grids/hexagons/

    // Dist from hexagon centre to center of hexagon vertex (inc. wire thickness)
    G4double hex_size = (in_radius + thickness)/std::sqrt(3.0);

    G4int rmax, rmin;

    // Place center of each hexagon to subtract from disk
    for (G4int q = -n_hole; q <= n_hole; q++){

      // This logic make sure we place hexagons in the pattern of a hexagon
      // This ensures that we dont create too many holes saving memory
      if (q < 0){
        if (q ==  -n_hole){
          rmin = 0;
          rmax = n_hole;
        }
        else {
          rmin-=1;
        }
      }
      if (q == 0){
        rmin = -n_hole;
        rmax = n_hole;
      }
      if (q > 0){
        rmax-=1;
      }
      for (G4int r = rmin; r <= rmax; r++){

        G4double x = hex_size * 3.0/2.0 * q;
        G4double y = hex_size * (std::sqrt(3)/2.0*q + r*std::sqrt(3));


        G4double R = std::sqrt(x*x + y*y);
        
        // Only place hexagons within a circle the size of the mesh
        if (R > mesh_diam/2.0)
          continue;

        
        new G4PVPlacement(0, G4ThreeVector(x, y, 0*mm), hex_logical, "MESH_HOLE_GAS", disk_logical, false, 0, false);
      }
    }

  }



} // end namespace nexus
