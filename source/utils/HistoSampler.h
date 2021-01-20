#ifndef HISTO_SAMPLER_H
#define HISTO_SAMPLER_H
#include <vector>
#include <G4RunManager.hh>
#include <g4csv.hh>

class HistoSampler1D{
  unsigned int nbins    = 0;
  unsigned int nentries = 0;
  unsigned int nbins_x  = 0;
  const tools::histo::h1d * h1G4;
  std::vector<double> cdf;
  std::vector<double> edges_x;
  void create_cdf();
public:
  HistoSampler1D(const tools::histo::h1d * hG4):h1G4(hG4){};
  void sample(double &x, double r);
};

class HistoSampler2D{
  unsigned int nbins    = 0;
  unsigned int nentries = 0;
  unsigned int nbins_x  = 0;
  unsigned int nbins_y  = 0;
  const tools::histo::h2d * h2G4;
  std::vector<double> cdf;
  std::vector<double> edges_x;
  std::vector<double> edges_y;
  void create_cdf();
public:
  HistoSampler2D(const tools::histo::h2d * hG4):h2G4(hG4){};
  void sample(double &x, double &y, double r1, double r2);
};

class HistoSampler3D{
  unsigned int nbins    = 0;
  unsigned int nentries = 0;
  unsigned int nbins_x  = 0;
  unsigned int nbins_y  = 0;
  unsigned int nbins_z  = 0;
  const tools::histo::h3d * h3G4;
  std::vector<double> cdf;
  std::vector<double> edges_x;
  std::vector<double> edges_y;
  std::vector<double> edges_z;
  void create_cdf();
public:
  HistoSampler3D(const tools::histo::h3d * hG4):h3G4(hG4){};
  void sample(double &x, double &y, double &z, double r1,double r2, double r3);
};
#endif
