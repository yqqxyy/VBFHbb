#!/usr/bin/python

import os, math, sys

#### job version
job_version = "HIGG2D4.11-3"

####
#user = "user.XXXX."
user = "group.phys-higgs."

#### input file -> take care not to put / in the end, it will be handled later
file_in = sys.argv[1]
sample_file  = open('./'+file_in, 'r')

#### output file, to take back jobs
out_file = open('./out_sample.txt', 'w')

for i_sample in sample_file :
  # protect from commented lines
  if i_sample.find("#")>=0 : continue
  # protect from blank lines
  if i_sample == "\n" : continue
  #
  count = 0
  sample_out = user
  sample_out_s = ""
  count_s = 0
  is_offset = False
  count_total = 0
  for char in i_sample :
    # check offset
    if count_total==0 :
      if char=="g" or char=="u" : is_offset = True
    # protect from offset
    val_offset = 0
    dot_offset=False
    if is_offset :
      val_offset = 2
      if count+1==val_offset and char==".": dot_offset=True
    # form string from generator name
    if char == "." : count += 1
    if count >= val_offset and count < val_offset+3 and dot_offset==False : sample_out += char
    # form string from s reco name
    if count >= val_offset+5 and dot_offset==False :
     if char=="_" : count_s += 1
     elif count_s==1: sample_out_s += char
    count_total += 1
  if sample_out.find("data") < 0 : sample_out += "."+sample_out_s
  sample_out += "."+job_version+"\n"

  # extra pruning
  sample_out=sample_out.replace("ParticleGenerator","PG")
  sample_out=sample_out.replace("Pythia","Py")
  sample_out=sample_out.replace("Powheg","Pw")
  sample_out=sample_out.replace("MadGraph","MG")
  sample_out=sample_out.replace("EvtGen","EG")
  sample_out=sample_out.replace("Sherpa","Sh")
  sample_out=sample_out.replace("_CT10","")

  out_file.write(sample_out)

  # check length
  full_name = sample_out.strip()
  #full_name += ".CxAOD.root"
  max_str = 115
  if len(full_name) >= max_str : print "WARNING : "+full_name+" output name is "+str(len(full_name))+" chars, it will crash the job (should be <"+str(max_str)+"). Re-visit the name structure."

sample_file.close()
out_file.close()
