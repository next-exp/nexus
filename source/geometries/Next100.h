// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <justo.martin-albo@ific.uv.es>, <jmunoz@ific.uv.es>
///  \date     21 Nov 2011
///  \version  $Id$
///
///  Copyright (c) 2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT100__
#define __NEXT100__

#include "BaseGeometry.h"
#include "Next100Shielding.h"
#include "Next100Vessel.h"
#include "Next100Ics.h"
/* #include "Next100FieldCage.h" */
/* #include "Next100InternalVolume.h" */
#include "Next100InnerElements.h"


class G4LogicalVolume;


namespace nexus {


  class Next100: public BaseGeometry
  {
  public:
    /// Constructor
    Next100();

    /// Destructor
    ~Next100();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;


  private:
    void BuildLab();

    
  private:
    // Detector dimensions
    const G4double _lab_size;          /// Size of the air box containing the detector 
    const G4double _buffer_gas_size;   /// Size of the mother volume of shielding and vessel 

    // External diameter of nozzles and y positions
    const G4double _nozzle_ext_diam; 
    const G4double _up_nozzle_ypos, _central_nozzle_ypos, _down_nozzle_ypos, _bottom_nozzle_ypos;


    // Pointers to logical volumes
    G4LogicalVolume* _lab_logic;
    G4LogicalVolume* _buffer_gas_logic;

    // Detector parts
    Next100Shielding* _shielding;
    Next100Vessel*    _vessel;
    Next100Ics*       _ics;
    /* Next100FieldCage* _field_cage; */
    /* Next100InternalVolume* _internal_volume; */
    Next100InnerElements* _inner_elements;

  };

} // end namespace nexus

#endif
