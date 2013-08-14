// NEXT simulation: materials.cc
// 

#include <next/geometry.hh>


void geometry::define_materials()
{  
  m_.message("\n** Defining materials ...", bhep::NORMAL);
  
  G4double a, z, density, fraction_mass;
  G4String name, symbol;
  G4int    num_el, num_atoms;

  // ----- Elements -------
  
  // H
  a = 1.00794*g/mole;
  G4Element* el_H = new G4Element(name="Hidrogen", symbol="H", z=1., a);
  m_.message(" ", el_H, bhep::VVERBOSE);

  // He
  a = 4.002602*g/mole;
  G4Element* el_He = new G4Element(name="Helium", symbol="He", z=2., a);
  m_.message(" ", el_He, bhep::VVERBOSE);

  // C
  a = 12.011*g/mole;
  G4Element* el_C = new G4Element(name="Carbon", symbol="C", z=6., a);
  m_.message(" ", el_C, bhep::VVERBOSE);

  // N
  a = 14.00674*g/mole;
  G4Element* el_N = new G4Element(name="Nitrogen", symbol="N", z=7., a);
  m_.message(" ", el_N, bhep::VVERBOSE);

  // O
  a = 15.9994*g/mole;
  G4Element* el_O = new G4Element(name="Oxygen", symbol="O", z=8., a);
  m_.message(" ", el_O, bhep::VVERBOSE);

  // Na
  a = 22.98977*g/mole;
  G4Element* el_Na = new G4Element(name="Sodium", symbol="Na", z=11., a);
  m_.message(" ", el_Na, bhep::VVERBOSE);

  // Al
  a = 26.9815*g/mole;
  G4Element* el_Al = new G4Element(name="Aluminum", symbol="Al", z=13., a);
  m_.message(" ", el_Al, bhep::VVERBOSE);
    
  // Si
  a = 28.09*g/mole;
  G4Element* el_Si = new G4Element(name="Silicon", symbol="Si", z=14., a);
  m_.message(" ", el_Si, bhep::VVERBOSE);

  // Ar
  a = 39.948*g/mole;
  G4Element* el_Ar = new G4Element(name="Argon", symbol="Ar", z=18., a);
  m_.message(" ", el_Ar, bhep::VVERBOSE);
    
  // Ca
  a = 40.078*g/mole;
  G4Element* el_Ca = new G4Element(name="Calcium", symbol="Ca", z=20., a);
  m_.message(" ", el_Ca, bhep::VVERBOSE);

  // Cr
  a = 51.9961*g/mole;
  G4Element* el_Cr = new G4Element(name="Chromium", symbol="Cr", z=24., a);
  m_.message(" ", el_Cr, bhep::VVERBOSE);

  // Fe
  a = 55.845*g/mole;
  G4Element* el_Fe = new G4Element(name="Iron", symbol="Fe", z=26., a);
  m_.message(" ", el_Fe, bhep::VVERBOSE);

  // Ni
  a = 58.6934*g/mole;
  G4Element* el_Ni = new G4Element(name="Nickel", symbol="Ni", z=28., a);
  m_.message(" ", el_Ni, bhep::VVERBOSE);

  // Cu
  a = 63.546*g/mole;
  G4Element* el_Cu = new G4Element(name="Copper", symbol="Cu", z=29., a);
  m_.message(" ", el_Cu, bhep::VVERBOSE);

  // Se
  a = 78.96*g/mole;
  G4Element* el_Se = new G4Element(name="Selenium", symbol="Se", z=34., a);
  m_.message(" ", el_Se, bhep::VVERBOSE);

  // Mo
  a = 95.94*g/mole;
  G4Element* el_Mo = new G4Element(name="Molibdenium", symbol="Mo", z=42., a);
  m_.message(" ", el_Mo, bhep::VVERBOSE);

  // I
  a = 126.90447*g/mole;
  G4Element* el_I = new G4Element(name="Iodine", symbol="I", z=53., a);
  m_.message(" ", el_I, bhep::VVERBOSE);

  // Cs
  a = 132.90545*g/mole;
  G4Element* el_Cs = new G4Element(name="Cesium", symbol="Cs", z=55., a);
  m_.message(" ", el_Cs, bhep::VVERBOSE);

  // Nd
  a = 144.24*g/mole;
  G4Element* el_Nd = new G4Element(name="Neodymium", symbol="Nd", z=60., a);
  m_.message(" ", el_Nd, bhep::VVERBOSE);

  // W
  a = 183.84*g/mole;
  G4Element* el_W = new G4Element(name="Tungsten", symbol="W", z=74., a);
  m_.message(" ", el_W, bhep::VVERBOSE);

  // Pb
  a = 207.2*g/mole;
  G4Element* el_Pb = new G4Element(name="Lead", symbol="Pb", z=82., a);
  m_.message(" ", el_Pb, bhep::VVERBOSE);

  // Xe
  a = 131.293*g/mole;
  G4Element* el_Xe = new G4Element(name="Xenon", symbol="Xe", z=54., a);
  m_.message(" ", el_Xe, bhep::VVERBOSE);

  
  // ------ Materials -------
  
  // He
  a = 4.00*g/mole;
  density = 0.179*mg/cm3;
  He = new G4Material(name="Helium", z=2., a, density);
  m_.message(" ", He, bhep::VVERBOSE);

  // C (graphite)
  a = 12.01*g/mole;
  density = 1.81*g/cm3;
  C = new G4Material(name="Graphite", z=6., a, density);
  m_.message(" ", C, bhep::VVERBOSE);

  // Al
  a = 26.981539*g/mole;
  density = 2.70*g/cm3;
  Al = new G4Material(name="Aluminum", z=13., a, density);
  m_.message(" ", Al, bhep::VVERBOSE);
    
  // Fe
  a = 55.845*g/mole;
  density = 7.87*g/cm3;
  Fe = new G4Material(name="Iron", z=26., a, density);
  m_.message(" ", Fe, bhep::VVERBOSE);
  
  // Cu
  a = 63.546*g/mole;
  density = 8.96*g/cm3;
  Cu = new G4Material(name="Copper", z=29., a, density);
  m_.message(" ", Cu, bhep::VVERBOSE);
  
  // Se
  a = 82.*g/mole;
  density = 5.*g/cm3;
  Se = new G4Material(name="Selenium", z=34., a, density);
  m_.message(" ", Se, bhep::VVERBOSE);

  // Mo
  a = 95.94*g/mole;
  density = 10.2*g/cm3;
  Mo = new G4Material(name="Molibdenium", z=42., a, density);
  m_.message(" ", Mo, bhep::VVERBOSE);

  // Nd
  a = 144.24*g/mole;
  density = 7.00*g/cm3;
  Nd = new G4Material(name="Neodymium", z=60., a, density);
  m_.message(" ", Nd, bhep::VVERBOSE);

  // Pb
  a = 207.19*g/mole;
  density = 11.35*g/cm3;
  Pb = new G4Material(name="Lead", z=82., a, density);
  m_.message(" ", Pb, bhep::VVERBOSE);

  // Air
  density = 1.2929*mg/cm3;
  Air = new G4Material(name="Air", density, num_el=2, kStateGas, 
                       300.00*kelvin, 1.0*atmosphere);
  Air->AddElement(el_N, .8);
  Air->AddElement(el_O, .2);
  m_.message(" ", Air, bhep::VVERBOSE);
  //   Air = new G4Material(name="Air", density, num_el=2);
  //   Air->AddElement(el_N, .7);
  //   Air->AddElement(el_O, .3);
  //   m_.message(" ", Air, bhep::VVERBOSE);
  
  // H2O
  density = 1.00*g/cm3;
  //   H2O->AddElement(el_H, num_atoms=12);
  //   H2O->AddElement(el_O, num_atoms=11);
  //   H2O->GetIonisation()->SetMeanExcitationEnergy(75.0*eV);
  //   m_.message(" ", H2O, bhep::VVERBOSE);
  H2O = new G4Material(name="Water", density, num_el=2);
  H2O->AddElement(el_H, num_atoms=2);
  H2O->AddElement(el_O, num_atoms=1);
  m_.message(" ", H2O, bhep::VVERBOSE);
  
  // Sapphire
  density = 4.00*g/cm3;
  Sapphire = new G4Material(name="Sapphire", density, num_el=2);
  Sapphire->AddElement(el_O, num_atoms=3);
  Sapphire->AddElement(el_Al, num_atoms=2);
  m_.message(" ", Sapphire, bhep::VVERBOSE);

  // Concrete
  density = 2.30*g/cm3;
  Concrete = new G4Material(name="Concrete", density, num_el=6);
  Concrete->AddElement(el_O,  fraction_mass=0.60541);
  Concrete->AddElement(el_Si, fraction_mass=0.227915);
  Concrete->AddElement(el_Ca, fraction_mass=0.04986);
  Concrete->AddElement(el_H,  fraction_mass=0.09972);
  Concrete->AddElement(el_Fe, fraction_mass=0.00285);
  Concrete->AddElement(el_Al, fraction_mass=0.014245);
  m_.message(" ", Concrete, bhep::VVERBOSE);

  // Scintillator
  density = 1.032*g/cm3;
  Scint = new G4Material(name="Scint", density, num_el=2);
  Scint->AddElement(el_C, num_atoms=9);
  Scint->AddElement(el_H, num_atoms=10);
  m_.message(" ", Scint, bhep::VVERBOSE);

  // Na + I Scintillator
  density = 3.67*g/cm3;
  NaI_scint = new G4Material(name="NaI_scint", density, num_el=2);
  NaI_scint->AddElement(el_Na, num_atoms=1);
  NaI_scint->AddElement(el_I,  num_atoms=1);
  m_.message(" ", NaI_scint, bhep::VVERBOSE);

  // Cs + I Scintillator
  density = 4.53*g/cm3;
  CsI_scint = new G4Material(name="CsI_scint", density, num_el=2);
  CsI_scint->AddElement(el_Cs, num_atoms=1);
  CsI_scint->AddElement(el_I,  num_atoms=1);
  m_.message(" ", CsI_scint, bhep::VVERBOSE);

  // Polystyrene Scintillator
  density = 1.032*g/cm3;
  Polystyrene = new G4Material(name="Polystyrene", density, num_el=2);
  Polystyrene->AddElement(el_C, num_atoms=9);
  Polystyrene->AddElement(el_H, num_atoms=10);
  m_.message(" ", Polystyrene, bhep::VVERBOSE);

  // Vacuum
  Vacuum = new G4Material("Vacuum", z=1., a=1.01*g/mole, 
			  density= universe_mean_density, kStateGas,
			  3.e-18*pascal, 2.73*kelvin);
  m_.message(" ", Vacuum, bhep::VVERBOSE);

  // Mylar
  density=1.397*g/cm3;
  Mylar = new G4Material(name="Mylar", density, num_el=3);
  Mylar->AddElement(el_C,num_atoms=10);
  Mylar->AddElement(el_H,num_atoms=8);
  Mylar->AddElement(el_O,num_atoms=4);
  m_.message(" ", Mylar, bhep::VVERBOSE);

  // Inox
  density=8.*g/cm3;
  Inox = new G4Material(name="Inox", density, num_el=4);
  Inox->AddElement(el_Fe, fraction_mass=0.703);
  Inox->AddElement(el_Ni, fraction_mass=0.103);
  Inox->AddElement(el_Cr, fraction_mass=0.173);
  Inox->AddElement(el_Mo, fraction_mass=0.021);
  m_.message(" ", Inox, bhep::VVERBOSE);

  // Tracking Gas
  density = 2.25e-4 * g/cm3;
  G4double temperature = 300. * kelvin;
  G4double pressure = 0.890 * atmosphere;
  Tracking_gas = new G4Material(name="Tracking_gas", density, num_el=5, kStateGas, 
                               temperature, pressure);
  Tracking_gas->AddElement(el_He, fraction_mass=0.61114);
  Tracking_gas->AddElement(el_C,  fraction_mass=0.17067);
  Tracking_gas->AddElement(el_H,  fraction_mass=0.04267);
  Tracking_gas->AddElement(el_O,  fraction_mass=0.11378);
  Tracking_gas->AddElement(el_Ar, fraction_mass=0.06174);
  m_.message(" ", Tracking_gas, bhep::VVERBOSE);
  
  // Xenon mixture
  // Reading info from geometry parameters file
  bhep::gstore store;
  bhep::sreader reader(store);
  reader.file(geom_file_);
  reader.info_level(bhep::NORMAL);
  reader.group("GAS");
  reader.read();
  
  pressure = store.fetch_dstore("pressure") * atmosphere;
  
  // Xenon follows Ideal Gases Law at this pressure and temperature
  // pV = nRT -> p = nRT/V = mRT/aV = RTd/a -> d = pa/RT
  const double R = 8.314472 *m3*hep_pascal/(kelvin*mole); // gas constant
  a = 131.293*g/mole;
  temperature = (273.15+15.) * kelvin;
  density = (pressure*a)/(R*temperature);

  // TPC_Xenon defined in terms of Xe element with actual isotopic abundances
  // (i.e., non-enriched Xe)
  TPC_Xenon = new G4Material(name="TPC_Xenon", density, num_el=1, kStateGas,
			     temperature, pressure);
  TPC_Xenon->AddElement(el_Xe, fraction_mass=1.0);
  m_.message(" ", TPC_Xenon, bhep::VVERBOSE);
  
}
