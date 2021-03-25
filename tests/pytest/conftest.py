import pytest
import os

@pytest.fixture(scope = 'session')
def NEXUSDIR():
    return os.environ['NEXUSDIR']


@pytest.fixture(scope = 'session')
def config_tmpdir(tmpdir_factory):
    return tmpdir_factory.mktemp('configure_tests')


@pytest.fixture(scope = 'session')
def output_tmpdir(tmpdir_factory):
    return tmpdir_factory.mktemp('output_files')


@pytest.fixture(scope = 'session')
def full_base_name_new():
    return 'NEW_full_electron'


@pytest.fixture(scope = 'session')
def nexus_full_output_file_new(output_tmpdir, full_base_name_new):
    return os.path.join(output_tmpdir, full_base_name_new + '.h5')


@pytest.fixture(scope = 'session')
def full_base_name_next100():
    return 'NEXT100_full_electron'


@pytest.fixture(scope = 'session')
def nexus_full_output_file_next100(output_tmpdir, full_base_name_next100):
    return os.path.join(output_tmpdir, full_base_name_next100 + '.h5')


@pytest.fixture(scope = 'session')
def full_base_name_flex100():
    return 'FLEX100_full_electron'


@pytest.fixture(scope = 'session')
def nexus_full_output_file_flex100(output_tmpdir, full_base_name_flex100):
    return os.path.join(output_tmpdir, full_base_name_flex100 + '.h5')


@pytest.fixture(scope = 'session')
def full_base_name_demopp():
    return 'DEMOPP_full_electron'


@pytest.fixture(scope = 'session')
def nexus_full_output_file_demopp(output_tmpdir, full_base_name_demopp):
    return os.path.join(output_tmpdir, full_base_name_demopp + '.h5')


@pytest.fixture(scope = 'session')
def new_detector(nexus_full_output_file_new):
    pmt_ids         = [i for i in range(12)]
    board_ids       = [i for i in range(1, 29)]
    sipms_per_board = 64
    board_ordering  = 1000
    return nexus_full_output_file_new, pmt_ids, board_ids, sipms_per_board, board_ordering


@pytest.fixture(scope = 'session')
def next100_detector(nexus_full_output_file_next100):
    pmt_ids         = [i for i in range(60)]
    board_ids       = [i for i in range(1, 57)]
    sipms_per_board = 64
    board_ordering  = 1000
    return nexus_full_output_file_next100, pmt_ids, board_ids, sipms_per_board, board_ordering


@pytest.fixture(scope = 'session')
def flex100_detector(nexus_full_output_file_flex100):
    pmt_ids         = [i for i in range(60)]
    board_ids       = [1]
    sipms_per_board = 3093
    board_ordering  = 3093
    return nexus_full_output_file_flex100, pmt_ids,  board_ids, sipms_per_board, board_ordering


@pytest.fixture(scope = 'session')
def demopp_detector(nexus_full_output_file_demopp):
    pmt_ids         = [2, 3, 4]
    board_ids       = [14, 15, 16, 17]
    sipms_per_board = 64
    board_ordering  = 1000
    return nexus_full_output_file_demopp, pmt_ids, board_ids, sipms_per_board, board_ordering


@pytest.fixture(scope = "module",
                params= ["new_detector",
                         "next100_detector",
                         "flex100_detector",
                         "demopp_detector"],
                ids   = ["new", "next100", "flex100", "demopp"])
def detectors(request):
    return request.getfixturevalue(request.param)
