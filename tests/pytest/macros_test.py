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


def test_run_fast_examples(request):
    capmanager = request.config.pluginmanager.getplugin("capturemanager")
    my_env  = os.environ.copy()

    with capmanager.global_and_fixture_disabled():
        print(f'*** Fast simulations ***')

    for macro in fast_macros:
        command = ['./nexus', '-b', '-n', '1', macro]
        with capmanager.global_and_fixture_disabled():
            print(f'Running {macro}')
        p  = subprocess.run(command, check=True, env=my_env)


def test_run_full_examples(request):
    capmanager = request.config.pluginmanager.getplugin("capturemanager")
    my_env  = os.environ.copy()

    with capmanager.global_and_fixture_disabled():
        print('')
        print(f'*** Full simulations - some of them are slow ***')

    for macro in full_macros:
        command = ['./nexus', '-b', '-n', '1', macro]
        with capmanager.global_and_fixture_disabled():
            print(f'Running {macro}')
        p  = subprocess.run(command, check=True, env=my_env)
