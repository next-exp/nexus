/*   
 *  NEXUS: tracking
 *     - Created on: Feb 2007
 *     - Last modification: 16 May 2007, JMALBOS
 * 
 *  ---
 *  Copyright (C) 2007 - J. Martin-Albo, J. Muñoz
 * 
 *  This file is part of NEXUS.
 *
 *  NEXUS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEXUS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEXUS; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *  ---
 */

#ifndef __TRACKING_H__
#define __TRACKING_H__ 1


#include "globals.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"
#include "G4Step.hh"

#include <bhep/bhep_svc.h>
#include <bhep/event.h>
#include <bhep/particle.h>
#include <bhep/track.h>
#include <bhep/material.h>
#include <bhep/engine.h>


namespace nexus {

  class tracking : public G4UserTrackingAction {
    
  private:
    G4String ini_part_;
    
  public:
    tracking( bhep::prlevel vl=bhep::NORMAL );
    ~tracking() {};
    
    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);
    
  private:
    bhep::messenger msg;
    bhep::particle& get_particle( G4int );
  };
  
} // namespace nexus

#endif
