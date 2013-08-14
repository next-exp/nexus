### ------------------------------------------------------------------
###  SConstruct - Building script for NEXUS
###
###  Author : J. Martin-Albo <jmalbos@ific.uv.es>
###  Created: 23 July 2009
###  Version: $Id$
###
###  N.B. First time this script is executed, a configure-like step
###  tries to find out where are located the header and libraries
###  of all NEXUS dependencies. If it succeeds, this configuration is
###  stored in a file and used in following build (until a clean is
###  performed). The script tries to locate the dependencies by using
###  pkg-config scripts or searching at common paths. This should work
###  in most systems. However, if needed, users can provide via
###  command-line (or the BUILDVARS_FILE) the system path to any
###  dependency.
### ------------------------------------------------------------------

import os
import atexit


## Geant4 version required by NEXUS
NEXUS_G4VERSION_NUMBER = [930, 931]

## File where the build variables are stored
BUILDVARS_FILE = 'buildvars.scons'

## Dummy default for path-like variables
DEFAULT_PATH = '.'

## Geant4 global libraries
G4LIBS = ['G4FR',
          'G4GMocren',
          'G4OpenGL',
          'G4RayTracer',
          'G4Tree',
          'G4VRML',
          'G4digits_hits',
          'G4error_propagation',
          'G4event',
          'G4geometry',
          'G4gl2ps',
          'G4global',
          'G4graphics_reps',
          'G4intercoms',
          'G4interfaces',
          'G4materials',
          'G4modeling',
          'G4parmodels',
          'G4particles',
          'G4persistency',
          'G4physicslists',
          'G4processes',
          'G4readout',
          'G4run',
          'G4track',
          'G4tracking',
          'G4visHepRep',
          'G4visXXX',
          'G4vis_management']

## NEXUS source code directories
SRCDIR = ['actions',
          'base',
          'generators',
          'geometries',
          'physics',
          'physics_lists',
          'utils']


## Some useful functions

def Abort(message):
    """Outputs a message before exiting the script with an error."""
    print 'scons: Build aborted.'
    print 'scons:', message
    Exit(1)

def G4Version(path_to_headers):
    
    for line in open(path_to_headers+'/G4Version.hh'):
        if line.startswith('#define G4VERSION_NUMBER'):
            foo, bar, version = line.split()
            return int(version)



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

    PathVariable('G4INCLUDE',                         # var name
                 'Path to Geant4 headers directory',  # var description
                 DEFAULT_PATH),                       # var default value

    PathVariable('G4LIBDIR',
                 'Path to Geant4 libraries directory.',
                 DEFAULT_PATH),
    
    
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

    PathVariable('BHEP_PATH',                   
                 'Path to BHEP installation.',
                 DEFAULT_PATH),                 

    PathVariable('BHEP_INCDIR',                     
                 'Path to BHEP headers directory.', 
                 DEFAULT_PATH),                     

    PathVariable('BHEP_LIBDIR',
                 'Path to BHEP library directory.',
                 DEFAULT_PATH),
    
    
    ## ROOT

    PathVariable('ROOTSYS',
                 'Path to ROOT installation.',
                 DEFAULT_PATH),
    
    PathVariable('ROOT_INCDIR',
                 'Path to ROOT headers directory.',
                 DEFAULT_PATH),

    PathVariable('ROOT_LIBDIR',
                 'Path to ROOT libraries directory',
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

## If the LIBPATH buildvar is not defined, the configure step has
## not been run yet
if not env['LIBPATH']: 

    ## Create a Configure object that provides autoconf-like functionality
    conf = Configure(env, conf_dir='.sconf', log_file='.sconf/sconf.log')


    ## CLHEP .........................................................

    ## If user provided a CLHEP_PATH, assume that libraries and headers
    ## are in usual subfolders 'lib' and 'include'.
    if env['CLHEP_PATH'] != DEFAULT_PATH:
        env['CLHEP_INCDIR'] = env['CLHEP_PATH'] + "/include"
        env['CLHEP_LIBDIR'] = env['CLHEP_PATH'] + "/lib"

    ## Finding path to headers. Try: 1) user-defined var,
    ## 2) pkg-config script. If nothing succeeded, abort build.

    if env['CLHEP_INCDIR'] != DEFAULT_PATH:
        env.Append(CPPPATH = env['CLHEP_INCDIR'])
    else:
        try:
            env.ParseConfig("clhep-config --include")
        except OSError:
            Abort('No path to CLHEP headers was provided.')
    
    ## Check if headers are available in the path just found.
    if not conf.CheckCXXHeader('CLHEP/ClhepVersion.h'):
        Abort('CLHEP headers not found.')

    
    ## Finding libraries. Try the same we did for finding the headers.
    
    if env['CLHEP_LIBDIR'] != DEFAULT_PATH:
        env.Append(LIBPATH = env['CLHEP_LIBDIR'])
        env.Append(LIBS = "CLHEP") # CLHEP global library
    else:
        try:
            env.ParseConfig("clhep-config --libs")
        except OSError:
            Abort('No path to CLHEP libraries was provided.')

    ## Check if the library is available in the path we just found
    if not conf.CheckLib(library='CLHEP', language='CXX', autoadd=0):
        Abort('CLHEP libraries not found.')
    
    
    ## BHEP ..........................................................

    ## If user provided a BHEP_PATH, assume that libraries and headers
    ## are in usual subfolders 'lib' and 'include':
    if env['BHEP_PATH'] != DEFAULT_PATH:
        env['BHEP_INCDIR'] = env['BHEP_PATH'] + '/include'
        env['BHEP_LIBDIR'] = env['BHEP_PATH'] + '/lib'

    ## Finding path to headers. Try: 1) user-defined var,
    ## 2) pkg-config script. Abort build nothing worked.

    if env['BHEP_INCDIR'] != DEFAULT_PATH:
        env.Append(CPPPATH = env['BHEP_INCDIR'])
    else:
        try:
            env.ParseConfig("bhep-config --include")
        except OSError:
            Abort("Path to BHEP headers not defined!")

    ## Check if headers are available in the path just found
    ## (Caveat: We check for a header that does not include itself other
    ## external includes (namely ROOT) that we don't know yet where
    ## are located.)
    if not conf.CheckCXXHeader('bhep/system_of_units.h'):
        Abort('BHEP headers not found.')


    ## Finding path to library. Try: 1) user-defined var,
    ## 2) pkg-config script. Abort build nothing worked.

    if env['BHEP_LIBDIR'] != DEFAULT_PATH:
        env.Append(LIBPATH = env['BHEP_LIBDIR'])
        env.Append(LIBS = "bhep")
    else:
        try:
            env.ParseConfig("bhep-config --libs")
        except OSError:
            Abort("Path to BHEP library not defined!")

    ## Check if the library is available in the path we just found
    if not conf.CheckLib(library='bhep', language='CXX', autoadd=0):
        Abort('BHEP library not found.')
    
    
    ## Geant4 ........................................................

    ## Path to Geant4 headers. Try the following:
    ## 1) user-defined buildvar (if the buildvar is different from
    ## the dummy default, the user has defined it via command-line or
    ## file), 2) external environment variable.
    ## Abort the build if nothing worked.

    if env['G4INCLUDE'] != DEFAULT_PATH: 
        env.Append(CPPPATH = env['G4INCLUDE'])
    else:
        try:
            env['G4INCLUDE'] = env['ENV']['G4INCLUDE']
            env.Append(CPPPATH = env['G4INCLUDE'])
        except KeyError:
            Abort('No path to Geant4 headers was provided.')
            
    ## Check for a G4 header in the path just found
    if not conf.CheckCXXHeader('G4Version.hh'):
        Abort('Geant4 headers not found.')

    ## Make sure the user has the proper Geant4 version
    USER_G4VERSION_NUMBER = G4Version(env['G4INCLUDE'])
    if not USER_G4VERSION_NUMBER in NEXUS_G4VERSION_NUMBER:
        msg = 'This version of NEXUS requires Geant4 version(s) ' \
              + str(NEXUS_G4VERSION_NUMBER) \
              + ' while you are using version %i.' % USER_G4VERSION_NUMBER
        Abort(msg)


    ## Path to Geant4 libraries. Try:
    ## 1) user-defined buildvar, 2) external environment variable.
    ## Abort the build ifnothing worked.
    
    if env['G4LIBDIR'] != DEFAULT_PATH:
        env.Append(LIBPATH = env['G4LIBDIR'])
    else:
        try:
            env['G4LIBDIR'] = env['ENV']['G4LIB'] +'/'+ env['ENV']['G4SYSTEM']
            env.Append(LIBPATH = env['G4LIBDIR'])
        except KeyError:
            Abort('No path to Geant4 libraries was provided.')

    ## Check for a G4 library in the path just found
    if not conf.CheckLib(library='G4global', language='CXX', autoadd=0):
        Abort('Geant4 global libraries not found.')

    ## Add Geant4 global libraries to the environment
    env.Append(LIBS = G4LIBS)
    
    
    ## ROOT ..........................................................

    ROOTCONF = DEFAULT_PATH # full path to root-config

    if env['ROOTSYS'] != DEFAULT_PATH:
        ROOT_BINDIR = env['ROOTSYS'] + '/bin/'
    elif env['ENV']['ROOTSYS']:
        ROOT_BINDIR = env['ENV']['ROOTSYS'] + '/bin/'
    else:
        ROOT_BINDIR = ''

    try:
        env.ParseConfig(ROOT_BINDIR + 'root-config --cflags')
        env.ParseConfig(ROOT_BINDIR + 'root-config --libs')
    except OSError:
        Abort('ROOT headers and libraries could not be located.')


    ## OpenGL and X11 ................................................

    if env['PLATFORM'] == 'darwin':
        
        env.MergeFlags('-I/usr/X11R6/include')

        env.MergeFlags('-L/usr/X11R6/lib  -lXmu -lXt -lXext -lX11 -lXi -lSM -lICE')

        env.MergeFlags('-L/usr/X11R6/lib -lGLU -lGL')

        #env.MergeFlags('-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib')
        
    else: # assumed posix (typically Linux-g++)

        env.MergeFlags('-I/usr/include/X11')

        libdir = 'lib'
        import platform
        if platform.architecture()[0] == '64bit':
            libdir = 'lib64'

        env.MergeFlags('-L/usr/X11R6/'+libdir+' -lXmu -lXt -lXext -lX11 -lSM -lICE')

        env.MergeFlags('-I/usr/include')
        
        env.MergeFlags('-I/usr/'+libdir+' -lGLU -lGL')


    env = conf.Finish()


# save build variables to file
vars.Save(BUILDVARS_FILE, env)

# generate help text for build vars
Help(vars.GenerateHelpText(env))

G4DEFINE  = ['G4VIS_USE', 'G4VIS_USE_OPENGLX']
env['CPPDEFINES'] = G4DEFINE



## ################################################################### 
## BUILDING NEXUS

SRCDIR = ['source/' + dir for dir in SRCDIR]

env['CPPPATH'] += SRCDIR

src = []
for d in SRCDIR:
    src += Glob(d+'/*.cc')    

env['CXXCOMSTR']  = "Compiling $SOURCE"
env['LINKCOMSTR'] = "Linking $TARGET"

nexus = env.Program('nexus', ['source/nexus.cc']+src)

Clean(nexus, 'buildvars.scons')
