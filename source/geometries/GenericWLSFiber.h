// -----------------------------------------------------------------------------
//  nexus | GenericWLSFiber.h
//
//  Geometry of a configurable wave-length shifting optical fiber.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef GENERIC_WLS_FIBER_H
#define GENERIC_WLS_FIBER_H

#include "GeometryBase.h"
#include <G4MaterialPropertyVector.hh>

class G4Material;
class G4GenericMessenger;
class G4MaterialPropertiesTable;

namespace nexus {

  class GenericWLSFiber: public GeometryBase
  {

  public:
    // Constructor for a generic wave-length shifting optical fiber
    GenericWLSFiber(G4String    name,
                    G4String    shape,            // "round"  or "square"
                    G4double    thickness,        // diameter or side
                    G4double    length,
                    G4int       num_claddings,
                    G4bool      with_coating,
                    G4Material* core_material,
                    G4bool      visibility);

    // Destructor
    ~GenericWLSFiber();

    //
    void Construct();

    // Getters
    const G4String& GetName()          const;
    const G4String& GetShape()         const;
    G4double        GetThickness()     const;
    G4double        GetLength()        const;
    G4int           GetNumCladdings()  const;

    // Setters
    void SetVisibility(G4bool visibility);

  private:

    void DefineMaterials();
    void ComputeDimensions();
    void BuildRoundFiber();
    void BuildSquareFiber();

    G4String    name_;
    G4String    shape_;
    G4double    thickness_;
    G4double    length_;
    G4double    core_rad_;
    G4double    iclad_rad_;
    G4double    oclad_rad_;
    G4int       num_claddings_;
    G4bool      with_coating_;
    G4Material* core_mat_;
    G4Material* iclad_mat_;
    G4Material* oclad_mat_;
    G4Material* coating_mat_;    

    G4bool      visibility_;
  };


  inline const G4String& GenericWLSFiber::GetName()  const { return name_;  }
  inline const G4String& GenericWLSFiber::GetShape() const { return shape_; }
  inline G4double GenericWLSFiber::GetThickness()    const { return thickness_; }
  inline G4double GenericWLSFiber::GetLength()       const { return length_; }
  inline G4int    GenericWLSFiber::GetNumCladdings() const { return num_claddings_; }

  inline void GenericWLSFiber::SetVisibility(G4bool visibility)
  { visibility_ = visibility; }

} // namespace nexus

#endif
