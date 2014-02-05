// ----------------------------------------------------------------------------
///  \file   NextNew.h
///  \brief  
///
///  \author   <miquel.nebot@ific.uv.es>, <jmunoz@ific.uv.es>, <justo.martin-albo@ific.uv.es>
///  \date     Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXTNEW__
#define __NEXTNEW__

#include "BaseGeometry.h"
#include "BoxPointSampler.h"

namespace nexus { class Next100Shielding; }
namespace nexus { class NextNewPedestal; }
namespace nexus { class NextNewCuCastle; }
namespace nexus { class NextNewRnTube; }
namespace nexus { class NextNewVessel; }
namespace nexus { class NextNewIcs; }
namespace nexus { class NextNewInnerElements; }

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
    void BuildLab();
    void Construct();

  private:
    // Detector dimensions
    const G4double _lab_size; /// Size of the air box containing the detector 
    const G4double _buffer_gas_size;///Mother volume of shielding and vessel 
   
    G4double _pressure;  ///Pressure Gas Xenon

    // Pointers to logical volumes
    G4LogicalVolume* _lab_logic;
    G4LogicalVolume* _buffer_gas_logic;

    //Detector parts
    Next100Shielding* _shielding;
    NextNewPedestal* _pedestal;
    NextNewCuCastle* _cu_castle;
    NextNewRnTube* _rn_tube;
    NextNewVessel* _vessel;   
    NextNewIcs* _ics;
    NextNewInnerElements* _inner_elements;
    
    //Vertex genrator    
    BoxPointSampler* _lab_gen;

    /// Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };
  
} // end namespace nexus

#endif
