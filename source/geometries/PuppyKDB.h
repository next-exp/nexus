#ifndef PUPPY_KDB_H
#define PUPPY_KDB_H

#include "BaseGeometry.h"
#include <G4Material.hh>
#include <vector>


class G4GenericMessenger;
namespace nexus {class SiPMpet9mm2;}

namespace nexus {

  /// Geometry of the Kapton Dice Boards (KDBFixedPitch) used in the NEW detector

  class PuppyKDB: public BaseGeometry
  {
  public:
    /// Constructor
    PuppyKDB();

    /// Destructor
    ~PuppyKDB();

    // Dimension setter
    //   void SetXYsize(G4double xysize);
    //   void SetPitchSize(G4double pitch);

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

    // xy dimensions
    G4double xy_dim_;
    
    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;    

    SiPMpet9mm2* sipm_;
 
  };

} // end namespace nexus

#endif
