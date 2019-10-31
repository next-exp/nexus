// ----------------------------------------------------------------------------
///  \file   OpticalMaterialProperties.h
///  \brief  Optical properties of most used materials.
///  
///  \author  <justo.martin-albo@ific.uv.es>
///           <francesc.monrabal4@ific.uv.es>
///           <paola.ferrario@ific.uv.es>
///  \date    27 Mar 2009
///  \version $Id$
///
///  Copyright (c) 2009, 2010, 2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __OPTICAL_MATERIAL_PROPERTIES__
#define __OPTICAL_MATERIAL_PROPERTIES__

#include <globals.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

class G4MaterialPropertiesTable;


namespace nexus {

  using namespace CLHEP;

  /// Optical properties of several materials. 
  /// (N.B. This is a stateless class where all methods are static functions.)

  class OpticalMaterialProperties
  {
  public:

    static G4MaterialPropertiesTable* Vacuum();
    static G4MaterialPropertiesTable* Epoxy();
    static G4MaterialPropertiesTable* GlassEpoxy();

    /// Synthetic fused silica (suprasil) with transmission in the deep
    /// ultraviolet down to 160 nm
    static G4MaterialPropertiesTable* FusedSilica();

    static G4MaterialPropertiesTable* FakeFusedSilica(G4double transparency=.9, 
						      G4double thickness=1.*mm);
    
    static G4MaterialPropertiesTable* Sapphire();

    static G4MaterialPropertiesTable* OptCoupler();

    static G4MaterialPropertiesTable* GAr(G4double sc_yield);
    static G4MaterialPropertiesTable* LAr();

    static G4MaterialPropertiesTable* GXe(G4double pressure=1.*bar, 
					  G4double temperature=STP_Temperature,
					  G4int sc_yield=25510/MeV);

    static G4MaterialPropertiesTable* FakeGrid(G4double pressure=1.*bar, 
					       G4double temperature=STP_Temperature, 
					       G4double transparency=.9, 
					       G4double thickness=1.*mm,
					       G4int sc_yield=25510/MeV);

    static G4MaterialPropertiesTable* Glass();
    
    static G4MaterialPropertiesTable* TPB(G4double pressure=10.*bar, 
					  G4double temperature=STP_Temperature);
    static G4MaterialPropertiesTable* TPBOld();
    static G4MaterialPropertiesTable* TPBmatr();
    static G4MaterialPropertiesTable* WLS2();
    static G4MaterialPropertiesTable* TPH();
    static G4MaterialPropertiesTable* PS();
    static G4MaterialPropertiesTable* BC480(); 
    static G4MaterialPropertiesTable* PTFE();
    static G4MaterialPropertiesTable* PTFE_with_TPB();
    static G4MaterialPropertiesTable* PMMA();

  
  private:
    /// Constructor (hidden)
    OpticalMaterialProperties();
    /// Destructor (hidden)
    ~OpticalMaterialProperties();
  };

} // end namespace nexus

#endif
