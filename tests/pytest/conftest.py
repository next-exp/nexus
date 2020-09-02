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


@pytest.fixture(scope = "module",
                params= ["nexus_full_output_file_new",
                         "nexus_full_output_file_next100"],
                ids   = ["new", "next100"])
def nexus_files(request):
    return request.getfixturevalue(request.param)



@pytest.fixture(scope = 'session')
def new_detector(output_tmpdir, full_base_name_new):
    det_fname       = os.path.join(output_tmpdir, full_base_name_new + '.h5')
    num_pmts        = 12
    num_boards      = 28
    sipms_per_board = 64
    board_ordering  = 1000
    return det_fname, num_pmts, num_boards, sipms_per_board, board_ordering


@pytest.fixture(scope = 'session')
def next100_detector(output_tmpdir, full_base_name_next100):
    det_fname       = os.path.join(output_tmpdir, full_base_name_next100 + '.h5')
    num_pmts        = 60
    num_boards      = 56
    sipms_per_board = 64
    board_ordering  = 1000
    return det_fname, num_pmts, num_boards, sipms_per_board, board_ordering


@pytest.fixture(scope = "module",
                params= ["new_detector",
                         "next100_detector"],
                ids   = ["new", "next100"])
def detectors(request):
    return request.getfixturevalue(request.param)
