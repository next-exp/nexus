//
//  GenbbInterface.h
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 27 Mar 2009
//     Updated:    Mar 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#ifndef __GENBB_INTERFACE__
#define __GENBB_INTERFACE__

#include <G4VPrimaryGenerator.hh>

class G4Event;
namespace bhep {class gstore;}


namespace nexus {

  class GenbbInterface: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    GenbbInterface();
    /// Destructor
    ~GenbbInterface() {}
    
    void GeneratePrimaryVertex(G4Event*);

  private:
    void Initialize();

  private:
    const bhep::gstore& _input;
  };

} // end namespace nexus

#endif
