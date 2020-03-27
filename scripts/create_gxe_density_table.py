
############################################################

output_file = "gxe_density_table.txt"

pressure_min  =  0.0 # bar
pressure_max  = 30.0 # bar
pressure_step =  0.5 # bar

temperature_min  = 273.0 # Kelvin
temperature_max  = 315.0 # Kelvin
temperature_step =   1.0 # Kelvin

############################################################

import pandas as pd
import numpy  as np

data = []

for temperature in np.arange(temperature_min, temperature_max, temperature_step):

        url = "https://webbook.nist.gov/cgi/fluid.cgi?Action=Data&Wide=on&ID=C7440633&Type=IsoTherm&Digits=5"
        url += "&PLow="  + str(pressure_min)
        url += "&PHigh=" + str(pressure_max)
        url += "&PInc="  + str(pressure_step)
        url += "&T=" + str(temperature)
        url += "&RefState=DEF&TUnit=K&PUnit=bar&DUnit=kg%2Fm3&HUnit=kJ%2Fmol&WUnit=m%2Fs&VisUnit=uPa*s&STUnit=N%2Fm"

        data.append(pd.read_csv(url, header=0, sep="\t")[['Temperature (K)', 'Pressure (bar)', 'Density (kg/m3)']])

table = pd.concat(data, ignore_index=True)
table.to_csv(output_file, index=False, float_format='%.3f')
