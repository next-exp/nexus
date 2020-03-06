// ----------------------------------------------------------------------------
///  \file   WavelengthShifting.h
///  \brief  Physics process describing the wavelength shifting.
///
///  \author   P. Ferrario <paola.ferrario@ific.uv.es>    
///  \date     13 Sept 2013
///  \version  $Id $
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------
#ifndef WLS_H
#define WLS_H

#include <G4VDiscreteProcess.hh>

class G4ParticleChange;
class G4VWLSTimeGeneratorProfile;

//class TH1F;
//class TFile;


namespace nexus {

  /// This is a class with a nexus-defined process for wavelength shifting. It differs from
  /// G4OpWls, because the mean free path is not calculated from a wls_absorption length property
  /// but through a probability property, thus independent from the material thickness.
  class WavelengthShifting: public G4VDiscreteProcess
  {
  public:
    // Constructors
    WavelengthShifting(const G4String& name="WavelengthShifting", G4ProcessType type=fUserDefined);
    //Destructor
    ~WavelengthShifting();

    G4bool IsApplicable(const G4ParticleDefinition& aParticleType);
    G4VParticleChange* PostStepDoIt(const G4Track& aTrack, const G4Step& aStep);
    G4double GetMeanFreePath(const G4Track& track, G4double, G4ForceCondition*);

  private:
    void BuildThePhysicsTable();
    void ComputeCumulativeDistribution(const G4MaterialPropertyVector& pdf, G4PhysicsOrderedFreeVector& cdf);

  private:
    G4ParticleChange* _ParticleChange;
    G4PhysicsTable* _wlsIntegralTable;
    G4VWLSTimeGeneratorProfile*  _WLSTimeGeneratorProfile;

    //TH1F* hWLSTime;
    //TFile* histo_file;

  };

}

#endif
