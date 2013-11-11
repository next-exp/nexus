// ----------------------------------------------------------------------------
///  \file   XenonGasProperties.h
///  \brief  
///  
///  \author  <justo.martin-albo@ific.uv.es>
///  \date    25 Dec 2010
///  \version $Id$
///
///  Copyright (c) 2010-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __XENON_GAS_PROPERTIES__
#define __XENON_GAS_PROPERTIES__

#include <globals.hh>
#include <vector>

class G4MaterialPropertiesTable;


namespace nexus {

  class XenonGasProperties
  {
  public:
    /// Constructor
    XenonGasProperties(G4double pressure, G4double temperature);
    /// Destructor
    ~XenonGasProperties();

    /// Return the refractive index of xenon gas for a given photon energy
    G4double RefractiveIndex(G4double energy);
    
    G4double Scintillation(G4double energy);
    void Scintillation(G4int entries, G4double* energy, G4double* intensity);

    //    G4MaterialPropertiesTable* Scintillation();

    G4double Density();

    /// Electroluminescence yield of pure xenon gas
    G4double ELLightYield(G4double field_strength) const;
    

  private:
    G4double _pressure, _temperature;
    G4double _density;

    //static const G4double _densities[100];
  
  };

} // end namespace nexus

#endif
