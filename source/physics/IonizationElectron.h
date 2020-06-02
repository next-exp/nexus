// ----------------------------------------------------------------------------
// nexus | IonizationElectron.h
//
// Definition of the ionization electron.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef IONIZATION_ELECTRON_H
#define IONIZATION_ELECTRON_H

#include <G4ParticleDefinition.hh>


namespace nexus {

  /// Definition of the ionization electron

  class IonizationElectron: public G4ParticleDefinition
  {
  public:
    /// Returns a pointer to the only instance
    static IonizationElectron* Definition();

    /// Destructor
    ~IonizationElectron();

  private:
    /// Default constructor is hidden.
    /// No instance of this class can be created.
    IonizationElectron();

  private:
    static IonizationElectron* instance_;
  };

} // end namespace nexus

#endif
