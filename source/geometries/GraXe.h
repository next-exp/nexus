// ----------------------------------------------------------------------------
//  $Id: GraXe.h 2011-04-08 jmunoz $
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __GRAXE__
#define ____

#include "BaseGeometry.h"

//class G4LogicalVolume;
class G4Material;


namespace nexus {
  
  class SpherePointSampler;
  
  
  /// Geometry of a sphere filled with Xe
  
  class GraXe: public BaseGeometry
  {
  public:
    /// Constructor
    GraXe();
    /// Destructor
    ~GraXe();

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    void ReadParameters();
    void BuildGeometry();


  private:
    // Dimensions
    G4double _snitch_radius;
    G4double _in_active_radius;
    G4double _graph_ball_thickness;
    G4double _out_active_radius;
    G4double _cu_ball_thickness;

    // Materials
    G4Material* _graph_ball_mat;
    G4Material* _out_active_mat;
    G4Material* _cu_ball_mat;

    // Vertex Generator
    SpherePointSampler* _cu_ball_vertex_gen;
    SpherePointSampler* _graph_ball_vertex_gen;
    SpherePointSampler* _snitch_vertex_gen;
    SpherePointSampler* _in_active_vertex_gen;

  };


} // end namespace nexus

#endif
