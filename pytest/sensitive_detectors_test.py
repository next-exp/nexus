import pytest

import os
import subprocess
import pandas as pd
import tables as tb
import numpy as np

my_env = os.environ.copy()
command = ['./nexus', '-b', '-n', '1', 'pytest/test_macros/NEXT100_sensdet.init.mac']
p = subprocess.run(command, check=True, env=my_env)
output_file = 'pytest/NEXT100_sensdet.h5'


def test_sensor_ids_are_sensible():
     """
     Check that we are not registering all photoelectrons
     with the same sensor ID, for each type of sensor.
     """

     sns_response = pd.read_hdf(output_file, 'MC/sns_response')

     pmts  = sns_response[sns_response.sensor_id < 100]
     sipms = sns_response[sns_response.sensor_id > 100]

     pmt_ids   = pmts.sensor_id.unique()
     sipm_ids = sipms.sensor_id.unique()

     assert len(pmt_ids ) > 1
     assert len(sipm_ids) > 1
