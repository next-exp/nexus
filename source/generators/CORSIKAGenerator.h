// ----------------------------------------------------------------------------
// nexus | CORSIKAGenerator.h
//
// This class is the primary generator of muons following the angular
// distribution at sea level. Angles are saved to be plotted later.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef MUON_GENERATOR_H
#define MUON_GENERATOR_H

#include <G4VPrimaryGenerator.hh>
#include <iostream>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;


namespace nexus {

  enum class Format { UNDEFINED, NORMAL, COMPACT };
  const std::map<unsigned int, int> corsikaToPdgId = {
    {1, 22},     // gamma
    {2, -11},    // e+
    {3, 11},     // e-
    {5, -13},    // mu+
    {6, 13},     // mu-
    {7, 111},    // pi0
    {8, 211},    // pi+
    {9, -211},   // pi-
    {10, 130},   // K0_L
    {11, 321},   // K+
    {12, -321},  // K-
    {13, 2112},  // n
    {14, 2212},  // p
    {15, -2212}, // pbar
    {16, 310},   // K0_S
    {17, 221},   // eta
    {18, 3122},  // Lambda
    {19, 3222},  // Sigma+
    {20, 3212},  // Sigma0
    {21, 3112},  // Sigma-
    {22, 3322},  // Cascade0
    {23, 3312},  // Cascade-
    {24, 3334},  // Omega-
    {25, -2112}, // nbar
    {26, -3122}, // Lambdabar
    {27, -3112}, // Sigma-bar
    {28, -3212}, // Sigma0bar
    {29, -3222}, // Sigma+bar
    {30, -3322}, // Cascade0bar
    {31, -3312}, // Cascade+bar
    {32, -3334}, // Omega+bar

    {50, 223},    // omega
    {51, 113},    // rho0
    {52, 213},    // rho+
    {53, -213},   // rho-
    {54, 2224},   // Delta++
    {55, 2214},   // Delta+
    {56, 2114},   // Delta0
    {57, 1114},   // Delta-
    {58, -2224},  // Delta--bar
    {59, -2214},  // Delta-bar
    {60, -2114},  // Delta0bar
    {61, -1114},  // Delta+bar
    {62, 10311},  // K*0
    {63, 10321},  // K*+
    {64, -10321}, // K*-
    {65, -10311}, // K*0bar
    {66, 12},     // nu_e
    {67, -12},    // nu_ebar
    {68, 14},     // nu_mu
    {69, -14},    // nu_mubar

    {116, 421},  // D0
    {117, 411},  // D+
    {118, -411}, // D-bar
    {119, -421}, // D0bar
    {120, 431},  // D+_s
    {121, -431}, // D-_sbar
    {122, 441},  // eta_c
    {123, 423},  // D*0
    {124, 413},  // D*+
    {125, -413}, // D*-bar
    {126, -423}, // D*0bar
    {127, 433},  // D*+_s
    {128, -433}, // D*-_s

    {130, 443}, // J/Psi
    {131, -15}, // tau+
    {132, 15},  // tau-
    {133, 16},  // nu_tau
    {134, -16}, // nu_taubar

    {137, 4122},  // Lambda+_c
    {138, 4232},  // Cascade+_c
    {139, 4132},  // Cascade0_c
    {140, 4222},  // Sigma++_c
    {141, 4212},  // Sigma+_c
    {142, 4112},  // Sigma0_c
    {143, 4322},  // Cascade'+_c
    {144, 4312},  // Cascade'0_c
    {145, 4332},  // Omega0_c
    {149, -4122}, // Lambda-_cbar
    {150, -4232}, // Cascade-_cbar
    {151, -4132}, // Cascade0_cbar
    {152, -4222}, // Sigma--_cbar
    {153, -4212}, // Sigma-_cbar
    {154, -4112}, // Sigma0_cbar
    {155, -4322}, // Cascade'-_cbar
    {156, -4312}, // Cascade'0_cbar
    {157, -4332}, // Omega0_cbar
    {161, 4224},  // Sigma*++_c
    {162, 1214},  // Sigma*+_c
    {163, 4114},  // Sigma*0_c

    {171, -4224},     // Sigma*--_cbar
    {172, -1214},     // Sigma*-_cbar
    {173, -4114},     // Sigma*0_cbar
    {176, 511},       // B0
    {177, 521},       // B+
    {178, -521},      // B-bar
    {179, -511},      // B0bar
    {180, 531},       // B0_s
    {181, -531},      // B0_sbar
    {182, 541},       // B+_c
    {183, -541},      // B-_cbar
    {184, 5122},      // Lambda0_b
    {185, 5112},      // Sigma-_b
    {186, 5222},      // Sigma+_b
    {187, 5232},      // Cascade0_b
    {188, 5132},      // Cascade-_b
    {189, 5332},      // Omega-_b
    {190, -5112},     // Lambda0_bbar
    {191, -5222},     // Sigma+_bbar
    {192, -5112},     // Sigma-_bbar
    {193, -5232},     // Cascade0_bbar
    {194, -5132},     // Cascade+_bbar
    {195, -5332},      // Omega+_bbar

    {201, 1000010020}, // Deuteron
    {301, 1000010030}, // Tritium
    {402, 1000020040}, // alpha
    {5626, 1000260560}, // Iron
    {1206, 1000080120}, // Carbon
    {1407, 1000070140}, // Nitrogen
    {1608, 1000080160}, // Oxygen
    {2713, 1000130270}, // Aluminum
    {3216, 1000160320}, // Sulfur
    {2814, 1000140280}, // Silicon
    {9900, 22}          // Cherenkov gamma
  };

  class GeometryBase;

  class CORSIKAGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    CORSIKAGenerator();
    /// Destructor
    ~CORSIKAGenerator();

    /// This method is invoked at the beginning of the event. It sets
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);
    void OpenFile(G4String);
    double WrapVarBoxNo(const float var, const float low, const float high, int &boxno);

  private:

  private:
    static constexpr unsigned fbsize_words_ = 5733 + 2;

    union {
      float fl[fbsize_words_];
      unsigned in[fbsize_words_];
      char ch[sizeof(fl[0])*fbsize_words_];
    } buf_;

    Format infmt_ = Format::UNDEFINED;
    G4GenericMessenger* msg_;

    G4String region_;
    G4String corsika_file_;
    G4double height_;
    G4double plane_x_size_;
    G4double plane_y_size_;
    G4bool is_open_ = false;
    unsigned run_number_ = 0;
    unsigned current_event_number_ = 0;
    unsigned event_count_ = 0;

    std::ifstream *input_;

    const GeometryBase* geom_; ///< Pointer to the detector geometry

  };

} // end namespace nexus

#endif
