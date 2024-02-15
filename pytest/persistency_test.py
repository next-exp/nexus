import pandas as pd
import tables as tb
import numpy as np


def test_hdf5_structure(detectors):
    """Check that the hdf5 table structure is the correct one."""
    def test(filename):

        with tb.open_file(filename) as h5out:

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


    filename, _, _, _, _ = detectors
    if "DEMOPP" in filename:
        for run in ["run5", "run7", "run8", "run9", "run10"]:
            test(filename.format(run=run))
    else:
        test(filename)


def test_particle_ids_of_hits_exist_in_particle_table(detectors):
    """
    Check that the particle IDs of the hits are also contained
    in the particle table.
    """

    def test(filename):
        hits = pd.read_hdf(filename, 'MC/hits')
        hit_pids = hits.particle_id.unique()

        particles = pd.read_hdf(filename, 'MC/particles')
        particle_ids = particles.particle_id.unique()

        assert np.all(np.isin(hit_pids, particle_ids))

    filename, _, _, _, _ = detectors
    if "DEMOPP" in filename:
        for run in ["run5", "run7", "run8", "run9", "run10"]:
            test(filename.format(run=run))
    else:
        test(filename)


def test_hit_labels(detectors):
    """Check that there is at least one hit in the ACTIVE volume."""

    def test(filename):
        hits = pd.read_hdf(filename, 'MC/hits')
        hit_labels = hits.label.unique()

        assert 'ACTIVE' in hit_labels

    filename, _, _, _, _ = detectors
    if "DEMOPP" in filename:
        for run in ["run5", "run7", "run8", "run9", "run10"]:
            test(filename.format(run=run))
    else:
        test(filename)



def test_primary_always_exists(detectors):
    """Check that there is at least one primary particle."""

    def test(filename):
        particles = pd.read_hdf(filename, 'MC/particles')
        primary   = particles.primary.unique()

        assert 1 in primary

    filename, _, _, _, _ = detectors
    if "DEMOPP" in filename:
        for run in ["run5", "run7", "run8", "run9", "run10"]:
            test(filename.format(run=run))
    else:
        test(filename)



def test_sensor_binning_is_saved(detectors):
    """Check that the sensor binning is saved in the configuration table."""

    def test(filename):
        conf = pd.read_hdf(filename, 'MC/configuration')
        parameters = conf.param_key.values

        assert any('binning' in p for p in parameters)

    filename, _, _, _, _ = detectors
    if "DEMOPP" in filename:
        for run in ["run5", "run7", "run8", "run9", "run10"]:
            test(filename.format(run=run))
    else:
        test(filename)



def test_sensor_names_are_the_same_across_tables(detectors):
    """Check that the sensor labels are the same in all tables."""

    def test(filename):
        pos  = pd.read_hdf(filename, 'MC/sns_positions')
        conf = pd.read_hdf(filename, 'MC/configuration')

        pos_labels   = pos.sensor_name.unique()
        parameters   = conf.param_key.values
        sns_bin_conf = parameters[['_binning' in p for p in parameters]]

        for label in pos_labels:
            assert any(p == label + '_binning' for p in sns_bin_conf)

        for p in sns_bin_conf:
            assert p[:-8] in pos_labels

    filename, _, _, _, _ = detectors
    if "DEMOPP" in filename:
        for run in ["run5", "run7", "run8", "run9", "run10"]:
            test(filename.format(run=run))
    else:
        test(filename)


def test_keys_values_are_unique_in_string_map(nexus_output_file_no_strings):
    """Check that IDs and strings are not repeated in map table."""

    str_map = pd.read_hdf(nexus_output_file_no_strings, 'MC/string_map')

    assert str_map.name_id.nunique() == len(str_map)
    assert str_map.name.nunique()    == len(str_map)

def test_string_map_ids_are_in_hits_particles_tables(nexus_output_file_no_strings):
    """Check that all IDs of hit and particle tables appear in string map """

    hits      = pd.read_hdf(nexus_output_file_no_strings, 'MC/hits')
    particles = pd.read_hdf(nexus_output_file_no_strings, 'MC/particles')
    str_map   = pd.read_hdf(nexus_output_file_no_strings, 'MC/string_map')

    map_ids = str_map.name_id.values

    assert np.all(np.isin(hits.label.values, map_ids))
    assert np.all(np.isin(particles.particle_name.values, map_ids))
    assert np.all(np.isin(particles.initial_volume.values, map_ids))
    assert np.all(np.isin(particles.final_volume.values, map_ids))
    assert np.all(np.isin(particles.creator_proc.values, map_ids))
    assert np.all(np.isin(particles.final_proc.values, map_ids))
