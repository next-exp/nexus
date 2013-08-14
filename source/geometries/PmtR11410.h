// ----------------------------------------------------------------------------
///  \file   Pmt11410.h
///  \brief  Geometry model for the Hamamatsu R11410 PMT. 
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     9 Dec 2011
///  \version  $Id$
///
///  Copyright (c) 2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __PMT_R11410__
#define __PMT_R11410__

#include "BaseGeometry.h"
#include "CylinderPointSampler.h"

#include <G4ThreeVector.hh>
#include <G4OpticalSurface.hh>


namespace nexus {

  /// Geometry model for the Hamamatsu R11410-10 photomultiplier
  
  class PmtR11410: public BaseGeometry
  {
  public:

    /// Constructor
    PmtR11410();

    /// Destructor
    ~PmtR11410();

    G4ThreeVector GetRelPosition();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;


  private:
    G4OpticalSurface* GetPhotOptSurf();

  private:
    // Dimensions
    G4double _front_body_diam, _front_body_length;
    G4double _rear_body_diam, _rear_body_length;
    G4double _body_thickness;
    G4double _window_diam, _window_thickness;
    G4double _photocathode_diam, _photocathode_thickness;
    
    // Visibility of the PMT
    G4int _visibility;

    // Vertex generators
    CylinderPointSampler* _front_body_gen;
    CylinderPointSampler* _medium_body_gen;
    CylinderPointSampler* _rear_body_gen;
    CylinderPointSampler* _rear_cap_gen;

    G4double _front_body_perc, _fr_med_body_perc, _fr_med_re_body_perc;

  };


} // end namespace nexus

#endif
