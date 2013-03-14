// ----------------------------------------------------------------------------
///  \file   GeometryFactory.h
///  \brief  
/// 
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     13 March 2013
///  \version  $Id$
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __GEOMETRY_FACTORY__
#define __GEOMETRY_FACTORY__ 

#include <G4String.hh>

class G4GenericMessenger;


namespace nexus {

  class BaseGeometry;


  /// TODO. CLASS DESCRIPTION

  class GeometryFactory
  {
  public:
    GeometryFactory();
    ~GeometryFactory();

    BaseGeometry* CreateGeometry() const;

    // const G4String& GetGeometryName() const;
    // void SetGeometryName(const G4String&);

  private:
    G4GenericMessenger* _msg;
    G4String _name;
  };

  // INLINE DEFINITIONS ////////////////////////////////////

  // const G4String& GeometryFactory::GetGeometryName() const
  // { return _name; }

  // void GeometryFactory::SetGeometryName(const G4String& name)
  // { _name = name; }

}

#endif