import pytest

import os
import subprocess


@pytest.mark.first
def test_create_nexus_output_file_full_body(config_tmpdir, output_tmpdir, NEXUSDIR, base_name_full_body, nexus_output_file_full_body):

     init_text = f"""
/PhysicsList/RegisterPhysics G4EmStandardPhysics_option4
/PhysicsList/RegisterPhysics G4DecayPhysics
/PhysicsList/RegisterPhysics G4RadioactiveDecayPhysics
/PhysicsList/RegisterPhysics G4OpticalPhysics
/PhysicsList/RegisterPhysics NexusPhysics
/PhysicsList/RegisterPhysics G4StepLimiterPhysics

/Geometry/RegisterGeometry FULLRING

/Generator/RegisterGenerator BACK2BACK

/Actions/RegisterTrackingAction DEFAULT
/Actions/RegisterEventAction DEFAULT
/Actions/RegisterRunAction DEFAULT

/nexus/RegisterMacro {config_tmpdir}/{base_name_full_body}.config.mac
"""
     init_path = os.path.join(config_tmpdir, base_name_full_body+'.init.mac')
     init_file = open(init_path,'w')
     init_file.write(init_text)
     init_file.close()

     config_text = f"""
/run/verbose 1
/event/verbose 0
/tracking/verbose 0

/Geometry/FullRingInfinity/depth 3. cm
/Geometry/FullRingInfinity/pitch 7. mm
/Geometry/FullRingInfinity/inner_radius 380. mm
/Geometry/FullRingInfinity/sipm_rows 278
/Geometry/FullRingInfinity/instrumented_faces 1
/Geometry/FullRingInfinity/cryostat_width 200 cm
/Geometry/FullRingInfinity/specific_vertex_X 0. cm
/Geometry/FullRingInfinity/specific_vertex_Y 0. cm
/Geometry/FullRingInfinity/specific_vertex_Z 0. cm

/Geometry/SiPMpet/efficiency 0.2
/Geometry/SiPMpet/visibility true
/Geometry/SiPMpet/time_binning 1. microsecond
/Geometry/SiPMpet/size 6. mm

/Generator/Back2back/region CENTER

/nexus/persistency/outputFile {output_tmpdir}/{base_name_full_body}
/nexus/random_seed 16062020

"""

     config_path = os.path.join(config_tmpdir, base_name_full_body+'.config.mac')
     config_file = open(config_path,'w')
     config_file.write(config_text)
     config_file.close()

     my_env    = os.environ
     nexus_exe = NEXUSDIR + '/nexus'
     command   = [nexus_exe, '-b', '-n', '20', init_path]
     p         = subprocess.run(command, check=True, env=my_env)

     return nexus_output_file_full_body
