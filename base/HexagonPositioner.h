// ----------------------------------------------------------------------------
///  \file   HexagonPositioner.h
///  \brief  
///
///  \author  Luis Serra <luis.serra@ific.uv.es>
///  \date    3 March 2010 
///  \version $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __HEXAGON_POSITIONER__
#define __HEXAGON_POSITIONER__

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>


namespace nexus {

  /// FIXME. Class description.

  class HexagonPositioner
  {
  public:
    /// Constructor
    HexagonPositioner(G4double R, G4double size);
    /// Destructor
    ~HexagonPositioner() {};


    
    void GetPosition(G4int, G4int);

    
    G4int NumberOfElements() const;

  private:
    ///
    void ResizeGrid();

  private:
    G4double _radius; ///< Radius of the holder big hexagon

    ///Radius of the elements to fill the big hexagon
    G4double _element_radius;


    G4int _num_elements; ///< Number of elements
    G4int _cell_number;

    ///Number of rings 
    G4int _ring_order;
    G4ThreeVector _position;
};


inline G4int HexagonPositioner::NumberOfElements() const 
{ return _num_elements; }


} // end namespace nexus

#endif



