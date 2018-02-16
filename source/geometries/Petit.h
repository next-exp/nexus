#ifndef PETIT_H
#define PETIT_H

#include "BaseGeometry.h"
#include <TFile.h>

class G4GenericMessenger;
namespace nexus {class PetitModule;}

namespace nexus {
    
    class Petit: public BaseGeometry
    {
    public:
        /// Constructor
        Petit();
        /// Destructor
        ~Petit();
        
        /// Return vertex within region <region> of the chamber
        virtual G4ThreeVector GenerateVertex(const G4String& region) const;

        virtual void Construct();

    private:

        G4double lab_side_;
        G4double dist_between_cells_;

        G4double specific_vertex_X_;
        G4double specific_vertex_Y_;
        G4double specific_vertex_Z_;

        /// Messenger for the definition of control commands
        G4GenericMessenger* msg_;

        PetitModule* module_;

    };


} // end namespace nexus

#endif
