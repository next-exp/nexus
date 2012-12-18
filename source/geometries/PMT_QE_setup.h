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

#ifndef __PMT_QE_SETUP__
#define __PMT_QE_SETUP__

#include "BaseGeometry.h"
#include "PmtR7378A.h"


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
    /// Read user configuration parameters
    void SetParameters();
    /// Define the volumes of the geometry
    void BuildGeometry();

  private:   
    G4double _gxe_pressure; ///< Xenon gas pressure
    G4double _pmt_diameter;
    G4double _z_dist;

    G4double _length;

    PmtR7378A _pmt;
  };

} // end namespace nexus

#endif
