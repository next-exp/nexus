// ----------------------------------------------------------------------------
///  \file   Decay0.h, a translation of the FORTRAN program decay0
///          written by V.I.Tretyak et al
///          Please refer to: O.A.Ponkratenko, V.I.Tretyak, Yu.G.Zdesenko,
///                  "Event Generator DECAY4 for Simulating Double-Beta 
///                  Processes and Decays of Radioactive Nuclei",
///                  Phys. At. Nucl. 63 (2000) 1282 (nucl-ex/0104018).
///  \brief  Translation of Decay0
///  
///  \author   <lebrun@fnal.gov>
///  \date     Jan 23-31 2016
///  \version  $Id$
///
///  Copyright (c) 2016-2018 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef DECAY0_H
#define DECAY0_H

#include <fstream>
#include <vector>
#include <string>
#include <gsl/gsl_integration.h>

struct decay0Part {
  int _pdgCode;
  double _pmom[3];
  double _energy; // kinetic
  double _time;
};

class bbFinalState {
  public:
    bbFinalState();
    bbFinalState(int spin, int keyNum, double ELevel, int itr2); // I don't know what this key number is, it appear only on listing. 
    int _spin;
    int _key; // ??? 
    double _levelE; // binding energy of electron on K shell of parent atom
    int _itrans02; // flag for Quadrupole transition? 
};

class bbNuclide {
  public: 
    std::string _name;
    double _Qbb;
    float _Zdbb;
    float _Adbb;
    float _EK; // Not sure what that is...  
    std::vector<bbFinalState> _allFS; // Allowed final states.
};

struct eta_nme {
  float _chi_GTw, _chi_Fw, _chip_GT, _chip_F, _chip_T,
                  _chip_P, _chip_R;
}; // only for mode 18, currently not supported
 
class decay0 {

  public:
  
     decay0();
     decay0(const std::string nuclide, int finalStateNumber, int decayModeNumber,
                 double eRangeLow=0.0, double eRangeHigh=4.3); // no limits, be default. (for 2nbbdecay. )
     ~decay0();
    void decay0DoIt(std::vector<decay0Part> &outPart) const ;
    void fillInfo(); // to be used if the Nuclide, final state or decay mode is changed...Not advised..
     void printDecayModeList(); // For reference   
     
  
  private:
    bool _ready; // Initialize, ready to decay 
    double _emass; // electron mass 
    std::string _nuclideName;
    size_t _fsNum;
    size_t _modebb;
    size_t _modebbOld;
    bbNuclide _bbNucl; // Current choice of nuclide 
    bbFinalState _fsFinalState;
    std::vector<std::string> _knownbbEmitters;
    // Internal variable once a bb emitter has been selected. Variable names are those from decay0, F77 version. 
    double _levelE;
    std::vector<double> _spthe1;
    double _spmax; 
    float _dataMasses[4]; // only for, we don't simulate muons, hadrons, etc here. 
    float _dens, _denf; // in common/denrange/
    double _toallevents; // Normalization of the total decay probability: 
//         toallevents         - coefficient to calculate the corresponding
//                               number of bb events in full range of energies:
//                               full_number=generated_number*toallevents
//                               (for modes 4,5,6,8,10 and 13).
    int _mode; //  in common/denrange/
    gsl_integration_workspace *_gwk;  // For integration..  
//    eta_nme  .. not supported yet... 
    //
    // Internal variable, volatile all declared mutable. Filled and used in DoIt (subroutine bb in decay 0)  
    //
    mutable double _e0;
    mutable double _e1;
    mutable double _ebb1;
    mutable double _ebb2;
    mutable std::vector<double> _spthe2;
    
    void initSpectrum(); // Called from fillInfo, initialize array for matrix element, kinematics and so forth.
    void decay0DoItbb(std::vector<decay0Part> &outPart) const; // Main method, generate the two electrons. 
    void Ba136low(std::vector<decay0Part> &outPart) const;  // Baryum 136 de-excitation.
//    void Xe130low(std::vector<decay0Part> &outPart) const;  // Xenon de-excitation. // we (NEXT) don't care... 
     
    void nucltransK(std::vector<decay0Part> &outPart, const double eGam, const double eBind, 
                      const double conve, const double convp, const double tclev, const double thlev) const;
    void outGamma(std::vector<decay0Part> &outPart, const double eGam, const double tclev, const double thlev) const;
    void outElectron(std::vector<decay0Part> &outPart, const double e, const double tclev, const double thlev) const;
    void outPair(std::vector<decay0Part> &outPart, const double e, const double tclev, const double thlev) const;
    
    double timedParticle (std::vector<decay0Part> &outPart, int indexP, const double E1,
                           const double E2, const double teta1, const double teta2, 
			   const double phi1, const double phi2, const double tclev,
                           const double thlev) const; // return the time at which the particle(s) are produced
			   
    double fe12_mod18(double x, void *p, eta_nme &etaThing); // currently not supported. 		   
//
// Static function used to describe spectrum and related utilities. 			   
    static double fe1_mod1(double x, void *p);		   
    static double fe1_mod2(double x, void *p);		   
    static double fe1_mod3(double x, void *p);		   
    static double fe12_mod4(double x, void *p);		   
    static double fe2_mod4(double x, void *p);		   
    static double fe12_mod5(double x, void *p);		   
    static double fe2_mod5(double x, void *p);		   
    static double fe12_mod6(double x, void *p);		   
    static double fe2_mod6(double x, void *p);		   
    static double fe1_mod7(double x, void *p);		   
    static double fe12_mod7(double x, void *p);		   
    static double fe12_mod8(double x, void *p);		   
    static double fe2_mod8(double x, void *p);		   
    static double fe1_mod10(double x, void *p);		   
    static double fe12_mod10(double x, void *p);		   
    static double fe12_mod13(double x, void *p);		   
    static double fe2_mod13(double x, void *p);		   
    static double fe12_mod14(double x, void *p);		   
    static double fe2_mod14(double x, void *p);		   
    static double fe12_mod15(double x, void *p);		   
    static double fe2_mod15(double x, void *p);		   
    static double fe12_mod16(double x, void *p);		   
    static double fe2_mod16(double x, void *p);		   
    static double fe1_mod17(double x, void *p);		   
    static double fe12_mod17(double x, void *p);		   
    static double dshelp1(double x, void *p);		   
    static double dshelp2(double x, void *p);		   
    static double fermi(double z, double e);
    		   
  public:
  
    inline void SetNuclide(std::string nucl) { _nuclideName = nucl;}
    inline void SetFinalStateNumber(size_t n) { _fsNum = n;}
    inline void SetDecayModeNumber(size_t n) { _modebb = n;}
    inline void SetEnergyRangerForEnergySum(double e1, double e2) { 
        _ebb1 = e1; _ebb2=e2;
	size_t nnE1= static_cast<size_t> (e1*1000.) + 1;
	_spthe1.resize(nnE1);
	size_t nnE2= static_cast<size_t> (e2*1000.) + 1;
	_spthe2.resize(nnE2);
    } // Advanced option ?
    inline std::string GetNuclide() const { return _nuclideName;}
    inline size_t GetFinalStateNumber() { return _fsNum;}
    inline size_t GetDecayModeNumber() { return _modebb;}
    inline double GetEffectiveRatioToOfEvents() {return _toallevents; } 

};
#endif
