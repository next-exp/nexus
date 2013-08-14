// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : Luis Serra <luis.serra@ific.uv.es>    
//  Created: 3 March 2010
//  
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "HexagonPositioner.h"


namespace nexus {
  
  
  HexagonPositioner::HexagonPositioner(G4double R, G4double size):
    _radius(R)
  {
    ResizeGrid();
  }


  
  ///Task gets the number of elements, resizes the hexagons to fit the outher size 
  ///and calcules the number of rings around the central hexagon
  void HexagonPositioner::ResizeGrid()
  {
    G4double apothem = sqrt(3) * _radius/2.;

    _ring_order = int(2*(apothem/_element_radius-1)/3.);

    G4int total_cells = 0;
    for (int i=0;i<=_ring_order;i++)
      {
	total_cells += pow(6.,i);
      }	

    _cell_number = total_cells;

    G4double new_element_radius = apothem/(1+3*_ring_order/2.);
 
    _element_radius = new_element_radius;
  }
  
  
  
  ///We use the base of the hexagonal grid to define the elements
  void HexagonPositioner::GetPosition(G4int row, G4int column)
  {  
    double x  = _element_radius/2.*sqrt(3.)*(row-column);
    double y = _element_radius/2.*3.*(row+column);

    _position.setX(x);
    _position.setY(y);
    _position.setZ(0);
    
  } 

  

} //end namespace nexus
