// ----------------------------------------------------------------------------
// nexus | OpticalFibre.cc
//
// Cylindrical optical fibre with single photosensor.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "OpticalFibre.h"

#include "CylinderPointSampler.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "SensorSD.h"
#include "OpticalMaterialProperties.h"
#include "FactoryBase.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <GenericPhotosensor.h>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4VUserDetectorConstruction.hh>
<<<<<<< HEAD
<<<<<<< HEAD
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
=======
>>>>>>> b3cb53cd2ff45ccad25848b8b08b5e421d8a5d43
=======
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
>>>>>>> ce45cc3be60cac0b4c8323c97395e1440c62affb

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(OpticalFibre,GeometryBase)

OpticalFibre::OpticalFibre():
<<<<<<< HEAD
<<<<<<< HEAD
    GeometryBase(), radius_(1.*cm), length_(1.*cm), core_mat_("EJ280"), cyl_vertex_gen_(0)
=======
    GeometryBase(), radius_(1.*cm), length_(1.*m), cyl_vertex_gen_(0)
>>>>>>> b3cb53cd2ff45ccad25848b8b08b5e421d8a5d43
=======
    GeometryBase(), radius_(1.*cm), length_(1.*cm), cyl_vertex_gen_(0)
>>>>>>> ce45cc3be60cac0b4c8323c97395e1440c62affb
    {
        msg_=new G4GenericMessenger(this,"/Geometry/OpticalFibre/","Control commands of geometry OpticalFibre");

        G4GenericMessenger::Command& radius_cmd = 
            msg_->DeclareProperty("radius",radius_,"Radius of the cylindrical optical fibre");
        radius_cmd.SetUnitCategory("Length");
        radius_cmd.SetParameterName("radius",false);
        radius_cmd.SetRange("radius>0.");

        G4GenericMessenger::Command& length_cmd = 
            msg_->DeclareProperty("length",length_,"Length of the cylindrical optical fibre");
        length_cmd.SetUnitCategory("Length");
        length_cmd.SetParameterName("length",false);
        length_cmd.SetRange("length>0.");

<<<<<<< HEAD
<<<<<<< HEAD
        G4GenericMessenger::Command& mat_cmd = 
            msg_->DeclareProperty("core_mat",core_mat_,"Core material (EJ280, EJ286 or Y11)");
        mat_cmd.SetParameterName("core_mat",false);
=======
        cyl_vertex_gen_ = new CylinderPointSampler(radius_, length_, 0.,  0., G4ThreeVector(0., 0., 0.), 0);
>>>>>>> b3cb53cd2ff45ccad25848b8b08b5e421d8a5d43

=======
>>>>>>> ce45cc3be60cac0b4c8323c97395e1440c62affb
        // hardcoded thickness of the photosensor
        thickness_=2.*mm;
    }

OpticalFibre::~OpticalFibre()
{
    delete cyl_vertex_gen_;
    delete msg_;
}

void OpticalFibre::Construct()
{
    // LAB. This is just a volume of air surrounding the detector
    G4Box* lab_solid = new G4Box("LAB", 2*radius_+1.*cm, 2*radius_+1.*cm, length_+1.*cm);

    G4Material* air=G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    air->SetMaterialPropertiesTable(opticalprops::Vacuum());
    G4LogicalVolume* lab_logic =
      new G4LogicalVolume(lab_solid,
                          air,
                          "LAB");
    lab_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    this->SetLogicalVolume(lab_logic);
    G4String name = "OPTICAL_FIBRE";

    //OUTER CLADDING
    G4Tubs* Cyl_outer = new G4Tubs("OUTER",0.,radius_,length_,0.,twopi);

    //define material
    G4Material* FP = materials::FPethylene();
    FP->SetMaterialPropertiesTable(opticalprops::FPethylene());

    //define logical volume
    G4LogicalVolume* outer_logic = new G4LogicalVolume(Cyl_outer,FP,"OUTER");
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), outer_logic,
		      "OUTER", lab_logic, true, 0, true);

    //INNER CLADDING
    G4Tubs* Cyl_inner = new G4Tubs("INNER",0.,0.98*radius_,length_,0.,twopi);

    //define material
    G4Material* pmma = materials::PMMA();
    pmma->SetMaterialPropertiesTable(opticalprops::PMMA());

    //define logical volume
    G4LogicalVolume* logic_inner = new G4LogicalVolume(Cyl_inner,pmma,"INNER");
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logic_inner,
		      "INNER", outer_logic, true, 1, true);

    //CORE
    G4Tubs* Cyl_solid = new G4Tubs(name,0.,0.96*radius_,length_,0.,twopi);

    //define material
    G4Material* core_mat;
    if (core_mat_=="EJ280") {
        core_mat = materials::EJ280();
        core_mat->SetMaterialPropertiesTable(opticalprops::EJ280());
    }
    if (core_mat_=="EJ286") {
        core_mat = materials::EJ286();
        core_mat->SetMaterialPropertiesTable(opticalprops::EJ286());
    }
    if (core_mat_=="Y11") {
        core_mat = materials::Y11();
        core_mat->SetMaterialPropertiesTable(opticalprops::Y11());
    }

    //define logical volume
<<<<<<< HEAD
    G4LogicalVolume* Cyl_logic = new G4LogicalVolume(Cyl_solid,core_mat,name);
=======
    G4LogicalVolume* Cyl_logic = new G4LogicalVolume(Cyl_solid,y11,name);
>>>>>>> ce45cc3be60cac0b4c8323c97395e1440c62affb
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), Cyl_logic,
		      name, logic_inner, true, 2, true);

    // Set the logical volume of the fibre as an ionization
    // sensitive detector, i.e. position, time and energy deposition
    // will be stored for each step of any charged particle crossing
    // the volume.
//    IonizationSD* ionizsd = new IonizationSD("/OPTICAL_FIBRE");
//    G4SDManager::GetSDMpointer()->AddNewDetector(ionizsd);
//    Cyl_logic->SetSensitiveDetector(ionizsd);
//
//    // Set the logical volume of the fibre as a sensitive detector
//    SensorSD* senssd = new SensorSD("/OPTICAL_FIBRE_SENS");
//    senssd->SetDetectorVolumeDepth(0);
//    senssd->SetTimeBinning(1.0*us);
//    G4SDManager::GetSDMpointer()->AddNewDetector(senssd);
//    Cyl_logic->SetSensitiveDetector(senssd);

    //Build the sensor
    sensor_  = new GenericPhotosensor("SENSOR", 4*radius_, 4*radius_, thickness_);
    sensor_ -> SetVisibility(true);

    //Set the sensor window material
    //G4Material* window_mat_ = materials::Y11();
    //window_mat_->SetMaterialPropertiesTable(opticalprops::Y11());
    //G4MaterialPropertyVector* window_rindex = window_mat_->GetMaterialPropertiesTable()->GetProperty("RINDEX");
    G4MaterialPropertyVector* window_rindex = air->GetMaterialPropertiesTable()->GetProperty("RINDEX");
    sensor_ -> SetWindowRefractiveIndex(window_rindex);

    //Set the optical properties of the sensor
    G4MaterialPropertiesTable* photosensor_mpt = new G4MaterialPropertiesTable();
    G4double energy[]       = {0.2 * eV, 3.5 * eV, 3.6 * eV, 11.5 * eV};
    G4double reflectivity[] = {0.0     , 0.0     , 0.0     ,  0.0     };
    G4double efficiency[]   = {1.0     , 1.0     , 1.0     ,  1.0     };
    photosensor_mpt->AddProperty("REFLECTIVITY", energy, reflectivity, 4);
    photosensor_mpt->AddProperty("EFFICIENCY",   energy, efficiency,   4);
    sensor_->SetOpticalProperties(photosensor_mpt);
    sensor_->SetTimeBinning(1*us);

    sensor_->SetWithWLSCoating(false);

    //Set sensor depth and naming order
    sensor_ ->SetSensorDepth(1);
    //sensor_ ->SetMotherDepth(0);
    //sensor_ ->SetNamingOrder(0);

    sensor_ -> Construct();

    //Placing the sensor
    G4LogicalVolume* sensor_logic = sensor_ -> GetLogicalVolume();
    G4RotationMatrix sensor_rot;
    sensor_rot.rotateY(pi);
    G4ThreeVector sensor_pos = G4ThreeVector(0,
                                             0,
                                             length_+thickness_/3+0.333333333*mm);

    new G4PVPlacement(G4Transform3D(sensor_rot, sensor_pos), sensor_logic,
                        sensor_logic->GetName(), lab_logic, true,
                        3, true);

    // Debug volume to reflect trapped photons
    G4Box* absorb_box = new G4Box("ABS",2*radius_,2*radius_,0.2*mm);
    G4LogicalVolume* abs_log = new G4LogicalVolume(absorb_box,materials::FR4(),"ABS");
    new G4PVPlacement(0,G4ThreeVector(0,0,-length_-0.2*mm),abs_log,abs_log->GetName(),lab_logic,true,4,true);
    // Reflective surface
    G4MaterialPropertiesTable* refl_surf = new G4MaterialPropertiesTable();
    G4double energy2[]       = {0.2 * eV, 3.5 * eV, 3.6 * eV, 11.5 * eV};
<<<<<<< HEAD
    G4double reflectivity2[] = {0.     , 0.     , 0.     ,  0.     };
=======
    G4double reflectivity2[] = {0.9     , 0.9     , 0.9     ,  0.9     };
>>>>>>> ce45cc3be60cac0b4c8323c97395e1440c62affb
    photosensor_mpt->AddProperty("REFLECTIVITY", energy2, reflectivity2, 4);
    G4OpticalSurface* refl_opsurf =
    new G4OpticalSurface("Refl_optSurf", unified, polished, dielectric_metal);
    refl_opsurf->SetMaterialPropertiesTable(refl_surf);
    new G4LogicalSkinSurface(name + "_optSurf", abs_log, refl_opsurf);


    cyl_vertex_gen_ = new CylinderPointSampler(0., 2*length_, radius_,  0., G4ThreeVector(0., 0., 0.), 0);
 
}

G4ThreeVector OpticalFibre::GenerateVertex(const G4String& region) const
{
    return cyl_vertex_gen_->GenerateVertex(region);
}