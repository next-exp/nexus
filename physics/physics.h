/*   
 *  NEXUS: physics
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

#ifndef __PHYSICS_H__
#define __PHYSICS_H__ 1


#include "G4VUserPhysicsList.hh"
#include "globals.hh"

#include <bhep/messenger.h>
#include <bhep/gstore.h>

namespace nexus {

  class physics: public G4VUserPhysicsList {
  
  public:
    
    physics( const bhep::gstore& physics_store, bhep::prlevel vl=bhep::NORMAL );
    ~physics();
    
  protected:
    
    void initialize();
    void read_params();

    // Construct particle and physics
    void ConstructParticle();
    void ConstructProcess();
    
    void SetCuts();
  
    // these methods Construct particles 
    void ConstructBosons();
    void ConstructLeptons();
    void ConstructMesons();
    void ConstructBaryons();
  
    // these methods Construct physics processes and register them
    void ConstructGeneral();
    void ConstructEM();
  
  private:

    bhep::messenger msg;
    
    bhep::gstore* __physics_store;

    G4int __energy_loss;
    G4int __mult_scattering;
    G4double __bulk_cut;
  };

} // namespace nexus

#endif
