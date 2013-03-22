// ----------------------------------------------------------------------------
///  \file   PrimaryGeneration.h
///  \brief  User initialization class for event generation.
/// 
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     9 Mar 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __PRIMARY_GENERATION__
#define __PRIMARY_GENERATION__

#include <G4VUserPrimaryGeneratorAction.hh>

class G4VPrimaryGenerator;


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

    /// Sets the primary generator
    void SetGenerator(G4VPrimaryGenerator*);
    /// Returns a pointer to the primary generator
    const G4VPrimaryGenerator* GetGenerator() const;

  private:
    G4VPrimaryGenerator* _generator; ///< Pointer to the primary generator
  };

  // INLINE DEFINITIONS //////////////////////////////////////////////

  inline void PrimaryGeneration::SetGenerator(G4VPrimaryGenerator* pg)
  { _generator = pg; }

  inline const G4VPrimaryGenerator* PrimaryGeneration::GetGenerator() const
  { return _generator; }
  
} // end namespace nexus

#endif
