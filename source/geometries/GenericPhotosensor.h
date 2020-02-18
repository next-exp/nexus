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


namespace nexus {

  class GenericPhotosensor: public nexus::BaseGeometry
  {
  public:
    // Constructor for a rectangular sensor
    GenericPhotosensor(G4double width, G4double height);
    // Constructor for a square sensor
    GenericPhotosensor(G4double size);
    // Destructor
    virtual ~GenericPhotosensor();
    //
    virtual void Construct();
    //
    virtual G4ThreeVector GenerateVertex(const G4String&) const;

    G4double GetWidth()     const;
    G4double GetHeight()    const;
    G4double GetThickness() const;

    //void SetRefractiveIndex(G4MaterialPropertyVector*);

  private:
    //void SetDefaultOpticalProperties();

  private:
    G4double width_, height_, thickness_;
    G4Material* window_mat_;
  };

  inline G4double GenericPhotosensor::GetWidth() const { return width_; }
  inline G4double GenericPhotosensor::GetHeight() const { return height_; }
  inline G4double GenericPhotosensor::GetThickness() const { return thickness_; }

} // namespace nexus

#endif
