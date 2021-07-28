// -----------------------------------------------------------------------------
// nexus | Next100SiPMBoard.h
//
// Geometry of the NEXT-100 SiPM board, consisting of an 8x8 array of
// silicon photomultipliers (1.3x1.3 mm2 of active area) mounted on a Kapton
// board covered with a TPB-coated teflon mask.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT100_SIPM_BOARD_H
#define NEXT100_SIPM_BOARD_H

#include "GeometryBase.h"
#include <G4ThreeVector.hh>
#include <vector>

class G4VPhysicalVolume;
class G4GenericMessenger;

namespace nexus {

  class BoxPointSampler;
  class Next100SiPM;

  // Geometry of the 8x8 SiPM boards used in the tracking plane of NEXT-100

  class Next100SiPMBoard: public GeometryBase
  {
  public:
    // Default constructor
    Next100SiPMBoard();
    // Destructor
    ~Next100SiPMBoard();
    //
    void SetMotherPhysicalVolume(G4VPhysicalVolume*);
    //
    void Construct() override;
    //
    G4ThreeVector GenerateVertex(const G4String&) const override;

    G4double GetSize() const;
    G4double GetThickness() const;

    const std::vector<G4ThreeVector>& GetSiPMPositions() const;

  private:
    G4GenericMessenger* msg_;
    G4double size_, pitch_, margin_;
    G4double hole_diam_;
    G4double board_thickness_, mask_thickness_;
    G4double time_binning_;
    std::vector<G4ThreeVector> sipm_positions_;
    G4bool   visibility_, sipm_visibility_;
    G4VPhysicalVolume*  mpv_;
    BoxPointSampler*    vtxgen_;
    Next100SiPM* sipm_;
  };

  inline void Next100SiPMBoard::SetMotherPhysicalVolume(G4VPhysicalVolume* p)
  { mpv_ = p;}

  inline G4double Next100SiPMBoard::GetSize() const
  { return size_; }

  inline G4double Next100SiPMBoard::GetThickness() const
  { return (board_thickness_ + mask_thickness_); }

  inline const std::vector<G4ThreeVector>& Next100SiPMBoard::GetSiPMPositions() const
  { return sipm_positions_; }

} // namespace nexus

#endif
