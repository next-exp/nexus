// ----------------------------------------------------------------------------
// nexus | XeSphere.cc
//
// Sphere filled with xenon.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "XeSphere.h"

#include "SpherePointSampler.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "FactoryBase.h"

#include <G4GenericMessenger.hh>
#include <G4Orb.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4VUserDetectorConstruction.hh>

#include <CLHEP/Units/SystemOfUnits.h>

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(XeSphere, GeometryBase)

namespace nexus {

  XeSphere::XeSphere():
    GeometryBase(), liquid_(true), pressure_(STP_Pressure),
    radius_(1.*m), sphere_vertex_gen_(0)
  {
    msg_ = new G4GenericMessenger(this, "/Geometry/XeSphere/",
      "Control commands of geometry XeSphere.");

    msg_->DeclareProperty("LXe", liquid_,
      "Build the sphere with liquid xenon.");

    G4GenericMessenger::Command& pressure_cmd =
      msg_->DeclareProperty("pressure", pressure_,
      "Set pressure for gaseous xenon (if selected).");
    pressure_cmd.SetUnitCategory("Pressure");
    pressure_cmd.SetParameterName("pressure", false);
    pressure_cmd.SetRange("pressure>0.");

    G4GenericMessenger::Command& radius_cmd =
      msg_->DeclareProperty("radius", radius_, "Radius of the xenon sphere.");
    radius_cmd.SetUnitCategory("Length");
    radius_cmd.SetParameterName("radius", false);
    radius_cmd.SetRange("radius>0.");

    // Create a vertex generator for a sphere
    sphere_vertex_gen_ = new SpherePointSampler(0., radius_);
  }



  XeSphere::~XeSphere()
  {
    delete sphere_vertex_gen_;
    delete msg_;
  }



  void XeSphere::Construct()
  {
    G4String name = "XE_SPHERE";

    // Define solid volume as a sphere
    G4Orb* sphere_solid = new G4Orb(name, radius_);

    // Define the material (LXe or GXe) for the sphere.
    // We use for this the NIST manager or the nexus materials list.
    G4Material* xenon = 0;
    if (liquid_)
      xenon = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    else
      xenon = materials::GXe(pressure_);

    // Define the logical volume of the sphere using the material
    // and the solid volume defined above
    G4LogicalVolume* sphere_logic =
    new G4LogicalVolume(sphere_solid, xenon, name);
    GeometryBase::SetLogicalVolume(sphere_logic);

    // Set the logical volume of the sphere as an ionization
    // sensitive detector, i.e. position, time and energy deposition
    // will be stored for each step of any charged particle crossing
    // the volume.
    IonizationSD* ionizsd = new IonizationSD("/XE_SPHERE");
    G4SDManager::GetSDMpointer()->AddNewDetector(ionizsd);
    sphere_logic->SetSensitiveDetector(ionizsd);
  }



  G4ThreeVector XeSphere::GenerateVertex(const G4String& /*region*/) const
  {
    return sphere_vertex_gen_->GenerateVertex(VOLUME);
  }


} // end namespace nexus
