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


@pytest.fixture(scope="module",
         params=["nexus_output_file_full_body"],
         ids=["full_body"])
def nexus_files(request):
    return request.getfixturevalue(request.param)
