// ----------------------------------------------------------------------------
///  \file   Next100InnerElements.h
///  \brief  Geometry of the field cage and sensor planes of NEXT-100.
///
///  \author   <jmunoz@ific.uv.es>
///  \date     2 Mar 2012
///  \version  $Id$
///
///  Copyright (c) 2012-2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_INNER_ELEMENTS_H
#define NEXT100_INNER_ELEMENTS_H

#include <G4ThreeVector.hh>
#include <vector>
#include "BaseGeometry.h"

class G4LogicalVolume;
class G4Material;
class G4GenericMessenger;


namespace nexus {

  class Next100FieldCage;
  class Next100EnergyPlane;
  class Next100TrackingPlane;


  /// This is a geometry placer that encloses the energy and tracking planes and
  /// the field cage, including all internal elements such as active and buffer volumes.

  class Next100InnerElements : public BaseGeometry
  {

  public:
    ///Constructor
    Next100InnerElements();

    /// Destructor
    ~Next100InnerElements();

    /// Set the logical volume that encloses the entire geometry
    void SetLogicalVolume(G4LogicalVolume*);

    /// Return the relative position respect to the rest of NEXT100 geometry
    G4ThreeVector GetPosition() const;

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();


  private:

    G4LogicalVolume* _mother_logic;
    G4Material* _gas;

    G4double _pressure;
    G4double _temperature;

    // Detector parts
    Next100FieldCage*     _field_cage;
    Next100EnergyPlane*   _energy_plane;
    Next100TrackingPlane* _tracking_plane;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg;

  };

} // end namespace nexus

#endif
