// ----------------------------------------------------------------------------
///  \file   GeneratorFactory.h
///  \brief  
/// 
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     13 March 2013
///  \version  $Id$
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __GENERATOR_FACTORY__
#define __GENERATOR_FACTORY__ 

#include <G4String.hh>

class G4VPrimaryGenerator;
class G4GenericMessenger;


namespace nexus {

  /// TODO. CLASS DESCRIPTION

  class GeneratorFactory
  {
  public:
    GeneratorFactory();
    ~GeneratorFactory();

    G4VPrimaryGenerator* CreateGenerator() const;

    // const G4String& GetGeneratorName() const;
    // void SetGeneratorName(const G4String&);

  private:
    G4GenericMessenger* _msg;
    G4String _name;
  };

  // INLINE DEFINITIONS ////////////////////////////////////

  // const G4String& GeneratorFactory::GetGeneratorName() const
  // { return _name; }

  // void GeneratorFactory::SetGeneratorName(const G4String& name)
  // { _name = name; }

}

#endif