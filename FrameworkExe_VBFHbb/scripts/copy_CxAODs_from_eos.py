#!/usr/bin/python

import os, math, sys

#############################
#
# Script for copying grid-job output-files (CxAODs) from EOS.
#
#############################

#############################
# parameters
#############################

# only print commands instead of executing them
test_only = True

# paths
eos_path = "/eos/atlas/atlasgroupdisk/phys-higgs/HSG5/Run2/VH/"
#afs_path = "/afs/cern.ch/work/d/djamin/framework15/CxAOD/"
afs_path = "./CxAOD/"

# determines full output path (full_path)
energy_tag     = "13TeV"
#samples_tag    = "p1784"
derivation_tag = "HIGG5D1"
production_tag = "00-09-01"

#############################
# sample definition
#############################

our_samples = [
#"muonData",  
#"egammaData",
#"metData",   
"WenuB",
"WenuC",
"WenuL",
"WmunuB",
"WmunuC",
"WmunuL",
"WtaunuB",
"WtaunuC",
"WtaunuL",
"ZeeB",
"ZeeC",
"ZeeL",
"ZmumuB",
"ZmumuC",
"ZmumuL",
"ZnunuB",
"ZnunuC",
"ZnunuL",
"ZtautauB",
"ZtautauC",
"ZtautauL",
"singletop_Wt",
"singletop_s",
"singletop_t",
"ttbar",
"ttbar_add",
"ZHvv125",
"ZHvv125J_MINLO",
"WH125",
"WHlv125J_MINLO",
"ZHll125",
"ZHll125J_MINLO",
#"QCD",
#"HVT",
#"VBFHNWA",
#"ggA",
#"ggHNWA",
]

# get eos symbolic link string (to avoid : "eos: command not found") -> store path got from : type -a eos
# should find better way -> not succeed to use popen
# and this path would maybe be updated with next AnalysisBase
eos_cmd = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"

#############################
# code
#############################

#ana_dir = derivation_tag + "_" + energy_tag + "_" + samples_tag + "/"
ana_dir = derivation_tag + "_" + energy_tag + "/"
prod_dir = "CxAOD_" + production_tag + "/"

def makeDir(path):
  command = "mkdir -p " + path
  print command
  if not test_only : os.system(command)


if __name__ == "__main__":

  # loop over samples
  for sample in our_samples :

    print "Sample:", sample

    # define final paths
    full_path_eos = eos_path + ana_dir + prod_dir + sample + "/"
    full_path_afs = afs_path + ana_dir + prod_dir + sample + "/"

    # make sample dirs
    makeDir(full_path_afs)

    # copy from eos
    command = eos_cmd + " cp -r "
    command += full_path_eos + " "+ full_path_afs
    if test_only : print command
    else :         os.system(command)
      
  print "Copied files from eos to", full_path_afs
  print "Done!"

