// ----------------------------------------------------------------------------
///  \file   Next1EL.h
///  \brief  
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     16 Feb 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT1_EL__
#define __NEXT1_EL__

#include "BaseGeometry.h"


namespace nexus {


  class Next1EL: public BaseGeometry
  {
  public:
    /// constructor
    Next1EL();
    /// destructor
    ~Next1EL();

    void DefineGeometry();
  };


} // end namespace nexus

#endif
