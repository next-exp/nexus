// ---------------------------------------------------------------------------------------
///  A cylindric tracking chamber made of plastic and surrounded by Liquid scintillator
///
///  \author   Javier Muñoz-Vidal <jmunoz@ific.uv.es>
///  \date     15 Sep 2010
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __SX_CHAMBER__
#define __SX_CHAMBER__

#include "BaseGeometry.h"

class G4Material;


namespace nexus {

  class CylinderVertexGenerator;

  /// Geometry of a cylindric plastic made chamber filled with a tracking fluid
  /// and surrounded by liquid scintillator
  
  class SXChamber: public BaseGeometry
  {
  public:
    /// Constructor
    SXChamber();
    /// Destructor
    ~SXChamber();

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region) const;
    
  private:
    void SetParameters();
    void BuildGeometry();

  private:
    // Dimensions
    G4double _chamber_radius;  ///< Chamber internal radius
    G4double _chamber_length;  ///< Chamber internal length
    G4double _chamber_thickn;  ///< Chamber thickness
    G4double _scint_thickn;    ///< Scintillator thickness

    // Materials
    G4Material* _tracking_mat; ///< Material used as tracker
    G4Material* _chamber_mat;  ///< Material the chamber is made of
    G4Material* _scint_mat; ///< Material as liquid scintillator

    /// Vertices generator
    CylinderVertexGenerator* _chamber_vertex_gen;
  };

} // end namespace nexus

#endif
