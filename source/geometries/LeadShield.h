// ----------------------------------------------------------------------------
///  \file   LeadShield.h
///  \brief  
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     27 Oct 2009
///  \version  $Id$
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------


#ifndef __LEAD_SHIELD__
#define __LEAD_SHIELD__

#include "BaseGeometry.h"


namespace nexus {

  class LeadShield: public BaseGeometry
  {
  public:
    /// constructor
    LeadShield(G4double X, G4double Y, G4double Z, G4double thickn);
    /// destructor
    ~LeadShield() {}

    
    void SetLength(G4double length);
    void SetWidth(G4double width);
    void SetHeight(G4double height);


    G4ThreeVector GenerateVertex(const G4String&) const;
    
  private:

    void DefineGeometry();

  private:

    G4double _thickness;
    G4double _width, _height, _length;
  };
  
} // end namespace nexus

#endif
