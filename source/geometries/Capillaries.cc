#include "Capillaries.hh"
#include "G4VisAttributes.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "OpticalMaterialProperties.h"
#include "FactoryBase.h"


namespace nexus{

REGISTER_CLASS(Capillaries, GeometryBase)

Capillaries::Capillaries() : GeometryBase(), specific_vertex_(0.,0.,17*cm)
{}

Capillaries::~Capillaries()
{}
// G4String* region = new G4String("AD_HOC");

void Capillaries::Construct(){

    G4NistManager *nist = G4NistManager::Instance();
    G4double temperature = 298.*kelvin;
    G4double pressure = 10.*atmosphere;
    G4int sc_yield = 1; //value irrelevant for my usecase
    G4double e_lifetime = 1; //value irrelevant for my usecase
    G4Material *Xe = nist->FindOrBuildMaterial("G4_Xe", temperature, pressure);
    G4Material *Teflon = nist->FindOrBuildMaterial("G4_TEFLON", temperature, pressure);
    G4Material *Si = nist->FindOrBuildMaterial("G4_Si", temperature, pressure);

    //Optical propoerties for Xe and TEFLON
    Xe->SetMaterialPropertiesTable(opticalprops::GXe(pressure, temperature, sc_yield, e_lifetime ));
    Teflon->SetMaterialPropertiesTable(opticalprops::PTFE());



    ///// Create the world /////
    G4double edge = 100*cm;

    G4Box *solidWorld = new G4Box("World", //name
                              edge, //side a
                              edge, //side b
                              edge); //side c

    G4LogicalVolume *worldLogicalVolume = new G4LogicalVolume(
                                                                solidWorld, //the cylinder object
                                                                Xe, //material of cylinder
                                                                "World"); //name
    this->SetLogicalVolume(worldLogicalVolume);

    G4VPhysicalVolume* world = new G4PVPlacement(
                0,                // no rotation
                G4ThreeVector(0.,0.,0.),  // at (0,0,0)
                worldLogicalVolume, // its logical volume
                "World",          // its name
                0,                // its mother  volume
                false,            // no boolean operation
                0,                // copy number
                true);          // checking overlaps



    ///// Create the detector cylinder /////
    G4double r1Cyl = 45*cm;
    G4double r2Cyl = 50.*cm;
    G4double cylLength = 25.*cm;

    G4Tubs *barrel = new G4Tubs("Barrel", //name
                              r1Cyl, //inner radius
                              r2Cyl, //outer radius
                              cylLength, //length
                              0., //initial revolve angle
                              2*M_PI); //final revolve angle

    G4LogicalVolume *barrelLogicalVolume = new G4LogicalVolume(
                                                                barrel, //the cylinder object
                                                                Teflon, //material of cylinder
                                                                "Barrel"); //name

new G4PVPlacement(
                0,                // no rotation
                G4ThreeVector(0.,0.,0.),// at (0,0,0)
                barrelLogicalVolume, // its logical volume
                "Barrel",          // its name
                worldLogicalVolume,                // its mother  volume
                false,            // no boolean operation
                0,                // copy number
                true);          // checking overlaps

    // Add reflective surface
    G4OpticalSurface* TeflonSurface = new G4OpticalSurface("Teflon_coating"
                                                        , unified
                                                        , ground
                                                        , dielectric_metal
                                                        );

    TeflonSurface->SetMaterialPropertiesTable(opticalprops::PTFE());
    // new G4LogicalSkinSurface( "Teflon_coating", barrelLogicalVolume, TeflonSurface);



    //create cap
    r1Cyl = 0*cm;
    r2Cyl = 50.*cm;
    G4double capWidth = 2.5*cm;
    G4double capLocationZ = cylLength+capWidth;
    G4Tubs *cap = new G4Tubs("Cap1", //name
                            r1Cyl, //inner radius
                             r2Cyl, //outer radius
                             capWidth, //length
                             0., //initial revolve angle
                             2*M_PI); //final revolve angle

    G4LogicalVolume *capLogicalVolume = new G4LogicalVolume(cap, //the cylinder object
                                                            Teflon, //material of cylinder
                                                            "Cap1"); //name

    new G4PVPlacement(
                    0,                // no rotation
                    G4ThreeVector(0.,0.,capLocationZ),
                    capLogicalVolume, // its logical volume
                    "Cap1",          // its name
                    worldLogicalVolume, // its mother  volume
                    false,            // no boolean operation
                    0,                // copy number
                    true);          // checking overlaps


   //  //add reflective surface to cap
   // new G4LogicalSkinSurface( "Teflon_coating", capLogicalVolume, TeflonSurface);



    ///// create hexagonal fiber pattern /////

    G4double r1Fiber = 0.1*cm;
    G4double r2Fiber = 0.16*cm;
    G4double fiberLength = 3.5*cm;

    G4Tubs *fiber = new G4Tubs("Fiber", //name
                           r1Fiber, //inner radius
                           r2Fiber, //outer radius
                           fiberLength, //length
                           0., //initial revolve angle
                           2*M_PI); //finale revolve angle


    G4LogicalVolume *fiberLogicalVolume = new G4LogicalVolume(
                                                        fiber, //the fiber object
                                                        Teflon, //material of fiber
                                                        "Fiber"); //name


    new G4LogicalSkinSurface( "capillar_inner_coating", fiberLogicalVolume, TeflonSurface);


    G4Color brown(0.8,0.3,0.1);
    G4VisAttributes* fiberColor = new G4VisAttributes(brown);
    fiberLogicalVolume->SetVisAttributes(fiberColor);


    G4int numOfCapillaryHexagons = 1;
    G4float n = numOfCapillaryHexagons; //shorter
    G4float capillarySpacing = 2*r2Fiber;

    G4int count = 0;
    G4float xCapillar;
    G4float yCapillar;
    G4float d = 1*cm;
    // G4float zCapillar = 0;
    G4float zCapillar = cylLength - d - fiberLength;


    for(G4int i=0; i<=(n-1); i++) {
        yCapillar = (sqrt(3)*i*capillarySpacing)/2.0;

        for (G4int j = 0; j < (2*n-1-i); j++) {
            xCapillar = (-(2*n-i-2)*capillarySpacing)/2.0 + j*capillarySpacing;
            new G4PVPlacement(
                            0,                // no rotation
                            G4ThreeVector(xCapillar,yCapillar,zCapillar),  // at (0,0,0)
                            fiberLogicalVolume, // its logical volume
                            "Fiber",          // its name
                            worldLogicalVolume, // its mother  volume
                            false,            // no boolean operation
                            count,              // copy number
                            false);          // checking overlaps
            count+= 1;

            if (yCapillar!=0) {
                new G4PVPlacement(
                                0,                // no rotation
                                G4ThreeVector(xCapillar,-yCapillar,zCapillar),  // at (0,0,0)
                                fiberLogicalVolume, // its logical volume
                                "Fiber",          // its name
                                worldLogicalVolume, // its mother  volume
                                false,            // no boolean operation
                                count,              // copy number
                                false);          // checking overlaps
                count += 1;
            }
        }
    }



    //create SiPM wall
    // G4double size = 0.65*mm;
    G4double size = 1.5*mm;
    G4Box *SiPMSolidVolume = new G4Box("SiPM", //name
                              size, //side a
                              size, //side b
                              size/2); //side c

    G4LogicalVolume* SiPMLogicalVolume = new G4LogicalVolume(SiPMSolidVolume, //the cylinder object
                                            Si, //material of cylinder
                                            "SiPM"); //name

 
    G4Color purple(0.5,0.5,0.8);
    G4VisAttributes* SiPMColor = new G4VisAttributes(purple);
    SiPMLogicalVolume->SetVisAttributes(SiPMColor);

    G4int numberOfSiPMs = 25; //in a row/column
    // G4float spacingBetweenSiPMS = 15.6*mm;
    G4float spacingBetweenSiPMS = 10*mm;
    G4float zSiPM = cylLength-size;
    G4String name;
    //G4float zSiPM = 25*cm;

    for (G4int i=0; i<numberOfSiPMs; i++){
        for (G4int j=0; j<numberOfSiPMs; j++){

            // G4float xSiPM = i*spacingBetweenSiPMS - (numberOfSiPMs*spacingBetweenSiPMS/2);
            // G4float ySiPM = j*spacingBetweenSiPMS - (numberOfSiPMs*spacingBetweenSiPMS/2);

            G4float xSiPM = i*spacingBetweenSiPMS - ((numberOfSiPMs+1)*spacingBetweenSiPMS/2);
            G4float ySiPM = j*spacingBetweenSiPMS - ((numberOfSiPMs+1)*spacingBetweenSiPMS/2);  

            name = "SiPM_" + std::to_string(j+i*numberOfSiPMs); 
            new G4PVPlacement(
                            0,                // no rotation
                            G4ThreeVector(xSiPM,ySiPM,zSiPM),  // at (0,0,0)
                            SiPMLogicalVolume, // its logical volume
                            name,               //its name
                            worldLogicalVolume,   //its mother volume
                            false,            // no boolean operation
                            j+i*numberOfSiPMs,// copy number, this time each SiPM has a unique copy number
                            false);          // checking overlaps
        }
    }



//    //Create coating of fiber

//    G4double r2Coating = 0.25*cm;

//    G4OpticalSurface *coating1 = new G4OpticalSurface("Coating1", //name
//                               r2Fiber, //inner radius
//                               r2Coating, //outer radius
//                               tubeLength, //length
//                               0., //initial revolve angle
//                               2*M_PI); //finale revolve angle


//    G4LogicalVolume *coating1LogicalVolume = new G4LogicalVolume(
//                                                        coating1, //the coating object
//                                                        Si, //material of fiber
//                                                        "Coating1"); //name

//     new G4PVPlacement(
//                0,                // no rotation
//                G4ThreeVector(),  // at (0,0,0)
//                coating1LogicalVolume, // its logical volume
//                "Coating1",          // its name
//                barrel,   // its mother  volume
//                false,            // no boolean operation
//                0,                // copy number
//                true);          // checking overlaps




//    G4OpticalSurface* teflon_surf = new G4OpticalSurface("teflon_surface"
//                                                        , unified
//                                                        , ground
//                                                        , dielectric_metal
//                                                        );
//    //teflon_surf->SetMaterialPropertiesTable(opticalprops::PTFE());
//    teflon_surf->SetMaterialPropertiesTable(PTFE());
//    new G4LogicalSkinSurface( "walls_surface", teflon_walls_logic, teflon_surf);

} //close Construct()


G4ThreeVector Capillaries::GenerateVertex(const G4String& region) const{

    G4ThreeVector vertex(0.,0.,0.);

    // WORLD
    if (region == "CENTER") {
      return vertex;
    }
    else if (region == "AD_HOC") {
      return specific_vertex_;
    }
    else {
      G4Exception("[Capillaries]", "GenerateVertex()", FatalException,
          "Unknown vertex generation region!");
    }
    return vertex;

}



// void capillaries::ConstructSDandField(){
//     MySiPM* SiPM = new MySiPM("SiPM");
//     SiPMLogicalVolume->SetSensitiveDetector(SiPM);
// }


}//close namespace