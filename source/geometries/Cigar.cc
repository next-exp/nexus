// ----------------------------------------------------------------------------
// nexus | Cigar.cc
//
// Box containing optical fibers
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Cigar.h"

#include "FactoryBase.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "GenericSquarePhotosensor.h"

#include <G4GenericMessenger.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4VisAttributes.hh>
#include <G4SubtractionSolid.hh>


using namespace nexus;

REGISTER_CLASS(Cigar, GeometryBase)

namespace nexus {

  Cigar::Cigar():
    GeometryBase(),
    cigar_length_ (400 * mm),
    cigar_width_ (30 * mm),
    fiber_diameter_(1 * mm),
    gas_("Ar"),
    pressure_(0. * bar),
    coating_ ("TPB"),
    fiber_type_ ("Y11"),
    coated_(true)
  {
    msg_ = new G4GenericMessenger(this, "/Geometry/Cigar/",
      "Control commands of geometry Cigar.");

    G4GenericMessenger::Command&  width_cmd =
      msg_->DeclareProperty("cigar_width", cigar_width_,
                            "Barrel fiber radius");
    width_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  length_cmd =
      msg_->DeclareProperty("cigar_length", cigar_length_,
                            "Barrel fiber length");
    length_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  fiber_diameter_cmd =
      msg_->DeclareProperty("fiber_diameter", fiber_diameter_,
                            "Fiber diameter");
    fiber_diameter_cmd.SetUnitCategory("Length");

    msg_->DeclareProperty("gas", gas_, "Gas (Ar or Xe)");

    G4GenericMessenger::Command&  pressure_cmd =
      msg_->DeclareProperty("pressure", pressure_,
                            "Vessel pressure");
    pressure_cmd.SetUnitCategory("Pressure");

    msg_->DeclareProperty("coating", coating_, "Fiber coating (TPB or PTH)");
    msg_->DeclareProperty("fiber_type", fiber_type_, "Fiber type (Y11 or B2)");
    msg_->DeclareProperty("coated", coated_, "Coat fibers with WLS coating");

  }



  Cigar::~Cigar()
  {
    delete msg_;
  }



  void Cigar::Construct()
  {

    G4cout << "[Cigar] *** Cigar geometry ***" << G4endl;
    G4cout << "[Cigar] Using " << fiber_type_ << " fibers";
    if (coated_)
      G4cout << " with " << coating_ << " coating";
    G4cout << G4endl;

    G4RotationMatrix *temp_rot = new G4RotationMatrix();
    temp_rot->rotateY(0 * deg);
    // inside_cigar_ = new BoxPointSampler(cigar_width_, cigar_width_, cigar_length_, 0, G4ThreeVector(0, 0, 0));
    inside_cigar_ = new BoxPointSampler(1*mm, 1*mm, 1*mm, 1*mm, G4ThreeVector(0.,0.,0.));

    world_z_ = cigar_length_ * 2;
    world_xy_ = cigar_width_ * 2;

    G4Material *this_fiber = materials::PS();
    G4MaterialPropertiesTable *this_fiber_optical = nullptr;
    if (fiber_type_ == "Y11") {
      this_fiber_optical = opticalprops::Y11();
    } else if (fiber_type_ == "B2") {
      this_fiber_optical = opticalprops::B2();
    } else {
      G4Exception("[Cigar]", "Construct()",
                  FatalException, "Invalid fiber type, must be Y11 or B2");
    }


    G4Material *this_coating = nullptr;
    G4MaterialPropertiesTable *this_coating_optical = nullptr;
    if (coated_) {
      if (coating_ == "TPB") {
        this_coating = materials::TPB();
        this_coating_optical = opticalprops::TPB();
      } else if (coating_ == "TPH") {
        this_coating = materials::TPH();
        this_coating_optical = opticalprops::TPH();
      } else {
        G4Exception("[Cigar]", "Construct()",
                    FatalException, "Invalid coating, must be TPB or TPH");
      }
    }

    fiber_ = new GenericWLSFiber(fiber_type_, true, true, fiber_diameter_, cigar_length_ + 7 * cm, true, coated_, this_coating, this_fiber, true);

    // WORLD /////////////////////////////////////////////////

    G4Material* world_mat = nullptr;

    if (gas_ == "Ar") {
      world_mat = materials::GAr(pressure_);
      world_mat->SetMaterialPropertiesTable(opticalprops::GAr(1. / (68 * eV)));

    } else if (gas_ == "Xe") {
      world_mat = materials::GXe(pressure_);
      // world_mat->SetMaterialPropertiesTable(opticalprops::GXe(pressure_, 273.15, 250, (1.0E9)));
      world_mat->SetMaterialPropertiesTable(opticalprops::GXe());
    } else {
      G4Exception("[Cigar]", "Construct()",
            FatalException, "Invalid gas, must be Ar or Xe");
    }

    G4Box* world_solid_vol =
     new G4Box("WORLD", world_xy_/2., world_xy_/2., world_z_/2.);

    G4LogicalVolume* world_logic_vol =
      new G4LogicalVolume(world_solid_vol, world_mat, "WORLD");
    world_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());
    GeometryBase::SetLogicalVolume(world_logic_vol);

    // TEFLON PANELS ////////////////////////////////////////////
    G4double panel_width = 2.5 * mm;
    G4double extra_width = (5) * mm;
    // G4Box* teflon_panel_top =
    //   new G4Box("TEFLON_PANEL_TOP", cigar_width_ / 2 + extra_width + panel_width, panel_width / 2, cigar_length_ / 2);
    G4Box* teflon_panel_top =
      new G4Box("TEFLON_PANEL_TOP", cigar_width_ / 2 + extra_width + panel_width, panel_width / 2, cigar_length_ / 2);
    G4Box* teflon_panel_side =
      new G4Box("TEFLON_PANEL_SIDE", cigar_width_ / 2 + extra_width, panel_width / 2, cigar_length_ / 2);
    G4Box* teflon_panel_close =
      new G4Box("TEFLON_PANEL_CLOSE", cigar_width_ / 2 + extra_width+panel_width, cigar_width_ / 2 + extra_width+panel_width, panel_width / 2);
    G4Material* teflon = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
    teflon->SetMaterialPropertiesTable(opticalprops::PTFE());
    G4LogicalVolume* teflon_logic_top =
      new G4LogicalVolume(teflon_panel_top, teflon, "TEFLON");
    G4LogicalVolume* teflon_logic_side =
      new G4LogicalVolume(teflon_panel_side, teflon, "TEFLON");
    G4LogicalVolume* teflon_logic_close =
      new G4LogicalVolume(teflon_panel_close, teflon, "TEFLON_CLOSE");
    teflon_logic_top->SetVisAttributes(nexus::White());
    teflon_logic_side->SetVisAttributes(nexus::White());
    teflon_logic_close->SetVisAttributes(nexus::White());





    G4OpticalSurface* opsur_teflon =
      new G4OpticalSurface("TEFLON_OPSURF", unified, groundteflonair, dielectric_metal);
    opsur_teflon->SetMaterialPropertiesTable(opticalprops::PTFE());

    new G4LogicalSkinSurface("TEFLON_OPSURF", teflon_logic_top, opsur_teflon);

    G4VPhysicalVolume *teflon_top = new G4PVPlacement(0, G4ThreeVector(0, cigar_width_/2+panel_width/2+fiber_diameter_+extra_width/2+panel_width/2+fiber_diameter_/4,0),
                      teflon_logic_top, "TEFLON1", world_logic_vol,
                      true, 0, false);

    G4VPhysicalVolume *teflon_bottom = new G4PVPlacement(0, G4ThreeVector(0, -cigar_width_/2-panel_width/2-fiber_diameter_-(+extra_width/2+panel_width/2+fiber_diameter_/4), 0),
                      teflon_logic_top, "TEFLON2", world_logic_vol,
                      true, 1, false);

    // new G4PVPlacement(0, G4ThreeVector(0, 0, cigar_length_/2+panel_width/2),
    //                   teflon_logic_close, "TEFLON_FRONT", world_logic_vol,
    //                   true, 1, false);

    new G4PVPlacement(0, G4ThreeVector(0, 0, -cigar_length_/2-panel_width/2),
                      teflon_logic_close, "TEFLON_BACK", world_logic_vol,
                      true, 1, false);

    G4RotationMatrix *rot_x = new G4RotationMatrix();
    rot_x->rotateZ(90 * deg);
    new G4PVPlacement(G4Transform3D(*rot_x, G4ThreeVector(cigar_width_ / 2 + panel_width / 2 + fiber_diameter_+extra_width/2+panel_width/2+fiber_diameter_/4, 0, 0)),
                      teflon_logic_side, "TEFLON3", world_logic_vol,
                      true, 1, false);

    new G4PVPlacement(G4Transform3D(*rot_x, G4ThreeVector(-cigar_width_ / 2 - panel_width / 2 - fiber_diameter_-extra_width/2-panel_width/2-fiber_diameter_/4, 0, 0)),
                      teflon_logic_side, "TEFLON4", world_logic_vol,
                      true, 1, false);

    // // FIBER ////////////////////////////////////////////////////
    
    fiber_->SetCoreOpticalProperties(this_fiber_optical);
    fiber_->SetCoatingOpticalProperties(this_coating_optical);

    fiber_->Construct();
    G4LogicalVolume *fiber_logic = fiber_->GetLogicalVolume();
    if (fiber_type_ == "Y11")
      fiber_logic->SetVisAttributes(nexus::LightGreenAlpha());
    else if (fiber_type_ == "B2")
      fiber_logic->SetVisAttributes(nexus::LightBlueAlpha());

    G4int n_fibers = floor(cigar_width_ / fiber_diameter_);
    G4cout << "[Cigar] Box with " << n_fibers << " fibers each side" << G4endl;

    // // DETECTOR /////////////////////////////////////////////
    G4double photosensor_thickness = 0.4 * mm;
    GenericSquarePhotosensor *sipm  = new GenericSquarePhotosensor("FIBER_SENSOR", fiber_diameter_, n_fibers * fiber_diameter_, photosensor_thickness);
    G4int sipm_entries = 24;
    G4double sipm_energy[] = {
      h_Planck * c_light / (866.20 * nm), h_Planck * c_light / (808.45 * nm),
      h_Planck * c_light / (766.20 * nm), h_Planck * c_light / (721.13 * nm),
      h_Planck * c_light / (685.92 * nm), h_Planck * c_light / (647.89 * nm),
      h_Planck * c_light / (623.94 * nm), h_Planck * c_light / (597.18 * nm),
      h_Planck * c_light / (573.24 * nm), h_Planck * c_light / (545.07 * nm),
      h_Planck * c_light / (518.31 * nm), h_Planck * c_light / (502.82 * nm),
      h_Planck * c_light / (454.93 * nm), h_Planck * c_light / (421.13 * nm),
      h_Planck * c_light / (395.77 * nm), h_Planck * c_light / (378.87 * nm),
      h_Planck * c_light / (367.61 * nm), h_Planck * c_light / (359.15 * nm),
      h_Planck * c_light / (349.30 * nm), h_Planck * c_light / (340.85 * nm),
      h_Planck * c_light / (336.62 * nm), h_Planck * c_light / (332.39 * nm),
      h_Planck * c_light / (326.76 * nm), h_Planck * c_light / (319.72 * nm)
    };

    G4double sipm_efficiency[] = {
      5.75, 9.38,
      12.48, 16.37,
      20.18, 24.34,
      28.67, 33.01,
      37.26, 41.50,
      45.93, 48.32,
      50.00, 47.61,
      43.54, 39.03,
      34.25, 29.91,
      25.40, 20.62,
      16.02, 11.50,
      6.81, 3.36
    };
    for (G4int i=0; i < sipm_entries; i++) {
      sipm_efficiency[i] /= 100;
    }
    G4double energy[]       = {opticalprops::optPhotMinE_, opticalprops::optPhotMaxE_};
    G4double reflectivity[] = {0.0     , 0.0     };
    G4double efficiency[]   = {1.      , 1.      };
    G4MaterialPropertiesTable* photosensor_mpt = new G4MaterialPropertiesTable();
    photosensor_mpt->AddProperty("REFLECTIVITY", energy, reflectivity, 2);
    photosensor_mpt->AddProperty("EFFICIENCY",   sipm_energy, sipm_efficiency, sipm_entries);
    sipm->SetOpticalProperties(photosensor_mpt);
    sipm->SetVisibility(true);
    sipm->SetSensorDepth(1);
    sipm->SetTimeBinning(50 * ns);
    sipm->SetWindowRefractiveIndex(opticalprops::OptCoupler()->GetProperty("RINDEX"));
    sipm->Construct();
    G4LogicalVolume* sipm_logic = sipm->GetLogicalVolume();
    // // ALUMINIZED ENDCAP

    G4Material *fiber_end_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");

    G4double fiber_end_z = 350 * nm;

    G4Tubs *fiber_end_solid_vol =
        new G4Tubs("fiber_end", 0, fiber_diameter_ / 2, fiber_end_z, 0, 2 * M_PI);

    G4LogicalVolume *fiber_end_logic_vol =
        new G4LogicalVolume(fiber_end_solid_vol, fiber_end_mat, "FIBER_END");
    G4OpticalSurface *opsur_al =
        new G4OpticalSurface("AL_OPSURF", glisur, ground, dielectric_metal);
    opsur_al->SetPolish(0.75);
    opsur_al->SetMaterialPropertiesTable(opticalprops::PolishedAl());

    new G4LogicalSkinSurface("AL_OPSURF", fiber_end_logic_vol, opsur_al);

    // // PLACEMENT /////////////////////////////////////////////
    G4RotationMatrix *rot_y = new G4RotationMatrix();
    G4RotationMatrix *rot_z = new G4RotationMatrix();
    rot_y->rotateY(180 * deg);
    rot_z->rotateZ(90 * deg);
    new G4PVPlacement(G4Transform3D(*rot_y, G4ThreeVector(cigar_width_ / 2 + fiber_diameter_ / 2+extra_width/2+panel_width/2+fiber_diameter_/4, 0, cigar_length_ / 2 + photosensor_thickness / 2 + 7 * cm)),
                      sipm_logic, "SIPM1", world_logic_vol,
                      false, 1, false);

    new G4PVPlacement(G4Transform3D(*rot_y, G4ThreeVector(- cigar_width_ / 2 - fiber_diameter_ / 2-(extra_width/2+panel_width/2+fiber_diameter_/4), 0, cigar_length_ / 2 + photosensor_thickness / 2 + 7 * cm)),
                      sipm_logic, "SIPM2", world_logic_vol,
                      false, 2, false);
    new G4PVPlacement(G4Transform3D(*rot_z, G4ThreeVector(0, - cigar_width_ / 2 - fiber_diameter_ / 2-(extra_width/2+panel_width/2+fiber_diameter_/4), cigar_length_ / 2 + photosensor_thickness / 2 + 7 * cm)),
                      sipm_logic, "SIPM3", world_logic_vol,
                      false, 3, false);
    new G4PVPlacement(G4Transform3D(*rot_z, G4ThreeVector(0, cigar_width_ / 2 + fiber_diameter_ / 2+extra_width/2+panel_width/2+fiber_diameter_/4, cigar_length_ / 2 + photosensor_thickness / 2 + 7 * cm)),
                      sipm_logic, "SIPM4", world_logic_vol,
                      false, 4, false);



    // Teflon special closing panel
    G4Box* teflon_closing_panel =
      new G4Box("TEFLON_PANEL_FRONT_TEMP", cigar_width_ / 2 + extra_width+panel_width, cigar_width_ / 2 + extra_width+panel_width, panel_width / 2);
    // G4LogicalVolume* teflon_logic_front =
    //   new G4LogicalVolume(teflon_closing_panel, teflon, "TEFLON");
    // teflon_logic_front->SetVisAttributes(nexus::White());
    G4VSolid* temp_solid = teflon_closing_panel;
    G4double fiber_radius = fiber_diameter_/1.1;
    G4double fiber_length = fiber_->GetLength();
    G4VSolid* fiber_solid = new G4Tubs("FIBER_SOLID", 0, fiber_radius / 2, fiber_length / 2, 0, 2 * pi);
    G4SubtractionSolid* subtracted_solid = nullptr;


    for (G4int ifiber = 0; ifiber < n_fibers; ifiber++)
    {

      std::string label = std::to_string(ifiber);

      G4PVPlacement* fiber_placement1 = new G4PVPlacement(0, G4ThreeVector(cigar_width_ / 2 + fiber_diameter_ / 2+extra_width/2+panel_width/2+fiber_diameter_/4, -cigar_width_ / 2 + ifiber * fiber_diameter_ + fiber_diameter_ / 2, 3.5 * cm),
                                                          fiber_logic, "FIBER1-" + label, world_logic_vol,
                                                          true, ifiber, false);

      G4PVPlacement* fiber_placement2 = new G4PVPlacement(0, G4ThreeVector(-cigar_width_ / 2 - fiber_diameter_ / 2-(extra_width/2+panel_width/2+fiber_diameter_/4), -cigar_width_ / 2 + ifiber * fiber_diameter_ + fiber_diameter_ / 2,  3.5 * cm),
                                                          fiber_logic, "FIBER2-" + label, world_logic_vol,
                                                          true, ifiber * 2, false);

      G4PVPlacement* fiber_placement3 = new G4PVPlacement(0, G4ThreeVector(-cigar_width_ / 2 + ifiber * fiber_diameter_ + fiber_diameter_ / 2, -cigar_width_ / 2 - fiber_diameter_ / 2-(extra_width/2+panel_width/2+fiber_diameter_/4),  3.5 * cm),
                                                          fiber_logic, "FIBER3-" + label, world_logic_vol,
                                                          true, ifiber * 3, false);

      G4PVPlacement* fiber_placement4 = new G4PVPlacement(0, G4ThreeVector(-cigar_width_ / 2 + ifiber * fiber_diameter_ + fiber_diameter_ / 2, cigar_width_ / 2 + fiber_diameter_ / 2+(extra_width/2+panel_width/2+fiber_diameter_/4),  3.5 * cm),
                                                          fiber_logic, "FIBER4-" + label, world_logic_vol,
                                                          true, ifiber * 4, false);

      new G4PVPlacement(0, G4ThreeVector(cigar_width_ / 2 + fiber_diameter_ / 2+extra_width/2+panel_width/2+fiber_diameter_/4, -cigar_width_ / 2 + ifiber * fiber_diameter_ + fiber_diameter_ / 2, -cigar_length_ / 2 - fiber_end_z),
                        fiber_end_logic_vol, "ALUMINUM1-" + label, world_logic_vol,
                        true, ifiber, false);

      new G4PVPlacement(0, G4ThreeVector(-cigar_width_ / 2 - fiber_diameter_ / 2-(extra_width/2+panel_width/2+fiber_diameter_/4), -cigar_width_ / 2 + ifiber * fiber_diameter_ + fiber_diameter_ / 2, -cigar_length_ / 2 - fiber_end_z),
                        fiber_end_logic_vol, "ALUMINUM2-" + label, world_logic_vol,
                        true, ifiber, false);

      new G4PVPlacement(0, G4ThreeVector(-cigar_width_ / 2 + ifiber * fiber_diameter_ + fiber_diameter_ / 2, -cigar_width_ / 2 - fiber_diameter_ / 2-(extra_width/2+panel_width/2+fiber_diameter_/4), -cigar_length_ / 2 - fiber_end_z),
                        fiber_end_logic_vol, "ALUMINUM3-" + label, world_logic_vol,
                        true, ifiber, false);

      new G4PVPlacement(0, G4ThreeVector(-cigar_width_ / 2 + ifiber * fiber_diameter_ + fiber_diameter_ / 2, cigar_width_ / 2 + fiber_diameter_ / 2+(extra_width/2+panel_width/2+fiber_diameter_/4), -cigar_length_ / 2 - fiber_end_z),
                        fiber_end_logic_vol, "ALUMINUM4-" + label, world_logic_vol,
                        true, ifiber, false);

     



      // Create a cylinder solid to represent the fiber

      subtracted_solid = new G4SubtractionSolid("SUBTRACTED_SOLID", temp_solid, fiber_solid, rot_z, G4ThreeVector(cigar_width_ / 2 + fiber_diameter_ / 2+extra_width/2+panel_width/2+fiber_diameter_/4, -cigar_width_ / 2 + ifiber * fiber_diameter_ + fiber_diameter_ / 2, 3.5 * cm));
      temp_solid = subtracted_solid;
      subtracted_solid = new G4SubtractionSolid("SUBTRACTED_SOLID", temp_solid, fiber_solid, rot_z, G4ThreeVector(-cigar_width_ / 2 - fiber_diameter_ / 2-(extra_width/2+panel_width/2+fiber_diameter_/4), -cigar_width_ / 2 + ifiber * fiber_diameter_ + fiber_diameter_ / 2,  3.5 * cm));
      temp_solid = subtracted_solid;
      subtracted_solid = new G4SubtractionSolid("SUBTRACTED_SOLID", temp_solid, fiber_solid, 0, G4ThreeVector(-cigar_width_ / 2 + ifiber * fiber_diameter_ + fiber_diameter_ / 2, -cigar_width_ / 2 - fiber_diameter_ / 2-(extra_width/2+panel_width/2+fiber_diameter_/4),  3.5 * cm));
      temp_solid = subtracted_solid;
      subtracted_solid = new G4SubtractionSolid("SUBTRACTED_SOLID", temp_solid, fiber_solid, 0, G4ThreeVector(-cigar_width_ / 2 + ifiber * fiber_diameter_ + fiber_diameter_ / 2, cigar_width_ / 2 + fiber_diameter_ / 2+(extra_width/2+panel_width/2+fiber_diameter_/4),  3.5 * cm));
      temp_solid = subtracted_solid;
      
      

    }



    // Simpler Teflon Close

    G4VSolid* teflon_closing_panel_temp =
      new G4Box("TEFLON_PANEL_FRONT_TEMP", cigar_width_ / 2 + extra_width+panel_width, cigar_width_ / 2 + extra_width+panel_width, panel_width / 2);
    G4VSolid* fiber_space =
      new G4Box("TEFLON_PANEL_FRONT_TEMP", fiber_diameter_, cigar_width_ / 2, panel_width / 2);
    G4SubtractionSolid* subtracted_solid_test = new G4SubtractionSolid("SUBTRACTED_SOLID", teflon_closing_panel_temp, fiber_space, rot_z, G4ThreeVector(0,cigar_width_ / 2 + fiber_diameter_ / 2+extra_width/2+panel_width/2+fiber_diameter_/4,0));
    subtracted_solid_test = new G4SubtractionSolid("SUBTRACTED_SOLID", subtracted_solid_test, fiber_space, rot_z, G4ThreeVector(0,-cigar_width_ / 2 - fiber_diameter_ / 2-(extra_width/2+panel_width/2+fiber_diameter_/4),0));
    rot_y->rotateY(90 * deg);
    subtracted_solid_test = new G4SubtractionSolid("SUBTRACTED_SOLID", subtracted_solid_test, fiber_space, rot_y, G4ThreeVector(-cigar_width_ / 2 - fiber_diameter_ / 2-(extra_width/2+panel_width/2+fiber_diameter_/4),0,0));
    subtracted_solid_test = new G4SubtractionSolid("SUBTRACTED_SOLID", subtracted_solid_test, fiber_space, rot_y, G4ThreeVector(+cigar_width_ / 2 + fiber_diameter_ / 2+(extra_width/2+panel_width/2+fiber_diameter_/4),0,0));
    
    G4LogicalVolume* teflon_closing_panel_logic_temp = new G4LogicalVolume(subtracted_solid_test, teflon, "TEFLON_PANEL");
    teflon_closing_panel_logic_temp->SetVisAttributes(nexus::White());
    rot_x->rotateZ(90 * deg);
    new G4PVPlacement(rot_x, G4ThreeVector(0, 0, cigar_length_/2+panel_width/2), teflon_closing_panel_logic_temp, "TEFLON_FRONT", world_logic_vol, true, 1, false);


    // Create logical volume for the modified box after subtracting fibers
    // G4LogicalVolume* teflon_closing_panel_logic = new G4LogicalVolume(temp_solid, teflon, "TEFLON_PANEL");
    // teflon_closing_panel_logic->SetVisAttributes(nexus::White());
    // rot_x->rotateZ(90 * deg);
    // new G4PVPlacement(rot_x, G4ThreeVector(0, 0, cigar_length_/2+panel_width/2), teflon_closing_panel_logic, "TEFLON_FRONT", world_logic_vol, true, 1, false);







    // // Vacuum chamber

    // G4Tubs* vacuum_chamber =
    //   new G4Tubs("VACUUM_CHAMBER", cigar_width_+3*cm, cigar_width_+5*cm, cigar_length_, 0,2*pi);
    // G4Material* steel = materials::Steel();

    // G4LogicalVolume* vacuum_chamber_logic =
    //   new G4LogicalVolume(vacuum_chamber, steel, "CHAMBER");


    // vacuum_chamber_logic->SetVisAttributes(nexus::DarkGrey());

    // new G4PVPlacement(0, G4ThreeVector(0, 0, 0),
    //                   vacuum_chamber_logic, "VAC_CHAMBER", world_logic_vol,
    //                   true, 0, false);
    




    // G4OpticalSurface* opsur_teflon =
    //   new G4OpticalSurface("TEFLON_OPSURF", unified, groundteflonair, dielectric_metal);
    // opsur_teflon->SetMaterialPropertiesTable(opticalprops::PTFE());

    // new G4LogicalSkinSurface("TEFLON_OPSURF", teflon_logic_top, opsur_teflon);

    // new G4PVPlacement(0, G4ThreeVector(0, 0, cigar_length_/2+panel_width/2),
    //                   teflon_logic_close, "TEFLON_FRONT", world_logic_vol,
    //                   true, 1, false);
    

    // // Check source position inside CIGAR

    // G4Material * cylinder_material = materials::OpticalSilicone();
    // cylinder_material->SetMaterialPropertiesTable(opticalprops::PTFE());

    // G4Tubs* LED_cylinder = new G4Tubs("CYLINDER", 10*mm, 15*mm, 20*mm, 0, 2*pi );
    // G4LogicalVolume* LED_logic =
    //   new G4LogicalVolume(LED_cylinder,
    //                       cylinder_material,
    //                       "CYLINDER");
    // LED_logic->SetVisAttributes(nexus::BloodRed());
    // new G4PVPlacement(0, G4ThreeVector(0, 0, 0),
    //               LED_logic, "CYLINDER", world_logic_vol,
    //               true, 1, true);



    // G4OpticalSurface* ptfe_surface = new G4OpticalSurface("PTFE_SURFACE");
    // ptfe_surface->SetType(dielectric_LUT);
    // ptfe_surface->SetFinish(polishedteflonair);
    // ptfe_surface->SetModel(LUT);
    // ptfe_surface->SetMaterialPropertiesTable(opticalprops::PTFE());

    // new G4LogicalBorderSurface(
    //   "XE_PTFE", world_logic_vol, teflon_top, ptfe_surface);
  }



  // G4ThreeVector Cigar::GenerateVertex(const G4String& region) const
  // {
  //   G4ThreeVector vertex(0., 0., 0.);

  //   // WORLD
  //   if (region == "INSIDE_CIGAR") {
  //     // return vertex;
  //     return inside_cigar_->GenerateVertex(nexus::INSIDE);
  //     // return inside_cigar_->GenerateVertex(nexus::VOLUME);
  //   } else {
  //     G4Exception("[Cigar]", "GenerateVertex()", FatalException,
  //                 "Unknown vertex generation region!");
  //   }
  //   return vertex;
  // }
  G4ThreeVector Cigar::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // WORLD
    if (region == "INSIDE_CIGAR") {
      // return vertex;
      return inside_cigar_->GenerateVertex("INSIDE");
      // return inside_cigar_->GenerateVertex(nexus::VOLUME);
    } else {
      G4Exception("[Cigar]", "GenerateVertex()", FatalException,
                  "Unknown vertex generation region!");
    }
    return vertex;
  }


} // end namespace nexus