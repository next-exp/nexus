// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 12 May 2010
//
//  Copyright (c) 2010-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "UniformElectricDriftField.h"
#include "SegmentPointSampler.h"

#include <Randomize.hh>

#include "CLHEP/Units/SystemOfUnits.h"


namespace nexus {
  
  using namespace CLHEP;

  const G4double UniformElectricDriftField::_secmargin = 1. * micrometer;

  

  UniformElectricDriftField::UniformElectricDriftField
  (G4double anode_position, G4double cathode_position, EAxis axis): 
    BaseDriftField(), 
    _axis(axis), _anode_pos(anode_position), _cathode_pos(cathode_position), 
    _drift_velocity(0.), _transv_diff(0.), _longit_diff(0.)   
  {
    // initialize random generator with dummy values
    _rnd = new SegmentPointSampler(G4LorentzVector(0.,0.,0.,-999.),
                                   G4LorentzVector(0.,0.,0.,-999.));
  }
  
  
  
  UniformElectricDriftField::~UniformElectricDriftField()
  {
    delete _rnd;
  }
  


  G4double UniformElectricDriftField::Drift(G4LorentzVector& xyzt)
  {
    // If the origin is not between anode and cathode,
    // the charge carrier, obviously, doesn't move.
    if (!CheckCoordinate(xyzt[_axis]))
      return 0.;

    // Calculate drift time and distance to anode
    G4double drift_length = _anode_pos - xyzt[_axis];
    G4double drift_time = drift_length / _drift_velocity;
    
    // Calculate longitudinal and transversal deviation due to diffusion
    G4double transv_sigma = _transv_diff * sqrt(drift_length);
    G4double longit_sigma = _longit_diff * sqrt(drift_length);
    G4double time_sigma = longit_sigma / _drift_velocity;

    G4ThreeVector position;
    G4double time;

    for (G4int i=0; i<3; i++) {
      if (i != _axis)  {     // Transverse coordinate
        position[i] = G4RandGauss::shoot(xyzt[i], transv_sigma);
      } 
      else { // Longitudinal coordinate
        position[i] = _anode_pos + _secmargin;
        G4double deltat = G4RandGauss::shoot(0, time_sigma);
        time = xyzt.t() + drift_time + deltat;
        if (time < 0.) time = xyzt.t() + drift_time;
      }
    }

    // Calculate step length as euclidean distance between initial
    // and final positions
    G4ThreeVector displacement = position - xyzt.vect();
    G4double step_length = displacement.mag();

    // Set the new time and position of the drifting charge
    xyzt.set(time, position);
    
    return step_length;
  }

  
  
  G4LorentzVector UniformElectricDriftField::GeneratePointAlongDriftLine(
    const G4LorentzVector& origin)
  {
    if (origin != _rnd->GetPrePoint()) {
      G4LorentzVector end(origin);
      Drift(end);
      _rnd->SetPoints(origin, end);
    }
    
    return _rnd->Shoot();
  }



  G4bool UniformElectricDriftField::CheckCoordinate(G4double coord)
  {
    G4double max_coord = std::max(_anode_pos, _cathode_pos);
    G4double min_coord = std::min(_anode_pos, _cathode_pos);
    return !((coord > max_coord) || (coord < min_coord));
  }


} // end namespace nexus
