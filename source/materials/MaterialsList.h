// ----------------------------------------------------------------------------
// nexus | MaterialsList.h
//
// Definition of materials of common use.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef MATERIALS_LIST
#define MATERIALS_LIST

#include <globals.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

class G4Material;

namespace nexus {

  using namespace CLHEP;

  /// (This is a stateless class. All methods must be defined as static.)

  class MaterialsList
  {
  public:

    // Gaseous xenon
    static G4Material* GXe(G4double pressure=STP_Pressure,
			   G4double temperature=STP_Temperature);
    static G4Material* GXeEnriched(G4double pressure=STP_Pressure,
			   G4double temperature=STP_Temperature);
    static G4Material* GXeDepleted(G4double pressure=STP_Pressure,
				   G4double temperature=STP_Temperature);

    // Gaseous xenon set by density rather than temperature (maybe temp.)
    static G4Material* GXe_bydensity(G4double density,
				     G4double temperature=STP_Temperature,
				     G4double pressure=STP_Pressure);
    static G4Material* GXeEnriched_bydensity(G4double density,
					     G4double temperature=STP_Temperature,
					     G4double pressure=STP_Pressure);
    static G4Material* GXeDepleted_bydensity(G4double density,
					     G4double temperature=STP_Temperature,
					     G4double pressure=STP_Pressure);

    // Argon
    static G4Material* GAr(G4double pressure=STP_Pressure,
			   G4double temperature=STP_Temperature);
    // Mixture Xe+Ar
    static G4Material* GXeAr(G4double pressure=STP_Pressure,
			     G4double temperature=STP_Temperature, G4double percXe=0.);

    // Mixture Xe+He
    static G4Material* GXeHe(G4double pressure=STP_Pressure,
                             G4double temperature=STP_Temperature,
			     G4double percXe=98,
			     G4int mass_num=4);


    // Stainless Steel (grade 304L)
    static G4Material* Steel();

    // Stainless steel grade 316Ti
    static G4Material* Steel316Ti();

    // Epoxy resin
    static G4Material* Epoxy();

    // Kovar (nickel-cobalt ferrous alloy)
    static G4Material* Kovar();

    // PEEK (Polyether ether ketone)
    static G4Material* PEEK();

    /// Sapphire
    static G4Material* Sapphire();

    // Fused silica (synthetic quartz)
    static G4Material* FusedSilica();

    // PS (Polystyrene)
    static G4Material* PS();

    // TPB (tetraphenyl butadiene)
    static G4Material* TPB();

    // ITO (indium tin oxide)
    static G4Material* ITO();

    // PTH (p-terphenyl)
    static G4Material* TPH();

    // PVT (Polyvinyltoluene)
    static G4Material* PVT();

    // KEVLAR (-NH-C6H4-NH-CO-C6H4-CO-)*n
    static G4Material* Kevlar();

    /// High density polyethylene
    static G4Material* HDPE();

    /// Optical Silicone
    static G4Material* OpticalSilicone();

    /// Selenium Hexafluoride
    static G4Material* SeF6(G4double pressure=STP_Pressure,
			   G4double temperature=STP_Temperature);

    // FR4
    static G4Material* FR4();

    // WLS EJ-280
    static G4Material* EJ280();

    // Kuraray Fiber Y11
    static G4Material* Y11();

    // Pethylene (cladding)
    static G4Material* Pethylene();

    // FPethylene Fluorated Phethylene (cladding)
    static G4Material* FPethylene();

    // PolyMethylmethacrylate (cladding)
    static G4Material* PMMA();

    // Poly(3,4-ethylenedioxythiophene) a.k.a. PEDOT
    static G4Material* PEDOT();

    // Fake dielectric (to be deprecated)
    static G4Material* FakeDielectric(G4Material*, G4String);

    static G4Material* CopyMaterial(G4Material*, const G4String&);

  private:
    /// Constructor (hidden)
    MaterialsList();
    /// Destructor (hidden)
    ~MaterialsList();
  };

} // end namespace nexus

#endif
