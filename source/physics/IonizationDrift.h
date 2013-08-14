// ----------------------------------------------------------------------------
///  \file   IonizationDrift.h
///  \brief  Drift of ionization clusters.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     1 June 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __IONIZATION_DRIFT__
#define __IONIZATION_DRIFT__

#include <G4VContinuousDiscreteProcess.hh>
#include <G4ParticleChangeForTransport.hh>

class G4Navigator;


namespace nexus {

  /// Physics process describing the drift of an ionization cluster
  /// or an ionization electron under the influence of a field.

  class IonizationDrift: public G4VContinuousDiscreteProcess
  {
  public:
    
    /// Constructor
    IonizationDrift(const G4String& name="Drift",
		    G4ProcessType type=fUserDefined);
    /// Destructor
    ~IonizationDrift();

    /// The process is applicable only to ionization clusters
    /// or ionization electrons
    G4bool IsApplicable(const G4ParticleDefinition&);

    ///
    void ShootChargesAfterDrift(G4bool);
    
    G4bool GetShootChargesAfterDrift() const;

  public:

    G4VParticleChange* AlongStepDoIt(const G4Track&, const G4Step&);
    
    G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);
        
  private:
    
    /// Returns infinity; i.e., the process does not limit the step,
    /// but sets the 'StronglyForced' condition so that the PostStepDoIt 
    /// is invoked at every step.
    G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*);

    /// Returns zero if no electric field is defined for the region.
    G4double GetContinuousStepLimit(const G4Track&, G4double, 
				    G4double, G4double&);
    
  private:

    G4bool _shoot_charges;
    G4LorentzVector _xyzt;
    G4ParticleChangeForTransport _ParticleChange;
    G4Navigator* _nav; ///< Pointer to the G4 navigator for tracking
  };

  // inline methods ..................................................
  
  inline void IonizationDrift::ShootChargesAfterDrift(G4bool sc)
  { _shoot_charges = sc; }

  inline G4bool IonizationDrift::GetShootChargesAfterDrift() const
  { return _shoot_charges; }

} // end namespace nexus

#endif
