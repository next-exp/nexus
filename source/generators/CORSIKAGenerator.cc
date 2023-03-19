// ----------------------------------------------------------------------------
// nexus | CORSIKAGenerator.cc
//
// This class is the primary generator of muons following the angular
// distribution at sea level. Angles are saved to be plotted later.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "CORSIKAGenerator.h"
#include "DetectorConstruction.h"
#include "GeometryBase.h"
#include "FactoryBase.h"

#include <G4GenericMessenger.hh>
#include <G4ParticleDefinition.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <G4RandomDirection.hh>


using namespace nexus;

REGISTER_CLASS(CORSIKAGenerator, G4VPrimaryGenerator)

CORSIKAGenerator::CORSIKAGenerator():
  G4VPrimaryGenerator(), msg_(0), height_(4 * m), plane_x_size_(5 * m), plane_y_size_(5 * m), geom_(0)
{
  msg_ = new G4GenericMessenger(this, "/Generator/CORSIKAGenerator/",
				"Control commands of CORSIKAGenerator.");

  msg_->DeclareProperty("corsika_file", corsika_file_,
			"Name of the CORSIKA binary file.");

  G4GenericMessenger::Command& height = msg_->DeclareProperty("height", height_,
			"Height of the cosmic generation plane.");
  height.SetUnitCategory("Length");
  height.SetRange("height>0.");

  G4GenericMessenger::Command& plane_x_size = msg_->DeclareProperty("plane_x_size", plane_x_size_,
			"Size of the cosmic generation plane on the x axis.");
  plane_x_size.SetUnitCategory("Length");
  plane_x_size.SetRange("plane_x_size > 0.");

  G4GenericMessenger::Command& plane_y_size = msg_->DeclareProperty("plane_y_size", plane_y_size_,
			"Size of the cosmic generation plane on the y axis.");
  plane_y_size.SetUnitCategory("Length");
  plane_y_size.SetRange("plane_y_size > 0.");

  msg_->DeclareProperty("region", region_,
			"Set the region of the geometry where the vertex will be generated.");

  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();
}


CORSIKAGenerator::~CORSIKAGenerator()
{
  delete msg_;
}

void CORSIKAGenerator::OpenFile(G4String corsika_binary)
{
  	input_ = new std::ifstream(corsika_binary);

    while(input_->read(buf_.ch, 4)) {
      unsigned reclen = buf_.in[0];
      // CORSIKA records are in units of 4 bytes
      if(reclen % 4) {
        throw std::runtime_error("[CORSIKAGenerator] Error: record size not a multiple of 4");
      }

      // We will be looking at at least 8 bytes to determine the
      // input file format, and all real CORSIKA records are longer
      // than that.
      if(reclen < 2*4) {
        throw std::runtime_error("[CORSIKAGenerator] Error: reclen too small");
      }

      // Read the full record
      if(!input_->read(buf_.ch, reclen)) {
        break;
      }

      // Determine the format and and store the decision for future blocks.
      // We are starting file read, so should see the RUNH marker
      // In COMPACT format each block is preceded by 4 bytes
      // giving the size of the block in words.

      if(!strncmp(buf_.ch+0, "RUNH", 4)) {
        G4cout << "[CORSIKAGenerator] Reading NORMAL format" << G4endl;
        infmt_ = Format::NORMAL;
      }
      else if(!strncmp(buf_.ch+4, "RUNH", 4)) {
        G4cout << "[CORSIKAGenerator] Reading COMPACT format" << G4endl;
        infmt_ = Format::COMPACT;
      }
      else {
        throw std::runtime_error("[CORSIKAGenerator] Error: did not find the RUNH record to determine COMPACT flag");
      }

      unsigned iword = 0;
      if(infmt_ == Format::COMPACT) {
        // Move to the beginning of the actual block
        ++iword;
      }

      break;

    }
    input_->clear();
    input_->seekg(0, std::ios::beg);
}

double CORSIKAGenerator::WrapVarBoxNo(const float var, const float low, const float high, int &boxno)
{
  //wrap variable so that it's always between low and high
  boxno = int(floor(var / (high - low)));
  return (var - (high - low) * floor(var / (high - low))) + low;
}

void CORSIKAGenerator::GeneratePrimaryVertex(G4Event* event)
{
  // FORTRAN sequential records are prefixed with their length
  // in a 4-byte word
  if (!is_open_) {
    OpenFile(corsika_file_);
    is_open_ = true;
  }

  unsigned n_part = 0;
  std::map<std::pair<int, int>, std::vector< std::pair<G4PrimaryVertex *, G4PrimaryParticle*>>> particles_map;

  while(input_->read(buf_.ch, 4)) {

    unsigned reclen = buf_.in[0];
    // CORSIKA records are in units of 4 bytes
    if(reclen % 4) {
      throw std::runtime_error("Error: record size not a multiple of 4");
    }

    // We will be looking at at least 8 bytes to determine the
    // input file format, and all real CORSIKA records are longer
    // than that.
    if(reclen < 2*4) {
      throw std::runtime_error("Error: reclen too small");
    }

    if(reclen > 4*fbsize_words_) {
      throw std::runtime_error("Error: reclen too big");
    }

    // Read the full record
    if(!input_->read(buf_.ch, reclen)) {
      break;
    }

    //================================================================
    // Go over blocks in the record
    for(unsigned iword = 0; iword < reclen/4; ) {

      unsigned block_words = (infmt_ == Format::COMPACT) ? buf_.in[iword] : 273;

      if(!block_words) {
        throw std::runtime_error("Got block_words = 0\n");
      }

      if(infmt_ == Format::COMPACT) {
        // Move to the beginning of the actual block
        ++iword;
      }

      std::string event_marker = (infmt_ == Format::NORMAL || !event_count_) ? "EVTH" : "EVHW";

      // Determine the type of the data block
      if (!strncmp(buf_.ch+4*iword, "RUNH", 4)) {
        run_number_ = lrint(buf_.fl[1+iword]);
      }
      else if(!strncmp(buf_.ch+4*iword, "RUNE", 4)) {
        unsigned end_run_number = lrint(buf_.fl[1+iword]);
        unsigned end_event_count = lrint(buf_.fl[2+iword]);
        if(end_run_number != run_number_) {
          throw std::runtime_error("Error: run number mismatch in end of run record\n");
        }
        if(event_count_ != end_event_count) {
          std::cerr<<"RUNE: _event_count = " << event_count_ << " end record = " << end_event_count << G4endl;
          throw std::runtime_error("Error: event count mismatch in end of run record\n");
        }
        // Exit the read loop at the end of run
        // _primaries = 0;
        G4cout << "[CORSIKAGenerator] End of CORSIKA file" << G4endl;
        break;
      }
      else if(!strncmp(buf_.ch+4*iword, event_marker.data(), 4)) {
        ++event_count_;
        current_event_number_ = lrint(buf_.fl[1+iword]);
        G4cout << "[CORSIKAGenerator] Event " << current_event_number_ << G4endl;
        particles_map.erase(particles_map.begin(), particles_map.end());
        // ++_primaries;
      } else if(!strncmp(buf_.ch+4*iword, "EVTE", 4)) {
        unsigned end_event_number = lrint(buf_.fl[1+iword]);
        if (end_event_number != current_event_number_) {
          throw std::runtime_error("Error: event number mismatch in end of event record\n");
        }
      } else {
        for (unsigned i_part = 0; i_part < block_words; i_part+=7) {
          unsigned id = buf_.fl[iword + i_part] / 1000;
          if (id == 0){
            continue;
          }
          n_part++;
          const int pdgId = corsikaToPdgId.at(id);

          // The footprint of a CORSIKA shower is very large. We tile the event
          // with user-defined dimensions, then we pick only the first tile, discarding the rest.
          int box_x = 0;
          const float x = WrapVarBoxNo(buf_.fl[iword + i_part + 4] * cm, -plane_x_size_/2, plane_x_size_/2, box_x);
          int box_y = 0;
          const float y = WrapVarBoxNo(buf_.fl[iword + i_part + 5] * cm, -plane_y_size_/2, plane_y_size_/2, box_y);
          std::pair xy(box_x, box_y);

          const float px = buf_.fl[iword + i_part + 1] * GeV;
          const float py = buf_.fl[iword + i_part + 2] * GeV;
          const float pz = -buf_.fl[iword + i_part + 3] * GeV; // In CORSIKA the momentum is in the -z direction
          const float t = buf_.fl[iword + i_part + 6];
          G4PrimaryVertex* vertex = new G4PrimaryVertex(G4ThreeVector(x,y,height_), t);

          G4ParticleDefinition* particle_definition_ =
            G4ParticleTable::GetParticleTable()->FindParticle(pdgId);
          G4PrimaryParticle* particle = new G4PrimaryParticle(particle_definition_, px, py, pz);

          // Here we store the particles in a map where the key is the tile ID and
          // the value is a pair of vertex, particle
          std::pair vp(vertex, particle);
          if (particles_map.count(xy) == 0) {
            std::vector<std::pair<G4PrimaryVertex *, G4PrimaryParticle *>> vps;
            vps.push_back(vp);
            particles_map.insert({xy, vps});
          } else {
            particles_map[xy].push_back(vp);
          }

        }

      }

      // Move to the next block
      iword += block_words;

    } // loop over blocks in a record

    // Here we expect the FORTRAN end of record padding,
    // read and verify its value.
    if(!input_->read(buf_.ch, 4)) {
      break;
    }
    if(buf_.in[0] != reclen) {
      throw std::runtime_error("Error: unexpected FORTRAN record end padding");
    }

    if (n_part > 0) {
      // Pick the first tile and insert the particle into the event
      for (auto vp: particles_map.begin()->second) {
        vp.first->SetPrimary(vp.second);
        event->AddPrimaryVertex(vp.first);
      }
      G4cout << "[CORSIKAGenerator] End event " << event_count_ << " with " << n_part << " particles" << G4endl;
      n_part = 0;
      break;
    }

  } // loop over records
}
