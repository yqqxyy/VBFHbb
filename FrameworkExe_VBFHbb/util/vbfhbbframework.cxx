#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "SampleHandler/DiskListLocal.h"
#include "SampleHandler/DiskListXRD.h"
#include "SampleHandler/ScanDir.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#include "EventLoop/LSFDriver.h"
#include "EventLoopGrid/PrunDriver.h"

#include <iostream>
#include <fstream>
#include <TSystem.h> 
#include <stdlib.h> 
#include <iomanip>

#include "CxAODMaker_VBFHbb/AnalysisBase_VBFHbb.h"
#include "CxAODTools/ConfigStore.h"

#include "AsgTools/StatusCode.h"
#include "EventLoop/StatusCode.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/SystematicCode.h"

#include <getopt.h>
#include <boost/lexical_cast.hpp>

void printUsage(std::ostream & os) {
	os << "Usage: " << std::endl;
	os << "vbfhbbframework [ options ]" << std::endl;
	os << "   Options: " << std::endl;
	os << "    " << std::setw(24) << "-h | --help : print this text" << std::endl;
	os << "    " << std::setw(24) << "-s | --sampleIn <path> : override the sampleIn value from the config file"  << std::endl;
	os << "    " << std::setw(24) << "-d | --submitDir <path> : override the submitDir value from the config file" << std::endl;
	os << "    " << std::setw(24) << "-n | --numEvents <int> : override the maxEvents value from the config file" << std::endl;
	os << "    " << std::setw(24) << "-c | --config <file> : override the config file" << std::endl;
	os << "    " << std::setw(24) << "-o | --official : configure for official production" << std::endl;
	os << "    " << std::setw(24) << "-x | --noSubmit : turn off grid submission for testing before launching grid jobs" << std::endl;
}

void replaceAll( std::string &s, const std::string &search, const std::string &replace ) {
    for( size_t pos = 0; ; pos += replace.length() ) {
        pos = s.find( search, pos );
        if( pos == std::string::npos ) break;
        s.erase( pos, search.length() );
        s.insert( pos, replace );
    }
}

int main(int argc, char* argv[]) {
  // flags for command line overrides
  bool overrideInput = false;
  bool overrideSubmitDir = false;
  bool overrideEvents = false;
  bool overrideConfig = false;
  bool officialProduction = false;
  bool overrideSubmit = false;
  // command line override values
  std::string sampleIn_override="";
  std::string submitDir_override="";
  std::string configFile_override="";
  int nEvents_override = -1;
  
  static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"config",    required_argument, 0, 'c'},
    {"sampleIn",  required_argument, 0, 's'},
    {"submitDir", required_argument, 0, 'd'},
    {"numEvents", required_argument, 0, 'n'},
    {"official",  no_argument, 0, 'o'},
    {"noSubmit", no_argument, 0, 'x'},
    {0,0,0,0}
  };

  int option_index =0;
  int c = 0;
  while ( (c = getopt_long( argc, argv, "c:s:d:n:ohx", long_options, &option_index))!=-1) {
    switch(c) 
      {
      case 'h': 
	printUsage(std::cout);
	return 0;
      case 'c':
	//std::cout << "configFile : " << optarg << std::endl;
	configFile_override = std::string(optarg);
	overrideConfig = true;
	break;
      case 's':
	//std::cout << "sampleIn : " << optarg << std::endl;
	sampleIn_override = std::string(optarg);
	overrideInput = true;
	break;
      case 'd':
	//std::cout << "submitDir : " << optarg << std::endl;
	submitDir_override = std::string(optarg);
	overrideSubmitDir = true;
	break;
      case 'n':
	//std::cout << "numEvents : " << optarg << std::endl;
	nEvents_override = boost::lexical_cast<int>(optarg);
	overrideEvents = true;
	break;
      case 'o':
	//std::cout << "Enabling official production mode" << std::endl;
	officialProduction = true;
	break;
      case 'x':
	//std::cout << "Disabling grid submission for testing" << std::endl;
	overrideSubmit = true;
	break;
      default:
	printUsage(std::cout);
	return EXIT_FAILURE;
      }
    //std::cout << "End of option loop..."  << optarg << std::endl;
  }
  // steer file has to put here: FrameworkExe/data/framework-run.cfg
  // RootCore will copy it to $ROOTCOREBIN/data/FrameworkExe/framework-run.cfg
  // ConfigStore::createStore() will prepend "$ROOTCOREBIN/" to the path given
  std::string configPath = "data/FrameworkExe_VBFHbb/framework-run.cfg";
  if(overrideConfig) {
    configPath = configFile_override;
    Info("vbfhbbframework","Overriding configPath : %s", configPath.c_str());
  }
  
  ConfigStore * config = ConfigStore::createStore( configPath );
  if (!config) {
    Error("vbfhbbframework","Couldn't instantiate ConfigStore");
    return EXIT_FAILURE;
  }

  // enable failure on unchecked status codes
  bool enableFailure = false;
  config->getif<bool>("failUncheckedStatusCodes", enableFailure);
  if (enableFailure) {
    xAOD::TReturnCode::enableFailure();
    StatusCode::enableFailure();
    CP::CorrectionCode::enableFailure();
    CP::SystematicCode::enableFailure();
  }

  bool runCxAODMaker = true;
  config->getif<bool>("runCxAODMaker",runCxAODMaker);
  
  // output directory and file names (if not specified in steer file, default names are used)
  //  - output directory (submitDir)
  //  - output file name (sample_out) - in grid mode this is overridden below
  std::string submitDir  = "submitDir";
  std::string sample_out = "CxAOD"; 
  config->getif<std::string>("submitDir",submitDir);
  config->getif<std::string>("sample_out",sample_out);

  // overrides from command line
  if(overrideSubmitDir) {
    submitDir = submitDir_override;
    Info("vbfhbbframework","Overriding submitDir : %s",submitDir.c_str());
  }
  //
  // check that output directory does not exist (otherwise EventLoop will crash)
  if (!gSystem->AccessPathName(submitDir.c_str())) {
    Error("vbfhbbframework","Output directory already exists, please change the value of submitDir in the configuration file");
    return EXIT_FAILURE;
  }

  // input sample name
  // - local running : takes path or txt file as input
  // - batch running : txt file as input with local files, or not local files
  // - grid running  : takes txt files as input 
  std::string sample_in  = ""; 
  sample_in = config->get<std::string>("sample_in");
  //
  if(overrideInput) {
    sample_in = sampleIn_override;
    Info("vbfhbbframework","Overriding sampleIn : %s",sample_in.c_str());
  }
  //
  // for grid running - contains sample names from text file
  std::vector<std::string> sample_name_in;

  // set up the job for xAOD access:
  xAOD::Init().ignore();

  std::string mode = "local";
  config->getif<std::string>("mode", mode);
  bool inputIsTxt = sample_in.find(".txt") != std::string::npos;
  Info("vbfhbbframework","Run mode is '%s'", mode.c_str());

  // instantiate SampleHandler
  SH::SampleHandler sampleHandler;

  // set input samples
  if (mode == "local") {
    // read from local disk 
    if (inputIsTxt) {
      SH::readFileList (sampleHandler, sample_out, sample_in);
    } else {
      SH::DiskListLocal list(sample_in);
      if(runCxAODMaker){
        SH::scanSingleDir(sampleHandler, sample_out, list);
      }
      else{
        // for creating tuples from CxAOD
        SH::scanDir(sampleHandler,list,"*CxAOD.root*");
      }
    }
  }
  else if (mode == "batch" || mode == "grid") {
    //    sample_out = "";
    SH::readFileList (sampleHandler, sample_out, sample_in);

      // ====
   // read input file
   std::ifstream infile;
   infile.open(sample_in);
   if ( infile.fail() ) {
     Error("vbfhbbframework","Sample list file '%s' not found!", sample_in.c_str());
     return EXIT_FAILURE;
   }

   std::string line;
   while (!infile.eof()) {
     getline(infile, line);
     // remove all spaces from line
     line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
     // skip empty lines to avoid crash
     if (line.length() == 0) continue;
     // don't run over commented lines
     if (line.find("#") != std::string::npos) continue;
     // add sample name to vector
     sample_name_in.push_back(line);
   }
   infile.close();

     // check if text file contains sample names
   if ( sample_name_in.size() == 0) {
     Error("vbfhbbframework","No samples specified in file '%s'!", sample_in.c_str());
     return EXIT_FAILURE;
   }
   // declare the jobs
   for (unsigned int isam = 0; isam < sample_name_in.size(); ++isam) {
     if (sample_name_in[isam].find("/atlas/dq2/") != std::string::npos){
       auto const pos = sample_name_in[0].find_last_of('/');
       std::string path = sample_name_in[isam].substr(0, pos);
       std::string file = sample_name_in[isam].substr(pos);
std:: cout << "filepath: " <<path << file << "\n";
       SH::ScanDir().filePattern(file).scan(sampleHandler, path);
     }
     else 
       SH::scanDQ2(sampleHandler, sample_name_in[isam]);
     //os << "sample -> " << sample_name_in[isam] << std::endl;
   }
//======
  }
  else {
    Error("vbfhbbframework","Mode '%s' not known.", mode.c_str());
    return EXIT_FAILURE;
  }
  // Set the name of the input TTree. It's always "CollectionTree" for xAOD files.
  sampleHandler.setMetaString("nc_tree", "CollectionTree");
  //sampleHandler.setMetaString("nc_grid_filter", "AOD.01512594._000099.pool.root.1"); // use only 1 file to run on

  // print what we found:
  sampleHandler.print();

  // create an EventLoop job:
  EL::Job job;
  job.sampleHandler(sampleHandler);

  // limit number of events to maxEvents - set in config
  int eventMax = -1;
  config->getif<int>("maxEvents",eventMax);
  if(overrideEvents) {
    eventMax = nEvents_override;
    Info("vbfhbbframework","Overriding maxEvents : %i",eventMax);
  }

  job.options()->setDouble (EL::Job::optMaxEvents, eventMax);
  // Set the xAOD access mode of the job:
  //job.options()->setString( EL::Job::optXaodAccessMode, EL::Job::optXaodAccessMode_branch );
  //job.options()->setString (EL::Job::optXaodAccessMode, EL::Job::optXaodAccessMode_athena); //fk// Advice for reading reprocessed data xAOD, because of split_level branch issue (Jonathan Hays, June 8)
  job.options()->setString (EL::Job::optXaodAccessMode, EL::Job::optXaodAccessMode_class);


  // setup CxAODMaker
  if(runCxAODMaker){
    AnalysisBase_VBFHbb* algorithm = new AnalysisBase_VBFHbb();
    algorithm->m_maxEvent = static_cast<int>(job.options()->castDouble(EL::Job::optMaxEvents));
    algorithm->setConfig(config);
    job.algsAdd(algorithm);
  }

  // run the job using the direct driver (local) or the prun driver (grid) or the LSF driver (batch)
  if (mode == "local"){
    EL::DirectDriver*  eldriver = new EL::DirectDriver;
    eldriver->submit(job, submitDir);
  } 
  else if (mode == "batch"){
    EL::LSFDriver* eldriver = new EL::LSFDriver;
    std::string queue = "atlas-t3 -W 80:00";
    config->getif<std::string>("queue", queue);
    Info("vbfhbbframework","Submitting to queue %s",queue.c_str());
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

  } 
  else if (mode == "grid") {

    EL::PrunDriver driver;
    // determine ana_tag name
    std::string ana_tag = "ana_tag"; // default value if it cannot be determined below
    Info("SONO QUI","SAMPLE NAME IN: %s",sample_name_in[0].c_str());
    if      (sample_name_in[0].find("HIGG5D1") != std::string::npos) ana_tag = "HIGG5D1";
    else if (sample_name_in[0].find("HIGG5D2") != std::string::npos) ana_tag = "HIGG5D2";
    else if (sample_name_in[0].find("HIGG5D3") != std::string::npos) ana_tag = "HIGG5D3";    
    else if (sample_name_in[0].find("HIGG2D4") != std::string::npos) ana_tag = "HIGG2D4";

    bool autoDetermineSelection = false;
    config->getif<bool>("autoDetermineSelection",autoDetermineSelection);
    if (!autoDetermineSelection && ana_tag == "ana_tag") {
      std::string selectionName = "";
      config->getif<std::string>("selectionName",selectionName);
      if      (selectionName == "0lep") ana_tag = "HIGG5D1";
      else if (selectionName == "1lep") ana_tag = "HIGG5D2";
      else if (selectionName == "2lep") ana_tag = "HIGG2D4";
      else if (selectionName == "vbf" ) ana_tag = "HIGG5D3";
    }

    // form needed names
    std::string vtag = "vtag"; // default in case it is not specified in steer file
    config->getif<std::string>("vtag",vtag);

    bool haveOfficialSample = false;
    bool haveGroupSample = false;
    for (std::string name : sample_name_in ) {
      if (name.find("group.") != std::string::npos ||
          name.find("user.") != std::string::npos) {
        haveGroupSample = true;
      } else {
        haveOfficialSample = true;
      }
    }

    if (haveGroupSample && haveOfficialSample) {
      Error("vbfhbbframework","Found official and group production samples in input files! Cannot determine output sample names.");
      return EXIT_FAILURE;
    }

    // position of tag "mc14_13TeV" or similar in input sample
    // 1 = beginning (for official production)
    // 3 = for group/user production
    int tag_offset = 1;
    if (haveGroupSample) {
      tag_offset = 3;
    }

    // output dataset name definition
    //    // init    
    //    std::ostringstream ostr_sample_out;
    //    ostr_sample_out << "user.%nickname%.%in:name["
    //                    << tag_offset + 0 << "]%.%in:name["
    //                    << tag_offset + 1 << "]%.%in:name["
    //                    << tag_offset + 2 << "]%."
    //                    << ana_tag << "." << vtag;
    //    sample_out = ostr_sample_out.str();
    //    // options
    //    driver.options()->setString("nc_outputSampleName", sample_out);

    //driver.options()->setString(EL::Job::optGridExcludedSite,  "RRC-KI-T1_DATADISK"); //fk avoid a certain machine, because of compiler errors there

    // new
    std::ostringstream ostr_sample_out_begin;
    if(officialProduction) { // use group name for officialProduction
      Info("vbfhbbframework","Configuring for official production");
      ostr_sample_out_begin << "group.phys-higgs.%in:name["
        << tag_offset + 0 << "]%.%in:name["
        << tag_offset + 1 << "]%.";
    } else { // use user name for non-official production
      ostr_sample_out_begin << "user.%nickname%.%in:name["
        << tag_offset + 0 << "]%.%in:name["
        << tag_offset + 1 << "]%.";
    }
    std::string sample_out_begin = ostr_sample_out_begin.str();
    std::ostringstream ostr_sample_out_end;
    ostr_sample_out_end << "." << ana_tag << "." << vtag;
    std::string sample_out_end = ostr_sample_out_end.str();

    // loop on all input samples
    for (unsigned int isam = 0; isam < sample_name_in.size(); ++isam) {
      // find the proper part of the string to prune
      std::string sample_out_name2 = "";
      std::string str = sample_name_in[isam];
      int count_dots = 0;
      //
      std::string sample_out_sname = ".";
      int count_underscore = 0;
      for(unsigned int i = 0 ; i < str.length(); i++) {
        if ( str[i]=='.' ) count_dots++;
        // keep generator part of the name
        else if (count_dots==(tag_offset+1) ) sample_out_name2 += str[i];
        // keep the 1st _s... of reco
        else if (count_dots>(tag_offset+3) ) {
          if ( str[i]=='_' ) count_underscore++;
          else if (count_underscore==1) sample_out_sname += str[i];
        }
      }
      // prune generators
      std::string sample_out_prune = sample_out_name2;
      replaceAll(sample_out_prune,"ParticleGenerator","PG");
      replaceAll(sample_out_prune,"Pythia","Py");
      replaceAll(sample_out_prune,"Powheg","Pw");
      replaceAll(sample_out_prune,"MadGraph","MG");
      replaceAll(sample_out_prune,"EvtGen","EG");
      replaceAll(sample_out_prune,"Sherpa","Sh");
      replaceAll(sample_out_prune,"_CT10","");
      // set the prun option -> s-tag only on MC
      if (str[0]=='d') sampleHandler.setMetaString (".*"+sample_out_name2+".*", "nc_outputSampleName", sample_out_begin+sample_out_prune+sample_out_end);
      else             sampleHandler.setMetaString (".*"+sample_out_name2+".*"+sample_out_sname+".*", "nc_outputSampleName", sample_out_begin+sample_out_prune+sample_out_sname+sample_out_end);
    }  // output job name

    if( officialProduction) { // for official production - set the official command line option
      driver.options()->setString(EL::Job::optSubmitFlags, "--official");
    }

    // other options
    double nFilesPerJob = -1.;
    config->getif<double>("nFilesPerJob",nFilesPerJob);
    if (nFilesPerJob > 0) driver.options()->setDouble("nc_nFilesPerJob", nFilesPerJob);
    //
    double nGBPerJob = -1.;
    config->getif<double>("nGBPerJob",nGBPerJob);
    if (nGBPerJob > 0) {
      if (nGBPerJob == 1000.) driver.options()->setString("nc_nGBPerJob", "MAX");
      else driver.options()->setDouble("nc_nGBPerJob", nGBPerJob);
    }
    //
    std::string excludedSite = "none";
    config->getif<std::string>("excludedSite",excludedSite);
    if (excludedSite != "none") driver.options()->setString("nc_excludedSite", excludedSite);
    //
    bool submit = false;
    if(overrideSubmit) {
      Info("vbfhbbframework","Grid submission disabled from command line");
    }

    config->getif<bool>("submit",submit);
    if (!submit || overrideSubmit) { // don't submit the job if the config file says not to - or if overridden from the command line
      driver.options()->setDouble("nc_noSubmit", 1);
      driver.options()->setDouble("nc_showCmd", 1);
    }
    driver.options()->setString("nc_mergeOutput", "false"); // can do for CxAOD by now
    // run
    //driver.submit(job, submitDir); // with monitoring
    driver.submitOnly(job, submitDir); // no monitoring
  }
  else {
    Error("vbfhbbframework","Mode '%s' not known.", mode.c_str());
    return EXIT_FAILURE;
  }
  return 0;

}
