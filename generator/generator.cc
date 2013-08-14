// NEXUS: generator.cc
//    -- Created on: Jan 2007
//    -- Last modification: July 4th 2007, jmalbos
//

#include <nexus/generator.h>

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleMomentum.hh"

#include "G4ThreeVector.hh"

#include <bhep/EventManager2.h>


namespace nexus {


  /// Constructor
  generator::generator( const bhep::gstore& gen_store, 
			const bhep::gstore& geom_store, 
			EventManager2& evt_mgr, bhep::prlevel vl )
  {
    __gen_store  = new bhep::gstore(gen_store);
    __evt_mgr    = &evt_mgr;
    
    msg = bhep::messenger(vl);
    msg.message("nexus::generator instance created.", bhep::VERBOSE);

    initialize();
  }


  /// Destructor
  generator::~generator()
  {
    delete __particle_gun;
    delete __gen_store;
    msg.message("nexus::generator instance deleted.", bhep::VERBOSE);
  }


  /// Initializes event generator
  void generator::initialize()
  {
    msg.message("\n", bhep::NORMAL);
    msg.message("-------------------------------------------------", bhep::NORMAL);
    msg.message("  INITIALIZING EVENT GENERATOR                   ", bhep::NORMAL);
    msg.message("-------------------------------------------------", bhep::NORMAL);
  
    msg.message("  Reading input parameters.", bhep::DETAILED);
    read_params();
    
    msg.message("  Setting particle gun.", bhep::DETAILED);
    set_particle_gun();

    msg.message("-------------------------------------------------\n", bhep::DETAILED);
  }


  void generator::read_params()
  {
    // GENERATOR MODE
    //
    __gen_type = __gen_store->fetch_sstore("type");
    msg.message("     + Event generator type:", __gen_type, bhep::VERBOSE);

    if( __gen_type == "single_particle" ) {
      __particle_name = __gen_store->fetch_sstore("particle_name");
      __min_energy    = __gen_store->fetch_dstore("min_energy") * keV;
      __max_energy    = __gen_store->fetch_dstore("max_energy") * keV;
      msg.message("     + Primary particle:", __particle_name, bhep::VERBOSE);
      msg.message("     + Maximum kinetic energy:", __max_energy/keV, "keV", bhep::VERBOSE);
      msg.message("     + Minimum kinetic energy:", __min_energy/keV, "keV", bhep::VERBOSE);
    }
    
    else if( __gen_type == "idst" ) {
      __idst_filename = __gen_store->fetch_sstore("idst_filename");
      msg.message("     + Events read from: ", __idst_filename, bhep::VERBOSE);
    }

    // VERTEX GENERATION
    //
    __vtx_type = __gen_store->fetch_sstore("vtx_type");
    msg.message("     + Vertex type: ", __vtx_type, bhep::VERBOSE);
    
    if( __vtx_type == "spot" ) {
      __vtx_spot = __gen_store->fetch_vstore("spot") * mm;
      msg.message("     + Events vertex (spot):", __vtx_spot, "mm", bhep::VERBOSE);
    }
    
    else if( __vtx_type == "random" ) {
      __vtx_region = __gen_store->fetch_sstore("region");
      msg.message("     + Region of generation of vertexes:", __vtx_region, bhep::VERBOSE);
    }
  }


  void generator::set_particle_gun()
  {
    // Number of initial particles
    G4int num_particle = 1;
    __particle_gun = new G4ParticleGun(num_particle);
    
    if( __gen_type=="single_particle" ) {
      
      // Setting the particle type
      G4ParticleTable* part_table = G4ParticleTable::GetParticleTable();
      G4ParticleDefinition* part = part_table->FindParticle( __particle_name );
      __particle_gun->SetParticleDefinition( part );  
    }
  }


  void generator::set_particle_energy()
  {
    G4double kinetic_energy = G4UniformRand() * (__max_energy-__min_energy) + __min_energy;
    __particle_gun->SetParticleEnergy(kinetic_energy);
  }

  
  void generator::set_particle_position()
  {
    if( __vtx_type == "spot")
      __particle_gun->SetParticlePosition( G4ThreeVector( __vtx_spot[0], 
							  __vtx_spot[1], 
							  __vtx_spot[2] ) );
    else {
      bhep::Point3D& pos = get_vertex();
      __particle_gun->SetParticlePosition( G4ThreeVector(pos.x(), pos.y(), pos.z()) );
    }
  }


  void generator::set_particle_direction()
  {
    //distribution uniform in solid angle
    G4double cosTheta = 2*G4UniformRand() - 1.;
    G4double sinTheta = std::sqrt(1. - cosTheta*cosTheta);
    G4double phi = twopi*G4UniformRand();
    
    G4double ux = sinTheta*std::cos(phi);
    G4double uy = sinTheta*std::sin(phi);
    G4double uz = cosTheta;

    //G4double ux=0., uy=0., uz=0.;
    //uz = -1.;
    
    __particle_gun->SetParticleMomentumDirection(G4ThreeVector(ux,uy,uz));
  }

  
  void generator::read_event_from_idst( G4Event* anEvent )
  {
    // read event using bhep EventManager
    bhep::event& evt = __evt_mgr->read();
    
    // vertex
    bhep::Point3D vtx;
    
    if( __vtx_type == "spot") 
      vtx.set( __vtx_spot[0], __vtx_spot[1], __vtx_spot[2] );
    else
      vtx = get_vertex();

    // time
    G4double evt_time = 0.0*second;

    // looping through event particles
    for(size_t i=0; i<evt.true_particles().size(); i++) {
    
      bhep::particle& particle = *evt.true_particles()[i];
      bhep::Vector3D mom = particle.p3();
      
      G4String part_name = particle.name();
      G4ParticleTable* part_table = G4ParticleTable::GetParticleTable();
      G4ParticleDefinition* part = part_table->FindParticle( part_name );
      __particle_gun->SetParticleDefinition( part );
      
      G4double mass = part->GetPDGMass();
      G4double kinetic_energy = sqrt( sqr(mom.mag())+sqr(mass) ) - mass;
      
      G4double part_time = 0.0*second;
      if ( particle.find_property("time") ) {
        G4String foo = particle.fetch_property("time");
        part_time = bhep::double_from_string(foo)*second;
      } 

      G4ThreeVector* momentum = new G4ThreeVector(mom.x(), mom.y(), mom.z());
      __particle_gun->SetParticleEnergy(kinetic_energy*MeV);
      __particle_gun->SetParticleTime( evt_time+part_time );
      __particle_gun->SetParticleMomentumDirection(*momentum);
      __particle_gun->SetParticlePosition( G4ThreeVector(vtx.x(), vtx.y(), vtx.z()) );
      
      // Shoot the particle gun
      __particle_gun->GeneratePrimaryVertex(anEvent);
    } 
  }


  void generator::GeneratePrimaries(G4Event* anEvent)
  {
    if( __gen_type == "idst" )
      read_event_from_idst(anEvent);
    
    else if( __gen_type == "single_particle" ) {
      
      set_particle_position();
      set_particle_direction();
      set_particle_energy();
      
      __particle_gun->GeneratePrimaryVertex(anEvent);
    }
  }
  
} // namespace nexus
