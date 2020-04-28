import pytest

import os
import subprocess
import pandas as pd
import tables as tb
import numpy as np


@pytest.fixture(scope="module")
def input_file(request):
     my_env = os.environ.copy()
     command = ['./nexus', '-b', '-n', '1', 'tests/pytest/test_macros/NEXT100_sensdet.init.mac']
     p = subprocess.run(command, check=True, env=my_env)
     input_file_for_tests = 'tests/pytest/NEXT100_sensdet.h5'

     return input_file_for_tests


def test_sensor_ids_are_sensible(input_file):
     """
     Check that we are not registering all photoelectrons
     with the same sensor ID, for each type of sensor.
     """

     sns_response = pd.read_hdf(input_file, 'MC/sns_response')

     pmts  = sns_response[sns_response.sensor_id < 100]
     sipms = sns_response[sns_response.sensor_id > 100]

     pmt_ids   = pmts.sensor_id.unique()
     sipm_ids = sipms.sensor_id.unique()

     assert len(pmt_ids ) > 1
     assert len(sipm_ids) > 1
