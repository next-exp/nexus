#ifndef nemo_field_H
#define nemo_field_H 1

#include "globals.hh"
#include "G4MagneticField.hh"


class nemo_field : public G4MagneticField
{

  private:
    G4double B_;

  public:
    nemo_field(G4double B);
    ~nemo_field();

    void GetFieldValue( const  double Point[3],
		        double *Bfield ) const;

};

#endif

