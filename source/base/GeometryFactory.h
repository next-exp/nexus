// ----------------------------------------------------------------------------
// nexus | GeometryFactory.h
//
// This class instantiates the main geometry of the simulation that the user
// specifies via configuration parameter.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef GEOMETRY_FACTORY_H
#define GEOMETRY_FACTORY_H

#include <G4String.hh>

class G4GenericMessenger;


namespace nexus {

  class BaseGeometry;

  class GeometryFactory
  {
  public:
    GeometryFactory();
    ~GeometryFactory();

    BaseGeometry* CreateGeometry() const;

  private:
    G4GenericMessenger* msg_;
    G4String name_;
  };

}

#endif
