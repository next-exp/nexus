// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <miquel.nebot@ific.uv.es>
///  \date     25 Jul 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEW__
#define __NEW__

#include "BaseGeometry.h"
#include "BoxPointSampler.h"
#include "CylinderPointSampler.h"

#include "Next100Shielding.h"
#include "Next100Vessel.h"
#include "Next100Ics.h"
#include "Next100InnerElements.h"

class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

  class NEW: public BaseGeometry {
  public:
    /// Constructor
    NEW();

    /// Destructor
    ~NEW();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;


  private:
    void BuildLab();
    void Construct();

  private:
    // Detector dimensions
    const G4double _lab_size; /// Size of the air box containing the detector 
    const G4double _buffer_gas_size;///Mother volume of shielding and vessel 
    G4double _vessel_dim; ///Vessel Diameter
    G4double _vessel_length; /// vessel length
    G4double _ics_width; /// inner copper shielding width
    G4double _pressure;  ///Pressure Gas Xenon

    // Pointers to logical volumes
    G4LogicalVolume* _lab_logic;
    G4LogicalVolume* _buffer_gas_logic;

    // Vertex generators
    BoxPointSampler* _lab_gen;
    CylinderPointSampler* _ics_gen;
    CylinderPointSampler* _xefv_gen;


    /// Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };
  
} // end namespace nexus

#endif
