/*   
 *  NEXUS: stepping_trigger
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

#ifndef __STEPPING_TRIGGER_H__
#define __STEPPING_TRIGGER_H__ 1


#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4VTouchable.hh"

#include "G4SteppingVerbose.hh"

#include <bhep/bhep_svc.h>
#include <bhep/engine.h>
#include <bhep/point.h>
#include <bhep/event.h>
#include <bhep/particle.h>
#include <bhep/hit.h>
#include <bhep/utilities.h>
#include <bhep/sreader.h>

#include <fstream>
#include <sstream>
#include <vector>


namespace nexus {

  class stepping_trigger : public G4UserSteppingAction {

  private:
    bhep::messenger msg;
    
  public:
    stepping_trigger( bhep::prlevel vl=bhep::NORMAL );
    ~stepping_trigger() {}

  public:
    void UserSteppingAction(const G4Step*);
    bhep::particle& get_particle( G4int );
  };

} // namespace nexus

#endif
