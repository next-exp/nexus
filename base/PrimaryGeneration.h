//
//  PrimaryGeneration.h
//
//     Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//     Created:  9 Mar 2009
//     Updated: 27 Mar 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#ifndef __PRIMARY_GENERATION__
#define __PRIMARY_GENERATION__

#include <G4VUserPrimaryGeneratorAction.hh>

class G4VPrimaryGenerator;


namespace nexus {

  class PrimaryGeneration: public G4VUserPrimaryGeneratorAction
  {
  public:
    /// Constructor
    PrimaryGeneration();
    /// Destructor
    ~PrimaryGeneration();

    ///
    void GeneratePrimaries(G4Event*);

    ///
    void SetGenerator(G4VPrimaryGenerator*);

  private:
    ///
    void Initialize();
    
  private:
    G4VPrimaryGenerator* _generator;
  };

} // namespace nexus

#endif
