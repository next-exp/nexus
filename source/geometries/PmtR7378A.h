// ----------------------------------------------------------------------------
///  \file   PmtR7378A.h
///  \brief  Geometry model for the Hamamatsu R7378A PMT. 
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     17 Feb 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __PMT_R7378A__
#define __PMT_R7378A__

#include "BaseGeometry.h"

#include <G4ThreeVector.hh>


namespace nexus {

  /// Geometry model for the Hamamatsu R7378A photomultiplier (PMT).
  
  class PmtR7378A: public BaseGeometry
  {
  public:
    /// Constructor
    PmtR7378A();
    /// Destructor
    ~PmtR7378A();

    /// Returns the PMT diameter
    G4double Diameter() const;
    /// Returns the PMT length
    G4double Length() const;

    /// Sets a sensitive detector associated to the

  private:
    void BuildGeometry();

  private:
    G4double _pmt_diam, _pmt_length; ///< PMT dimensions
  };

  inline G4double PmtR7378A::Diameter() const { return _pmt_diam; }
  inline G4double PmtR7378A::Length() const { return _pmt_length; }

} // end namespace nexus

#endif
