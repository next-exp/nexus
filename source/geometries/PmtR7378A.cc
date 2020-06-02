// ----------------------------------------------------------------------------
// nexus | PmtR7378A.cc
//
// Geometry of the Hamamatsu R7378A photomultiplier.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PmtR7378A.h"

#include "PmtSD.h"
#include "OpticalMaterialProperties.h"
#include "MaterialsList.h"
#include "Visibilities.h"

#include <G4NistManager.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  PmtR7378A::PmtR7378A(): BaseGeometry()
  {
  }



  PmtR7378A::~PmtR7378A()
  {
  }



  void PmtR7378A::Construct()
  {
    // PMT BODY //////////////////////////////////////////////////////

    pmt_diam_   = 25.4 * mm;
    pmt_length_ = 43.0 * mm;

    G4Tubs* pmt_solid =
      new G4Tubs("PMT_R7378A", 0., pmt_diam_/2., pmt_length_/2., 0., twopi);

    G4Material* aluminum =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");

    G4LogicalVolume* pmt_logic =
      new G4LogicalVolume(pmt_solid, aluminum, "PMT_R7378A");

    this->SetLogicalVolume(pmt_logic);


    // PMT WINDOW ////////////////////////////////////////////////////

    G4double window_diam = pmt_diam_;
    G4double window_length = 6. * mm;

    G4Tubs* window_solid =
      new G4Tubs("PMT_WINDOW", 0., window_diam/2., window_length/2., 0., twopi);

    G4Material* quartz =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    quartz->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());

    G4LogicalVolume* window_logic =
      new G4LogicalVolume(window_solid, quartz, "PMT_WINDOW");

    new G4PVPlacement(0, G4ThreeVector(0., 0., (pmt_length_-window_length)/2.),
		      window_logic, "PMT_WINDOW", pmt_logic, false, 0, false);

    G4VisAttributes wndw_col = nexus::Blue();
    window_logic->SetVisAttributes(wndw_col);


    // PHOTOCATHODE //////////////////////////////////////////////////

    G4double phcath_diam   = 22.0 * mm;
    G4double phcath_height =  4.0 * mm;
    G4double phcath_thickn =  0.1 * mm;
    G4double phcath_posz   =-16.0 * mm;

    G4double rmax =
      0.5 * (phcath_diam*phcath_diam / (4*phcath_height) + phcath_height);
    G4double rmin = rmax - phcath_thickn;
    G4double theta = asin(phcath_diam/(2.*rmax));

    G4Sphere* phcath_solid =
      new G4Sphere("PHOTOCATHODE", rmin, rmax, 0, twopi, 0, theta);

    G4LogicalVolume* phcath_logic =
      new G4LogicalVolume(phcath_solid, aluminum, "PHOTOCATHODE");

    G4VisAttributes vis_solid;
    vis_solid.SetForceSolid(true);
    phcath_logic->SetVisAttributes(vis_solid);

    //G4PVPlacement* phcath_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,phcath_posz), phcath_logic,
			"PHOTOCATHODE", window_logic, false, 0, false);

    // Sensitive detector
    PmtSD* pmtsd = new PmtSD("/PMT_R7378A/Pmt");
    pmtsd->SetDetectorVolumeDepth(1);
    pmtsd->SetTimeBinning(100.*nanosecond);
    G4SDManager::GetSDMpointer()->AddNewDetector(pmtsd);
    window_logic->SetSensitiveDetector(pmtsd);

    // OPTICAL SURFACES //////////////////////////////////////////////

    // The values for the efficiency are chosen in order to match
    // the curve of the quantum efficiency provided by Hamamatsu:
    // http://sales.hamamatsu.com/en/products/electron-tube-division/detectors/photomultiplier-tubes/part-r7378a.php
    // The source of light is point-like, isotropic and it has been placed at a
    // distance of 25 cm from the surface of the PMT window.
    // The quantity to be compared with the Hamamatsu curve is:
    // number of detected photons/ number of photons that reach the PMT window.
    // The total number of generated photons is taken as
    // number of photons that reach the PMT window because
    // light is generated only in that fraction of solid angle that subtends the
    // window of the PMT.

    const G4int entries = 30;

    G4double ENERGIES[entries] =
      {1.72194*eV, 1.77114*eV, 1.82324*eV, 1.87848*eV, 1.93719*eV,
       1.99968*eV,  2.06633*eV, 2.13759*eV, 2.21393*eV, 2.29593*eV,
       2.38423*eV, 2.47960*eV, 2.58292*eV, 2.69522*eV, 2.81773*eV,
       2.95190*eV, 3.0995*eV, 3.26263*eV, 3.44389*eV, 3.64647*eV,
       3.87438*eV, 4.13267*eV, 4.42786*eV, 4.76846*eV, 5.16583*eV,
       5.63545*eV, 6.19900*eV, 6.88778*eV, 7.74875*eV, 8.85571*eV};
    G4double EFFICIENCY[entries] =
      { 0.00000, 0.00028, 0.00100, 0.00500, 0.00100,
    	0.02200, 0.04500, 0.07000, 0.11500, 0.16000,
    	0.20500, 0.23500, 0.27000, 0.29000, 0.31300,
    	0.35200, 0.38000, 0.38000, 0.37300, 0.37300,
    	0.37000, 0.36000, 0.35500, 0.33500, 0.31000,
    	0.29500, 0.27500, 0.23000, 0.52000, 0.00000};

    G4double REFLECTIVITY[entries] =
      { 0., 0., 0., 0., 0.,
	0., 0., 0., 0., 0.,
	0., 0., 0., 0., 0.,
	0., 0., 0., 0., 0.,
	0., 0., 0., 0., 0.,
	0., 0., 0., 0., 0. };

    G4MaterialPropertiesTable* phcath_mpt = new G4MaterialPropertiesTable();
    phcath_mpt->AddProperty("EFFICIENCY", ENERGIES, EFFICIENCY, entries);
    phcath_mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY, entries);

    G4OpticalSurface* phcath_opsur =
      new G4OpticalSurface("PHOTOCATHODE", unified, polished, dielectric_metal);
    phcath_opsur->SetMaterialPropertiesTable(phcath_mpt);

    new G4LogicalSkinSurface("PHOTOCATHODE", phcath_logic, phcath_opsur);
  }


} // end namespace nexus
