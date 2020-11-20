// -----------------------------------------------------------------------------
// nexus | NextNew.h
//
// Main class that builds the NEXT-WHITE detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXTNEW_H
#define NEXTNEW_H

#include "BaseGeometry.h"
#include <G4RotationMatrix.hh>

namespace nexus {class LSCHallA; }
namespace nexus {class Next100Shielding; }
namespace nexus {class NextNewPedestal; }
namespace nexus {class NextNewMiniCastle; }
namespace nexus {class NextNewVessel; }
namespace nexus {class NextNewIcs; }
namespace nexus {class NextNewInnerElements; }
namespace nexus {class BoxPointSampler;}
namespace nexus {class CylinderPointSampler;}
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

    /// Returns a point within a region projecting from a
    /// given point backwards along a line.
    G4ThreeVector ProjectToRegion(const G4String& region,
				  const G4ThreeVector& point,
				  const G4ThreeVector& dir) const;

  private:
    void BuildExtScintillator(G4ThreeVector pos, const G4RotationMatrix& rot);
    void Construct();

  private:

    // Detector dimensions
    const G4double lab_size_; ///< Size of the air box containing the detector

    //   G4double pressure_; ///< Pressure xenon gas

    // Pointers to logical volumes
    G4LogicalVolume* lab_logic_;
    //  G4LogicalVolume* shielding_air_logic_;
    G4LogicalVolume* air_logic_;
    G4LogicalVolume* hallA_logic_;

    //Detector parts
    LSCHallA* hallA_walls_;
    Next100Shielding* shielding_;
    NextNewPedestal* pedestal_;
    NextNewMiniCastle* mini_castle_;
    NextNewVessel* vessel_;
    NextNewIcs* ics_;
    NextNewInnerElements* inner_elements_;
    NaIScintillator* naI_;
    SurroundingAir* air_;
    LeadCollimator* coll_;
    ExtraVessel* extra_;

    BoxPointSampler* lab_gen_; ///< Vertex generator
    CylinderPointSampler* lat_source_gen_;
    CylinderPointSampler* axial_source_gen_;
    CylinderPointSampler* source_gen_up_;
    CylinderPointSampler* source_gen_lat_;
    CylinderPointSampler* source_gen_random_;

    // Rotation around Y and displacement of the whole geometry in the g4 system of reference
    G4ThreeVector displ_;
    G4double rot_angle_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Kind of block of lead on the lateral port placed to shield source
    G4bool lead_block_; ///< true if the two lead blocks inlateral port are placed as shielding
    G4double lead_dist_; ///< distance between the two pieces of lead
    G4bool ext_scint_; ///< true if external scintillator is placed
    G4String calib_port_; /// position of calibration source (lateral/axial)
    G4double dist_scint_; ///< distance from the end of lateral/axial port tube and scintillator

    // Incapsulated calibration source volume
    CalibrationSource* cal_;
    // Distance from the end of the lateral feedthrough to the source pos
    //   G4double ext_source_distance_;

    G4bool lead_castle_; ///< false if castle is open (e.g., lead collimator in axial port)
    G4bool lab_walls_; ///< false (default) if no wall simulation

    G4bool disk_source_; ///< true if external disk-like calibration source
    G4String source_mat_; ///< Kind of external disk-like calibration source (Na/Th so far)
    DiskSource* source_;
    G4double source_dist_from_anode_;

    G4RotationMatrix* extra_rot_; ///< rotation of the external elements outside the vessel, behind the tracking plane
    G4ThreeVector extra_pos_; ///< position of the external elements outside the vessel, behind the tracking plane

    G4double pedestal_pos_;
  };

} // end namespace nexus

#endif
