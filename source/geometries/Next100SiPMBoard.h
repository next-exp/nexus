// -----------------------------------------------------------------------------
//  nexus | Next100SiPMBoard.h
//
//  * Author: <justo.martin-albo@ific.uv.es>
//  * Creation date: 7 January 2020
// -----------------------------------------------------------------------------

#ifndef NEXT100_SIPM_BOARD_H
#define NEXT100_SIPM_BOARD_H

#include "BaseGeometry.h"


namespace nexus {

  class Next100SiPMBoard: public nexus::BaseGeometry
  {
  public:
    // Default constructor
    Next100SiPMBoard();
    // Destructor
    virtual ~Next100SiPMBoard();
    //
    virtual void Construct();
    //
    virtual G4ThreeVector GenerateVertex(const G4String&) const;

    G4double GetSize() const;
    G4double GetThickness() const;

  private:
    G4double size_, thickness_, pitch_, margin_;
  };

  inline G4double Next100SiPMBoard::GetSize() const { return size_; }
  inline G4double Next100SiPMBoard::GetThickness() const { return thickness_; }

} // namespace nexus

#endif
