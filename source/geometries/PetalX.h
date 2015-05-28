// ----------------------------------------------------------------------------
///  \file   PetalX.h
///  \brief  
///
///  \author   <jmunoz@ific.uv.es>
///  \date     January 2014
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef PETALX__
#define PETALX__

#include "BaseGeometry.h"
#include "BoxPointSampler.h"

class G4LogicalVolume;
class G4GenericMessenger;
class G4Material;


namespace nexus {

  class PetalX: public BaseGeometry {
  public:
    /// Constructor
    PetalX();

    /// Destructor
    ~PetalX();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    void Construct();

  private:

    void BuildDetector(); 
    void BuildActive(); 
    void BuildSiPMPlane(); 

    G4LogicalVolume* lab_logic_;
    G4LogicalVolume* det_logic_;

    // Detector dimensions
    const G4double det_size_; /// Size of the detector
    const G4double active_size_; /// Size of the Xe active volume   

    // ACTIVE gas Xenon
    G4Material* lXe_;

    // Parameters
    G4double max_step_size_;  /// Maximum Step Size
    
    //Vertex generator    
    BoxPointSampler* active_gen_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

  };
  
} // end namespace nexus

#endif
