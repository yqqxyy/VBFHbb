#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "SampleHandler/DiskListLocal.h"
#include "SampleHandler/DiskListEOS.h"
#include "EventLoop/Job.h"

#include "EventLoop/LSFDriver.h"
#include "EventLoop/DirectDriver.h"
#include "EventLoop/CondorDriver.h"
#include "EventLoop/Driver.h"

#include "SampleHandler/Sample.h"

#include <stdlib.h>
#include <vector>

#include <TSystem.h>
#include <TFile.h>
#include <TPython.h>

#include "CxAODTools/ConfigStore.h"

#include "CxAODReader_VBFHbb/AnalysisReader_VBFHbb.h"

// TO RUN ON LYON BATCH
#include "SampleHandler/ToolsSplit.h"
#include "EventLoop/GEDriver.h"

void tag(SH::SampleHandler& sh, const std::string& tag);

int main(int argc, char* argv[]) {

  // read run config
  std::string configPath = "data/FrameworkExe_VBFHbb/framework-read.cfg";
  static ConfigStore* config = ConfigStore::createStore(configPath);
  // Take the submit directory from the input if provided:
  std::string submitDir = "submitDir";
  config->getif<std::string>("submitDir",submitDir);


  if (argc > 1) submitDir = argv[1];
  if (argc > 2) configPath = argv[2];

  // enable failure on unchecked status codes
  bool enableFailure = false;
  config->getif<bool>("failUncheckedStatusCodes", enableFailure);
  if (enableFailure) {
    xAOD::TReturnCode::enableFailure();
    StatusCode::enableFailure();
    CP::CorrectionCode::enableFailure();
    CP::SystematicCode::enableFailure();
  }

  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // Construct the samples to run on:
  std::vector<std::string> sample_names = {"data",
                                        "ZeeB", "ZeeC", "ZeeL",
  					"ZmumuB", "ZmumuC", "ZmumuL",
  					"ZtautauB", "ZtautauC", "ZtautauL",
 	 				"ZnunuB", "ZnunuC", "ZnunuL",
  					"WenuB", "WenuC", "WenuL",
  					"WmunuB", "WmunuC", "WmunuL",
  					"WtaunuB", "WtaunuC", "WtaunuL",
  					"ttbar", "singletop_t", "singletop_s", "singletop_Wt",
                                        "ttbar_PwHerwigppEG", "ttbar_PwPyEG", "ttbar_aMcAtNloHerwigppEG",
  					"ZHvv125", "WH125", "ZHll125", "ZHll125J_MINLO",
  					"bbA", "ggA", "HVT", "HbbjjaSM125", "HbbjjaHEFT125", "NonResbbjja", "ZbbjjaEWK", "ZbbjjaQCD",
					"Wenu_MG", "Wmunu_MG", "Wtaunu_MG",
					"Wenu_Pw", "Wmunu_Pw", "Wtaunu_Pw",
					"Zee_MG", "Zmumu_MG", "Ztautau_MG", "Znunu_MG",
					"Zee_Pw", "Zmumu_Pw", "Ztautau_Pw", "Znunu_Pw",
					"WW", "WZ", "ZZ",
					"ZeeL_v22", "ZeeC_v22", "ZeeB_v22",
					"ZmumuL_v22", "ZmumuC_v22", "ZmumuB_v22",
                                        "HVT_WH",
                                        "HVT_0500", "HVT_0600", "HVT_0700", "HVT_0800", "HVT_0900", "HVT_1000",
                                        "HVT_1100", "HVT_1200", "HVT_1300", "HVT_1400", "HVT_1500", "HVT_1600",
                                        "HVT_1700", "HVT_1800", "HVT_1900", "HVT_2000", "HVT_2200", "HVT_2400",
                                        "HVT_2600", "HVT_2800", "HVT_3000", "HVT_3500", "HVT_4000", "HVT_4500", "HVT_5000",
                                        "bbA_1000", "bbA_1100", "bbA_1200", "bbA_1300", 
                                        "bbA_1400", "bbA_1500", "bbA_1600", "bbA_1700", "bbA_1800", "bbA_1900", "bbA_200", "bbA_2000", 
                                        "bbA_280", "bbA_300", "bbA_320", "bbA_340", "bbA_360", "bbA_380", "bbA_400", 
                                        "bbA_420", "bbA_440", "bbA_460", "bbA_480", "bbA_500", "bbA_550", "bbA_600", 
                                        "bbA_650", "bbA_700", "bbA_750", "bbA_800", "bbA_850", "bbA_900", "bbA_950",
                                        "ggA_1000", "ggA_1100", "ggA_1200", "ggA_1300", 
                                        "ggA_1400", "ggA_1500", "ggA_1600", "ggA_1700", "ggA_1800", "ggA_1900", "ggA_200", "ggA_2000", 
                                        "ggA_280", "ggA_300", "ggA_320", "ggA_340", "ggA_360", "ggA_380", "ggA_400", 
                                        "ggA_420", "ggA_440", "ggA_460", "ggA_480", "ggA_500", "ggA_550", "ggA_600", 
                                        "ggA_650", "ggA_700", "ggA_750", "ggA_800", "ggA_850", "ggA_900", "ggA_950"
                                        };

  // Override sample list from config
   config->getif<std::vector<std::string> >("samples", sample_names);

  // Possibility to skip some of the samples above
  // Read bools from config file and if false remove sample from vector
  std::vector<std::string>::iterator itr;
  for (itr = sample_names.begin(); itr != sample_names.end();) {
    bool includeSample = true;
    config->getif<bool>(*itr, includeSample);
    if ( !includeSample ) itr = sample_names.erase(itr);
    else ++itr;
  }

  std::string dataset_dir = config->get<std::string>("dataset_dir");

  bool eos;
  std::string prefix = "/eos/";
  if ( dataset_dir.substr(0, prefix.size()).compare(prefix) == 0 ) {
    eos = true;
    std::cout << "Will read datasets from EOS directory " << dataset_dir << std::endl;
  }
  else {
    eos = false;
    std::cout << "Will read datasets from local directory " << dataset_dir << std::endl;
  }

  // Query - I had to put each background in a separate directory
  // for samplehandler to have sensible sample names etc.
  // Is it possible to define a search string for directories and assign all those to a sample name?
  // SH::scanDir (sampleHandler, list); // apparently it will come

  // create the sample handler
  SH::SampleHandler sampleHandler;
  for (unsigned int isamp(0) ; isamp<sample_names.size() ; isamp++) {
    std::string sample_name(sample_names.at(isamp));
    std::string sample_dir(dataset_dir+"/"+sample_name);

    if (!eos) {
      bool direxists=gSystem->OpenDirectory (sample_dir.c_str());
      if (!direxists) {
	std::cout << " No sample exists: " << sample_name << " , skipping: "  << sample_dir << std::endl;
	continue;
      }
    }

    std::string inFiles = "";
    config->getif<std::string>("inFiles", inFiles);

    // eos, local disk or afs
    if (eos) {
      //SH::DiskListEOS list(sample_dir,"root://eosatlas/"+sample_dir );
      SH::DiskListEOS list(sample_dir,"root://eosatlas.cern.ch:/"+sample_dir );
      //SH::DiskListEOS list("eosatlas.cern.ch", sample_dir);
      // tuples are downloaded to same directory as CxAOD so specify CxAOD root file pattern
      if ( inFiles.empty() ) SH::scanSingleDir (sampleHandler, sample_name, list, "*CxAOD*") ;
      else SH::scanSingleDir (sampleHandler, sample_name, list, inFiles) ;
      // SH::scanSingleDir (sampleHandler, sample_name, list, "*outputLabel*") ;
    } else {
      SH::DiskListLocal list(sample_dir);
      // tuples are downloaded to same directory as CxAOD so specify CxAOD root file pattern
      if ( inFiles.empty() ) SH::scanSingleDir (sampleHandler, sample_name, list, "*CxAOD*") ;
      else SH::scanSingleDir (sampleHandler, sample_name, list, inFiles) ;
      // SH::scanSingleDir (sampleHandler, sample_name, list, "*outputLabel*") ;
    }

    //
    SH::Sample* sample_ptr = sampleHandler.get(sample_name);
    sample_ptr->setMetaString("SampleID", sample_name);
    int nsampleFiles=sample_ptr->numFiles();

    std::cout<<"Sample name "<<sample_name<<" with nfiles : "<<nsampleFiles<<std::endl;
  }


  // Set the name of the input TTree. It's always "CollectionTree"
  // for xAOD files.
  sampleHandler.setMetaString("nc_tree", "CollectionTree");

  // Print what we found:
  sampleHandler.print();
  
  // generate yield file from input CxAODs
  std::string yieldFile = "";
  bool generateYieldFile = false;
  config->getif<bool>("generateYieldFile", generateYieldFile);
  if (generateYieldFile) {
    // write file list to temporary text file
    std::ofstream file;
    file.open("fileList_temp.txt");
    SH::SampleHandler::iterator it = sampleHandler.begin();
    SH::SampleHandler::iterator it_end = sampleHandler.end();
    for (; it != it_end; it++) {
      SH::Sample* sample = *it;
      for (unsigned int i = 0; i < sample->numFiles(); i++) {
	file << sample->fileName(i) << std::endl;
      }
    }
    file.close();
    // sort file list and generate yield file
    system("sort fileList_temp.txt > fileList.txt");
    TPython::Exec("import sys");
    TPython::Exec("sys.argv=['dummy','fileList.txt']");
    TPython::LoadMacro("./FrameworkExe_VBFHbb/scripts/count_Nentry_SumOfWeight.py");
    yieldFile = (const char*) TPython::Eval("out_file_md5");
    system("rm fileList_temp.txt fileList.txt");
  }

  // Create an EventLoop job:
  EL::Job job;
  job.sampleHandler(sampleHandler);

  // remove submit dir before running the job
  //job.options()->setDouble(EL::Job::optRemoveSubmitDir, 1);

  // create algorithm, set job options, maniplate members and add our analysis to the job:
  AnalysisReader* algorithm;
  std::string modelType    = config->get<std::string>("modelType");
  std::string analysisType = config->get<std::string>("analysisType");
  algorithm = new AnalysisReader_VBFHbb();
  algorithm->setConfig(config);
  algorithm->setSumOfWeightsFile(yieldFile);

  //limit number of events to maxEvents - set in config
  job.options()->setDouble (EL::Job::optMaxEvents, config->get<int>("maxEvents"));

  // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/EventLoop#Access_the_Data_Through_xAOD_EDM
  bool nominalOnly = false;
  config->getif<bool>("nominalOnly", nominalOnly);
  if (nominalOnly) {
    // branch access shows better performance for CxAODReader with Nominal only
    job.options()->setString (EL::Job::optXaodAccessMode, EL::Job::optXaodAccessMode_branch);
  } else {
    // branch access cannot read shallow copies, so we need class access in case reading systematics
    job.options()->setString (EL::Job::optXaodAccessMode, EL::Job::optXaodAccessMode_class);
  }
  

  // add algorithm to job
  job.algsAdd(algorithm);

  // Number of files to submit per job
  int nFilesPerJob = 20;
  //config->getif<int>("nFilesPerJob", nFilesPerJob);

  // Run the job using the local/direct driver:
  std::string driver = "direct";
  config->getif<std::string>("driver",driver);
  if (driver=="direct"){
      EL::DirectDriver*  eldriver = new EL::DirectDriver;
      eldriver->submit(job, submitDir);
  } else if (driver=="LSF"){
      EL::LSFDriver* eldriver = new EL::LSFDriver;
      eldriver->options()->setString (EL::Job::optSubmitFlags, "-L /bin/bash");
      eldriver->shellInit = "export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase && source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh";

      job.options()->setDouble (EL::Job::optFilesPerWorker, nFilesPerJob);
      std::string bQueue = "1nh";
      config->getif<std::string>("bQueue",bQueue);
      job.options()->setString (EL::Job::optSubmitFlags, ("-q "+bQueue).c_str()); //1nh 8nm
      eldriver->submitOnly(job, submitDir);
  } else if (driver == "batch"){
    EL::LSFDriver* eldriver = new EL::LSFDriver;
    std::string queue = "atlas-t3 -W 80:00";
    config->getif<std::string>("queue", queue);
    Info("vbfhbbframeworkReader","Submitting to queue %s",queue.c_str());
    std::string submitFlags = " -o out%J.txt";
    //submitFlags += " -R rusage[scratch=10.2]";
    //submitFlags += "";
    if (queue != "short")
      submitFlags += " -q " + queue + " -W 80:00";
    else
      submitFlags += " -q " + queue;
    job.options()->setString (EL::Job::optSubmitFlags, submitFlags);
    job.options()->setBool (EL::Job::optResetShell, false);

    double nFilesPerJob = -1.;
    config->getif<double>("nFilesPerJob",nFilesPerJob);
    if (nFilesPerJob > 0) job.options()->setDouble(EL::Job::optFilesPerWorker, nFilesPerJob);
    eldriver->submitOnly(job, submitDir);
 } else if ( driver == "condor" ) { // Spyros: add condor driver
    EL::CondorDriver* eldriver = new EL::CondorDriver;
    eldriver->shellInit = "export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase && source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh";
    job.options()->setDouble (EL::Job::optFilesPerWorker, nFilesPerJob);
    job.options()->setString (EL::Job::optCondorConf, "accounting_group = group_atlas.uiowa");
    eldriver->submitOnly(job, submitDir);
  } else if (driver=="GE") { // grid engine - Camilla (lyon batch), Felix (MPI batch)
      std::vector<std::string> vec_submit_flags = config->get<std::vector<std::string> >("submitFlags");
      std::string submit_flags("");
      for (auto str : vec_submit_flags) submit_flags += str + " ";
      EL::GEDriver *eldriver = new EL::GEDriver;
      job.options()->setDouble (EL::Job::optFilesPerWorker, nFilesPerJob);
      job.options()->setString(EL::Job::optSubmitFlags, submit_flags);
      eldriver->submitOnly(job, submitDir);
  } else {
    Error("vbfhbbframework", "Unknown driver '%s'", driver.c_str());
    return 0;
  }
  return 0;
}
