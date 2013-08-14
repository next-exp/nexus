// ----------------------------------------------------------------------------
//  $Id: HexagonNavigator.cc 2598 2010-13-05 10:34Z francesc $
//
//  Author : Francesc Monrabal Capilla <francesc.monrabal@ific.uv.es>
//  Created: 13 May 2010
//
//  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "HexagonNavigator.h"

#include "ConfigService.h"

#include <Randomize.hh>
#include <G4RunManager.hh>

#include <vector>


namespace nexus {
  
  
  HexagonNavigator::HexagonNavigator(G4double inner_radius,
				     G4double inner_length, 
				     G4double binning,
				     G4ThreeVector origin,
				     G4RotationMatrix* rotation):
    _inner_radius(inner_radius), _inner_length(inner_length), 
    _binning(binning), _origin(origin), _rotation(rotation)
  {
    i = 0; 
    
    _number_events =
      ConfigService::Instance().Job().GetIParam("number_events");
    
    //_vertexes = this->TriangleWalker(_inner_radius, binning,_inner_length/2.-2.*mm);
  }
  
  
  
  HexagonNavigator::~HexagonNavigator()
  {
  }
  
  
  
  G4ThreeVector HexagonNavigator::GenerateVertex(const G4String& region)
  {
    G4double x, y, z;
    
    // Center of the chamber
    if (region == "CENTER") {
      return RotateAndTranslate(G4ThreeVector(0., 0., 0.)); 
    }
    
    // Generating in the body volume
    else if (region == "INSIDE") {

      ///Three Vertex of the Triangle
      G4ThreeVector A(0.,0.,0.);
      G4ThreeVector P(0.,0.,0.);
      G4ThreeVector B(_inner_radius,0.,0.);
      G4ThreeVector C(_inner_radius*cos(pi/3.), _inner_radius*sin(pi/3.), 0.);
      
      G4double ran1, ran2, c;
      ran1 = G4UniformRand();
      ran2 = G4UniformRand();
      
      
      if((ran1+ran2)>=1.){
	ran1 = 1-ran1;
	ran2 = 1-ran2;
      }

      c = 1-ran1-ran2;

      ///Random point in single triangle
      P[0] = ran1*A[0]+ran2*B[0]+c*C[0];
      P[1] = ran1*A[1]+ran2*B[1]+c*C[1];
      P[2] = G4UniformRand()*_inner_length-_inner_length/2.;

      ///Rotate the point
      G4double dsector = 6.*G4UniformRand();
      G4double  isector;
      modf(dsector, &isector);
      G4double phi = isector*pi/3.;
      
      G4double p0 = cos(phi)*P[0]-sin(phi)*P[1];
      P[1] = cos(phi)*P[1]+sin(phi)*P[0];
      P[0] = p0;

      return P;
    }
    ///Only for create the look-up tables

    else if (region == "TABLE") {


      if(_number_events<_vertexes.size()){
	G4cout<<"number events too small, you need at least "<<_vertexes.size()
	      <<" events to generate tables"<<G4endl;

	G4cout<<"Aborting the run ..."<<G4endl;
	G4RunManager::GetRunManager()->AbortRun();
      }
      else{
	i++;	

	if(i>=_vertexes.size()){
	  G4cout<<"Aborting the run ..."<<G4endl;
	  G4RunManager::GetRunManager()->AbortRun();
	}

	//	G4cout<<"i = "<<i<<G4endl;
	if(i<=_vertexes.size()){

	  return _vertexes[i-1];
	}


      }
    }
    // Unknown region
    else {
      G4Exception("[HexagonNavigator] ERROR: Unknown region!");

    }
  }
  
  
  
  G4double HexagonNavigator::GetRadius(G4double inner, G4double outer) 
  {
    G4double rnd = G4UniformRand();
    G4double r = sqrt((1.-rnd) * inner*inner + rnd * outer*outer);
    return r;
  }
  
  
  
  G4double HexagonNavigator::GetPhi() 
  {
    return (G4UniformRand() * twopi);
  }
  
  
  
  G4double HexagonNavigator::GetLength(G4double origin, G4double max_length) 
  {
    return (origin + (G4UniformRand() - 0.5) * max_length);
  }
  
  
  
  G4ThreeVector 
  HexagonNavigator::RotateAndTranslate(const G4ThreeVector& position) 
  {
    G4ThreeVector real_position(position);
    
    // Rotating if needed
    if (_rotation) 
      real_position *= (*_rotation);
    // Translating
    real_position += _origin;
    
    return real_position;
  }
  
  
  
  std::vector<G4ThreeVector> 
  HexagonNavigator::TriangleWalker(G4double radius, G4double binning, G4double z)
  {
    G4double x, y= 0; 

    std::vector<G4ThreeVector> vertexes;

    G4double apothem;
    G4int flag = 0;
    apothem = radius * cos(pi/6.);
    
    G4ThreeVector vertex(x,y,z);
    G4double phi = pi/6.;

    while(x<=radius/2.){

      y = flag*binning/cos(pi/6.);
      flag++;

      while (y<=apothem){
	
	//	G4double v0 = cos(phi)*vertex[0]-sin(phi)*vertex[1];
	//	vertex[1] = cos(phi)*vertex[1]+sin(phi)*vertex[0];
	//	vertex[0] = v0;
	
	y = y + binning;
	vertex[1] = y;

	vertexes.push_back(vertex);

      }

      x = x + binning/sin(pi/6.);
      vertex[0] = x;
      vertex[1] = y;

    }

    return vertexes;
  }



  // G4ThreeVector
  // HexagonNavigator::TriangleWalker(G4double radius, G4double binning, G4double z)
  // {
  //   G4double x, y= 0; 

  //   std::vector<G4ThreeVector> vertexes;

  //   G4double apothem;
  //   G4int flag = 0;
  //   apothem = radius * cos(pi/6.);
    
  //   G4ThreeVector vertex(x,y,z);
  //   G4double phi = pi/6.;

  //   while(x<=radius/2.){

  //     y = flag*binning/cos(pi/6.);
  //     flag++;

  //     while (y<=apothem){
	
  // 	//	G4double v0 = cos(phi)*vertex[0]-sin(phi)*vertex[1];
  // 	//	vertex[1] = cos(phi)*vertex[1]+sin(phi)*vertex[0];
  // 	//	vertex[0] = v0;
	
  // 	y = y + binning;
  // 	vertex[1] = y;

  // 	vertexes.push_back(vertex);

  //     }

  //     x = x + binning/sin(pi/6.);
  //     vertex[0] = x;
  //     vertex[1] = y;

  //   }

  //   return vertexes;
  // }





  std::vector<G4ThreeVector> 
  HexagonNavigator::SetDetector(G4double rmin, G4double rmax, G4double z, 
				G4bool fixed_rmax )
  {
    std::vector<G4ThreeVector> detectors;
    
    G4double x, y= 0; 
    G4ThreeVector position = (x,y,z);
    G4double pos1 = 0;
    //      G4ThreeVector SiPM_dim = basic_SiPM->GetDimensions();

    position[2] = z;

    G4int sections;
    G4double layers;

    sections = rmax/rmin;

    if(fixed_rmax)
      rmin = sections/rmax;

    layers = (sections+1.)/2.;

    G4double Sdiam = ((2.*rmax)/sections);

    //      z = (total_length-SiPM_dim[2])/2.;

    //Place PMTs
    for(G4int i=0;i<(sections+1)/2;i++){   
      pos1 = -rmax+Sdiam/2.+i*Sdiam/2.;
      position[1] = i*Sdiam*sin(pi/3.);
      for(G4int j=0;j<sections-i;j++){
          
	position[0] = pos1+Sdiam*j;
	detectors.push_back(position);

	if(position[1]!=0){
	  position[1] = -position[1];
	  detectors.push_back(position);
	}
	/*
          physi_SiPM = 
	  new G4PVPlacement(0, SiPM_pos,
	  logic_SiPM, "SiPM_L1"+bhep::to_string(i),
	  active_logic, false, 0);
          
          if(i!=0)
	  {
	  physi_SiPM = 
	  new G4PVPlacement(0, 
	  G4ThreeVector(-SiPM_pos[0],
	  SiPM_pos[1],SiPM_pos[2]),
	  logic_SiPM, "SiPM_L1"+bhep::to_string(i),
	  active_logic, false, 0);
              
	  }*/
      }
    }
  


    //      G4cout<<"num PMTs = "<<num_PMT<<G4endl;

    return detectors;


  }


} // end namespace nexus
