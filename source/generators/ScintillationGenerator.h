// ----------------------------------------------------------------------------
///  \file   ScintillationGenerator.h
///  \brief  Primary generator for scintillation in HPXe
///  
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     27 Mar 2009
///  \version  $Id: ScintillationGenerator.h 9216 2013-09-05 12:57:42Z paola $
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __SCINTILLATIONGENERATOR__
#define __SCINTILLATIONGENERATOR__

#include <G4VPrimaryGenerator.hh>
#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

class G4GenericMessenger;
class G4Event;
class G4PhysicsOrderedFreeVector;

namespace nexus {

  class BaseGeometry;


  /// Primary generator (concrete class of G4VPrimaryGenerator) for events 
  /// consisting of a single particle. The user must specify via configuration
  /// parameters the particle type, a kinetic energy interval (a random
  /// value with random .
  
  class ScintillationGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    ScintillationGenerator();
    /// Destructor
    ~ScintillationGenerator();
    
    /// This method is invoked at the beginning of the event. It sets 
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);
    
  private:

    void ComputeCumulativeDistribution(const G4PhysicsOrderedFreeVector&,
                                       G4PhysicsOrderedFreeVector&);

    G4GenericMessenger* msg_;
    G4Navigator* geom_navigator_; ///< Geometry Navigator
    const BaseGeometry* geom_; ///< Pointer to the detector geometry

    G4String region_;
    G4int    nphotons_;
   

  };

} // end namespace nexus

#endif // __SCINTILLATIONGENERATOR__
