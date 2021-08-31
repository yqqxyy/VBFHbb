#!/usr/bin/python

import os, math, sys, string, ROOT
import hashlib
from operator import itemgetter
ROOT.gROOT.SetBatch(True)

# Script for extracting initial number of events and sum of weights
# from CxAODs, created from xAODs or DxAODs.
# The current run directory is searched for the CxAODs.
# Alternatively a sampleListFile can be given.

countNumberInDxAOD=False # default False, set to true to replace events out to in (for checking DxAOD yields)
if countNumberInDxAOD:
  extrafiletag='_DxAOD'
else:
  extrafiletag=''

out_file_md5 = "undefined"

def main(argv):

  sampleListFile = ""
  if len(argv) == 0:
    print "Scanning current directory for root files and trying to extract yields..."
  elif len(argv) == 1:
    sampleListFile = argv[0]
    print "Trying to extract yields from files listed in '" + sampleListFile + "'..."
  else:
    print "Usage: python $ROOTCOREBIN/../FrameworkExe/scripts/count_Nentry_SumOfWeight.py [sampleListFile]"
    return
  
  nrj = [
    "13TeV",
    ]
  
  counts = {}
  
  # prepare counter
  for i_nrj in nrj :
    counts[i_nrj] = []
  
  # check for valid file list file
  if sampleListFile is not "" and not os.path.isfile(sampleListFile):
    print "Error: File '" + sampleListFile + "' does not exist!"
    return
  
  # check for valid output directory
  yieldDir = "./FrameworkSub/data/"
  copyFilesMD5 = False
  if sampleListFile is not "":
    if os.path.isdir(yieldDir):
      copyFilesMD5 = True
    else:
      print "Warning: Directory '" + yieldDir + "' does not exist, can't copy output files!"
  
  # copy file by appending its md5sum (first 10 characters) to the name
  md5sum = ""
  global out_file_md5
  if copyFilesMD5:
    md5sum = hashlib.md5(open(sampleListFile, 'rb').read()).hexdigest()
    if len(md5sum) > 10:
      md5sum = md5sum[:10]
    out_file_md5 = yieldDir + 'yields.txt.' + md5sum
    if os.path.isfile(out_file_md5):
      print "File '" + out_file_md5 + "' exists already, skipping yield counting."
      return
  
  # compile file list
  fileList = []
  if sampleListFile is "":
    for subdir, dirs, files in os.walk(".") :
      for file in files :
        fileList.append(os.path.join(subdir, file))
        #print os.path.join(subdir, file)
  else:
    files = open(sampleListFile)
    for file in files :
      fileList.append(file.rstrip())
      #print file.rstrip()
    
  
  # loop over root files and count
  for my_file in fileList :

    # root file?
    if not ".root" in my_file :
      print "Skipping non-root file", my_file
      continue

    # energy
    curr_nrj = "unknown"
    for i_nrj in nrj :
      if i_nrj in my_file :
        curr_nrj = i_nrj
    if "unknown" in curr_nrj :
      print "Energy not defined, skipping file", my_file
      continue

    # mc_channel_number
    start = string.find(my_file, curr_nrj + ".") + len(curr_nrj) + 1
    end = string.find(my_file, ".", start)
    if start < 0 or end < 0 or end - start < 2 :
      print "mc_channel_number not defined, skipping file", my_file
      continue
    mc_channel_number = my_file[start : end]

    # sample_name
    start = end + 1
    end = string.find(my_file, ".", start)
    sample_name = "unknown"
    if start < 0 or end < 0 or end - start < 2 :
      print "warning: sample name not defined for file", my_file
    sample_name = my_file[start : end]

    # read counts
    f = ROOT.TFile.Open(my_file,"read")
    h = f.Get("MetaData_EventCount")
    if not h:
      print "warning: MetaData_EventCount not found, skipping file", my_file
      continue

    # from DxAOD metadata
    n_entries = h.GetBinContent(1)
    n_selout_entries = h.GetBinContent(3)
    if countNumberInDxAOD:
      n_selout_entries = h.GetBinContent(2)  # replace out with in for testing DxAOD yields
    n_sum = h.GetBinContent(4)
    # from processed xAOD events
    if n_entries == 0 :
      n_entries = h.GetBinContent(7)
      n_sum = h.GetBinContent(8)
    if n_entries == 0 :
      print "Warning 0 entries for file", my_file

    print "Have", curr_nrj, mc_channel_number, n_entries, n_selout_entries, n_sum, sample_name, my_file

    # find counter
    count = counts[curr_nrj]
    counter = ["unknown", 0, 0, 0, "unknown"]
    for i_count in count :
      if mc_channel_number in i_count[0] :
        counter = i_count
    if "unknown" in counter[0] :
      counter = [mc_channel_number, 0, 0, 0, sample_name]
      count.append(counter)



    counter[1] += n_entries
    counter[2] += n_selout_entries 
    counter[3] += n_sum 

    f.Close()

  # write list
  for i_nrj in nrj :
    # pick the good table
    count = counts[i_nrj]
    if len(count) is 0:
      print "Info: No yields found for " + i_nrj + ". Skipping file writing."
      continue
    # sort by mc_channel_number
    count = sorted(count, key=itemgetter(0))
    # define file
    out_file_name = './yields.'+i_nrj+extrafiletag+'_sorted.txt'
    print "Writing", out_file_name
    out_file = open(out_file_name, 'w')
    # write
    for i_count in count :
      #print str(i_nrj)+" -> "+i_count[0]+" = "+str(i_count[1])+" "+str(i_count[2])+" "+str(i_count[3])
      out_file.write(str(i_count[0])+"\t\t"+str(i_count[1])+"\t\t"+str(i_count[2])+"\t\t"+str(i_count[3])+"\t\t"+str(i_count[4])+"\n")
    out_file.close()
  
  # copy yield file by appending the md5sum of input files
  if copyFilesMD5:
    # check which energy
    # TODO should energy be removed from this script completely?
    curr_nrj = ""
    for i_nrj in nrj :
      if len(counts[i_nrj]) > 0:
        if curr_nrj != "":
          print "Error: Found multiple energies in file list, can't copy output file!"
          return
        curr_nrj = i_nrj
    if curr_nrj is "":
      return
    out_file_sort = './yields.'+curr_nrj+'_sorted.txt'
    print "Copying file '" + out_file_sort + "' to '" + out_file_md5 + "'..."
    os.system("cp " + out_file_sort + " " + out_file_md5)
  
    sampleListFile_md5 = yieldDir + sampleListFile + "." + md5sum
    print "Copying file '" + sampleListFile + "' to '" + sampleListFile_md5 + "'..."
    os.system("cp " + sampleListFile + " " + sampleListFile_md5)
  
if __name__ == "__main__":
   main(sys.argv[1:])
