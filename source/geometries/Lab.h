// ----------------------------------------------------------------------------
// nexus | Lab.h
//
// This class consists of two LXe cells placed opposite to each other.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef LAB_H
#define LAB_H

#include "BaseGeometry.h"

#include <TFile.h>

class G4GenericMessenger;
namespace nexus {class Pet2boxes;}
namespace nexus {class PetLYSObox;}
namespace nexus {class PetLXeCell;}
namespace nexus {class PetLYSOCell;}

namespace nexus {

  class Lab: public BaseGeometry
  {
  public:
    /// Constructor
    Lab();
    /// Destructor
    ~Lab();

    /// Return vertex within region <region> of the chamber
    virtual G4ThreeVector GenerateVertex(const G4String& region) const;

    virtual void Construct();

  private:
    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    PetLXeCell* module_;

  };

} // end namespace nexus

#endif
