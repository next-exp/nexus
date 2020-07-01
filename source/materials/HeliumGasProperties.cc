// ----------------------------------------------------------------------------
// nexus | HeliumGasProperties.cc
//
// This class collects the relevant physical properties of gaseous helium.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "HeliumGasProperties.h"

#include <G4AnalyticalPolSolver.hh>
#include <G4MaterialPropertiesTable.hh>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <stdexcept>


namespace nexus {

  using namespace CLHEP; 
  
  HeliumGasProperties::HeliumGasProperties(G4double pressure,
					 G4double /*temperature*/):
    pressure_(pressure)
  {
    //Density();
  }
  
  
  
  HeliumGasProperties::~HeliumGasProperties()
  {
  }
  
  G4double HeliumGasProperties::MassPerMole(G4int mass_num)
  {
    G4double mass_per_mole = 4.0026*g/mole;
    if ( mass_num == 3 )
      mass_per_mole = 3.016*g/mole;
    else if ( mass_num == 4)
      mass_per_mole = 4.0026*g/mole;
    else {
      G4cout << "Unknown Helium mass number: A = " << mass_num << G4endl;
      G4cout << "Reverting to Helium-4" << G4endl;
    }
    
    return mass_per_mole;
  }
  
  G4double HeliumGasProperties::Density(G4double pressure)
  {
    //These values are for a temperature of 300 K
    // taken from https://webbook.nist.gov/cgi/fluid.cgi?Action=Data&Wide=on&ID=C7440597&Type=IsoTherm&Digits=5&PLow=1&PHigh=40&PInc=0.1&T=300&RefState=DEF&TUnit=K&PUnit=bar&DUnit=kg%2Fm3&HUnit=kJ%2Fkg&WUnit=m%2Fs&VisUnit=uPa*s&STUnit=N%2Fm
    
    G4double density = 1.60279*kg/m3;
    
    if (pressure/bar >1 && pressure/bar <1.1)	
	density =0.1604*kg/m3;
    else if (pressure/bar >1.1 && pressure/bar <1.2)	
	density =0.17643*kg/m3;
    else if (pressure/bar >1.2 && pressure/bar <1.3)	
	density =0.19246*kg/m3;
    else if (pressure/bar >1.3 && pressure/bar <1.4)	
	density =0.20849*kg/m3;
    else if (pressure/bar >1.4 && pressure/bar <1.5)	
	density =0.22451*kg/m3;
    else if (pressure/bar >1.5 && pressure/bar <1.6)	
	density =0.24054*kg/m3;
    else if (pressure/bar >1.6 && pressure/bar <1.7)	
	density =0.25657*kg/m3;
    else if (pressure/bar >1.7 && pressure/bar <1.8)	
        density =0.27259*kg/m3;
    else if (pressure/bar >1.8 && pressure/bar <1.9)	
	density =0.28861*kg/m3;
    else if (pressure/bar >1.9 && pressure/bar <2)	
	density =0.30463*kg/m3;
    else if (pressure/bar >2 && pressure/bar <2.1)	
	density =0.32065*kg/m3;
    else if (pressure/bar >2.1 && pressure/bar <2.2)	
	density =0.33666*kg/m3;
    else if (pressure/bar >2.2 && pressure/bar <2.3)		
	density =0.35268*kg/m3;
    else if (pressure/bar >2.3 && pressure/bar <2.3)		
	density =0.36869*kg/m3;
    else if (pressure/bar >2.3 && pressure/bar <2.3)		
	density =0.36869*kg/m3;
    else if (pressure/bar >2.3 && pressure/bar <2.4)		
	density =0.36869*kg/m3;
    else if (pressure/bar >2.4 && pressure/bar <2.5)		
	density =0.38471*kg/m3;
    else if (pressure/bar >2.5 && pressure/bar <2.6)		
	density =0.40072*kg/m3;
    else if (pressure/bar >2.6 && pressure/bar <2.7)		
	density =0.41673*kg/m3;
    else if (pressure/bar >2.7 && pressure/bar <2.8)		
	density =0.43274*kg/m3;
    else if (pressure/bar >2.8 && pressure/bar <2.9)		
	density =0.44874*kg/m3;
    else if (pressure/bar >2.9 && pressure/bar <3)		
	density =0.46475*kg/m3;
    else if (pressure/bar >3 && pressure/bar <3.1)		
	density =0.48075*kg/m3;
    else if (pressure/bar >3.1 && pressure/bar <3.2)		
	density =0.49675*kg/m3;
    else if (pressure/bar >3.2 && pressure/bar <3.3)		
	density =0.51276*kg/m3;
    else if (pressure/bar >3.3 && pressure/bar <3.4)		
	density =0.52876*kg/m3;
    else if (pressure/bar >3.4 && pressure/bar <3.5)		
	density =0.54475*kg/m3;
    else if (pressure/bar >3.5 && pressure/bar <3.6)		
	density =0.56075*kg/m3;
    else if (pressure/bar >3.6 && pressure/bar <3.7)		
	density =0.57674*kg/m3;
    else if (pressure/bar >3.7 && pressure/bar <3.8)		
	density =0.59274*kg/m3;
    else if (pressure/bar >3.8 && pressure/bar <3.9)		
	density =0.60873*kg/m3;
    else if (pressure/bar >3.9 && pressure/bar <4)		
	density =0.62472*kg/m3;
    else if (pressure/bar >4 && pressure/bar <4.1)		
	density =0.64071*kg/m3;
    else if (pressure/bar >4.1 && pressure/bar <4.2)		
	density =0.6567*kg/m3;
    else if (pressure/bar >4.2 && pressure/bar <4.3)		
	density =0.67268*kg/m3;
    else if (pressure/bar >4.3 && pressure/bar <4.4)		
	density =0.68867*kg/m3;
    else if (pressure/bar >4.4 && pressure/bar <4.5)		
	density =0.70465*kg/m3;
    else if (pressure/bar >4.5 && pressure/bar <4.6)		
	density =0.72064*kg/m3;
    else if (pressure/bar >4.6 && pressure/bar <4.7)		
	density =0.73662*kg/m3;
    else if (pressure/bar >4.7 && pressure/bar <4.8)		
	density =0.75259*kg/m3;
    else if (pressure/bar >4.8 && pressure/bar <4.9)		
	density =0.76857*kg/m3;
    else if (pressure/bar >4.9 && pressure/bar <5)		
	density =0.78455*kg/m3;
    else if (pressure/bar >5 && pressure/bar <5.1)		
	density =0.80052*kg/m3;
    else if (pressure/bar >5.1 && pressure/bar <5.2)		
	density =0.8165*kg/m3;
    else if (pressure/bar >5.2 && pressure/bar <5.3)		
	density =0.83247*kg/m3;
    else if (pressure/bar >5.3 && pressure/bar <5.4)		
	density =0.84844*kg/m3;
    else if (pressure/bar >5.4 && pressure/bar <5.5)		
	density =0.86441*kg/m3;
    else if (pressure/bar >5.5 && pressure/bar <5.6)		
	density =0.88037*kg/m3;
    else if (pressure/bar >5.6 && pressure/bar <5.7)		
	density =0.89634*kg/m3;
    else if (pressure/bar >5.7 && pressure/bar <5.8)		
	density =0.91231*kg/m3;
    else if (pressure/bar >5.8 && pressure/bar <5.9)		
	density =0.92827*kg/m3;
    else if (pressure/bar >5.9 && pressure/bar <6)		
	density =0.94423*kg/m3;
    else if (pressure/bar >6 && pressure/bar <6.1)		
	density =0.96019*kg/m3;
    else if (pressure/bar >6.1 && pressure/bar <6.2)		
	density =0.97615*kg/m3;
    else if (pressure/bar >6.2 && pressure/bar <6.3)		
	density =0.99211*kg/m3;
    else if (pressure/bar >6.3 && pressure/bar <6.4)		
	density =1.0081*kg/m3;
    else if (pressure/bar >6.4 && pressure/bar <6.5)		
	density =1.024*kg/m3;
    else if (pressure/bar >6.5 && pressure/bar <6.6)		
	density =1.04*kg/m3;
    else if (pressure/bar >6.6 && pressure/bar <6.7)		
	density =1.0559*kg/m3;
    else if (pressure/bar >6.7 && pressure/bar <6.8)		
	density =1.0719*kg/m3;
    else if (pressure/bar >6.8 && pressure/bar <6.9)		
	density =1.0878*kg/m3;
    else if (pressure/bar >6.9 && pressure/bar <7)		
	density =1.1038*kg/m3;
    else if (pressure/bar >7 && pressure/bar <7.1)		
	density =1.1197*kg/m3;
    else if (pressure/bar >7.1 && pressure/bar <7.2)		
	density =1.1357*kg/m3;
    else if (pressure/bar >7.2 && pressure/bar <7.3)		
	density =1.1516*kg/m3;
    else if (pressure/bar >7.3 && pressure/bar <7.4)		
	density =1.1675*kg/m3;
    else if (pressure/bar >7.4 && pressure/bar <7.5)		
	density =1.1835*kg/m3;
    else if (pressure/bar >7.5 && pressure/bar <7.6)		
	density =1.1994*kg/m3;
    else if (pressure/bar >7.6 && pressure/bar <7.7)		
	density =1.2154*kg/m3;
    else if (pressure/bar >7.7 && pressure/bar <7.8)		
	density =1.2313*kg/m3;
    else if (pressure/bar >7.8 && pressure/bar <7.9)		
	density =1.2472*kg/m3;
    else if (pressure/bar >7.9 && pressure/bar <8)		
	density =1.2632*kg/m3;
    else if (pressure/bar >8 && pressure/bar <8.1)		
	density =1.2791*kg/m3;
    else if (pressure/bar >8.1 && pressure/bar <8.2)		
	density =1.295*kg/m3;
    else if (pressure/bar >8.2 && pressure/bar <8.3)		
	density =1.3109*kg/m3;
    else if (pressure/bar >8.3 && pressure/bar <8.4)		
	density =1.3269*kg/m3;
    else if (pressure/bar >8.4 && pressure/bar <8.5)		
	density =1.3428*kg/m3;
    else if (pressure/bar >8.5 && pressure/bar <8.6)		
	density =1.3587*kg/m3;
    else if (pressure/bar >8.6 && pressure/bar <8.7)		
	density =1.3746*kg/m3;
    else if (pressure/bar >8.7 && pressure/bar <8.8)		
	density =1.3906*kg/m3;
    else if (pressure/bar >8.8 && pressure/bar <8.9)		
	density =1.4065*kg/m3;
    else if (pressure/bar >8.9 && pressure/bar <9)		
	density =1.4224*kg/m3;
    else if (pressure/bar >9 && pressure/bar <9.1)		
	density =1.4383*kg/m3;
    else if (pressure/bar >9.1 && pressure/bar <9.2)		
	density =1.4542*kg/m3;
    else if (pressure/bar >9.2 && pressure/bar <9.3)		
	density =1.4701*kg/m3;
    else if (pressure/bar >9.3 && pressure/bar <9.4)		
	density =1.4861*kg/m3;
    else if (pressure/bar >9.4 && pressure/bar <9.5)		
	density =1.502*kg/m3;
    else if (pressure/bar >9.5 && pressure/bar <9.6)		
	density =1.5179*kg/m3;
    else if (pressure/bar >9.6 && pressure/bar <9.7)		
	density =1.5338*kg/m3;
    else if (pressure/bar >9.7 && pressure/bar <9.8)		
	density =1.5497*kg/m3;
    else if (pressure/bar >9.8 && pressure/bar <9.9)		
	density =1.5656*kg/m3;
    else if (pressure/bar >9.9 && pressure/bar <10)		
	density =1.5815*kg/m3;
    else if (pressure/bar >10 && pressure/bar <10.1)		
	density =1.5974*kg/m3;
    else if (pressure/bar >10.1 && pressure/bar <10.2)
	density =1.6133*kg/m3;
    else if (pressure/bar >10.2 && pressure/bar <10.3)
	density =1.6292*kg/m3;
    else if (pressure/bar >10.3 && pressure/bar <10.4)
	density =1.6451*kg/m3;
    else if (pressure/bar >10.4 && pressure/bar <10.5)
	density =1.661*kg/m3;
    else if (pressure/bar >10.5 && pressure/bar <10.6)
	density =1.6769*kg/m3;
    else if (pressure/bar >10.6 && pressure/bar <10.7)
	density =1.6928*kg/m3;
    else if (pressure/bar >10.7 && pressure/bar <10.8)
	density =1.7087*kg/m3;
    else if (pressure/bar >10.8 && pressure/bar <10.9)
	density =1.7246*kg/m3;
    else if (pressure/bar >10.9 && pressure/bar <11)		
	density =1.7405*kg/m3;
    else if (pressure/bar >11 && pressure/bar <11.1)		
	density =1.7563*kg/m3;
    else if (pressure/bar >11.1 && pressure/bar <11.2)
	density =1.7722*kg/m3;
    else if (pressure/bar >11.2 && pressure/bar <11.3)
	density =1.7881*kg/m3;
    else if (pressure/bar >11.3 && pressure/bar <11.4)
	density =1.804*kg/m3;
    else if (pressure/bar >11.4 && pressure/bar <11.5)
	density =1.8199*kg/m3;
    else if (pressure/bar >11.5 && pressure/bar <11.6)
	density =1.8358*kg/m3;
    else if (pressure/bar >11.6 && pressure/bar <11.7)
	density =1.8516*kg/m3;
    else if (pressure/bar >11.7 && pressure/bar <11.8)
	density =1.8675*kg/m3;
    else if (pressure/bar >11.8 && pressure/bar <11.9)
	density =1.8834*kg/m3;
    else if (pressure/bar >11.9 && pressure/bar <12)		
	density =1.8993*kg/m3;
    else if (pressure/bar >12 && pressure/bar <12.1)		
	density =1.9151*kg/m3;
    else if (pressure/bar >12.1 && pressure/bar <12.2)
	density =1.931*kg/m3;
    else if (pressure/bar >12.2 && pressure/bar <12.3)
	density =1.9469*kg/m3;
    else if (pressure/bar >12.3 && pressure/bar <12.4)
	density =1.9627*kg/m3;
    else if (pressure/bar >12.4 && pressure/bar <12.5)
	density =1.9786*kg/m3;
    else if (pressure/bar >12.5 && pressure/bar <12.6)
	density =1.9945*kg/m3;
    else if (pressure/bar >12.6 && pressure/bar <12.7)
	density =2.0103*kg/m3;
    else if (pressure/bar >12.7 && pressure/bar <12.8)
	density =2.0262*kg/m3;
    else if (pressure/bar >12.8 && pressure/bar <12.9)
	density =2.0421*kg/m3;
    else if (pressure/bar >12.9 && pressure/bar <13)	
	density =2.0579*kg/m3;
    else if (pressure/bar >13 && pressure/bar <13.1)	
	density =2.0738*kg/m3;
    else if (pressure/bar >13.1 && pressure/bar <13.2)
	density =2.0896*kg/m3;
    else if (pressure/bar >13.2 && pressure/bar <13.3)
	density =2.1055*kg/m3;
    else if (pressure/bar >13.3 && pressure/bar <13.4)
	density =2.1214*kg/m3;
    else if (pressure/bar >13.4 && pressure/bar <13.5)
	density =2.1372*kg/m3;
    else if (pressure/bar >13.5 && pressure/bar <13.6)
	density =2.1531*kg/m3;
    else if (pressure/bar >13.6 && pressure/bar <13.7)
	density =2.1689*kg/m3;
    else if (pressure/bar >13.7 && pressure/bar <13.8)
	density =2.1848*kg/m3;
    else if (pressure/bar >13.8 && pressure/bar <13.9)
	density =2.2006*kg/m3;
    else if (pressure/bar >13.9 && pressure/bar <14)
	density =2.2165*kg/m3;
    else if (pressure/bar >14 && pressure/bar <14.1)
	density =2.2323*kg/m3;
    else if (pressure/bar >14.1 && pressure/bar <14.2)
	density =2.2481*kg/m3;
    else if (pressure/bar >14.2 && pressure/bar <14.3)
	density =2.264*kg/m3;
    else if (pressure/bar >14.3 && pressure/bar <14.4)
	density =2.2798*kg/m3;
    else if (pressure/bar >14.4 && pressure/bar <14.5)
	density =2.2957*kg/m3;
    else if (pressure/bar >14.5 && pressure/bar <14.6)
	density =2.3115*kg/m3;
    else if (pressure/bar >14.6 && pressure/bar <14.7)
	density =2.3273*kg/m3;
    else if (pressure/bar >14.7 && pressure/bar <14.8)
	density =2.3432*kg/m3;
    else if (pressure/bar >14.8 && pressure/bar <14.9)
	density =2.359*kg/m3;
    else if (pressure/bar >14.9 && pressure/bar <15)
	density =2.3748*kg/m3;
    else if (pressure/bar >15 && pressure/bar <15.1)
	density =2.3907*kg/m3;
    else if (pressure/bar >15.1 && pressure/bar <15.2)
	density =2.4065*kg/m3;
    else if (pressure/bar >15.2 && pressure/bar <15.3)
	density =2.4223*kg/m3;
    else if (pressure/bar >15.3 && pressure/bar <15.4)
	density =2.4381*kg/m3;
    else if (pressure/bar >15.4 && pressure/bar <15.5)
	density =2.454*kg/m3;
    else if (pressure/bar >15.5 && pressure/bar <15.6)
	density =2.4698*kg/m3;
    else if (pressure/bar >15.6 && pressure/bar <15.7)
	density =2.4856*kg/m3;
    else if (pressure/bar >15.7 && pressure/bar <15.8)
	density =2.5014*kg/m3;
    else if (pressure/bar >15.8 && pressure/bar <15.9)
	density =2.5172*kg/m3;
    else if (pressure/bar >15.9 && pressure/bar <16)
	density =2.5331*kg/m3;
    else
	G4Exception("[HeliumGasProperties]","Density()",FatalException,
		"Unknown helium desity for theis pressure!");
return density;

  }  
} // end namespace nexus
