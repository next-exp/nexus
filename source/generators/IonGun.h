// ----------------------------------------------------------------------------
///  \file   IonGun.h
///  \brief  
///  
///  \author   Miquel Nebot Guinot <miquel.nebot@ific.uv.es>       
///  \date     01 Aug 2013
///  \version  $Id: IonGun.h $
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __IonGun__
#define __IonGun__

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;

namespace nexus{
  class BaseGeometry;

  
  /// Primary generator (concrete class of G4VPrimaryGenerator) for events 
  /// consisting of a decay of an  Ion. The user must specify via configuration
  /// parameters the Isotope, the atomic number and the mass number
 
   class IonGun: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    IonGun();
    /// Destructor
    ~IonGun();
    
    /// This method is invoked at the beginning of the event. It sets 
    /// a primary vertex in the event.
    void GeneratePrimaryVertex(G4Event*);
 
  private:
    void SetParticleDefinition(G4String);

 private:
    G4GenericMessenger* _msg;
    G4ParticleDefinition* _particle_definition;
    G4String _region;
    G4double _z; //atomic number
    G4double _a; //mass number

    const BaseGeometry* _geom; ///< Pointer to the detector geometry
  };
  
} // end namespace nexus

#endif
