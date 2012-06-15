// ----------------------------------------------------------------------------
///  \file   IonizationElectron.h
///  \brief  Definition of the ionization electron.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     19 May 2010
///  \version  $Id$
///
///  Copyright (c) 2010, 2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __IONIZATION_ELECTRON__
#define __IONIZATION_ELECTRON__

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
    static IonizationElectron* _instance;
  };

  inline IonizationElectron::IonizationElectron() {}
  inline IonizationElectron::~IonizationElectron() {}

} // end namespace nexus

#endif
