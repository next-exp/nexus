// ----------------------------------------------------------------------------
///  file   LXeScintillationGenerator.h
///  brief  Primary generator for scintillation in LXe
///
///  author  Paola Ferrario <paola.ferrario@dipc.org>
///  date    2 Apr 2019
///  Copyright (c) 2019 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __LXESCINTILLATIONGENERATOR__
#define __LXESCINTILLATIONGENERATOR__

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;
class G4PhysicsOrderedFreeVector;

namespace nexus {

  class BaseGeometry;


  /// Primary generator (concrete class of G4VPrimaryGenerator) for events
  /// consisting of a single particle. The user must specify via configuration
  /// parameters the particle type, a kinetic energy interval (a random
  /// value with random .

  class LXeScintillationGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    LXeScintillationGenerator();
    /// Destructor
    ~LXeScintillationGenerator();

    /// This method is invoked at the beginning of the event. It sets
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);

  private:

    void ComputeCumulativeDistribution(const G4PhysicsOrderedFreeVector&,
                                       G4PhysicsOrderedFreeVector&);

    G4GenericMessenger* msg_;
    const BaseGeometry* geom_; ///< Pointer to the detector geometry

    G4String region_;
    G4int    nphotons_;

  };

} // end namespace nexus

#endif // __LXESCINTILLATIONGENERATOR__
