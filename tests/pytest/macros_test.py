import pytest

import glob
import os
import subprocess
import numpy as np

all_macros = np.array(glob.glob('macros/**/*.init.mac', recursive=True))
full       = np.array(['full'  in m for m in all_macros])
lu_table   = np.array(['table' in m for m in all_macros])

full_macros = all_macros[full | lu_table]
fast_macros = all_macros[~(full | lu_table)]


def copy_and_modify_macro(config_tmpdir, output_tmpdir, init_macro):
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


def go_through_macro_list(capmanager, config_tmpdir, output_tmpdir, macro_list):
    my_env = os.environ.copy()

    for macro in macro_list:
        init_macro = copy_and_modify_macro(config_tmpdir, output_tmpdir, macro)
        command = ['./nexus', '-b', '-n', '1', init_macro]
        with capmanager.global_and_fixture_disabled():
            print(f'Running {macro}')
        p  = subprocess.run(command, check=True, env=my_env)



def test_run_fast_examples(request, config_tmpdir, output_tmpdir):
    capmanager = request.config.pluginmanager.getplugin("capturemanager")

    with capmanager.global_and_fixture_disabled():
        print(f'*** Fast simulations ***')

    go_through_macro_list(capmanager, config_tmpdir, output_tmpdir, fast_macros)


def test_run_full_examples(request, config_tmpdir, output_tmpdir):
    capmanager = request.config.pluginmanager.getplugin("capturemanager")

    with capmanager.global_and_fixture_disabled():
        print('')
        print(f'*** Full simulations - some of them are slow ***')

    go_through_macro_list(capmanager, config_tmpdir, output_tmpdir, full_macros)
