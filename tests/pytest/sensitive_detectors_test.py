import pandas as pd


def test_sensor_ids(nexus_files):
     """
     Check that sensors are correctly numbered.
     """
     filename, nsipms, n_boards, sipms_per_board, board_ordering = nexus_files

     sns_response = pd.read_hdf(filename, 'MC/sns_response')
     sipm_ids     = sns_response.sensor_id.unique()

     assert 1 < len(sipm_ids) <= nsipms

     # No sensor position is repeated
     sns_positions = pd.read_hdf(filename, 'MC/sns_positions')
     assert len(sns_positions) == len(sns_positions.sensor_id.unique())

     if n_boards != 0:
          # SiPM-board numbering is correct
          assert (sipm_ids // board_ordering).min() >= 1
          assert (sipm_ids // board_ordering).max() <= n_boards

          # SiPM number inside boards is correct
          assert (sipm_ids % board_ordering).min() >= 0
          print(sipm_ids)
          assert (sipm_ids % board_ordering).max() <=  sipms_per_board
