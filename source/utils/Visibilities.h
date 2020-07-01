// ----------------------------------------------------------------------------
///  \file   Visibilities.h
///  \brief  General class to define colors for displaying geometries
// ----------------------------------------------------------------------------
// nexus | Visibilities.h
//
// This class define colors for geometry display.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include <G4VisAttributes.hh>

#ifndef VIS_H
#define VIS_H

namespace nexus {

  inline G4VisAttributes Yellow()
  { G4Colour myColour; G4Colour::GetColour("yellow", myColour); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes White()
  { G4Colour myColour; G4Colour::GetColour("white", myColour); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes Red()
  { G4Colour myColour(1., 0., 0.); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes DarkRed()
  { G4Colour myColour(.88, .87, .86); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes BloodRed()
  { G4Colour myColour(.55, .09, .09); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes DarkGreen()
  { G4Colour myColour(0., .6, 0.); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes LightGreen()
  { G4Colour myColour(.6,.9,.2); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes DirtyWhite()
  { G4Colour myColour(1., 1., .8); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes CopperBrown()
  { G4Colour myColour(.72, .45, .20); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes Brown()
  { G4Colour myColour(.93, .87, .80); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes Blue()
  { G4Colour myColour(0., 0., 1.); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes LightBlue()
  { G4Colour myColour(.6, .8, .79); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes Lilla()
  { G4Colour myColour(.5,.5,.7); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes DarkGrey()
  { G4Colour myColour(.3, .3, .3); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes LightGrey()
  { G4Colour myColour(.7,.7,.7); G4VisAttributes myAttr(myColour); return myAttr;}
  inline G4VisAttributes TitaniumGrey()
  { G4Colour myColour(.71, .69, .66); G4VisAttributes myAttr(myColour); return myAttr;}


}  // end namespace nexus

#endif
