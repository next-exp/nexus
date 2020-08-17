// ----------------------------------------------------------------------------
// nexus | ECECGenerator.h
//
// This class is the primary generator for events consisting in the decay
// of a double electron capture. The user must specify via configuration
// parameters the atomic number, mass number and energy level of the
// daughter isotope of interest.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef ECEC_GENERATOR_H
#define ECEC_GENERATOR_H

#include <G4VPrimaryGenerator.hh>
#include <G4AtomicShellEnumerator.hh>

class BaseGeometry;
class G4Event;
class G4GenericMessenger;
class G4AtomicShell;
class G4PrimaryParticle;
class G4DynamicParticle;
class G4UAtomicDeexcitation;


namespace nexus{

  class BaseGeometry;

  class ECECGenerator: public G4VPrimaryGenerator
  {
  public:
    // Constructor
    ECECGenerator();
    // Destructor
    ~ECECGenerator();

    // This method is invoked at the beginning of the event,
    // setting a primary vertex that contains the chosen ion
    void GeneratePrimaryVertex(G4Event*);

 private:
   void                    Initialize();
   G4AtomicShellEnumerator GetShellID(G4String);
   G4PrimaryParticle*      GetPrimaryParticle(G4DynamicParticle*);

 private:
    G4int    atomic_number_;
    G4String shell_name_;
    G4String region_;
    G4GenericMessenger* msg_;

    const BaseGeometry* geom_;
    const G4AtomicShell* shell_;
    G4UAtomicDeexcitation* atom_;
  };

} // end namespace nexus

#endif
