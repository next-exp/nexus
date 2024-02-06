// -----------------------------------------------------------------------------
//  nexus | GenericSquarePhotosensor.h
//
//  Geometry of a configurable box-shaped photosensor.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef GENERIC_SQUAREPHOTOSENSOR_H
#define GENERIC_SQUAREPHOTOSENSOR_H

#include "GeometryBase.h"
#include <G4MaterialPropertyVector.hh>

class G4Material;
class G4GenericMessenger;
class G4MaterialPropertiesTable;

namespace nexus {

  class GenericSquarePhotosensor: public GeometryBase
  {
  public:
    // Constructor for a rectangular sensor providing
    // The default thickness corresponds to a typical value for
    // a silicon photomultiplier.
    GenericSquarePhotosensor(G4String name,   G4double width,
                       G4double height, G4double thickness = 2.0 * mm);

    // Constructor for a square sensor
    GenericSquarePhotosensor(G4String name, G4double size);

    // Destructor
    ~GenericSquarePhotosensor();

    //
    void Construct();

    //
    G4double GetWidth()       const;
    G4double GetHeight()      const;
    G4double GetThickness()   const;
    const G4String& GetName() const;

    void SetVisibility           (G4bool visibility);
    void SetWithWLSCoating       (G4bool with_wls_coating);
    void SetWindowRefractiveIndex(G4MaterialPropertyVector* rindex);
    void SetOpticalProperties    (G4MaterialPropertiesTable* mpt);
    void SetTimeBinning          (G4double time_binning);
    void SetSensorDepth          (G4int sensor_depth);
    void SetMotherDepth          (G4int mother_depth);
    void SetNamingOrder          (G4int naming_order);

  private:

    void ComputeDimensions();
    void DefineMaterials();

    G4String name_;

    G4double width_, height_, thickness_;
    G4double window_thickness_;
    G4double sensarea_thickness_;
    G4double wls_thickness_;
    G4double reduced_width_, reduced_height_;

    G4Material* case_mat_;
    G4Material* window_mat_;
    G4Material* sensitive_mat_;
    G4Material* wls_mat_;

    G4bool                     with_wls_coating_;
    G4MaterialPropertyVector*  window_rindex_;
    G4MaterialPropertiesTable* sensitive_mpt_;

    G4int    sensor_depth_;
    G4int    mother_depth_;
    G4int    naming_order_;
    G4double time_binning_;

    G4bool visibility_;
  };


  inline G4double GenericSquarePhotosensor::GetWidth()       const { return width_; }
  inline G4double GenericSquarePhotosensor::GetHeight()      const { return height_; }
  inline G4double GenericSquarePhotosensor::GetThickness()   const { return thickness_; }
  inline const G4String& GenericSquarePhotosensor::GetName() const { return name_; }

  inline void GenericSquarePhotosensor::SetVisibility(G4bool visibility)
  { visibility_ = visibility; }

  inline void GenericSquarePhotosensor::SetWithWLSCoating(G4bool with_wls_coating)
  { with_wls_coating_ = with_wls_coating; }

  inline void GenericSquarePhotosensor::SetWindowRefractiveIndex(G4MaterialPropertyVector* rindex)
  { window_rindex_ = rindex; }

  inline void GenericSquarePhotosensor::SetOpticalProperties(G4MaterialPropertiesTable* mpt)
  { sensitive_mpt_ = mpt; }

  inline void GenericSquarePhotosensor::SetTimeBinning(G4double time_binning)
  { time_binning_ = time_binning; }

  inline void GenericSquarePhotosensor::SetSensorDepth(G4int sensor_depth)
  { sensor_depth_ = sensor_depth; }

  inline void GenericSquarePhotosensor::SetMotherDepth(G4int mother_depth)
  { mother_depth_ = mother_depth; }

  inline void GenericSquarePhotosensor::SetNamingOrder(G4int naming_order)
  { naming_order_ = naming_order; }


} // namespace nexus

#endif
