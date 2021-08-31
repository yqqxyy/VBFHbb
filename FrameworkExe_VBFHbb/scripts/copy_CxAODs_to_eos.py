#!/usr/bin/python

import os, math, sys

#############################
#
# Script for copying grid-job output-files (CxAODs) to EOS.
#
# Sub-directories are created on EOS to group the files into samples.
#
# VERY IMPORTANT :
# Execute in the directory where you have dq2-get your jobs.
#
#############################

#############################
# parameters
#############################

# only print commands instead of executing them
test_only = False

# verbose mode (otherwise print warnings only)
verbose = True

# skip existing files (also if being corrupt, e.g. size 0)
skip_existing = True

# to copy the samples to an eos directory
#main_path = "/eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/"
#main_path = "/eos/atlas/unpledged/group-tokyo/users/yenari/20160205/NominalOnly/"
main_path = "/eos/atlas/unpledged/group-tokyo/users/yenari/20160210/"
# to copy the samples to a local directory
#main_path = "/disk/lustre/atlas/pdt/DAOD/"

# determines full output path (full_path)
energy_tag     = "13TeV"
derivation_tag = "HIGG2D4"
production_tag = "00-20-00_mc15b"

#############################
# sample definition
#############################

our_samples = [
# sample		1st match		 2nd match
["data",		"physics_Main",	         ""],
["dijet",		"jetjet_JZ",		 ""],
["ZeeB",		"Sh_Zee",       	 "BFilter"],
["ZeeC",		"Sh_Zee",	         "CFilterBVeto"],
["ZeeL",		"Sh_Zee",	         "CVetoBVeto"],
["ZmumuB",		"Sh_Zmumu",	         "BFilter"],
["ZmumuC",		"Sh_Zmumu",	         "CFilterBVeto"],
["ZmumuL",		"Sh_Zmumu",	         "CVetoBVeto"],
["ZtautauB",		"Sh_Ztautau",	         "BFilter"],
["ZtautauC",		"Sh_Ztautau",	         "CFilterBVeto"],
["ZtautauL",		"Sh_Ztautau",	         "CVetoBVeto"],
["ZnunuB",		"Sh_Znunu",	         "BFilter"],
["ZnunuC",		"Sh_Znunu",	         "CFilterBVeto"],
["ZnunuL",		"Sh_Znunu",	         "CVetoBVeto"],
["WenuB",		"Sh_Wenu",	         "BFilter"],
["WenuC",		"Sh_Wenu",	         "CFilterBVeto"],
["WenuL",		"Sh_Wenu",	         "CVetoBVeto"],
["WmunuB",		"Sh_Wmunu",              "BFilter"],
["WmunuC",		"Sh_Wmunu",              "CFilterBVeto"],
["WmunuL",		"Sh_Wmunu",              "CVetoBVeto"],
["WtaunuB",		"Sh_Wtaunu",             "BFilter"],
["WtaunuC",		"Sh_Wtaunu",             "CFilterBVeto"],
["WtaunuL",		"Sh_Wtaunu",             "CVetoBVeto"],
# Sherpa v2.2
["ZeeB_v22",		"Sh_NNPDF30NNLO_Zee",    "BFilter"],
["ZeeC_v22",		"Sh_NNPDF30NNLO_Zee",	 "CFilterBVeto"],
["ZeeL_v22",		"Sh_NNPDF30NNLO_Zee",	 "CVetoBVeto"],
["ZmumuB_v22",		"Sh_NNPDF30NNLO_Zmumu",	 "BFilter"],
["ZmumuC_v22",		"Sh_NNPDF30NNLO_Zmumu",	 "CFilterBVeto"],
["ZmumuL_v22",		"Sh_NNPDF30NNLO_Zmumu",	 "CVetoBVeto"],
["ZtautauB_v22",	"Sh_NNPDF30NNLO_Ztautau","BFilter"],
["ZtautauC_v22",	"Sh_NNPDF30NNLO_Ztautau","CFilterBVeto"],
["ZtautauL_v22",	"Sh_NNPDF30NNLO_Ztautau","CVetoBVeto"],
["ZnunuB_v22",		"Sh_NNPDF30NNLO_Znunu",	 "BFilter"],
["ZnunuC_v22",		"Sh_NNPDF30NNLO_Znunu",	 "CFilterBVeto"],
["ZnunuL_v22",		"Sh_NNPDF30NNLO_Znunu",	 "CVetoBVeto"],
["WenuB_v22",		"Sh_NNPDF30NNLO_Wenu",   "BFilter"],
["WenuC_v22",		"Sh_NNPDF30NNLO_Wenu",	 "CFilterBVeto"],
["WenuL_v22",		"Sh_NNPDF30NNLO_Wenu",	 "CVetoBVeto"],
["WmunuB_v22",		"Sh_NNPDF30NNLO_Wmunu",	 "BFilter"],
["WmunuC_v22",		"Sh_NNPDF30NNLO_Wmunu",	 "CFilterBVeto"],
["WmunuL_v22",		"Sh_NNPDF30NNLO_Wmunu",	 "CVetoBVeto"],
["WtaunuB_v22",	        "Sh_NNPDF30NNLO_Wtaunu", "BFilter"],
["WtaunuC_v22",	        "Sh_NNPDF30NNLO_Wtaunu", "CFilterBVeto"],
["WtaunuL_v22",	        "Sh_NNPDF30NNLO_Wtaunu", "CVetoBVeto"],
#
["ttbar",		"PwPyEG_P2012_ttbar",	 "allhad."],
["ttbar_PwPy8EG",       "PwPy8EG_A14_ttbar",     ""],
["ttbar_Syst",		"ttbar",		 "PwPyEG_P2012rad"],
["ttbar_Syst",		"aMcAtNloHerwigppEG_ttbar", ""],
["ttbar_Syst",          "PwHerwigppEG_UEEE5_ttbar", ""],
["singletop_t",		"P2012_singletop_tchan", ""],
["singletop_t_Syst",    "_singletop_tchan",      "rad"],
["singletop_s",		"Schan",		 ""],
["singletop_s_Syst",	"Schan",		 "rad"],
["singletop_Wt",	"P2012_Wt_inclusive",    ""],
["singletop_Wt_dilep",	"P2012_Wt_dilepton",     ""],
["singletop_Wt_Syst",   "P2012_Wt_DS_",          ""],
["ZHvv125",		"ZH125_nunubb",		 ""], ##
["ZHvv125",             "ZvvH125_bb",            ""],
["ZHvv125",             "ggZH125",               "vevebb"],
["ZHvv125",             "ggZH125",               "vmuvmubb"],
["ZHvv125",             "ggZH125",               "vtauvtaubb"],
["ZHll125",             "ZH125_llbb",            ""], ##
["ZHll125",             "ggZH125",               "eebb"],
["ZHll125",             "ggZH125",               "mumubb"],
["ZHll125",             "ggZH125",               "tautaubb"],
["ZHll125",             "ZllH125_bb",            ""],
["WH125",		"WH125_lnubb",		 ""], ##
["WH125",               "WlvH125_bb",            ""],
["ZHvv125J_MINLO",      "ZH125J_MINLO_v",        ""], ##
["ZHvv125J_MINLO",	"ggZH125_vevebb",  	 "NLO"],
["ZHvv125J_MINLO",      "ggZH125_vmuvmubb",      "NLO"], 
["ZHvv125J_MINLO",      "ggZH125_vtauvtaubb",    "NLO"],   
["WHlv125J_MINLO",	"WpH125J_MINLO",	 ""],
["WHlv125J_MINLO",	"WmH125J_MINLO",	 ""],
["ZHll125J_MINLO",	"ZH125J_MINLO_ee",	 ""],
["ZHll125J_MINLO",	"ZH125J_MINLO_mumu",	 ""],
["ZHll125J_MINLO",	"ZH125J_MINLO_tautau",	 ""],
["ZHll125J_MINLO",      "ggZH125_eebb",          "NLO"],
["ZHll125J_MINLO",      "ggZH125_mumubb",        "NLO"],
["ZHll125J_MINLO",      "ggZH125_tautaubb",      "NLO"],
["WW",			"WplvWmqq",		 ""],
["WW",			"WmlvWpqq",		 ""],
["WW",                  "WpqqWmlv",              ""],
["WW",                  "WmqqWplv",              ""],
["WW_Pw",               "WWlvqq",                "PwPy8EG"],
["WW_Pw",               "WWlvlv",                "PwPy8EG"],
["WZ",                  "WlvZqq",                ""],
["WZ",                  "WqqZll",                ""],
["WZ",                  "WqqZvv",                ""],
["WZ_Pw",               "WZqqll",                "PwPy8EG"],
["WZ_Pw",               "WZqqvv",                "PwPy8EG"],
["WZ_Pw",               "WZlvqq",                "PwPy8EG"],
["WZ_Pw",               "WZlvll",                "PwPy8EG"],
["WZ_Pw",               "WZlvvv",                "PwPy8EG"],
["ZZ",                  "ZqqZll",                ""],
["ZZ",                  "ZqqZvv",                ""],
["ZZ_Pw",               "ZZqqll",                "PwPy8EG"],
["ZZ_Pw",               "ZZllll",                "PwPy8EG"],
["ZZ_Pw",               "ZZvvll",                "PwPy8EG"],
["ZZ_Pw",               "ZZvvvv",                "PwPy8EG"],
["ggA",			"_ggA",			 ""],
["bbA",			"bbA",			 "bb.a"],
["bbA_fs",		"bbA",			 "bb.s"],  # full sim validation
["HVT",			"_HVT",			 ""],
["monoHbb",		"_monoHbb",		 ""],
["monoWjj",		"_monoWjj",		 ""],
["monoZjj",		"_monoZjj",		 ""],
["ExcitedQ",		"_ExcitedQ",		 ""],
["HZZllqq",		"ZZllqq",		 ""],
["HZZvvqq",             "ZZvvqq",                ""],
["Zee_MG",		"MGPy8EG_A14NNPDF23LO_Zee", ""],
["Zmumu_MG",		"MGPy8EG_A14NNPDF23LO_Zmumu", ""],
["Ztautau_MG",		"MGPy8EG_A14NNPDF23LO_Ztautau", ""],
["Znunu_MG",		"MGPy8EG_A14NNPDF23LO_Znunu", ""],
["Wenu_MG",		"MGPy8EG_A14NNPDF23LO_Wenu", ""],
["Wmunu_MG",		"MGPy8EG_A14NNPDF23LO_Wmunu", ""],
["Wtaunu_MG",		"MGPy8EG_A14NNPDF23LO_Wtaunu", ""],
["Wenu_Pw",		"PwPy8EG_AZNLOCTEQ6L1_Wplusenu", ""],
["Wenu_Pw",		"PwPy8EG_AZNLOCTEQ6L1_Wminusenu", ""],
["Wmunu_Pw",		"PwPy8EG_AZNLOCTEQ6L1_Wplusmunu", ""],
["Wmunu_Pw",		"PwPy8EG_AZNLOCTEQ6L1_Wminusmunu", ""],
["Wtaunu_Pw",		"PwPy8EG_AZNLOCTEQ6L1_Wplustaunu", ""],
["Wtaunu_Pw",		"PwPy8EG_AZNLOCTEQ6L1_Wminustaunu", ""],
["Zee_Pw",		"PwPy8EG_AZNLOCTEQ6L1_Zee", ""],
["Zmumu_Pw",		"PwPy8EG_AZNLOCTEQ6L1_Zmumu", ""],
["Ztautau_Pw",		"PwPy8EG_AZNLOCTEQ6L1_Ztautau", ""],
["gammajet",		"gammajet", ""],
["SinglePhoton",	"SinglePhoton", ""],
["Hmumu",		"_mumu.", ""],  # careful not to copy VHbb signal files
]


#############################
# code
#############################

is_eos = ("/eos/" in main_path)
ana_dir = derivation_tag + "_" + energy_tag + "/"
prod_dir = "CxAOD_" + production_tag + "/"
full_path = main_path + ana_dir + prod_dir

def makeDir(path):
  if (is_eos) : command = "xrd eosatlas.cern.ch mkdir " + path
  else :     command = "mkdir " + path
  if verbose : print command
  if not test_only : os.system(command)


if __name__ == "__main__":

  print "Going to copy files from current directory to", full_path

  # make sample dirs

  makeDir(main_path + ana_dir)
  makeDir(full_path)
  for our_sample in our_samples :
    makeDir(full_path + our_sample[0]+"/")

  # loop over subdirs in current dir

  for subdir, dirs, files in os.walk(".") :

    if verbose : print "Directory:", subdir

    # determine sample

    sample = "Unknown"
    for our_sample in our_samples :
      if not our_sample[1] in subdir : continue
      if not our_sample[2] in subdir : continue
      sample = our_sample[0]
    if sample is "Unknown":
      print "Warning: could not determine sample from directory'" + subdir + "'. Skipping."
      continue
    if verbose : print "Sample:", sample

    final_path = full_path + sample + "/" + subdir + "/"
    makeDir(final_path)

    # loop over files in current subdir and copy

    for file in files :
      file_path = subdir + "/" + file
      if verbose : print "File:", file
      if (is_eos) :
        command = "xrdcp "
        if not skip_existing : command += "--force "
        command += file_path + " root://eosatlas.cern.ch/" + final_path + file
      else :
        command = "cp "
        if skip_existing : command += "-u "
        command += file_path + " "+ final_path
      if test_only :
        if verbose : print command
      else :         os.system(command)
      
  print "Copied files from current directory to", full_path
  print "Done!"

