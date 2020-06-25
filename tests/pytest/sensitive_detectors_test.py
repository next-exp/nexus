import pandas     as pd
#from   pytest import mark


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


def test_sensors_numbering(nexus_files):
    """
    Check that sensors are correctly numbered.
    """

    # Getting detector name
    config = pd.read_hdf(nexus_files, 'MC/configuration')
    config.set_index("param_key", inplace = True)
    det_name = config.loc["/Geometry/RegisterGeometry"].param_value

    # Settint detector data
    if det_name == "NEXT_NEW":
        num_pmts        = 12
        num_boards      = 28
        sipms_per_board = 64
        board_ordering  = 1000
    elif (det_name == "NEXT100_OPT") or (det_name == "NEXT100"):
        num_pmts        = 60
        num_boards      = 56
        sipms_per_board = 64
        board_ordering  = 1000
    num_sipms = num_boards * sipms_per_board

    # Checking that only 2 types of sensors exist
    sns_positions = pd.read_hdf(nexus_files, 'MC/sns_positions')
    assert len(sns_positions.sensor_name.unique()) == 2

    # Assert the total number of sensors is correct
    pmt_ids  = sns_positions[sns_positions.sensor_name == "PmtR11410"].sensor_id
    sipm_ids = sns_positions[sns_positions.sensor_name == "SiPM"].sensor_id
    assert     len(pmt_ids)  == num_pmts
    assert 1 < len(sipm_ids) <= num_sipms

    # Assert PMT numbering is correct
    assert pmt_ids.sort_values().tolist() == list(range(num_pmts))

    # Assert SiPM-Boards numbering is correct
    assert  (sipm_ids // board_ordering).min() >= 1
    assert  (sipm_ids // board_ordering).max() <= num_boards

    # Assert SiPM number inside Boards is correct
    assert  (sipm_ids % board_ordering).min() >= 0
    assert  (sipm_ids % board_ordering).max() <  sipms_per_board


#@mark.parametrize("i_fname,num_pmts,num_boards,sipms_per_board,board_ordering",
#                  [(nexus_full_output_file_new    , 12, 28, 64, 1000),
#                   (nexus_full_output_file_next100, 60, 56, 64, 1000)])
#def test_sensors_numbering(i_fname, num_pmts, num_boards,
#                           sipms_per_board, board_ordering):
#    """
#    Check that sensors are correctly numbered.
#    """
#
#    # NEW data
#    num_sipms       = num_boards * sipms_per_board
#
#    # Checking that only 2 types of sensors exist
#    sns_positions = pd.read_hdf(i_fname, 'MC/sns_positions')
#    assert len(sns_positions.sensor_name.unique()) == 2
#
#    # Assert the total number of sensors is correct
#    pmt_ids  = sns_positions[sns_positions.sensor_name == "PmtR11410"].sensor_id
#    sipm_ids = sns_positions[sns_positions.sensor_name == "SiPM"].sensor_id
#    assert     len(pmt_ids)  == num_pmts
#    assert 1 < len(sipm_ids) <= num_sipms
#
#    # Assert PMT numbering is correct
#    assert pmt_ids.sort_values().tolist() == list(range(num_pmts))
#
#    # Assert SiPM-Boards numbering is correct
#    assert  (sipm_ids // board_ordering).min() >= 1
#    assert  (sipm_ids // board_ordering).max() <= num_boards
#
#    # Assert SiPM number inside Boards is correct
#    assert  (sipm_ids % board_ordering).min() >= 0
#    assert  (sipm_ids % board_ordering).max() <  sipms_per_board
