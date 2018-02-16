#ifndef PETITMODULE_H
#define PETITMODULE_H

#include "BaseGeometry.h"
#include "BoxPointSampler.h"

class G4LogicalVolume;
class G4GenericMessenger;
class G4Material;
namespace nexus {class PetitKDB;}
namespace nexus {class PetitPlainDice;}
namespace nexus {class BoxPointSampler;}
namespace nexus {class SiPMpet9mm2;}

namespace nexus {

  class PetitModule: public BaseGeometry {
  public:
    /// Constructor
    PetitModule();

    /// Destructor
    ~PetitModule();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  public:
    void Construct();

    // Functions for getting the coordinates of the cylinder
    G4double GetDiameter();
    G4double GetLength();

  private:

    //  void BuildDetector();
    void BuildLXe();
    void BuildActive();
    void BuildSiPMPlane();

    G4LogicalVolume* det_logic_;
    G4LogicalVolume* lXe_logic_;
    G4LogicalVolume* active_logic_;

    // Detector dimensions
    G4double det_in_diam_, det_length_;
    const G4double det_thickness_; // Thickness of the walls of the detector
    G4double max_step_size_; /// Maximum step size of electrons in Xe

    // Vertex position of generator
    G4double specific_vertex_X_;
    G4double specific_vertex_Y_;
    G4double specific_vertex_Z_;

    // ACTIVE gas Xenon
    G4Material* lXe_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;
    SiPMpet9mm2* sipm_;

    // Dice class
    //PetitKDB* db_;
    PetitPlainDice* pdb_;

    /// Thickness and xy dimension of plain KDB with no SiPMs
    G4double pdb_thickness_, pdb_lateral_dim_;

    // Dimensions of the active volume
    G4double active_z_dim_, active_xy_dim_;

    /// Distance of the centre of a kapton dice from the
    /// end of the dice perpendicolar to it.
    G4double dice_offset_;

    BoxPointSampler* active_gen_;

    // Number of instrumented faces
    G4bool two_instrumented_planes_;

    // Visibility of the Kapton Dice Boards
    G4bool visibility_dices_;

    void SetDiameter(G4double d);
    G4double diameter_; // diameter of the cylinder

    void SetLength(G4double l);
    G4double length_; // length of the cylinder

  };

  inline void PetitModule::SetDiameter(G4double d) {  diameter_ = d; }

  inline  G4double PetitModule::GetDiameter()  { return diameter_; }

  inline void PetitModule::SetLength(G4double l) {  length_ = l; }

  inline  G4double PetitModule::GetLength()  { return length_; }


} // end namespace nexus

#endif
