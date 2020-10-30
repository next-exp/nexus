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
    pmt_ids         = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
    board_ids       = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28]
    sipms_per_board = 64
    board_ordering  = 1000
    return nexus_full_output_file_new, pmt_ids, board_ids, sipms_per_board, board_ordering


@pytest.fixture(scope = 'session')
def next100_detector(nexus_full_output_file_next100):
    pmt_ids         = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59]
    board_ids       = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56]
    sipms_per_board = 64
    board_ordering  = 1000
    return nexus_full_output_file_next100, pmt_ids, board_ids, sipms_per_board, board_ordering


@pytest.fixture(scope = 'session')
def flex100_detector(nexus_full_output_file_flex100):
    num_pmts        = 60
    num_boards      = 1
    sipms_per_board = 3093
    board_ordering  = 3093
    return nexus_full_output_file_flex100, num_pmts, num_boards, sipms_per_board, board_ordering


@pytest.fixture(scope = 'session')
def demopp_detector(nexus_full_output_file_demopp):
    pmt_ids         = [10, 11, 12]
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
