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

namespace nexus{class PetitPlainDice;}
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
    G4ThreeVector GetParameters() const;

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
    //   const G4double active_size_; /// Size of the LXe active volume
    const G4double internal_diam_; /// internal diameter of ring
 
   G4int n_cells_;
   
    // ACTIVE gas Xenon
    G4Material* lXe_;

    // Parameters
    G4double max_step_size_;  /// Maximum Step Size
    
    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

    G4double db_z_;

    G4double r_dim_; /// size of cells in radial dimension
    G4double ring_diameter_;
    G4double dim_int_, dim_ext_;

    PetitPlainDice* pdb_;

  };
  
} // end namespace nexus

#endif
