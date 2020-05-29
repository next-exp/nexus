// ----------------------------------------------------------------------------
///  \file   PMT_QE_setup.h
///  \brief  A set-up to measure the quantum efficiency of PMTs.
///
///  \author   P. Ferrario <paolafer@ific.uv.es>    
///  \date     14 December 2012
///  \version  $Id: PMT_QE_setup.h  $
///
///  Copyright (c) 2012 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef _PMT_QE_SETUP___
#define _PMT_QE_SETUP___

#include "BaseGeometry.h"
#include "PmtR7378A.h"
#include "PmtR11410.h"

class G4GenericMessenger;

namespace nexus {

  /// Geometry of a square tracking chamber filled with gaseous xenon

  class PMT_QE_setup: public BaseGeometry
  {
  public:
    /// Constructor
    PMT_QE_setup();
    /// Destructor
    ~PMT_QE_setup();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    /// Define the volumes of the geometry
    void Construct();

  private:   
    G4GenericMessenger* msg_;
    G4double z_dist_;
    G4double length_;
    G4double pmt_length_;

    PmtR11410 pmt_;
    //PmtR7378A pmt_;
  };

} // end namespace nexus

#endif
