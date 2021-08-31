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

	sed -i "s@string sample_in.*@string sample_in = ${split[1]}@" ../data/framework-test-stage2.cfg
	sed -i "s@string submitDir.*@string submitDir = validationCxAOD-${sampleName[1]}-${sampleName[2]}/${sampleName[0]}@" ../data/framework-test-stage2.cfg
	sed -i "s@string selectionName.*@string selectionName = ${sampleName[2]}@" ../data/framework-test-stage2.cfg
	cd $ORIGINDIR
	mkdir "validationCxAOD-${sampleName[1]}-${sampleName[2]}"
	hsg5framework data/FrameworkExe/framework-test-stage2.cfg
	mv validationCxAOD-${sampleName[1]}-${sampleName[2]}/${sampleName[0]}/data-CxAOD/CxAOD.root validationCxAOD-${sampleName[1]}-${sampleName[2]}/${sampleName[0]}/CxAOD.root
	cd validationCxAOD-${sampleName[1]}-${sampleName[2]}/${sampleName[0]}
	ls | grep -vx "CxAOD.root" | xargs rm -r
	cd $ORIGINDIR


        # Run CxAODReader over the output from the maker
	cd $SCRIPTPATH
	sed -i "s@string dataset_dir.*@string dataset_dir = validationCxAOD-${sampleName[1]}-${sampleName[2]}/@" ../data/framework-read-test-stage2.cfg
	sed -i "s@string analysisType.*@string analysisType = ${sampleName[2]}@" ../data/framework-read-test-stage2.cfg
	cd $ORIGINDIR
	hsg5frameworkReadCxAOD validationHists-${split[0]} data/FrameworkExe/framework-read-test-stage2.cfg

        # Run limitFileCheck.C over the output histograms to create image files
	cd $SCRIPTPATH
	root -b "limitFileCheck.C(\"${ORIGINDIR}/validationHists-${split[0]}/hist-${sampleName[0]}.root\", \"/afs/cern.ch/user/r/rhuang/public/validation/Tag-00-12-00/${sampleName[0]}/hist-${split[0]}.root\", \"${ORIGINDIR}/plots/${sampleName[0]}-${sampleName[1]}/${sampleName[2]}/\")"

    fi
done

# Run htmlWriter.py on images directory to create webpage 
python htmlwriter.py --outdir="${ORIGINDIR}/html" "${ORIGINDIR}/plots"

