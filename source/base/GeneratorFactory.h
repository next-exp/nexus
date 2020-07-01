// ----------------------------------------------------------------------------
// nexus | GeneratorFactory.h
//
// This class instantiates the generator of the simulation that the user
// specifies via configuration parameter.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef GENERATOR_FACTORY_H
#define GENERATOR_FACTORY_H

#include <G4String.hh>

class G4VPrimaryGenerator;
class G4GenericMessenger;


namespace nexus {

  class GeneratorFactory
  {
  public:
    /// Constructor. Defines the messenger commands.
    GeneratorFactory();
    /// Destructor
    ~GeneratorFactory();
    /// Returns an instance of the chosen generator
    G4VPrimaryGenerator* CreateGenerator();

  private:
    G4GenericMessenger* msg_; ///< Pointer to the messenger
    G4String name_; ///< Name of the chosen primary generator
    G4VPrimaryGenerator* p_;
  };

} // end namespace nexus

#endif
