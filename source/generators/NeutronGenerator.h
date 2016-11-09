// ----------------------------------------------------------------------------                                                                                                      
///  \file     NeutronGenerator.h                                                                                                                                                    
///  \brief    Primary generator (concrete class of G4VPrimaryGenerator) for neutron                                                                                                 
///            events from measured flux. The user must specify via configuration parameters                                                                                         
///                                                                                                                                                                                  
///  \author   Miquel Nebot Guinot <miquel.nebot@ific.uv.es>                                                                                                                         
///  \date     Aug 2016                                                                                                                                                              
///  \version                                                                                                                                                                        
///                                                                                                                                                                                  
///  Copyright (c) 2016 NEXT Collaboration. All rights reserved.                                                                                                                     
// ----------------------------------------------------------------------------                                                                                                      

#ifndef __NeutronGenerator__
#define __NeutronGenerator__

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;


namespace nexus{

  class BaseGeometry;

  class NeutronGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor                                                                                                                                                                  
    NeutronGenerator();
    /// Destructor                                                                                                                                                                   
    ~NeutronGenerator();

    /// This method is invoked at the beginning of the event. It sets                                                                                                                
    /// a primary vertex in the event.                                                                                                                                               
    void GeneratePrimaryVertex(G4Event*);

  private:
    void SetParticleDefinition();

    G4double RandomEnergy() const;

  private:
    G4GenericMessenger* _msg;
    G4ParticleDefinition* _particle_definition;
    G4String _region;
    G4double _energy_min; ///< Minimum kinetic energy                                                                                                                                
    G4double _energy_max; ///< Maximum kinetic energy                                                                                                                                


    const BaseGeometry* _geom; ///< Pointer to the detector geometry                                                                                                                 
  };

} // end namespace nexus                                                                                                                                                             

#endif
