// ----------------------------------------------------------------------------
// nexus | OpticalFibre.cc
//
// Bundle of cylindrical optical fibres with single photosensor.
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
#include "GenericWLSFiber.h"

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
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <Randomize.hh>
#include <string>

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(OpticalFibre,GeometryBase)

OpticalFibre::OpticalFibre():
    GeometryBase(), radius_(1.*mm), length_(1.*cm), fiber_dist_(1.*mm), core_mat_("EJ280"), num_fibers_(1), cyl_vertex_gen_(0)
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

        G4GenericMessenger::Command& dist_cmd = 
            msg_->DeclareProperty("fiber_dist",fiber_dist_,"Distance between the fibers in the bundle");
        length_cmd.SetUnitCategory("Length");
        length_cmd.SetParameterName("fiber_dist",false);
        length_cmd.SetRange("fiber_dist>0.");

        G4GenericMessenger::Command& mat_cmd = 
            msg_->DeclareProperty("core_mat",core_mat_,"Core material (EJ280, EJ286 or Y11)");
        mat_cmd.SetParameterName("core_mat",false);

        G4GenericMessenger::Command& num_cmd =
            msg_->DeclareProperty("num_fibers",num_fibers_,"Number of fibers");
        num_cmd.SetParameterName("num_fibers",false);

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
    if (!issquare(num_fibers_)) G4Exception("[OpticalFibre]", "Construct()", FatalException,
              "Number of fibres must be a square!");
    

    // LAB. This is just a volume of air surrounding the detector
    G4Box* lab_solid = new G4Box("LAB", (sqrt(num_fibers_)-1)*(2*radius_+fiber_dist_)+1.*cm,
                                        (sqrt(num_fibers_)-1)*(2*radius_+fiber_dist_)+1.*cm,
                                        length_+1.*cm);

    G4Material* air=G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    air->SetMaterialPropertiesTable(opticalprops::Vacuum());
    G4LogicalVolume* lab_logic =
      new G4LogicalVolume(lab_solid,
                          air,
                          "LAB");
    lab_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    this->SetLogicalVolume(lab_logic);
    G4String name = "OPTICAL_FIBRE";

    //define materials
    G4Material* FP = materials::FPethylene();
    FP->SetMaterialPropertiesTable(opticalprops::FPethylene());

    G4Material* pmma = materials::PMMA();
    pmma->SetMaterialPropertiesTable(opticalprops::PMMA());

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
    GenericWLSFiber* fiber =
    new GenericWLSFiber("FIBER", "round", 2*radius_,
                        2*length_, 2, true,
                        core_mat, true);
    fiber->Construct();
    G4LogicalVolume* fiber_logic = fiber->GetLogicalVolume();

    //place fibers
    G4int cntr=0;
    for(G4int i=0; i<sqrt(num_fibers_); i++){
        for(G4int j=0; j<sqrt(num_fibers_); j++){
            G4ThreeVector position = G4ThreeVector((int(sqrt(num_fibers_))-1)*(2*radius_+fiber_dist_)-i*(2*radius_+fiber_dist_),
                                                   (int(sqrt(num_fibers_))-1)*(2*radius_+fiber_dist_)-j*(2*radius_+fiber_dist_),
                                                    0);
            new G4PVPlacement(0,position,fiber_logic,
                            fiber_logic->GetName(),lab_logic,true,cntr,true);
            cntr+=1;
        }
    }
            


    //Build the sensor
    sensor_  = new GenericPhotosensor("SENSOR", (sqrt(num_fibers_)-1)*(2*radius_+fiber_dist_)+1.*cm,
                                                (sqrt(num_fibers_)-1)*(2*radius_+fiber_dist_)+1.*cm, thickness_);
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
    G4ThreeVector sensor_pos = G4ThreeVector(((sqrt(num_fibers_)-1)*(2*radius_+fiber_dist_))/2,
                                             ((sqrt(num_fibers_)-1)*(2*radius_+fiber_dist_))/2,
                                             length_+thickness_/3+0.333333333*mm);

    new G4PVPlacement(G4Transform3D(sensor_rot, sensor_pos), sensor_logic,
                        sensor_logic->GetName(), lab_logic, true,
                        cntr+1, true);

    // Debug volume to reflect trapped photons
    G4Box* absorb_box = new G4Box("ABS",(sqrt(num_fibers_)-1)*(2*radius_+fiber_dist_),(sqrt(num_fibers_)-1)*(2*radius_+fiber_dist_),0.2*mm);
    G4LogicalVolume* abs_log = new G4LogicalVolume(absorb_box,materials::FR4(),"ABS");
    new G4PVPlacement(0,G4ThreeVector(((sqrt(num_fibers_)-1)*(2*radius_+fiber_dist_))/2,((sqrt(num_fibers_)-1)*(2*radius_+fiber_dist_))/2
                                        ,-length_-0.2*mm),abs_log,abs_log->GetName(),lab_logic,true,4,true);
    // Reflective surface
    G4MaterialPropertiesTable* refl_surf = new G4MaterialPropertiesTable();
    G4double energy2[]       = {0.2 * eV, 3.5 * eV, 3.6 * eV, 11.5 * eV};
    G4double reflectivity2[] = {0.9     , 0.9     , 0.9     ,  0.9     };
    refl_surf->AddProperty("REFLECTIVITY", energy2, reflectivity2, 4);
    G4OpticalSurface* refl_opsurf =
    new G4OpticalSurface("Refl_optSurf", unified, polished, dielectric_metal);
    refl_opsurf->SetMaterialPropertiesTable(refl_surf);
    new G4LogicalSkinSurface(name + "_optSurf", abs_log, refl_opsurf);

}

bool OpticalFibre::issquare(G4int n) {
    for (G4int i = 0; i < n / 2 + 2; i++) {
        if (i * i == n) {
      return true;
        }
    }
    return false;
}

G4ThreeVector OpticalFibre::GenerateVertex(const G4String& region) const
{
    G4int i = G4RandFlat::shootInt((long) 0, sqrt(num_fibers_));
    G4int j = G4RandFlat::shootInt((long) 0, sqrt(num_fibers_));
    G4ThreeVector position = G4ThreeVector((int(sqrt(num_fibers_))-1)*(2*radius_+fiber_dist_)-i*(2*radius_+fiber_dist_),
                                                   (int(sqrt(num_fibers_))-1)*(2*radius_+fiber_dist_)-j*(2*radius_+fiber_dist_),
                                                    0);
    CylinderPointSampler* cyl_vertex_gen_ = new CylinderPointSampler(0., 2*length_, radius_,  0., position, 0);
    return cyl_vertex_gen_->GenerateVertex(region);
}