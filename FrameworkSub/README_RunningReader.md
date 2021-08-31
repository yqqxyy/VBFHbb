You are a SM VH(bb) analyser. You want to run the Reader at your own institute (also called to produce inputs), without systematics (for several studies), and with systematics (for proper input to the fit). You need to do three steps: 

* check out and compile the code
* get locally the CxAOD files
* run using a local batch system

The same instructions will also cover the case where you want to get the code on `lxplus` and run on the CxAOD stored on `eos`. But this is slow, `eos` is not reliable, and especially with systematics (which takes several days), we want to run at institutes. The more institutes we have available to run these inputs, the more physics studies we can do and propagate all the way to the end (ft inputs with systematics and run fits on them). For example with a different b-jet energy correction, a different retraining of the MVA, a new analysis category added (like low pT) or an event selection cut change. 

## Check out the code

Reader code changes very rapidly. We add new histograms. Or add new filtered samples. Or change the b-tagging. We recommend you check out the latest HEAD of the master branch to test the code, to develop further. Only when you really make a bit input production for a big result, say for the unblinding, ask for a tag to be made, and then check out the tag and make a new production. OK, so to check out the master. 

If your institute has a different username than that at CERN, define the environment variable `CERN_USER` to that of CERN. Add to the `.bashrc` or equivalent to not have to worry about that in the future.

Go to your favorite code folder. There is one script by Adrian Buzatu (Adrian.Buzatu@cern.ch) that allows you to do all in one go: check out, set up the desired `AnalysisBase` and compile. 

```
source /afs/cern.ch/user/a/abuzatu/work/public/BuzatuAll/BuzatuATLAS/CxAODFramework/getMaster.sh
```

Running without arguments will tell you what arguments it expects. The first row tells you the latest AB branch available, and the second row the latest tag available. Note the `FOLDER` names are just names. They can be anything else, but are chosen to be suggestive of what is checked out. The assumption is in the same folder you will check out other versions of the CxAOD package too. `DO_PACKAGES` will use `FrameworkSub` to check out the other packages. `DO_BUILD` will set up the `AnalysisBase` hard coded in `FramworkSub` and will compile. You want to set this to zero if you want to change the `AB` version by hand (to a new one, for example) and then to compile. So to check out the head now, when the `AB` is `21.2.33`, you do.

```
source /afs/cern.ch/user/a/abuzatu/work/public/BuzatuAll/BuzatuATLAS/CxAODFramework/getMaster.sh origin/master CxAODFramework_branch_master_21.2.33_1 1 1
```
It will take about 5 minutes to check out and compile. You will be left in the `run` folder.

## Get the CxAOD locally

Go to your favorite `CxAOD` folder and create one for example based on the date. We will get the CxAOD `31-10` in this example. This also contains the systematics. There are two ways of getting the code: to copy from eos, or to download from the grid. The latter option is recommended, as is faster, and it is possible as we have replicated the CxAOD to the grid. But note that some productions like `2L d` has not been replicated to the grid, so you have to copy from eos.

# Copy from eos

You can get the eos locations from Adrian's script
```
less /afs/cern.ch/user/a/abuzatu/work/public/BuzatuAll/BuzatuATLAS/CxAODFramework/eosLocation.txt
```
meaning
```
CxAOD31-10 (with systematics)
/eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10/
0L a: /eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10/HIGG5D1_13TeV/CxAOD_31-10_a
0L d: /eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10/HIGG5D1_13TeV/CxAOD_31-10_d
1L a: /eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10/HIGG5D2_13TeV/CxAOD_31-10_a
1L d: /eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10/HIGG5D2_13TeV/CxAOD_31-10_d
2L a: /eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10/HIGG2D4_13TeV/CxAOD_31-10_a
2L d: /eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10/HIGG2D4_13TeV/CxAOD_31-10_d
```
You can also take it from the [Twiki](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/Release21Migration#NEW_Tag_r31_10_on_30_April_CxAOD).

If you copy from eos, you copy the exact folder structure. To copy one folder from eos to your local institute using `xrdcp` you do, for example for `2L d`, you do: 
```
xrdcp -r root://eosatlas.cern.ch///eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10/HIGG2D4_13TeV/CxAOD_31-10_d .
```
Expect this to be slow (1-2 days). So better to create first the folder structure you want:
```
mkdir -p CxAOD_r31-10/HIGG2D4_13TeV/CxAOD_31-10_d
cd CxAOD_r31-10/HIGG2D4_13TeV/CxAOD_31-10_d
xrdcp -r root://eosatlas.cern.ch///eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10/HIGG2D4_13TeV/CxAOD_31-10_d .
```
Once it finishes, you verify that you have all, by comparing with AMI. At eos the files are not executable, so if you copy from there they will not be either. So let's run by putting python in front.
```
pythoon count_Nentry_SumOfWeight.py 1 0 0 
```
You may need to setup root first for this 
```
setupATLAS
lsetup root
```
Maybe your file is not executable (as on eos it is not), then either make it executable 
```
chmod +x 
```
Then you compare with AMI
```
python checkYields.py
```
You will get an output like this
```
Total CxAOD =  1421334920.0
Total DxAOD =  1439004758.0
Ratio       =  0.987720792512
Yield ratios written to: dslist_NevtDxAOD_yield.txt
```
You can look inside `dslist_NevtDxAOD_yield.txt` to see for each the fraction of events done. And to see the list of those not full done do
```
grep -v 1.000 dslist_NevtDxAOD_yield.txt
```
Hoping the samples you are not at 100% are not critical to your channel, we go ahead.
We count the yields for the Reader.
```
python python count_Nentry_SumOfWeight.py 0 0 1
```
This will create the file `yields.13TeV_sorted.txt` from where we will use the yields. Note: because we will use `submitReader.sh` we will not have to first copy these yielsds into `FrameworkSub/data` with a given name and then recompile. Remember the path of this folder, as we will put this in our config file.

## Copy from grid

Insead of copying from eos, we will copy from grid. It should be faster, as eos is slow in general. Especially for those that are outside Europe.

Let's open a new terminal. 

Create another folder than the one where your final `eos-like` structure will be present. Say a folder with the current date in the name. 

Store an environment variable with the name of your current CxAOD framework package that you just checked out. 

```
C=/data06/abuzatu/code/CxAODFramework_branch_master_21.2.33_1
```
We use the script `getMaker.sh` from Adrian Buzatu (adrian.buzatu@cern.ch).
```
source $C/source/FrameworkExe/scripts/getMaker.sh
```
It will crash in a controlled way and tell you the arguments it needs. Behind the scene it will call another script from Adrian, `prepareCxAODProduction.sh`, which does in one go many steps that were presented in the twiki of the first analysis in Run-2 to be done by hand. 

Let's say you want to be ready to download any of the `0L,1L,2L` and `a,d`. So we run.
```
source $C/source/FrameworkExe/scripts/getMaker.sh 0L,1L,2L a,d VHbb mcdata Higgs 31-10 $C 1
```
It will ask you for the grid password.

This will take maybe 20 minutes to do for all the channels. You can decide to do for only one of them. 

Then for each of the pairs like `1L_a` it will go through all the output samples defined in `FrameworkSub/data/In/VHbb` for `mcdata` of `1L_a` and for each go to `AMI` to find out the number of derivation events. After we download the CxAOD, we will count the yields and compare with AMI to make sure we downloaded all. You have seen these steps done in the case above of when copying from eos. Note that currently we have an extra complication with some samples coming from another derivation tag `VHbb2`. One should redo the same exercise for that. If you copy from eos, you do not need to worry about that.

Now go inside one folder, and download from the grid with 
```
cd prepare_2L_d_31-10
./rucio_get_jobs.py out_sample_list_sample_grid.13TeV_25ns.mcdata_d.HIGG2D4.txt 
```
Once it finishes (it may take 1-2 days) you repeat the steps from the eos instructions, to compare with ami, and then compute the yields you can use.
```
./count_Nentry_SumOfWeight.py 1 0 0 
./checkYields.py
grep -v 1.000 dslist_NevtDxAOD_yield.txt
./count_Nentry_SumOfWeight.py 0 0 1
```
Let's say all is fine. You now want to create the `eos-like` folder structure. We will use the script that copies to eos. Except we will copy locally to another folder (the same folder as form the eos instructions). And to not occupy twice the disk space, we will create there the eos-like folders and inside make sybmolic links to the CxAOD we have downloaded here. So we run with no arguments to tell us what the arguments are.

```
./copy_CxAODs_to_eos.py
```
Assuming I copy the `2L` and `d` into `/data06/abuzatu/data/CxAOD/180310All` you do
```
./copy_CxAODs_to_eos.py 0 1 1 1 HIGG2D4    31-10_d /data06/abuzatu/data/CxAOD/180310All
```
Now you have your folder structure. 
```
cd /data06/abuzatu/data/CxAOD/180310All/HIGG2D4_13TeV/CxAOD_31-10_d
ls
```
You can double check that it is indeed all fine by running again the steps 
```
./count_Nentry_SumOfWeight.py 1 0 0 
./checkYields.py
grep -v 1.000 dslist_NevtDxAOD_yield.txt
./count_Nentry_SumOfWeight.py 0 0 1
```
Now you have the CxAOD ready to run the full inputs on.

Of course, for a quick test you can put only one file of one process, and build the folder struture by hand. 

## Run the Reader

We have many choices, which translate to many changes in the config file `FrameworkExe/data/framework-read.cfg`. You have channels `0L`, `1L` or `2L`. You have periods `a` (data15+data16) or `d` (data17), with different integrated luminosities, good run lists, lumi files and pileup reweighting files. There is different types of b-tagging: direct, truth and hybrid. To have the EPS style as suggested by Yanhui, you need to run only a few samples (V+jets with C or L filter) in the truth b-tagging and combine with the rest of the studies with direct b-tagging. So you need change the samples lists in these cases. Then you can run the MVA selection, or the CUT (cut based analysis) selection. Adrian Buzatu (Adrian.Buzatu@cern.ch) created a script `submitReader.sh` that uses the `FrameworkExe/data/framework-read-automatic.cfg` to do all of these for you in the background, so that you can simply submit the options you want. And not only one, but a series of options, so that you can submit once, say at night, and overnight you have all these inputs running on your local batch node (for systematics it may take only about 1 hour per channel), so that by morning you can study the inputs (i.e. make plots and draw physics conclusions). As new features are identified, they are added to the script. For example. 

Let's assume you start a new terminal after you have already checked out and compiled the code. Go to the run folder.

```
cd /data06/abuzatu/code/CxAODFramework_branch_master_21.2.33_3
cd build
asetup
source x86_64-slc6-gcc62-opt/setup.sh 
```
By the way, if you make any modifications to the code and you want to recompile you do.
```
cd build
cmake --build .
```
If you have also added a new file, you do
```
cd build 
cmake ../source
cmake --build .
```
Let's take a look at the script.
```
cd ../run
emacs -nw ../source/FrameworkExe/scripts/submitReader.sh 
```
The script will take its mostly changed options from the command line arguments. But it also has some options you may want to change (maybe more rearly) from the variables defined at the top. Based on that, the code below will do all it needs for you. It will create the config file and run with that. 

Let's say we want to run only on 1000 events, locally, of a signal sample, with direct tagging. We change.
```
NUMBEROFEVENTS="-1" 
```
to 
```
NUMBEROFEVENTS="1000"
```
and similarly to
```
DRIVER="direct"
```
and we uncomment these lines
```
# SAMPLES1="qqZvvHbbJ_PwPy8MINLO"                                                                                                                               
# SAMPLES2="ZnunuL_Sh221" 
```
And we run with
```
../source/FrameworkExe/scripts/submitReader.sh /data06/abuzatu/data/CxAOD/180310All /data06/abuzatu/data/Reader/180610_01 0L a MVA D1 31-10 1
```
We then go at the folder `/data06/abuzatu/data/Reader/180610_01` and check the output.

If you expect to develop the code, it is useful to use these workflow to compile and run in one command. And you must every time put in a different folder.
```
cd ../build && cmake --build . && cd ../run && ../source/FrameworkExe/scripts/submitReader.sh /data06/abuzatu/data/CxAOD/180310All /data06/abuzatu/data/Reader/180610_Test_${N} 0L a MVA D1 31-10 1
```

Let's say you finished your development and now you want to submit the Reader for the night, so that you check the results in the morning. You want `0L` boht `a` and `d`, both `MVA` and `CUT` selections and to have available both direct and truth b-tagging. You then go back to the settings as they were. Number of events to -1. The batch system that you have at your institute. It is condor by default. Comment again the `SAMPLES` you added, to use those already defined. Well, the first time you use it it is safe to just run on one case on one sample. But here how to run them all. 

```
N=01 && cd ../build && cmake --build . && cd ../run && ../source/FrameworkExe/scripts/submitReader.sh /data06/abuzatu/data/CxAOD/180310All /data06/abuzatu/data/Reader/180610_Test_${N} 0L a,d MVA,CUT D1,D2,T2 31-10 1
```

The `V+jets` samples with `C` and `L` filters are processed with both direct (D2) and truth (T2) tagging, whereas the others are processed only with direct (D1). In the morning you will `hadd` D1+D2 to get the pure direct tagging, and `D1+T2` to get the Truth tagging a la EPS and a la Yanhui. 

The sample lists are those of `0L` at the moment. Modify them if you have some particularities for your channel, say `ttbar_dilep` for `2L`. 
The script is generic, but the configs of running with the `VVbb` filtered samples and the `ttbar` filtered samples have not yet been added.

The script will be updated gradually with the things that become default. 

## Resubmit failed jobs on batch

Sometimes on batch some jobs fail. You can submit them one at a time like
You see that like
```
status/fail-25
```
And you resubmit with 
```
./submit/run 25
```
More info in this [GitLab issue](https://gitlab.cern.ch/CxAODFramework/FrameworkExe/issues/19)













