import pandas as pd


def test_sensor_ids_are_sensible(nexus_files):
     """
     Check that we are not registering all photoelectrons
     with the same sensor ID, for each type of sensor.
     """

     sns_response = pd.read_hdf(nexus_files, 'MC/sns_response')

     pmts  = sns_response[sns_response.sensor_id < 100]
     sipms = sns_response[sns_response.sensor_id > 100]

     pmt_ids   = pmts.sensor_id.unique()
     sipm_ids = sipms.sensor_id.unique()

     assert len(pmt_ids ) > 1
     assert len(sipm_ids) > 1
