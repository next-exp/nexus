import pandas as pd


def test_sensor_ids_are_sensible(nexus_files):
     """
     Check that we are not registering all photoelectrons
     with the same sensor ID.
     """

     sns_response = pd.read_hdf(nexus_files, 'MC/sns_response')
     sipm_ids     = sns_response.sensor_id.unique()

     assert len(sipm_ids) > 1
