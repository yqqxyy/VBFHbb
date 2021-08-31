#!/usr/bin/env python

# script to merge xsection based on exclusive division between MC15a and MC15b

def reformat(line):
    # assume only valid lines are given
    s = line.split()
    return "%s  %4e   %4e   %4e  %20s    %s" % (s[0], float(s[1]), float(s[2]), float(s[3]), s[4], s[5])

mc15a = []
for line in file("FrameworkSub/In/list_sample_grid.mc15a_only_13TeV_25ns_HIGG5D2_09feb16.txt"):
    s = line.split(".")
    if len(s) > 2: mc15a.append(s[1])

mc15b = []
for line in file("FrameworkSub/In/list_sample_grid.mc15b_only_13TeV_25ns_HIGG5D2_09feb16.txt"):
    s = line.split(".")
    if len(s) > 2: mc15b.append(s[1])

for line in file("FrameworkSub/data/XSections_13TeV.txt"):
    if any(DSID in line for DSID in mc15a):
        print reformat(line)

for line in file("FrameworkSub/data/XSections_13TeV_mc15b.txt"):
    if any(DSID in line for DSID in mc15b):
        print reformat(line)
