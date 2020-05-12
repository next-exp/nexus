import pytest

import os
import subprocess
import pandas as pd
import tables as tb
import numpy as np


@pytest.fixture(scope="module")
def input_file(request):
     my_env = os.environ.copy()
     command = ['./nexus', '-b', '-n', '1', 'tests/pytest/test_macros/NEXT100_optical.init.mac']
     p = subprocess.run(command, check=True, env=my_env)
     input_file_for_tests = 'tests/pytest/NEXT100_electron_full.h5'

     return input_file_for_tests



def test_hdf5_structure(input_file):
     """ Check that the hdf5 table structure is the correct one."""

     with tb.open_file(input_file) as h5out:

         assert 'MC'            in h5out.root
         assert 'particles'     in h5out.root.MC
         assert 'hits'          in h5out.root.MC
         assert 'sns_response'  in h5out.root.MC
         assert 'configuration' in h5out.root.MC
         assert 'sns_positions' in h5out.root.MC


         pcolumns = h5out.root.MC.particles.colnames

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


         hcolumns = h5out.root.MC.hits.colnames

         assert 'event_id'    in hcolumns
         assert 'x'           in hcolumns
         assert 'y'           in hcolumns
         assert 'z'           in hcolumns
         assert 'time'        in hcolumns
         assert 'energy'      in hcolumns
         assert 'label'       in hcolumns
         assert 'particle_id' in hcolumns
         assert 'hit_id'      in hcolumns


         scolumns = h5out.root.MC.sns_response.colnames

         assert 'event_id'  in scolumns
         assert 'sensor_id' in scolumns
         assert 'time_bin'  in scolumns
         assert 'charge'    in scolumns


         sposcolumns = h5out.root.MC.sns_positions.colnames

         assert 'sensor_id'   in sposcolumns
         assert 'sensor_name' in sposcolumns
         assert 'x'           in sposcolumns
         assert 'y'           in sposcolumns
         assert 'z'           in sposcolumns



def test_particle_ids_of_hits_exist_in_particle_table(input_file):
    """
    Check that the particle IDs of the hits are also contained
    in the particle table.
    """

    hits = pd.read_hdf(input_file, 'MC/hits')
    hit_pids = hits.particle_id.unique()

    particles = pd.read_hdf(input_file, 'MC/particles')
    particle_ids = particles.particle_id.unique()

    assert np.all(np.isin(hit_pids, particle_ids))


def test_hit_labels(input_file):
     """Check that there is at least one hit in the ACTIVE volume """
     hits = pd.read_hdf(input_file, 'MC/hits')
     hit_labels = hits.label.unique()

     assert 'ACTIVE' in hit_labels


def test_primary_always_exists(input_file):
     """Check that there is at least one primary particle """
     particles = pd.read_hdf(input_file, 'MC/particles')
     primary   = particles.primary.unique()

     assert 1 in primary


def test_sensor_binning_is_saved(input_file):
     """Check that the sensor binning is saved in the configuration table. """
     conf = pd.read_hdf(input_file, 'MC/configuration')
     parameters = conf.param_key.values

     assert any('binning' in p for p in parameters)
