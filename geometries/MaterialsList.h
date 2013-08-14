// ----------------------------------------------------------------------------
///  \file   MaterialsList.cc
///  \brief  Definition of common materials
///  
///  \author   J Martin-Albo <jmalbos@ific.uv.es>
///  \date     27 Mar 2009
///  \version  $Id$     
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __MATERIALS_LIST__
#define __MATERIALS_LIST__

#include <globals.hh>

class G4MaterialPropertiesTable;
class G4Material;

namespace nexus {

  /// Stateless class with static methods for the definition of materials

  class MaterialsList
  {
  public:

    static G4Material* GXe(G4double pressure = STP_Pressure,
			   G4double temperature = STP_Temperature);

    static G4Material* Steel();

    static G4Material* Epoxy();
 
  private:
    /// Constructor (hidden)
    MaterialsList();
    /// Destructor (hidden)
    ~MaterialsList();
  };

} // end namespace nexus

#endif
