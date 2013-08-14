### Xenon follows Ideal Gases Law at low pressure and room temperature
### pV = nRT -> p = nRT/V = mRT/aV = RTd/a -> d = pa/RT

import math
import numpy
from centella.tools.system_of_units import *


R = 8.314472 * m3 * hep_pascal / ( kelvin * mole ) #gas constant
a = 131.293 * g / mole

temperature = (273.15 + 15.) * kelvin; # 15 C

pressure = 1000 * atmosphere
mass = 100 * kg

density = (pressure*a)/(R*temperature)
volume = mass / density

print ""
print "============================="
print "  pressure = %f atm" % (pressure / atmosphere)
print "  mass = %f kg" % (mass / kg)
print "  density = %f kg/m3" % (density / (kg/m3))
print "  volume = %f m3" % (volume / m3)
print "  axis = %f m" % (math.pow(volume, 1./3) / m)
print "============================="

