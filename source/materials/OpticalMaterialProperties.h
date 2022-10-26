// ----------------------------------------------------------------------------
// nexus | OpticalMaterialProperties.h
//
// Optical properties of relevant materials.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef OPTICAL_MATERIAL_PROPERTIES_H
#define OPTICAL_MATERIAL_PROPERTIES_H

#include <globals.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

class G4MaterialPropertiesTable;


namespace opticalprops {

  using namespace CLHEP;


  G4MaterialPropertiesTable* Vacuum();

  G4MaterialPropertiesTable* GlassEpoxy();

  G4MaterialPropertiesTable* FusedSilica();

  G4MaterialPropertiesTable* FakeFusedSilica(G4double transparency = .9,
                                            G4double thickness    = 1. * mm);

  G4MaterialPropertiesTable* ITO();

  G4MaterialPropertiesTable* PEDOT();

  G4MaterialPropertiesTable* Sapphire();

  G4MaterialPropertiesTable* OptCoupler();

  G4MaterialPropertiesTable* GAr(G4double sc_yield,
                                G4double e_lifetime=1000.*ms);

  G4MaterialPropertiesTable* GXe(G4double pressure=1.*bar,
                                G4double temperature=STP_Temperature,
                                G4int sc_yield=25510/MeV,
                                G4double e_lifetime=1000.*ms);

  G4MaterialPropertiesTable* LXe();

  G4MaterialPropertiesTable* FakeGrid(G4double pressure=1.*bar,
                                      G4double temperature=STP_Temperature,
                                      G4double transparency=.9,
                                      G4double thickness=1.*mm,
                                      G4int sc_yield=25510/MeV,
                                      G4double e_lifetime=1000.*ms,
                                      G4double photoe_p=0);

  G4MaterialPropertiesTable* TPB();

  G4MaterialPropertiesTable* PTP();

  G4MaterialPropertiesTable* DegradedTPB(G4double wls_eff);

  G4MaterialPropertiesTable* TPH();

  G4MaterialPropertiesTable* PTFE();

  G4MaterialPropertiesTable* EJ280();

  G4MaterialPropertiesTable* EJ286();

  G4MaterialPropertiesTable* Y11();

  G4MaterialPropertiesTable* Pethylene();

  G4MaterialPropertiesTable* FPethylene();

  G4MaterialPropertiesTable* PMMA();

  G4MaterialPropertiesTable* XXX();


  constexpr G4double optPhotMinE_ =  0.2  * eV;
  constexpr G4double optPhotMaxE_ = 11.5  * eV;
  constexpr G4double noAbsLength_ = 1.e8  * m;

  // Constant that allows to convert nm to eV:
  // nm_to_eV_ / wavelength (nm) = energy (eV)
  constexpr G4double nm_to_eV_ = h_Planck * c_light * 1.e6;


} // end namespace opticalprops

#endif
