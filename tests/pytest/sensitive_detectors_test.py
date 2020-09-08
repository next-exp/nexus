import pandas     as pd
#from   pytest import mark



def test_sensors_numbering(detectors):
    """
    Check that sensors are correctly numbered.
    """

    fname, num_pmts, num_boards, sipms_per_board, board_ordering = detectors

    num_sipms = num_boards * sipms_per_board

    # Checking that only 2 types of sensors exist
    sns_positions = pd.read_hdf(fname, 'MC/sns_positions')
    assert len(sns_positions.sensor_name.unique()) == 2

    # Assert the total number of sensors is correct
    pmt_ids  = sns_positions[sns_positions.sensor_name == "PmtR11410"].sensor_id
    sipm_ids = sns_positions[sns_positions.sensor_name.str.contains("SiPM")].sensor_id
    assert     len(pmt_ids)  == num_pmts
    assert 1 < len(sipm_ids) <= num_sipms

    # Assert PMT numbering is correct
    assert pmt_ids.sort_values().tolist() == list(range(num_pmts))

    # Assert SiPM-Boards numbering is correct
    if 'FLEX100' not in fname:
        assert (sipm_ids // board_ordering).min() >= 1
        assert (sipm_ids // board_ordering).max() <= num_boards

    # Assert SiPM number inside Boards is correct
    if 'FLEX100' not in fname:
        assert (sipm_ids % board_ordering).min() >= 0
        assert (sipm_ids % board_ordering).max() <  sipms_per_board

    # Assert there is no sensor positions repeated
    assert len(sns_positions) == len(sns_positions.sensor_id.unique())
