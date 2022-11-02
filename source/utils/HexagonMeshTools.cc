// ----------------------------------------------------------------------------
// nexus | HexagonMeshTools.cc
//
// This class provides the functions needed to create a hexagonal mesh
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "HexagonMeshTools.h"
#include <G4SubtractionSolid.hh>
#include <G4Polyhedra.hh>
#include <G4PVPlacement.hh>


namespace nexus {

  using namespace CLHEP;



  G4ExtrudedSolid* HexagonMeshTools::CreateHexagon(G4double Thickness, G4double circumradius){
    
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
    return new G4ExtrudedSolid("Extruded", polygon, Thickness, offsetA, scaleA, offsetB, scaleB);

  }

  void HexagonMeshTools::PlaceHexagons(G4int nHole, G4double InRadius, G4double Thickness, G4LogicalVolume* DiskLogical, G4LogicalVolume* HexLogical, G4double Mesh_D){
    
    // Logical Volume of SS Disk

    // Use cube coordinate system defined in:
    // https://www.redblobgames.com/grids/hexagons/

    // Dist from hexagon centre to center of hexagon vertex (inc. wire thickness)
    G4double HexSize = (InRadius + Thickness)/std::sqrt(3.0)*mm;

    G4int rmax, rmin;

    // Place center of each hexagon to subtract from disk
    for (G4int q = -nHole; q <= nHole; q++){

      // This logic make sure we place hexagons in the pattern of a hexagon
      // This ensures that we dont create too many holes saving memory
      if (q < 0){
        if (q ==  -nHole){
          rmin = 0;
          rmax = nHole;
        }
        else {
          rmin-=1;
        }
      }
      if (q == 0){
        rmin = -nHole;
        rmax = nHole;
      }
      if (q > 0){
        rmax-=1;
      }
      for (G4int r = rmin; r <= rmax; r++){

        G4double x = HexSize * 3.0/2.0 * q * mm;
        G4double y = HexSize * (std::sqrt(3)/2.0*q + r*std::sqrt(3)) * mm;


        G4double R = std::sqrt(x*x + y*y);
        
        // Only place hexagons within a circle the size of the mesh
        if (R > Mesh_D/2.0)
          continue;

        
        G4RotationMatrix rot = G4RotationMatrix(0, 0, 0);
        G4Transform3D tr = G4Transform3D(rot, G4ThreeVector(x, y, 0*mm));
        new G4PVPlacement(0, G4ThreeVector(x, y, 0*mm), HexLogical, "EL_MeshP", DiskLogical, false, 0, false);
      }
    }

  }



} // end namespace nexus
