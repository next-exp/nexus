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

  class GeometryBase;

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
    void SetGeometry(GeometryBase*);
    /// Get the detector geometry
    const GeometryBase* GetGeometry() const;

  private:
    GeometryBase* geometry_;
  };


  // INLINE DEFINITIONS /////////////////////////////////////////////

  inline void DetectorConstruction::SetGeometry(GeometryBase* g)
  { geometry_ = g; }

  inline const GeometryBase* DetectorConstruction::GetGeometry() const
  { return geometry_; }

} // end namespace nexus

#endif
