// ----------------------------------------------------------------------------
///  \file   NextNew.h
///  \brief  
///
///  \author   <miquel.nebot@ific.uv.es>, <jmunoz@ific.uv.es>, 
///            <justo.martin-albo@ific.uv.es>
///  \date     Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013-2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXTNEW_H
#define NEXTNEW_H

#include "BaseGeometry.h"
#include <G4RotationMatrix.hh>

namespace nexus { class Next100Shielding; }
namespace nexus { class NextNewPedestal; }
namespace nexus { class NextNewMiniCastle; }
namespace nexus { class NextNewVessel; }
namespace nexus { class NextNewIcs; }
namespace nexus { class NextNewInnerElements; }
namespace nexus {class BoxPointSampler;}
namespace nexus {class CylinderPointSampler;}
namespace nexus {class MuonsPointSampler;}
namespace nexus {class CalibrationSource;}
namespace nexus {class NaIScintillator;}
namespace nexus {class SurroundingAir;}
namespace nexus {class LeadCollimator;}
namespace nexus {class DiskSource;}
namespace nexus {class ExtraVessel;}


class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

  class NextNew: public BaseGeometry {
  public:
    /// Constructor
    NextNew();

    /// Destructor
    ~NextNew();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    void BuildExtScintillator(G4ThreeVector pos, const G4RotationMatrix& rot);
    void Construct();

  private:
    
    // Detector dimensions
    const G4double _lab_size; ///< Size of the air box containing the detector  
   
    //   G4double _pressure; ///< Pressure xenon gas

    // Pointers to logical volumes
    G4LogicalVolume* _lab_logic;
    //  G4LogicalVolume* _shielding_air_logic;
    G4LogicalVolume* _air_logic;

    //Detector parts
    Next100Shielding* _shielding;
    NextNewPedestal* _pedestal;
    NextNewMiniCastle* _mini_castle;
    NextNewVessel* _vessel;   
    NextNewIcs* _ics;
    NextNewInnerElements* _inner_elements;
    NaIScintillator* _naI;
    SurroundingAir* _air;
    LeadCollimator* _coll;
    ExtraVessel* _extra;
    
    BoxPointSampler* _lab_gen; ///< Vertex generator
    CylinderPointSampler* _lat_source_gen;
    CylinderPointSampler* _axial_source_gen;
    CylinderPointSampler* _source_gen_up;
    CylinderPointSampler* _source_gen_lat;
    CylinderPointSampler* _source_gen_random;
    MuonsPointSampler* _muon_gen; ///< Vertex generator for muons

    /// Messenger for the definition of control commands
    //  G4GenericMessenger* _msg;

    // Rotation around Y and displacement of the whole geometry in the g4 system of reference
    G4ThreeVector _displ;
    G4double _rot_angle;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

    // Kind of block of lead on the lateral port placed to shield source
    G4bool _lead_block; ///< true if the two lead blocks inlateral port are placed as shielding
    G4double _lead_dist; ///< distance between the two pieces of lead
    G4bool _ext_scint; ///< true if external scintillator is placed
    G4String _calib_port; /// position of calibration source (lateral/axial)
    G4double _dist_scint; ///< distance from the end of lateral/axial port tube and scintillator
    
    // Incapsulated calibration source volume
    CalibrationSource* _cal;
    // Distance from the end of the lateral feedthrough to the source pos
    //   G4double _ext_source_distance;

    G4bool _lead_castle; ///< false if castle is open (e.g., lead collimator in axial port)

    G4bool _disk_source; ///< true if external disk-like calibration source
    G4String _source_mat; ///< Kind of external disk-like calibration source (Na/Th so far)
    DiskSource* _source;
    G4double _source_dist_from_anode;

    G4RotationMatrix* _extra_rot; ///< rotation of the external elements outside the vessel, behind the tracking plane
    G4ThreeVector _extra_pos; ///< position of the external elements outside the vessel, behind the tracking plane
    
  };
  
} // end namespace nexus

#endif
