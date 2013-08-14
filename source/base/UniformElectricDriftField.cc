// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 12 May 2010
//
//  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "UniformElectricDriftField.h"

#include "DriftTrackInfo.h"
#include "AlongStepRandomGenerator.h"

#include <G4Material.hh>
#include <G4MaterialPropertiesTable.hh>


namespace nexus {
  
  
  UniformElectricDriftField::UniformElectricDriftField
  (G4double anode_position, G4double cathode_position, EAxis axis, 
   G4double field_strength, G4Material* material): 
    BaseDriftField(), _axis(axis), _anode_pos(anode_position),
    _cathode_pos(cathode_position), _field_strength(field_strength), 
    _drift_velocity(0.), _longit_diff(0.), _transv_diff(0.), 
    _material(material)
  {
    // initialize random generator with dummy values
    _rnd = new AlongStepRandomGenerator(G4LorentzVector(0.,0.,0.,-999.),
					G4LorentzVector(0.,0.,0.,-999.));
  }
  
  
  
  UniformElectricDriftField::~UniformElectricDriftField()
  {
    delete _rnd;
  }



  void UniformElectricDriftField::GetMaterialProperties()
  {
    // FIXME. Drift velocity and diffusion should be fetched from
    // the material properties table.
    
    // G4MaterialPropertiesTable* mpt = _material->GetMaterialPropertiesTable();
    
    // if (!mpt) {
    //   G4cout << "[UniformElectricDriftField] " 
    // 	     << "WARNING.- Material " << _material->GetName()
    // 	     << " has no properties defined." << G4endl;
    // }
  }
  
  
  
  G4LorentzVector
  UniformElectricDriftField::Drift(const G4LorentzVector& origin,
				   DriftTrackInfo* drift_info)
  {
    if (!CheckCoordinate(origin[_axis]))
      return origin;
    
    G4double drift_length = _anode_pos - origin[_axis];
    G4double drift_time = drift_length / _drift_velocity;
    
    G4double trasnv_sigma = _transv_diff * sqrt(drift_length);
    G4double longit_sigma = _longit_diff * sqrt(drift_length);
    G4double time_sigma = longit_sigma / _drift_velocity;
   
    G4ThreeVector position = origin.v();
    position[_axis] = _anode_pos + 1.*mm;
    G4double time = origin.t() + drift_time;

    G4LorentzVector end(position, time);

    if (drift_info) {
      drift_info->SetDriftStatus(false);
      drift_info->SetDriftLength(drift_length);
      drift_info->SetDriftTime(drift_time);
      drift_info->SetTimeSpread(time_sigma);
      drift_info->SetLongitudinalSpread(longit_sigma);
      drift_info->SetTransverseSpread(trasnv_sigma);
      // G4double y = LightYield() * drift_length;
      // G4cout << "y = " << y << G4endl;
      drift_info->SetELLightYield(LightYield()*drift_length);
    }
    
    return end;
  }
  
  
  
  G4LorentzVector UniformElectricDriftField::GeneratePointAlongDriftLine
  (const G4LorentzVector& origin)
  {
    if (origin != _rnd->GetPrePoint()) {
      G4LorentzVector end = Drift(origin, 0);
      _rnd->SetPoints(origin, end);
    }
    
    return _rnd->Shoot();
  }



  G4bool UniformElectricDriftField::CheckCoordinate(G4double coord)
  {
    // Check if origin is between anode and cathode
    G4double max_coord = std::max(_anode_pos, _cathode_pos);
    G4double min_coord = std::min(_anode_pos, _cathode_pos);

    return !((coord > max_coord) || (coord < min_coord));
  }
  
  
  G4double UniformElectricDriftField::LightYield()
  {
    G4double a = 140. / (kilovolt * cm);
    G4double b = 116. / (bar * cm);

    G4double pressure = _material->GetPressure();

    G4double yield = (a * _field_strength - b * pressure);
    
    if (yield < 0.) yield = 0.;

    return yield;
  }



} // end namespace nexus
