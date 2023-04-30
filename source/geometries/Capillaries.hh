#ifndef CAPILLARIES_HH
#define CAPILLARIES_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "GeometryBase.h"

namespace nexus {

    class Capillaries : public GeometryBase{

    public:
        Capillaries();
        ~Capillaries();
        void Construct();
        G4ThreeVector specific_vertex_;

        /// Return vertex within region <region> of the chamber
        G4ThreeVector GenerateVertex(const G4String& region) const;
    };

}
#endif
