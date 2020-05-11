// -----------------------------------------------------------------------------
//  nexus | Next100SiPMBoard.h
//
//  NEXT-100 SiPM board implemntation. It contains the 8x8 SiPMs arranged in a
//  square matrix, and the teflon masks.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT100_SIPM_BOARD_H
#define NEXT100_SIPM_BOARD_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>
#include <vector>

class G4VPhysicalVolume;
class G4GenericMessenger;

namespace nexus {

  class BoxPointSampler;
  class GenericPhotosensor;

  // Geometry of the 8x8 SiPM boards used in the tracking plane of NEXT-100

  class Next100SiPMBoard: public nexus::BaseGeometry
  {
  public:
    // Default constructor
    Next100SiPMBoard();
    // Destructor
    ~Next100SiPMBoard();
    //
    void SetMotherPhysicalVolume(G4VPhysicalVolume*);
    //
    void Construct();
    //
    G4ThreeVector GenerateVertex(const G4String&) const;

    G4double GetSize() const;
    G4double GetThickness() const;

    const std::vector<G4ThreeVector>& GetSiPMPositions() const;

  private:
    G4GenericMessenger* msg_;
    G4double size_, board_thickness_, pitch_, margin_;
    G4double mask_thickness_;
    std::vector<G4ThreeVector> sipm_positions_;
    G4VPhysicalVolume*  mpv_;
    BoxPointSampler*    vtxgen_;
    GenericPhotosensor* sipm_;
    G4double            time_binning_;
  };

  inline void     Next100SiPMBoard::SetMotherPhysicalVolume(G4VPhysicalVolume* p)
  { mpv_ = p;}

  inline G4double Next100SiPMBoard::GetSize() const
  { return size_; }

  inline G4double Next100SiPMBoard::GetThickness() const
  { return (board_thickness_ + mask_thickness_); }
  
  inline const std::vector<G4ThreeVector>& Next100SiPMBoard::GetSiPMPositions() const
  { return sipm_positions_; }

} // namespace nexus

#endif
