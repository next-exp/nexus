// ----------------------------------------------------------------------------
///  \file   BhepUtils.h
///  \brief  
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>
///  \date     7 May 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration 
// ----------------------------------------------------------------------------

#ifndef __BHEP_UTILS__
#define __BHEP_UTILS__

#include <G4ThreeVector.hh>
#include <globals.hh>
#include <bhep/gstore.h>

namespace bhep { class particle; }


namespace nexus {

  /// Set of functions that perform common tasks (mainly on the bhep 
  /// transient objects). They are all static methods defined under a
  /// stateless class.

  class BhepUtils 
  {
  public:
    
    /// Return bhep particle from transient event identified
    /// by its G4TrackID
    static bhep::particle& GetBParticle(G4int G4TrackID);

    ///
    static void OpenOutputDst(const G4String& dst_name);
    ///
    static void CloseOutputDst();
    
    static G4ThreeVector DVto3Vector(const bhep::vdouble&);

  private:
    /// Constructor and destructor are hidden. No instance of this class
    /// can be created.
    BhepUtils();
    ~BhepUtils();
  };

} // end namespace nexus

#endif
