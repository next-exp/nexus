import pandas as pd
import tables as tb
import numpy as np


def test_hdf5_structure(nexus_files):
     """Check that the hdf5 table structure is the correct one."""

     filename, _, _, _, _ = nexus_files

     with tb.open_file(filename) as h5out:

         assert 'MC'                in h5out.root
         assert 'particles'         in h5out.root.MC
         assert 'hits'              in h5out.root.MC
         assert 'sns_response'      in h5out.root.MC
         assert 'tof_sns_response'  in h5out.root.MC
         assert 'configuration'     in h5out.root.MC
         assert 'sns_positions'     in h5out.root.MC


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


         scolumns = h5out.root.MC.sns_response.colnames

         assert 'event_id'  in scolumns
         assert 'sensor_id' in scolumns
         assert 'time_bin'  in scolumns
         assert 'charge'    in scolumns


         tscolumns = h5out.root.MC.tof_sns_response.colnames

         assert 'event_id'  in tscolumns
         assert 'sensor_id' in tscolumns
         assert 'time_bin'  in tscolumns
         assert 'charge'    in tscolumns


         sposcolumns = h5out.root.MC.sns_positions.colnames

         assert 'sensor_id'   in sposcolumns
         assert 'sensor_name' in sposcolumns
         assert 'x'           in sposcolumns
         assert 'y'           in sposcolumns
         assert 'z'           in sposcolumns



def test_particle_ids_of_hits_exist_in_particle_table(nexus_files):
    """
    Check that the particle IDs of the hits are also contained
    in the particle table.
    """
    filename, _, _, _, _ = nexus_files

    hits = pd.read_hdf(filename, 'MC/hits')
    hit_pids = hits.particle_id.unique()

    particles = pd.read_hdf(filename, 'MC/particles')
    particle_ids = particles.particle_id.unique()

    assert np.all(np.isin(hit_pids, particle_ids))


def test_hit_labels(nexus_files):
     """Check that there is at least one hit in the ACTIVE volume."""
     filename, _, _, _, _ = nexus_files

     hits = pd.read_hdf(filename, 'MC/hits')
     hit_labels = hits.label.unique()

     assert 'ACTIVE' in hit_labels


def test_primary_always_exists(nexus_files):
     """Check that there is at least one primary particle."""
     filename, _, _, _, _ = nexus_files
     particles = pd.read_hdf(filename, 'MC/particles')
     primary   = particles.primary.unique()

     assert 1 in primary


def test_sensor_binning_is_saved(nexus_files):
     """Check that the sensor binning is saved in the configuration table."""
     filename, _, _, _, _ = nexus_files
     conf = pd.read_hdf(filename, 'MC/configuration')
     parameters = conf.param_key.values

     assert any('bin_size' in p for p in parameters)
     assert any('tof_bin_size' in p for p in parameters)
