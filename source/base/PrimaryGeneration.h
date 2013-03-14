// ----------------------------------------------------------------------------
///  \file   PrimaryGeneration.h
///  \brief  User initialization class for event generation.
/// 
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     9 Mar 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __PRIMARY_GENERATION__
#define __PRIMARY_GENERATION__

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4String.hh>

class G4VPrimaryGenerator;
class G4GenericMessenger;


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
//    void CreateGenerator(G4String);

  private:
    G4GenericMessenger* _msg;
    G4VPrimaryGenerator* _generator; ///< Pointer to the primary generator
  };

  // INLINE DEFINITIONS //////////////////////////////////////////////

  inline void PrimaryGeneration::SetGenerator(G4VPrimaryGenerator* pg)
  { _generator = pg; }

  inline const G4VPrimaryGenerator* PrimaryGeneration::GetGenerator()
  { return _generator; }
  
} // end namespace nexus

#endif
