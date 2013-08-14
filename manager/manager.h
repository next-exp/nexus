/*   
 *  NEXUS: manager
 *     - Created on: June 2007
 *     - Last modification: June 2, 2008 (jmalbos)
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

#ifndef __MANAGER_H__
#define __MANAGER_H__ 1


#include "globals.hh"

#include <bhep/bhep_svc.h>
#include <bhep/gstore.h>

#include <bhep/EventManager2.h>


namespace nexus {

  class manager {

  public:
    
    // constructor/destructor
    manager( std::string params_filename, bhep::prlevel vl=bhep::NORMAL );
    ~manager();
    
    // init method
    void initialize();
    
    // return verbosity
    bhep::prlevel get_verbosity();
    std::string get_tracking_verbosity();

    int get_trigger();
    
    // return number of events
    int get_num_events();
    
    // return bhep EventManager2 instance
    EventManager2& get_event_mgr();
    
    // return filenames
    const std::string& get_params_filename() const { return __params_filename; }
    const std::string& get_geom_filename();
    
    // return gstores with the options parameters
    const bhep::gstore& get_global_store();
    const bhep::gstore& get_geom_store();
    const bhep::gstore& get_physics_store();
    const bhep::gstore& get_generator_store();

  private:
    
    // set the manager
    void set_defaults();
    void read_params(bhep::gstore&, std::string, std::string);
    void set_params_stores();
    void set_event_mgr();
    void set_odst_header();
    
    /// check user inputs consistency
    void check_global_params();
    void check_physics_params();
    void check_generator_params();
    void check_geom_params();
    
    void assert_option( bool ok, const std::string& option );
    void assert_init( const std::string& );
    void option_is_not_valid( const std::string& option, const std::string& value );
        
  private:
    
    bhep::messenger __msg;
    
    bool __init;
    
    std::string __params_filename;
    std::string __geom_filename;
    std::string __idst_filename;
    std::string __odst_filename;
    
    std::string __verbosity;
    std::string __G4_verbosity;
    
    int __energy_loss;
    int __mult_scattering;
    
    EventManager2* __evt_mgr; // no namespace
    
    bhep::gstore __physics_store;
    bhep::gstore __geom_store;
    bhep::gstore __generator_store;
    bhep::gstore __debug_store;
    bhep::gstore __global_store;
  };

} // namespace nexus

#endif

