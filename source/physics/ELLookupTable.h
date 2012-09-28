// ----------------------------------------------------------------------------
///  \file   ELLookupTable.h
///  \brief  
///
///  \author   <justo.martin-albo@ific.uv.es>, <paola.ferrario@ific.uv.es>
///  \date     22 Oct 2011
///  \version  $Id$
///
///  Copyright (c) 2011, 2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __EL_LOOKUP_TABLE__
#define __EL_LOOKUP_TABLE__

#include <G4VUserRegionInformation.hh>
#include <G4ThreeVector.hh>
#include <globals.hh>

#include <vector>
#include <map>


namespace nexus {

  class ELLookupTable: public G4VUserRegionInformation 
  {
  public:
    /// Constructor
    ELLookupTable(G4String);
    /// Destructor
    ~ELLookupTable();

    /// Read input files and store their content in the transient table
    void ReadFiles(G4String);
    
    /// Returns the appropiate sensor map for a given point in the EL gap
    virtual const std::map<int, std::vector<double> >& 
    GetSensorsMap(const G4ThreeVector&);


  private:

    std::vector<std::map<int, std::vector<double> > > _ELtable;

  };

} // end namespace nexus

#endif
