// ----------------------------------------------------------------------------
// nexus | MaterialsList.h
//
// Definition of materials of common use.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef MATERIALS_LIST_H
#define MATERIALS_LIST_H

#include <globals.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

class G4Material;

namespace materials {

  using namespace CLHEP;


  // Gaseous xenon
  G4Material* GXe(G4double pressure=STP_Pressure,
			            G4double temperature=STP_Temperature);
  G4Material* GXeEnriched(G4double pressure=STP_Pressure,
			                    G4double temperature=STP_Temperature);
  G4Material* GXeDepleted(G4double pressure=STP_Pressure,
				                  G4double temperature=STP_Temperature);

  // Gaseous xenon set by density rather than temperature (maybe temp.)
  G4Material* GXe_bydensity(G4double density,
				                    G4double temperature=STP_Temperature,
				                    G4double pressure=STP_Pressure);
  G4Material* GXeEnriched_bydensity(G4double density,
					                          G4double temperature=STP_Temperature,
					                          G4double pressure=STP_Pressure,
                                    const std::vector<std::pair<int, double>>& isotopicComposition = {});
  G4Material* GXeDepleted_bydensity(G4double density,
					                          G4double temperature=STP_Temperature,
					                          G4double pressure=STP_Pressure,
                                    const std::vector<std::pair<int, double>>& isotopicComposition = {});

  // Liquid xenon
  G4Material* LXe();

  // Argon
  G4Material* GAr(G4double pressure=STP_Pressure,
			            G4double temperature=STP_Temperature);
  // Mixture Xe+Ar
  G4Material* GXeAr(G4double pressure=STP_Pressure,
			              G4double temperature=STP_Temperature, G4double percXe=0.);

  // Mixture Xe+He
  G4Material* GXeHe(G4double pressure=STP_Pressure,
                    G4double temperature=STP_Temperature,
			              G4double percXe=98,
			              G4int mass_num=4);


  // Stainless Steel (grade 304L)
  G4Material* Steel();

  // Stainless steel grade 316Ti
  G4Material* Steel316Ti();

  // Epoxy resin
  G4Material* Epoxy();

  // Kovar (nickel-cobalt ferrous alloy)
  G4Material* Kovar();

  // PEEK (Polyether ether ketone)
  G4Material* PEEK();

  /// Sapphire
  G4Material* Sapphire();

  // Fused silica (synthetic quartz)
  G4Material* FusedSilica();

  // PS (Polystyrene)
  G4Material* PS();

  // TPB (tetraphenyl butadiene)
  G4Material* TPB();

  // ITO (indium tin oxide)
  G4Material* ITO();

  // PTH (p-terphenyl)
  G4Material* TPH();

  // PVT (Polyvinyltoluene)
  G4Material* PVT();

  // KEVLAR (-NH-C6H4-NH-CO-C6H4-CO-)*n
  G4Material* Kevlar();

  /// High density polyethylene
  G4Material* HDPE();

  /// Optical Silicone
  G4Material* OpticalSilicone();

  /// PE1000 (PE UHMW, ultra-high molecular weight polyethylene)
  G4Material* PE1000();

  /// PE500 (PE HD, high density)
  G4Material* PE500();

  /// Selenium Hexafluoride
    G4Material* SeF6(G4double pressure=STP_Pressure,
			               G4double temperature=STP_Temperature);

  // FR4
  G4Material* FR4();

  // Pethylene (cladding)
  G4Material* Pethylene();

  // FPethylene Fluorated Phethylene (cladding)
  G4Material* FPethylene();

  // PolyMethylmethacrylate (cladding)
  G4Material* PMMA();

  // Poly(3,4-ethylenedioxythiophene) a.k.a. PEDOT
  G4Material* PEDOT();

  // Fake dielectric (to be deprecated)
  G4Material* FakeDielectric(G4Material*, G4String);

  // Limestone (calcium carbonate)
  G4Material* Limestone();

  G4Material* CopyMaterial(G4Material*, const G4String&);


} // end namespace materials

#endif
