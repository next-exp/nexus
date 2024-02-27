// ----------------------------------------------------------------------------
// nexus | PolygonPointSampler.cc
//
// This class is a sampler of random uniform points in a polygon.
// This is designed for sampling inside the light tube with n sides
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PolygonPointSampler.h"

#include <G4VPhysicalVolume.hh>
#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4VSolid.hh>
#include <G4PhysicalConstants.hh>
#include <Randomize.hh>


namespace nexus {

  // Constructor following Geant4 dimensions convention
  PolygonPointSampler::PolygonPointSampler (
    G4double min_radius, G4double max_radius, G4double half_length,
    G4int n_sides, G4RotationMatrix* rotation,
    G4ThreeVector origin ) :
    min_radius_(min_radius),
    max_radius_(max_radius),
    half_length_(half_length),
    n_sides_(n_sides),
    rotation_(rotation),
    origin_(origin)
  {

    InitalizePolygons();

  }

  PolygonPointSampler::~PolygonPointSampler()
  {
  }

  void PolygonPointSampler::InitalizePolygons(){


    // Plot the regular polygon
    std::vector<G4double> theta(n_sides_);
    for (G4int i = 0; i < n_sides_; ++i) {
        theta[i] = 2 * M_PI * i / n_sides_;
    }

    for (int i = 0; i < n_sides_; ++i) {
        polygon_inner_.push_back({min_radius_ * cos(theta[i]) + origin_.x(), min_radius_ * sin(theta[i]) + origin_.y()});
        polygon_outer_.push_back({max_radius_ * cos(theta[i]) + origin_.x(), max_radius_ * sin(theta[i]) + origin_.y()});

    }

  }


  G4ThreeVector PolygonPointSampler::GenerateVertex(const vtx_region& region)
  {
    G4double x = 0.;
    G4double y = 0.;
    G4double z = 0.;

    // Center of the chamber
    if (region == CENTER) {
      x = y = z = 0.;
    }

    // Center of the chamber
    if (region == INSIDE) {
      // sample position based on interior radius
      G4bool valid_sample = false;
      std::vector<G4double>  point;
      while (!valid_sample){
        point = {2.0 * min_radius_ * (G4UniformRand() - 0.5), 2.0 * min_radius_ * (G4UniformRand() - 0.5)};
        valid_sample= true;
        valid_sample = CheckXYBoundsPolygon(point, polygon_inner_);
      }

      x = point[0];
      y = point[1];
      z = GetLength(half_length_);
    }

    // Generating from inside the cylinder (between min_radius and max_radius)
    else if (region == VOLUME) {

      // sample position based on interior radius
      G4bool valid_sample = false;
      std::vector<G4double>  point;

      while (!valid_sample){
        point = {2.0 * max_radius_ * (G4UniformRand() - 0.5),
                 2.0 * max_radius_ * (G4UniformRand() - 0.5)};
        G4bool inside_inner = CheckXYBoundsPolygon(point, polygon_inner_);
        G4bool inside_outer = CheckXYBoundsPolygon(point, polygon_outer_);

        // Want outside inner and inside outter
        if (!inside_inner && inside_outer)
          valid_sample = true;
      }

      x = point[0];
      y = point[1];
      z = GetLength(half_length_);
    }

    // Generating from the INNER surface
    else if (region == INNER_SURF) {

      // sample position based on interior radius
      std::vector<G4double> point = SampleXYonPolygonEdge(min_radius_);
      x = point[0];
      y = point[1];
      z = GetLength(half_length_);
    }

    // Generating from the OUTER surface
    else if (region == OUTER_SURF) {
      // sample position based on exterior radius
      std::vector<G4double> point;
      point = SampleXYonPolygonEdge(max_radius_);
      x = point[0];
      y = point[1];
      z = GetLength(half_length_);
    }

    // Unknown region
    else {
      G4Exception("[PolygonPointSampler]", "GenerateVertex()", FatalErrorInArgument,
                  "Unknown vertex region! Possible are VOLUME, INNER_SURF and OUTER_SURF");
    }

    return RotateAndTranslate(G4ThreeVector(x, y, z));
  }

  G4double PolygonPointSampler::GetLength(G4double half_length)
  {
    return ((G4UniformRand() * 2.0 - 1.0) * half_length);
  }


  G4bool PolygonPointSampler::CheckXYBoundsPolygon(std::vector<G4double> point, std::vector<std::vector<G4double>> polygon){

    // This function checks if a point is inside a polygon (needed for checking if
    // inside the drift tube which has an octagonal-like shape).
    // It works by counting intersections of the point in the horizontal plane
    // as described in 
    // https://www.geeksforgeeks.org/how-to-check-if-a-given-point-lies-inside-a-polygon/
    

    G4double x = point[0], y = point[1];
    G4bool inside = false;

    // Store the first point in the polygon and initialize the second point
    std::vector<G4double> p1 = {polygon[0][0],polygon[0][1]};

    // Loop through each edge in the polygon
    for (G4int i = 1; i <= n_sides_; i++) {
      
      // Get the next point in the polygon
      std::vector<G4double> p2 ={polygon[i % n_sides_][0], polygon[i % n_sides_][1]};

      // 1. Check if the point is above the minimum y coordinate of the edge
      // 2. Check if the point is below the maximum y coordinate of the edge
      // 3. Check if the point is to the left of the maximum x coordinate of the edge
      if ( (y > std::min(p1[1], p2[1])) && (y <= std::max(p1[1], p2[1])) && (x <= std::max(p1[0], p2[0])) ) {

        // Calculate the x-intersection of the line connecting the point to the edge
        G4double x_intersection = (y - p1[1]) * (p2[0] - p1[0]) / (p2[1] - p1[1]) + p1[0];

        // Check if the point is on the same line as the edge or to the left of
        // the x-intersection
        if (p1[0] == p2[0] || x <= x_intersection) {
          // Flip the inside flag
          inside = !inside;
        }
      }

        // Store the current point as the first point for the next iteration
        p1 = p2;
    }
   
    // Return the value of the inside flag
    return inside;
    
  }


  // Function to sample x and y positions on the edge of a regular polygon with given radius and number of sides
  std::vector<G4double> PolygonPointSampler::SampleXYonPolygonEdge(G4double radius) {

    // Calculate the angle between consecutive vertices of the polygon
    G4double angle_increment = 2 * M_PI / n_sides_;

    // Randomly select an edge
    G4int edge_index = static_cast<G4int>(G4UniformRand() * n_sides_);

     // Calculate the indices of the vertices for the selected edge
    G4int vertex_index1 = edge_index;
    G4int vertex_index2 = (edge_index + 1) % n_sides_; // Wrap around if at the last edge

    // Calculate the coordinates of the selected edge's vertices
    G4double x1 = radius * cos(angle_increment * vertex_index1);
    G4double y1 = radius * sin(angle_increment * vertex_index1);
    G4double x2 = radius * cos(angle_increment * vertex_index2);
    G4double y2 = radius * sin(angle_increment * vertex_index2);

    // Sample a random position along the selected edge
    G4double rand = G4UniformRand(); // Random value between 0 and 1
    G4double x = x1 + rand * (x2 - x1);
    G4double y = y1 + rand * (y2 - y1);

    // Return the sampled x and y positions
    std::vector<G4double> xy = {x, y};
    return xy;
}


  G4ThreeVector PolygonPointSampler::RotateAndTranslate(G4ThreeVector position)
  {
    // Rotating if needed
    if (rotation_) position *= *rotation_;
    // Translating
    position += origin_;

    return position;
  }

  void
  PolygonPointSampler::InvertRotationAndTranslation(G4ThreeVector& vec, G4bool translate)
  {
    if (translate)
      vec -= origin_;
    if (rotation_)
      vec.rotate(-rotation_->delta(), rotation_->axis());
  }
} // end namespace nexus
