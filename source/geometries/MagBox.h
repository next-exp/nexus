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

#ifndef __MAGBOX__
#define __MAGBOX__

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
    const G4double _detector_size; /// Size of the Xe box    

    // ACTIVE gas Xenon
    G4Material* _gas;

    // Parameters
    G4double _max_step_size;  /// Maximum Step Size
    G4String _gas_name;       /// Gas name
    G4double _pressure;       /// Pressure Gas Xenon
    G4double _mag_intensity;  /// Magnetic Field Intensity
    
    //Vertex genrator    
    BoxPointSampler* _active_gen;

    /// Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };
  
} // end namespace nexus

#endif
