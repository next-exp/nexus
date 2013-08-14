// ----------------------------------------------------------------------------
///  \file   IonizationCluster.h
///  \brief  Definition of the ionization cluster.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     19 May 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __IONIZATION_CLUSTER__
#define __IONIZATION_CLUSTER__

#include <G4ParticleDefinition.hh>


namespace nexus {

  /// Definition of the ionization cluster as a Geant4 particle.

  class IonizationCluster: public G4ParticleDefinition
  {
  public:
    /// Returns 
    static IonizationCluster* Definition();

    /// Destructor
    ~IonizationCluster();
    
  private:
    /// Constructor is hidden. No instance of this class can be created.
    IonizationCluster();

  private:
    static IonizationCluster* _instance;
  };

  inline IonizationCluster::IonizationCluster() {}
  inline IonizationCluster::~IonizationCluster() {}

} // end namespace nexus

#endif
