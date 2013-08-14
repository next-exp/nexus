// NEXT simulation: geometry.hh
// Definition of the geometrical setup
//

#ifndef geometry_h
#define geometry_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VisAttributes.hh"
#include "G4Region.hh"
#include "G4RotationMatrix.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4UserLimits.hh"
#include "G4Material.hh"
#include "G4ios.hh"
#include "G4Colour.hh"

//#include "G4SubtractionSolid.hh"
#include "G4Box.hh"
//#include "G4Tubs.hh"
//#include "G4Polycone.hh"
//#include "G4Polyhedra.hh"

#include <bhep/bhep_svc.h>
#include <bhep/sreader.h>
#include <bhep/error.h>
#include <bhep/engine.h>


//using namespace bhep;

class G4Box;
class G4Tubs;
//class G4Polycone;
//class G4Polyhedra;
class G4Trd;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4FieldManager;


class geometry : public G4VUserDetectorConstruction , public bhep::engine {

private:
  
  // MATERIALS
  G4Material* He;
  G4Material* C;
  G4Material* Al;
  G4Material* Fe;
  G4Material* Cu;
  G4Material* Se;
  G4Material* Mo;
  G4Material* Nd;
  G4Material* Pb;
  G4Material* Air;
  G4Material* H2O;
  G4Material* Sapphire;
  G4Material* Concrete;
  G4Material* Scint;
  G4Material* NaI_scint;
  G4Material* CsI_scint;
  G4Material* Polystyrene;
  G4Material* Vacuum;
  G4Material* Mylar;
  G4Material* Inox;
  G4Material* Tracking_gas;
  G4Material* TPC_Xenon;
  
  // VISIBILITIES 
  G4VisAttributes* invis_VisAtt;
  G4VisAttributes* grey_VisAtt;
  G4VisAttributes* blue_VisAtt;
  G4VisAttributes* brown_VisAtt;
  G4VisAttributes* green_VisAtt;
  G4VisAttributes* purple_VisAtt;
  G4VisAttributes* pink_VisAtt;
  
  // VOLUMES
  // Pointers to the solid, logical & physical world: "WORLD"
  G4Box*             solid_WORLD;    
  G4LogicalVolume*   logic_WORLD;    
  G4VPhysicalVolume* physi_WORLD;    
  // Pointers to the solid, logical & physical detector mother volume: "DETECTOR"
  G4Box*             solid_DETECTOR;    
  G4LogicalVolume*   logic_DETECTOR;    
  G4VPhysicalVolume* physi_DETECTOR;    
  // Pointers to the solid, logical & physical TPC volume: "CHAMBER"
  G4Box*             solid_CHAMBER;    
  G4LogicalVolume*   logic_CHAMBER;    
  G4VPhysicalVolume* physi_CHAMBER;    
  // Pointers to the solid, logical & physical active TPC volume: "ACTIVE"
  G4Box*              solid_ACTIVE;
  G4LogicalVolume*    logic_ACTIVE;    
  G4VPhysicalVolume*  physi_ACTIVE;    
  

  // REGIONS
  G4Region* bulk_region;
  
  // Others
  G4String geom_file_;

   
private:
  
  void define_materials();
  void set_visibilities();
  
public:
  
  // constructor
  geometry(G4String geom_file);
  // destructor
  ~geometry() { }
  // Implement the interface defined by G4VUserDetectorConstruction
  G4VPhysicalVolume* Construct();

};
#endif
