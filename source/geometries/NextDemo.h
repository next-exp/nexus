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
    
    const G4double _lab_size; ///< Size of the air box containing the detector

    const G4double _vessel_diam;
    const G4double _vessel_length; //length of the vessel to be used also in NextDemoFieldCage.cc
    const G4double _vessel_thickn;

    //const G4double _rot_angle;

    const G4double _endcap_diam;   ///< Vessel endcap diameter
    const G4double _endcap_thickn; ///< Vessel endcap thickness
    
    // Pointers to logical volumes
    G4LogicalVolume* _lab_logic;


    void BuildLab();
    void BuildMuons();

    //Detector parts
    NextDemoVessel* _vessel;                // Ruty
    NextDemoInnerElements* _inner_elements; // Ruty
    NaIScintillator* _naI;                  // Ruty

    
  private:
    CylinderPointSampler*_source_gen_side;
    MuonsPointSampler* _muons_sampling;

 
    // Pointers to materials definition
    G4Material* _air;       ///< Air
    G4Material* _steel;     ///< Stainless Steel
  

    // Dimensions read through input parameters file
    // TO CHECK WHAT CAN BE COMMENT OUT WHILE WRITEN IN VESSEL ONLY !!!
//////////////////////////////////////////////////////////////   
    G4double _axialport_diam;   ///< Axial source-port internal diameter
    G4double _axialport_length; ///< Axial source-port internal length
    G4double _axialport_thickn; ///< Axial source-port internal thickness
    /*
    G4double _axialport_flange_diam;   ///< Axial port flange diameter
    G4double _axialport_flange_thickn; ///< Axial port flange thickness
    
    G4double _axialport_tube_diam;   ///< Collimation tube int. diameter
    G4double _axialport_tube_length; ///< Collimation tube int. length
    G4double _axialport_tube_thickn; ///< Collimation tube thickness
    G4double _axialport_tube_window_thickn; ///< Tube window thickness
    */
/////////////////////////////////////////////////////////////////

    G4bool _verbosity;
    //  For External Source
    G4double _sideport_angle;

    G4String _calib_port; /// position of calibration source (lateral/axial)
    G4bool _disk_source; ///< true if external disk-like calibration source
    G4String _source_mat; ///< Kind of external disk-like calibration source (Na/Th so far)
    DiskSource* _source;

    G4bool _ext_scint; ///< true if external scintillator is placed
    G4double _dist_scint; ///< distance from the end of lateral/axial port tube and scintillator

    //G4ThreeVector _specific_vertex;
    //G4double _specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z;
    G4bool _muonsGenerator;
    
    // Positions in the detector geometry relevant for event vertex generation
    //
    
    /// Position of the side source-port in the world reference system
    G4ThreeVector _sideport_ext_position;
    //G4ThreeVector _sideNa_pos;

    /// Position of the axial source-port in the world reference system
    //G4ThreeVector _axialport_position;
    /// Position of the fieldcage in the world system of reference
    ///    or active
    //G4ThreeVector _active_position;
   

    /// Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

    /// To generate vertices in the Na22 source
    // CylinderPointSampler* _cps; // old
  };


} // end namespace nexus

#endif
