// ----------------------------------------------------------------------------
///  \file   Utils.h
///  \brief  Utility class with functions that do not fit in any other place.
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>
///  \date     7 May 2009
///  \version  $Id$  
///
///  Copyright (c) 2009 NEXT Collaboration 
// ----------------------------------------------------------------------------

#ifndef __UTILS__
#define __UTILS__

#include <G4ThreeVector.hh>
#include <globals.hh>
#include <bhep/gstore.h>

namespace bhep { class particle; }


namespace nexus {

  /// Set of functions that perform common tasks (mainly on the bhep 
  /// transient objects). They are all static methods defined under a
  /// stateless class.

  class Utils 
  {
  public:
    
    /// Return bhep particle from transient event identified
    /// by its G4TrackID
    static bhep::particle& GetBParticle(G4int G4TrackID);

    ///
    static void OpenOutputDst(const G4String& dst_name);
    ///
    static void CloseOutputDst();
    
    static G4ThreeVector DVto3Vec(const bhep::vdouble&);

  private:
    // Ctors, dtor and assig-operator are hidden
    Utils();
    ~Utils();
    Utils(Utils const&);
    Utils& operator=(Utils const&);
  };

} // end namespace nexus

#endif
