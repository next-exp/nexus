import pytest
import os


@pytest.fixture(scope = 'session')
def config_tmpdir(tmpdir_factory):
    return tmpdir_factory.mktemp('configure_tests')


@pytest.fixture(scope = 'session')
def output_tmpdir(tmpdir_factory):
    return tmpdir_factory.mktemp('output_files')


@pytest.fixture(scope = 'session')
def full_base_name():
    return 'NEXT100_full_electron'


@pytest.fixture(scope = 'session')
def nexus_full_output_file(output_tmpdir, full_base_name):
    return os.path.join(output_tmpdir, full_base_name+'.h5')
