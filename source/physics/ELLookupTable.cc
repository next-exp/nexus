// ----------------------------------------------------------------------------
// nexus | ELLookupTable.cc
//
// This class describes the generation of the EL light.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ELLookupTable.h"

#include <fstream>



namespace nexus {


  ELLookupTable::ELLookupTable(G4String filename)
  {
    // read the text files and store their content in the transient table
    ReadFiles(filename);
  }



  ELLookupTable::~ELLookupTable()
  {
  }



  void ELLookupTable::ReadFiles(G4String filename)
  {
    // Open the file containing the light table
    std::ifstream file(filename, std::ifstream::out);

    if (!file.is_open())
      {}
      //G4Exception("[ELLookupTable] ERROR: cannot open input file!");

    // Deal with the header file
    G4String line;

    do {
      getline(file, line);
    } while(line[0] == '*');

    // Read file and store content in the transient table

    int num_bins = 5;
    int last_point_id = 0;
    std::map<int, std::vector<double> > sensor_map;

    while (!file.eof()) {

      G4int point_id, sensor_id;
      std::vector<double> probs;

      file >> point_id >> sensor_id;

      for (G4int i=0; i<num_bins; i++) {
	G4double prob;
	file >> prob;
	probs.push_back(prob);
      }

      sensor_map.insert(std::make_pair(sensor_id, probs));

      if (point_id != last_point_id) {
	ELtable_.push_back(sensor_map);
      }

      last_point_id = point_id;
    }
  }



  const std::map<int, std::vector<double> >&
  ELLookupTable::GetSensorsMap(const G4ThreeVector& hitpos)
  {
    /// The EL points must be in the middle of the bins.
    double radius = 92.5; // mm
    double binning = 5.; // mm
    int maxidx = radius*2./binning + 1;
    /// If the number of bins per axis is odd, a different math must be applied
    bool even = false;
    if ((maxidx/2.-floor(maxidx/2.)) == 0.){
      even = true;
    }
    std::vector<double> bincenters;
    std::vector<int> content;
    /// Fill a vector with the coordinates of the center of bins (they are the same in
    /// x and y, because it is a regular squared grid)
    for (int i=0; i<maxidx; i++){
      double bincenter = -binning*(maxidx/2.) + binning/2.+ i*binning;
      bincenters.push_back(bincenter);
    }
    /// For every coordinate in x, a column is built with a number of bins equal
    /// to the number of EL points which have that x. Remember that only the points
    /// which falls inside a circle of a fixed radius are taken into account,
    /// so columns have not all the same number of points

    /// Content of elements for every column in the grid
    std::vector<int> columns;
    if (even){
      for (int i=0; i<maxidx; i++){
	double y = sqrt(radius*radius - bincenters[i]*bincenters[i]);
	double col = 0;
	///If the y coord of the circle falls further than the center of the bin,
	///that bin is included, otherwise it isn't.
	if ((y/binning) - floor(y/binning)<0.5){
	  col = floor(y/binning)*2.;
	} else {
	  col = ceil(y/binning)*2.;
	}
	columns.push_back(col);
      }
    } else {
      columns.push_back(0);
      for (int i=1; i<maxidx-1; i++){
	double y = sqrt(radius*radius - bincenters[i]*bincenters[i]);
	double col = 0;
	///If the y coord of the circle falls further than the center of the bin,
	///that bin is included, otherwise it isn't.
	if ((y-binning/2.)/binning - floor((y-binning/2.)/binning)<0.5){
	  col = floor((y-binning/2.)/binning)*2.+1;
	} else {
	  if (y < radius){
	    col = ceil((y-binning/2.)/binning)*2.+1;
	  } else {
	    col = ceil((y-binning/2.)/binning)*2.-1;
	  }
	}
	columns.push_back(col);
      }
      columns.push_back(0);
    }

    for (unsigned int i=0; i<columns.size(); i++){
      content.push_back(columns[i]);
    }

    int id;

    /// Maths to obtain the right ID number of the points
    int zero = maxidx/2;
    int binX = zero + ceil(hitpos[0]/5.);
    int binY = zero + ceil(hitpos[1]/5.);
    if (!even) {
      zero = floor(maxidx/2)+1;
      if (hitpos[0]/binning-floor(hitpos[0]/binning) >= 0.5){
	binX = zero + ceil(hitpos[0]/binning);
      } else {
	binX = zero + floor(hitpos[0]/binning);
      }
      if (hitpos[1]/binning-floor(hitpos[1]/binning) >= 0.5){
	binY = zero + ceil(hitpos[1]/binning);
      } else {
	binY = zero + floor(hitpos[1]/binning);
      }
    }
    // "binX-1" is due to the fact that content is a vector (starting from 0),
    // while binX and binY starts from 1

    // number of empty bins starting from below
    int base = (maxidx - content[binX-1])/2;

    // Check if the point is in a bin which does not correspond to any
    // EL point. In this case the closest bin is chosen.
    if (binY > base + content[binX-1] || binY < base+1){
      double centerX = bincenters[binX-1];
      double centerY = bincenters[binY-1];
      double min_dist = 1.E6;
      int min_xbin = 100;
      int min_ybin = 100;
      for (int i=0; i<maxidx; i++){
	double x = bincenters[i];
	for (int j=0; j<maxidx; j++){
	  double y = bincenters[j];
	  int bx = zero + ceil(x/5.);
	  int by = zero + ceil(y/5.);
	  int ped = (maxidx - content[bx-1])/2;;
	  if (!(by > ped + content[bx-1] || by < ped+1)){
	    double dist = sqrt(pow(centerX-x,2)+pow(centerY-y,2));
	    if (dist < min_dist){
	      min_dist = dist;
	      min_xbin = bx;
	      min_ybin = by;
	    }
	  }
	}
      }
      binX = min_xbin;
      binY = min_ybin;
      base = (maxidx - content[binX-1])/2;
    }

    int sum = 0;
    for (int i=0; i<binX-1; i++){
      sum = sum + content[i];
    }
    // The "-1" comes because the EL point IDs start from 0
    id = sum + binY - base - 1;

    return ELtable_[id];
  }


} // end namespace nexus
