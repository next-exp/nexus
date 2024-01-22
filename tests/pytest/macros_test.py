import pytest

from pytest import mark

import glob
import os
import subprocess
import numpy as np

"""
This module runs all the example init macros contained in the macro folder.
Caveats:
1. Each config file needs to have the same name of the init file that calls it.
2. Examples of config files only are not run, for the time being.
"""

@pytest.fixture(scope='module')
def check_list(NEXUSDIR):

    macro_list = [NEXUSDIR + "/macros/DEMOPP_grid.init.mac",
                  NEXUSDIR + "/macros/DEMOPP_plate.init.mac",
                  NEXUSDIR + "/macros/NEW.init.mac",
                  NEXUSDIR + "/macros/NEXT100.init.mac",
                  NEXUSDIR + "/macros/NEXT100_full.init.mac",
                  NEXUSDIR + "/macros/NextFlex_fullKr.init.mac",
                  NEXUSDIR + "/macros/NextTonScale.init.mac",
                  NEXUSDIR + "/macros/black_box.init.mac"]

    return macro_list


@pytest.fixture(scope='module')
def macro_list(NEXUSDIR):

    all_macros = np.array(glob.glob(NEXUSDIR + '/macros/**/*.init.mac', recursive=True))
    full       = np.array(['full'  in m for m in all_macros])
    lu_table   = np.array(['table' in m for m in all_macros])

    full_macros = all_macros[  full | lu_table]
    fast_macros = all_macros[~(full | lu_table)]

    return fast_macros, full_macros


def copy_and_modify_macro(config_tmpdir, output_tmpdir, init_macro):
    """
    Copy the init and config macro to a temporary directory, modifying
    the fields related to the path of the config and the output file.
    """
    init_name  = init_macro.split('/')[-1]
    config_macro = init_macro.replace('init', 'config')
    config_name  = config_macro.split('/')[-1]

    cp_init_macro   = os.path.join(config_tmpdir, init_name)
    cp_config_macro = os.path.join(config_tmpdir, config_name)

    with open(init_macro,'r') as f:
        with open(cp_init_macro,'w') as f_cp:
            lines = f.readlines()
            for l in lines:
                l1 = l.split(' ')
                if l1[0] == '/nexus/RegisterMacro':
                    l2 = l1[1].split('/')
                    f_cp.write(l1[0] + ' ' + str(config_tmpdir) + '/' + l2[-1])
                else:
                    f_cp.write(l)

    with open(config_macro,'r') as f:
        with open(cp_config_macro,'w') as f_cp:
            lines = f.readlines()
            for l in lines:
                l1 = l.split(' ')
                if l1[0] == '/nexus/persistency/output_file':
                    l2 = l1[1].split('/')
                    f_cp.write(l1[0] + ' ' + str(output_tmpdir) + '/' + l2[-1])
                elif l1[0] == '/Actions/ValidationTrackingAction/valid_file':
                    l2 = l1[1].split('/')
                    f_cp.write(l1[0] + ' ' + str(output_tmpdir) + '/' + l2[-1])
                elif l1[0] == '/Actions/MuonsEventAction/stringHist':
                    l2 = l1[1].split('/')
                    f_cp.write(l1[0] + ' ' + str(output_tmpdir) + '/' + l2[-1])
                elif l1[0] == '/Generator/Decay0Interface/decay_file':
                    l2 = l1[1].split('/')
                    f_cp.write(l1[0] + ' ' + str(output_tmpdir) + '/' + l2[-1])
                else:
                    f_cp.write(l)

    return cp_init_macro


def execute_overlap_checks(capsys, config_tmpdir, output_tmpdir, NEXUSDIR, check_list):
    my_env = os.environ.copy()

    for macro in check_list:
        init_macro = copy_and_modify_macro(config_tmpdir, output_tmpdir, macro)
        nexus_exe  = NEXUSDIR + '/bin/nexus'
        command    = [nexus_exe, '-b', '-o', init_macro]
        with capsys.disabled():
            print(f'Checking {macro}')
        p  = subprocess.run(command, check=True, env=my_env)


def execute_example_jobs(capsys, config_tmpdir, output_tmpdir, NEXUSDIR, macro_list):
    my_env = os.environ.copy()

    for macro in macro_list:
        init_macro = copy_and_modify_macro(config_tmpdir, output_tmpdir, macro)
        nexus_exe  = NEXUSDIR + '/bin/nexus'
        command    = [nexus_exe, '-b', '-n', '1', init_macro]
        with capsys.disabled():
            print(f'Running {macro}')
        p  = subprocess.run(command, check=True, env=my_env)


@mark.slow
def test_run_overlap_check(capsys, config_tmpdir, output_tmpdir, NEXUSDIR, check_list):

    with capsys.disabled():
        print('')
        print(f'*** Overlap checks ***')

    execute_overlap_checks(capsys, config_tmpdir, output_tmpdir, NEXUSDIR, check_list)

@pytest.mark.order('second_to_last')
def test_run_fast_examples(capsys, config_tmpdir, output_tmpdir, NEXUSDIR, macro_list):
    """Run fast simulation macros"""

    with capsys.disabled():
        print('')
        print(f'*** Fast simulations ***')

    execute_example_jobs(capsys, config_tmpdir, output_tmpdir, NEXUSDIR, macro_list[0])


@pytest.mark.order('last')
def test_run_full_examples(capsys, config_tmpdir, output_tmpdir, NEXUSDIR, macro_list):
    """Run full simulation macros"""

    with capsys.disabled():
        print('')
        print(f'*** Full simulations - some of them are slow ***')

    execute_example_jobs(capsys, config_tmpdir, output_tmpdir, NEXUSDIR, macro_list[1])
