set -e

SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
ORIGINDIR=$(pwd)

# Format of samples is <sample type>-<derivation>-<selection type>:<path to sample>
samples=()
samples+=('ttbar-HIGG5D4-2lep:/afs/cern.ch/user/a/atlashbb/public/ttbar/group.phys-exotics.mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.r6630_r6264_p2361_EXT2/')
samples+=('ttbar-HIGG5D1-1lep:/afs/cern.ch/user/a/atlashbb/public/ttbar/group.phys-exotics.mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.r6630_r6264_p2361_EXT1/')
samples+=('ttbar-HIGG5D0-1lep:/afs/cern.ch/user/a/atlashbb/public/ttbar/group.phys-exotics.mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.r6630_r6264_p2361_EXT0/')
samples+=('ttbar-HIGG5D0-0lep:/afs/cern.ch/user/a/atlashbb/public/ttbar/group.phys-exotics.mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.r6630_r6264_p2361_EXT0/')

# Loop over all the samples given
for input in "${samples[@]}"
do
    cd $SCRIPTPATH
    if [ -n "$input" ]; then

        # Run CxAODMaker for the given sample and analysis type
	echo "Running sample $input"

	# split[1] = sample location
	# sampleName[0] = sample type
	# sampleName[1] = derivation
	# sampleName[2] = selection type
	split=(${input//:/ })
	sampleName=(${split[0]//-/ })

	sed -i "s@string sample_in.*@string sample_in = ${split[1]}@" ../data/framework-test-stage1.cfg
	sed -i "s@string submitDir.*@string submitDir = validationTuple-${split[0]}@" ../data/framework-test-stage1.cfg
	sed -i "s@string selectionName.*@string selectionName = ${sampleName[2]}@" ../data/framework-test-stage1.cfg
	cd $ORIGINDIR
	hsg5framework data/FrameworkExe/framework-test-stage1.cfg

        # Run limitFileCheck.C over the output histograms to create image files
	root -q "$SCRIPTPATH/CompareTuples.C(\"${ORIGINDIR}/validationTuple-${split[0]}/data-CxAOD/CxAOD.root\", \"/afs/cern.ch/user/r/rhuang/public/validation/Tag-00-12-00/${sampleName[0]}/tuple-${split[0]}.root\")" > log-${split[0]}.log
    fi
done
