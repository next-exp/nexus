// ----------------------------------------------------------------------------
///  \file   ELLookupTableGenerator.h
///  \brief  
///  
///  \author   <justo.martin-albo@ific.uv.es>    
///  \date     13 December 2011
///  \version  $Id$
///
///  Copyright (c) 2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __EL_LOOKUP_TABLE_GENERATOR__
#define __EL_LOOKUP_TABLE_GENERATOR__

#include <G4VPrimaryGenerator.hh>

class G4Event;


namespace nexus {
  
  class BaseGeometry;
  

  /// FIXME. Class description.

  class ELLookupTableGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    ELLookupTableGenerator();
    /// Destructor
    ~ELLookupTableGenerator();
    
    /// This method is invoked at the beginning of the event. It sets 
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);
    
  private:
    const BaseGeometry* _geom;
  };

} // end namespace nexus

#endif
