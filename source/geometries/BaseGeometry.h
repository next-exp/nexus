// ----------------------------------------------------------------------------
///  \file   BaseGeometry.h
///  \brief  Abstract base class for encapsulation of geometries.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     27 Mar 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __BASE_GEOMETRY__
#define __BASE_GEOMETRY__

#include <G4ThreeVector.hh>

class G4LogicalVolume;


namespace nexus {

  /// Abstract base class for encapsulation of detector geometries.

  class BaseGeometry
  {
  public:
    /// Returns the logical volume representing the geometry
    G4LogicalVolume* GetLogicalVolume();
    
    /// Returns a point within a given region of the geometry
    virtual G4ThreeVector GenerateVertex(const G4String&) const;

    virtual G4ThreeVector GenerateVertex(G4int) const;

    /// Returns the span (maximum dimension) of the geometry
    G4double GetSpan();
    
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
    
  private:
    /// Copy-constructor (hidden)
    BaseGeometry(const BaseGeometry&);
    /// Assignment operator (hidden)
    const BaseGeometry& operator=(const BaseGeometry&);

  private:
    G4LogicalVolume* _logicVol; ///< Pointer to the logical volume    
    G4double _span; ///< Maximum dimension of the geometry
  };
  
  
  // inline methods ..................................................

  inline BaseGeometry::BaseGeometry(): _logicVol(0), _span(25.*m) {}

  inline BaseGeometry::~BaseGeometry() {}

  inline G4LogicalVolume* BaseGeometry::GetLogicalVolume()
  { return _logicVol; }
  
  inline void BaseGeometry::SetLogicalVolume(G4LogicalVolume* lv)
  { _logicVol = lv; }
  
  inline G4ThreeVector BaseGeometry::GenerateVertex(const G4String&) const
  { return G4ThreeVector(0., 0., 0.); }
  
  inline G4ThreeVector BaseGeometry::GenerateVertex(G4int) const
  { return G4ThreeVector(); }

  inline void BaseGeometry::SetSpan(G4double s)
  { _span = s; }
  
  inline G4double BaseGeometry::GetSpan()
  { return _span; }
    
  
} // end namespace nexus

#endif
