// ----------------------------------------------------------------------------
// nexus | ELLookupTable.h
//
// This class describes the generation of the EL light.
//
// ****************************** WARNING *******************************
// ******************* This class has been deprecated *******************
// **********************************************************************
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef EL_LOOKUP_TABLE_H
#define EL_LOOKUP_TABLE_H

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

    std::vector<std::map<int, std::vector<double> > > ELtable_;

  };

} // end namespace nexus

#endif
