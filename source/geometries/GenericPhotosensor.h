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


namespace nexus {

  class GenericPhotosensor: public BaseGeometry
  {
  public:
    // Constructor for a rectangular sensor providing
    // width (w), height (h) and thickness (t).
    // The default thickness corresponds to a typical value for
    // a silicon photomultiplier.
    GenericPhotosensor(G4double w, G4double h, G4double t=2.0*mm);
    // Constructor for a square sensor
    GenericPhotosensor(G4double size);
    // Destructor
    ~GenericPhotosensor();
    //
    void Construct();

    G4double GetWidth()     const;
    G4double GetHeight()    const;
    G4double GetThickness() const;

    void SetRefractiveIndex(G4MaterialPropertyVector*);

  private:
    G4double width_, height_, thickness_;
    G4double time_binning_;
    G4Material* window_mat_;
    G4GenericMessenger* msg_;
  };

  inline G4double GenericPhotosensor::GetWidth() const { return width_; }
  inline G4double GenericPhotosensor::GetHeight() const { return height_; }
  inline G4double GenericPhotosensor::GetThickness() const { return thickness_; }

} // namespace nexus

#endif
