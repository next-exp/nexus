// ----------------------------------------------------------------------------
///  \file   GeneratorFactory.h
///  \brief  Factory class for the construction of primary generators.
/// 
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     13 March 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef GENERATOR_FACTORY_H
#define GENERATOR_FACTORY_H

#include <G4String.hh>

class G4VPrimaryGenerator;
class G4GenericMessenger;


namespace nexus {

  /// Factory class for the construction of primary generators
  /// chosen by the user via messenger.

  class GeneratorFactory
  {
  public:
    /// Constructor. Defines the messenger commands.
    GeneratorFactory();
    /// Destructor
    ~GeneratorFactory();
    /// Returns an instance of the chosen generator
    G4VPrimaryGenerator* CreateGenerator() const;

  private:
    G4GenericMessenger* _msg; ///< Pointer to the messenger
    G4String _name; ///< Name of the chosen primary generator
  };

} // end namespace nexus

#endif
