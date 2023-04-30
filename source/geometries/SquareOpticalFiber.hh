#ifndef SQUARE_OPTICAL_FIBER_HH
#define QUARE_OPTICAL_FIBER_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "GeometryBase.h"
#include <random>
#include "Randomize.hh"
#include "G4MultiUnion.hh"
#include "G4GenericMessenger.hh"


#include "G4VSensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"


namespace nexus {

    class SquareOpticalFiber : public GeometryBase{

    mutable std::mt19937 gen_; // move the generator to the class scope and make it mutable
    mutable std::uniform_real_distribution<double> z_dist_; // make the distribution mutable as well

    public:
        SquareOpticalFiber();
        ~SquareOpticalFiber();
        // void UpdateELGapLimits();
        std::pair<G4double, G4double> UpdateELGapLimits();
        void Construct();
        G4GenericMessenger *msg_;
        /// Return vertex within region <region> of the chamber
        G4ThreeVector GenerateVertex(const G4String& region) const;

        G4double holderThickness_;
        G4double TPBThickness_;

        // Controlled from macro
        G4ThreeVector specific_vertex_;
        G4double ELGap_;
        G4double pitch_;
        G4double distanceFiberHolder_;
        G4double distanceAnodeHolder_;



    };




    // class SiPMSD : public G4VSensitiveDetector {
        
    //     public:
    //     SiPMSD(G4String name);
    //     virtual ~SiPMSD();

    //     virtual void Initialize(G4HCofThisEvent* hitCollection);
    //     virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    //     virtual void EndOfEvent(G4HCofThisEvent* hitCollection);
    // };
}
#endif
