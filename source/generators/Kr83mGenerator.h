// ----------------------------------------------------------------------------
// nexus | Kr83mGenerator.h
//
// This class is the primary generator for the decay chain
// of the isomeric state krypton 83.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef Kr83m_GENERATOR_H
#define Kr83m_GENERATOR_H

#include <vector>
#include <G4VPrimaryGenerator.hh>

class G4Event;
class G4ParticleDefinition;
class G4GenericMessenger;

namespace nexus {

  class BaseGeometry;
 
  /// This state decays into the fundamental state of Kr 83 in two steps,
  ///  (JP 1/2- --> Jp 7/2+ -> 9/2+), with transition energies of 32.15 and 9.4 keV
  ///  The life time of 83mKr is long, ~ 1.83 hours, so, infinite for us,
  ///  while the life time of the intermediate state is relatively 
  /// short (154 ns), not completely negligible, since 
  /// it is ~ 1.5 time bin for the SiPMT..
  /// However, the recoil velocity of the Krypton atoms is small 
  /// enough such that the distance bewtween the two emission point point is only ~ 100 microns,
  /// assuming it keeps it's velocity constant during the life time of the 
  /// intermediate state. 
  /// This  <~ 100 micron distance is negligible for position resolution studies. 
  /// Thus, the gammas, X-ray and Electron conversion will be emitted 
  /// from the same vertex point.
  ///
  /// The Radioactivity data from Geant4 lists  these two excited states of 
  /// 83Kr as decaying 100% via electron conversion.  This is not quite correct, 
  /// following the TORI data sheet, at
  ///  (http://ie.lbl.gov/toi/nuclide.asp?iZA=360383) data sheet. 
  /// See also Phys Rev C 80, 045809 (2009) and 
  /// Nuclear Phys A194 (1972) 614-624 
  ///  The 9.4 keV state is both gamma and EC emittor. Unfortunate.  
  /// Also, I think we have to simulate X-ray transition following the 32 keV electron.  
  /// the EC. Since the path length of  12 and 14 keV x-ray is not strictly zero, 
  /// it makes life a bit more complicated...
  /// So, we have to simulated 2 or 3 particles for each decay..  
  
  class Kr83mGenerator: public G4VPrimaryGenerator
  {
  public:
    //Constructor
    Kr83mGenerator();
    //Destructor
    ~Kr83mGenerator();

    void GeneratePrimaryVertex(G4Event* evt);

  private:

    G4GenericMessenger* msg_;
    const BaseGeometry* geom_;

    G4double energy_32_; // Transition energy from the 1/2- state to the intermediate state, 7/2+ 
    G4double energy_9_; // ... from the JP 7/2+ to the Kr83 fundamental state. 
    G4double probGamma_9_; // ...The probability for a gamma instead of an EC conversion (related to the alpha EC) 
    G4double lifetime_9_; // ...The lifetime of the intermediate state.  
    std::vector<double> energy_Xrays_; // Energies of various X-ray, as the Kr83 atom relaxes to 
    std::vector<double> probability_Xrays_; // Probability to emit an X-ray of the above energy, per decay. 
                                            // We make cumulative, for easy access for random number. 

    G4String region_;
    G4ParticleDefinition*  particle_defgamma_;
    G4ParticleDefinition*  particle_defelectron_;
  };

}// end namespace nexus
#endif
