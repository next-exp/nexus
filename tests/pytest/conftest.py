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
def base_name_full_body():
    return 'PET_full_body_sd_test'


@pytest.fixture(scope = 'session')
def nexus_output_file_full_body(output_tmpdir, base_name_full_body):
    return os.path.join(output_tmpdir, base_name_full_body+'.h5')


@pytest.fixture(scope = 'session')
def base_name_ring_tiles():
    return 'PET_ring_tiles_sd_test'


@pytest.fixture(scope = 'session')
def nexus_output_file_ring_tiles(output_tmpdir, base_name_ring_tiles):
    return os.path.join(output_tmpdir, base_name_ring_tiles+'.h5')


@pytest.fixture(scope="module",
         params=["nexus_output_file_full_body", "nexus_output_file_ring_tiles"],
         ids=["full_body", "ring_tiles"])
def nexus_files(request):
    return request.getfixturevalue(request.param)
