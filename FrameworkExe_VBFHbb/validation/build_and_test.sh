#!/bin/bash

RESULTSDIR=/afs/cern.ch/work/n/nmorange/public/CI/results
PLOTSDIR=/afs/cern.ch/user/n/nmorange/www/HbbPlots/CI/
SAMPLE=/afs/cern.ch/work/n/nmorange/public/CI/sample

mkdir -vp ${RESULTSDIR}
mkdir -vp ${PLOTSDIR}

TODAY=$(date +%y-%m-%d-%H-%M)
if [ -z $1 ]
then
  AGAINST=$(ls $RESULTSDIR | tail -n1)
else
  AGAINST=$1
fi
echo "Testing against $AGAINST"

mkdir -vp $RESULTSDIR/$TODAY

cd /tmp/nmorange
mkdir CxAOD
cd CxAOD
svn co svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/HiggsPhys/Run2/Hbb/CxAODFramework/FrameworkSub/trunk FrameworkSub
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
source FrameworkSub/bootstrap/setup-trunk.sh
rc build

for deriv in HIGG5D1 HIGG5D2 HIGG2D4
do

  # Run once with event selection and OR

  # need to hack the config file to replace the selectionName :-((((
  if [ $deriv = "HIGG5D1" ]
  then
    selName=0lep
  elif [ $deriv = "HIGG5D2" ]
  then
    selName=1lep
  else
    selName=2lep
  fi

  sed '/string selectionName/c\string selectionName = '"${selName}"'' FrameworkExe/data/framework-run.cfg > FrameworkExe/data/config_${deriv}.cfg

  hsg5framework --sampleIn $SAMPLE/${deriv}/ --submitDir $RESULTSDIR/$TODAY/${deriv} --config data/FrameworkExe/config_${deriv}.cfg
  mkdir -vp ${PLOTSDIR}/${TODAY}/${deriv}
  root -b -l -q "FrameworkExe/validation/CompareCxAODs.C(\"${RESULTSDIR}/${AGAINST}/${deriv}/data-CxAOD/CxAOD.root\", \"${RESULTSDIR}/${TODAY}/${deriv}/data-CxAOD/CxAOD.root\", \"${PLOTSDIR}/${TODAY}/${deriv}\")"

done

# Run once WITHOUT event selection and OR. It's ok if it's done only for 1 derivation then
sed '/bool applyEventSelection/c\bool applyEventSelection = false'  FrameworkExe/data/config_${deriv}.cfg > tmp.cfg
sed '/bool applyOverlapRemoval/c\bool applyOverlapRemoval = false'  tmp.cfg > FrameworkExe/data/config_nosel.cfg
hsg5framework --sampleIn $SAMPLE/${deriv}/ --submitDir $RESULTSDIR/$TODAY/NoSelections --config data/FrameworkExe/config_nosel.cfg
mkdir -vp ${PLOTSDIR}/${TODAY}/NoSelections
root -b -l -q "FrameworkExe/validation/CompareCxAODs.C(\"${RESULTSDIR}/${AGAINST}/NoSelections/data-CxAOD/CxAOD.root\", \"${RESULTSDIR}/${TODAY}/NoSelections/data-CxAOD/CxAOD.root\", \"${PLOTSDIR}/${TODAY}/NoSelections\")"

python FrameworkExe/validation/createHtmlOverview.py ${RESULTSDIR}

