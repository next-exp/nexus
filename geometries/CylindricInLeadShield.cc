// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 27 Oct 2009
//  
//  Copyright (c) 2009 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "CylindricInLeadShield.h"
#include "ConfigService.h"
#include "CylindricChamber.h"
#include "LeadShield.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>


namespace nexus {


  CylindricInLeadShield::CylindricInLeadShield(): BaseGeometry()
  {
    SetParameters();
  }



  CylindricInLeadShield::~CylindricInLeadShield()
  {
  }


  
  void CylindricInLeadShield::SetParameters()
  {
    const ParamStore& cfg = ConfigService::Instance().Geometry();
  }



  void CylindricInLeadShield::DefineGeometry()
  {
    _chamber = new CylindricChamber();
    //_shield = new LeadShield();

    G4LogicalVolume* chamber_logic = _chamber->GetLogicalVolume();
    
  }
  

} // end namespace nexus
