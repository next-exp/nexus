// ----------------------------------------------------------------------------
///  \file   OpticalMaterialProperties.h
///  \brief  Optical properties of common materials.
///  
///  \author J. Martin-Albo <jmalbos@ific.uv.es>
///          F. Monrabal <franmon4@ific.uv.es>
///          L. Serra <sediaz@ific.uv.es>
///             
///  \date     27 Mar 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __OPTICAL_MATERIAL_PROPERTIES__
#define __OPTICAL_MATERIAL_PROPERTIES__

#include <G4PhysicalConstants.hh>
#include <globals.hh>

class G4MaterialPropertiesTable;


namespace nexus {

  /// Optical properties of several materials. (Notice this is a 
  /// stateless class.)

  class OpticalMaterialProperties
  {
  public:
    
    /// Optical properties of gaseous xenon
    static G4MaterialPropertiesTable* GXe(G4double pressure=STP_Pressure);
    
    /// Optical properties of fused silica (non-crystalline quartz)
    static G4MaterialPropertiesTable* FusedSilica();

  
  private:



    /// Constructor (hidden)
    OpticalMaterialProperties();
    /// Destructor (hidden)
    ~OpticalMaterialProperties();
  };

} // end namespace nexus

#endif
