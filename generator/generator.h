/*   
 *  NEXUS: generator
 *     - Created on: Feb 2007
 *     - Last modification: April 2008, JMALBOS
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

#ifndef __GENERATOR_H__
#define __GENERATOR_H__ 1


#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

#include "Randomize.hh"

#include "G4ParticleGun.hh"

#include <bhep/bhep_svc.h>
#include <bhep/sreader.h>


class G4ParticleGun;
class G4Event;

class EventManager2;


namespace nexus {

  class generator : public G4VUserPrimaryGeneratorAction {

  public:
    // constructor
    generator( const bhep::gstore&, const bhep::gstore&, 
	       EventManager2& , bhep::prlevel=bhep::NORMAL );
    // destructor
    ~generator();
    // Geant4 mandatory method
    void GeneratePrimaries( G4Event* );
    // set geometry volumes dimensions
    void set_active_dimensions( bhep::vdouble );
    void set_ivessel_dimensions( bhep::vdouble );
    void set_ovessel_dimensions( bhep::vdouble );
    void set_buffer_dimensions( bhep::vdouble );

  private:
  
    bhep::messenger msg;
    
    G4String __gen_filename;
    
    bhep::gstore* __gen_store;
    
    G4ParticleGun* __particle_gun;

    G4String __gen_type;

    G4String __particle_name;
    G4double __min_energy;
    G4double __max_energy;

    G4String __idst_filename;
    EventManager2* __evt_mgr;
    
    G4String __vtx_type;
    G4String __vtx_region;
    bhep::vdouble __vtx_spot;

    bhep::vdouble __ACTIVE_dimensions;
    bhep::vdouble __IVESSEL_dimensions;
    bhep::vdouble __OVESSEL_dimensions;
    bhep::vdouble __BUFFER_dimensions;
    
    
    void initialize();
    
    void read_params();
        
    void set_particle_gun();
    void read_event_from_idst(G4Event*);
    void set_particle_energy();
    void set_particle_position();
    void set_particle_direction();
  
    bhep::Point3D& get_vertex();
    bhep::Point3D& active_region();
    bhep::Point3D& ivessel_region();
    bhep::Point3D& ovessel_region();
  };

} // namespace nexus

#endif
