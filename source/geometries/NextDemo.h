// ----------------------------------------------------------------------------
///  \file   NextDemo.h   
///  \brief  Geometry of the DEMO++ detector.
///          Inheriting from previous code in Next1EL.cc
///
///  \author   <neuslopezmarch@gmail.com>
///  \date     16 Feb 2018
///  \version  $Id$
///
///  Copyright (c) 2010, 2018 NEXT Collaboration
//
//  Updated to NextDemo++  by  Ruth Weiss Babai <ruty.wb@gmail.com>
//  Date:    June 2019
//  Add "energy_plane"  &  "tracking_endcap"
// ----------------------------------------------------------------------------

#ifndef __NEXT_DEMO__
#define __NEXT_DEMO__

#include "BaseGeometry.h"
#include "NextDemoVessel.h"
#include "NextDemoInnerElements.h"

#include <G4RotationMatrix.hh>
#include <vector>

class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;
//class G4UIcmdWith3VectorAndUnit;


namespace nexus { 
  class NextDemoVessel;
  class NextDemoInnerElements;

  class Na22Source;
  class NaIScintillator;
  class DiskSource;
  }


namespace nexus {

  class BoxPointSampler;
  class CylinderPointSampler;
  class MuonsPointSampler;
  
  /// Geometry of the NEXTDEMO detector
    
  class NextDemo: public BaseGeometry
  {
  public:
    /// Constructor
    NextDemo();
    /// Destructor
    ~NextDemo();

    /// Returns a vertex in a region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const override;
    void Construct() override;

  private:
    void BuildExtScintillator(G4ThreeVector pos, const G4RotationMatrix& rot);
    void BuildExtSource(G4ThreeVector pos, const G4RotationMatrix& rot);
 

  private:

    // Detector dimensions
    
    const G4double lab_size_; ///< Size of the air box containing the detector

    const G4double vessel_diam_;
    const G4double vessel_length_; //length of the vessel to be used also in NextDemoFieldCage.cc
    const G4double vessel_thickn_;

    //const G4double _rot_angle;

    const G4double endcap_diam_;   ///< Vessel endcap diameter
    const G4double endcap_thickn_; ///< Vessel endcap thickness
    
    // Pointers to logical volumes
    G4LogicalVolume* lab_logic_;


    void BuildLab();
    void BuildMuons();

    //Detector parts
    NextDemoVessel* vessel_;                // Ruty
    NextDemoInnerElements* inner_elements_; // Ruty
    NaIScintillator* naI_;                  // Ruty

    
  private:
    CylinderPointSampler* source_gen_side_;

 
    // Pointers to materials definition
    G4Material* air_;       ///< Air
    G4Material* steel_;     ///< Stainless Steel
  

    // Dimensions read through input parameters file
    // TO CHECK WHAT CAN BE COMMENT OUT WHILE WRITEN IN VESSEL ONLY !!!
//////////////////////////////////////////////////////////////   
    G4double axialport_diam_;   ///< Axial source-port internal diameter
    G4double axialport_length_; ///< Axial source-port internal length
    G4double axialport_thickn_; ///< Axial source-port internal thickness
    /*
    G4double axialport_flange_diam_;   ///< Axial port flange diameter
    G4double axialport_flange_thickn_; ///< Axial port flange thickness
    
    G4double axialport_tube_diam_;   ///< Collimation tube int. diameter
    G4double axialport_tube_length_; ///< Collimation tube int. length
    G4double axialport_tube_thickn_; ///< Collimation tube thickness
    G4double axialport_tube_window_thickn_; ///< Tube window thickness
    */
/////////////////////////////////////////////////////////////////

    G4bool verbosity_;
    //  For External Source
    G4double sideport_angle_;

    G4String calib_port_; /// position of calibration source (lateral/axial)
    G4bool disk_source_; ///< true if external disk-like calibration source
    G4String source_mat_; ///< Kind of external disk-like calibration source (Na/Th so far)
    DiskSource* source_;

    G4bool ext_scint_; ///< true if external scintillator is placed
    G4double dist_scint_; ///< distance from the end of lateral/axial port tube and scintillator

    //G4ThreeVector _specific_vertex;
    //G4double _specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z;
    G4bool muonsGenerator_;
    
    // Positions in the detector geometry relevant for event vertex generation
    //
    
    /// Position of the side source-port in the world reference system
    G4ThreeVector sideport_ext_position_;
    //G4ThreeVector _sideNa_pos;

    /// Position of the axial source-port in the world reference system
    //G4ThreeVector _axialport_position;
    /// Position of the fieldcage in the world system of reference
    ///    or active
    //G4ThreeVector _active_position;
   

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

    /// To generate vertices in the Na22 source
    // CylinderPointSampler* _cps; // old

    /// Position of gate in its mother volume
    G4double gate_zpos_in_vessel_;

    G4double specific_vertex_X_;
    G4double specific_vertex_Y_;
    G4double specific_vertex_Z_;

  };


} // end namespace nexus

#endif
