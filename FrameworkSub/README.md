This README describes in detail the steps the scripts that allow you to do in one go the checkout and compilation of code; then to submit the Maker on the grid (or locally); then to download the Maker from the grid, compare with AMI, copy to eos, compare with AMI again, and count the yields. 

First we will show directly the commands to run out of the box for the CxAOD production v31 with systematics with tag r31-10. In the following sections we explore in details each of the scripts, and the command line arguments they support for different features.

# Instructions for producers CxAOD production on grid r31-10 (v31 with systematics)

Check out the tag `r31-10`, compile and set it up. All in one go by the script.

```
source /afs/cern.ch/user/a/abuzatu/work/public/BuzatuAll/BuzatuATLAS/CxAODFramework/getMaster.sh r31-10 CxAODFramework_tag_r31-10_1 1 1
```
You will be left in the `run` folder. 
To simplify the instructions, let's define your CxAOD folder for yourself, for example.
```
C=/data06/abuzatu/code/CxAODFramework_tag_r31-09_2
```
In the future we will use `$C`, so that you can copy/paste the commands below.
Let's go before the `run` folder.
```
-bash-4.1$ cd ..
-bash-4.1$ pwd
/data06/abuzatu/code/CxAODFramework_tag_r31-10
-bash-4.1$ ls
build  run  source
```

Let's suppose you are the producer of '0L a' with `Higgs` production role. You will change the arguments from below by using `1L` or `2L` instead of `0L` and `d` instead of `a`, and `Exotics` instead of `Higgs` for the production role.

We will need to submit two lists with different settings of number of files per jobs. The `mcdata_nominal` list contains the and nominal MC. The `mcdata_alternative` contains the data and alternative MC. Both will be submitted with `nominalOnly = false` (so to store systematics). The `AnalysisBase.cxx` has been updated to be smart enough to set it back internall to `nominalOnly = true` for data and alternative MC. But what we noticed is that when running with systematics it takes very long, so we will submit with one file per job for systematics for the nominal MC, which is in the sample list `mcdata_nominal`. For the alternative MC and data, stored in `mcdata_alternative`, we can set 600 GB per job and still run reasonably fast. 

```
emacs -nw source/FrameworkExe/scripts/submitMaker.sh 
```
Looks what the settings are. 
```
NRGBPERJOB="-1"
NRFILESPERJOB="1"
```
These are the values for the nominal. So submit those samples.
```
source source/FrameworkExe/scripts/submitMaker.sh 0L a VHbb mcdata_nominal 31-10 Higgs 1 none
```
You will be asked for your CERN grid certificate. The grid will be given to you for 4 days. The submission can take 15-30 minutes. It will wait at the first sample for the tarball to be submitted to the grid. All the other samples will then be submitted fast, as this new tarball will be used.

You will be left in the `run` folder, where the submission folder will be `Maker_VHbb_mcdata_nominal_0L_a`. Go back outside the `run` folder.

```
cd ...
```

We are ready to start the new production for the `mcdata_alternative` samples.
```
emacs -nw source/FrameworkExe/scripts/submitMaker.sh 
```
Change the settings to 
```
NRGBPERJOB="600"
NRFILESPERJOB="-1"
```
Launch the production 
```
source source/FrameworkExe/scripts/submitMaker.sh 0L a VHbb mcdata_alternative 31-10 Higgs 1 none
```
You will be asked again for the grid certificate. You already have one valid for 4 days, so no need to put one again. Simply press enter. You will again have to wait 15-30 minutes for the first tarball to be submitted to the grid, and then the other samples will go faster.

You will be left in the `run` folder, where now you see also the folder `Maker_VHbb_mcdata_alternative_0L_a`. Go out.
```
cd ...
```

OK, you have submitted, now you should _right away_ prepare the folder where you will download from the grid and compare with AMI. For easier book keeping, we will do that in one go, using the sample `mcdata`, instead of the two samples when we submitted `mcdata_nominal` and `mcdata_alternative`. 

Open a new terminal on the machine where you want to copy, and go there. If you do not have large hard disk at your institute with fast internet, you may want to download in your own `eos` folder at CERN. If you do not run from your office desktop, but from your laptop, you may want to use `screen` environment so that the download continues independent of you closing down your laptop. Assumming you are use the laptop at your institute with screen, here is what I do.

First ssh to that machine, go to that folder and do not use screen yet.
```
cd /data06/abuzatu/data/CxAOD/
mkdir 180430
cd 180430
```
Define `C` as the path to your CxAOD folder, same as above, but not yet defined in the new terminal
```
C=/data06/abuzatu/code/CxAODFramework_tag_r31-10
```
One script that prepares everything.
```
source $C/source/FrameworkExe/scripts/getMaker.sh 0L a VHbb mcdata Higgs 31-10 $C 1
```
You will be asked for your grid certificate password, if you are on a new machine, you need to put it again. If on the same machine as before, you can press enter.
This will take about 5 minutes to read the AMI info for each of the about 500 samples. This will produce the AMI info file, the output sample name, the script to download from the grid with rucio, the script to copy to eos, the script to replicate to grid. Please see in the section below all of these options.

You have the folder appearing `prepare_mcdata_0L_a`. Go there. You start to download from the grid with 
```
./rucio_get_jobs.py out[tab]
```
Since you just launched, nothing will be found, but it will also inside the script compare with AMI and give you the result. The final outcome will be nothing is done.
At this point you can launch with screen to run in a loop to download as soon as samples finish. First get out of the folder. 
```
cd ..
```
And here copy this script
```
cp $C/source/FrameworkExe/scripts/runRepeatedlyRucioGetJobs.py .
```
Open the file and modify to your taste. It has a template of downloading several times for several folders in parallel (you hve only one folder), wait some time and do it again. I find it useful to set to 10 times during the night, check in the morning, then maybe launch it again. 

Or given you have a grid certificate for 4 days and Tuesday is a free day at CERN, you can set it 20 times with 3 hour break between each download. 

You can even optimise this more to add the step to copy to eos in the same script, so that every 3 hours you first download all that is new (rucio is smart enough not do download what you already have), then copy everything to eos (eos is smart enough not to copy what you already have there). With this workflow, when the jobs finish running, shortly after they are all both locally and in eos. 

At the end when you come back to office you check the comparison with AMI, relaunch jobs that are problematic (the usual), but those that are good are already here and people can already use them.

Two weeks after finshing, grid outputs will start to be deleted. You have to replicate before that. There is a script that does that. 

You also have to compare with AMI both locally and in eos with option `1 0 0` (even if locally you are at 100%, on eos maybe some files had problems to be copied), and then run the final yields for the reader (option `0 0 1`). Note that on eos the machine is slow and running these yields can take even 30 minutes. And at the end, if a single file is not good, the yield file is not created, and you have to either remove the bad file, and start again. But note that often the file is good, just that the eos had problem to make the file readable at that very time. So doing it again may work on those files, but fail on others. Eos is a pain, but it is your duty as producer to persevere until eos is at 100% done (ratio `1.000` for all).

To copy to eos 
```
cd prepare_mcdata_0L_a
./copy_CxAODs_to_eos.py
```
It will tell you the eos locations. 0L and 2L will be in the Higgs area. The 1L will be on Tatsuya
```
/eos/atlas/unpledged/group-tokyo/users/tatsuya/CxAOD/CxAOD_r31-10/HIGG5D2_13TeV
```
A symbolic link will be in the Higgs area. 
```
/eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10/HIGG5D2_13TeV
```
The others would be in the same place
```
/eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10/HIGG5D1_13TeV
/eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10/HIGG2D4_13TeV
```

So we can all use the same path `/eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10`.
```
 ./copy_CxAODs_to_eos.py 0 1 1 0 HIGG5D1 31-10_a /eos/atlas/atlascerngroupdisk/phys-higgs/HSG5/Run2/VH/CxAOD_r31-10
```
For various cases, replace `31-10_a` with `31-10_d` for `d`, and replace the derivation with your own `HIGG5D2` and `HIGG2D4`.

We can integrate this script to copy to eos in the same script that does it in a loop, as shown above.

The following sections will present in detail the other sections.

# Check out and compile in one go

```
source /afs/cern.ch/user/a/abuzatu/work/public/BuzatuAll/BuzatuATLAS/CxAODFramework/getMaster.sh
```
The script will fail in a controlled way, telling you what arguments it expects. To get the head of a given branch, use `origin/` followed by the branch name, typically `origin/master`, while if you want a tag, use the tag name, like `r31-09`. The second argument is the CxAOD folder name. The first thing it does, it checks out the `FrameworkSub` package. The third argument gives you a choice how to proceed. With `0` you stop here. With `1` the boostrap script from `FrameworkSub` will check out all the other packages. Again you have a choice. Do you want to compile, do `1`. If not, do `0`. The latter option is useful when you want to test a new AnalysisBase release. In that case you change the hard coded value in `FrameworkSub` and then proceed to compile by hand. Examples are below.

* Checkout and compile the head of the master branch

```
source /afs/cern.ch/user/a/abuzatu/work/public/BuzatuAll/BuzatuATLAS/CxAODFramework/getMaster.sh origin/master CxAODFramework_branch_master_21.2.27_1 1 1
```

* Checkout and compile the tag r31-09

```
source /afs/cern.ch/user/a/abuzatu/work/public/BuzatuAll/BuzatuATLAS/CxAODFramework/getMaster.sh r31-09 CxAODFramework_tag_r31-09_1               1           1
```

* Checkout the head of the master, change the Analysis Base and compile by hand

Check out all packages withour recompiling.
```
source /afs/cern.ch/user/a/abuzatu/work/public/BuzatuAll/BuzatuATLAS/CxAODFramework/getMaster.sh origin/master CxAODFramework_branch_master_21.2.27_1 1 0
cd FrameworkSub/boostrap
emacs -nw release.txt
```
Change from `21.2.27` to `21.2.28` for example. Now compile.
```
cd ../../../build/
setupATLAS
lsetup asetup
release=`cat ../source/FrameworkSub/bootstrap/release.txt`
echo "release=$release"
asetup AnalysisBase,$release,here
cp CMakeLists.txt ../source
cmake ../source
cmake --build .
source x86_64-slc6-gcc62-opt/setup.sh
```

* If in addition you want to have access ty PyRoot, say to read your CxAOD files in a PyRoot format, set extra
```
lsetup 'lcgenv -p LCG_91 x86_64-slc6-gcc62-opt numpy' 
```

# Run Maker to produce CxAOD

There is one script that allows you do all the channels and mcType in one go: `(0L,1L,2L)x(a,c,d)`. 

It uses the config file `FrameworkExe/scripts/framework-run-automatic.cfg`, which has dummy strings that are replaced for a given channel, say `0L a` with the appropriate settings for the pileup reweighting, but also others. For example, for 1L in the current derivations the `XbbTag` must be false, as b-tagging info is missing for the track jets. For `2L` we need to set trigger emulation to false. For `2L` the `TruthWZ` is not set in the current derivations, so we set `Truth` instead. To avoid human error, all these are set for you. 

The sample lists are stored in `FrameworkSub/In` for two derivations `VHbb` (ours) and `monoHbb` (for the mono-Hbb analysis), and we can add others in the future. The samples have the same naming convention for all the channels, so that the script can pick them up based on the info like `0L a`. The samples `test` contain a few data runs, a few MC signals, and a few MC backgrounds. The MC backgrounds are both some nominal and some alternative. When we run with `bool nominalOnly = false`, the systematic uncertainties are stored in the CxAOD files for nominal MC, but not for data or alternative bkg. The samples `mcdata` contain all the samples available. Some are commented out: for data, those not in GRL. For all, those that although have a container in rucio, have zero events (unfinished production). If for a given DSID both full simulation (FS) and ATLAS Fast Simulation 2 (AF2) is present, the AF2 is commented out and only FS is used. Otherwise it would double count in the analysis when stored in eos in the same folder. Each of `test` and `mcdata` come in flavors of `_a`, `_c` and `_d`. By now `d` has completely removed `c`, except for a few samples not yet available. `a` contains `data15`, `data16` and `mc16a`. `c` contains only `mc16c`. `d` contains `data16` and `mc16d`. They can be grouped together as they use the same pileup reweighting settings. It is also safest and easiest book keeping to submit them all to grid in one go. You can create your own list and you can use this script if you respect the same naming convention. If you add new samples to an existing list, you can submit again with the same `vtag`. Panda is smart enough to not submit the same dataset twice. The script can be run also for local samples. So to run the script, either from a fresh terminal, or from the one where you already compiled.

```
cd CxAODFramework_tag_r31-09_1
ls
```
You will see `build`, `run`, `source`. Run 
```
source source/FrameworkExe/scripts/submitMaker.sh 
```
It will crash in a controlled way and tell you what arguments it needs. When arguments have commas inside it means it splits by the comma and do in a loop. For example `0L,1L,2L` and `a` means thar for each of `0L`, `1L` or `2L` the `a` is done. If we use `0L,1L,2L` and `a,d`, then for each of `0L`, `1L` or `2L` the `a` and `d` is done. The third argument is what derivations to take. We have `VHbb` (our analysis) or `monoHbb` (monoHbb anlaysis, though they have been shown to have a bug still and are being discontinued). You also have defined `v2` which is test derivation files provided by the derivation contacts. You can extend the script locally in similar way when they will provide new test files. The fourth argument is the list of samples. You can run only `mcdata`, or only `test`, or both with a comma between them, or you can create your own sample list, or several, and call that. If you add new sample lists, you need to recompile, so that they are picked up. 
```
cd build
cmake ../source
cmake --build .
source x86_64-slc6-gcc62-opt/setup.sh
```
The fifth argument is the `vtag`. If you run from a tag number, use that. For `r31-09`, use `31-09`. If you run from master, add something to differentiate your vtag, maybe `31-09-MA-0428`, meaning that it is some master done after `31-09`, and implicitely before a future `31-10`, on the date of 28 April. That way, when looking back, you know what info was in that tag. As the different commits and logs are maintained in the Twiki.

https://twiki.cern.ch/twiki/bin/view/AtlasProtected/Release21Migration#List_of_Merge_requests

The next argument is about grid privileges. If you submit with your regular user account, choose `user`. If you have a Higgs production role and you use that, say `Higgs`. If you have an Exotics production role and you use that, choose `Exotics`. The script will produce the right command of rucio to set this up. It also sets the proxy for 4 days, instead of the usual 12 hours. That is also recommended when you are downloading. 

The next argument is a boolean. With 1, you actually run. With 0, it will make printouts of what it is doing. The looping over categories, the creating of the configuration file to run, but it will not actually try to submit. You can thus check if the config file looks reasonable.

The last argument is about if you run locally or not. As we submit to grid, we say `none`. 

The submission will ask for your grid password. It will also tell you how much time you have your current proxy valid. If you have many more days, you can press enter instead of the password. The current attempt will fail, but your previous proxy remains valid. The submission will take some time, for the tarball to be copied to the grid for the first sample. It can be 10-30 minutes. Then all the samples in the same list are submitted fast, using the same tarball now on the grid. You see now how this speeds up by putting all samples into one go (signal, signDBL, backgr, data). And it allows you to submit only one script for all channels and let it do its magic, while you are free to work on other topics. 

The same script allows you to run locally on a CxAOD. The last argument should not be `none`, but the local path to the folder, say `/data06/abuzatu/data/DxAOD/fromGoetz/180413/user.goetz.410501.HIGG5D1_20180410T0823.e5458_s3126_r9781_r9778._EXT0`. To guide the script to not mix the two cases (in one case it's a sample list, in the other only a certain process locally), some of the previous arguments are then set to `none`. 

To summarise, to run to the grid on `0L` both `a` and `d` on `test` you do.
```
source source/FrameworkExe/scripts/submitMaker.sh 0L a,d VHbb test 31-09 Higgs 1 none
```
and to run locally on the `v2` of the derivation samples, which are in `c`, you do
```
source source/FrameworkExe/scripts/submitMaker.sh  0L c v2 none none Higgs 1 /data06/abuzatu/data/DxAOD/fromGoetz/180413/user.goetz.410501.HIGG5D1_20180410T0823.e5458_s3126_r9781_r9778._EXT0
```

# Monitor jobs and resubmit when needed

To monitor your jobs, you can use the regular bigpanda, for example,
```
https://bigpanda.cern.ch/tasks/?taskname=*.CAOD_HIGG5D1.*.31-10/&display_limit=500&days=40&username=Adrian%20Buzatu
```

There is also a python based interface: a script to monitor the jobs on big panda. 

https://github.com/dguest/pandamonium

Get the code somewhere and append the new folder to `PATH`.

```
git clone https://github.com/dguest/pandamonium
cd pandamonium
pwd
```
Get the folder name and append it to `PATH`, for example in `.bashrc` to have it in all terminals.
```
cd ~
emacs -nw .bashrc
export PATH=/afs/cern.ch/user/a/abuzatu/work/public/pandamonium:$PATH # https://github.com/dguest/pandamonium 
```

Start a new terminal. We need Python `2.7`, but we get only `2.6` when we do the regular
```
setupATLAS
lsetup root
```
So let's go to the CxAOD folder and do `asetup`
```
cd /data06/abuzatu/code/CxAODFramework_branch_master_21.2.27_8
cd build
asetup
source x86_64-slc6-gcc62-opt/setup.sh
```

We can run this from any folder, but let's do it from the folder with sample lists.
```
cd ../source/FrameworkSub/In/VHbb/
```
Now we can run our command the same as in `bigpanda`. 

https://bigpanda.cern.ch/tasks/?taskname=*.CAOD_HIGG5D1.*.31-10/&display_limit=500&days=40&username=Adrian%20Buzatu

The equivalent command is
```
pandamon group.phys-higgs.*.CAOD_HIGG5D1.*.31-10 -d 40 --user adrian%20buzatu
```
We can add `-d 40` to show the jobs from the last 40 days.
If you run with group privileges it is useful to see only your jobs, so add like `--user adrian%20buzatu`.

You will see a nice output with 4 columns, color coded, that show which jobs are done, which are running (and what percent has finished), what `taskid` and the job output name. For example:
```
running     11958     6%     group.phys-higgs.mc16_13TeV.361030.CAOD_HIGG5D1.e3569_s3126_r10201_r10210_p3371.31-10/ 
done        11957     100%   group.phys-higgs.mc16_13TeV.361029.CAOD_HIGG5D1.e3569_s3126_r10201_r10210_p3371.31-10/ 
running     11954     13%    group.phys-higgs.mc16_13TeV.361028.CAOD_HIGG5D1.e3569_s3126_r10201_r10210_p3371.31-10/ 
running     11948     96%    group.phys-higgs.mc16_13TeV.361027.CAOD_HIGG5D1.e3668_s3126_r10201_r10210_p3371.31-10/ 
running     11955     0%     group.phys-higgs.mc16_13TeV.361026.CAOD_HIGG5D1.e3569_e5984_s3126_r10201_r10210_p3371.31-10/ 
done        11953     100%   group.phys-higgs.mc16_13TeV.361025.CAOD_HIGG5D1.e3668_s3126_r10201_r10210_p3371.31-10/ 
done        11952     100%   group.phys-higgs.mc16_13TeV.361024.CAOD_HIGG5D1.e3668_s3126_r10201_r10210_p3371.31-10/ 
running     11951     98%    group.phys-higgs.mc16_13TeV.361023.CAOD_HIGG5D1.e3668_s3126_r10201_r10210_p3371.31-10/ 
done        11950     100%   group.phys-higgs.mc16_13TeV.361022.CAOD_HIGG5D1.e3668_s3126_r10201_r10210_p3371.31-10/ 
done        11949     100%   group.phys-higgs.mc16_13TeV.361021.CAOD_HIGG5D1.e3569_s3126_r10201_r10210_p3371.31-10/ 
done        11947     100%   group.phys-higgs.mc16_13TeV.361020.CAOD_HIGG5D1.e3569_s3126_r10201_r10210_p3371.31-10/ 
running     11946     71%    group.phys-higgs.mc16_13TeV.345935.CAOD_HIGG5D1.e6620_e5984_s3126_r10201_r10210_p3371.31-10/ 
broken      11945     0%     group.phys-higgs.mc16_13TeV.345114.CAOD_HIGG5D1.e6042_e5984_s3126_r10201_r10210_p3374.31-10/ 
done        11944     100%   group.phys-higgs.mc16_13TeV.345110.CAOD_HIGG5D1.e5765_e5984_s3126_r10201_r10210_p3374.31-10/ 
running     11942     29%    group.phys-higgs.mc16_13TeV.345109.CAOD_HIGG5D1.e5765_e5984_s3126_r10201_r10210_p3374.31-10/ 
running     11940     47%    group.phys-higgs.mc16_13TeV.345058.CAOD_HIGG5D1.e6004_e5984_s3126_r10201_r10210_p3374.31-10/ 
running     11939     31%    group.phys-higgs.mc16_13TeV.345057.CAOD_HIGG5D1.e5706_e5984_s3126_r10201_r10210_p3374.31-10/ 
```

More instructions about `README` 
https://github.com/dguest/pandamonium/blob/python2/README.md

We can see arguments with 
```
pandamon -h
```


Let's assume you run `0L d` with the Higgs group privileges. To not overlap with the jobs from `0L a`, we have to do in two steps: one for `data17`, one for `mc16` with the r-tag `r10210`.

```
pandamon group.phys-higgs.*.CAOD_HIGG5D1.*.31-10 -d 40 --user adrian%20buzatu > a.log
```

You can resumbmit via `pbook` all jobs that are either `finished` or `failed`, but not those thare are `broken`. 
```
cat a.log | awk '$1 ~ /finished/ {print $2}'
cat a.log | awk '$1 ~ /failed/ {print $2}'
```
Normally we open `pbook`, which synchronises, and then we put the `taskid` in a list and then do 
```
l=[13930301,13930239,13930039,13920981,13920972,13920888,13920740,13920700,13920683,13920642,13920600,13920599]
for id in l:
    retry(id,newOpts={'newSite':True,'nFilesPerJob':1})
```
We need to take the `taskid` by copy paste from the website `bigpanda`. But thanks to `pandamonium` we can do it in one go
```
cat a.log | awk '$1 ~ /finished/ {print $2}' | panda-resub-taskid
cat a.log | awk '$1 ~ /failed/ {print $2}' | panda-resub-taskid
```
And you are done resubmitting the `finished` and `failed` jobs.

Other interesting commands are if we give a `task name`, which is for example `group.phys-higgs.data17_13TeV.00338263.CAOD_HIGG5D1.f877_m1885_p3372.31-10/` we can find out the input and output containers.

input:
```
pandamon -s IN group.phys-higgs.data17_13TeV.00338263.CAOD_HIGG5D1.f877_m1885_p3372.31-10/
```
obtaining
```
data17_13TeV.00338263.physics_Main.deriv.DAOD_HIGG5D1.f877_m1885_p3372
```

output:
```
pandamon -s OUT group.phys-higgs.data17_13TeV.00338263.CAOD_HIGG5D1.f877_m1885_p3372.31-10/
```
obtaining
```
group.phys-higgs.data17_13TeV.00338263.CAOD_HIGG5D1.f877_m1885_p3372.31-10_hist/
group.phys-higgs.data17_13TeV.00338263.CAOD_HIGG5D1.f877_m1885_p3372.31-10_CxAOD.root/
```

These commands can be very handy if we want to automate the process of resubmitting the `broken` jobs, which we still have to resubmit from the terminal by hand. But we can automate a bit part of the process. Let's find the input containers of the `broken` jobs.

Let's be in the sample list folder.
```
cd /data06/abuzatu/code/CxAODFramework_branch_master_21.2.27_8/source/FrameworkSub/In/VHbb
pandamon group.phys-higgs.*.CAOD_HIGG5D1.*.31-10 -d 40 --user adrian%20buzatu > a.log
less a.log | grep broken
```
Let's find the list of inputs that is broken.
```
for taskname in `cat a.log | awk '$1 ~ /broken/ {print $4}'`; do echo ${taskname}; pandamon -s IN ${taskname}; done | grep DAOD
```
You get the output like this
```
mc16_13TeV.410503.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_dil.deriv.DAOD_HIGG5D1.e5475_e5984_s3126_r10201_r10210_p3371
mc16_13TeV.410649.PowhegPythia8EvtGen_A14_Wt_DR_dilepton_antitop.deriv.DAOD_HIGG5D1.e6615_e5984_s3126_r10201_r10210_p3371
mc16_13TeV.364169.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV1000_E_CMS.deriv.DAOD_HIGG5D1.e5340_e5984_s3126_s3136_r10201_r10210_p3371
mc16_13TeV.364153.Sherpa_221_NNPDF30NNLO_Znunu_MAXHTPTV280_500_BFilter.deriv.DAOD_HIGG5D1.e5308_e5984_s3126_r10201_r10210_p3371
mc16_13TeV.364148.Sherpa_221_NNPDF30NNLO_Znunu_MAXHTPTV140_280_CVetoBVeto.deriv.DAOD_HIGG5D1.e5308_e5984_s3126_r10201_r10210_p3371
mc16_13TeV.364129.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV0_70_CFilterBVeto.deriv.DAOD_HIGG5D1.e5307_e5984_s3126_r10201_r10210_p3371
mc16_13TeV.345114.PowhegPythia8EvtGen_NNPDF3_AZNLO_ggZH125_vvcc.deriv.DAOD_HIGG5D1.e6042_e5984_s3126_r10201_r10210_p3374
```
The initial sample list was `list_sample_grid.13TeV_25ns.mcdata_d.HIGG5D1.txt`. So let's put these files into `list_sample_grid.13TeV_25ns.mcdata2_d.HIGG5D1.txt`. Notice the extra 2. We want now to split this list into `mcdata2_nominal` and `mcdata2_alternative`. For this we run the script
```
../../../FrameworkExe/scripts/splitSampleList.py mcdata2 d HIGG5D1
```
This produced the new files
```
-rw-r--r--. 1 abuzatu zp  884 May  2 23:35 list_sample_grid.13TeV_25ns.mcdata2_d.HIGG5D1.txt
-rw-r--r--. 1 abuzatu zp  763 May  2 23:36 list_sample_grid.13TeV_25ns.mcdata2_nominal_d.HIGG5D1.txt
-rw-r--r--. 1 abuzatu zp  121 May  2 23:36 list_sample_grid.13TeV_25ns.mcdata2_alternative_d.HIGG5D1.txt
```
And counting the number of lines things are consistent
```
-bash-4.1$ wc -l list_sample_grid.13TeV_25ns.mcdata2_d.HIGG5D1.txt
7 list_sample_grid.13TeV_25ns.mcdata2_d.HIGG5D1.txt
-bash-4.1$ wc -l list_sample_grid.13TeV_25ns.mcdata2_nominal_d.HIGG5D1.txt
6 list_sample_grid.13TeV_25ns.mcdata2_nominal_d.HIGG5D1.txt
-bash-4.1$ wc -l  list_sample_grid.13TeV_25ns.mcdata2_alternative_d.HIGG5D1.txt
1 list_sample_grid.13TeV_25ns.mcdata2_alternative_d.HIGG5D1.txt
```
We are now ready to submit with tage two of tag `31-10`, which means with vtag `31-10-2`. 

After creating new sample lists, we do not need to recompile.

```
cd /data06/abuzatu/code/CxAODFramework_branch_master_21.2.27_8
```

This folder is OK, as it is the same physics content as the tag `31-10`. The only changes were some scripts added.
We check the settings of the number of files per job
```
emacs -nw source/FrameworkExe/scripts/submitMaker.sh
```
We see
```
NRGBPERJOB="-1"
NRFILESPERJOB="1"
```
It is for nominal. So we first submit the `mcdata2_nominal`.
```
source source/FrameworkExe/scripts/submitMaker.sh 0L d VHbb mcdata2_nominal 31-10-2 Higgs 1 none
```
This leaves us in the `run` folder.
Then we can change the settings for the alternative.
```
emacs -nw source/FrameworkExe/scripts/submitMaker.sh
```
We change to 
```
NRGBPERJOB="600"                                                                                                                                                    NRFILESPERJOB="-1"
```
And we can submit for alternative too.
```
source source/FrameworkExe/scripts/submitMaker.sh 0L d VHbb mcdata2_alternative 31-10-2 Higgs 1 none
```
Now do not forget to update the output sample list for these `taskname` to replace `31-10` with `31-10-2`.
To help us, let's see first the tasks that we submitted. 
Looking from above in the terminal
```
-bash-4.1$ for taskname in `cat a.log | awk '$1 ~ /broken/ {print $4}'`; do echo ${taskname}; done
group.phys-higgs.mc16_13TeV.410503.CAOD_HIGG5D1.e5475_e5984_s3126_r10201_r10210_p3371.31-10/
group.phys-higgs.mc16_13TeV.410649.CAOD_HIGG5D1.e6615_e5984_s3126_r10201_r10210_p3371.31-10/
group.phys-higgs.mc16_13TeV.364169.CAOD_HIGG5D1.e5340_e5984_s3126_s3136_r10201_r10210_p3371.31-10/
group.phys-higgs.mc16_13TeV.364153.CAOD_HIGG5D1.e5308_e5984_s3126_r10201_r10210_p3371.31-10/
group.phys-higgs.mc16_13TeV.364148.CAOD_HIGG5D1.e5308_e5984_s3126_r10201_r10210_p3371.31-10/
group.phys-higgs.mc16_13TeV.364129.CAOD_HIGG5D1.e5307_e5984_s3126_r10201_r10210_p3371.31-10/
group.phys-higgs.mc16_13TeV.345114.CAOD_HIGG5D1.e6042_e5984_s3126_r10201_r10210_p3374.31-10/
```
Since they are only few, we can change by hand for now. In the future maybe we will find a more automatic way to do that.
```
cd /data06/abuzatu/data/CxAOD/180430/prepare_0L_d_31-10
emacs -nw out_sample_list_sample_grid.13TeV_25ns.mcdata_d.HIGG5D1.txt
```
# Get CxAOD from grid and prepare to run the Reader on them

In another terminal, go to the folder where you want to download. For me it is a local institute machine. For simplicity of making instructions easy to copy/paste, let's define in the terminal where we will download an evinorment variable with that path.
```
F=/data06/abuzatu/data/CxAOD/180428-31-08
cd $F
```
I will get in the same folder the `0L` from both `a` and `d` in one go. Each will have its own folder where the download will happen. 
From the first terminal, get the path to your CxAOD folder. For simplicity of making instructions easy to copy/paste, let's define in the terminal where we will download an evinorment variable with that path.
```
C=/data06/abuzatu/code/CxAODFramework_tag_r31-08_1
```
Let's run source the script. source is needed as it also sets up some environment variables.
```
source $C/source/FrameworkExe/scripts/getMaker.sh
```
It crashes in a controlled way, telling us the arguments we need. These are very similar to those of `submitMaker.sh`, to which we are now familiar, so we can quickly use them.
```
source /data06/abuzatu/code/CxAODFramework_tag_r31-08_1/source/FrameworkExe/scripts/getMaker.sh 0L a,d VHbb test Higgs 31-08 $C 1
```
You will be asked your grid proxy, but if are on the same machine as the submission, you will have a bit less than 4 days left, so you can click enter and skip this renewal of proxy. This script will do several steps for you automatically. You see that each DSID has its info read from AMI. That will be our reference to check if we have ran indeed the CxAOD on all the available samples. 

Two folders have been produced, as per our request `0L a,d`:
```
-bash-4.1$ ls
prepare_0L_a_31-08  prepare_0L_d_31-08
```
The commands below will be for one of them, but they are the same for the other and to gain time they can be done in parallel by creating a file `run.sh`, make it executable with `chmod +x run.sh` with a content like this
```
for folder in prepare_0L_a_31-08  prepare_0L_d_31-08  prepare_0L_d_31-08-avgMu  prepare_0L_d_31-09
do
    cd $folder
    ./rucio_get_jobs.py out*
    cd ..
done
```
This will loop over the several folder, and in each downloads. You can then repeat it later, by hand, several times, to download what has finished in the meantime. You can automate more by putting it in a loop, like this.

```
#!/bin/bash

STEMs="0L_a_31-08 0L_d_31-08 0L_d_31-08-avgMu 0L_d_31-09"
# run 10 times
for i in $(seq 1 1 10)
do
   echo "Running i=${i}"
   # download automatically for first step and before each other step
   if [[ i -gt 1 ]]; then
       command="sleep 600" # sleep 10 minutes, to let time for more samples to finish running
       echo ${command}
       eval ${command}
   fi
   # download in each folder one at a time what is new
   for folder in ${STEMs}
   do
       cd $folder
       ./rucio_get_jobs.py out*
       cd ..
   done # done for loop over folder stems
done # done for loop over iterations
```

This file is also stored in `FrameworkExe/scripts/runRepeatedlyRucioGetJobs.py`. You can modify it to personalize for your your needs.


```
cd prepare_0L_d_31-08
```
The script has collected several files for you in one place, for easy access to them.
```
-bash-4.1$ ls -1
checkYields.py
copy_CxAODs_to_eos.py
count_Nentry_SumOfWeight.py
dslist_NevtDxAOD.txt
out_sample_list_sample_grid.13TeV_25ns.test_d.HIGG5D1.txt
replicateToGrid.py
rucio_get_jobs.py
sample_info.txt
setupDownload.sh
```
The scripts `.sh` and `.py` are taken directly from `FrameworkExe/scripts`. The file `sample_info.txt` is taken from `FrameworkSub/data` and tells us the important information of which folder name (eos or local) should a given DSID be put in. That folder structure will then be taken by the scripts that read the Reader. 

Two files are produced by the script you just ran. The file `dslist_NevtDxAOD.txt` collects the number of events in the derivation that is available for each pair of `DSID` and AMI tag. Note that for `mc16c` and `mc16d` there can be two AMI tags per DSID, as there is a normal statistics sample and a statistical extension sample. For example for `test_d`
```
  00327342   10512176    f838_m1824_p3372                         physics_Main                                                 
  00327490    8523917    f838_m1824_p3372                         physics_Main                                                 
  00340453   13437911    f894_m1902_p3402                         physics_Main                                                 
    345053    1999950    e5706_e5984_s3126_r10201_r10210_p3374    PowhegPythia8EvtGen_NNPDF3_AZNLO_WmH125J_MINLO_lvbb_VpT      
    345054    3993700    e5706_e5984_s3126_r10201_r10210_p3374    PowhegPythia8EvtGen_NNPDF3_AZNLO_WpH125J_MINLO_lvbb_VpT      
    345055    2999950    e5706_e5984_s3126_r10201_r10210_p3374    PowhegPythia8EvtGen_NNPDF3_AZNLO_ZH125J_MINLO_llbb_VpT       
    345056    1999700    e5706_e5984_s3126_r10201_r10210_p3374    PowhegPythia8EvtGen_NNPDF3_AZNLO_ZH125J_MINLO_vvbb_VpT       
    345057     745000    e5706_e5984_s3126_r10201_r10210_p3374    PowhegPythia8EvtGen_NNPDF3_AZNLO_ggZH125_llbb                
    345058     499000    e6004_e5984_s3126_r10201_r10210_p3374    PowhegPythia8EvtGen_NNPDF3_AZNLO_ggZH125_vvbb                
    410470   41996793    e6337_e5984_s3126_r10201_r10210_p3371    PhPy8EG_A14_ttbar_hdamp258p75_nonallhad                      
    363489    5028233    e5525_e5984_s3126_r10201_r10210_p3371    Sherpa_221_NNPDF30NNLO_WlvZqq                                
    410646    2270972    e6552_e5984_s3126_r10201_r10210_p3371    PowhegPythia8EvtGen_A14_Wt_DR_inclusive_top                  
    410647    2265545    e6552_e5984_s3126_r10201_r10210_p3371    PowhegPythia8EvtGen_A14_Wt_DR_inclusive_antitop              
    410654    2191383    e6552_e5984_s3126_r10201_r10210_p3371    PowhegPythia8EvtGen_A14_Wt_DS_inclusive_top                  
    410655    2193423    e6552_e5984_s3126_r10201_r10210_p3371    PowhegPythia8EvtGen_A14_Wt_DS_inclusive_antitop  
```

The file name is the same for all the folders. The second file is the output file name and it differs from folder to folder. To use it in command line arguments to the scripts say `o` and press `tab`. 
The file contains the stem of the CxAOD containers on the grid, to be the same name, but in a different code, as the code in `hsg5framework.cxx`. The file looks like this.
```
group.phys-higgs.data17_13TeV.00327342.CAOD_HIGG5D1.f838_m1824_p3372.31-08
group.phys-higgs.data17_13TeV.00327490.CAOD_HIGG5D1.f838_m1824_p3372.31-08
group.phys-higgs.data17_13TeV.00340453.CAOD_HIGG5D1.f894_m1902_p3402.31-08
group.phys-higgs.mc16_13TeV.345053.CAOD_HIGG5D1.e5706_e5984_s3126_r10201_r10210_p3374.31-08
group.phys-higgs.mc16_13TeV.345054.CAOD_HIGG5D1.e5706_e5984_s3126_r10201_r10210_p3374.31-08
group.phys-higgs.mc16_13TeV.345055.CAOD_HIGG5D1.e5706_e5984_s3126_r10201_r10210_p3374.31-08
group.phys-higgs.mc16_13TeV.345056.CAOD_HIGG5D1.e5706_e5984_s3126_r10201_r10210_p3374.31-08
group.phys-higgs.mc16_13TeV.345057.CAOD_HIGG5D1.e5706_e5984_s3126_r10201_r10210_p3374.31-08
group.phys-higgs.mc16_13TeV.345058.CAOD_HIGG5D1.e6004_e5984_s3126_r10201_r10210_p3374.31-08
group.phys-higgs.mc16_13TeV.410470.CAOD_HIGG5D1.e6337_e5984_s3126_r10201_r10210_p3371.31-08
group.phys-higgs.mc16_13TeV.363489.CAOD_HIGG5D1.e5525_e5984_s3126_r10201_r10210_p3371.31-08
group.phys-higgs.mc16_13TeV.410646.CAOD_HIGG5D1.e6552_e5984_s3126_r10201_r10210_p3371.31-08
group.phys-higgs.mc16_13TeV.410647.CAOD_HIGG5D1.e6552_e5984_s3126_r10201_r10210_p3371.31-08
group.phys-higgs.mc16_13TeV.410654.CAOD_HIGG5D1.e6552_e5984_s3126_r10201_r10210_p3371.31-08
group.phys-higgs.mc16_13TeV.410655.CAOD_HIGG5D1.e6552_e5984_s3126_r10201_r10210_p3371.31-08
```
These are only the stem, as there are three types of containers available when adding the following suffixes: `_CxAOD.root` (containing the actual CxAOD we want), `_hist` (containing the cut flow histograms, very useful for comparing later productions to this one), and `_log` (the log files, which we do not care too much about, as they are too big anyways).

We can now start downloading from the grid. rucio will already be set up by the script `getMaker.sh` that prepared the folders, like `prepare_0L_d`. If we come back in a fresh terminal, we already have there `setupDownload.sh`, that we can source to have all the environment ready again.

```
source setupDevelopment.sh
```

We start downloading with 
```
./rucio_get_jobs.py o [tab]
```
Note the tab, so that it picks up the correct output file name in each folder. You will see green color text, a sign that the download is progressing well. If you see red errors, read them carefully. It may be that `rucio` was not set up, or you do not have a grid proxy valid. It also does not work if you setup rucio in the same terminal where you built your CxAOD folder.

After finishing downloading, and already included in the same script, a count of events or yields is done on the files that were downloaded. You can also do it at any time. 

```
./count_Nentry_SumOfWeight.py 1 0 0
```
It produces the file `./yields.13TeV_DxAOD_sorted.txt`, which, at first, when only  few lines have been downloaded, looks like this
```
  00327342    2939292     545528  2939292.000000 f838_m1824_p3372              
    410654     489000     170834 17996664.560427 e6552_e5984_s3126_r10201_r10210_p3371
    410655    5917000    2077722 221975030.761511 e6552_e5984_s3126_r10201_r10210_p3371
```
The first column is the DSID (note the extra `00` for data). The last is the AMITag. A sample is identified uniquely by the pair of DSID and AMITag. We want to check that we have downloaded CxAOD files produced with all the available derivation files. For this we run a script `checkYields.py` which will take as input internally two files: `dslist_NevtDxAOD.txt` and `yields.13TeV_DxAOD_sorted.txt`. Just run. 

```
./checkYields.py
```

You will get an output that will say for which datasets you have not a single event so far, what fraction of the total events expected for all the samples you have downloaded, and a text file that gives for each pair of `DSID` vs `AMITag` the fraction you have, including `0.000` if you have nothing and `1.000` if you have it all.

```
-bash-4.1$ ./checkYields.py 
Input file (yields from CxAODs): yields.13TeV_DxAOD_sorted.txt
Input file (yields from DxAODs): dslist_NevtDxAOD.txt
Running ...
No match found for dataset  00327490 f838_m1824_p3372
No match found for dataset  00340453 f894_m1902_p3402
No match found for dataset  345053 e5706_e5984_s3126_r10201_r10210_p3374
No match found for dataset  345054 e5706_e5984_s3126_r10201_r10210_p3374
No match found for dataset  345055 e5706_e5984_s3126_r10201_r10210_p3374
No match found for dataset  345056 e5706_e5984_s3126_r10201_r10210_p3374
No match found for dataset  345057 e5706_e5984_s3126_r10201_r10210_p3374
No match found for dataset  345058 e6004_e5984_s3126_r10201_r10210_p3374
No match found for dataset  410470 e6337_e5984_s3126_r10201_r10210_p3371
No match found for dataset  363489 e5525_e5984_s3126_r10201_r10210_p3371
No match found for dataset  410646 e6552_e5984_s3126_r10201_r10210_p3371
No match found for dataset  410647 e6552_e5984_s3126_r10201_r10210_p3371
Total CxAOD =  2794084.0
Total DxAOD =  100657653.0
Ratio       =  0.0277582867941
Yield ratios written to: dslist_NevtDxAOD_yield.txt
```
Looking inside the output file `dslist_NevtDxAOD_yield.txt`, it looks like this.
```
00327342   0.052    f838_m1824_p3372                                             physics_Main                                                
00327490   0.000    f838_m1824_p3372                                             physics_Main                                                
00340453   0.000    f894_m1902_p3402                                             physics_Main                                                
345053     0.000    e5706_e5984_s3126_r10201_r10210_p3374                        PowhegPythia8EvtGen_NNPDF3_AZNLO_WmH125J_MINLO_lvbb_VpT     
345054     0.000    e5706_e5984_s3126_r10201_r10210_p3374                        PowhegPythia8EvtGen_NNPDF3_AZNLO_WpH125J_MINLO_lvbb_VpT     
345055     0.000    e5706_e5984_s3126_r10201_r10210_p3374                        PowhegPythia8EvtGen_NNPDF3_AZNLO_ZH125J_MINLO_llbb_VpT      
345056     0.000    e5706_e5984_s3126_r10201_r10210_p3374                        PowhegPythia8EvtGen_NNPDF3_AZNLO_ZH125J_MINLO_vvbb_VpT      
345057     0.000    e5706_e5984_s3126_r10201_r10210_p3374                        PowhegPythia8EvtGen_NNPDF3_AZNLO_ggZH125_llbb               
345058     0.000    e6004_e5984_s3126_r10201_r10210_p3374                        PowhegPythia8EvtGen_NNPDF3_AZNLO_ggZH125_vvbb               
410470     0.000    e6337_e5984_s3126_r10201_r10210_p3371                        PhPy8EG_A14_ttbar_hdamp258p75_nonallhad                     
363489     0.000    e5525_e5984_s3126_r10201_r10210_p3371                        Sherpa_221_NNPDF30NNLO_WlvZqq                               
410646     0.000    e6552_e5984_s3126_r10201_r10210_p3371                        PowhegPythia8EvtGen_A14_Wt_DR_inclusive_top                 
410647     0.000    e6552_e5984_s3126_r10201_r10210_p3371                        PowhegPythia8EvtGen_A14_Wt_DR_inclusive_antitop             
410654     0.078    e6552_e5984_s3126_r10201_r10210_p3371                        PowhegPythia8EvtGen_A14_Wt_DS_inclusive_top                 
410655     0.947    e6552_e5984_s3126_r10201_r10210_p3371                        PowhegPythia8EvtGen_A14_Wt_DS_inclusive_antitop   
```

You repeat the procedure to download from the grid the files that have finished in the meantime. The download script will already count the yields for you, so that you type less, so you can always check what is the status. You can do the download in a loop over the first day of production for example so that samples are locally very soon after they have finished. The rucio script is smart enough to not download files that you already have. And the script will only attempt to download with rucio those samples that are not yet at 100%.

When you reach 100%, but for MC you can also without having 100%, as the yield will be correctly calculated (just the plot will be spiky and the statistical error will be large), you compute one more time the yields, but this time with a different configuration, to be run for the Reader. In addition, the various AMI tags for a given DSID are now added up together. There is one number per DSID that the Reader will use.

```
./count_Nentry_SumOfWeight.py 0 0 1
```
This will produce the file `./yields.13TeV_sorted.txt`, which at first, when only a few files have been downloaded, looks like this
```
  00327342   14992887      46958 14992887.000000
  00327490    2005852       6069  2005852.000000
  00340453   10276960      48182 10276960.000000
    410654    4809000      72726 176797768.284422
    410655    6247000      96757 234352806.618860
```
The first column is the DSID (notice the extra 00 for data events). 

Assuming all is all right now, we move the next step, of creating the folder structure that the Reader can use. For this we use the script to copy to eos. The script can either copy to eos, but it can in fact copy anywhere, including on your same machine. For the latter case, no need to actually copy, and occupy double the disk space. Instead, you can change one command-line argument and make only symbolic links to the files we already have. The folder structure will create the folders only for the samples that you have locally, taking the names from the file `sample_info.txt`. 

Let's imagine I want to copy into destination
```
D=/data06/abuzatu/data/CxAOD/180310All
```
It is a place where I can put different CxAOD tags, so that when I run the Reader I can easily from one command run on one tag, the other, or several. This structure is not followed up in eos, but it could. 
```
ls $D
HIGG2D4_13TeV  HIGG5D1_13TeV  HIGG5D2_13TeV
ls $D/HIGG5D2_13TeV
CxAOD_31-01_a
CxAOD_31-01_d
CxAOD_31-01-v2_c
```

First to copy the files locally, from inside our `prepare_0L_d` folder we run.
```
./copy_CxAODs_to_eos.py 
```
There will be a controlled crash, telling what command line arguments are needed. To create symbolic links locally use
```
 ./copy_CxAODs_to_eos.py 0 1 1 1 HIGG5D1    31-08_d /data06/abuzatu/data/CxAOD/180310All
 ```
 Since there are only symbolic links, it finishes fast. Going there. 
```
cd /data06/abuzatu/data/CxAOD/180310All/HIGG5D1_13TeV/CxAOD_31-08_d
ls -1
checkYields.py
count_Nentry_SumOfWeight.py
data17
dslist_NevtDxAOD.txt
dslist_NevtDxAOD_yield.txt
out_sample_list_sample_grid.13TeV_25ns.test_d.HIGG5D1.txt
stopWt_PwPy8_DS
yields.13TeV_DxAOD_sorted.txt
yields.13TeV_sorted.txt
```
The structure is made from the folders that belong to the samples, here `data17` and `stopWt_PwPy8_DS`. It also copies the scripts used in the `prepare_0L_d` folder, so that you can count again the yields to compare with AMI, to make sure nothing was lost. And then you can compute again the yields for the Reader.

```
./count_Nentry_SumOfWeight.py 1 0 0
./checkYields.py
./count_Nentry_SumOfWeight.py 0 0 1
```

You can do the same procedure for eos.
```
 ./copy_CxAODs_to_eos.py 0 1 1 0 HIGG5D1    31-08_d /eos/atlas/atlascerngroupdisk/phys-exotics/CxAOD/CxAOD_31/
 ```
 Note the fourth argument is now `0`, meaning no symbolic link, but a real copy to eos. The script is again smart enough to not copy the files that have already been copied. So you should start to copy early and you can do it often. That way you have in eos relatively fast after the production finished.
 
When you think you trust your samples, you can replicate them to the grid. This must be done before the 2 weeks since the jobs finished, as after that they are not guaranteed to stay on the grid. For this you use the script `replicateToGrid.py`, initially found in `FrameworkExe/scripts`, but which is found in our `prepare_0L_d_31-08` already.

```
./replicateToGrid.py
```

It will tell us what arguments it needs. You must have production role (we have not confirmed with Exotics we can store in their grid area), 
```
./replicateToGrid.py physgroup=phys-higgs out_*
```
Be careful, the script will only replicate those datasets that are at 100% in agreememt with AMI. If you really trust your sample (like you know it will never go from 98% done to 100%) you have to hack the script, or do it by hand. The script has at the top the instructions on how to do for one sample. You can then do by hand for all samples, or you can make a short script to loop over all your samples.


# How to update to a new Analysis Base

Here is the work flow and the copy/paste instructions on how to update to a new AnalysisBase (AB) release.

Go to the code folder and run the usual script, but with the last argument to `0` instead of `1`. It means we do not want to set up the AB and compile automatically. Instead we will edit the AB version by hand, then set up and compile by hand. Then we test locally that on several samples the code runs fine. When all fine, we make a MR. 

Check out the master and chooose a folder name appropriate for the new AB tag (38).
```
source /afs/cern.ch/user/a/abuzatu/work/public/BuzatuAll/BuzatuATLAS/CxAODFramework/getMaster.sh origin/master          CxAODFramework_branch_master_21.2.38_1    1           0
```

Edit by hand the AB version and update `21.2.37` to `21.2.38`.
```
cd FrameworkSub/bootstrap/
ls
emacs -nw release.txt
```

Go to the build folder and set up this AB, then compile and set up.
```
cd ../../../build/
setupATLAS
lsetup asetup
release=`cat ../source/FrameworkSub/bootstrap/release.txt`
echo "release=$release"
asetup AnalysisBase,$release,here
cp CMakeLists.txt ../source
cmake ../source
cmake --build .
source x86_64-slc6-gcc62-opt/setup.sh
```

Now test by running this new code on many derivations of different channels. Basically do what the pipeline does, but locally. This is the usual recommendation. Do not rely on the pipeline to test for you, test yourself first. Note, this script may be updated in the future to use the new `submitMaker.sh` script.

```
cd ../run
source ../source/FrameworkExe/scripts/testLocallyAsInCIPipeline.sh
```

You see jobs submitted in parallel and the following folders appearing: `config`, `log` and `submitDir`. We look into log with `tail -f in a few of the files`. But `bash` will also show us some list when things finished correctly, or if they crashed, when doing `cd log` and then `ls`. We see most work, but some crash with aborded with core dump. We investigate what those are. 

```
2]   Done                    `echo ${COMMAND}` &>${logFile}  (wd: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run)
(wd now: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run/log)
[3]   Aborted                 (core dumped) `echo ${COMMAND}` &>${logFile}  (wd: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run)
(wd now: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run/log)
[4]   Done                    `echo ${COMMAND}` &>${logFile}  (wd: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run)
(wd now: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run/log)
[10]   Done                    `echo ${COMMAND}` &>${logFile}  (wd: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run)
(wd now: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run/log)
[13]   Aborted                 (core dumped) `echo ${COMMAND}` &>${logFile}  (wd: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run)
(wd now: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run/log)
[23]   Aborted                 (core dumped) `echo ${COMMAND}` &>${logFile}  (wd: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run)
(wd now: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run/log)
[31]   Done                    `echo ${COMMAND}` &>${logFile}  (wd: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run)
(wd now: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run/log)
[32]   Done                    `echo ${COMMAND}` &>${logFile}  (wd: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run)
(wd now: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run/log)
[33]-  Done                    `echo ${COMMAND}` &>${logFile}  (wd: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run)
(wd now: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run/log)
[34]+  Done                    `echo ${COMMAND}` &>${logFile}  (wd: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run)
(wd now: /data06/abuzatu/code/CxAODFramework_branch_master_21.2.38_1/run/log)
```

Those that crash at the moment are those for period of high pileup, so I will remove them now from our checks, for example. 
```
tail -f 2L_mc16hp_ZHllbb.log
```

So all that should be good are good, we can move ahead and make a MR. 
```
cd ../../source/FrameworkSub/bootstrap/
git status
git diff release.txt
git add release.txt
git commit -m "Bump to AB 21.2.38"
git branch
git branch -m "master-abuzatu-bump-to-AB-21-2-38"
git branch
git log
git push -u origin master-abuzatu-bump-to-AB-21-2-38
```

This shows us
```
remote: To create a merge request for master-abuzatu-bump-to-AB-21-2-38, visit:
remote:   https://gitlab.cern.ch/CxAODFramework/FrameworkSub/merge_requests/new?merge_request%5Bsource_branch%5D=master-abuzatu-bump-to-AB-21-2-38
remote: 
To ssh://gitlab.cern.ch:7999/CxAODFramework/FrameworkSub.git
 * [new branch]      master-abuzatu-bump-to-AB-21-2-38 -> master-abuzatu-bump-to-AB-21-2-38
Branch master-abuzatu-bump-to-AB-21-2-38 set up to track remote branch master-abuzatu-bump-to-AB-21-2-38 from origin.
```

We follow the url and follow up the info according to the template. We choose a very concice and clear title, description, choose an assigne (Adrian), choose the label of updating the AnalysisBase release. A pipeline is starting automatically, which will only check that our code compiles. We we put this in the browser.

```
https://gitlab.cern.ch/CxAODFramework/FrameworkSub/merge_requests/new?merge_request%5Bsource_branch%5D=master-abuzatu-bump-to-AB-21-2-38
```

From `Choose a template` choose `Feature`.

At `Contributing`, remove the text and say `Bump to AB 21.2.38` and show the `url` to the [twiki](https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AnalysisBaseReleaseNotes21_2#21_2_38_built_on_2018_07_18) for this, to have easily the changes with respect to the previous one. Remove the sections (title and content) of `## Mere Request Description`, `# WIP`, `# Merge Requests Procedure`, as they are info for you, but not helpful for the person reviewing your MR (Adrian). At the section of `# Checklist Before Requesting Approver` tick the first three boxes, but do not tick the forth one now (but only after the pipeline actually completes successfully), and the fourth one only after you finished the work and you removed the `WIP`. For a quick merge request (MR) like this you do not need to put initially `[WIP]` at the beginning of the title, but for longer ones it is a good practice to do so. So that others do not merge it by mistake. 

At `Assignee` choose `Adrian Buzatu`. At `Label` choose `Update Analysis Base Release`. Tick the two boxes `Remove source branch when merge request is accepted. ` and `Squash commits when merge request is accepted.` Click on `Changes` to see what is actually changed in the code. If all makes sense, click on `Submit merge request`. Follow the progress of the pipeline (you should receive an email also if it fails or succeeds). If it fails, follow the error and make new commits to fix it. Once it succeeds, write a comment in the `Discussion` section tagging Adrian directly with `@abuzatu, ready to merge`, so that he receives an email. 

Once merged, please add it to the [Twiki](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/Release21Migration) to add by hand this commit to the list there. Search on it for `List of Merge requests` and click on it and you get at the list of MR, which is at this [direct url](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/Release21Migration#List_of_Merge_requests). Click on top right at the `Edit` of the twiki, search for `List of Merge requests` and add a new line there following the previous examples, for example 

```
---++ List of Merge requests
   * [2018-07-26] [[https://gitlab.cern.ch/CxAODFramework/CxAODTools/merge_requests/225#note_1743993][MR1]] CxAODTools, CxAODMaker: save more info 1L pTW ttbar reweighting in the Maker to simplify the Reader code
   * [2018-07-26] [[https://gitlab.cern.ch/CxAODFramework/FrameworkExe/merge_requests/394/diffs][MR]] FrameworkExe: update scripts that get AOD and PRW for our DAOD samples
   * [2018-07-26] [[https://gitlab.cern.ch/CxAODFramework/CxAODTools/commit/b5fbda8614476e748d17baaf6e1dc917b4dcd4e5][Commit]] CxAODTools: Update BTaggingTool.cxx - allow either of the June CDI (24 and 29) to compare the two 
```

That is it!

# Check the eos quota of Higgs and Exotics groups

See the details in this [GitLab issue](https://gitlab.cern.ch/CxAODFramework/FrameworkSub/issues/32)