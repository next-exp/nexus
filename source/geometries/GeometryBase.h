// ----------------------------------------------------------------------------
// nexus | GeometryBase.h
//
// This is an abstract base class for encapsulation of geometries.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef GEOMETRY_BASE_H
#define GEOMETRY_BASE_H

#include <G4ThreeVector.hh>
#include <CLHEP/Units/SystemOfUnits.h>

class G4LogicalVolume;

namespace nexus {

  using namespace CLHEP;

  /// Abstract base class for encapsulation of detector geometries.

  class GeometryBase
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

    /// Returns a point within a region projecting from a
    /// given point backwards along a line.
    virtual G4ThreeVector ProjectToRegion(const G4String&,
					  const G4ThreeVector&,
					  const G4ThreeVector&) const;

    /// Returns the span (maximum dimension) of the geometry
    G4double GetSpan();

    /// Returns the 3 dimensions of the geometry (x, y, z)
    G4ThreeVector GetDimensions();

    // Getter for the starting point of EL generation in z
    G4double GetELzCoord() const;

    /// Setter for the starting point of EL generation in z
    void SetELzCoord(G4double z);

    /// Getter for the origin of coordinates
    G4ThreeVector GetCoordOrigin() const;

    /// Setter for the origin of coordinates
    void SetCoordOrigin(G4ThreeVector origin);

    /// Translates position to G4 global position
    void CalculateGlobalPos(G4ThreeVector& vertex) const;

    /// Destructor
    virtual ~GeometryBase();

  protected:
    /// Default constructor defined as protected so no instance of
    /// this base class can be created.
    GeometryBase();

    /// Sets the logical volume representing the geometry
    void SetLogicalVolume(G4LogicalVolume*);

    /// Sets the span (maximum dimension) of the geometry
    void SetSpan(G4double);

    /// Sets the 3 dimensions of the geometry (x, y, z)
    void SetDimensions(G4ThreeVector dim);

  private:
    /// Copy-constructor (hidden)
    GeometryBase(const GeometryBase&);
    /// Assignment operator (hidden)
    const GeometryBase& operator=(const GeometryBase&);

  private:
    G4LogicalVolume* logicVol_; ///< Pointer to the logical volume
    G4double span_; ///< Maximum dimension of the geometry
    G4ThreeVector dimensions_; ///< XYZ dimensions of a regular geometry
    G4double el_z_; ///< Starting point of EL generation in z
    G4ThreeVector coord_origin_; ///< Origin of coordinates of the mother volume
  };


  // Inline definitions ///////////////////////////////////

  inline GeometryBase::GeometryBase(): logicVol_(0), span_(25.*m),  el_z_(0.*mm), coord_origin_(G4ThreeVector(0., 0., 0.)) {}

  inline GeometryBase::~GeometryBase() {}

  inline G4LogicalVolume* GeometryBase::GetLogicalVolume() const
  { return logicVol_; }

  inline void GeometryBase::SetLogicalVolume(G4LogicalVolume* lv)
  { logicVol_ = lv; }

  inline G4ThreeVector GeometryBase::GenerateVertex(const G4String&) const
  { return G4ThreeVector(0., 0., 0.); }

  inline G4ThreeVector GeometryBase::ProjectToRegion(const G4String&,
						     const G4ThreeVector&,
						     const G4ThreeVector&) const
  { return G4ThreeVector(0., 0., 0.); }

  inline void GeometryBase::SetSpan(G4double s) { span_ = s; }

  inline G4double GeometryBase::GetSpan() { return span_; }

  inline void GeometryBase::SetDimensions(G4ThreeVector dim) {  dimensions_ = dim; }

  inline  G4ThreeVector GeometryBase::GetDimensions()  { return dimensions_; }

  inline G4double GeometryBase::GetELzCoord() const {return el_z_;}

  inline void GeometryBase::SetELzCoord(G4double z) {el_z_ = z;}

  inline G4ThreeVector GeometryBase::GetCoordOrigin() const {return coord_origin_;}

  inline void GeometryBase::SetCoordOrigin(G4ThreeVector origin) {coord_origin_ = origin;}

  // This methods is to be used only in the Next1EL and NEW geometries
  inline void GeometryBase::CalculateGlobalPos(G4ThreeVector& vertex) const
  {
    // Rotates and translates into G4 global
    // coordinates, needed for volume checks.
    vertex.rotate(pi, G4ThreeVector(0., 1., 0.));

    vertex += G4ThreeVector(0., 0., GetELzCoord());
  }

} // end namespace nexus

#endif
