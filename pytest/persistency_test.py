import pytest

import os
#import subprocess
import pandas as pd
import tables as tb
import numpy as np


def test_hdf5_structure():

     command = './nexus -b -n 1 pytest/test_macros/NEW_optical.init.mac'
     os.system(command)

     #my_env = os.environ.copy()
     #command = ['./nexus', '-b', '-n', '1', 'pytest/test_macros/NEW_optical.init.mac']
     #p = subprocess.Popen(command, env=my_env)
     #try:
     #     p.wait(timeout=120)
     #except TimeoutExpired:
     #     exit()

     output_file = 'pytest/NEW_electron_sim.h5'

     with tb.open_file(output_file) as h5out:

         assert 'MC'            in h5out.root
         assert 'particles'     in h5out.root.MC
         assert 'hits'          in h5out.root.MC
         assert 'sns_response'  in h5out.root.MC
         assert 'configuration' in h5out.root.MC

         particles = h5out.root.MC.particles
         pcolumns  = particles.colnames

         assert 'event_id'           in pcolumns
         assert 'particle_id'        in pcolumns
         assert 'particle_name'      in pcolumns
         assert 'primary'            in pcolumns
         assert 'mother_id'          in pcolumns
         assert 'initial_x'          in pcolumns
         assert 'initial_y'          in pcolumns
         assert 'initial_z'          in pcolumns
         assert 'initial_t'          in pcolumns
         assert 'final_x'            in pcolumns
         assert 'final_y'            in pcolumns
         assert 'final_z'            in pcolumns
         assert 'final_t'            in pcolumns
         assert 'initial_volume'     in pcolumns
         assert 'final_volume'       in pcolumns
         assert 'initial_momentum_x' in pcolumns
         assert 'initial_momentum_y' in pcolumns
         assert 'initial_momentum_z' in pcolumns
         assert 'final_momentum_x'   in pcolumns
         assert 'final_momentum_y'   in pcolumns
         assert 'final_momentum_z'   in pcolumns
         assert 'kin_energy'         in pcolumns
         assert 'length'             in pcolumns
         assert 'creator_proc'       in pcolumns
         assert 'final_proc'         in pcolumns


def test_particle_ids_of_hits_exist_in_particle_table():

    command = './nexus -b -n 1 pytest/test_macros/NEW_optical.init.mac'
    os.system(command)

    output_file = 'pytest/NEW_electron_sim.h5'

    hits = pd.read_hdf(output_file, 'MC/hits')
    hit_pids = hits.particle_id.unique()

    particles = pd.read_hdf(output_file, 'MC/particles')
    particle_ids = particles.particle_id.unique()

    assert np.all(np.isin(hit_pids, particle_ids))
