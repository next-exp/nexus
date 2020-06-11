// ----------------------------------------------------------------------------
///
///  
///  Author : P Ferrario <paola.ferrario@dipc.org>    
///  Created: 25 February 2020
///
// ----------------------------------------------------------------------------

#ifndef SENSITIVITY_MAP_GENERATOR_H
#define SENSITIVITY_MAP_GENERATOR_H

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;


namespace nexus {

  class BaseGeometry;

  
  class SensMap: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    SensMap();
    /// Destructor
    ~SensMap();
    
    /// This method is invoked at the beginning of the event. It sets 
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);

  private:
    G4GenericMessenger* msg_; ///< Pointer to UI messenger
    const BaseGeometry* geom_; ///< Pointer to the detector geometry

    G4int num_gammas_;
  };

} // end namespace nexus

#endif
