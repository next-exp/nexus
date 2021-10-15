// ----------------------------------------------------------------------------
// nexus | ELParamSimulation.h
//
// This class (WIP) implements a parametrized simulation .
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef EL_PARAM_SIMULATION_H
#define EL_PARAM_SIMULATION_H

#include <G4VFastSimulationModel.hh>
#include "SensorHit.h"
#include <vector>


namespace nexus {

  class ELLookupTable;

  class ELParamSimulation: public G4VFastSimulationModel
  {
  public:
    /// Constructor
    ELParamSimulation(G4Region* region);
    /// Destructor
    ~ELParamSimulation();

    // This model is only valid for ionization electrons
    G4bool IsApplicable(const G4ParticleDefinition&);

    // You have to return "true" when the dynamics conditions to trigger your
    // parameterisation are fulfilled. The G4FastTrack provides you access to
    // the current G4Track, gives simple access to envelope related features
    // (G4LogicalVolume, G4VSolid, G4AffineTransform references between the
    // global and the envelope local coordinates systems) and simple access to
    // the position, momentum expressed in the envelope coordinate system.
    // Using those quantities and the G4VSolid methods, you can for example
    // easily check how far you are from the envelope boundary.
    G4bool ModelTrigger(const G4FastTrack &);

    // Your parameterisation properly said. The G4FastTrack reference provides
    // input informations. The final state of the particles after parameterisation
    // has to be returned through the G4FastStep reference. This final state is
    // described has "requests" the tracking will apply after your
    // parameterisation has been invoked.
    void DoIt(const G4FastTrack&, G4FastStep&);

    //To load EL table
    // void ReadELTable(const char* filename,
    // 		     std::map<int, std::map<int, std::vector<double> > >& eltable);


  private:
    //BaseELGrid* elgrid_;
    // std::map<G4int, std::map<G4int, std::vector<G4double> > >  eltable_;

    // bool timebinning_;
    // int num_tbins_;

    ELLookupTable* table_;

    std::vector<SensorHitsCollection*> HCE_;

  };

} // end namespace nexus

#endif
