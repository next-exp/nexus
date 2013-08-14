/// Implementation of B field for nemo detector
///

#include <next/nemo_field.hh>

#include "G4ThreeVector.hh"
#include "G4UnitsTable.hh"


nemo_field::nemo_field(G4double B)
{
  B_ = B;
}


nemo_field::~nemo_field()
{
    // G4cout << "nemo mag. field destructor" << G4endl;
}


void nemo_field::GetFieldValue(const double Point[3], double *Bfield) const
{

  Bfield[0] = 0.;
  Bfield[1] = B_;
  Bfield[2] = 0.;

//   G4cout << G4endl << "Evaluating B in position: (" << Point[0] << " , " << Point[1] << " , " << Point[2] << ") " 
// 	 << "  Bx = "   << G4BestUnit(Bfield[0],"Magnetic flux density")
// 	 << "  By =  "  << G4BestUnit(Bfield[1],"Magnetic flux density") 
// 	 << "  Bz =  "  << G4BestUnit(Bfield[2],"Magnetic flux density") 
// 	 << G4endl;
}
