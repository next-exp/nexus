// ----------------------------------------------------------------------------
///  \file   MagBox.h
///  \brief  
///
///  \author   <jmunoz@ific.uv.es>
///  \date     January 2014
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef _MAGBOX___
#define _MAGBOX___

#include "BaseGeometry.h"
#include "BoxPointSampler.h"

class G4LogicalVolume;
class G4GenericMessenger;
class G4Material;


namespace nexus {

  class MagBox: public BaseGeometry {
  public:
    /// Constructor
    MagBox();

    /// Destructor
    ~MagBox();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    void Construct();

  private:
    // Detector dimensions
    const G4double detector_size_; /// Size of the Xe box    

    // ACTIVE gas Xenon
    G4Material* gas_;

    // Parameters
    G4double max_step_size_;  /// Maximum Step Size
    G4String gas_name_;       /// Gas name
    G4double pressure_;       /// Pressure Gas Xenon
    G4double mag_intensity_;  /// Magnetic Field Intensity
    
    //Vertex genrator    
    BoxPointSampler* active_gen_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

  };
  
} // end namespace nexus

#endif
