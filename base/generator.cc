// NEXT simulation: generator.cc
// Implementation of event generator
// Jan 2007
//

#include <next/generator.hh>


// Constructor
// 'shoot'
generator::generator( G4double E_min, G4double E_max, 
		      G4String particle_name, G4String geom_file )
{
  idata_type_ = "shoot";
  E_min_ = E_min;
  E_max_ = E_max;

  // Setting the number of initial particles
  G4int num_particle = 1;
  particle_gun_ = new G4ParticleGun(num_particle);

  // Setting the particle type
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle(particle_name);
  particle_gun_->SetParticleDefinition(particle);
  
  read_param(geom_file);
}

// 'iDST'
generator::generator( G4String idst_file )
{
  idata_type_ = "idst";

  // Setting the number of initial particles
  G4int num_particle = 1;
  particle_gun_ = new G4ParticleGun(num_particle);

  r_.open(idst_file);
}

generator::~generator()
{
  delete particle_gun_;
}


void generator::read_param(string geom_file)
{
  // Getting needed data to generate initial particles position
  bhep::gstore store;
  bhep::sreader param_reader(store);
  param_reader.file(geom_file);
  param_reader.info_level(bhep::NORMAL);
  param_reader.group("GEOMETRY");
  param_reader.read();
  
  CHAMBER_length_  = store.fetch_dstore("CHAMBER_length") * cm;      
  CHAMBER_height_ = store.fetch_dstore("CHAMBER_height") * cm;
  CHAMBER_width_  = store.fetch_dstore("CHAMBER_width") * cm;  
}


void generator::read_event(G4Event* anEvent)
{
  bhep::event& evt = r_.read_next();
  bhep::Point3D vtx = evt.vertex();
  
  for(size_t i=0; i<evt.true_particles().size(); i++) {
    
    bhep::particle& part = *evt.true_particles()[i];
    bhep::Vector3D mom = part.p3();
    
    // Setting the particle type
    G4String particle_name = part.name();
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle(particle_name);
    G4double part_mass = particle->GetPDGMass();
    G4double ekin = sqrt(sqr(mom.mag()) + sqr(part_mass)) - part_mass;

    particle_gun_->SetParticleDefinition(particle);
    
    particle_gun_->SetParticlePosition(G4ThreeVector(vtx.x(), vtx.y(), vtx.z()));

    particle_gun_->SetParticleEnergy(ekin);
    particle_gun_->SetParticleMomentum(G4ThreeVector(mom.x(), mom.y(), mom.z()));
    
    particle_gun_->GeneratePrimaryVertex(anEvent);
  } 
}


void generator::gen_random_position()
{
  // Setting the position
  G4double pos_x = G4UniformRand() * CHAMBER_width_ - CHAMBER_width_/2.;
  G4double pos_y = G4UniformRand() * CHAMBER_height_ - CHAMBER_height_/2.;
  G4double pos_z = G4UniformRand() * CHAMBER_length_ - CHAMBER_length_/2.;

  // m_.message("Primary particle kinetic E =", ekin, " phi =", phi, 
  // 	     "Sin of theta =", sin_theta, bhep::VVERBOSE);
  
  particle_gun_->SetParticlePosition(G4ThreeVector(pos_x, pos_y, pos_z));
}


void generator::gen_random_direction()
{
  // Generating a random momentum direction
  G4double sin_theta = G4UniformRand();
  G4double cos_theta = sqrt(1 - sqr(sin_theta));
  G4double phi   = G4UniformRand() * 180 - 90;
  
  //   m_.message("Primary particle kinetic E =", ekin, " phi =", phi, 
  // 	     "Sin of theta =", sin_theta, bhep::VVERBOSE);    
  
  // Setting the momentum direction
  particle_gun_->SetParticleMomentumDirection(G4ThreeVector(cos_theta * sin(phi),
							    sin_theta,
							    cos_theta * cos(phi)));
}


void generator::gen_random_energy()
{
  G4double ekin  = (G4UniformRand() * (E_max_ - E_min_) + E_min_) * MeV;
  
  //   m_.message("Primary particle kinetic E =", ekin, " phi =", phi, 
  // 	     "Sin of theta =", sin_theta, bhep::VVERBOSE);   

  particle_gun_->SetParticleEnergy(ekin);
}


void generator::GeneratePrimaries(G4Event* anEvent)
{
  // Random generator
  if (idata_type_ == "shoot") {
    
    gen_random_position();
    gen_random_direction();
    gen_random_energy();

    particle_gun_->GeneratePrimaryVertex(anEvent);
  }
  else {
    // Generator data from a file
    read_event(anEvent);
  }
}
