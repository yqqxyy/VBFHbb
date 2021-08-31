#!/usr/bin/python

import os, math, sys

in_file = sys.argv[1]

##############################
# rucio documentation :
# https://twiki.cern.ch/twiki/bin/view/AtlasComputing/RucioClientsHowTo
##############################

# very improtant not to have / in the end of dataset name
type_file = [
#"_tuple.root", # tuple
"_CxAOD.root", # CxAOD
#"_hist-output.root", # histo for normalisation
]

#### output job file list to read
read_file = open('./'+in_file, 'r')

for line in read_file :
 for type in type_file :
  # protect from commented lines
  if line.find("#")>=0 : continue
  # define input/output jobs
  sample = line.strip()
  # define command
  command = "rucio download "+sample+type
  print command
  os.system(command)

print "end of script"
