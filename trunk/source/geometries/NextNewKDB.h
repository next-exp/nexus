// ----------------------------------------------------------------------------
///  \file   NextNewKDB.h
///  \brief  Geometry of the Kapton Dice Boards used in the NEW detector.
///
///  \author  <miquel.nebot@ific.uv.es>
///  \date    18 Sept 2013
///  \version $Id$
//
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef NEXT_NEW_KDB_H
#define NEXT_NEW_KDB_H

#include "BaseGeometry.h"
#include <vector>


namespace nexus {

  /// Geometry of the Kapton Dice Boards (KDB) used in the NEW detector

  class NextNewKDB: public BaseGeometry
  {
  public:
    /// Constructor
    NextNewKDB(G4int rows, G4int columns);
    /// Destructor
    ~NextNewKDB();

    G4ThreeVector GetDimensions() const;
    const std::vector<std::pair<int, G4ThreeVector> >& GetPositions();

    /// Builder
    virtual void Construct();

  private:
    G4int _rows, _columns;
    G4ThreeVector _dimensions;
    std::vector<std::pair<int, G4ThreeVector> > _positions;
  };

} // end namespace nexus

#endif
