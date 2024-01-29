// ----------------------------------------------------------------------------
// nexus | Next1EL.h
//
// Geometry of the NEXT-DEMO detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT1_EL_H
#define NEXT1_EL_H

#include "GeometryBase.h"
#include <G4RotationMatrix.hh>
#include <vector>

class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;
class G4UIcmdWith3VectorAndUnit;


namespace nexus {

  class HexagonPointSampler;
  class CylinderPointSamplerLegacy;
  class BoxPointSamplerLegacy;

  /// Geometry of the NEXT1-EL detector

  class Next1EL: public GeometryBase
  {
  public:
    /// Constructor
    Next1EL();
    /// Destructor
    ~Next1EL();

    /// Returns a vertex in a region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;
    void CalculateELTableVertices(G4double radius, G4double binning, G4double z);

  private:
    void Construct();
    void DefineMaterials();
    void BuildLab();
    void BuildMuons();
    void BuildExtScintillator();
    void BuildVessel();
    void BuildFieldCage();
    void BuildEnergyPlane();
    void BuildSiPMTrackingPlane();
    void BuildPMTTrackingPlane();
    void PrintAbsoluteSiPMPos();

  private:

    HexagonPointSampler* hexrnd_;
    BoxPointSamplerLegacy* muons_sampling_;

    // Pointers to materials definition
    G4Material* air_;       ///< Air
    G4Material* gxe_;       ///< Gaseous xenon
    G4Material* teflon_;    ///< PTFE (Teflon)
    G4Material* steel_;     ///< Stainless Steel
    G4Material* aluminum_;  ///< Aluminum
    G4Material* lead_;  ///< Lead
    G4Material* plastic_;  ///< plastic
    G4Material* tpb_; /// tpb


    // Pointers to logical volumes
    G4LogicalVolume* lab_logic_;
    G4LogicalVolume* gas_logic_;
    G4LogicalVolume* pmt_logic_;


    // Detector dimensions

    const G4double lab_size_; ///< Size of the air box containing the detector

    const G4double vessel_diam_;   ///< Can internal diameter
    const G4double vessel_length_; ///< Can internal length
    const G4double vessel_thickn_; ///< Can thickness

    const G4double endcap_diam_;   ///< Vessel endcap diameter
    const G4double endcap_thickn_; ///< Vessel endcap thickness

    const G4double sideport_diam_;   ///< Side source-port internal diameter
    const G4double sideport_length_; ///< Side source-port internal length
    const G4double sideport_thickn_; ///< Side source-port thickness

    const G4double sideport_flange_diam_;   ///< Side-port flange diameter
    const G4double sideport_flange_thickn_; ///< Side-port flange thickness

    const G4double sideport_tube_diam_;   ///< Collimation tube internal diameter
    const G4double sideport_tube_length_; ///< Collimation tube internal length
    const G4double sideport_tube_thickn_; ///< Collimation tube thickness
    const G4double sideport_tube_window_thickn_; ///< Tube window thickness

    const G4double axialport_diam_;   ///< Axial source-port internal diameter
    const G4double axialport_length_; ///< Axial source-port internal length
    const G4double axialport_thickn_; ///< Axial source-port internal thickness

    const G4double axialport_flange_diam_;   ///< Axial port flange diameter
    const G4double axialport_flange_thickn_; ///< Axial port flange thickness

    const G4double axialport_tube_diam_;   ///< Collimation tube int. diameter
    const G4double axialport_tube_length_; ///< Collimation tube int. length
    const G4double axialport_tube_thickn_; ///< Collimation tube thickness
    const G4double axialport_tube_window_thickn_; ///< Tube window thickness

    const G4double elgap_length_; ///< Length of the EL gap

    const G4double elgap_ring_diam_;
    const G4double elgap_ring_thickn_;
    const G4double elgap_ring_height_;
    //   const G4double wire_diam_; ///< diameter of the EL meshes wires

    const G4double tpb_thickn_;

    const G4double ltube_diam_;
    const G4double ltube_thickn_;
    const G4double ltube_up_length_;
    const G4double ltube_bt_length_;
    const G4double ltube_gap_;

    const G4double active_diam_;
    const G4double active_length_;

    const G4double fieldcage_length_; ///< Fieldcage length
    const G4double fieldcage_displ_;  ///< Distance btw fieldcage & anode endcap

    const G4double ring_diam_;
    //   const G4double ring_height_;
    const G4double ring_thickn_;

    const G4double bar_width_;
    const G4double bar_thickn_;
    const G4double bar_addon_length_;

    const G4double pmtholder_cath_diam_;
    const G4double pmtholder_cath_height_;
    const G4double pmtholder_cath_cutout_depth_;
    const G4double pmtholder_cath_displ_;
    const G4double pmt_pitch_;

    const G4double pmtholder_anode_diam_;
    const G4double pmtholder_anode_thickn_;

   G4double max_step_size_;
   G4double sc_yield_;
   G4double e_lifetime_;


    // Dimensions read through input parameters file
    //

    G4double sideport_angle_;
    G4double sideport_posz_;
    G4bool tpb_coating_;
    G4bool elfield_;
    G4bool external_scintillator_;
    G4double elgrid_transparency_;
    G4double gate_transparency_;
    G4ThreeVector specific_vertex_;
    G4bool muonsGenerator_;

    // Positions in the detector geometry relevant for event vertex generation
    //

    /// Position of the side source-port in the world reference system
    G4ThreeVector sideport_position_;
    G4ThreeVector sideport_ext_position_;
    G4ThreeVector sideNa_pos_;
    /// Position of the axial source-port in the world reference system
    G4ThreeVector axialport_position_;
    /// Position of the fieldcage in the world system of reference
    G4ThreeVector fieldcage_position_;
    ///
    G4ThreeVector active_position_;
    ///
    G4ThreeVector elgap_position_;

    G4String tracking_plane_; ///< Tracking plane type: SIPM or PMT


    // Positions (Z axis) of the TPC electrodes with respect to the
    // FIELDCAGE system of reference
    G4double gate_posz_;    ///< Z position of the gate wrt FIELDCAGE
    G4double anode_posz_;   ///< Z position of the anode wrt FIELDCAGE
    G4double cathode_posz_; ///< Z position of the cathode wrt FIELDCAGE

    G4double pressure_;

    mutable unsigned int idx_table_;
    mutable std::vector<G4ThreeVector> table_vertices_;

    std::vector<G4ThreeVector> pmt_positions_;

    //Vector to store the ID number of each SiPM together with its absolute position in gas
    std::vector<std::pair<int, G4ThreeVector> > absSiPMpos_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    /// To generate vertices in the Na22 source
    CylinderPointSamplerLegacy* cps_;
  };


} // end namespace nexus

#endif
