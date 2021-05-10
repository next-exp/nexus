// ----------------------------------------------------------------------------
// nexus | XenonProperties.h
//
// This class collects the relevant physical properties of xenon.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef XENON_PROPERTIES_H
#define XENON_PROPERTIES_H

#include <globals.hh>
#include <vector>

class G4MaterialPropertiesTable;


namespace nexus {

  class XenonProperties
  {
  public:
    /// Constructor
    XenonProperties(G4double pressure, G4double temperature);
    XenonProperties();
    /// Destructor
    ~XenonProperties();

    /// Return the refractive index of xenon gas for a given photon energy
    G4double RefractiveIndex(G4double energy, G4double density);

    G4double GasScintillation(G4double energy);
    G4double LiquidScintillation(G4double energy);
    void Scintillation(G4int entries, G4double* energy, G4double* intensity, G4bool gas=true);
    void Scintillation(std::vector<G4double>& energy, std::vector<G4double>& intensity, G4bool gas=true);

    void MakeDataTable();
    G4double GetGasDensity(G4double pressure, G4double temperature);

    static G4double GasDensity(G4double pressure);
    static G4double LiquidDensity();
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
