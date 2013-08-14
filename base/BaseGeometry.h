//
//  BaseGeometry.h
//
//     Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 27 Mar 2009
//     Updated:  7 Apr 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#ifndef __BASE_GEOMETRY__
#define __BASE_GEOMETRY__

#include <G4ThreeVector.hh>

class G4LogicalVolume;
namespace bhep {class gstore;}


namespace nexus {

  class BaseGeometry
  {
  public:
    /// Constructor
    BaseGeometry() {}
    /// Destructor
    virtual ~BaseGeometry() {};

    /// Return logical volume representing the geometry
    virtual G4LogicalVolume* GetLogicalVolume() = 0;
    
    ///
    virtual G4ThreeVector GetRandomVertex(const G4String&) = 0;
  };

} // end namespace nexus

#endif
