// ----------------------------------------------------------------------------
///  \file   UniformElectricDriftField.h
///  \brief  
///  
///  \author  J Martin-Albo <jmalbos@ific.uv.es>    
///  \date    12 May 2010
///  \version $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __UNIFORM_ELECTRIC_DRIFT_FIELD__
#define __UNIFORM_ELECTRIC_DRIFT_FIELD__

#include "BaseDriftField.h"
#include <geomdefs.hh>

class G4Material;


namespace nexus {
  
  class AlongStepRandomGenerator;


  /// This class defines a homogeneuos electric drift field with drift
  /// lines parallel to a cartesian axis and a constant drift velocity.

  class UniformElectricDriftField: public BaseDriftField
  {
  public:
    /// Constructor
    UniformElectricDriftField(G4double anode_position=0., 
			      G4double cathode_position=0.,
			      EAxis axis=kZAxis, 
			      G4double field_strength=0,
			      G4Material* material=0);
    
    /// Destructor
    ~UniformElectricDriftField();

    /// Returns the final position and time of a charge carrier
    /// that has drifted under the influence of the field
    G4LorentzVector Drift(const G4LorentzVector& origin,
			  DriftTrackInfo* drift_info);

    G4LorentzVector GeneratePointAlongDriftLine(const G4LorentzVector&);
    
    void SetAnodePosition(G4double);
    G4double GetAnodePosition() const;

    void SetCathodePosition(G4double);
    G4double GetCathodePosition() const;

    void SetFieldStrength(G4double);
    G4double GetFieldStrength() const;

    void SetMaterial(G4Material*);
    G4Material* GetMaterial() const;

    void SetDriftVelocity(G4double);
    G4double GetDriftVelocity() const;

    void SetLongitudinalDiffusion(G4double);
    G4double GetLongitudinalDiffusion() const;

    void SetTransverseDiffusion(G4double);
    G4double GetTransverseDiffusion() const;

    void SetELLightYield(G4double);
    G4double ELLightYield() const;

  private:
    
    void GetMaterialProperties();

    /// Returns true if coordinate is between anode and cathode
    G4bool CheckCoordinate(G4double);

    G4double LightYield();

  private:
    
    EAxis _axis;               ///< Axis parallel to field lines
    
    G4double _anode_pos;       ///< Anode position in axis _axis
    G4double _cathode_pos;     ///< Cathode position in axis _axis

    G4double _field_strength;  ///< Electric drift field intensity
    G4double _drift_velocity;
    G4double _transv_diff;
    G4double _longit_diff;

    G4Material* _material;

    AlongStepRandomGenerator* _rnd;
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

  inline void UniformElectricDriftField::SetFieldStrength(G4double fs)
  { _field_strength = fs; }

  inline G4double UniformElectricDriftField::GetFieldStrength() const
  { return _field_strength; }

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

  inline void UniformElectricDriftField::SetMaterial(G4Material* mat)
  { _material = mat;
    GetMaterialProperties(); }

  inline G4Material* UniformElectricDriftField::GetMaterial() const
  { return _material; }

} // end namespace nexus

#endif
