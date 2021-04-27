// ----------------------------------------------------------------------------
///  \file   XenonLiquidProperties.h
///  \brief
///
///  \author  <paola.ferrario@ific.uv.es>
///  \date    1 Jun 2015
///  \version $Id$
///
///  Copyright (c) 2010-2015 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __XENON_LIQUID_PROPERTIES__
#define __XENON_LIQUID_PROPERTIES__

#include <globals.hh>
#include <vector>

class G4MaterialPropertiesTable;


namespace nexus {

  class XenonLiquidProperties
  {
  public:
    /// Constructor
    XenonLiquidProperties();
    /// Destructor
    ~XenonLiquidProperties();

    G4double Density();

    /// Return the refractive index of xenon gas for a given photon energy
    G4double RefractiveIndex(G4double energy);

    G4double Scintillation(G4double energy);
    void Scintillation(G4int entries, G4double* energy, G4double* intensity);


  private:
    G4double _density;

    //static const G4double _densities[100];

  };

} // end namespace nexus

#endif
