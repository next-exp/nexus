// ----------------------------------------------------------------------------
// nexus | Back2backGammas.h
//
// This generator simulates two gammas of 510.999 keV back to back, with
// random direction. It is possible to specify a limited solid angle.
// Control histograms of the solid angle where gammas are generated
// are produced.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef BACK2BACK_H
#define BACK2BACK_H

#include <G4VPrimaryGenerator.hh>

class G4Event;
class G4GenericMessenger;

class TFile;
class TH1F;

namespace nexus {

  class BaseGeometry;
  
  class Back2backGammas: public G4VPrimaryGenerator
  {
  public:
    //Constructor
    Back2backGammas();
    //Destructor
    ~Back2backGammas();

    void GeneratePrimaryVertex(G4Event* evt);

  private:

    G4GenericMessenger* msg_;
    const BaseGeometry* geom_;

    G4String region_;

    G4double costheta_min_;
    G4double costheta_max_;
    G4double phi_min_;
    G4double phi_max_;

    TH1F*  theta_angle_;
    TH1F*  phi_angle_;
    TFile* out_file_;
  };

}// end namespace nexus
#endif
