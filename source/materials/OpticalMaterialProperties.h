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


namespace nexus {

  using namespace CLHEP;

  /// This is a stateless class where all methods are static functions.

  class OpticalMaterialProperties
  {
  public:

    static G4MaterialPropertiesTable* Vacuum();
    static G4MaterialPropertiesTable* Epoxy();
    static G4MaterialPropertiesTable* GlassEpoxy();
    static G4MaterialPropertiesTable* EpoxyFixedRefr(G4double n);
    static G4MaterialPropertiesTable* EpoxyLXeRefr();

    /// Synthetic fused silica (suprasil) with transmission in the deep
    /// ultraviolet down to 160 nm
    static G4MaterialPropertiesTable* FusedSilica();

    static G4MaterialPropertiesTable* FakeFusedSilica(G4double transparency=.9,
						      G4double thickness=1.*mm);

    static G4MaterialPropertiesTable* FakeGenericMaterial(G4double transparency=.89,
    G4double thickness=0.6*mm);
    
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

   static G4MaterialPropertiesTable* LXe();
   static G4MaterialPropertiesTable* FakeLXe();
   static G4MaterialPropertiesTable* LXe_nconst();
   static G4MaterialPropertiesTable* LXe_window();


    static G4MaterialPropertiesTable* Glass();

    static G4MaterialPropertiesTable* TPB(G4double pressure=10.*bar,
					  G4double temperature=STP_Temperature);
    static G4MaterialPropertiesTable* TPB_LXe(G4double decay=2.2*nanosecond);
    static G4MaterialPropertiesTable* TPB_LXe_nconst(G4double decay=2.2*nanosecond);

    static G4MaterialPropertiesTable* TPBOld();
    static G4MaterialPropertiesTable* TPBmatr();
    static G4MaterialPropertiesTable* WLS2();
    static G4MaterialPropertiesTable* PTFE();
    static G4MaterialPropertiesTable* PTFE_LXe(G4double reflectivity=0.95);
    static G4MaterialPropertiesTable* PTFE_with_TPB();
    static G4MaterialPropertiesTable* PTFE_non_reflectant();
    static G4MaterialPropertiesTable* LYSO();
    static G4MaterialPropertiesTable* LYSO_nconst();
    static G4MaterialPropertiesTable* FakeLYSO();
    static G4MaterialPropertiesTable* ReflectantSurface(G4double reflectivity=0.95);


  private:
    /// Constructor (hidden)
    OpticalMaterialProperties();
    /// Destructor (hidden)
    ~OpticalMaterialProperties();
  };

} // end namespace nexus

#endif
