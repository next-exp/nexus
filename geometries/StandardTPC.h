//
//  StandardTPC.h
//
//     Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 29 Mar 2009
//     Updated: 
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#ifndef __STANDARD_TPC__
#define __STANDARD_TPC__

#include "BaseGeometry.h"

class G4LogicalVolume;
namespace bhep {class gstore;}


namespace nexus {

  class StandardTPC: public BaseGeometry
  {
  public:
    /// Constructor
    StandardTPC();
    /// Destructor
    ~StandardTPC() {}

    G4LogicalVolume* GetLogicalVolume();

    G4ThreeVector GetRandomVertex(const G4String&);

  private:
    void Initialize();
    G4LogicalVolume* DefineGeometry();

  private:
    const bhep::gstore& _input;
    G4LogicalVolume* _logVol;
  };

} // end namespace nexus

#endif
