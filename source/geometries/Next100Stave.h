// ----------------------------------------------------------------------------
// nexus | Next100Stave.h
//
// Geometry of the staves that support the field rings
// in the NEXT-100 field cage.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_STAVE_H
#define NEXT100_STAVE_H

#include "GeometryBase.h"

class G4GenericMessenger;

namespace nexus {

  class Next100Stave: public GeometryBase
  {
  public:
    Next100Stave(G4double ring_drift_buffer_dist_,
                 G4int num_drift_rings,
                 G4int num_buffer_rings);
    ~Next100Stave();

    void Construct() override;

    G4double GetHolderShortY() const;
    G4double GetHolderLongY() const;
    G4double GetCathodeLongY() const;
    G4double GetBufferLongLength() const;
    G4double GetCathodeOpening() const;

  private:
    G4double visibility_;
    const G4int num_drift_rings_, num_buffer_rings_;
    const G4double ring_drift_buffer_dist_;
    const G4double holder_x_, holder_long_y_, holder_short_y_;
    const G4double buffer_ring_dist_, buffer_long_length_;
    const G4double buffer_short_length_, buffer_last_z_;
    const G4double drift_ring_dist_, drift_long_length_;
    const G4double drift_short_first_length_;
    const G4double drift_short_length_;
    const G4double cathode_opening_, cathode_long_y_, cathode_long_length_;
    const G4double cathode_short_length_;
    const G4double overlap_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

  inline G4double Next100Stave::GetHolderShortY() const {return holder_short_y_;};
  inline G4double Next100Stave::GetHolderLongY() const {return holder_long_y_;};
  inline G4double Next100Stave::GetCathodeLongY() const {return cathode_long_y_;};
  inline G4double Next100Stave::GetBufferLongLength() const {return buffer_long_length_;};
  inline G4double Next100Stave::GetCathodeOpening() const {return cathode_opening_;};

}

#endif
