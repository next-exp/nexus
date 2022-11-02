// -----------------------------------------------------------------------------
//  nexus | GenericWLSFiber.h
//
//  Geometry of a configurable wavelength shifting optical fiber.
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
    // Constructor for a generic wavelength shifting optical fiber
    GenericWLSFiber(G4String    name,
                    G4bool      isround,            // "round"  or "square"
                    G4double    thickness,        // diameter or side
                    G4double    length,
                    G4bool      doubleclad,
                    G4bool      with_coating,
                    G4Material* coating_material,
                    G4Material* core_material,
                    G4bool      visibility);

    // Destructor
    ~GenericWLSFiber();

    //
    void Construct();

    // Getters
    const G4String& GetName()          const;
    const G4bool&   GetShape()         const;
    G4double        GetThickness()     const;
    G4double        GetLength()        const;
    G4bool          GetNumCladdings()  const;

    void SetCoatingOpticalProperties(G4MaterialPropertiesTable* ctmp);
    void SetCoreOpticalProperties(G4MaterialPropertiesTable* crmp);

    // Setters
    void SetVisibility(G4bool visibility);

  private:

    void DefineMaterials();
    void ComputeDimensions();
    void BuildRoundFiber();
    void BuildSquareFiber();

    G4String    name_;
    G4bool      isround_;
    G4double    thickness_;
    G4double    length_;
    G4double    core_rad_;
    G4double    iclad_rad_;
    G4double    oclad_rad_;
    G4bool      doubleclad_;
    G4bool      with_coating_;
    G4Material* core_mat_;
    G4Material* iclad_mat_;
    G4Material* oclad_mat_;
    G4Material* coating_mat_;
    G4MaterialPropertiesTable* coating_optProp_;
    G4MaterialPropertiesTable* core_optProp_;

    G4bool      visibility_;
  };


  inline const G4String& GenericWLSFiber::GetName()  const { return name_;  }
  inline const G4bool& GenericWLSFiber::GetShape() const { return isround_; }
  inline G4double GenericWLSFiber::GetThickness()    const { return thickness_; }
  inline G4double GenericWLSFiber::GetLength()       const { return length_; }
  inline G4bool   GenericWLSFiber::GetNumCladdings() const { return doubleclad_; }

  inline void GenericWLSFiber::SetVisibility(G4bool visibility)
  { visibility_ = visibility; }
  inline void GenericWLSFiber::SetCoatingOpticalProperties(G4MaterialPropertiesTable* ctmp)
  { coating_optProp_ = ctmp; }
  inline void GenericWLSFiber::SetCoreOpticalProperties(G4MaterialPropertiesTable* crmp)
  { core_optProp_ = crmp; }

} // namespace nexus

#endif
