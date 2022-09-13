// -----------------------------------------------------------------------------
// nexus | BlackBoxSiPMBoard.h
//
// SiPM Board used in BlackBox.
// -----------------------------------------------------------------------------

#ifndef BLACKBOX_SIPM_BOARD_H
#define BLACKBOX_SIPM_BOARD_H

#include "GeometryBase.h"
#include <G4ThreeVector.hh>
#include <vector>

class G4VPhysicalVolume;
class G4GenericMessenger;

namespace nexus {

  class BoxPointSampler;
  class SiPMSensl;

  class BlackBoxSiPMBoard: public GeometryBase
  {
  public:
    BlackBoxSiPMBoard();
    ~BlackBoxSiPMBoard();

    void Construct() override;
    G4ThreeVector GenerateVertex(const G4String&) const override;

    void SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys);
    void SetMaskThickness    (G4double thickn);
    void SetCoatingThickness (G4double thickn);
    void SetHoleDiameter     (G4double diam);
    void SetHoleX            (G4double x);
    void SetHoleY            (G4double y);

    G4ThreeVector GetBoardSize() const;
    G4double      GetKaptonThickness() const;
    void          GenerateSiPMPositions();

  private:
    G4bool verbosity_;

    G4int    num_columns_, num_rows_, num_sipms_;
    G4double sipm_pitch_;
    G4double side_reduction_;

    G4double kapton_thickn_;
    G4double mask_thickn_;
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

  inline void BlackBoxSiPMBoard::SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys)
  { mother_phys_ = mother_phys; }

  inline void BlackBoxSiPMBoard::SetMaskThickness(G4double thickn)
  { mask_thickn_ = thickn; }

  inline void BlackBoxSiPMBoard::SetHoleDiameter(G4double diam)
  { hole_diam_ = diam; }

  inline void BlackBoxSiPMBoard::SetHoleX(G4double x)
  { hole_x_ = x; }

  inline void BlackBoxSiPMBoard::SetHoleY(G4double y)
  { hole_y_ = y; }

  inline G4ThreeVector BlackBoxSiPMBoard::GetBoardSize() const
  { return board_size_; }

  inline G4double BlackBoxSiPMBoard::GetKaptonThickness() const
  { return kapton_thickn_; }

} // namespace nexus

#endif
