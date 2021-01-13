import pandas as pd
import numpy  as np


def test_sensor_ids(nexus_params):
     """
     Check that sensors are correctly numbered.
     """
     filename, nsipms, n_boards, sipms_per_board, board_ordering = nexus_params

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
          assert (sipm_ids % board_ordering).max() <=  sipms_per_board


def test_sensor_ids_pet_box(nexus_pet_box_params):
     """
     Check that sensors are correctly numbered for the PetBox geometry.
     """
     filename, nsipms, sipms_per_tile, init_sns_id, sensor_name = nexus_pet_box_params

     sns_response = pd.read_hdf(filename, 'MC/sns_response')
     sipm_ids     = sns_response.sensor_id.unique()

     assert 1 < len(sipm_ids) <= nsipms

     # No sensor position is repeated
     sns_positions = pd.read_hdf(filename, 'MC/sns_positions')
     assert len(sns_positions) == len(sns_positions.sensor_id.unique())
     assert min(sipm_ids) >= init_sns_id
     assert sns_positions.sensor_name.unique() == sensor_name

     sns_z_pos_left  = sns_positions[ sns_positions.sensor_id<nsipms/2].z.values
     sns_z_pos_right = sns_positions[~sns_positions.sensor_id<nsipms/2].z.values
     if len(sns_z_pos_left) > 0:
         assert len(np.unique(sns_z_pos_left)) == 1
         assert     np.unique(sns_z_pos_left)  < 0
         assert len(sns_z_pos_left) <= sipms_per_tile*4

     if len(sns_z_pos_right) > 0:
         assert len(np.unique(sns_z_pos_right)) == 1
         assert     np.unique(sns_z_pos_right)  < 0
         assert len(sns_z_pos_right) <= sipms_per_tile*4
