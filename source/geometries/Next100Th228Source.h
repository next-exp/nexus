// ----------------------------------------------------------------------------
// nexus | Next100Th228Source.h
//
// Th-228 calibration specific source used at LSC with NEXT-100.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_TH228_SOURCE_H
#define NEXT100_TH228_SOURCE_H

#include "GeometryBase.h"

#include <globals.hh>

namespace nexus {

  class Next100Th228Source: public GeometryBase {
  public:
    /// Constructor
    Next100Th228Source();

    /// Destructor
    ~Next100Th228Source();

    void Construct();

    G4double GetSupportLength() const;
    // Distance between center of Th and the end of the support 
    G4double GetDiffThZPosEnd() const;
    G4double GetThRadius() const;

  private:
    const G4double source_diam_, source_offset_;
    const G4double support_ext_diam_, support_int_diam_, support_length_;
    const G4double support_screw_length_;


  };
}
#endif

