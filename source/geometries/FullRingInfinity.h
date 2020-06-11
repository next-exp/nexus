// ----------------------------------------------------------------------------
// nexus | FullRingInfinity.h
//
// This class implements the geometry of a cylindric ring of LXe,
// of configurable length and diameter.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef FULL_RING_INF_H
#define FULL_RING_INF_H

#include "BaseGeometry.h"
#include <vector>

class G4GenericMessenger;
class G4LogicalVolume;
namespace nexus {
  class SiPMpetFBK;
  class SpherePointSampler;
}

namespace nexus {
  class FullRingInfinity : public BaseGeometry {

  public:
    // Constructor
    FullRingInfinity();
    //Destructor
    ~FullRingInfinity();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    private:
    void Construct();
    void BuildCryostat();
    void BuildQuadSensors();
    void BuildSensors();
    void BuildPhantom();
    void BuildPointfile(G4String pointFile);
    G4int binarySearchPt(G4int low, G4int high, G4double rnd) const;
    G4ThreeVector RandomPointVertex() const;
    void CalculateSensitivityVertices(G4double binning);

    SiPMpetFBK* sipm_;

    G4LogicalVolume* lab_logic_;
    G4LogicalVolume* LXe_logic_;
    G4LogicalVolume* active_logic_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    G4double axial_length_;
    G4double sipm_pitch_;
    G4int n_sipm_rows_;
    G4int instr_faces_; ///< number of instrumented faces
    G4double kapton_thickn_;
    G4double depth_;

    G4double inner_radius_, external_radius_;
    G4double cryo_width_, cryo_thickn_;

    G4double phantom_diam_;
    G4double phantom_length_;

    G4double max_step_size_;

    SpherePointSampler* spheric_gen_;

    G4double specific_vertex_X_;
    G4double specific_vertex_Y_;
    G4double specific_vertex_Z_;

    G4bool phantom_;

    // Variables for the point generator.
    G4int pt_Nx_, pt_Ny_, pt_Nz_;
    G4float pt_Lx_, pt_Ly_, pt_Lz_;
    G4float *pt_;

    G4bool sensitivity_;
    G4int events_per_point_;
    G4int sensitivity_point_id_;
    mutable G4int sensitivity_index_;
    mutable std::vector<G4ThreeVector> sensitivity_vertices_;
    G4double sensitivity_binning_;
    G4double sens_x_min_, sens_x_max_;
    G4double sens_y_min_, sens_y_max_;
    G4double sens_z_min_, sens_z_max_;



  };
}
#endif
