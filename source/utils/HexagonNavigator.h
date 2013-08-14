// ----------------------------------------------------------------------------
///  \file   HexagonNavigator.h
///  \brief  Generator of vertices in a cylindric geometry.
///
///  \author   Francesc Monrabal Capilla <francesc.monrabal@ific.uv.es>
///  \date     7 May 2010
///  \version  $Id: Navigator.h 2598 2010-03-05 15:54:53Z jmalbos $
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __HEXAGON_NAVIGATOR__
#define __HEXAGON_NAVIGATOR__

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>
#include <globals.hh>

#include <vector>


namespace nexus {

  /// FIXME. Class description.

  class HexagonNavigator
  {
  public:
    /// Constructor
    HexagonNavigator(G4double inner_rad, G4double inner_length,
		     G4double binning=5.*mm,
		     G4ThreeVector origin=G4ThreeVector(0.,0.,0.),
		     G4RotationMatrix* rotation=0);
    /// Destructor
    ~HexagonNavigator();

    /// Returns vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region);

    /// Returns a vector with the positions of all the points needed
    /// to create the look-up tables
    std::vector<G4ThreeVector> TriangleWalker(G4double, G4double, G4double);

    /// Returns a vector with the positions of all the detectors
    /// of radius rmin placed in a hexagon of side rmax
    std::vector<G4ThreeVector> 
      SetDetector(G4double rmin, G4double rmax, G4double z = 0., 
		  G4bool fixed_rmax = 1);


    G4int i;

  private:
    G4double GetRadius(G4double inner, G4double outer);
    G4double GetPhi();
    G4double GetLength(G4double origin, G4double max_length);
    G4ThreeVector RotateAndTranslate(const G4ThreeVector&);

  private:
    /// Default constructor is hidden
    HexagonNavigator();

  private:
    G4double _inner_length, _inner_radius;        ///< Internal dimensions

    G4double _perc_body_surf, _perc_body_vol; ///< Cylinder body percentages
    G4double _binning;
    G4ThreeVector _origin;

    G4RotationMatrix* _rotation;
    std::vector<G4ThreeVector> _vertexes;
    G4int _number_events;
  };

  
} // namespace nexus

#endif
