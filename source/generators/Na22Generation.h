// ----------------------------------------------------------------------------
// nexus | Na22Generator.h
//
// This class is the primary generator of Na22-like events consisting of
// two particles in coincidence (back-to-back) coming from e+e- annihilation
// and one disexcitation gamma.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NA22GENERATION__
#define NA22GENERATION__

#include <G4VPrimaryGenerator.hh>

class G4Event;
class G4GenericMessenger;

namespace nexus {

  class BaseGeometry;

  class Na22Generation: public G4VPrimaryGenerator
  {
  public:
    //Constructor
    Na22Generation();
    //Destructor
    ~Na22Generation();

    void GeneratePrimaryVertex(G4Event* evt);

  private:

    G4GenericMessenger* msg_;
    const BaseGeometry* geom_;

    G4String region_;

  };

}// end namespace nexus
#endif
