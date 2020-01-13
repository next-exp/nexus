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
#include "Next100InnerElements.h"

class G4LogicalVolume;
class G4GenericMessenger;

namespace nexus {class BoxPointSampler;}
namespace nexus {class MuonsPointSampler;}


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
    void Construct();


  private:
    // Detector dimensions
    const G4double _lab_size;          /// Size of the air box containing the detector

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
    Next100InnerElements* _inner_elements;

    BoxPointSampler* _lab_gen; ///< Vertex generator
    MuonsPointSampler* _muon_gen; ///< Vertex generator for muons

    /// Messenger for the definition of control commands
    G4GenericMessenger* _msg;

    /// Specific vertex for AD_HOC region
    G4double _specific_vertex_X;
    G4double _specific_vertex_Y;
    G4double _specific_vertex_Z;

    /// Position of gate in its mother volume
    G4double _gate_zpos_in_vessel;
  };

} // end namespace nexus

#endif
