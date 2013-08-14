// ----------------------------------------------------------------------------
///  \file   IonizationDrift.h
///  \brief  Drift of ionization electrons under a homogeneus electric field.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     1 June 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __IONIZATION_DRIFT__
#define __IONIZATION_DRIFT__

#include <G4LorentzVector.hh>

class G4Step;


namespace nexus {

  class AlongStepRandomGenerator;


  /// Drift of ionization electrons in a fluid.
  /// 
  
  class IonizationDrift
  {
  public:
    /// Default constructor. Sets all parameters to zero.
    IonizationDrift();
    
    /// Constructor
    IonizationDrift(G4double drift_velocity,
		    G4double transverse_diffusion,
		    G4double longitudinal_diffusion,
		    G4double attachment,
		    G4double ionization_energy,
		    G4double max_drift_length);
    
    /// Destructor
    ~IonizationDrift();

    /// set a step for processing
    void SetStep(const G4Step&);

    /// 
    G4LorentzVector GenerateOne();

    G4long NumberOfCarriers() const;
    
  public:
    
    // Setters & getters

    void SetDriftVelocity(G4double);
    G4double GetDriftVelocity() const;
    
    void SetTransverseDiffusion(G4double);
    G4double GetTransverseDiffusion() const;

    void SetLongitudinalDiffusion(G4double);
    G4double GetLongitudinalDiffusion() const;
    
    void SetAttachment(G4double);
    G4double GetAttachment() const;



    
    
  private:
    
    G4double _drift_velocity;  ///< Electron drift velocity
    G4double _transv_diff;     ///< Transverse diffusion
    G4double _longit_diff;     ///< Longitudinal diffusion
    G4double _attachment;      ///< Attachment
    
    G4double _ioniz_energy; ///< Energy for ionization pair production
    G4double _fano_factor;  ///< Fano factor

    G4long _num_electrons;

    G4double _max_drift_length;
    
    G4double _drift_length;
    G4double _drift_time;

    AlongStepRandomGenerator* _rnd;
  };


  // inline methods ........................................
  
  inline void IonizationDrift::SetDriftVelocity(G4double v) 
  { _drift_velocity = v; }
  
  inline G4double IonizationDrift::GetDriftVelocity() const
  { return _drift_velocity; }

  inline void IonizationDrift::SetTransverseDiffusion(G4double td)
  { _transv_diff = td; }

  inline G4double IonizationDrift::GetTransverseDiffusion() const
  { return _transv_diff; }

  inline void IonizationDrift::SetLongitudinalDiffusion(G4double ld)
  { _longit_diff = ld; }

  inline G4double IonizationDrift::GetLongitudinalDiffusion() const
  { return _longit_diff; }

  
  inline G4long IonizationDrift::NumberOfCarriers() const
  { return _num_electrons; }


} // end namespace nexus

#endif
