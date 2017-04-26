#include "LeadCollimator.h"
#include "Visibilities.h"

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>

namespace nexus {
  using namespace CLHEP;

  LeadCollimator::LeadCollimator() : _axis_centre(0.*mm), _length(0.*mm)
  {
  }

  LeadCollimator::~LeadCollimator()
  {
  }

  void LeadCollimator::Construct()
  {

     G4double offset_sub = 1.*mm;
     G4double offset_union = 1.*mm;

       // Freddy lead block around nozzle

      G4double lead_size_horizontal = 103.*mm;
      G4double lead_size_vertical = 100.*mm;

      G4double lateral_nozzle_flange_diam = 75.*mm;
      G4double lateral_port_tube_diam = 10.*mm;
      G4double lateral_port_tube_thick = 1.*mm;

      G4double middle_lead_thick = 50.4*mm;
      G4double outer_and_middle_lead_thick = 75.3*mm;
      
      G4double inner_lead_thick = 16.*mm;
      G4double screw_lead_thick = 5.*mm;
      G4double nozzle_lead_thick = inner_lead_thick - 5.*mm;
      
      G4double lateral_nozzle_diam = 30.*mm + 2.*3.*mm;
      // G4double offset_sub = 1.*mm;


      // Piece of lead around nozzle
      G4Box* lead_nozzle_solid = new G4Box("LEAD_COLLIMATOR", lead_size_horizontal/2., lead_size_vertical/2.,  nozzle_lead_thick/2.);
      G4Tubs* lateral_nozzle_solid = 
       	new G4Tubs("NOZZLE_INNER", 0., lateral_nozzle_diam/2., (nozzle_lead_thick + offset_sub)/2., 0., twopi);    
      G4SubtractionSolid* lead_nozzle_freddy_solid =
	new G4SubtractionSolid("LEAD_COLLIMATOR", lead_nozzle_solid, lateral_nozzle_solid, 0 , G4ThreeVector(0.,0.,0.));

      _axis_centre = nozzle_lead_thick/2.;

      // Piece of lead around screws
      G4Box* lead_screw_flange_solid = new G4Box("LEAD_BLOCK_SCREW_FLANGE_FULL", lead_size_horizontal/2., lead_size_vertical/2.,  (screw_lead_thick + offset_union)/2.);
      G4Tubs* screw_flange_solid = 
      	new G4Tubs("SCREW_FLANGE_INNER", 0., lateral_nozzle_flange_diam/2., (screw_lead_thick + offset_union + offset_sub)/2., 0., twopi);      
      G4SubtractionSolid* lead_screw_freddy_solid =
       	new G4SubtractionSolid("LEAD_BLOCK_SCREW_FLANGE", lead_screw_flange_solid, screw_flange_solid, 0 , G4ThreeVector(0., 0., 0.));

      G4ThreeVector union_pos(0, 0, -nozzle_lead_thick/2. - screw_lead_thick/2. + offset_union/2.);
      G4UnionSolid* union_solid = new G4UnionSolid("LEAD_COLLIMATOR", lead_nozzle_freddy_solid, lead_screw_freddy_solid, 0, union_pos);


       // Middle piece of lead
      G4Box* lead_middle_solid = new G4Box("LEAD_BLOCK_MIDDLE_FULL", lead_size_horizontal/2., lead_size_vertical/2.,  (middle_lead_thick + offset_union)/2.);
      G4Tubs* lateral_port_tube_middle_solid = 
	new G4Tubs("LAT_PORT_TUBE_MIDDLE", 0., lateral_nozzle_flange_diam/2., (middle_lead_thick + offset_union + offset_sub)/2., 0., twopi);
      G4SubtractionSolid* lead_middle_freddy_solid =
	new G4SubtractionSolid("LEAD_BLOCK_MIDDLE", lead_middle_solid, lateral_port_tube_middle_solid, 0 , G4ThreeVector(0.,0.,0.));

      union_pos = G4ThreeVector(0, 0, -nozzle_lead_thick/2. - screw_lead_thick - middle_lead_thick/2. + offset_union/2.);
      union_solid = new G4UnionSolid("LEAD_COLLIMATOR", union_solid, lead_middle_freddy_solid, 0, union_pos);

      // Outer piece of lead
      G4double outer_lead_thick = outer_and_middle_lead_thick- middle_lead_thick;
      G4Box* lead_out_solid =
	new G4Box("LEAD_BLOCK_OUT_FULL", lead_size_horizontal/2., lead_size_vertical/2., (outer_lead_thick + offset_union)/2.);
      G4Tubs* lateral_port_tube_out_solid = 
      	new G4Tubs("LAT_PORT_TUBE_OUT", 0., (lateral_port_tube_diam + 2.*lateral_port_tube_thick)/2., (outer_lead_thick + offset_union + offset_sub)/2., 0., twopi);
      G4SubtractionSolid* lead_out_freddy_solid =
	new G4SubtractionSolid("LEAD_BLOCK_OUT", lead_out_solid, lateral_port_tube_out_solid, 0 , G4ThreeVector(0.,0.,0.));

      union_pos = G4ThreeVector(0, 0, -nozzle_lead_thick/2. - screw_lead_thick - middle_lead_thick - outer_lead_thick/2. + offset_union/2.);
      union_solid = new G4UnionSolid("LEAD_COLLIMATOR", union_solid, lead_out_freddy_solid, 0, union_pos);

      // Jordi lead collimator block     
      G4double diam = 5.*mm;     
      G4double coll_thick = 50.*mm;
      G4Box* lead_coll_full_solid = 
	new G4Box("LEAD_COLL", 100.*mm/2., 100.*mm/2.,  (coll_thick + offset_union)/2.);
      G4Tubs* cylinder_coll_1_solid = 
       	new G4Tubs("CYLINDER_COLL1", 0., diam/2., (coll_thick + offset_union + offset_sub)/2., 0., twopi); 
      G4SubtractionSolid* lead_coll_solid =
	new G4SubtractionSolid("LEAD_COLL",
                               lead_coll_full_solid, cylinder_coll_1_solid, 
			       0 , G4ThreeVector(0.,0.,0.));

      union_pos = G4ThreeVector(0, 0, -nozzle_lead_thick/2. - screw_lead_thick - middle_lead_thick - outer_lead_thick - coll_thick/2. + offset_union/2.);
      union_solid = new G4UnionSolid("LEAD_COLLIMATOR", union_solid, lead_coll_solid, 0, union_pos);

       // Jordi lead source block     
      diam = 10.*mm;     
      G4double source_thick = 48.*mm;
      G4Box* lead_source_full_solid = 
	new G4Box("LEAD_SOURCE", 100.*mm/2., 100.*mm/2.,  (source_thick + offset_union)/2.);
      G4Tubs* cylinder_coll_2_solid = 
       	new G4Tubs("CYLINDER_COLL2", 0., diam/2., (source_thick + offset_union + offset_sub)/2., 0., twopi); 
      G4SubtractionSolid* lead_source_solid =
	new G4SubtractionSolid("LEAD_SOURCE",
                               lead_source_full_solid, cylinder_coll_2_solid, 
			       0 , G4ThreeVector(0.,0.,0.));

      union_pos = G4ThreeVector(0, 0, -nozzle_lead_thick/2. - screw_lead_thick - middle_lead_thick - outer_lead_thick - coll_thick - source_thick/2. + offset_union/2.);
      union_solid = new G4UnionSolid("LEAD_COLLIMATOR", union_solid, lead_source_solid, 0, union_pos);

      _length = nozzle_lead_thick + screw_lead_thick + outer_and_middle_lead_thick + coll_thick + source_thick;


      G4LogicalVolume* union_logic = new G4LogicalVolume(union_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb"), "LEAD_COLLIMATOR");
      this->SetLogicalVolume(union_logic);

      // G4VisAttributes blue_col = nexus::Blue();
      // blue_col.SetForceSolid(true);
      // union_logic->SetVisAttributes(blue_col);
  }

  G4double LeadCollimator::GetAxisCentre()
  {
    return _axis_centre;
  }

  G4double LeadCollimator::GetLength()
  {
    return _length;
  }
  
}
