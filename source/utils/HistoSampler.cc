#include "HistoSampler.h"
#include <G4Exception.hh>
#include <G4Voxelizer.hh>

void HistoSampler1D::create_cdf(){
  double integral = 0;
  double sum = 0;
  cdf.clear();
  nentries = h1G4->all_entries();
  auto axis = h1G4->axis();
  nbins = axis.bins();
  edges_x.reserve(nbins+1);
  cdf.reserve(nbins+1);
  size_t i;
  for (i = 0; i < nbins; i++)
    {
      double sig = h1G4->bin_height(i);
      if (sig<0) G4Exception("", "", FatalException, "Negative entries in hist");
      integral += sig;
    }
  cdf.push_back(0.);
  for (i = 0; i < nbins; i++)
    {
      sum += (h1G4->bin_height(i) /integral);
      cdf.push_back(sum);
    }
  for (i = 0; i < nbins; i++)
    {
      edges_x.push_back(axis.bin_lower_edge(i));
    }
  edges_x.push_back(axis.bin_upper_edge(nbins-1));
}

void HistoSampler2D::create_cdf(){
  double integral = 0;
  double sum = 0;
  cdf.clear();
  nentries = h2G4->all_entries();
  auto axis_x = h2G4->axis_x();
  auto axis_y = h2G4->axis_y();
  nbins_x = axis_x.bins();
  nbins_y = axis_y.bins();
  nbins = nbins_x*nbins_y;
  cdf.reserve(nbins+1);
  edges_x.reserve(nbins_x+1);
  edges_y.reserve(nbins_y+1);
  size_t i, j;
  for (i = 0; i < nbins_x; i++)
    for (j = 0; j < nbins_y; j++)
      {
	double sig = h2G4->bin_height(i, j);
	if (sig<0) G4Exception("", "", FatalException, "Negative entries in hist");
	integral += sig;
      }
  cdf.push_back(0.);
  for (i = 0; i < nbins_x; i++)
    for (j = 0; j < nbins_y; j++)
      {
	sum += (h2G4->bin_height(i, j) /integral);
	cdf.push_back(sum);
      }
  for (i = 0; i < nbins_x; i++)
    {
      edges_x.push_back(axis_x.bin_lower_edge(i));
    }
  edges_x.push_back(axis_x.bin_upper_edge(nbins_x-1));
  for (j = 0; j < nbins_y; j++)
    {
      edges_y.push_back(axis_y.bin_lower_edge(j));
    }
  edges_y.push_back(axis_y.bin_upper_edge(nbins_y-1));
  }

void HistoSampler3D::create_cdf(){
  double integral = 0;
  double sum = 0;
  nentries = h3G4->all_entries();
  auto axis_x = h3G4->axis_x();
  auto axis_y = h3G4->axis_y();
  auto axis_z = h3G4->axis_z();
  nbins_x = axis_x.bins();
  nbins_y = axis_y.bins();
  nbins_z = axis_z.bins();
  nbins = nbins_x*nbins_y*nbins_z;
  cdf.reserve(nbins+1);
  edges_x.reserve(nbins_x+1);
  edges_y.reserve(nbins_y+1);
  edges_z.reserve(nbins_y+1);
  size_t i, j, k;
  for (i = 0; i < nbins_x; i++)
    for (j = 0; j < nbins_y; j++)
      for (k = 0; k < nbins_z; k++)
	{
	  double sig = h3G4->bin_height(i, j, k);
	  if (sig<0) G4Exception("", "", FatalException, "Negative entries in hist");	  
	  integral += sig;
	}
  cdf.push_back(0.);
  for (i = 0; i < nbins_x; i++)
    for (j = 0; j < nbins_y; j++)
      for (k = 0; k < nbins_y; k++)
	{
	  sum += (h3G4->bin_height(i, j, k) /integral);
	  cdf.push_back(sum);
	}
  for (i = 0; i < nbins_x; i++)
    {
      edges_x.push_back(axis_x.bin_lower_edge(i));
    }
  edges_x.push_back(axis_x.bin_upper_edge(nbins_x-1));
  for (j = 0; j < nbins_y; j++)
    {
      edges_y.push_back(axis_y.bin_lower_edge(j));
    }
  edges_y.push_back(axis_y.bin_upper_edge(nbins_y-1));
  for (k = 0; k < nbins_z; k++)
    {
      edges_z.push_back(axis_z.bin_lower_edge(k));
    }
  edges_z.push_back(axis_z.bin_upper_edge(nbins_z-1));
}


void HistoSampler1D::sample(double &x, double r){
  if ((r < 0) || (r>1)){
    G4Exception("", "", FatalException, "Bad input");}
  if (nentries != h1G4->all_entries()) {create_cdf();}
  r = (r==1.0) ? 0.0 : r;
  auto i = G4Voxelizer::BinarySearch(cdf, r);
  x = edges_x[i];
  double delta = (r - cdf[i]) / (cdf[i + 1] - cdf[i]);
  x += delta * (edges_x[i+1]-edges_x[i]);
}


void HistoSampler2D::sample(double &x, double &y, double r1, double r2){
  if ((r1 < 0) || (r1>1) || (r2 < 0) || (r2>1)){
    G4Exception("", "", FatalException, "Bad input");}
  if (nentries != h2G4->all_entries()) {create_cdf();}
  r1 = (r1==1.0) ? 0.0 : r1;
  r2 = (r2==1.0) ? 0.0 : r2;
  auto k = G4Voxelizer::BinarySearch(cdf, r1);
  size_t i = k / nbins_y;
  size_t j = k - (i * nbins_y);
  x = edges_x[i];
  double delta = (r1 - cdf[k]) / (cdf[k + 1] - cdf[k]);
  x += delta * (edges_x[i+1]-edges_x[i]);
  y = edges_y[j] + r2 * (edges_y[j+1]-edges_y[j]);
}

void HistoSampler3D::sample(double &x, double &y, double &z, double r1, double r2, double r3){
  if ((r1 < 0) || (r1>1) || (r2 < 0) || (r2>1) || (r3 < 0) || (r3>1)){
    G4Exception("", "", FatalException, "Bad input");}
  if (nentries != h3G4->all_entries()) {create_cdf();}
  r1 = (r1==1.0) ? 0.0 : r1;
  r2 = (r2==1.0) ? 0.0 : r2;
  r3 = (r3==1.0) ? 0.0 : r3;
  auto l = G4Voxelizer::BinarySearch(cdf, r1);
  size_t i = l / (nbins_z*nbins_x);
  size_t j = (l/nbins_z)*(1-  (i *nbins_y));
  size_t k = l-nbins_z*(j+nbins_y*nbins_x*i);
  x = edges_x[i];
  double delta = (r1 - cdf[l]) / (cdf[l + 1] - cdf[l]);
  x += delta * (edges_x[i+1]-edges_x[i]);
  y = edges_y[j] + r2 * (edges_y[j+1]-edges_y[j]);
  z = edges_z[k] + r3 * (edges_z[k+1]-edges_z[k]);
}
