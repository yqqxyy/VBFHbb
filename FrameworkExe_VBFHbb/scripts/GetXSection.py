########################
#setup first pyAMI and voms proxy
#
# $ setupATLAS
# $ localSetupPyAMI
# $ voms-proxy-init -voms atlas
#
########################

import sys
import pyAMI.client
import pyAMI.atlas.api as AtlasAPI
client = pyAMI.client.Client("atlas")
AtlasAPI.init()

if len(sys.argv)==1:
    print "You must provide a DSID to search for"


DSID = str(sys.argv[1])

print "Getting info for: ",DSID

checkname      = "mc15_13TeV.%"+DSID+"%.AOD.%"

print checkname

possible_names = AtlasAPI.list_datasets(client, patterns = [checkname])

if possible_names==[]:
    print "NO DATASET FOUND FOR: ",checkname
else:
    datasetnameAMI   = str(possible_names[0]["ldn"])
    datasetinfo      = AtlasAPI.get_dataset_info(client,datasetnameAMI)
    neventsAMI       = float(datasetinfo[0]['totalEvents'])
    crosssectionAMI  = float(datasetinfo[0]['crossSection'])
    filtereffAMI     = float(datasetinfo[0]['approx_GenFiltEff'])
    
    print "dataset name  = ",datasetnameAMI
    print "cross section = ",crosssectionAMI
    print "filter eff    = ",filtereffAMI
    
