// ----------------------------------------------------------------------------
///  \file   BaseGeometry.h
///  \brief  Abstract base class for encapsulation of geometries.
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     27 Mar 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef BASE_GEOMETRY_H
#define BASE_GEOMETRY_H

#include <G4ThreeVector.hh>
#include <CLHEP/Units/SystemOfUnits.h>

class G4LogicalVolume;

namespace nexus {

  using namespace CLHEP;

  /// Abstract base class for encapsulation of detector geometries.

  class BaseGeometry
  {
  public:
    /// The volumes (solid, logical and physical) must be defined
    /// in this method, which will be invoked during the detector
    /// construction phase
    virtual void Construct() = 0;

    /// Returns the logical volume representing the geometry
    G4LogicalVolume* GetLogicalVolume() const;

    /// Returns a point within a given region of the geometry
    virtual G4ThreeVector GenerateVertex(const G4String&) const;

    /// Returns the span (maximum dimension) of the geometry
    G4double GetSpan();

    /// Returns true if geometry has a drift field
    G4bool GetDrift() const;

    // Getter for the starting point of EL generation in z
    G4double GetELzCoord() const;

    /// Setter for the starting point of EL generation in z
    void SetELzCoord(G4double z);

    /// Translates position to G4 global position
    void CalculateGlobalPos(G4ThreeVector& vertex) const;

    /// Destructor
    virtual ~BaseGeometry();

  protected:
    /// Default constructor defined as protected so no instance of
    /// this base class can be created.
    BaseGeometry();

    /// Sets the logical volume representing the geometry
    void SetLogicalVolume(G4LogicalVolume*);

    /// Sets the span (maximum dimension) of the geometry
    void SetSpan(G4double);

    /// Sets the drift variable to true if a drift field exists
    void SetDrift(G4bool);

  private:
    /// Copy-constructor (hidden)
    BaseGeometry(const BaseGeometry&);
    /// Assignment operator (hidden)
    const BaseGeometry& operator=(const BaseGeometry&);

  private:
    G4LogicalVolume* _logicVol; ///< Pointer to the logical volume
    G4double _span; ///< Maximum dimension of the geometry
    G4bool _drift; ///< True if geometry contains a drift field (for hit coordinates)
    G4double _el_z; ///< Starting point of EL generation in z
  };


  // Inline definitions ///////////////////////////////////

  inline BaseGeometry::BaseGeometry(): _logicVol(0), _span(25.*m), _drift(false), _el_z(0.*mm) {}

  inline BaseGeometry::~BaseGeometry() {}

  inline G4LogicalVolume* BaseGeometry::GetLogicalVolume() const
  { return _logicVol; }

  inline void BaseGeometry::SetLogicalVolume(G4LogicalVolume* lv)
  { _logicVol = lv; }

  inline G4ThreeVector BaseGeometry::GenerateVertex(const G4String&) const
  { return G4ThreeVector(0., 0., 0.); }

  inline void BaseGeometry::SetSpan(G4double s) { _span = s; }

  inline G4double BaseGeometry::GetSpan() { return _span; }

  inline void BaseGeometry::SetDrift(G4bool drift) { _drift = drift; }

  inline G4bool BaseGeometry::GetDrift() const { return _drift; }

  inline G4double BaseGeometry::GetELzCoord() const {return _el_z;}

  inline void BaseGeometry::SetELzCoord(G4double z) {_el_z = z;}

  // This methods is to be used only in the Next1EL and NEW geometries
  inline void BaseGeometry::CalculateGlobalPos(G4ThreeVector& vertex) const
  {
    // Rotates and translates into G4 global
    // coordinates, needed for volume checks.
    vertex.rotate(pi, G4ThreeVector(0., 1., 0.));

    vertex += G4ThreeVector(0., 0., GetELzCoord());
  }

} // end namespace nexus

#endif
