// ----------------------------------------------------------------------------
// nexus | Next100Ics.h
//
// Inner copper shielding of the NEXT-100 detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_ICS_H
#define NEXT100_ICS_H

#include "BaseGeometry.h"

#include <G4Navigator.hh>

class G4GenericMessenger;


namespace nexus {

  class CylinderPointSampler;
  class SpherePointSampler;
  class BoxPointSampler;

  class Next100Ics: public BaseGeometry
  {
  public:
    /// Constructor
    Next100Ics(const G4double nozzle_ext_diam,
	       const G4double up_nozzle_ypos,
	       const G4double central_nozzle_ypos,
	       const G4double down_nozzle_ypos,
	       const G4double bottom_nozzle_ypos);

    /// Destructor
    ~Next100Ics();

    /// Sets the Logical Volume where ICS will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);
    
    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();

  private:
    void GenerateDBPositions();


  private:
    // Mother Logical Volume of the ICS
    G4LogicalVolume* mother_logic_;
    // Dimensions
    G4double body_in_rad_, body_length_, body_thickness_;
    G4double tracking_orad_, tracking_length_;
    G4double plug_x_, plug_y_, plug_z_, plug_posz_;
    G4double DB_columns_, num_DBs_;
    //    G4double tracking_cone_height_, tracking_irad_;
    G4double energy_theta_, energy_orad_, energy_thickness_, energy_sph_zpos_, energy_cyl_length_;

    // Dimensions coming from outside  
    G4double nozzle_ext_diam_, up_nozzle_ypos_, central_nozzle_ypos_;
    G4double down_nozzle_ypos_, bottom_nozzle_ypos_;

    std::vector<G4ThreeVector> DB_positions_;


    // Visibility of the shielding
    G4bool visibility_;

    // Vertex generators
    CylinderPointSampler* body_gen_;
    CylinderPointSampler* tracking_gen_;
    CylinderPointSampler* energy_cyl_gen_;
    SpherePointSampler*   energy_sph_gen_;
    BoxPointSampler* plug_gen_;

    G4double perc_body_vol_, perc_tracking_vol_, perc_energy_cyl_vol_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

  };

} // end namespace nexus

#endif
