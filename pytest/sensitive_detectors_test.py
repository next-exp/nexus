import pandas as pd
import numpy  as np


def test_sensors_numbering(detectors):
    """
    Check that sensors are correctly numbered.
    """

    def test(filename, pmt_ids, board_ids, sipms_per_board, board_ordering):

        num_sipms = len(board_ids) * sipms_per_board

        # Checking that only 2 types of sensors exist
        sns_positions = pd.read_hdf(filename, 'MC/sns_positions')
        assert len(sns_positions.sensor_name.unique()) == 2

        # Assert the total number of sensors is correct
        sns_sipm_ids = sns_positions[sns_positions.sensor_name.str.contains("SiPM")].sensor_id
        print(sns_sipm_ids)
        assert 1 < len(sns_sipm_ids) <= num_sipms

        # Assert PMT numbering is correct
        sns_pmt_ids = sns_positions[sns_positions.sensor_name == "PmtR11410"].sensor_id
        assert np.all(sns_pmt_ids.sort_values().tolist() == pmt_ids)

        # Assert SiPM-Boards numbering is correct
        if 'FLEX100' not in filename:
            assert (sns_sipm_ids // board_ordering).min() >= 1
            assert (sns_sipm_ids // board_ordering).max() in board_ids

        # Assert SiPM number inside Boards is correct
        if 'FLEX100' not in filename:
            assert (sns_sipm_ids % board_ordering).min() >= 0
            assert (sns_sipm_ids % board_ordering).max() <  sipms_per_board

        # Assert there is no sensor positions repeated
        assert len(sns_positions) == len(sns_positions.sensor_id.unique())


    filename, pmt_ids, board_ids, sipms_per_board, board_ordering = detectors

    if "DEMOPP" in filename:
        for run in ["run5", "run7", "run8", "run9", "run10"]:
            test(filename.format(run=run), pmt_ids, board_ids, sipms_per_board, board_ordering)
    else:
        test(filename, pmt_ids, board_ids, sipms_per_board, board_ordering)
