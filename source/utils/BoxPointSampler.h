// ----------------------------------------------------------------------------
///  \file   BoxPointSampler.h
///  \brief  Sampler of random uniform points in a box-shaped volume.
///
///  \author   Javier Munyoz Vidal <jmunoz@ific.uv.es>
///  \date     2 November 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef BOX_POINT_SAMPLER_H
#define BOX_POINT_SAMPLER_H

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>


namespace nexus {

  /// Sampler of random uniform points in a box-shaped volume

  class BoxPointSampler
  {
  public:
    /// Constructor
    BoxPointSampler(G4double inner_x, G4double inner_y, G4double inner_z, 
		                G4double thickness,
          		      G4ThreeVector origin=G4ThreeVector(0.,0.,0.),
		                G4RotationMatrix* rotation=0);
    
    /// Destructor
    ~BoxPointSampler();
    
    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region);
    
  private:
    G4double GetLength(G4double origin, G4double max_length);
    G4ThreeVector RotateAndTranslate(G4ThreeVector position);
						    
  private:
    G4double inner_x_, inner_y_, inner_z_; ///< Internal dimensions
    G4double outer_x_, outer_y_, outer_z_; ///< External dimensions
    G4double thickness_; ///< Walls thickness
 
    G4double perc_Zvol_, perc_Yvol_;   ///< Faces volumes percentages
    G4double perc_Zsurf_, perc_Ysurf_; ///< Faces surfaces percentages

    G4ThreeVector origin_;
    G4RotationMatrix* rotation_;
  };

} // namespace nexus

#endif
