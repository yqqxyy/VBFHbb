#include "CxAODTools/XSectionProvider.h"
#include "PathResolver/PathResolver.h"

#include <TSystem.h>
#include <fstream>
#include <sstream>

#include "TError.h"

XSectionProvider::XSectionProvider(std::string fileName):
  m_config(nullptr),
  m_usePMGxsecDB(false),
  m_timestamp(""),
  m_analysis("")
{
  ReadXSectionLocal(fileName);
}

XSectionProvider::XSectionProvider(ConfigStore *config, std::string fileName):
  m_config(nullptr),
  m_usePMGxsecDB(false),
  m_timestamp(""),
  m_analysis("")
{
  if( !config ){
    Error("XSectionProvider()", "config is a null pointer");
  }
  m_config = config;
  m_config->getif<bool>("usePMGxsecDB", m_usePMGxsecDB);
  m_config->getif<std::string>("analysis", m_analysis);
  m_config->getif<std::string>("xsecFile_timestamp", m_timestamp);
  if(m_usePMGxsecDB){
    ReadXSectionPMG(); 
  } else {
    ReadXSectionLocal(fileName);
  }
}

void XSectionProvider::ReadXSectionLocal(std::string fileName) {

  std::ifstream file;
  file.open(gSystem->ExpandPathName(fileName.c_str()));
  if (!file.good()) {
    Error("XSectionProvider()", "Can't open file '%s'.", fileName.c_str());
    exit(EXIT_FAILURE);
  }

  Info("XSectionProvider()", "Reading file '%s'.", fileName.c_str());

  // process file
  while (!file.eof()) {
    // read line
    std::string lineString;
    getline(file, lineString);
    //std::cout << lineString << std::endl;

    // skip empty lines
    // TODO - is there a better way to implement this check?
    if (lineString.find(".") > 1000) {
      continue;
    }

    // skip lines starting with #
    if (lineString.find("#") == 0) {
      continue;
    }

    // store in map
    std::stringstream line(lineString);
    int sid;
    std::string name;
    float xSection;
    float kFactor;
    float filterEff;
    std::string samplename;

    line >> sid >> xSection >> kFactor >> filterEff >> samplename >> name;

    if (m_xSections.count(sid) != 0) {
      Warning("XSectionProvider()", "Skipping duplicated mc_channel_number for line '%s'.", lineString.c_str());
      continue;
    }

    m_xSections[sid] = {name, xSection, kFactor, filterEff, samplename};
  }
  file.close();
  return;
}

void XSectionProvider::ReadXSectionPMG() { 
  // Read DSIDs we are interested in from sample_info_ANALYSIS.txt to get histogram names
  // and cross-sections that we want to override PMG DB with
  // 
  readSampleInfo();
  
  // Read the time-stamped PMG DB
  auto fileNameXSDB = PathResolverFindCalibFile("dev/PMGTools/"+ m_timestamp +"/PMGxsecDB_mc16.txt");  
  std::ifstream file(gSystem->ExpandPathName(fileNameXSDB.c_str()));
  
  if (!file.good()) {
    Error("XSectionProvider()", "Can't open file '%s'.", fileNameXSDB.c_str());
    exit(EXIT_FAILURE);
  }

  Info("XSectionProvider()", "Reading file '%s'.", fileNameXSDB.c_str());

  // process file
  std::string content( (std::istreambuf_iterator<char>(file) ),
                       (std::istreambuf_iterator<char>()     ) ); 
  std::istringstream ss;
  ss.str(content);
  for (std::string lineString; std::getline(ss, lineString); ) {
    // skip empty lines
    if (lineString.empty()) continue;

    // skip lines starting with #
    if (lineString.find("#") == 0) continue;
    
    // store in map
    std::stringstream line(lineString);
    int sid;
    std::string name, genName, etag;
    float xSection, kFactor, filterEff;
    float uncertUp, uncertDn;
 
    line >> sid >> name >> xSection >> filterEff >> kFactor >> uncertUp >> uncertDn >> genName >> etag;
    
    // Do not store cross-section for samples not in the sample list
    if (m_sampleNamesAndTags.count(sid) == 0) continue;
    
    // If a specific e-tag was requested we must make sure we pick the correct one
    if (!m_sampleNamesAndTags[sid].etag.empty()){
      if (m_sampleNamesAndTags[sid].etag != etag) {
        Info("XSectionProvider()", "Sample: %6d forced to use e-tag %5s - skipping e-tag %5s!", 
	     sid, m_sampleNamesAndTags[sid].etag.c_str(), etag.c_str()); 
	continue;     
      }
    }
    
    // If XSection struct already exists for DSID use those values instead of the ones from PMG DB
    if (m_xSections.count(sid) != 0) {
      if (m_sampleNamesAndTags[sid].etag.empty()) {
        // We should probably turn this into a failure and exit
        Warning("XSectionProvider()", "Sample: %6d has multiple e-tags. You should choose which one to use!", sid);
      }
      if (m_xSections[sid].xSection  != 0.0) xSection  = m_xSections[sid].xSection;
      if (m_xSections[sid].kFactor   != 0.0) kFactor   = m_xSections[sid].kFactor;
      if (m_xSections[sid].filterEff != 0.0) filterEff = m_xSections[sid].filterEff;
    }
    // Fill XSection struct
    m_xSections[sid] = {name, xSection, kFactor, filterEff, m_sampleNamesAndTags[sid].sampleName};
    Info("XSectionProvider()", "Sample: %6d : xsec = %f , kfac =%f , filt = %f", 
	 sid, m_xSections[sid].xSection, m_xSections[sid].kFactor, m_xSections[sid].filterEff);
  }
  file.close();
  return;
}

bool XSectionProvider::hasMCchannel(int mc_channel_number) {
  return (m_xSections.count(mc_channel_number) != 0);
}

float XSectionProvider::getXSection(int mc_channel_number) {
  if (m_xSections.count(mc_channel_number) == 0) {
    Error("XSectionProvider::getXSection()", "Unknown mc_channel_number %i", mc_channel_number);
    exit(EXIT_FAILURE);
  }

  return m_xSections[mc_channel_number].xSection * m_xSections[mc_channel_number].kFactor * m_xSections[mc_channel_number].filterEff;
}

std::string XSectionProvider::getSampleName(int mc_channel_number) {
  //This function should only be called when running on MC
  if (m_xSections.count(mc_channel_number) == 0) {
    Error("XSectionProvider::getSampleName()", "Unknown mc_channel_number %i", mc_channel_number);
    exit(EXIT_FAILURE);
  }

  return m_xSections[mc_channel_number].samplename;
}

std::string XSectionProvider::getSampleDetailName(int mc_channel_number) {
  if (m_xSections.count(mc_channel_number) == 0) {
    Error("XSectionProvider::getSampleDetailName()", "Unknown mc_channel_number %i", mc_channel_number);
    exit(EXIT_FAILURE);
  }

  return m_xSections[mc_channel_number].name;
}

void XSectionProvider::readSampleInfo() {
  // Determine which sample_info file to use based on analysis
  std::string fileExt = "";
  if (m_analysis.find("VH")!=std::string::npos) fileExt = "VHqq";
  else if (m_analysis == "AZH_vvbb" || m_analysis == "AZH_lltt") fileExt = "AZH";
  
  // Open the file where the sample - short name map is stored
  std::string mapFile = gSystem->Getenv("WorkDir_DIR");
  mapFile += "/data/CxAODTools/sample_info_"+fileExt+".txt"; 
  std::ifstream file(gSystem->ExpandPathName(mapFile.c_str()));

  if (!file.good()) {
    Error("XSectionProvider()", "Can't open file '%s'.", mapFile.c_str());
    exit(EXIT_FAILURE);
  }

  Info("XSectionProvider()", "Reading file '%s'.", mapFile.c_str());
  
  // Read the file in memory
  std::string content( (std::istreambuf_iterator<char>(file) ),
                       (std::istreambuf_iterator<char>()     ) ); 
  std::istringstream ss;
  ss.str(content);
  for (std::string lineString; std::getline(ss, lineString); ) {
    // store in map
    std::stringstream line(lineString);
    int sid;
    std::string shortName;
    std::string longName;
    std::string etag;
    float xsec = 0.0;
    float kfac = 0.0;
    float filt = 0.0;
    
    // Read the columns
    line >> sid >> shortName >> longName >> etag >> xsec >> kfac >> filt;
        
    // If the 5th-7th columns exist we should overwrite the PMG cross-sections
    // with the ones from the sample_info file
    if (xsec != 0.0 || filt != 0.0 || kfac != 0.0) {
      m_xSections[sid] = {longName, xsec, kfac, filt, shortName};
    }
            
    m_sampleNamesAndTags[sid] = {etag, shortName};
  }  
  file.close();
}
