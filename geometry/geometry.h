/*   
 *  NEXUS: geometry
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

#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__ 1


#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

#include "G4Region.hh"
#include "G4LogicalVolume.hh"

#include "G4VisAttributes.hh"

#include <bhep/bhep_svc.h>
#include <bhep/sreader.h>
#include <bhep/engine.h>


class G4Box;
class G4Tubs;

class G4LogicalVolume;
class G4VPhysicalVolume;

class G4Material;

class G4VisAttributes;


namespace nexus {

  class geometry : public G4VUserDetectorConstruction {

  public:
    
    /// constructor
    geometry( const bhep::gstore&, const bhep::gstore&, bhep::prlevel vl=bhep::NORMAL );
    /// destructor
    ~geometry();
    /// Geant4 mandatory method
    G4VPhysicalVolume* Construct();
    // Return volume dimensions
    bhep::vdouble get_active_dimensions();
    bhep::vdouble get_ovessel_dimensions();
    bhep::vdouble get_ivessel_dimensions();
    bhep::vdouble get_buffer_dimensions();
    
  private:
  
    /// Materials
    G4Material* He;
    G4Material* C;
    G4Material* Al;
    G4Material* Fe;
    G4Material* Cu;
    G4Material* Se;
    G4Material* Mo;
    G4Material* Nd;
    G4Material* Pb;
    G4Material* Air;
    G4Material* H2O;
    G4Material* Sapphire;
    G4Material* Concrete;
    G4Material* Scint;
    G4Material* NaI_scint;
    G4Material* CsI_scint;
    G4Material* Polystyrene;
    G4Material* Vacuum;
    G4Material* Mylar;
    G4Material* Inox;
    G4Material* Tracking_gas;
    G4Material* HPXe;
    G4Material* HPNe;
    G4Material* HPNeXe;
    G4Material* HP10Ne90Xe;
    G4Material* HP5Ne95Xe;
    G4Material* HP1Ne99Xe;
    G4Material* LXe;
  
    /// Visibilities 
    G4VisAttributes* invis_VisAtt;
    G4VisAttributes* grey_VisAtt;
    G4VisAttributes* blue_VisAtt;
    G4VisAttributes* brown_VisAtt;
    G4VisAttributes* green_VisAtt;
    G4VisAttributes* purple_VisAtt;
    G4VisAttributes* pink_VisAtt;
  
    /// Volumes (solid, logical, physical)
    // WORLD
    G4Box*              solid_WORLD;    
    G4LogicalVolume*    logic_WORLD;    
    G4VPhysicalVolume*  physi_WORLD;    
    // Outer VESSEL
    G4Box*              solid_OVESSEL;    
    G4LogicalVolume*    logic_OVESSEL;    
    G4VPhysicalVolume*  physi_OVESSEL;    
    // BUFFER
    G4Box*              solid_BUFFER;    
    G4LogicalVolume*    logic_BUFFER;    
    G4VPhysicalVolume*  physi_BUFFER;    
    // Inner VESSEL
    G4Box*              solid_IVESSEL;    
    G4LogicalVolume*    logic_IVESSEL;    
    G4VPhysicalVolume*  physi_IVESSEL;    
    // ACTIVE volume
    G4Box*              solid_ACTIVE;
    G4LogicalVolume*    logic_ACTIVE;    
    G4VPhysicalVolume*  physi_ACTIVE;    
      
    /// Regions
    G4Region* bulk_region;
    

  private:

    bhep::messenger msg;

    bhep::gstore* __geom_store;
    bhep::gstore* __physics_store;

    G4String __geom_filename, __physics_filename;
  
    G4String __CHAMBER_shape;
    G4double __IVESSEL_thickness, __OVESSEL_thickness;
    G4double __BUFFER_gap;
    G4double __pressure;
    G4double __bulk_max_step;
    
    bhep::vdouble __ACTIVE_dimensions;
    bhep::vdouble __IVESSEL_dimensions;
    bhep::vdouble __BUFFER_dimensions;
    bhep::vdouble __OVESSEL_dimensions;
    
      
    void read_geom_params();
    void read_physics_params();
  
    void calculate_dimensions();

    void define_world();
    void define_ovessel();
    void define_buffer();
    void define_ivessel();
    void define_active();
  
    void define_regions();

    void define_materials();
  
    void set_visibilities();
  };

} // namespace nexus

#endif
