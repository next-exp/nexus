// ----------------------------------------------------------------------------
///  \file   CylindricInLeadShield.h
///  \brief  
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     27 Oct 2009
///  \version  $Id$
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------


#ifndef __CYLINDRIC_IN_LEAD_SHIELD__
#define __CYLINDRIC_IN_LEAD_SHIELD__

#include "BaseGeometry.h"


namespace nexus {

  class CylindricChamber;
  class LeadShield;


  class CylindricInLeadShield: public BaseGeometry
  {
  public:
    /// constructor
    CylindricInLeadShield();
    /// destructor
    ~CylindricInLeadShield();

    //G4ThreeVector GenerateVertex(const G4String&) const;
    
  private:

    void SetParameters();
    void DefineGeometry();

  private:

    CylindricChamber* _chamber;
    LeadShield* _shield;

  };
  
} // end namespace nexus

#endif
