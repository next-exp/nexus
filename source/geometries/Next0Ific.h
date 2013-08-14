// ----------------------------------------------------------------------------
///  \file   Next0Ific.h
///  \brief  Geometry model of the NEXT-0 prototype at IFIC.
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>
///            F Monrabal <francesc.monrabal@ific.uv.es>    
///  \date     27 Jan 2010
///  \version  $Id$ 
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT0_IFIC__
#define __NEXT0_IFIC__

#include "BaseGeometry.h"

class G4LogicalVolume;
class G4SPSRandomGenerator;

namespace nexus {

  /// This class is a Geant4 geometry model of the NEXT-0 prototype at IFIC.

  class Next0Ific: public BaseGeometry
  {
  public:
    /// Constructor
    Next0Ific();
    /// Destructor
    ~Next0Ific();

  private:
    void DefineGeometry();

  private:
    // Dimensions
    G4double _radius;  ///< Chamber internal radius
    G4double _length;  ///< Chamber internal length
    G4double _thickn;  ///< Chamber thickness
    G4double _source_R;  ///< Source R
    G4double _source_Z;  ///< Source thickness

    // Materials
    G4String _chamber_mat;  ///< Material the chamber is made of
    G4String _tracking_mat; ///< Material used as tracker
    G4String _source_mat;   ///< Material used as source

    G4SPSRandomGenerator* _gen;
  };

} // end namespace nexus

#endif
