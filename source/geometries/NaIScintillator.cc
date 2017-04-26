#include "NaIScintillator.h"
#include "IonizationSD.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>
#include <G4VisAttributes.hh>

namespace nexus {

   using namespace CLHEP;

  NaIScintillator::NaIScintillator(): _visibility(false), _length(38.7*mm)
  {
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("naI_vis", _visibility, "Visibility of NaI scintillator");
  }

  NaIScintillator::~NaIScintillator()
  {
  }

 void NaIScintillator::Construct()
  {
    G4double radius = 44.5/2.*mm;
  
    G4Tubs* sc_solid = new G4Tubs("NaI", 0., radius, _length/2., 0., twopi);
    G4Material* mat = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_SODIUM_IODIDE");
    G4LogicalVolume* sc_logic = new G4LogicalVolume(sc_solid, mat, "NaI");
    sc_logic->SetUserLimits(new G4UserLimits(1.*mm));

    this->SetLogicalVolume(sc_logic);

    // NaI is defined as an ionization sensitive volume.
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
    G4String detname = "/NEXTNEW/NAI";
    IonizationSD* ionisd = new IonizationSD(detname);
    ionisd->IncludeInTotalEnergyDeposit(false);
    sdmgr->AddNewDetector(ionisd);
    sc_logic->SetSensitiveDetector(ionisd);

     if (_visibility) {
      G4VisAttributes yellow_col = nexus::Yellow();
      yellow_col.SetForceSolid(true);
      sc_logic->SetVisAttributes(yellow_col);
    }
  }

  G4double NaIScintillator::GetLength()
  {
    return _length;
  }
}
