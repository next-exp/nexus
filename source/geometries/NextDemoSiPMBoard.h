// -----------------------------------------------------------------------------
// nexus | NextDemoSiPMBoard.h
//
// Geometry of the DEMO++ SiPM board.
// It consists of an 8x8 array of SensL SiPMs on a kapton board.
// The board can be covered with a teflon mask, or not.
// The teflon mask may have membranes covering the holes, or not.
// The teflon mask may be coated with TPB or not.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXTDEMO_SIPM_BOARD_H
#define NEXTDEMO_SIPM_BOARD_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>
#include <vector>

class G4VPhysicalVolume;
class G4GenericMessenger;

namespace nexus {

  class BoxPointSampler;
  class SiPMSensl;

  class NextDemoSiPMBoard: public BaseGeometry
  {
  public:
    NextDemoSiPMBoard();
    ~NextDemoSiPMBoard();

    void Construct() override;
    G4ThreeVector GenerateVertex(const G4String&) const override;

    void SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys);
    void SetMaskThickness    (G4double thickn);
    void SetMembraneThickness(G4double thickn);
    void SetCoatingThickness (G4double thickn);
    void SetHoleType         (G4String type);
    void SetHoleDiameter     (G4double diam);
    void SetHoleX            (G4double x);
    void SetHoleY            (G4double y);

    G4ThreeVector GetBoardSize() const;
    G4double      GetKaptonThickness() const;
    void          GenerateSiPMPositions();

  private:
    G4bool verbosity_;
    G4bool sipm_verbosity_;
    G4bool visibility_;

    G4int    num_columns_, num_rows_, num_sipms_;
    G4double sipm_pitch_;
    G4double side_reduction_;

    G4double kapton_thickn_;
    G4double mask_thickn_;
    G4double membrane_thickn_;
    G4double coating_thickn_;
    G4String hole_type_;
    G4double hole_diam_;
    G4double hole_x_;
    G4double hole_y_;

    G4ThreeVector board_size_;
    SiPMSensl* sipm_;
    std::vector<G4ThreeVector> sipm_positions_;
    G4VPhysicalVolume* mother_phys_;
    BoxPointSampler* kapton_gen_;

    G4GenericMessenger* msg_;
  };

  inline void NextDemoSiPMBoard::SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys)
  { mother_phys_ = mother_phys; }

  inline void NextDemoSiPMBoard::SetMaskThickness(G4double thickn)
  { mask_thickn_ = thickn; }

  inline void NextDemoSiPMBoard::SetMembraneThickness(G4double thickn)
  { membrane_thickn_ = thickn; }

  inline void NextDemoSiPMBoard::SetCoatingThickness(G4double thickn)
  { coating_thickn_ = thickn; }

  inline void NextDemoSiPMBoard::SetHoleType(G4String type)
  { hole_type_ = type; }

  inline void NextDemoSiPMBoard::SetHoleDiameter(G4double diam)
  { hole_diam_ = diam; }

  inline void NextDemoSiPMBoard::SetHoleX(G4double x)
  { hole_x_ = x; }

  inline void NextDemoSiPMBoard::SetHoleY(G4double y)
  { hole_y_ = y; }

  inline G4ThreeVector NextDemoSiPMBoard::GetBoardSize() const
  { return board_size_; }

  inline G4double NextDemoSiPMBoard::GetKaptonThickness() const
  { return kapton_thickn_; }

} // namespace nexus

#endif
