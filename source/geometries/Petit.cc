// ----------------------------------------------------------------------------
//  $Id: Petit.cc  $

//  Author:  <romo@ific.uv.es>
//  Created: February 2018
//
//  Copyright (c) 2015-2017 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Petit.h"
#include "PetitModule.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "UniformElectricDriftField.h"
#include "IonizationSD.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UserLimits.hh>
#include <G4NistManager.hh>
#include <G4UniformMagField.hh>
#include <G4FieldManager.hh>
#include <G4TransportationManager.hh>
#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4RotationMatrix.hh>


namespace nexus {
    using namespace CLHEP;

    Petit::Petit():
      BaseGeometry(), msg_(0),
      lab_side_(2.*m),
      dist_between_cells_(10.*cm)

    {
        msg_ = new G4GenericMessenger(this, "/Geometry/Petit/",
                                      "Control commands of geometry PetitModule.");
        
        G4GenericMessenger::Command& dist_cells_cmd =
        msg_->DeclareProperty("dist_between_cells", dist_between_cells_,
                              "Distance between cells");
        dist_cells_cmd.SetUnitCategory("Length");
        dist_cells_cmd.SetParameterName("dist_between_cells", true);
        dist_cells_cmd.SetRange("dist_between_cells>0.");
        
        G4GenericMessenger::Command& lab_side_cmd =
        msg_->DeclareProperty("lab_side", lab_side_, "dimensions of LAB");
        lab_side_cmd.SetUnitCategory("Length");
        lab_side_cmd.SetParameterName("lab_side", true);
        lab_side_cmd.SetRange("lab_side>0.");
        
        G4GenericMessenger::Command&  specific_vertex_X_cmd =
        msg_->DeclareProperty("specific_vertex_X", specific_vertex_X_,
                        "If region is AD_HOC, x coord where particles are generated");
        specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
        specific_vertex_X_cmd.SetUnitCategory("Length");
        
        G4GenericMessenger::Command&  specific_vertex_Y_cmd =
        msg_->DeclareProperty("specific_vertex_Y", specific_vertex_Y_,
                        "If region is AD_HOC, y coord where particles are generated");
        specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
        specific_vertex_Y_cmd.SetUnitCategory("Length");
        
        G4GenericMessenger::Command&  specific_vertex_Z_cmd =
        msg_->DeclareProperty("specific_vertex_Z", specific_vertex_Z_,
                        "If region is AD_HOC, z coord where particles are generated");
        specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
        specific_vertex_Z_cmd.SetUnitCategory("Length");


        module_ = new PetitModule();

    }


    Petit::~Petit()
    {
        delete msg_;
    }


    void Petit::Construct()
    {

        // LAB /////////////////////////////////////////////////////////////
        // This is just a volume of air surrounding the detector so that
        // events can be generated on the outside.
    
        G4Box* lab_solid =
        new G4Box("LAB", lab_side_/2., lab_side_/2., lab_side_/2.);
        
        G4LogicalVolume* lab_logic =
        new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "AIR");
        lab_logic->SetVisAttributes(G4VisAttributes::Invisible);

        // Set this volume as the wrapper for the whole geometry
        // (i.e., this is the volume that will be placed in the world)
        this->SetLogicalVolume(lab_logic);
    
        // Cylinder
        module_->Construct();
        G4double cylinder_diam = module_->GetDiameter();
        // G4double cylinder_len = module_->GetLength();
        G4LogicalVolume* module_logic = module_->GetLogicalVolume();

        // We should rotate both cylinders because they have been created in horizontal position

        // First cylinder:
        G4RotationMatrix rot;
        rot.rotateX(pi/2.);

        new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0.,0., - dist_between_cells_/2 - cylinder_diam/2.)),
                          module_logic, "MODULE_0", lab_logic, false, 1, true);

        // Second cylinder:
        rot.rotateY(pi);
        new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0.,0., + dist_between_cells_/2 + cylinder_diam/2.)),
                          module_logic, "MODULE_1", lab_logic, false, 2, true);

    }

    G4ThreeVector Petit::GenerateVertex(const G4String& region) const
    {
        G4ThreeVector vertex(0., 0., 0.);
    
        if (region == "CENTER") {
            vertex = G4ThreeVector(0.,0.,0.);
        }
        else if (region == "AD_HOC") {
            vertex = G4ThreeVector(specific_vertex_X_, specific_vertex_Y_, specific_vertex_Z_);
        }
    
        else {
            G4Exception("[Petit]", "GenerateVertex()", FatalException,
                        "Unknown vertex generation region!");
        }

        return vertex;
    }


} // end namespace nexus

