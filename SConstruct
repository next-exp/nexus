### ----------------------------------------------------------------------------
###  SConstruct - Building script for NEXUS
###
###  Note.- The first time this script is executed, a configure-like step
###  tries to find out where the header and libraries
###  of all NEXUS dependencies are located. If it succeeds,
###  this configuration is stored in a file and used in future builds
###  until a clean is performed. The script tries to locate the dependencies
###  by using pkg-config scripts or searching at common paths.
###  This should work in most systems. However, if needed,
###  users can provide via command-line (or the BUILDVARS_FILE)
###  the system path to any dependency.
###
### The NEXT Collaboration
### ----------------------------------------------------------------------------

from __future__ import print_function
import os
import subprocess

## Geant4 version required by NEXUS
MIN_NEXUS_G4VERSION_NUMBER = 1050

## NEXUS source code directories
SRCDIR = ['actions',
          'base',
          'generators',
          'geometries',
          'materials',
          'persistency',
          'physics',
          'physics_lists',
          'sensdet',
          'utils']

## File where the build variables are stored
BUILDVARS_FILE = 'buildvars.scons'

## Dummy default for path-like variables
NULL_PATH = '/dev/null'

## Dummy default for path-like variables
DEFAULT_PATH = '.'

## Some useful functions

def Abort(message):
    """Outputs a message before exiting with an error."""
    print ('scons: Build aborted.')
    print ('scons: {}'.format(message))
    Exit(1)

def AssertG4Version(path):

    ## If a path was specified, add it to the cmd to be executed
    cmd = "geant4-config --version"
    if path != NULL_PATH:
        cmd = path + '/' + cmd

    ## Execute the command and store the stdout and stderr
    p = subprocess.Popen(cmd, shell=True,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE)

    ## If the stdout is empty, the execution of the command failed
    version = p.stdout.read()
    if version == '':
        Abort("Failed to establish Geant4 version.")
    else:
        g4version = version.decode('utf-8').strip()
        g4version = int(''.join(g4version.split('.')))
        msg = "Checking for Geant4 version..."
        print (msg, g4version)
        if g4version < MIN_NEXUS_G4VERSION_NUMBER:
            msg = 'This version of NEXUS requires Geant4 version >= ' + \
                  str (MIN_NEXUS_G4VERSION_NUMBER)
            Abort(msg)


## write nexus-config file
def WriteNexusConfig(dir_prefix):
    if not os.path.exists(dir_prefix+'/bin'):
       os.makedirs(dir_prefix+'/bin')
    file_name = dir_prefix+'/bin/nexus-config'
    file = open(file_name, 'w')
    s = '#! /bin/sh' + '\n' + '#nexus-config'
    s = s + '\n' + '\n'
    s = s + 'prefix=' + dir_prefix + '\n'
    s = s + 'exec_prefix=${prefix}/bin' + '\n'
    s = s + 'includedir=${prefix}/include'
    s = s + '\n' + '\n'
    s = s + 'usage()' + '\n' + '{' + '\n' + '    cat  <<EOF' + '\n'
    s = s + 'Usage: nexus-config [OPTION]' + '\n' + '\n'
    s = s + 'Known values for OPTION are:' + '\n'
    s = s + '--prefix show installation prefix' + '\n'
    s = s + '--include prints include path' + '\n'
    s = s + '--libdir prints the path to the library' + '\n'
    s = s + '--libs prints name of libraries to link against' + '\n'
    s = s + '--help displays this help and exit' + '\n'
    s = s + '--version print version information' + '\n' + '\n'
    s = s + 'EOF' + '\n' + '\n'
    s = s + '    exit $1' + '\n' + '}' + '\n' + '\n'
    s = s + 'if test $# -eq 0; then' + '\n' + '    usage 1' + '\n' + 'fi' + '\n'
    s = s + 'while test $# -gt 0; do' + '\n' + '    case "$1" in' + '\n' + '    -*=*)' + '\n'
    s = s + """        optarg=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'`""" + '\n'
    s = s +  '        ;;' + '\n' + '    *)'
    s = s + '\n' + '        optarg=' + '\n' + '        ;;' + '    esac' + '\n' + '\n'
    s = s + '    case "$1" in' + '\n'
    s = s + '    --prefix)' + '\n' + '        echo ' + dir_prefix + '\n'
    s = s + '        ;;' + '\n' + '\n'
    #s = s + '    --version)' + '\n' + '        echo ' + 'v0r1p4 ' + '\n'
    #s = s + '        exit 0' + '\n' + '        ;;' + '\n' + '\n'
    s = s +  '    --help)' + '\n' + '        usage 0' + '\n'
    s = s + '        ;;' + '\n' + '\n'
    s = s +  '    --include)' + '\n' + '        echo -I'+ dir_prefix + '/include' + '\n'
    s = s + '        ;;' + '\n' + '\n'
    s = s +  '    --libdir)' + '\n' + '        echo -L'+ dir_prefix + '/lib' + '\n'
    s = s + '        ;;' + '\n' + '\n'
    s = s +  '    --libs)' + '\n' + '        echo -L'+ dir_prefix + '/lib' + ' -lnexus' + '\n'
    s = s + '        ;;' + '\n' + '\n'
    s = s + '    *)' + '\n' + '        usage' +'\n' + '        exit 1' + '        ;;'
    s = s + '    esac' + '\n' + '    shift' + '\n' + 'done' + '\n' + '\n' + 'exit 0'
    file.write(s)



## ###################################################################
## BUILD VARIABLES
## These variables can be used to specify (via command-line or file)
## the location of dependencies. They are also used to store in file
## configured values of compilation flags ($CPPPATH, $LIBS, etc.).

## Create a Variables instance associated to  a file
vars = Variables(BUILDVARS_FILE)

## Definition of the variables
vars.AddVariables(

    ## Geant4

    PathVariable('GEANT4_BINDIR',                     # var name
                 'Path to Geant4 headers directory',  # var description
                 NULL_PATH),                       # var default value

    ## ROOT

    PathVariable('ROOT_BINDIR',
                 'Path to ROOT installation.',
                 NULL_PATH),

    ## HDF5

    PathVariable('HDF5_DIR',
                 'Path to HDF5 installation.',
                 NULL_PATH),

    ## gsl

    PathVariable('GSL_DIR',
                 'Path to gsl installation.',
                 NULL_PATH),

    ## installation directory
    PathVariable('PREFIX',
                 'Path to installation directory',
                 DEFAULT_PATH),


    ## The following vars shouldn't be defined by users unless they
    ## know what they are doing.

    ('CPPDEFINES',
     'Preprocessor definitions.',
     []),

    ('CCFLAGS',
     'General options passed to the compiler.',
     []),

    ('CPPFLAGS',
     'User-specified preprocessor options.',
     ['-g']),

    ('CXXFLAGS',
     'c++ compiler options.',
     ['-std=c++11']),

    ('CPPPATH',
     'List of directories where the include headers are located.',
     []),

    ('LIBPATH',
     'List of directories where the linked libraries are located.',
     []),

    ('LIBS',
     'List of libraries to link against.',
     []),

    ('LINKFLAGS',
     'User options passed to the linker.',
     [])

    )


## ###################################################################
## CONFIGURE BUILD

## Create a construction environment adding the build vars and
## propagating the user's external environment
env = Environment(variables=vars, ENV=os.environ)

## If the LIBPATH buildvar (for instance) is not defined, the configure
## step has not been run yet
if not env['LIBPATH']:

    ## Create a Configure object that provides autoconf-like functionality
    conf = Configure(env, conf_dir='.sconf', log_file='.sconf/sconf.log')


    ## Geant4 configuration --------------------------------

    AssertG4Version(env['GEANT4_BINDIR'])

    if env['GEANT4_BINDIR'] != NULL_PATH:
        env.PrependENVPath('PATH', env['GEANT4_BINDIR'])

    env.ParseConfig('geant4-config --cflags --libs')


    ## ROOT configuration ----------------------------------

    if env['ROOT_BINDIR'] != NULL_PATH:
        env.PrependENVPath('PATH', env['ROOT_BINDIR'])

    env.ParseConfig('root-config --cflags --libs')

    ## Check for libraries and headers ---------------------

    if not conf.CheckCXXHeader('G4Event.hh'):
        Abort('Geant4 headers could not be found.')

    if not conf.CheckLib(library='G4global', language='CXX', autoadd=0):
        Abort('Geant4 libraries could not be found.')

    if not conf.CheckCXXHeader('TObject.h'):
        Abort('ROOT headers could not be found.')

    if not conf.CheckLib(library='Core', language='CXX', autoadd=0):
        Abort('ROOT libraries could not be found.')


    ## HDF5 configuration ----------------------------------

    if env['HDF5_DIR'] != NULL_PATH:
        env.PrependENVPath('PATH', env['HDF5_DIR'])
    try:
        env['HDF5_LIB'] = os.environ['HDF5_LIB']
        env.Append( LIBPATH = [env['HDF5_LIB']] )
        env.Append(LIBS = ['hdf5'])
        env['HDF5_INC'] = os.environ['HDF5_INC']
        env.Append( CPPPATH = [env['HDF5_INC']] )
    except KeyError: pass


    ## GSL configuration --------------------------   -------

    if env['GSL_DIR'] != NULL_PATH:
        env.PrependENVPath('PATH', env['GSL_DIR'])

    env.ParseConfig('gsl-config --cflags --libs')

    if not conf.CheckCXXHeader('gsl/gsl_errno.h'):
        Abort('GSL headers not found.')

 #   env.Append(LIBS = ['gsl','gslcblas'])


#    if not conf.CheckLib(library='GSL', language='CXX', autoadd=0):
#        Abort('GSL library not found.')
## ##################################################################
    env = conf.Finish()

vars.Save(BUILDVARS_FILE, env)


## ###################################################################
## BUILDING NEXUS

SRCDIR  = ['source/' + dir for dir in SRCDIR]

env.Append(CPPPATH = SRCDIR)

src = []
for d in SRCDIR:
    src += Glob(d+'/*.cc')

headers = []
for d in SRCDIR:
    headers += Glob(d+'/*.h')


env['CXXCOMSTR']  = "Compiling $SOURCE"
env['LINKCOMSTR'] = "Linking $TARGET"

LIBDIR = env['PREFIX'] + '/lib/nexus'
libnexus = env.SharedLibrary(LIBDIR, src)

INCDIR = env['PREFIX'] + '/include/nexus'
env.Install(INCDIR, headers)
env.Alias('install', '.')

w_prefix_dir = env['PREFIX']
w_prefix_dir = os.path.abspath(w_prefix_dir)

## If the installation directory is the current one, find its absolute path
if env['PREFIX'] == DEFAULT_PATH:
   w_prefix_dir = os.getcwd()

WriteNexusConfig(w_prefix_dir)

env.Execute(Chmod(w_prefix_dir+'/bin/nexus-config', 0o755))
nexus = env.Program('bin/nexus', ['source/nexus.cc']+src)

TSTDIR = ['materials',
          'utils',
          'example']
TSTDIR = ['source/tests/' + dir for dir in TSTDIR]

tst = []
for d in TSTDIR:
    tst += Glob(d+'/*.cc')

env.Append(CPPPATH = ['source/tests'])
nexus_test = env.Program('bin/nexus-test', ['source/nexus-test.cc']+tst+src)

Clean(nexus, 'buildvars.scons')
