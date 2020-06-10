// ----------------------------------------------------------------------------
// nexus | XenonGasProperties.h
//
// This class collects the relevant physical properties of gaseous xenon.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef XENON_GAS_PROPERTIES_H
#define XENON_GAS_PROPERTIES_H

#include <globals.hh>
#include <vector>

class G4MaterialPropertiesTable;


namespace nexus {

  class XenonGasProperties
  {
  public:
    /// Constructor
    XenonGasProperties(G4double pressure, G4double temperature);
    XenonGasProperties();
    /// Destructor
    ~XenonGasProperties();

    /// Return the refractive index of xenon gas for a given photon energy
    G4double RefractiveIndex(G4double energy);

    G4double Scintillation(G4double energy);
    void Scintillation(G4int entries, G4double* energy, G4double* intensity);

    void MakeDataTable();
    G4double GetDensity(G4double pressure, G4double temperature);

    static G4double Density(G4double pressure);
    static G4double MassPerMole(G4int a);

    /// Electroluminescence yield of pure xenon gas
    G4double ELLightYield(G4double field_strength) const;


  private:
    G4double pressure_;
    //G4double temperature_;
    G4int npressures_;
    G4int ntemps_;
    std::vector<std::vector<G4double>> data_; // temp, press, density

  };

} // end namespace nexus

#endif
