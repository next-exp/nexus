// ----------------------------------------------------------------------------
///  \file   Electroluminescence.h
///  \brief  
///  
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     3 August 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __ELECTROLUMINESCENCE__
#define __ELECTROLUMINESCENCE__

#include <G4VRestDiscreteProcess.hh>
#include <G4EmSaturation.hh>
#include <G4PhysicsOrderedFreeVector.hh>


namespace nexus {

  class IonizationDrift;
  class AlongStepRandomGenerator;

  
  /// FIXME.
  /// Class description.


  class Electroluminescence: public G4VRestDiscreteProcess
  {
  public:
    /// constructor
    Electroluminescence(const G4String& process_name="Electroluminescence",
			G4ProcessType type=fUserDefined);
    /// destructor
    ~Electroluminescence();
    
    void Initialize();

  public: 

    /// Returns true for any charged particle type 
    G4bool IsApplicable(const G4ParticleDefinition& aParticleType);


    /// Returns infinity; i. e. the process does not limit the step,
    /// but sets the 'StronglyForced' condition for the DoIt to be 
    /// invoked at every step.
    G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*);

    /// Returns infinity; i. e. the process does not limit the time,
    /// but sets the 'StronglyForced' condition for the DoIt to be
    /// invoked at every step.
    G4double GetMeanLifeTime(const G4Track&, G4ForceCondition*);

    // These are the methods implementing the scintillation process.
    // G4Scintillation Process has both PostStepDoIt (for energy 
    // deposition of particles in flight) and AtRestDoIt (for energy
    // given to the medium by particles at rest)
    G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);
    G4VParticleChange* AtRestDoIt(const G4Track&, const G4Step&);

    /// If true, the primary particle tracking is interrupted and any
    /// produced scintillation photons are tracked next. When all 
    /// have been tracked, the tracking of the primary resumes.
    void SetTrackSecondariesFirst(const G4bool);
    /// Returns the boolean flag for tracking secondaries first.
    G4bool GetTrackSecondariesFirst() const;


    G4int LightYield(G4double, G4double);

  private:
    AlongStepRandomGenerator* _rnd;
    IonizationDrift* _drift;
    G4bool _secondaries_first;
    
  };

  
  // inline methods ..................................................

  inline 
  G4bool Electroluminescence::IsApplicable(const G4ParticleDefinition& ptype)
  { return ((ptype.GetPDGCharge() != 0) ? true : false); }

  inline 
  void Electroluminescence::SetTrackSecondariesFirst(const G4bool state) 
  { _secondaries_first = state; }

  inline
  G4bool Electroluminescence::GetTrackSecondariesFirst() const
  { return _secondaries_first; }




//   inline
//   void Electroluminescence::SetScintillationYieldFactor(const G4double yieldfactor)
//   {
//     YieldFactor = yieldfactor;
//   }

//   inline
//   G4double Electroluminescence::GetScintillationYieldFactor() const
//   {
//     return YieldFactor;
//   }

//   inline
//   void Electroluminescence::SetScintillationExcitationRatio(const G4double excitationratio)
//   {
//     ExcitationRatio = excitationratio;
//   }

//   inline
//   G4double Electroluminescence::GetScintillationExcitationRatio() const
//   {
//     return ExcitationRatio;
//   }

//   inline
//   G4PhysicsTable* Electroluminescence::GetSlowIntegralTable() const
//   {
//     return theSlowIntegralTable;
//   }

//   inline
//   G4PhysicsTable* Electroluminescence::GetFastIntegralTable() const
//   {
//     return theFastIntegralTable;
//   }

//   inline
//   void Electroluminescence::DumpPhysicsTable() const
//   {
//     if (theFastIntegralTable) {
//       G4int PhysicsTableSize = theFastIntegralTable->entries();
//       G4PhysicsOrderedFreeVector *v;

//       for (G4int i = 0 ; i < PhysicsTableSize ; i++ )
// 	{
// 	  v = (G4PhysicsOrderedFreeVector*)(*theFastIntegralTable)[i];
// 	  v->DumpValues();
// 	}
//     }

//     if (theSlowIntegralTable) {
//       G4int PhysicsTableSize = theSlowIntegralTable->entries();
//       G4PhysicsOrderedFreeVector *v;

//       for (G4int i = 0 ; i < PhysicsTableSize ; i++ )
// 	{
// 	  v = (G4PhysicsOrderedFreeVector*)(*theSlowIntegralTable)[i];
// 	  v->DumpValues();
// 	}
//     }
//   }

    
} // end namespace nexus

#endif
