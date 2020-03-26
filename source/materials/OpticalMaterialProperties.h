// -----------------------------------------------------------------------------
//  nexus | OpticalMaterialProperties.h
//
//  * Info:         : NEXT-Flex Field Cage geometry for performance studies.
//  * Author        : <jmunoz@ific.uv.es>
//  *                 <paola.ferrario@ific.uv.es>
//  *                 <justo.martin-albo@ific.uv.es>
//  * Creation date : February 2020
//  * Description   : Optical properties of most used materials.
// -----------------------------------------------------------------------------

#ifndef __OPTICAL_MATERIAL_PROPERTIES__
#define __OPTICAL_MATERIAL_PROPERTIES__

#include <globals.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

class G4MaterialPropertiesTable;


namespace nexus {

  using namespace CLHEP;

  // Optical properties of several materials.
  // (N.B. This is a stateless class where all methods are static functions.)

  class OpticalMaterialProperties
  {
  public:

    static G4MaterialPropertiesTable* Vacuum();

    static G4MaterialPropertiesTable* GlassEpoxy();

    // Synthetic fused silica (suprasil) with transmission in the deep
    // ultraviolet down to 160 nm
    static G4MaterialPropertiesTable* FusedSilica();

    static G4MaterialPropertiesTable* FakeFusedSilica(G4double transparency = .9,
                                                      G4double thickness    = 1. * mm);

    static G4MaterialPropertiesTable* Sapphire();

    static G4MaterialPropertiesTable* OptCoupler();

    static G4MaterialPropertiesTable* GAr(G4double sc_yield,
                                          G4double e_lifetime=1000.*ms);

    static G4MaterialPropertiesTable* LAr();

    static G4MaterialPropertiesTable* GXe(G4double pressure=1.*bar,
                                          G4double temperature=STP_Temperature,
                                          G4int sc_yield=25510/MeV,
                                          G4double e_lifetime=1000.*ms);

    static G4MaterialPropertiesTable* FakeGrid(G4double pressure=1.*bar,
                                               G4double temperature=STP_Temperature,
                                               G4double transparency=.9,
                                               G4double thickness=1.*mm,
                                               G4int sc_yield=25510/MeV,
                                               G4double e_lifetime=1000.*ms);

    static G4MaterialPropertiesTable* TPB();

    static G4MaterialPropertiesTable* TPH();

    static G4MaterialPropertiesTable* PTFE();

    static G4MaterialPropertiesTable* EJ280();

    static G4MaterialPropertiesTable* EJ286();

    static G4MaterialPropertiesTable* Y11();

    static G4MaterialPropertiesTable* Pethylene();

    static G4MaterialPropertiesTable* FPethylene();

    static G4MaterialPropertiesTable* PMMA();

    static G4MaterialPropertiesTable* XXX();


  private:

    static constexpr G4double optPhotMinE_ =  0.2  * eV;
    static constexpr G4double optPhotMaxE_ = 11.5  * eV;
    static constexpr G4double noAbsLength_ = 1.e8  * m;

    // Constant that allows to convert nm to eV:
    // nm_to_eV_ / wavelength (nm) = energy (eV)
    static constexpr G4double nm_to_eV_ = h_Planck * c_light * 1.e6;


  private:
    // Constructor (hidden)
    OpticalMaterialProperties();

    // Destructor (hidden)
    ~OpticalMaterialProperties();
  };

} // end namespace nexus

#endif
