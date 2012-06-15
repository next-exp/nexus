// ----------------------------------------------------------------------------
///  \file   PmtR8520.h
///  \brief  Hamamatsu R8520 radiopure, 1-inch, square PMT.
///
///  \author   <justo.martin-albo@ific.uv.es>
///            <edgar.gomez@uan.edu.co>
///  \date     17 Aug 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __PMT_R8520__
#define __PMT_R8520__

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>


namespace nexus {

  /// Geometry of the PMT Hamamatsu R8520. This is a 1-inch, square
  /// PMT optimized for the VUV region.
  
  class PmtR8520: public BaseGeometry
  {
  public:
    /// Constructor
    PmtR8520();
    /// Destructor
    ~PmtR8520();

    G4ThreeVector GetDimensions() const;
   G4LogicalVolume* GetWindowLogic();
    
  private:
    void BuildGeometry();   

  private:
    G4ThreeVector _dimensions; ///< external dimensions of the PMT
    G4LogicalVolume* _window_logic;
  };

} // end namespace nexus

#endif
