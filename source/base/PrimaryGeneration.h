// ----------------------------------------------------------------------------
///  \file   PrimaryGeneration.h
///  \brief  User initialization class for event generation.
/// 
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     9 Mar 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __PRIMARY_GENERATION__
#define __PRIMARY_GENERATION__

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4VPrimaryGenerator.hh>


namespace nexus {

  /// Geant4 (mandatory) initialization class for event generation.
  
  class PrimaryGeneration: public G4VUserPrimaryGeneratorAction
  {
  public:
    /// Constructor
    PrimaryGeneration();
    /// Destructor
    ~PrimaryGeneration();

    /// 
    void GeneratePrimaries(G4Event*);

    /// Sets the primary generator class object
    void SetGenerator(G4VPrimaryGenerator*);
    /// Returns a pointer to the primary generator
    const G4VPrimaryGenerator* GetGenerator();

  private:
    G4VPrimaryGenerator* _generator; ///< Pointer to the primary generator
  };


  // inline methods ..................................................

  inline PrimaryGeneration::PrimaryGeneration(): _generator(0) {}

  inline PrimaryGeneration::~PrimaryGeneration() { delete _generator; }

  inline void PrimaryGeneration::SetGenerator(G4VPrimaryGenerator* pg)
  { _generator = pg; }

  inline const G4VPrimaryGenerator* PrimaryGeneration::GetGenerator()
  { return _generator; }

  
} // end namespace nexus

#endif
