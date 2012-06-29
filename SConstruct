### ------------------------------------------------------------------
###  SConstruct - Building script for NEXUS
###
###  Author : J. Martin-Albo <jmalbos@ific.uv.es>
###  Created: 23 July 2009
###  Version: $Id$
###
###  Note.- First time this script is executed, a configure-like step
###  tries to find out where are located the header and libraries
###  of all NEXUS dependencies. If it succeeds, this configuration is
###  stored in a file and used in future builds until a clean is
###  performed. The script tries to locate the dependencies by using
###  pkg-config scripts or searching at common paths. This should work
###  in most systems. However, if needed, users can provide via
###  command-line (or the BUILDVARS_FILE) the system path to any
###  dependency.
### ------------------------------------------------------------------

import os
import atexit

## Geant4 version required by NEXUS
NEXUS_G4VERSION_NUMBER = [950, 951]

## NEXUS source code directories
SRCDIR = ['actions',
          'base',
          'generators',
          'geometries',
          'materials',
          'physics',
          'physics_lists',
          'sensdet',
          'utils']

## File where the build variables are stored
BUILDVARS_FILE = 'buildvars.scons'

## Dummy default for path-like variables
DEFAULT_PATH = '.'

## Some useful functions

def Abort(message):
    """Outputs a message before exiting with an error."""
    print 'scons: Build aborted.'
    print 'scons: ', message
    Exit(1)

def AssertG4Version(path):
    """Checks whether the user's G4 version can be used to run nexus."""
    command = 'geant4-config --version'
    if path != DEFAULT_PATH:
        command = path + '/' + command
    version = os.popen(command).read()
    version = int(''.join(version.split('.')))
    if not version in NEXUS_G4VERSION_NUMBER:
        msg = 'This version of nexus requires Geant4 version(s) ' \
            + str(NEXUS_G4VERSION_NUMBER) \
            + ', and you are using version %i.' % version
        Abort(msg)


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
                 DEFAULT_PATH),                       # var default value
    
    
    ## CLHEP

    PathVariable('CLHEP_PATH',
                 'Path to CLHEP installation',
                 DEFAULT_PATH),
    
    PathVariable('CLHEP_INCDIR',
                 'Path to CLHEP headers directory.',
                 DEFAULT_PATH),
    
    PathVariable('CLHEP_LIBDIR',
                 'Path to CLHEP libraries directory.',
                 DEFAULT_PATH),
    
    
    ## BHEP 

    PathVariable('BHEP_BINDIR',                   
                 'Path to BHEP installation.',
                 DEFAULT_PATH),                 
    
    ## ROOT

    PathVariable('ROOT_BINDIR',
                 'Path to ROOT installation.',
                 DEFAULT_PATH),

    
    ## OpenGL and X11

    PathVariable('OGLPATH',
                 'Path to OpenGL installation.',
                 '/usr/X11R6'),

    PathVariable('X11PATH',
                 'Path to X11 installation.',
                 '/usr/X11R6'),


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
     []),
    
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
    
    if env['GEANT4_BINDIR'] != DEFAULT_PATH:
        env.PrependENVPath('PATH', env['GEANT4_BINDIR'])

    env.ParseConfig('geant4-config --cflags --libs')

    if not conf.CheckCXXHeader('G4Event.hh'):
        Abort('Geant4 headers could not be found.')

    if not conf.CheckLib(library='G4global', language='CXX', autoadd=0):
        Abort('Geant4 libraries could not be found.')


    ## BHEP configuration ----------------------------------

    if env['BHEP_BINDIR'] != DEFAULT_PATH:
        env.PrependENVPath('PATH', env['BHEP_BINDIR'])

    env.ParseConfig("bhep-config --libs --ldflags --include")

    if not conf.CheckCXXHeader('bhep/system_of_units.h'):
        Abort('BHEP headers not found.')

    if not conf.CheckLib(library='bhep', language='CXX', autoadd=0):
        Abort('BHEP library not found.')


    ## ROOT configuration ----------------------------------

    if env['ROOT_BINDIR'] != DEFAULT_PATH:
        env.PrependENVPath('PATH', env['ROOT_BINDIR'])
        
    env.ParseConfig('root-config --cflags --libs')

    if not conf.CheckCXXHeader('TObject.h'):
        Abort('ROOT headers could not be found.')

    if not conf.CheckLib(library='Cint', language='CXX', autoadd=0):
        Abort('ROOT libraries could not be found.')


    ## -----------------------------------------------------
        
    env = conf.Finish()

vars.Save(BUILDVARS_FILE, env)

## ###################################################################
## BUILDING NEXUS

SRCDIR = ['source/' + dir for dir in SRCDIR]

env.Append(CPPPATH = SRCDIR)

src = []
for d in SRCDIR:
    src += Glob(d+'/*.cc')

env['CXXCOMSTR']  = "Compiling $SOURCE"
env['LINKCOMSTR'] = "Linking $TARGET"

nexus = env.Program('nexus', ['source/nexus.cc']+src)

Clean(nexus, 'buildvars.scons')
