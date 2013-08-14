// vertex_generator.cc
//    -- Created on: Feb 2007
//    -- Last modification: May 7, 2008 - JMALBOS
//

#include <nexus/generator.h>


namespace nexus {

  
  void generator::set_active_dimensions( bhep::vdouble dim)
  {
    __ACTIVE_dimensions = dim;
  }

  void generator::set_ivessel_dimensions( bhep::vdouble dim)
  {
    __IVESSEL_dimensions = dim;
  }

  void generator::set_ovessel_dimensions( bhep::vdouble dim)
  {
    __OVESSEL_dimensions = dim;
  }

  void generator::set_buffer_dimensions( bhep::vdouble dim)
  {
    __BUFFER_dimensions = dim;
  }

  
  // return a random vertex within the selected region
  bhep::Point3D& generator::get_vertex()
  {
    if (__vtx_region == "ivessel") {
      bhep::Point3D& vertex = ivessel_region();
      return vertex;
    }
    else if (__vtx_region=="ovessel") {
      bhep::Point3D& vertex = ovessel_region();
      return vertex;
    }
    else {
      bhep::Point3D& vertex = active_region();
      return vertex;
    }
  }


  // regions descriptions
  //

  // active volume
  bhep::Point3D& generator::active_region()
  {
    // Setting the position
    G4double pos_x = G4UniformRand() * __ACTIVE_dimensions[0] - __ACTIVE_dimensions[0]/2.;
    G4double pos_y = G4UniformRand() * __ACTIVE_dimensions[1] - __ACTIVE_dimensions[1]/2.;
    G4double pos_z = G4UniformRand() * __ACTIVE_dimensions[2] - __ACTIVE_dimensions[2]/2.;

    bhep::Point3D* vtx = new bhep::Point3D(pos_x, pos_y, pos_z);
    return *vtx;
  }

  
  bhep::Point3D& generator::ivessel_region()
  {
    size_t coord;
    G4double pos[3];
    
    // First, the wall where the vertex
    // will be generated is selected:
    // 0) YZ; 1) XZ; 2) XY. 
    // 
    //G4double rnd = G4UniformRand();
    //if ( rnd<1/3. )
    //coord=0;
    //else if ( rnd<2/3. )
    //coord=1;
    //else
    //coord=2;
    
    // (29/07/08) - jmalbos
    // We shoot them always from the readout wall
    coord=2;

    for (size_t i=0; i<3; i++) {
      
      if (coord==i) {
 	pos[i] = G4UniformRand() * 
	  (.5*__IVESSEL_dimensions[i] - .5*__ACTIVE_dimensions[i]) + 
	  0.5*__ACTIVE_dimensions[i];
 	
	//if( G4UniformRand()<0.5 )
	//pos[i]=-pos[i];
      }
      else
	pos[i] = G4UniformRand()*__IVESSEL_dimensions[i] - 0.5*__IVESSEL_dimensions[i];
    }
    
    bhep::Point3D* vtx = new bhep::Point3D(pos[0], pos[1], pos[2]);
    return *vtx;
  }



  bhep::Point3D& generator::ovessel_region()
  {
    size_t coord;
    G4double pos[3];
    
    // First, the wall where the vertex
    // will be generated is selected:
    // 0) YZ; 1) XZ; 2) XY. 
    // 
    G4double rnd = G4UniformRand();
    if (rnd < 1/3.) coord=0;
    else if (rnd < 2/3.) coord=1;
    else coord=2;
    
    for (size_t i=0; i<3; i++) {
      
      if (coord==i) {
 	pos[i] = G4UniformRand() * 
	  (.5*__OVESSEL_dimensions[i] - .5*__BUFFER_dimensions[i]) + 
	  0.5*__BUFFER_dimensions[i];
 	
	if (G4UniformRand() < 0.5) pos[i]=-pos[i];
      }
      else
	pos[i] = G4UniformRand()*__OVESSEL_dimensions[i] - 0.5*__OVESSEL_dimensions[i];
    }
    
    bhep::Point3D* vtx = new bhep::Point3D(pos[0], pos[1], pos[2]);
    return *vtx;
  }


} //namespace nexus
