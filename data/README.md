# Folder Contents

## 0nuBB files
Bi214.genbb: Sample generated Bi214 events  
Tl208.genbb: Sample generated Tl208 events  
Xe136_bb0nu.genbb: Sample generated 0nu double beta events  
Xe136_bb2nu.genbb: Sample generated 2nu double beta events  

## Xenon Gas Files
gxe_density_table.txt:  File containing gXe densities

## Muon Generator Files
MuonAnaAllRuns.csv: Measured muon azimuth and zenith file
                    use with `angle_dist za` option

File format:
value (just a header string),<intensity/flux in bin>,<histogram azimuth bin i centre>,<histogram zenith bin j centre>,<histogram azimuth bin i width>,<histogram zenith bin j width>

At the end of the file are the bin low edges e.g.
zenith (var name), 0         # The first zenith bin low edge
zenith (var name), 0.0392699 # The second zenith bin low edge

-----
SimulatedMuonsProposalMCEq.csv: Simulated muon azimuth and zenith file, 
                                use with `angle_dist za` option

File format is identical to MuonAnaAllRuns.csv

-----
SimulatedMuonsProposalMCEqEnergy.csv: Simulated muon azimuth, zenith and energy file, 
                                use with `angle_dist zae` option

File format:
value,<intensity in bin>,<histogram azimuth bin i centre>,<histogram zenith bin j centre>,<histogram energy bin k centre>,<histogram azimuth bin i width>,<histogram zenith bin j width>,<histogram energy bin k width>

Then at the end of the file we include the bin edges, same as MuonAnaAllRuns.csv. 