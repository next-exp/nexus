// -----------------------------------------------------------------------------
//  nexus | GenericPhotosensor.h
//
//  * Author: <justo.martin-albo@ific.uv.es>
//  * Creation date: 22 January 2020
// -----------------------------------------------------------------------------

#ifndef GENERIC_PHOTOSENSOR_H
#define GENERIC_PHOTOSENSOR_H

#include "BaseGeometry.h"
#include <G4MaterialPropertyVector.hh>

class G4Material;
class G4GenericMessenger;
class G4MaterialPropertiesTable;

namespace nexus {

  class GenericPhotosensor: public BaseGeometry
  {
  public:
    // Constructor for a rectangular sensor providing
    // width (w), height (h) and thickness (t).
    // The default thickness corresponds to a typical value for
    // a silicon photomultiplier.
    GenericPhotosensor(G4String name, G4double width,
                       G4double height, G4double thickness = 2.0*mm);
    
    // Constructor for a square sensor
    GenericPhotosensor(G4String name, G4double size);
    
    // Destructor
    ~GenericPhotosensor();

    //
    void Construct();
    void ComputeDimensions();
    void DefineMaterials();

    //
    G4double GetWidth()     const;
    G4double GetHeight()    const;
    G4double GetThickness() const;
    G4String GetName()      const;

    void SetWithWLSCoating       (G4bool with_WLScoating);
    void SetWindowRefractiveIndex(G4MaterialPropertyVector* rIndex);
    void SetOpticalProperties    (G4MaterialPropertiesTable* mpt);
    void SetSensorDepth          (G4int sensor_depth);
    void SetMotherDepth          (G4int mother_depth);
    void SetNamingOrder          (G4int naming_order);

  private:
    G4GenericMessenger* msg_;

    G4String name_;
    
    G4double width_, height_, thickness_;
    G4double sensitive_z_;
    G4double encasing_x_, encasing_y_, encasing_z_;
    G4double window_x_,   window_y_,   window_z_;
    G4double wls_x_,      wls_y_,      wls_z_;

    G4Material* encasing_mat_;
    G4Material* window_mat_;
    G4Material* sensitive_mat_;
    G4Material* wls_mat_;

    G4bool                     with_WLScoating_;
    G4MaterialPropertyVector*  window_rIndex_;
    G4MaterialPropertiesTable* sensitive_mpt_;

    G4int    sensor_depth_;
    G4int    mother_depth_;
    G4int    naming_order_;
    G4double time_binning_;

    G4bool visibility_;
  };


  inline G4double GenericPhotosensor::GetWidth()     const { return width_; }
  inline G4double GenericPhotosensor::GetHeight()    const { return height_; }
  inline G4double GenericPhotosensor::GetThickness() const { return thickness_; }
  inline G4String GenericPhotosensor::GetName()      const { return name_; }

  inline void GenericPhotosensor::SetWithWLSCoating(G4bool with_WLScoating)
  { with_WLScoating_ = with_WLScoating; }

  inline void GenericPhotosensor::SetWindowRefractiveIndex(G4MaterialPropertyVector* rIndex)
  { window_rIndex_ = rIndex; }

  inline void GenericPhotosensor::SetOpticalProperties(G4MaterialPropertiesTable* mpt)
  { sensitive_mpt_ = mpt; }

  inline void GenericPhotosensor::SetSensorDepth(G4int sensor_depth)
  { sensor_depth_ = sensor_depth; }

  inline void GenericPhotosensor::SetMotherDepth(G4int mother_depth)
  { mother_depth_ = mother_depth; }

  inline void GenericPhotosensor::SetNamingOrder(G4int naming_order)
  { naming_order_ = naming_order; }


} // namespace nexus

#endif
