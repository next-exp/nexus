import pytest
import os


@pytest.fixture(scope = 'session')
def config_tmpdir(tmpdir_factory):
    return tmpdir_factory.mktemp('configure_tests')


@pytest.fixture(scope = 'session')
def output_tmpdir(tmpdir_factory):
    return tmpdir_factory.mktemp('output_files')


@pytest.fixture(scope = 'session',
                params = ['NEXT100_full_electron.h5'])
def nexus_output_file(request, output_tmpdir):
    return os.path.join(output_tmpdir, request.param)


@pytest.fixture(scope = 'session')
def base_name():
    return 'NEXT100_full_electron'
