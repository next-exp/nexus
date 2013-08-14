### ------------------------------------------------------------------
###  SConstruct - Building script for NEXUS
###
###  Author : J. Martin-Albo <jmalbos@ific.uv.es>
###  Created: 23 July 2009
###  Version: $Id$
### ------------------------------------------------------------------

import os

### ..................................................................
### First time this script is executed, a configure-like step tries
### to find out where headers and libraries of all dependencies are
### located. If it succeeds, compilation flags are stored in a file
### and used in following builds until a cleaning is performed.
### 
### The script tries to locate the dependencies using pkg-config 
### scripts or searching at common paths (/usr or /usr/local).
### That should work for most systems. However, if needed, users can
### specify the location of any dependency via command-line or the 
### BUILDVARS_FILE.
###


### Some useful functions ............................................

def G4GlobalLibs():
    G4LIBS = ['G4digits_hits', 'G4event', 'G4FR', 
              'G4geometry', 'G4global', 'G4graphics_reps',
              'G4intercoms', 'G4interfaces', 'G4materials',
              'G4modeling', 'G4OpenGL', 'G4parmodels', 
              'G4particles', 'G4persistency', 'G4physicslists',
              'G4processes', 'G4RayTracer', 'G4readout', 
              'G4run', 'G4track', 'G4tracking', 'G4Tree', 
              'G4visHepRep', 'G4vis_management', 'G4visXXX', 
              'G4VRML']
    return G4LIBS


def RootLibs():
    return ['Core', 'Cint', 'RIO', 'Net', 'Hist', 'Graf', 'Graf3d', 
            'Gpad', 'Tree', 'Rint', 'Postscript', 'Matrix', 'Physics', 
            'MathCore', 'Thread', 'freetype']


def Abort(message):
    print "scons: Build aborted."
    print "scons:", message
    Exit(1)


### Build variables ..................................................
### These vars can be used to specify (via command-line or file) the
### location of dependencies. They are also used to store in file
### configured values of compilation flags ($CPPPATH, $LIBS, etc.)
###

# file where the variables are (will be) stored
BUILDVARS_FILE = 'buildvars.scons'

# dummy default for path-variables
DEFAULT_PATH = '.'

# create Variables instance (associated to file) and add it the vars
vars = Variables(BUILDVARS_FILE)

vars.AddVariables(

    PathVariable('BHEP_INCDIR',                     # var name
                 'Path to BHEP headers directory.', # var description
                 DEFAULT_PATH),                     # var default value

    PathVariable('BHEP_LIBDIR',
                 'Path to BHEP library directory.',
                 DEFAULT_PATH),

    PathVariable('CLHEP_INCDIR',
                 'Path to CLHEP headers directory.',
                 DEFAULT_PATH),

    PathVariable('CLHEP_LIBDIR',
                 'Path to CLHEP library directory.',
                 DEFAULT_PATH),

    PathVariable('ROOT_INCDIR',
                 'Path to ROOT headers directory.',
                 DEFAULT_PATH),

    PathVariable('ROOT_LIBDIR',
                 'Path to ROOT installation',
                 DEFAULT_PATH),

    PathVariable('G4INCLUDE',
                 'Path to Geant4 headers directory',
                 DEFAULT_PATH),

    PathVariable('G4LIBDIR',
                 'Path to Geant4 global libraries directory.',
                 DEFAULT_PATH),

    PathVariable('OGLPATH',
                 'Path to OpenGL libraries and headers',
                 '/usr/X11R6'),

    PathVariable('X11PATH',
                 'Path to X11 libraries and headers',
                 '/usr/X11R6'),

    # The following vars should not be defined by users unless they 
    # know what they are doing.

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


### Construction environment .........................................

env = Environment(variables=vars, ENV=os.environ)



### Configure build ..................................................

if not env['LIBPATH']:

    # CLHEP ................................................
    
    if env['CLHEP_INCDIR'] != DEFAULT_PATH:
        env.Append(CPPPATH = env['CLHEP_INCDIR'])
    else:
        try:
            env.ParseConfig("clhep-config --include")
        except OSError:
            Abort("CLHEP couldn't be configured.")

    if env['CLHEP_LIBDIR'] != DEFAULT_PATH:
        env.Append(LIBPATH = env['CLHEP_LIBDIR'])
        env.Append(LIBS = "CLHEP")
    else:
        try:
            env.ParseConfig("clhep-config --libs")
        except OSError:
            Abort("CLHEP couldn't be configured.")


    # BHEP .................................................

    if env['BHEP_INCDIR'] != DEFAULT_PATH:
        env.Append(CPPPATH = env['BHEP_INCDIR'])
    else:
        try:
            env.ParseConfig("bhep-config --include")
        except OSError:
            Abort("BHEP couldn't be configured.")

    if env['BHEP_LIBDIR'] != DEFAULT_PATH:
        env.Append(LIBPATH = env['BHEP_LIBDIR'])
        env.Append(LIBS = "bhep")
    else:
        try:
            env.ParseConfig("bhep-config --libs")
        except OSError:
            Abort("BHEP couldn't be configured.")


    # ROOT .................................................

    if env['ROOT_INCDIR'] != DEFAULT_PATH:
        env.Append(CPPPATH = env['ROOT_INCDIR'])
    else:
        try:
            env.ParseConfig("root-config --cflags")
            #env.MergeFlags('-I/Users/jmalbos/Software/root/include')
        except OSError:
            Abort("ROOT couldn't be configured.")

    if env['ROOT_LIBDIR'] != DEFAULT_PATH:
        env.Append(LIBPATH = env['ROOT_LIBDIR'])
        env.Append(LIBS = RootLibs())
    else:
        try:
            env.ParseConfig("root-config --libs")
        except OSError:
            Abort("ROOT couldn't be configured.")


    # Geant4 ...............................................

    if env['G4INCLUDE'] != DEFAULT_PATH:
        env.Append(CPPPATH = env['G4INCLUDE'])
    else:
        try:
            env.Append(CPPPATH = env['ENV']['G4INCLUDE'])
        except KeyError:
            Abort("G4INCLUDE not set.")

    if env['G4LIBDIR'] != DEFAULT_PATH:
        env.Append(LIBPATH = env['G4LIBDIR'])
    else:
        try:
            G4LIBDIR = env['ENV']['G4LIB'] + "/" + env['ENV']['G4SYSTEM']
            env.Append(LIBPATH = G4LIBDIR)
        except KeyError:
            Abort('G4LIBDIR not set.')
    
    env.Append(LIBS = G4GlobalLibs())


    # OpenGL and X11 .......................................

    if env['PLATFORM'] == 'darwin':
        
        env.MergeFlags('-I/usr/X11R6/include')

        env.MergeFlags('-L/usr/X11R6/lib  -lXmu -lXt -lXext -lX11 -lXi -lSM -lICE')

        env.MergeFlags('-L/usr/X11R6/lib -lGLU -lGL')

        env.MergeFlags('-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib')
        
    else: # assumed posix (typically Linux-g++)

        env.MergeFlags('-I/usr/include/X11')

        libdir = 'lib'
        import platform
        if platform.architecture()[0] == '64bit':
            libdir = 'lib64'

        env.MergeFlags('-L/usr/X11R6/'+libdir+' -lXmu -lXt -lXext -lX11 -lSM -lICE')

        env.MergeFlags('-I/usr/include')
        
        env.MergeFlags('-I/usr/'+libdir+' -lGLU -lGL')
        


    #conf = Configure(env)
    
    #if not conf.CheckLibWithHeader('CLHEP','CLHEP/ClhepVersion.h','C++','',0):
    #    Abort("CLHEP library or headers not found!")

    #if not conf.CheckLibWithHeader('bhep', 'bhep/system_of_units.h', 'C++'):
    #    Abort("BHEP library or headers not found!")
    
    #if not conf.CheckLib('Cint', '', '', 'C++'):
    #    Abort("ROOT")

    #env = conf.Finish()


# save build variables to file
vars.Save(BUILDVARS_FILE, env)

# generate help text for build vars
Help(vars.GenerateHelpText(env))

G4DEFINE  = ['G4VIS_USE', 'G4VIS_USE_OPENGLX']
env['CPPDEFINES'] = G4DEFINE


### NEXUS source code ................................................

dirs = ['actions', 
        'base', 
        'generators', 
        'geometries',
        'physics']

env['CPPPATH'] += dirs           
env.MergeFlags('-lz')

src = []
for d in dirs:
    src += Glob(d+'/*.cc')    

env['CXXCOMSTR']  = "Compiling $SOURCE"
env['LINKCOMSTR'] = "Linking $TARGET"

nexus = env.Program('nexus', ['nexus.cc']+src)

Clean(nexus, 'buildvars.scons')
