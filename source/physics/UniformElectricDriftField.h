// -----------------------------------------------------------------------------
///  \file   UniformElectricDriftField.h
///  \brief  Uniform electric drift field
///  
///  \author  J Martin-Albo <jmalbos@ific.uv.es>    
///  \date    12 May 2010
///  \version $Id$
///
///  Copyright (c) 2010-2012 NEXT Collaboration. All rights reserved.
// -----------------------------------------------------------------------------

#ifndef UNIFORM_ELECTRIC_DRIFT_FIELD_H
#define UNIFORM_ELECTRIC_DRIFT_FIELD_H

#include "BaseDriftField.h"
#include <geomdefs.hh>

class G4Material;


namespace nexus {
  
  class SegmentPointSampler;


  /// This class defines a homogeneuos electric drift field with constant 
  /// drift lines from cathode to anode and parallel to a cartesian axis 

  class UniformElectricDriftField: public BaseDriftField
  {
  public:
    /// Constructor providing position of anode and cathode,
    /// and axis parallel to the drift lines
    UniformElectricDriftField(G4double anode_position=0., 
                              G4double cathode_position=0.,
                              EAxis axis=kZAxis); 
    
    /// Destructor
    ~UniformElectricDriftField();
    
    /// Calculate final state (position, time, drift time and length, etc.)
    /// of an ionization electron
    G4double Drift(G4LorentzVector& xyzt);
    
    G4LorentzVector GeneratePointAlongDriftLine(const G4LorentzVector&);
    
    // Setters/getters
    
    void SetAnodePosition(G4double);
    G4double GetAnodePosition() const;

    void SetCathodePosition(G4double);
    G4double GetCathodePosition() const;
    
    void SetDriftVelocity(G4double);
    G4double GetDriftVelocity() const;

    void SetLongitudinalDiffusion(G4double);
    G4double GetLongitudinalDiffusion() const;

    void SetTransverseDiffusion(G4double);
    G4double GetTransverseDiffusion() const;

    void SetAttachment(G4double);
    G4double GetAttachment() const;

    void SetLightYield(G4double);
    virtual G4double LightYield() const;

    void SetNumberOfPhotons(G4double);
    G4double GetNumberOfPhotons() const;
    
  private:
    /// Returns true if coordinate is between anode and cathode
    G4bool CheckCoordinate(G4double);



  private:
    
    EAxis _axis; ///< Axis parallel to field lines
    
    G4double _anode_pos;   ///< Anode position in axis _axis
    G4double _cathode_pos; ///< Cathode position in axis _axis

    G4double _drift_velocity; ///< Drift velocity of the charge carrier
    G4double _transv_diff;    ///< Transverse diffusion
    G4double _longit_diff;    ///< Longitudinal diffusion
    G4double _attachment;
    G4double _light_yield;
    G4double _num_ph;

    SegmentPointSampler* _rnd;

    static const G4double _secmargin;
  };

  
  // inline methods ..................................................
  
  inline void UniformElectricDriftField::SetAnodePosition(G4double p)
  { _anode_pos = p; }
  
  inline G4double UniformElectricDriftField::GetAnodePosition() const
  { return _anode_pos; }

  inline void UniformElectricDriftField::SetCathodePosition(G4double p)
  { _cathode_pos = p; }
  
  inline G4double UniformElectricDriftField::GetCathodePosition() const
  { return _cathode_pos; }

  inline void UniformElectricDriftField::SetDriftVelocity(G4double dv)
  { _drift_velocity = dv; }
  
  inline G4double UniformElectricDriftField::GetDriftVelocity() const
  { return _drift_velocity; }

  inline void UniformElectricDriftField::SetLongitudinalDiffusion(G4double ld)
  { _longit_diff = ld; }
  
  inline G4double UniformElectricDriftField::GetLongitudinalDiffusion() const
  { return _longit_diff; }

  inline void UniformElectricDriftField::SetTransverseDiffusion(G4double td)
  { _transv_diff = td; }
  
  inline G4double UniformElectricDriftField::GetTransverseDiffusion() const
  { return _transv_diff; }

  inline void UniformElectricDriftField::SetAttachment(G4double a)
  { _attachment = a; }

  inline G4double UniformElectricDriftField::GetAttachment() const
  { return _attachment; }

  inline void UniformElectricDriftField::SetLightYield(G4double ly)
  { _light_yield = ly; }

  inline G4double UniformElectricDriftField::LightYield() const
  { return _light_yield; }

  inline void UniformElectricDriftField::SetNumberOfPhotons(G4double nph)
  { _num_ph = nph; }

  

} // end namespace nexus

#endif
 

