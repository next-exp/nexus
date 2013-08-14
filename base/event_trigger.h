/*   
 *  NEXUS: event_trigger
 *     - Created on: April 2008
 *     - Last modification: 
 * 
 *  ---
 *  Copyright (C) 2008 - J. Martin-Albo, J. Muñoz
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

#ifndef __EVENT_TRIGGER_H__
#define __EVENT_TRIGGER_H__ 1


#include "globals.hh"
#include "G4UserEventAction.hh"
#include "G4ParticleTable.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"

#include <bhep/bhep_svc.h>
#include <bhep/event.h>
#include <bhep/sreader.h>
#include <bhep/particle.h>


class G4Event;
class EventManager2;

namespace nexus {

  class event_trigger : public G4UserEventAction {
  
  public:
   
    // constructor, destructor
    event_trigger( EventManager2&, bhep::prlevel vl=bhep::NORMAL);
    ~event_trigger() {}
    
    // Geant4 User Events Actions
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
       
  private:
    bhep::messenger msg;

    EventManager2* __evt_mgr;
    G4String __dst_ext;
    G4int store_all_;
    G4double ini_part_mass_;    
  };

} // namespace nexus

#endif

    
