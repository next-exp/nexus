//
//  DetectorConstruction.h
//
//     Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 9 Mar 2009
//     Updated: 
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#ifndef __MATERIALS_LIST__
#define __MATERIALS_LIST__

#include <globals.hh>


namespace nexus {

  class MaterialsList
  {
  public:
    ///
    static void DefineMaterials();
    ///
    static G4double IdealGasLaw(G4double, G4double, G4double);
    
  private:
    /// Constructor (hidden)
    MaterialsList();
    /// Destructor (hidden)
    ~MaterialsList();
  };

} // end namespace nexus

#endif
