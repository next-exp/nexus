// ----------------------------------------------------------------------------
///  \file 
///  \brief
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     29 August 2013
///  \version  $Id: Lab.h 9593 2014-02-13 16:38:56Z paola $
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef LAB_H
#define LAB_H

#include "BaseGeometry.h"

#include <TFile.h>

class G4GenericMessenger;
namespace nexus {class Pet2boxes;}


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
    std::pair<G4ThreeVector, G4ThreeVector> GenerateVertices(const G4String& /*region*/) const;

    virtual void Construct();

  private:
    /// Messenger for the definition of control commands
    G4GenericMessenger* _msg;

    Pet2boxes* module_;

    TFile* file_;
    G4float px1_, py1_, pz1_, px2_, py2_, pz2_;
    mutable G4int index_;
    mutable std::vector<std::pair<G4ThreeVector, G4ThreeVector> > vertices_;
    
  };

} // end namespace nexus

#endif
