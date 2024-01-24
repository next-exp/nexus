// ----------------------------------------------------------------------------
// nexus | Next100OpticalGeometry.cc
//
// This class builds a simplified version of the NEXT-100 geometry, where
// only the inner elements are instantiated.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100OpticalGeometry.h"
#include "Next100InnerElements.h"
#include "OpticalMaterialProperties.h"
#include "MaterialsList.h"
#include "FactoryBase.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4Box.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4UnitsTable.hh>

using namespace CLHEP;

namespace nexus {

  REGISTER_CLASS(Next100OpticalGeometry, GeometryBase)

  Next100OpticalGeometry::Next100OpticalGeometry():
    GeometryBase(),
    // common variables used in geometry components
    grid_thickness_ (0.1 * mm),
    gate_tracking_plane_distance_(25. * mm + grid_thickness_),
    gate_sapphire_wdw_distance_  (1458.8 * mm - grid_thickness_),
    pressure_(15. * bar),
    temperature_ (300 * kelvin),
    sc_yield_(25510. * 1/MeV),
    e_lifetime_(1000. * ms),
    fc_displ_x_ (-3.7 * mm), // displacement of the field cage volumes from 0
    fc_displ_y_ (-6.4 * mm), // displacement of the field cage volumes from 0
    specific_vertex_{},
    gas_("naturalXe")
  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/Next100/",
				  "Control commands of geometry Next100.");

    G4GenericMessenger::Command& pressure_cmd =
      msg_->DeclareProperty("pressure", pressure_, "Pressure of gas.");
    pressure_cmd.SetUnitCategory("Pressure");
    pressure_cmd.SetParameterName("pressure", false);
    pressure_cmd.SetRange("pressure>0.");

    new G4UnitDefinition("1/MeV","1/MeV", "1/Energy", 1/MeV);

    G4GenericMessenger::Command& sc_yield_cmd =
      msg_->DeclareProperty("sc_yield", sc_yield_, "Scintillation yield of gas. It is in photons/MeV");
    sc_yield_cmd.SetParameterName("sc_yield", true);
    sc_yield_cmd.SetUnitCategory("1/Energy");

    G4GenericMessenger::Command& e_lifetime_cmd =
      msg_->DeclareProperty("e_lifetime", e_lifetime_, "Electron lifetime in gas.");
    e_lifetime_cmd.SetParameterName("e_lifetime", false);
    e_lifetime_cmd.SetUnitCategory("Time");
    e_lifetime_cmd.SetRange("e_lifetime>0.");

    msg_->DeclarePropertyWithUnit("specific_vertex", "mm",  specific_vertex_,
                                  "Set generation vertex.");

    msg_->DeclareProperty("gas", gas_, "Gas being used");


    inner_elements_ = new Next100InnerElements(grid_thickness_);
  }


  void Next100OpticalGeometry::Construct()
  {
    // LAB /////////////////////////////////////////////////////////////
    // This is just a volume of air without optical properties surrounding
    // the gas so that optical photons die there.

  // AIR
  G4Material* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  G4double lab_size = 4.*m;
  G4Box* lab_solid =
    new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);
  G4LogicalVolume* lab_logic =
    new G4LogicalVolume(lab_solid, air, "LAB");

  lab_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  // Set this volume as the wrapper for the whole geometry
  // (i.e., this is the volume that will be placed in the world)
  this->SetLogicalVolume(lab_logic);

    ///MOTHER VOLUME
  // Build a big box of gas which hosts the optical geometry

  G4Material* gas_mat = nullptr;

  if (gas_ == "naturalXe") {
    gas_mat = materials::GXe(pressure_, temperature_);
    gas_mat->SetMaterialPropertiesTable(opticalprops::GXe(pressure_,
                                                          temperature_,
                                                          sc_yield_,
                                                          e_lifetime_));
  } else if (gas_ == "enrichedXe") {
    gas_mat =  materials::GXeEnriched(pressure_, temperature_);
    gas_mat->SetMaterialPropertiesTable(opticalprops::GXe(pressure_,
                                                          temperature_,
                                                          sc_yield_,
                                                          e_lifetime_));
  } else if  (gas_ == "depletedXe") {
    gas_mat =  materials::GXeDepleted(pressure_, temperature_);
    gas_mat->SetMaterialPropertiesTable(opticalprops::GXe(pressure_,
                                                          temperature_,
                                                          sc_yield_,
                                                          e_lifetime_));
  }  else {
    G4Exception("[Next100OpticalGeometry]", "Construct()", FatalException,
                "Unknown kind of gas, valid options are: naturalXe, enrichedXe, depletedXe.");
  }

  G4double gas_size = lab_size - 10.*cm;
  G4Box* gas_solid = new G4Box("GAS", gas_size/2., gas_size/2., gas_size/2.);
  G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gas_mat, "GAS");

  coord_origin_ = G4ThreeVector(fc_displ_x_, fc_displ_y_, 0);
  G4VPhysicalVolume* gas_phys =
    new G4PVPlacement(0, -coord_origin_, gas_logic,
		      "GAS", lab_logic, false, 0, false);

  ///INNER ELEMENTS
  inner_elements_->SetLogicalVolume(gas_logic);
  inner_elements_->SetPhysicalVolume(gas_phys);
  inner_elements_->SetCoordOrigin(coord_origin_);
  inner_elements_->SetELtoSapphireWDWdistance(gate_sapphire_wdw_distance_);
  inner_elements_->SetELtoTPdistance         (gate_tracking_plane_distance_);
  inner_elements_->Construct();

  // Visibilities
  gas_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  }


  Next100OpticalGeometry::~Next100OpticalGeometry()
  {

  }


  G4ThreeVector Next100OpticalGeometry::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);
    if (region == "AD_HOC") {
      // AD_HOC does not need to be shifted because it is passed by the user
      vertex = specific_vertex_;
      return vertex;
    }
    else {
      vertex = inner_elements_->GenerateVertex(region);
    }

    vertex = vertex - coord_origin_;
    return vertex;
  }


} // end namespace nexus
