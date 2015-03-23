// ----------------------------------------------------------------------------
///  \file   GeometryFactory.h
///  \brief  Factory class to create geometries chosen by the user
/// 
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     13 March 2013
///  \version  $Id$
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef GEOMETRY_FACTORY_H
#define GEOMETRY_FACTORY_H 

#include <G4String.hh>

class G4GenericMessenger;


namespace nexus {

  class BaseGeometry;


  /// Factory class to handle the creation of geometries according
  /// to the user's input via messenger commands.

  class GeometryFactory
  {
  public:
    GeometryFactory();
    ~GeometryFactory();

    BaseGeometry* CreateGeometry() const;

  private:
    G4GenericMessenger* _msg;
    G4String _name;
  };

}

#endif
