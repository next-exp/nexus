import pytest

import glob
import os
import subprocess
import numpy as np

"""
This module runs all the example init macros contained in the macro folder.
Caveats:
1. Each config file needs to have the same name of the init file that calls it.
2. Examples of config files only are not run, for the time being.
3. Examples in the /old folder are excluded.
"""

@pytest.fixture(scope='module')
def macro_list(NEXUSDIR):

    all_macros = np.array(glob.glob(NEXUSDIR + '/macros/**/*.init.mac',
                                    recursive=True))
    example_macros = all_macros[['/old/' not in m for m in all_macros]]

    return example_macros



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
                if l1[0] == '/nexus/persistency/outputFile':
                    l2 = l1[1].split('/')
                    f_cp.write(l1[0] + ' ' + str(output_tmpdir) + '/' + l2[-1])
                else:
                    f_cp.write(l)

    return cp_init_macro


@pytest.mark.last
def test_run_examples(capsys, config_tmpdir, output_tmpdir, NEXUSDIR, macro_list):
    """Run example macros"""

    my_env = os.environ.copy()

    with capsys.disabled():
        print('')

    for macro in macro_list:
        init_macro = copy_and_modify_macro(config_tmpdir, output_tmpdir, macro)
        if macro == NEXUSDIR + '/macros/PETit_ring_lutable.init.mac':
            n = 1
        else:
            n = 20
        nexus_exe  = NEXUSDIR + '/nexus'
        command = [nexus_exe, '-b', '-n', str(n), init_macro]
        with capsys.disabled():
            print(f'Running {macro}')
        p  = subprocess.run(command, check=True, env=my_env)
