import pytest

import os
import subprocess


@pytest.mark.first
def test_create_nexus_output_file_next100(config_tmpdir, output_tmpdir, NEXUSDIR, full_base_name_next100, nexus_full_output_file_next100):

     init_text = f"""
/PhysicsList/RegisterPhysics G4EmStandardPhysics_option4
/PhysicsList/RegisterPhysics G4DecayPhysics
/PhysicsList/RegisterPhysics G4RadioactiveDecayPhysics
/PhysicsList/RegisterPhysics G4OpticalPhysics
/PhysicsList/RegisterPhysics NexusPhysics
/PhysicsList/RegisterPhysics G4StepLimiterPhysics

/Geometry/RegisterGeometry NEXT100_OPT

/Generator/RegisterGenerator SINGLE_PARTICLE

/Actions/RegisterTrackingAction DEFAULT
/Actions/RegisterEventAction DEFAULT
/Actions/RegisterRunAction DEFAULT

/nexus/RegisterMacro {config_tmpdir}/{full_base_name_next100}.config.mac
"""
     init_path = os.path.join(config_tmpdir, full_base_name_next100+'.init.mac')
     init_file = open(init_path,'w')
     init_file.write(init_text)
     init_file.close()

     config_text = f"""
/run/verbose 1
/event/verbose 0
/tracking/verbose 0

/Geometry/Next100/elfield true
/Geometry/Next100/EL_field 13 kV/cm
/Geometry/Next100/max_step_size 1. mm
/Geometry/Next100/pressure 15. bar
/Geometry/Next100/sc_yield 10000 1/MeV

/Generator/SingleParticle/particle e-
/Generator/SingleParticle/min_energy 100. keV
/Generator/SingleParticle/max_energy 100. keV
/Generator/SingleParticle/region CENTER

/nexus/persistency/outputFile {output_tmpdir}/{full_base_name_next100}
/nexus/random_seed 21051817

"""

     config_path = os.path.join(config_tmpdir, full_base_name_next100+'.config.mac')
     config_file = open(config_path,'w')
     config_file.write(config_text)
     config_file.close()

     my_env    = os.environ
     nexus_exe = NEXUSDIR + '/bin/nexus'
     command   = [nexus_exe, '-b', '-n', '1', init_path]
     p         = subprocess.run(command, check=True, env=my_env)

     return nexus_full_output_file_next100


@pytest.mark.first
def test_create_nexus_output_file_new(config_tmpdir, output_tmpdir, NEXUSDIR, full_base_name_new, nexus_full_output_file_new):

     init_text = f"""
/PhysicsList/RegisterPhysics G4EmStandardPhysics_option4
/PhysicsList/RegisterPhysics G4DecayPhysics
/PhysicsList/RegisterPhysics G4RadioactiveDecayPhysics
/PhysicsList/RegisterPhysics G4OpticalPhysics
/PhysicsList/RegisterPhysics NexusPhysics
/PhysicsList/RegisterPhysics G4StepLimiterPhysics

/Geometry/RegisterGeometry NEXT_NEW

/Generator/RegisterGenerator SINGLE_PARTICLE

/Actions/RegisterTrackingAction DEFAULT
/Actions/RegisterEventAction DEFAULT
/Actions/RegisterRunAction DEFAULT

/nexus/RegisterMacro {config_tmpdir}/{full_base_name_new}.config.mac
"""
     init_path = os.path.join(config_tmpdir, full_base_name_new+'.init.mac')
     init_file = open(init_path,'w')
     init_file.write(init_text)
     init_file.close()

     config_text = f"""
/run/verbose 1
/event/verbose 0
/tracking/verbose 0

/Geometry/NextNew/elfield true
/Geometry/NextNew/EL_field 13 kV/cm
/Geometry/NextNew/max_step_size 1. mm
/Geometry/NextNew/pressure 15. bar
/Geometry/NextNew/sc_yield 10000 1/MeV

/Generator/SingleParticle/particle e-
/Generator/SingleParticle/min_energy 100. keV
/Generator/SingleParticle/max_energy 100. keV
/Generator/SingleParticle/region CENTER

/nexus/persistency/outputFile {output_tmpdir}/{full_base_name_new}
/nexus/random_seed 21051817

"""

     config_path = os.path.join(config_tmpdir, full_base_name_new+'.config.mac')
     config_file = open(config_path,'w')
     config_file.write(config_text)
     config_file.close()

     my_env    = os.environ
     nexus_exe = NEXUSDIR + '/bin/nexus'
     command   = [nexus_exe, '-b', '-n', '1', init_path]
     p         = subprocess.run(command, check=True, env=my_env)

     return nexus_full_output_file_new


@pytest.mark.first
def test_create_nexus_output_file_demopp(config_tmpdir, output_tmpdir, NEXUSDIR, full_base_name_demopp, nexus_full_output_file_demopp):

     init_text = f"""
/PhysicsList/RegisterPhysics G4EmStandardPhysics_option4
/PhysicsList/RegisterPhysics G4DecayPhysics
/PhysicsList/RegisterPhysics G4RadioactiveDecayPhysics
/PhysicsList/RegisterPhysics G4OpticalPhysics
/PhysicsList/RegisterPhysics NexusPhysics
/PhysicsList/RegisterPhysics G4StepLimiterPhysics

/Geometry/RegisterGeometry NEXT_DEMO

/Generator/RegisterGenerator SINGLE_PARTICLE

/Actions/RegisterTrackingAction DEFAULT
/Actions/RegisterEventAction DEFAULT
/Actions/RegisterRunAction DEFAULT

/nexus/RegisterMacro {config_tmpdir}/{full_base_name_demopp}.config.mac
"""
     init_path = os.path.join(config_tmpdir, full_base_name_demopp+'.init.mac')
     init_file = open(init_path,'w')
     init_file.write(init_text)
     init_file.close()

     config_text = f"""
/run/verbose 1
/event/verbose 0
/tracking/verbose 0

/Geometry/NextDemo/elfield true
/Geometry/NextDemo/EL_field_intensity 13 kV/cm
/Geometry/NextDemo/max_step_size 1. mm
/Geometry/NextDemo/pressure 10. bar
/Geometry/NextDemo/sc_yield 10000 1/MeV

/Geometry/NextDemo/specific_vertex_X 0. mm
/Geometry/NextDemo/specific_vertex_Y 0. mm
/Geometry/NextDemo/specific_vertex_Z 10. cm

/Generator/SingleParticle/particle e-
/Generator/SingleParticle/min_energy 100. keV
/Generator/SingleParticle/max_energy 100. keV
/Generator/SingleParticle/region AD_HOC

/nexus/persistency/outputFile {output_tmpdir}/{full_base_name_demopp}
/nexus/random_seed 21051817

"""

     config_path = os.path.join(config_tmpdir, full_base_name_demopp+'.config.mac')
     config_file = open(config_path,'w')
     config_file.write(config_text)
     config_file.close()

     my_env    = os.environ
     nexus_exe = NEXUSDIR + '/bin/nexus'
     command   = [nexus_exe, '-b', '-n', '1', init_path]
     p         = subprocess.run(command, check=True, env=my_env)

     return nexus_full_output_file_demopp
