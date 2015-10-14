// ----------------------------------------------------------------------------
///  \file   PetaoTrap.h
///  \brief  
///
///  \author   <jmunoz@ific.uv.es>
///  \date     January 2014
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef PETALOTRAP__
#define PETALOTRAP__

#include "BaseGeometry.h"
#include "BoxPointSampler.h"

class G4LogicalVolume;
class G4GenericMessenger;
class G4Material;
namespace nexus{class PetKDBFixedPitch;}
namespace nexus{class PetPlainDice;}
namespace nexus {class BoxPointSampler;}

namespace nexus {

  class PetaloTrap: public BaseGeometry {
  public:
    /// Constructor
    PetaloTrap();

    /// Destructor
    ~PetaloTrap();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;
    void SetParameters(G4double size1, G4double size2, G4double z);

    void Construct();

  private:

    void BuildDetector(); 
    //    void BuildLXe();
    //  void BuildActive(); 
    void BuildSiPMPlanes(); 

    G4LogicalVolume* lab_logic_;
    G4LogicalVolume* det_logic_;
    G4LogicalVolume* lXe_logic_;

    // Detector dimensions
    //   const G4double vacuum_thickn_; // thickness of vaccuum layer between walls
    //  const G4double outer_wall_thickn_; // Thickness of the outer wall
    const G4double det_thickness_; // Thickness of the walls of the detector
    //   const G4double det_size_; /// Size of the detector
    //   const G4double active_size_; /// Size of the LXe active volume   
 
   G4int n_modules_;
   
    // ACTIVE gas Xenon
    G4Material* lXe_;

    // Parameters
    G4double max_step_size_;  /// Maximum Step Size
    
    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

    G4double db_z_;

    G4double z_size_;
    G4double ring_diameter_;
    G4double size1_, size2_;

    PetKDBFixedPitch* db_;

  };
  
} // end namespace nexus

#endif
