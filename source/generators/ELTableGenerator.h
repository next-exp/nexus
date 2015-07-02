// ----------------------------------------------------------------------------
///  \file   ELTableGenerator.h
///  \brief  Primary generator for the production of EL lookup tables.
///  
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     2 July 2015
///
///  Copyright (c) 2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef EL_TABLE_GENERATOR_H
#define EL_TABLE_GENERATOR_H

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;


namespace nexus {

  class BaseGeometry;

  
  class ELTableGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    ELTableGenerator();
    /// Destructor
    ~ELTableGenerator();
    
    /// This method is invoked at the beginning of the event. It sets 
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);

  private:
    G4GenericMessenger* _msg; ///< Pointer to UI messenger
    const BaseGeometry* _geom; ///< Pointer to the detector geometry

    G4int _num_ie;
  };

} // end namespace nexus

#endif
