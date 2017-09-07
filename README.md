# readAIS-Multi
Reader for AIS-Multi device and minor implementation of AIS protocol decoder
Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

This is a simple AIS decoder that displays a list of moving AIS targets with the following information:

- Target type
- MMSI nr
- Speed over ground
- Course over ground
- Latitude and longtitude in decmimal degrees
- Distance of targets relative to receiver
- Country of AIS target
- Target length
- Vessel Name

Todo:

- less gps pos requests
- calc dist at printTarget..
- cpa
- extern keyword?
