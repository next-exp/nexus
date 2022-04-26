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

  inline G4VisAttributes Yellow()       { return { G4Colour::Yellow() }; }
  inline G4VisAttributes White()        { return { G4Colour::White () }; }
  inline G4VisAttributes Red()          { return { {1   ,  .0 ,  .0 } }; }
  inline G4VisAttributes DarkRed()      { return { { .88,  .87,  .86} }; }
  inline G4VisAttributes BloodRed()     { return { { .55,  .09,  .09} }; }
  inline G4VisAttributes DarkGreen()    { return { { .0 ,  .6 ,  .0 } }; }
  inline G4VisAttributes LightGreen()   { return { { .6 ,  .9 ,  .2 } }; }
  inline G4VisAttributes DirtyWhite()   { return { {1   , 1   ,  .8 } }; }
  inline G4VisAttributes CopperBrown()  { return { { .72,  .45,  .20} }; }
  inline G4VisAttributes Brown()        { return { { .93,  .87,  .80} }; }
  inline G4VisAttributes Blue()         { return { { .0 ,  .0 , 1   } }; }
  inline G4VisAttributes LightBlue()    { return { { .6 ,  .8 ,  .79} }; }
  inline G4VisAttributes Lilla()        { return { { .5 ,  .5 ,  .7 } }; }
  inline G4VisAttributes DarkGrey()     { return { { .3 ,  .3 ,  .3 } }; }
  inline G4VisAttributes LightGrey()    { return { { .7 ,  .7 ,  .7 } }; }
  inline G4VisAttributes TitaniumGrey() { return { { .71,  .69,  .66} }; }
  
  // Set colours with alpha for transparency
  inline G4VisAttributes YellowAlpha()       { return { {1.0 , 1.0 ,  0.0, .5} }; }
  inline G4VisAttributes WhiteAlpha()        { return { {1.0 , 1.0 ,  1.0, .3} }; }
  inline G4VisAttributes RedAlpha()          { return { { 1. ,  .0 ,  .0 , .5} }; }
  inline G4VisAttributes DarkRedAlpha()      { return { { .88,  .87,  .86, .5} }; }
  inline G4VisAttributes BloodRedAlpha()     { return { { .55,  .09,  .09, .5} }; }
  inline G4VisAttributes DarkGreenAlpha()    { return { { .0 ,  .6 ,  .0 , .5} }; }
  inline G4VisAttributes LightGreenAlpha()   { return { { .6 ,  .9 ,  .2 , .3} }; }
  inline G4VisAttributes DirtyWhiteAlpha()   { return { {1   , 1   ,  .8 , .5} }; }
  inline G4VisAttributes CopperBrownAlpha()  { return { { .72,  .45,  .20, .2} }; }
  inline G4VisAttributes BrownAlpha()        { return { { .93,  .87,  .80, .5} }; }
  inline G4VisAttributes BlueAlpha()         { return { { .0 ,  .0 , 1   , .5} }; }
  inline G4VisAttributes LightBlueAlpha()    { return { { .6 ,  .8 ,  .79, .5} }; }
  inline G4VisAttributes LillaAlpha()        { return { { .5 ,  .5 ,  .7 , .5} }; }
  inline G4VisAttributes DarkGreyAlpha()     { return { { .3 ,  .3 ,  .3 , .5} }; }
  inline G4VisAttributes LightGreyAlpha()    { return { { .7 ,  .7 ,  .7 , .5} }; }
  inline G4VisAttributes TitaniumGreyAlpha() { return { { .71,  .69,  .66, .5} }; }

}  // end namespace nexus

#endif
