// ----------------------------------------------------------------------------
// nexus | FullRingInfinity.cc
//
// This class implements the geometry of a cylindric ring of LXe,
// of configurable length and diameter.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "FullRingInfinity.h"
#include "SiPMpetFBK.h"
#include "SpherePointSampler.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Orb.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <G4LogicalVolume.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4OpticalSurface.hh>
#include <Randomize.hh>

#include <stdexcept>

namespace nexus {

  FullRingInfinity::FullRingInfinity():
    BaseGeometry(),
    // Detector dimensions
    sipm_pitch_(4.*mm),
    n_sipm_rows_(16),
    instr_faces_(2),
    kapton_thickn_(0.3*mm),
    depth_(5.*cm),
    inner_radius_(15.*cm),
    cryo_width_(12.*cm),
    cryo_thickn_(1.*mm),
    max_step_size_(1.*mm),
    phantom_(false),
    pt_Lx_(0.),
    pt_Ly_(0.),
    pt_Lz_(0.),
    sensitivity_(false),
    events_per_point_(1),
    sensitivity_point_id_(0),
    sensitivity_index_(0),
    sensitivity_binning_(1*mm),
    sens_x_min_(-inner_radius_),
    sens_x_max_(inner_radius_),
    sens_y_min_(-inner_radius_),
    sens_y_max_(inner_radius_)
  {
     // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/FullRingInfinity/",
                                  "Control commands of geometry FullRingInfinity.");
    G4GenericMessenger::Command& depth_cmd =
      msg_->DeclareProperty("depth", depth_, "Dimension in DOI");
    depth_cmd.SetUnitCategory("Length");
    depth_cmd.SetParameterName("depth", false);
    depth_cmd.SetRange("depth>0.");

    G4GenericMessenger::Command& pitch_cmd =
      msg_->DeclareProperty("pitch", sipm_pitch_, "Pitch of SiPMs");
    pitch_cmd.SetUnitCategory("Length");
    pitch_cmd.SetParameterName("pitch", false);
    pitch_cmd.SetRange("pitch>0.");

    G4GenericMessenger::Command& inner_r_cmd =
      msg_->DeclareProperty("inner_radius", inner_radius_, "Inner radius of ring");
    inner_r_cmd.SetUnitCategory("Length");
    inner_r_cmd.SetParameterName("inner_radius", false);
    inner_r_cmd.SetRange("inner_radius>0.");

    G4GenericMessenger::Command& cryo_width_cmd =
      msg_->DeclareProperty("cryostat_width", cryo_width_, "Width of cryostat in z");
    cryo_width_cmd.SetUnitCategory("Length");
    cryo_width_cmd.SetParameterName("cryostat_width", false);
    cryo_width_cmd.SetRange("cryostat_width>0.");

    msg_->DeclareProperty("sipm_rows", n_sipm_rows_, "Number of SiPM rows");
    msg_->DeclareProperty("instrumented_faces", instr_faces_, "Number of instrumented faces");
    msg_->DeclareProperty("phantom", phantom_, "True if spherical physical phantom is used");

    G4GenericMessenger::Command&  specific_vertex_X_cmd =
      msg_->DeclareProperty("specific_vertex_X", specific_vertex_X_,
                            "If region is AD_HOC, x coord where particles are generated");
    specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
    specific_vertex_X_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Y_cmd =
      msg_->DeclareProperty("specific_vertex_Y", specific_vertex_Y_,
                            "If region is AD_HOC, y coord where particles are generated");
    specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
    specific_vertex_Y_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Z_cmd =
      msg_->DeclareProperty("specific_vertex_Z", specific_vertex_Z_,
                            "If region is AD_HOC, z coord where particles are generated");
    specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
    specific_vertex_Z_cmd.SetUnitCategory("Length");

    // Read in the point distribution.
    msg_->DeclareMethod("pointFile", &FullRingInfinity::BuildPointfile, "Location of file containing distribution of event generation points.");

    G4GenericMessenger::Command& table_cmd =
      msg_->DeclareProperty("sensitivity_binning", sensitivity_binning_,
                            "Pitch for sensitivity point generation");
    table_cmd.SetUnitCategory("Length");
    table_cmd.SetParameterName("sensitivity_binning", false);
    table_cmd.SetRange("sensitivity_binning>0.");

    msg_->DeclareProperty("sensitivity", sensitivity_,
			  "True if sensitivity map is being run");
    msg_->DeclareProperty("sensitivity_point_id", sensitivity_point_id_,
			  "Starting point for sensitivity run");
    msg_->DeclareProperty("events_per_point", events_per_point_,
			  "Number of events to be generated per point");

    G4GenericMessenger::Command& sns_x_min_cmd =
      msg_->DeclareProperty("sens_x_min", sens_x_min_,
                            "Minimum x for sensitivity map");
    sns_x_min_cmd.SetUnitCategory("Length");
    sns_x_min_cmd.SetParameterName("sens_x_min", false);

    G4GenericMessenger::Command& sns_x_max_cmd =
      msg_->DeclareProperty("sens_x_max", sens_x_max_,
                            "Maxmimum x for sensitivity map");
    sns_x_max_cmd.SetUnitCategory("Length");
    sns_x_max_cmd.SetParameterName("sens_x_max", false);

    G4GenericMessenger::Command& sns_y_min_cmd =
      msg_->DeclareProperty("sens_y_min", sens_y_min_,
                            "Minimum y for sensitivity map");
    sns_y_min_cmd.SetUnitCategory("Length");
    sns_y_min_cmd.SetParameterName("sens_y_min", false);

    G4GenericMessenger::Command& sns_y_max_cmd =
      msg_->DeclareProperty("sens_y_max", sens_y_max_,
                            "Maxmimum y for sensitivity map");
    sns_y_max_cmd.SetUnitCategory("Length");
    sns_y_max_cmd.SetParameterName("sens_y_max", false);

    G4GenericMessenger::Command& sns_z_min_cmd =
      msg_->DeclareProperty("sens_z_min", sens_z_min_,
                            "Minimum z for sensitivity map");
    sns_z_min_cmd.SetUnitCategory("Length");
    sns_z_min_cmd.SetParameterName("sens_z_min", false);

    G4GenericMessenger::Command& sns_z_max_cmd =
      msg_->DeclareProperty("sens_z_max", sens_z_max_,
                            "Maxmimum z for sensitivity map");
    sns_z_max_cmd.SetUnitCategory("Length");
    sns_z_max_cmd.SetParameterName("sens_z_max", false);


    sipm_ = new SiPMpetFBK();
  }

  FullRingInfinity::~FullRingInfinity()
  {
    delete [] pt_;
  }

  void FullRingInfinity::Construct()
  {
    // LAB. This is just a volume of air surrounding the detector
    G4double lab_size = 10.*m;
    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);

    lab_logic_ =
      new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(lab_logic_);

    axial_length_ = sipm_pitch_ *  n_sipm_rows_;
    G4cout << "Axial dimensions (mm) = " << axial_length_/mm << G4endl;


    external_radius_ = inner_radius_ + depth_;
    G4cout << "Radial dimensions (mm): "<< inner_radius_/mm << ", "
	   << external_radius_/mm << G4endl;
    BuildCryostat();
    BuildSensors();

    if (phantom_)
      BuildPhantom();


    if (sensitivity_)
      CalculateSensitivityVertices(sensitivity_binning_);
  }

  void FullRingInfinity::BuildCryostat()
  {
    const G4double space_for_elec = 2. * cm;
    const G4double int_radius_cryo = inner_radius_ - cryo_thickn_ - space_for_elec;
    const G4double ext_radius_cryo = external_radius_ + cryo_thickn_ + space_for_elec;


    G4Tubs* cryostat_solid =
      new G4Tubs("CRYOSTAT", int_radius_cryo, ext_radius_cryo, cryo_width_/2., 0, twopi);
    G4Material* steel = MaterialsList::Steel();
    G4LogicalVolume* cryostat_logic =
      new G4LogicalVolume(cryostat_solid, steel, "CRYOSTAT");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), cryostat_logic,
		      "CRYOSTAT", lab_logic_, false, 0, true);


    G4double ext_offset = 0. * mm;
    G4Tubs* LXe_solid =
      new G4Tubs("LXE", inner_radius_ - kapton_thickn_, external_radius_ + ext_offset + kapton_thickn_,
                 (axial_length_ + 2.*kapton_thickn_)/2., 0, twopi);
    G4Material* LXe = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    LXe->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
    LXe_logic_ =
      new G4LogicalVolume(LXe_solid, LXe, "LXE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), LXe_logic_,
		      "LXE", cryostat_logic, false, 0, true);

    G4Tubs* active_solid =
      new G4Tubs("ACTIVE", inner_radius_, external_radius_ + ext_offset,
                 axial_length_/2., 0, twopi);
    active_logic_ =
      new G4LogicalVolume(active_solid, LXe, "ACTIVE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), active_logic_,
		      "ACTIVE", LXe_logic_, false, 0, true);

    // Set the ACTIVE volume as an ionization sensitive det
    IonizationSD* ionisd = new IonizationSD("/PETALO/ACTIVE");
    active_logic_->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Limit the step size in ACTIVE volume for better tracking precision
    active_logic_->SetUserLimits(new G4UserLimits(max_step_size_));

    G4Material* kapton =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");

    G4Tubs* kapton_int_solid =
      new G4Tubs("KAPTON", inner_radius_ - kapton_thickn_, inner_radius_,
                 axial_length_/2., 0, twopi);
    G4LogicalVolume* kapton_int_logic =
      new G4LogicalVolume(kapton_int_solid, kapton, "KAPTON");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), kapton_int_logic,
		      "KAPTON", LXe_logic_, false, 0, true);

    G4Tubs* kapton_ext_solid =
      new G4Tubs("KAPTON", external_radius_ + ext_offset, external_radius_ + ext_offset + kapton_thickn_,
                 axial_length_/2., 0, twopi);
    G4LogicalVolume* kapton_ext_logic =
      new G4LogicalVolume(kapton_ext_solid, kapton, "KAPTON");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), kapton_ext_logic,
    		      "KAPTON", LXe_logic_, false, 0, true);

    G4Tubs* kapton_lat_solid =
      new G4Tubs("KAPTON", inner_radius_ - kapton_thickn_, external_radius_ + ext_offset + kapton_thickn_,
                 kapton_thickn_/2., 0, twopi);
    G4LogicalVolume* kapton_lat_logic =
      new G4LogicalVolume(kapton_lat_solid, kapton, "KAPTON");
    G4double z_pos = axial_length_/2. + kapton_thickn_/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., z_pos), kapton_lat_logic,
    		      "KAPTON", LXe_logic_, false, 0, true);
    new G4PVPlacement(0, G4ThreeVector(0., 0., -z_pos), kapton_lat_logic,
    		      "KAPTON", LXe_logic_, false, 1, true);

    // OPTICAL SURFACE FOR REFLECTION
    G4OpticalSurface* db_opsur = new G4OpticalSurface("BORDER");
    db_opsur->SetType(dielectric_metal);
    db_opsur->SetModel(unified);
    db_opsur->SetFinish(ground);
    db_opsur->SetSigmaAlpha(0.1);
    db_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::ReflectantSurface(0.));
    new G4LogicalSkinSurface("BORDER", kapton_lat_logic, db_opsur);
    new G4LogicalSkinSurface("BORDER", kapton_int_logic, db_opsur);
    new G4LogicalSkinSurface("BORDER", kapton_ext_logic, db_opsur);

    // G4cout << (external_radius_  - kapton_thickn_) / cm << G4endl;

    G4VisAttributes kapton_col = nexus::CopperBrown();
    kapton_col.SetForceSolid(true);
    kapton_int_logic->SetVisAttributes(kapton_col);
    kapton_ext_logic->SetVisAttributes(kapton_col);
    kapton_lat_logic->SetVisAttributes(kapton_col);
    // G4VisAttributes active_col = nexus::Blue();
    // active_col.SetForceSolid(true);
    // active_logic->SetVisAttributes(active_col);

  }

  void FullRingInfinity::BuildSensors()
  {
    sipm_->SetSensorDepth(1);
    sipm_->Construct();

    G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();
    G4ThreeVector sipm_dim = sipm_->GetDimensions();
    G4cout << "SiPM size = " << sipm_dim << G4endl;
    //G4double sipm_pitch = sipm_dim.x() + 1. * mm;

    G4int n_sipm_int = 2*pi*inner_radius_/sipm_pitch_;
    if (instr_faces_ == 2) {
      G4cout << "Number of sipms in inner face: " <<  n_sipm_int *  n_sipm_rows_<< G4endl;
    }
    G4double step = 2.*pi/n_sipm_int;
    G4double radius = inner_radius_ + sipm_dim.z()/2.;

    G4RotationMatrix rot;
    rot.rotateX(-pi/2.);

    G4int copy_no = 999;
    for (G4int j=0; j<n_sipm_rows_; j++) {
      // The first must be positioned outside the loop
      if (j!=0) rot.rotateZ(step);
      G4double z_dimension = -axial_length_/2. + (j + 1./2.) * sipm_pitch_;
      G4ThreeVector position(0., radius, z_dimension);
      copy_no += 1;
      G4String vol_name = "SIPM_" + std::to_string(copy_no);
      if (instr_faces_ == 2) {
        new G4PVPlacement(G4Transform3D(rot, position), sipm_logic,
                          vol_name, active_logic_, false, copy_no, false);
      }

      for (G4int i=2; i<=n_sipm_int; ++i) {
        G4double angle = (i-1)*step;
        rot.rotateZ(step);
        position.setX(-radius*sin(angle));
        position.setY(radius*cos(angle));
        copy_no += 1;
        vol_name = "SIPM_" + std::to_string(copy_no);
        if (instr_faces_ == 2) {
          new G4PVPlacement(G4Transform3D(rot, position), sipm_logic,
                            vol_name, active_logic_, false, copy_no, false);
        }
      }
    }

    //G4double sipm_pitch_ext = sipm_dim.x() + 0.5 * mm;
    G4double offset = 0.1 * mm;
    G4int n_sipm_ext = 2*pi*external_radius_/sipm_pitch_;
    G4cout << "Number of sipms in external face: " <<  n_sipm_ext * n_sipm_rows_ << G4endl;
    radius = external_radius_ - sipm_dim.z()/2. - offset;

    rot.rotateZ(step);
    rot.rotateX(pi);

    step = 2.*pi/n_sipm_ext;

    //copy_no = 2000;
    if (instr_faces_ == 1) {
      copy_no = 999;
    }

    for (G4int j=0; j<n_sipm_rows_; j++) {
      // The first must be positioned outside the loop
      if (j!=0) rot.rotateZ(step);
      G4double z_pos = -axial_length_/2. + (j + 1./2.) * sipm_pitch_;
      G4ThreeVector position(0., radius, z_pos);
      copy_no = copy_no + 1;
      G4String vol_name = "SIPM_" + std::to_string(copy_no);
      new G4PVPlacement(G4Transform3D(rot, position), sipm_logic,
                        vol_name, active_logic_, false, copy_no, false);
      // G4cout << "INSERT INTO ChannelMatrixP7R410Z1950mm (MinRun, MaxRun, SensorID, PhiNumber, ZNumber) VALUES (0, 100000, "
      //	     << copy_no << ", 0, " << j << ");" << G4endl;
      //G4cout << "INSERT INTO ChannelPositionP7R410Z1950mm (MinRun, MaxRun, SensorID, X, Y, Z) VALUES (0, 100000, "
      //	       << copy_no << ", " << position.getX() << ", " << position.getY() << ", " << position.getZ() << ");" << G4endl;
      for (G4int i=2; i<=n_sipm_ext; ++i) {
        G4double angle = (i-1)*step;
        rot.rotateZ(step);
        position.setX(-radius*sin(angle));
        position.setY(radius*cos(angle));
        copy_no = copy_no + 1;
        vol_name = "SIPM_" + std::to_string(copy_no);
        new G4PVPlacement(G4Transform3D(rot, position), sipm_logic,
                          vol_name, active_logic_, false, copy_no, false);
	//	G4cout << "INSERT INTO ChannelMatrixP7R410Z1950mm (MinRun, MaxRun, SensorID, PhiNumber, ZNumber) VALUES (0, 100000, "
	//       << copy_no << ", " << i-1 << ", " << j << ");" << G4endl;
	//	G4cout << "INSERT INTO ChannelPositionP7R410Z1950mm (MinRun, MaxRun, SensorID, X, Y, Z) VALUES (0, 100000, "
	//	       << copy_no << ", " << position.getX() << ", " << position.getY() << ", " << position.getZ() << ");" << G4endl;
      }
    }

  }


 void FullRingInfinity::BuildPhantom()
  {
    phantom_diam_ = 6.*cm;

    G4Orb* phantom_solid = new G4Orb("PHANTOM",  phantom_diam_/2.);
    G4LogicalVolume* phantom_logic =
      new G4LogicalVolume(phantom_solid, MaterialsList::PEEK(), "PHANTOM");
    G4ThreeVector phantom_origin =
      G4ThreeVector(specific_vertex_X_, specific_vertex_Y_, specific_vertex_Z_);
    new G4PVPlacement(0, phantom_origin, phantom_logic, "PHANTOM", lab_logic_, false, 0, true);

    spheric_gen_ =
      new SpherePointSampler(0., phantom_diam_/2, phantom_origin);
  }


  G4ThreeVector FullRingInfinity::GenerateVertex(const G4String& region) const
  {

    G4ThreeVector vertex(0.,0.,0.);

    if (region == "CENTER") {
      return vertex;
    } else if (region == "AD_HOC") {
      vertex = G4ThreeVector(specific_vertex_X_, specific_vertex_Y_, specific_vertex_Z_);
    } else if (region == "PHANTOM") {
      vertex = spheric_gen_->GenerateVertex("VOLUME");
    } else if (region == "CUSTOM") {
      vertex = RandomPointVertex();
    } else if (region == "SENSITIVITY") {
      unsigned int i = sensitivity_point_id_ + sensitivity_index_;

      if (i == (sensitivity_vertices_.size()*events_per_point_-1)) {
       G4Exception("[FullRingInfinity]", "GenerateVertex()",
      		    RunMustBeAborted, "Reached last event in sensitivity map.");
      }

      try {
	auto pos = i % sensitivity_vertices_.size();
        vertex = sensitivity_vertices_.at(pos);
        sensitivity_index_++;
      }
      catch (const std::out_of_range& oor) {
        G4Exception("[FullRingInfinity]", "GenerateVertex()", FatalErrorInArgument, "Sensitivity point out of range.");
      }

    } else {
      G4Exception("[FullRingInfinity]", "GenerateVertex()", FatalException,
                  "Unknown vertex generation region!");
    }
    return vertex;
  }

  G4int FullRingInfinity::binarySearchPt(G4int low, G4int high, G4double rnd) const {

    // Error
    if(high < 0 || low >= pt_Nx_*pt_Ny_*pt_Nz_) return -1;

    // Return the element before the first element greater than rnd.
    if(pt_[low] > rnd)   return low-1;

    G4int mid = (low + high)/2;
    if(pt_[mid] <= rnd)
      return binarySearchPt(mid+1, high, rnd);
    return binarySearchPt(low, mid-1, rnd);
  }

  // Generates a vertex corresponding to a random point from the custom volume.
  G4ThreeVector FullRingInfinity::RandomPointVertex() const
  {
    // Select the index in the cumulative distribution.
    G4double rnd = G4UniformRand();
    G4int ipt = binarySearchPt(0, pt_Nx_*pt_Ny_*pt_Nz_-1, rnd);

    if(ipt < 0) {
      std::cerr << "ERROR: random point vertex selection failed." << std::endl;
      return G4ThreeVector(0,0,0);
    }
    //G4int jpt = 0;
    //while(pt_[jpt] < rnd) jpt++;
    //jpt -= 1;
    //if(ipt != jpt) std::cout << "WARNING: algorithms don't match i = " << ipt << ", j = " << jpt << "***" << std::endl;

    // Compute the vertex.
    G4int nx = ipt / (pt_Ny_*pt_Nz_);
    G4int ny = (ipt / pt_Nz_) % pt_Ny_;
    G4int nz = ipt % pt_Nz_;
    G4double x = pt_Lx_ * ( ((G4double) nx) / pt_Nx_ - 0.5);
    G4double y = pt_Ly_ * ( ((G4double) ny) / pt_Ny_ - 0.5);
    G4double z = pt_Lz_ * ( ((G4double) nz) / pt_Nz_ - 0.5);

    G4double xrnd = G4UniformRand()-0.5;
    G4double yrnd = G4UniformRand()-0.5;
    G4double zrnd = G4UniformRand()-0.5;

    //std::cout << "Generated at point (" << x << ", " << y << ", " << z << "), index " << ipt << std::endl;
    return G4ThreeVector(x+xrnd,y+yrnd,z+zrnd);
  }

  void FullRingInfinity::BuildPointfile(G4String pointFile)
  {
    int Nx, Ny, Nz;
    float Lx, Ly, Lz;

    // Open the file containing the point distribution.
    std::ifstream is;
    is.open(pointFile, std::ifstream::binary);

    // Read the header.
    is.read(reinterpret_cast<char*>(&Nx),sizeof(int));
    is.read(reinterpret_cast<char*>(&Ny),sizeof(int));
    is.read(reinterpret_cast<char*>(&Nz),sizeof(int));
    is.read(reinterpret_cast<char*>(&Lx),sizeof(float));
    is.read(reinterpret_cast<char*>(&Ly),sizeof(float));
    is.read(reinterpret_cast<char*>(&Lz),sizeof(float));
    pt_Nx_ = Nx; pt_Ny_ = Ny; pt_Nz_ = Nz;
    pt_Lx_ = Lx; pt_Ly_ = Ly; pt_Lz_ = Lz;

    // Read the distribution.
    int i = 0;
    float f;
    int length = pt_Nx_ * pt_Ny_ * pt_Nz_;
    pt_ = new G4float[length];
    while (is.read(reinterpret_cast<char*>(&f), sizeof(float))) {
        pt_[i] = f;
        i++;
    }

    is.close();

    std::cout << "Read distribution of (" << pt_Nx_ << ", " << pt_Ny_ << ", " << pt_Nz_
	      << "); Len (" << pt_Lx_ << ", " << pt_Ly_ << ", " << pt_Lz_
	      << "); with total elements = " << length << ", and first two = "
	      << pt_[0] << " , " << pt_[1] << std::endl;
  }

  void FullRingInfinity::CalculateSensitivityVertices(G4double binning)
  {
    sensitivity_vertices_.clear();

    G4int i_max = floor((sens_x_max_ - sens_x_min_)/binning);
    G4int j_max = floor((sens_y_max_ - sens_y_min_)/binning);
    G4int k_max = floor((sens_z_max_ - sens_z_min_)/binning);

    for (G4int i=0; i<i_max; i++) {
      G4double x = sens_x_min_ + i*binning;
      for (G4int j=0; j<j_max; j++) {
	G4double y = sens_y_min_ + j*binning;
	for (G4int k=0; k<k_max; k++) {
	  G4double z = sens_z_min_ + k*binning;
	  if ((x*x + y*y) < inner_radius_*inner_radius_) {
	    G4ThreeVector point(x, y, z);
	    sensitivity_vertices_.push_back(point);
	  }
	}
      }
    }
    G4cout << "Number of points in sensitivity map = " << sensitivity_vertices_.size()
	   << G4endl;
  }

}
