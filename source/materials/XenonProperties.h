// ----------------------------------------------------------------------------
// nexus | XenonProperties.h
//
// Functions to calculate relevant relevant physical properties of xenon.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef XENON_PROPERTIES_H
#define XENON_PROPERTIES_H

#include <globals.hh>
#include <vector>

class G4MaterialPropertiesTable;

G4double GXeDensity(G4double pressure);
G4double LXeDensity();
G4double XenonMassPerMole(G4int a);

/// Return the refractive index of xenon gas for a given photon energy
G4double XenonRefractiveIndex(G4double energy, G4double density);

G4double GXeScintillation(G4double energy, G4double pressure);
G4double LXeScintillation(G4double energy);

std::pair<G4int, G4int> MakeXeDensityDataTable(std::vector<std::vector<G4double>> &data);
G4double GetGasDensity(G4double pressure, G4double temperature);

/// Electroluminescence yield of pure xenon gas
G4double XenonELLightYield(G4double field_strength, G4double pressure);

#endif
