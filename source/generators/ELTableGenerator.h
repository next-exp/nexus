// ----------------------------------------------------------------------------
// nexus | ELTableGenerator.h
//
// This generator simulates ionization electrons to produce look-up tables.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef EL_TABLE_GENERATOR_H
#define EL_TABLE_GENERATOR_H

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;


namespace nexus {

  class BaseGeometry;


  class ELTableGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    ELTableGenerator();
    /// Destructor
    ~ELTableGenerator();

    /// This method is invoked at the beginning of the event. It sets
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);

  private:
    G4GenericMessenger* msg_; ///< Pointer to UI messenger
    const BaseGeometry* geom_; ///< Pointer to the detector geometry

    G4int num_ie_;
  };

} // end namespace nexus

#endif
