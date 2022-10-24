// -----------------------------------------------------------------------------
//  nexus | GenericWLSFiber.cc
//
//  Geometry of a configurable wave-length shifting optical fiber.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "GenericWLSFiber.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4Colour.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4GenericMessenger.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>


using namespace nexus;


GenericWLSFiber::GenericWLSFiber(G4String    name,
                                 G4String    shape,
                                 G4double    thickness,
                                 G4double    length,
                                 G4int       num_claddings,
                                 G4bool      with_coating,
                                 G4Material* core_mat,
                                 G4bool      visibility):
  GeometryBase    (),
  name_           (name),
  shape_          (shape),         // "round"  or "square"
  thickness_      (thickness),     // Diameter (for round), Side (for square)
  length_         (length),
  num_claddings_  (num_claddings),
  with_coating_   (with_coating),
  core_mat_       (core_mat),
  visibility_     (visibility)
{
  // Assert num_claddings in [1, 2]
  if ((num_claddings_ < 1) || (num_claddings_ > 2))
    G4Exception("[GenericWLSFiber]", "GenericWLSFiber()", FatalException,
                "Number of claddings not valid.");

  // Assert shape in ['round', 'square']
  if ((shape_ != "round") && (shape_ != "square"))
       G4Exception("[GenericWLSFiber]", "GenericWLSFiber()", FatalException,
                   "Wrong shape. Valid shapes: round or square.");
}


GenericWLSFiber::~GenericWLSFiber()
{
}


void GenericWLSFiber::ComputeDimensions()
{
  // According to Kuraray specifications, each one of the cladding layers
  // has a thickness of 2% the fiber thickness (side, or diameter)
  G4double clad_thickness = 0.02 * thickness_;

  // The coating (always of 1 micron thickness)
  G4double coating_thickness = 1. * um;

  // Single cladding fibers
  if (num_claddings_ == 1) {
    iclad_rad_ = thickness_ / 2.;
    core_rad_  = iclad_rad_ - clad_thickness;
    if (with_coating_) iclad_rad_ -= coating_thickness;
  }

  // Double cladding fibers
  else if (num_claddings_ == 2) {
    oclad_rad_ = thickness_ / 2.;
    iclad_rad_ = oclad_rad_ - clad_thickness;
    core_rad_  = iclad_rad_ - clad_thickness;
    if (with_coating_) oclad_rad_ -= coating_thickness;
  }
}


void GenericWLSFiber::DefineMaterials()
{
  // Fiber cladding materials to be used ...
  // PMMA:       in single cladding, and for inner in muticladding
  // FPethylene: for outer in muticladding

  iclad_mat_ = materials::PMMA();
  iclad_mat_->SetMaterialPropertiesTable(opticalprops::PMMA());

  // If 2 claddings, defining the outer cladding material
  if (num_claddings_ == 2) {
    oclad_mat_ = materials::FPethylene();
    oclad_mat_->SetMaterialPropertiesTable(opticalprops::FPethylene());
  }

  // If coated, always with TPB
  if (with_coating_) {
    coating_mat_ = materials::TPB();
    coating_mat_->SetMaterialPropertiesTable(opticalprops::TPB());
  }
}


void GenericWLSFiber::Construct()
{
  ComputeDimensions();

  DefineMaterials();

  if      (shape_ == "round")  BuildRoundFiber();
  else if (shape_ == "square") BuildSquareFiber();
}


void GenericWLSFiber::BuildRoundFiber()
{
  // Pointer to the innermost logical volume defined at any moment.
  G4LogicalVolume* innermost_logic = nullptr;

  // Coating (only if it exists ...)
  G4LogicalVolume* coating_logic;
  if (with_coating_) {
    G4String coating_name = name_ + "_WLS";
    G4cout << "**** Building COATING " << coating_name << G4endl;
    G4Tubs* coating_solid =
      new G4Tubs(coating_name, 0., thickness_/2., length_/2., 0., 360.*deg);
    coating_logic =
      new G4LogicalVolume(coating_solid, coating_mat_, coating_name);

    // Optical surface
    G4OpticalSurface* coating_optSurf =
      new G4OpticalSurface(coating_name + "_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);
    new G4LogicalSkinSurface(coating_name + "_OPSURF", coating_logic,
                             coating_optSurf);

    GeometryBase::SetLogicalVolume(coating_logic);
    innermost_logic = coating_logic;
  }

  // Outer Cladding (only if it exists ...)
  G4LogicalVolume* oclad_logic;
  if (num_claddings_ == 2) {
    G4String oclad_name = name_ + "_OCLAD";
    G4cout << "**** Building OCLAD " << oclad_name << G4endl;
    G4Tubs* oclad_solid =
      new G4Tubs(oclad_name, 0., oclad_rad_, length_/2., 0., 360.*deg);
    oclad_logic =
      new G4LogicalVolume(oclad_solid, oclad_mat_, oclad_name);

    if (GeometryBase::GetLogicalVolume() == nullptr)
      GeometryBase::SetLogicalVolume(oclad_logic);
    else
      new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), oclad_logic,
                        oclad_name, innermost_logic, false, 0, false);

    innermost_logic = oclad_logic;
  }

  // Inner Cladding (always built)
  G4String iclad_name = name_ + "_ICLAD";
  G4cout << "**** Building ICLAD " << iclad_name << G4endl;
  G4Tubs* iclad_solid =
    new G4Tubs(iclad_name, 0., iclad_rad_, length_/2., 0., 360.*deg);
  G4LogicalVolume* iclad_logic =
    new G4LogicalVolume(iclad_solid, iclad_mat_, iclad_name);

  if (GeometryBase::GetLogicalVolume() == nullptr)
      GeometryBase::SetLogicalVolume(iclad_logic);
  else
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), iclad_logic,
                      iclad_name, innermost_logic, false, 0, false);

  // Fiber core (always inside the iclad)
  G4cout << "**** Building CORE " << name_ << G4endl;
  G4Tubs* core_solid =
    new G4Tubs(name_, 0., core_rad_, length_/2., 0., 360.*deg);
  G4LogicalVolume* core_logic =
    new G4LogicalVolume(core_solid, core_mat_, name_);
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), core_logic,
                    name_, iclad_logic, false, 0, false);

  // VISIBILITIES
  if (visibility_) {
    if (num_claddings_ == 2)
      oclad_logic->SetVisAttributes(nexus::LightBlue());
    iclad_logic  ->SetVisAttributes(nexus::LightGrey());
    core_logic   ->SetVisAttributes(G4Colour::Green());
    if (with_coating_)
      coating_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  }
  else {
    if (num_claddings_ == 2)
      oclad_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    iclad_logic  ->SetVisAttributes(G4VisAttributes::GetInvisible());
    core_logic   ->SetVisAttributes(G4VisAttributes::GetInvisible());
    if (with_coating_)
      coating_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  }
}


void GenericWLSFiber::BuildSquareFiber()
{
  // For comfort, although we are dealing with G4Boxes, we are going to use
  // "_rad" in variable names instead of most accurate "half_thickness"


  // Pointer to the innermost logical volume defined at any moment.
  G4LogicalVolume* innermost_logic = nullptr;

  // Coating (only if it exists ...)
  G4LogicalVolume* coating_logic;
  if (with_coating_) {
    G4String coating_name = name_ + "_WLS";
    G4Box* coating_solid =
      new G4Box(coating_name, thickness_/2., thickness_/2., length_/2.);
    coating_logic =
      new G4LogicalVolume(coating_solid, coating_mat_, coating_name);

    // Optical surface
    G4OpticalSurface* coating_optSurf =
      new G4OpticalSurface(coating_name + "_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);
    new G4LogicalSkinSurface(coating_name + "_OPSURF", coating_logic,
                             coating_optSurf);

    GeometryBase::SetLogicalVolume(coating_logic);
    innermost_logic = coating_logic;
  }

  // Outer Cladding (only if it exists ...)
  G4LogicalVolume* oclad_logic;
  if (num_claddings_ == 2) {
    G4String oclad_name = name_ + "_OCLAD";
    G4Box* oclad_solid =
      new G4Box(oclad_name, oclad_rad_, oclad_rad_, length_/2.);
    oclad_logic =
      new G4LogicalVolume(oclad_solid, oclad_mat_, oclad_name);

    if (GeometryBase::GetLogicalVolume() == nullptr)
      GeometryBase::SetLogicalVolume(oclad_logic);
    else
      new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), oclad_logic,
                        oclad_name, innermost_logic, false, 0, false);

    innermost_logic = oclad_logic; 
  }

  // Inner Cladding (always built)
  G4String iclad_name = name_ + "_ICLAD";
  G4Box* iclad_solid =
    new G4Box(iclad_name, iclad_rad_, iclad_rad_, length_/2.);
  G4LogicalVolume* iclad_logic =
    new G4LogicalVolume(iclad_solid, iclad_mat_, iclad_name);

  if (GeometryBase::GetLogicalVolume() == nullptr)
    GeometryBase::SetLogicalVolume(iclad_logic);
  else
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), iclad_logic,
                      iclad_name, innermost_logic, false, 0, false);

  // Fiber core (always inside the iclad)
  G4Box* core_solid =
    new G4Box(name_, core_rad_, core_rad_, length_/2.);
  G4LogicalVolume* core_logic =
    new G4LogicalVolume(core_solid, core_mat_, name_);
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), core_logic,
                    name_, iclad_logic, false, 0, false);

  // VISIBILITIES
  if (visibility_) {
    if (num_claddings_ == 2)
      oclad_logic->SetVisAttributes(nexus::LightBlue());
    iclad_logic  ->SetVisAttributes(nexus::LightGrey());
    core_logic   ->SetVisAttributes(G4Colour::Green());
    if (with_coating_)
      coating_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  }
  else {
    if (num_claddings_ == 2)
      oclad_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    iclad_logic  ->SetVisAttributes(G4VisAttributes::GetInvisible());
    core_logic   ->SetVisAttributes(G4VisAttributes::GetInvisible());
    if (with_coating_)
      coating_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  }
}
