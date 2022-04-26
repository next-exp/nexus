// ----------------------------------------------------------------------------
// nexus | OpticalFibre.h
//
// Cylindrical optical fibre to be placed on the walls to increase light collection,
// with single photosensor.
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

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(OpticalFibre,GeometryBase)

OpticalFibre::OpticalFibre():
    GeometryBase(), radius_(1.*cm), length_(1.*m), cyl_vertex_gen_(0)
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

        cyl_vertex_gen_ = new CylinderPointSampler(radius_, length_, 0.,  0., G4ThreeVector(0., 0., 0.), 0);

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
    G4Box* lab_solid = new G4Box("LAB", length_+1.*cm, radius_+1.*cm, radius_+1.*cm);

    G4Material* air=G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    air->SetMaterialPropertiesTable(opticalprops::Vacuum());
    G4LogicalVolume* lab_logic =
      new G4LogicalVolume(lab_solid,
                          air,
                          "LAB");
    lab_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    this->SetLogicalVolume(lab_logic);
    
    G4String name = "OPTICAL_FIBRE";

    //define solid volume
    G4Tubs* Cyl_solid = new G4Tubs(name,0.,radius_,length_,0.,twopi);

    //define material
    G4Material* y11 = materials::Y11();
    y11->SetMaterialPropertiesTable(opticalprops::Y11());

    //define logical volume
    G4LogicalVolume* Cyl_logic = new G4LogicalVolume(Cyl_solid,y11,name);
    GeometryBase::SetLogicalVolume(Cyl_logic);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), Cyl_logic,
		      name, lab_logic, false, 0, false);

    // Set the logical volume of the fibre as an ionization
    // sensitive detector, i.e. position, time and energy deposition
    // will be stored for each step of any charged particle crossing
    // the volume.
    IonizationSD* ionizsd = new IonizationSD("/OPTICAL_FIBRE");
    G4SDManager::GetSDMpointer()->AddNewDetector(ionizsd);
    Cyl_logic->SetSensitiveDetector(ionizsd);

    // Set the logical volume of the fibre as a sensitive detector
    SensorSD* senssd = new SensorSD("/OPTICAL_FIBRE_SENS");
    senssd->SetDetectorVolumeDepth(0);
    senssd->SetTimeBinning(1.0*us);
    G4SDManager::GetSDMpointer()->AddNewDetector(senssd);
    Cyl_logic->SetSensitiveDetector(senssd);

    //Build the sensor
    sensor_  = new GenericPhotosensor("SENSOR", radius_, radius_, thickness_);
    sensor_ -> SetVisibility(true);

    //Set the sensor window material
    G4Material* window_mat_ = materials::TPB();
    window_mat_->SetMaterialPropertiesTable(opticalprops::TPB());
    G4MaterialPropertyVector* window_rindex = window_mat_->GetMaterialPropertiesTable()->GetProperty("RINDEX");
    //sensor_ -> SetWindowRefractiveIndex(window_rindex);

    //Set the optical properties of the sensor
    G4MaterialPropertiesTable* photosensor_mpt = new G4MaterialPropertiesTable();
    G4double energy[]       = {0.2 * eV, 3.5 * eV, 3.6 * eV, 11.5 * eV};
    G4double reflectivity[] = {0.0     , 0.0     , 0.0     ,  0.0     };
    G4double efficiency[]   = {1.0     , 1.0     , 1.0     ,  1.0     };
    photosensor_mpt->AddProperty("REFLECTIVITY", energy, reflectivity, 4);
    photosensor_mpt->AddProperty("EFFICIENCY",   energy, efficiency,   4);
    sensor_->SetOpticalProperties(photosensor_mpt);
    sensor_->SetTimeBinning(1*us);

    sensor_->SetWithWLSCoating(true);

    //Set sensor depth and naming order
    sensor_ ->SetSensorDepth(1);
    sensor_ ->SetMotherDepth(2);
    sensor_ ->SetNamingOrder(1);

    sensor_ -> Construct();

    //Placing the sensor
    G4LogicalVolume* sensor_logic = sensor_ -> GetLogicalVolume();
    G4RotationMatrix sensor_rot;
    sensor_rot.rotateY(pi);
    G4ThreeVector sensor_pos = G4ThreeVector(0,
                                             0,
                                             length_);

    new G4PVPlacement(G4Transform3D(sensor_rot, sensor_pos), sensor_logic,
                        sensor_logic->GetName(), lab_logic, true,
                        0, false);
}

G4ThreeVector OpticalFibre::GenerateVertex(const G4String& region) const
{
    return cyl_vertex_gen_->GenerateVertex(region);
}