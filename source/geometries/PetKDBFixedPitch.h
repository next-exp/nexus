// ----------------------------------------------------------------------------
///  \file   PetKDBFixedPitch.h
///  \brief  Geometry of the Kapton Dice Boards used in the NEW detector.
///
///  \author  <miquel.nebot@ific.uv.es>
///  \date    18 Sept 2013
///  \version $Id$
//
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef PET_KDBFixedPitch_H
#define PET_KDBFixedPitch_H

#include "BaseGeometry.h"
#include <G4Material.hh>
#include <vector>


class G4GenericMessenger;
namespace nexus {class SiPMpetVUV;}
namespace nexus {class SiPMpetTPB;}

namespace nexus {

  /// Geometry of the Kapton Dice Boards (KDBFixedPitch) used in the NEW detector

  class PetKDBFixedPitch: public BaseGeometry
  {
  public:
    /// Constructor
    PetKDBFixedPitch();

    /// Destructor
    ~PetKDBFixedPitch();

    // Dimension setter
    void SetXYsize(G4double xysize);
    void SetPitchSize(G4double pitch);

    G4ThreeVector GetDimensions() const;
    const std::vector<std::pair<int, G4ThreeVector> >& GetPositions();

    /// Builder
    virtual void Construct();

    void SetMaterial(G4Material& mat);

  private:
 
    //G4double _support_thickness;
    G4ThreeVector dimensions_;
    std::vector<std::pair<int, G4ThreeVector> > positions_;
 
    // Visibility of the shielding
    G4bool visibility_;

    // SiPM pitch
    G4double sipm_pitch_;

    // xy dimensions
    G4double xysize_;
    
    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;    
   
    
    // Outer material
    //G4Material* _out_mat;

    // SiPMpetVUV* sipm_;
     SiPMpetTPB* sipm_;
 
  };

} // end namespace nexus

#endif
