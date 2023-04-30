#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Dec 29 21:12:29 2022

@author: amir
"""
import pandas as pd
import numpy as np
from matplotlib import pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
plt.style.use('classic')
from tqdm import tqdm
pd.set_option('display.max_columns', None)
pd.set_option('display.max_rows', 500)
# In[0]
# load square fiber

path = '/media/amir/9C33-6BBD/NEXT_work/Geant4/nexus/SquareFiberHitpoints.next.h5'
# path = '/media/amir/9C33-6BBD/NEXT_work/Geant4/nexus/CapillariesHitpoints_reflectivity_0.98.next.h5'
df = pd.read_hdf(path,'/DEBUG/steps')
hits = df.loc[df['final_volume'].str.contains("SiPM_|Cap1")]
#make sure each row is really a new particle
if hits.event_id.duplicated().sum() == 0:
    print("No duplicates in data frame!", end="\n")


# In[1]
# load capillaries
# path = '/media/amir/9C33-6BBD/NEXT_work/Geant4/nexus/CapillariesHitpoints_reflectivity_0.9.next.h5'
# path = '/media/amir/9C33-6BBD/NEXT_work/Geant4/nexus/CapillariesHitpoints.next.h5'
# df = pd.read_hdf(path,'/DEBUG/steps')

# In[2]
# checks if photons left the Square fibers
passed = 0
SiPM_hits = 0
row = 0
n_particles = df['event_id'].max()
capillary_end_z = 248.5 # IMPORTANT VALUES IN HERE!!
x_final = []
y_final = []
z_final = []
# for particle in tqdm(range(n_particles)):
for row in tqdm(range(len(df)-1)):
    if df.iloc[row,0] != df.iloc[row+1,0] and df.iloc[row,-1] >= capillary_end_z:
        passed += 1
        x_final.append(df.iloc[row,-3])
        y_final.append(df.iloc[row,-2])
        z_final.append(df.iloc[row,-1])
        cell = df.iloc[row,5]
        if df.iloc[row,0] != df.iloc[row+1,0] and cell.startswith("SiPM_"):
            SiPM_hits += 1
    row += 1
    
print(f'percent passed = {(passed/(n_particles+1)) * 100}')
print('\n' + f'percent SiPM hits = {(SiPM_hits/(n_particles+1)) * 100}')

# In[2.5]
# Square fibers
x_sipm = []
y_sipm = []
count = 0
sipm_size = 1.5
n_sipms = 1
for i in range(len(x_final)):
    if x_final[i] <= n_sipms*sipm_size and x_final[i] >= -n_sipms*sipm_size and \
        y_final[i] <= n_sipms*sipm_size and y_final[i] >= -n_sipms*sipm_size:
            
        x_sipm.append(x_final[i])
        y_sipm.append(y_final[i])
        count += 1
        
        
fig, (ax0,ax1) = plt.subplots(1,2, figsize=(12,6), dpi=600)
im0 = ax0.hist2d(x_sipm, y_sipm, bins=int(np.sqrt(len(x_sipm))))[3]
ax0.set_xlabel("x [mm]")
ax0.set_ylabel("y [mm]")
ax0.set_title("Center SiPM hit map")

# Call hist2d and extract the histogram object from the returned tuple
im1 = ax1.hist2d(x_final, y_final, bins=int(np.sqrt(len(x_final))))[3]
ax1.set_xlabel("x [mm]")
ax1.set_ylabel("y [mm]")
ax1.set_title("Center SiPM hit map")

# Add a colorbar to each subplot using the extracted histogram object
fig.colorbar(im0, ax=ax0)
fig.colorbar(im1, ax=ax1)

plt.tight_layout()
plt.show()
# In[3]
# show pass vs reflectivity, capillaries
reflectivity = [70, 80, 90, 95, 98]
pass_percent = [0.098, 0.148, 0.265, 0.374, 0.468]
plt.plot(reflectivity, pass_percent, 'k-o')
plt.grid()
plt.xlabel("Teflon reflectivity [%]")
plt.ylabel("Photons exiting the capillary [%]")
plt.show()

# In[4]
# plot SiPM hits vs reflectivity, capillaries
reflectivity = [70, 80, 90, 95, 98]
setup1_sipm_hit_percentage = [0.029, 0.03, 0.022, 0.015, 0.009] #1.3mm, 15.6mm spacing
setup2_sipm_hit_percentage = [0.029, 0.03, 0.021, 0.018, 0.02] #1.3mm, 10mm spacing
setup3_sipm_hit_percentage = [0.069, 0.083, 0.083, 0.063, 0.047] #3mm, 15.6mm spacing
setup4_sipm_hit_percentage = [0.069, 0.083, 0.083, 0.083, 0.097] #3mm, 10mm spacing

plt.plot(reflectivity, setup1_sipm_hit_percentage, 'r-o', label="1.3mm|15.6mm")
plt.plot(reflectivity, setup2_sipm_hit_percentage, 'b-*', label="1.3mm|10mm")
plt.plot(reflectivity, setup3_sipm_hit_percentage, 'g-^', label="3mm|15.6mm")
plt.plot(reflectivity, setup4_sipm_hit_percentage, 'm-s', label="3mm|10mm")
plt.legend(loc='lower left', prop={'size': 10}, title="size|spacing:")
plt.xticks(np.arange(70, 100, 5))
plt.yticks(np.arange(0, 0.1, 0.01))
plt.grid()
plt.xlabel("Teflon reflectivity [%]")
plt.ylabel("Photons hitting the SiPMs [%]")


# In[1]
# show SiPM heat map

# head = df.iloc[0:500]

SiPM_x_hits = hits["final_x"]
SiPM_y_hits = hits["final_y"]
SiPM_x_hits = np.array(SiPM_x_hits)
SiPM_y_hits = np.array(SiPM_y_hits)

x_mask = np.ma.masked_array(SiPM_x_hits,mask=SiPM_x_hits > 10)
y_mask = np.ma.masked_array(SiPM_y_hits,mask=SiPM_y_hits > 10)


bins = int(np.sqrt(len(SiPM_x_hits)))
plt.Figure(dpi=500)
plt.hist2d(SiPM_x_hits,SiPM_y_hits,bins=25)
plt.xlabel("x [mm]")
plt.ylabel("y [mm]")
plt.title("2D histogram of SiPM hits")
# divider = make_axes_locatable(im)
# cax = divider.append_axes("right", size="5%", pad=0.05)
plt.colorbar()
plt.show()


# In[3]
#check max theoretical photons in acceptance cone for tube fiber

n_fiber = 1.49
#n_fiber = 1.46
n_xe = 1
fiber_R = 1.5 #mm
critical_theta = np.arcsin(n_xe/n_fiber)
angle_with_z_axis = np.pi/2 - critical_theta
d = fiber_R/np.tan(angle_with_z_axis)
n = 100000
in_acceptance_cone = 0
x_hits = []
y_hits = []
#generate rays forward (2*pi)
for i in range(n):
    cos_theta = np.random.uniform(-1,1)
    theta = np.arccos(cos_theta)
    phi = np.random.uniform(0,2*np.pi)
    r = d*np.tan(theta)
    xhit = r*np.cos(phi)
    yhit = r*np.sin(phi)
    if r > 0:
        if xhit**2 + yhit**2 <= fiber_R**2:
            in_acceptance_cone += 1
            x_hits.append(xhit)
            y_hits.append(yhit)
plt.hist2d(x_hits, y_hits, bins=int(np.sqrt(n)))
plt.colorbar()
plt.show()            
# In[4]       
#check max theoretical photons in acceptance cone for tube fiber     
  
# if xhit>=-fiber_side and xhit<=fiber_side and yhit>=-fiber_side and yhit<=fiber_side : #inside acceptance cone
#     in_acceptance_cone += 1
#     x_hits.append(xhit)
#     y_hits.append(yhit)
        


        



