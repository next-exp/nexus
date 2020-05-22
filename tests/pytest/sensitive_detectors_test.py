import pytest

import os
import subprocess
import pandas as pd


@pytest.mark.first
def test_create_nexus_output_file(config_tmpdir, output_tmpdir, base_name, nexus_output_file):

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

/nexus/RegisterMacro {config_tmpdir}/{base_name}.config.mac
"""
     init_path = os.path.join(config_tmpdir, base_name+'.init.mac')
     init_file = open(init_path,'w')
     init_file.write(init_text)
     init_file.close()

     config_text = f"""
/run/verbose 1
/event/verbose 0
/tracking/verbose 0

/Geometry/Next100/elfield true
/Geometry/Next100/EL_field 10 kV/cm
/Geometry/Next100/max_step_size 1. mm
/Geometry/Next100/pressure 15. bar
/Geometry/Next100/sc_yield 10000 1/MeV
/Geometry/PmtR11410/SD_depth 3

/Generator/SingleParticle/particle e-
/Generator/SingleParticle/min_energy 50. keV
/Generator/SingleParticle/max_energy 50. keV
/Generator/SingleParticle/region ACTIVE

/nexus/persistency/outputFile {output_tmpdir}/{base_name}
/nexus/random_seed 21051817

"""

     config_path = os.path.join(config_tmpdir, base_name+'.config.mac')
     config_file = open(config_path,'w')
     config_file.write(config_text)
     config_file.close()

     my_env = os.environ.copy()
     command = ['./nexus', '-b', '-n', '1', init_path]
     p = subprocess.run(command, check=True, env=my_env)

     return nexus_output_file


def test_sensor_ids_are_sensible(nexus_output_file):
     """
     Check that we are not registering all photoelectrons
     with the same sensor ID, for each type of sensor.
     """

     sns_response = pd.read_hdf(nexus_output_file, 'MC/sns_response')

     pmts  = sns_response[sns_response.sensor_id < 100]
     sipms = sns_response[sns_response.sensor_id > 100]

     pmt_ids   = pmts.sensor_id.unique()
     sipm_ids = sipms.sensor_id.unique()

     assert len(pmt_ids ) > 1
     assert len(sipm_ids) > 1
