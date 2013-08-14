###
###  NEXUS: NEXT Utility for Simulation
###      nexus_example.geom
###      (Example geometry file)
###


### Time Projection Chamber
### -------------------------


## TPC shape (box, cylinder)
GEOMETRY CHAMBER_shape S box

## TPC dimensions (cm)
GEOMETRY CHAMBER_dimensions V 3
100.
100.
200.

## BUFFER gap (cm)
GEOMETRY BUFFER_gap D 10.0

## Outer VESSEL thickness (cm)
GEOMETRY OVESSEL_thickness D 5.

## Inner VESSEL thickness (cm)
GEOMETRY IVESSEL_thickness D .5


### Xenon properties
### ------------------

## Pressure (atm)
GEOMETRY pressure D 10.


# 
### TPC shape (box, cylinder)
#GEOMETRY CHAMBER_shape S cylinder
#
### TPC dimensions (cm)
#GEOMETRY CHAMBER_dimensions V 2
#200.
#200.
#


