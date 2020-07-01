// ----------------------------------------------------------------------------
// nexus | DetectorConstruction.h
//
// This class is used to initialize the detector geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef DETECTOR_CONSTRUCTION_H
#define DETECTOR_CONSTRUCTION_H

#include <G4VUserDetectorConstruction.hh>

class G4GenericMessenger;


namespace nexus {

  class BaseGeometry;


  /// User initialization class for detector setup

  class DetectorConstruction: public G4VUserDetectorConstruction
  {
  public:
    /// Constructor
    DetectorConstruction();
    /// Destructor
    ~DetectorConstruction();

    /// Mandatory method invoked by the run manager.
    /// It returns the physical volume that represents the world.
    virtual G4VPhysicalVolume* Construct();

    /// Set a detector geometry
    void SetGeometry(BaseGeometry*);
    /// Get the detector geometry
    const BaseGeometry* GetGeometry() const;

  private:
    BaseGeometry* geometry_;
  };


  // INLINE DEFINITIONS /////////////////////////////////////////////

  inline void DetectorConstruction::SetGeometry(BaseGeometry* g)
  { geometry_ = g; }

  inline const BaseGeometry* DetectorConstruction::GetGeometry() const
  { return geometry_; }

} // end namespace nexus

#endif
