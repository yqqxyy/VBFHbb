#include <regex>

#include "xAODRootAccess/TEvent.h"

// CxAODTools includes
#include "CxAODTools/Utilities.h"
#include "CxAODTools/BTaggingTool.h"
#include "CxAODTools/CommonProperties.h"

// Infra structure (can be omitted - also included through BTaggingEfficiencyTool.h)
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicSet.h"

#include "PathResolver/PathResolver.h"

#include "EventLoop/StatusCode.h"
#include "CxAODTools/ReturnCheck.h"
#include "CxAODTools/XSectionProvider.h"

// ROOT includes
#include "TError.h"
#include "TDirectoryFile.h"
#include "TFile.h"
#include "TKey.h"
#include "TRandom3.h"

// STD includes
#include <utility>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>

using std::map;
using std::vector;
using std::set;
using std::string;
using CP::CorrectionCode;
using CP::SystematicSet;


//constructors for the BtagConfig
BTagConfig::BTagConfig(const std::map<std::string, std::string> &config) 

{

  // set default values
  tagger      = config.at("TaggerName");
  jetAuthor   = config.at("JetAuthor");
  btagcuteff  = config.at("OperatingPoint");
  scheme      = config.at("Scheme");

  std::string TTstrategy = config.at("TruthTagStrategy");
  doTruthTagging   = false;
  doHybridTagging  = false;
  if(TTstrategy == "T" || TTstrategy == "H"){
    doTruthTagging   = true;
    if( TTstrategy == "H")
      doHybridTagging  = true;
  }

  if (scheme == "Continuous"){
    wp = scheme;
    useContinuous = true;
  }
  else{
    wp = scheme +"_"+btagcuteff;
    useContinuous = false;
  }
  
  rScheme     = config.at("rScheme");
  ExcludeEV   = config.at("ExcludeEV");
  useQuantile = std::stoi(config.at("UseQuantile"));

  if (useContinuous){ //Needed for the truth tag tool.
    Info("BTagConfig()", "You are using a Continuous WP. Setting useQuantile to True");
    useQuantile = true;
  }

  maxTruthTag = std::stoi(config.at("maxTruthTag"));
  tagAlgorithm = config.at("tagAlgorithm");
  strategy = config.at("tagStrategy");

  useReducedSetOfBTagEV = false;
  
  if(ExcludeEV != ""){
    this->useReducedSetOfBTagEV = true;
    this->setExcludedBTagEV(ExcludeEV);
  }

  //Solution for VHcc - TODO: improve this?  
  if( btagcuteff.find("Veto") != string::npos ){
    TString vetotaggerstring = scheme+"_"+btagcuteff;
    this->setTaggerVeto(vetotaggerstring);
  }
  
  this->setTaggerType();

  Info("BTagConfig::BTagConfig()", "Initializing BtagConfig for %s %s %s", tagger.c_str(), jetAuthor.c_str(), wp.c_str());

  print();

}

// VHcc tagging: "veto" working points mean you require the jet to pass the nominal tagger, but fail the "veto-tagger"
BTagConfig& BTagConfig::setTaggerVeto(TString& vetotaggerstring){
  this->taggerName_veto = "";
  this->wp_veto = "";
    //op string should follow the format nominal_WP_Veto_vetotagger_vetoWP
    //for example, CTag_Loose_Veto_MV2c10_FixedCutBEff_70
    TObjArray* wptokens = vetotaggerstring.Tokenize("_");
    if( wptokens->GetEntries() != 6 ){
      Error("BTaggingTool: improperly formatted WP defintion: ",vetotaggerstring);
      exit(0);
    }
    this-> taggerName_veto = (std::string)(((TObjString *)(wptokens->At(3)))->String());
    this-> wp_veto = (std::string)(((TObjString *)(wptokens->At(4)))->String());
    this-> wp_veto = wp_veto+"_"+(std::string)(((TObjString *)(wptokens->At(5)))->String());
    
    Info("BTaggingTool::initialize", "Veto Tagger Name: %s OP veto: %s", this->taggerName_veto.c_str(), this->wp_veto.c_str());
    
    return *this; 
}

BTagConfig& BTagConfig::setExcludedBTagEV(const std::string &list)
{
  if (list == "") {
    return *this;
  }
  ExcludeEV = list;
  return *this;
}

BTagConfig& BTagConfig::setTagger (const std::string &tagName)
{
  if (this->tagger == tagName) {
    return *this;
  }
  this->tagger = tagName;
  setTaggerType();

  return *this;
}

BTagConfig& BTagConfig::setTaggerType ()
{

  if (this->tagger == "MV2c10") this->taggerEnum = TaggerType::MV2c10;
  else if (this->tagger == "DL1") this->taggerEnum = TaggerType::DL1;
  else if (this->tagger == "DL1r") this->taggerEnum = TaggerType::DL1r;
  else this->taggerEnum = TaggerType::UNKNOWN;

  if(this->taggerName_veto!=""){
    if (this->taggerName_veto == "MV2c10") this->Veto_taggerEnum = TaggerType::MV2c10;
    else if (this->taggerName_veto == "DL1") this->Veto_taggerEnum = TaggerType::DL1;
    else if (this->taggerName_veto == "DL1r") this->Veto_taggerEnum = TaggerType::DL1r;
    else this->Veto_taggerEnum = TaggerType::UNKNOWN;
  }
  return *this;
} // setTaggerType

BTagConfig& BTagConfig::setJetAuthor (const std::string &author)
{
  if (jetAuthor == author) {
    return *this;
  }
  this->jetAuthor = author;
  return *this;
}


BTagConfig& BTagConfig::setTaggingScheme (const std::string &sc)
{
  if (scheme == sc) {
    return *this;
  }
  this->scheme = sc;
  return *this;
}

BTagConfig& BTagConfig::setTaggerEfficiency (const int &eff)
{
  if (std::stoi(this->btagcuteff) == eff) {
    return *this;
  }
  this->btagcuteff = std::to_string(eff);

  return *this;

} // setTaggerEfficiency

BTagConfig& BTagConfig::setBinEdges(const std::string& fileNameCDI) {

  //for getting the quantile, extract the cut values from the CDI
  TFile* inf = TFile::Open(fileNameCDI.c_str(), "read");
  std::vector<std::string> availableOP_fixCut= {"FixedCutBEff_85", "FixedCutBEff_77","FixedCutBEff_70","FixedCutBEff_60"};
  for(unsigned int iop=0; iop< availableOP_fixCut.size(); iop++){
    TString cutname = this->tagger+"/"+this->jetAuthor+"/"+availableOP_fixCut.at(iop)+"/cutvalue";

    Info("setbinEdges()", "Retrieving bin edges for: %s ", cutname.Data());

    float cutval = ((TVector*) inf->Get(cutname))[0](0);
    this->binEdges.push_back(cutval);
  }

  inf->Close();

  return *this;
}

void BTagConfig::print() const {
  std::cout <<"================= print summary: =================" <<std::endl;
  std::cout <<"tagger          : " <<tagger          <<std::endl;  // Btagging Tagger: DL1, MV2c10, DL1r..
  std::cout <<"jetAuthor       : " <<jetAuthor       <<std::endl;  // Jet Collection: Pflow, TrackJets..
  std::cout <<"workingPoint    : " <<wp              <<std::endl;  // Working point in string
  std::cout <<"btagcuteff      : " <<btagcuteff      <<std::endl;  ///Threshold in string
  std::cout <<"scheme          : " <<scheme          <<std::endl;  // FixedCut or Continuous
  std::cout <<"rScheme         : " <<rScheme         <<std::endl;  // Reduction scheme
  std::cout <<"wp_veto         : " <<wp_veto         <<std::endl;  // Operating point for the veto used in VHcc only
  std::cout <<"taggerName_veto : " <<taggerName_veto <<std::endl;  //taggername for the veto used in VHcc only
  std::cout <<"ExcludeEV       : " <<ExcludeEV       <<std::endl;  ///list of EV to be excluded
  
  
  std::cout <<"useQuantile           : " <<useQuantile <<std::endl;
  std::cout <<"useContinuous         : " <<useContinuous <<std::endl;
  std::cout <<"useReducedSetOfBTagEV : " <<useReducedSetOfBTagEV <<std::endl;

  //Truth Tagging Settings
  std::cout <<"maxTruthTag      : " <<maxTruthTag <<std::endl;
  std::cout <<"requiredTT       : " <<requiredTT <<std::endl;
  std::cout <<"exclusive        : " <<exclusive <<std::endl;
  std::cout <<"doHybridTagging  : " <<doHybridTagging <<std::endl;
  std::cout <<"doTruthTagging   : " <<doTruthTagging <<std::endl;
  std::cout <<"tagAlgorithm     : " <<tagAlgorithm <<std::endl;
  std::cout <<"strategy         : " <<strategy <<std::endl;
  std::cout <<"binEdges         : ";
  for (auto bin : binEdges) std::cout <<bin <<" "; 
  std::cout <<std::endl;
  std::cout <<"==================================================" <<std::endl;

}

//////////////////////////////////////////////////////////////////////////////////////////////////

StatusCode BTaggingTool::initialize (std::unordered_map<std::string, BTagConfig >& btc_map, const std::string &filename, bool debug)
{ 
  Info("BTaggingTool::initialize()", "Initializing BTagging Tool");
  
  if(filename == ""){
    Error("BTaggingTool::Initialize()", "No CDI file specified. Exiting.");
    return StatusCode::FAILURE;
  }

  m_debug = debug;
  auto fileNameCDI = PathResolverFindCalibFile("xAODBTaggingEfficiency/13TeV/" + filename + ".root");
  
  // determine CDI file type
  //ttbar uncertaintites based on r21 samples, MC/MC SFs, first pseudo-cont. calibration to test
  
  if (fileNameCDI.find("2018") != string::npos){
    Error("Initialize()", "The BTaggingTool supports only CDIs released after 2019. Please update it.");
    return StatusCode::FAILURE;
  }
  
  if (fileNameCDI.find("2017-21-13TeV-MC16-CDI-2019-07-30") != string::npos) m_reference_file_type = CDI_FileType::OFFICIAL_JUL_2019;
  if (fileNameCDI.find("2019-21-13TeV-MC16-CDI-2019-10-07") != string::npos) m_reference_file_type = CDI_FileType::OFFICIAL_OCT_2019;
  if (fileNameCDI.find("2020-21-13TeV-MC16-CDI-2020-03-11") != string::npos) m_reference_file_type = CDI_FileType::OFFICIAL_MAR_2020;
  if (fileNameCDI.find("2020-21-13TeV-MC16-CDI-2020-12-02") != string::npos) m_reference_file_type = CDI_FileType::OFFICIAL_DEC_2020;
  if (fileNameCDI.find("2020-21-13TeV-MC16-CDI-2021-04-16") != string::npos) m_reference_file_type = CDI_FileType::OFFICIAL_APR_2021;

  //in case the path resolver does not find the file, assume the user gave the full path to a local CDI
  if(fileNameCDI==""){
    fileNameCDI = filename;
    //assume that if the user is using a local CDI it is similar to the latest official CDI
    m_reference_file_type = CDI_FileType::OFFICIAL_MAR_2020;
    //then check some specific options which have custom settings for efficiency maps
    if (fileNameCDI.find("2018") != string::npos){
      Error("Initialize()", "The BTaggingTool supports only CDIs released after 2019. Please update it.");
      return StatusCode::FAILURE;
    }
    if (fileNameCDI.find("2019-07-30") != string::npos){m_reference_file_type = CDI_FileType::OFFICIAL_JUL_2019;}
    if (fileNameCDI.find("2019-10-07") != string::npos){m_reference_file_type = CDI_FileType::OFFICIAL_OCT_2019;}
    if (fileNameCDI.find("2020-03-11") != string::npos){m_reference_file_type = CDI_FileType::OFFICIAL_MAR_2020;}
    if (fileNameCDI.find("2020-12-02") != string::npos){m_reference_file_type = CDI_FileType::OFFICIAL_DEC_2020;}
    if (fileNameCDI.find("2021-04-16") != string::npos){m_reference_file_type = CDI_FileType::OFFICIAL_APR_2021;}
    
    if (fileNameCDI.find("2017-21-13TeV-MC16-CDI-May_04_2019.root")!=string::npos){ m_reference_file_type = CDI_FileType::VHcc_Custom_CDI; }
    if (fileNameCDI.find("2017-21-13TeV-MC16-CDI-July10_2019.root")!=string::npos){ m_reference_file_type = CDI_FileType::VHcc_Custom_CDI; }
    if (fileNameCDI.find("2017-21-13TeV-MC16-CDI-October3_2019.root")!=string::npos){ m_reference_file_type = CDI_FileType::VHcc_Custom_CDI; }
    if (fileNameCDI.find("2017-21-13TeV-MC16-CDI-November4-2019.root")!=string::npos){ m_reference_file_type = CDI_FileType::VHcc_Custom_CDI; }
  }
  
  // determine which tagger, jet author, and working point combinations to initialize
  auto fileCDI = TFile::Open(fileNameCDI.c_str(), "read");
  
  for (auto i : *fileCDI->GetListOfKeys()) {
    auto key = static_cast<TKey*>(i);
    TString class_name(key->GetClassName());
    TString tagger_name(key->GetName());
    
    if ((tagger_name != "VersionInfo") && (class_name == "TDirectoryFile")) {
      auto jet_author_dir = static_cast<TDirectoryFile*>(fileCDI->GetObjectUnchecked(tagger_name.Data()));
      
      for (auto j : *jet_author_dir->GetListOfKeys()) {
	key        = static_cast<TKey*>(j);
	class_name = key->GetClassName();
	TString jet_author_name(key->GetName());
	
	if (class_name == "TDirectoryFile") {
	  auto working_point_dir = static_cast<TDirectoryFile*>(jet_author_dir->GetObjectUnchecked(jet_author_name.Data()));
	  
	  for (auto k : *working_point_dir->GetListOfKeys()) {
	    key        = static_cast<TKey*>(k);
	    class_name = key->GetClassName();
	    TString working_point_name(key->GetName());
	    
	    // check validity (directories with few entries are likely not valid)
	    if (class_name == "TDirectoryFile") {
	      auto flavor_dir = static_cast<TDirectoryFile*>(working_point_dir->GetObjectUnchecked(working_point_name.Data()));
	      
	      if (flavor_dir->GetListOfKeys()->GetEntries() < 1) continue;
	      for (auto l : *flavor_dir->GetListOfKeys()) {
		key = static_cast<TKey*>(l);
		class_name = key->GetClassName();
		TString flavor_name(key->GetName());
		
		if (class_name == "TDirectoryFile") {
		  auto calib_dir = static_cast<TDirectoryFile*>(flavor_dir->GetObjectUnchecked(flavor_name.Data()));
		  
		  for (auto m : *calib_dir->GetListOfKeys()) {
		    key = static_cast<TKey*>(m);
		    class_name = key->GetClassName();
		    TString calib_name(key->GetName());
		    
		    if (class_name == "TDirectoryFile") continue;
		    
		    string calib(calib_name.Data());
		    if (calib.find("_Eff") != string::npos &&
			calib.find("_CalibrationBinned_") == string::npos &&
			std::count(calib.begin(), calib.end(), '_') == 1) {
		      auto pos = std::find(calib.begin(), calib.end(), '_');
		      calib.erase(pos, calib.end());
		      if (Analysis::is_numeric(calib) && m_flavorDSID[flavor_name.Data()].find(calib) == string::npos) {
			if(m_flavorDSID[flavor_name.Data()].size() == 0) m_flavorDSID[flavor_name.Data()] += calib;
			else m_flavorDSID[flavor_name.Data()] += ";" + calib;
		      }
		    }
		  }
		}
	      }
	    } else continue;
	    
	    // efficiency tool
	    if(m_debug)
	      Info("BTaggingTool::initialize", "Found efficiency tool settings: %s, %s, %s", tagger_name.Data(), jet_author_name.Data(), working_point_name.Data());
	    
	    if (m_efficiencyTools.count(tagger_name.Data()) == 0) {
	      std::unordered_map < std::string, std::shared_ptr < BTaggingEfficiencyTool >>
		inner { { working_point_name.Data(), nullptr } };
	      std::unordered_map < std::string,
	      std::unordered_map < std::string, std::shared_ptr<BTaggingEfficiencyTool >>>
	      mid { { jet_author_name.Data(), inner } };
	      m_efficiencyTools[tagger_name.Data()] = mid;
	    }
	    else if (m_efficiencyTools[tagger_name.Data()].count(jet_author_name.Data()) == 0) {
	      std::unordered_map < std::string, std::shared_ptr < BTaggingEfficiencyTool >>
		inner { { working_point_name.Data(), nullptr } };
	      m_efficiencyTools[tagger_name.Data()][jet_author_name.Data()] = inner;
	    }
	    else {
	      m_efficiencyTools[tagger_name.Data()][jet_author_name.Data()][working_point_name.Data()] =
		nullptr;
	    }
	  
	    if(working_point_name.Contains("FixedCutBEff") || working_point_name.Contains("CTag") || working_point_name.Contains("Continuous")){
	    
	      if(m_debug)
		Info("BTaggingTool::initialize", "Found TruthTaggingTool settings: %s, %s, %s", tagger_name.Data(), jet_author_name.Data(), working_point_name.Data());
	      
	      if (m_TruthTaggingTools.count(tagger_name.Data()) == 0) {
		std::unordered_map < std::string, std::shared_ptr < BTaggingTruthTaggingTool >> inner { { working_point_name.Data(), nullptr } };
		std::unordered_map < std::string,
				     std::unordered_map < std::string, std::shared_ptr<BTaggingTruthTaggingTool >>>
		  mid { { jet_author_name.Data(), inner } };
		m_TruthTaggingTools[tagger_name.Data()] = mid;
	      }
	      else if (m_TruthTaggingTools[tagger_name.Data()].count(jet_author_name.Data()) == 0) {
		std::unordered_map < std::string, std::shared_ptr < BTaggingTruthTaggingTool >>
		  inner { { working_point_name.Data(), nullptr } };
		m_TruthTaggingTools[tagger_name.Data()][jet_author_name.Data()] = inner;
	      }
	      else {
		m_TruthTaggingTools[tagger_name.Data()][jet_author_name.Data()][working_point_name.Data()] =
		  nullptr;
	      }
	    }
	    // selection tool
	    if(m_debug)
	      Info("BTaggingTool::initialize", "Found selection tool settings: %s, %s, %s", tagger_name.Data(), jet_author_name.Data(), working_point_name.Data());
	    
	    if (m_selectionTools.count(tagger_name.Data()) == 0) {
	      std::unordered_map < std::string, std::shared_ptr < BTaggingSelectionTool >>
		inner { { working_point_name.Data(), nullptr } };
	      std::unordered_map < std::string,
				   std::unordered_map < std::string, std::shared_ptr<BTaggingSelectionTool >>>
		mid { { jet_author_name.Data(), inner } };
	      m_selectionTools[tagger_name.Data()] = mid;
	    }
	    else if (m_selectionTools[tagger_name.Data()].count(jet_author_name.Data()) == 0) {
	      std::unordered_map < std::string, std::shared_ptr < BTaggingSelectionTool >>
		inner { { working_point_name.Data(), nullptr } };
	      m_selectionTools[tagger_name.Data()][jet_author_name.Data()] = inner;
	    }
	    else {
	      m_selectionTools[tagger_name.Data()][jet_author_name.Data()][working_point_name.Data()] =
		nullptr;
	    }
	  }
	}
      }
    }
  }
  fileCDI->Close();
  
  //overwrite m_flavorDSID
  if(m_reference_file_type == CDI_FileType::OFFICIAL_JUL_2019
     || m_reference_file_type == CDI_FileType::OFFICIAL_OCT_2019
     || m_reference_file_type == CDI_FileType::OFFICIAL_MAR_2020
     || m_reference_file_type == CDI_FileType::OFFICIAL_DEC_2020){
    //pythia8, sherpa 2.2.1, herwig7, aMC@NLO+Pythia8
    m_flavorDSID["B"] = "410470;410250;410558;410464";
    m_flavorDSID["C"] = "410470;410250;410558;410464";
    m_flavorDSID["Light"] = "410470;410250;410558;410464";
    m_flavorDSID["T"] = "410470;410250;410558;410464";
  }
  if(m_reference_file_type == CDI_FileType::OFFICIAL_APR_2021){
    //Pythia8, Sherpa 2.2.1, Herwig 7.0.4, Py8_aMCNLO, Herwig 7.1.3, Sherpa 2.2.8, Sherpa 2.2.10, Herwig 7.2.1
    m_flavorDSID["B"] = "410470;410250;410558;410464;411233;421152;700122;600666";
    m_flavorDSID["C"] = "410470;410250;410558;410464;411233;421152;700122;600666";
    m_flavorDSID["Light"] = "410470;410250;410558;410464;411233;421152;700122;600666";
    m_flavorDSID["T"] = "410470;410250;410558;410464;411233;421152;700122;600666";
  }
  else if(m_reference_file_type == CDI_FileType::VHcc_Custom_CDI){
    
    m_flavorDSID["B"] = "410470;410250;410558;W_jets_a;ttbar_a;s_top_a;Diboson_a;W_jets_d;ttbar_d;s_top_d;Diboson_d;W_jets_e;ttbar_e;s_top_e;Diboson_e";
    m_flavorDSID["C"] = "410470;410250;410558;W_jets_a;ttbar_a;s_top_a;Diboson_a;W_jets_d;ttbar_d;s_top_d;Diboson_d;W_jets_e;ttbar_e;s_top_e;Diboson_e";
    m_flavorDSID["Light"] = "410470;410250;410558;W_jets_a;ttbar_a;s_top_a;Diboson_a;W_jets_d;ttbar_d;s_top_d;Diboson_d;W_jets_e;ttbar_e;s_top_e;Diboson_e";
    m_flavorDSID["T"] = "410470;410250;410558;W_jets_a;ttbar_a;s_top_a;Diboson_a;W_jets_d;ttbar_d;s_top_d;Diboson_d;W_jets_e;ttbar_e;s_top_e;Diboson_e";
  }

  m_fileNameCDI = fileNameCDI;
  
  if(m_debug){
    std::cout << "BTaggingTool:: CDIFilename (m_fileNameCDI):  " <<m_fileNameCDI <<std::endl;
    std::cout << "BTaggingTool:: m_flavorDSID[\"B\"]:     " <<m_flavorDSID["B"] <<std::endl;
    std::cout << "BTaggingTool:: m_flavorDSID[\"C\"]:     " <<m_flavorDSID["C"] <<std::endl;
    std::cout << "BTaggingTool:: m_flavorDSID[\"Light\"]: " <<m_flavorDSID["Light"] <<std::endl;
    std::cout << "BTaggingTool:: m_flavorDSID[\"T\"]:     " <<m_flavorDSID["T"] <<std::endl;
  }

  //now store in memory all the btagging configurations
  EL_CHECK("BtaggingTool::initialize()", Set_BtagConfigMap(btc_map));

  return StatusCode::SUCCESS;
} //initialize

StatusCode BTaggingTool::Set_BtagConfigMap(std::unordered_map<std::string, BTagConfig>& map){

  for(auto pair : map){
    auto btc = pair.second;
    
    if(m_btc_map.count(pair.first)){
      Info("Set_BtagConfigMap()", " BtaggingConfig named %s already exixts. Skipping.", pair.first.c_str());
      continue;
    }

    Info("Set_BtagConfigMap()", "Initializing BtaggingTool for %s %s %s", btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.wp.c_str());
    if(btc.wp.find("FixedCut") != string::npos ){
      btc.setBinEdges(m_fileNameCDI);
    }
    m_btc_map.insert(pair); //cache the BtagConfig
    selectTools(btc); //initialize the related tools

    //in case we have a tag +  veto system
    if(btc.taggerName_veto != ""){
      Info("Set_BtagConfigMap()", "Found also some VETO settings. Initializing BtaggingTool for %s %s %s",
	   btc.taggerName_veto.c_str(), btc.jetAuthor.c_str(), btc.wp_veto.c_str());
      selectTools(btc, true);
    }
  } //loop

  return StatusCode::SUCCESS;
} //Set_BtagConfigMap

StatusCode BTaggingTool::AddElement_BtagConfigMap(std::string name, BTagConfig& btc){

  if(m_btc_map.count(name)){
    Info("AddElement_BtagConfigMap()", " BtaggingConfig named %s already exixts. Skipping.", name.c_str());
    return StatusCode::SUCCESS;
  }

  Info("AddElement_BtagConfigMap()", "Initializing BtaggingTool for %s %s %s",
       btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.wp.c_str());

  if(btc.wp.find("FixedCut") != string::npos ){
    btc.setBinEdges(m_fileNameCDI);
  }
  m_btc_map.insert(std::pair<std::string, BTagConfig>(name, btc));
  selectTools(btc); //initialize the related tools

  //in case we have a tag +  veto system
  if(btc.taggerName_veto != ""){
    Info("AddElement_BtagConfigMap()", "Found also some VETO settings. Initializing BtaggingTool for %s %s %s",
	 btc.taggerName_veto.c_str(), btc.jetAuthor.c_str(), btc.wp_veto.c_str());
    selectTools(btc, true);
  }

  return StatusCode::SUCCESS;
}

StatusCode BTaggingTool::Load_DefaultBtagConfig(BTagConfig& default_btc){

if(m_btc_map.count("Default")){
  Info("Load_DefaultBtagConfig()", " BtaggingConfig named Default already exixts. Skipping.");
  return StatusCode::FAILURE;
 }

 Info("Load_DefaultBtagConfig()", "Initializing BtaggingTool for %s %s %s", 
      default_btc.tagger.c_str(), default_btc.jetAuthor.c_str(), default_btc.wp.c_str());

if(default_btc.wp.find("FixedCut") != string::npos ){
  default_btc.setBinEdges(m_fileNameCDI);
 }

 m_btc_map.insert(std::pair<std::string, BTagConfig>("Default", default_btc));

 selectTools(default_btc); //initialize the related tools

 //in case we have a tag +  veto system
 if(default_btc.taggerName_veto != ""){
   Info("Load_DefaultBtagConfig()", "Found also some VETO settings. Initializing BtaggingTool for %s %s %s", 
	default_btc.taggerName_veto.c_str(), default_btc.jetAuthor.c_str(), default_btc.wp_veto.c_str());
   selectTools(default_btc, true);
 }
return StatusCode::SUCCESS;
}

StatusCode BTaggingTool::Set_BtagConfigAsDefault(std::string btc_name){

if(m_btc_map.count("Default")){
  Error("Set_BtagConfigAsDefault()", " BtaggingConfig named Default already exixts. Are you sure you want to Overwrite?.");
  return StatusCode::FAILURE;
 }

if(!m_btc_map.count(btc_name)){
  Error("Set_BtagConfigAsDefault()", " BtaggingConfig named %s does not exist in memory. "
	"Please initialize it first or use the Load_DefaultBtagConfig method", btc_name.c_str());
  return StatusCode::FAILURE;
 }

 if (m_btc_map.find(btc_name) != m_btc_map.end()){
   auto tmp_btc = m_btc_map.at(btc_name);
   m_btc_map.insert(std::pair<std::string, BTagConfig>("Default", tmp_btc));
   m_btc_map.erase(btc_name);
 }

return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////
//Tagging Functions: Direct Tagging

//use MC label
void BTaggingTool::isTagged_UsingTruthFlavLabel(const xAOD::Jet *Jet){
  if (Props::HadronConeExclTruthLabelID.get(Jet) == 5){
    BTagProps::isTagged.set(Jet, true);
  }
  return;
}


bool BTaggingTool::isTagged(const xAOD::Jet &jet,  std::string btc_name){

  auto  btc = m_btc_map.at(btc_name);
  auto seltool = m_selectionTools[btc.tagger][btc.jetAuthor][btc.wp];
  if(!seltool){
    Warning("BTaggingTool::isTagged()", "selection tool is not initialized for "
	    "%s (tagger), %s (jet author), and %s (working point)!",
	  btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.wp.c_str());
  }
  
  if(btc.taggerName_veto != "")
    return isJetTaggedVeto(jet, seltool, btc.taggerEnum, btc.Veto_taggerEnum);
  else
    return isJetTagged(jet, seltool, btc.taggerEnum, btc.useContinuous, btc.btagcuteff);
}

bool BTaggingTool::isJetTaggedVeto (const xAOD::Jet &jet, std::shared_ptr<BTaggingSelectionTool> &select_ptr, TaggerType taggerEnum, TaggerType taggerEnum_veto) const
{
  if (auto tool = select_ptr){ 
    float tagger_weight_nominal = getJetTaggerWeight(jet , tool, taggerEnum);
    float tagger_weight_veto = getJetTaggerWeight(jet, tool, taggerEnum_veto);
    return static_cast<bool>(tool->accept(jet.pt(), jet.eta(), tagger_weight_nominal,tagger_weight_veto));
  }
  
  Error("BTaggingTool::isJetTaggedVeto()", "problem retrieving the selection tool. Exiting.");
  return false;
} // isJetTaggedVeto


bool BTaggingTool::isJetTagged (const xAOD::Jet &jet, std::shared_ptr<BTaggingSelectionTool> &select_ptr, TaggerType taggerEnum, bool useContinuous, std::string trsh) const
{
  if (auto tool = select_ptr){
    float tagger_weight = getJetTaggerWeight(jet, tool, taggerEnum);
      if(!useContinuous){
	//	Info("isTagged()", "pt %f, eta %f", jet.pt(), jet.eta());
	//	Info("isTagged()", "tagger %s, %s, %s,  tagger weight: %f,isTagged? %d", m_tagger.c_str(), m_jetAuthor.c_str(), m_workingPoint.c_str());
	return static_cast<bool>(tool->accept(jet.pt(), jet.eta(), tagger_weight));
      }
      else{
	int effBin = -99;
	getBinFromEfficiency(trsh, effBin);

	return static_cast<bool> (tool->getQuantile(jet.pt(), jet.eta(), tagger_weight) >= effBin ? true : false);
	//Info("isTagged()", "tagger %s, %s, %s,  tagger weight: %f, isTagged? %d", m_tagger.c_str(), m_jetAuthor.c_str(), m_workingPoint.c_str(), tagger_weight, isTag);
      }
  }
  
  Error("BTaggingTool::isJetTagged()", "problem retrieving the selection tool. Exiting.");
  return false;
  
} // isJetTagged


float BTaggingTool::getTaggerWeight (const xAOD::Jet &jet,  std::string btc_name){
  //this function is here to be a non-const interface for an external user of the tool (since it uses selectTools())
  //the private getJetTaggerWeight must be const, since its used inside other const functions.
  auto btc = m_btc_map.at(btc_name);
  auto seltool = m_selectionTools[btc.tagger][btc.jetAuthor][btc.wp];
  if(!seltool){
   Error("BTaggingTool::isTagged()", "selection tool is not initialized for "
	    "%s (tagger), %s (jet author), and %s (working point)!",
	    btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.wp.c_str());
  }

  return getJetTaggerWeight(jet, seltool, btc.taggerEnum);

}


float BTaggingTool::getJetTaggerWeight (const xAOD::Jet &jet, std::shared_ptr<BTaggingSelectionTool> &select_ptr, TaggerType taggerEnum) const{

  if (auto tool = select_ptr) {
    float tagger_weight = -1;

    switch(taggerEnum){

    case TaggerType::UNKNOWN:
      {
	Error("BTaggingTool::getJetTaggerWeight","tagger type unknown");
	break;
      }
    case TaggerType::MV2c10:
      {
	tagger_weight=Props::MV2c10.get(&jet);
	break;
      }
    case TaggerType::DL1:
      {
	float tagger_pb = Props::DL1_pb.get(&jet);
	float tagger_pc = Props::DL1_pc.get(&jet);
	float tagger_pu = Props::DL1_pu.get(&jet);

	double tagweight =-99;
	CorrectionCode code = tool->getTaggerWeight(tagger_pb, tagger_pc, tagger_pu,tagweight);
	if(code!=CorrectionCode::Ok){
	  Error("BTaggingTool::getJetTaggerWeight","failed to get DL1 tag weight");
	  return -99;
	}
	tagger_weight = (float)tagweight;
	break;
      }
    case TaggerType::DL1r:
      {
	float tagger_r_pb = Props::DL1r_pb.get(&jet);
	float tagger_r_pc = Props::DL1r_pc.get(&jet);
	float tagger_r_pu = Props::DL1r_pu.get(&jet);

	double tagrweight =-99;
	CorrectionCode code_r = tool->getTaggerWeight(tagger_r_pb, tagger_r_pc, tagger_r_pu,tagrweight);
	if(code_r!=CorrectionCode::Ok){
	  Error("BTaggingTool::getJetTaggerWeight","failed to get DL1r tag weight");
	  return -99;
	}
	tagger_weight = (float)tagrweight;
	break;
      }
    }
    return tagger_weight;

  }
  Error("BTaggingTool::getJetTaggerWeight()", "problem retrieving the selection tool. Exiting.");

  return -99;

}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Getters of the btagging tool: 
// GetEfficiency, GetMCInefficiency, GetMCEfficiency, GetScaleFactor, 
// GetInefficiencySF, getFullEfficiency, getFullInefficiency, getQuantile...
float BTaggingTool::getEfficiency (const xAOD::Jet &jet, std::string btc_name)
{
  auto  btc = m_btc_map.at(btc_name);
  // check if tool is initialized
  if (auto tool = m_efficiencyTools[btc.tagger][btc.jetAuthor][btc.wp]) {

    if (m_MCIndex == 9999 || !tool->setMapIndex(getFlavorLabel(jet), m_MCIndex)) {
      Error("BTaggingTool::getEfficiency",
	    "Couldn't set MC/MC index properly. Results will be biasd. Flavor: %s, Index: %u",
	    getFlavorLabel(jet).c_str(), m_MCIndex);
    }
    // check eta of jet
    // the tool will initiate a core dump (!) if we feed a jet with
    // |eta| > 2.5 to it... it's a design choice, like it or not...
    if (fabs(jet.eta()) > 2.5) return 0.0;

    // get efficiency
    float eff;
    auto status = CorrectionCode::OutOfValidityRange;
    if(btc.useContinuous){
      Analysis::CalibrationDataVariables m_variables;
      m_variables.jetAuthor = btc.jetAuthor;
      m_variables.jetPt = jet.pt(); // in MeV
      m_variables.jetEta = jet.eta();
      float tagger_weight = getTaggerWeight(jet, btc_name);

      m_variables.jetTagWeight = tagger_weight; // here you specify the MV1 or MV1c value of the jet

      status = tool->getEfficiency(getFlavor(jet), m_variables, eff);
  }
  else
    status = tool->getEfficiency(jet, eff);

  // check return code
  if (status == CorrectionCode::OutOfValidityRange && m_printOutOfRangeWarnings) {
    // out of extrapolation range of tool (high jet pt) - result should not be used
    Warning("BTaggingTool::getEfficiency", "Jet is out of the validity range of the BTaggingEfficiencyTool! "
              "Please exercise caution using this efficiency. Contact expert if unsure how to proceed. "
	    "jet details: eta %f, pt %f, eff %f", jet.eta(), jet.pt(), eff);
  }
  else if (status == CorrectionCode::Error) {
    // something went wrong - print error message
    Error("BTaggingTool::getEfficiency()", "tool returned CP::CorrectionCode::Error");
    return 0.;
  }
  // All is okay
  return eff;
}
 else {
   Error("BTaggingTool::getEfficiency()", "efficiency tool is not initialized for "
	 "%s (tagger), %s (jet author), %s (working point), and %s (efficiency scheme)!",
	 btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.wp.c_str(), btc.scheme.c_str());
   return 0.0;
 }
} // getEfficiency

float BTaggingTool::getInefficiency (const xAOD::Jet &jet, std::string btc_name)
{
  auto  btc = m_btc_map.at(btc_name);
  // check if tool is initialized
  if (auto tool = m_efficiencyTools[btc.tagger][btc.jetAuthor][btc.wp]){
    if (m_MCIndex == 9999 || !tool->setMapIndex(getFlavorLabel(jet), m_MCIndex)) {
      Error("BTaggingTool::getInefficiency",
	    "Couldn't set MC/MC index properly. Results will be biasd. Flavor: %s, Index: %u",
	    getFlavorLabel(jet).c_str(), m_MCIndex);
    }

    // check eta of jet
    // the tool will initiate a core dump (!) if we feed a jet with
    // |eta| > 2.5 to it... it's a design choice, like it or not...
    if (fabs(jet.eta()) > 2.5) return 0.0;

    // get efficiency
    float eff;
    auto  status = tool->getInefficiency(jet, eff);

    // check return code
    if (status == CorrectionCode::OutOfValidityRange && m_printOutOfRangeWarnings) {
      // out of extrapolation range of tool (high jet pt) - result should not be used
      Warning("BTaggingTool::getInefficiency()", "Jet is out of the validity range of the BTaggingEfficiencyTool! "
              "Please exercise caution using this efficiency. Contact expert if unsure how to proceed. "
              "jet details: eta %f, pt %f, eff %f", jet.eta(), jet.pt(), eff);
    }
    else if (status == CorrectionCode::Error) {
      // something went wrong - print error message
      Error("BTaggingTool::getInefficiency()", "tool returned CP::CorrectionCode::Error");
      return 0.;
    }

    // All is okay
    return eff;
  }
  else {
    Error("BTaggingTool::getInefficiency()", "efficiency tool is not initialized for "
          "%s (tagger), %s (jet author), %s (working point), and %s (efficiency scheme)!",
          btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.wp.c_str(), btc.scheme.c_str());
    return 0.0;
  }
} // getInefficiency

float BTaggingTool::getMCEfficiency (const xAOD::Jet &jet, std::string btc_name)
{
  auto btc = m_btc_map.at(btc_name);
  // check if tool is initialized
  if (auto tool = m_efficiencyTools[btc.tagger][btc.jetAuthor][btc.wp]){

    if (m_MCIndex == 9999 || !tool->setMapIndex(getFlavorLabel(jet), m_MCIndex)) {
      Error("BTaggingTool::getMCEfficiency",
	    "Couldn't set MC/MC index properly. Results will be biasd. Flavor: %s, Index: %u",
	    getFlavorLabel(jet).c_str(), m_MCIndex);
    }

    // check eta of jet
    // the tool will initiate a core dump (!) if we feed a jet with
    // |eta| > 2.5 to it... it's a design choice, like it or not...
    if (fabs(jet.eta()) > 2.5) return 0.0;
    // get efficiency
    float eff;
    auto  status = tool->getMCEfficiency(jet, eff);

    // check return code
    if (status == CorrectionCode::OutOfValidityRange && m_printOutOfRangeWarnings) {
      // out of extrapolation range of tool (high jet pt) - result should not be used
      Warning("BTaggingTool::getMCEfficiency()", "Jet is out of the validity range of the BTaggingEfficiencyTool! "
              "Please exercise caution using this efficiency. Contact expert if unsure how to proceed. "
              "jet details: eta %f, pt %f, eff %f", jet.eta(), jet.pt(), eff);
    }
    else if (status == CorrectionCode::Error) {
      // something went wrong - print error message
      Error("BTaggingTool::getMCEfficiency()", "tool returned CP::CorrectionCode::Error");
      return 0.;
    }
    // All is okay
    return eff;
  }
  else {
    Error("BTaggingTool::getMCEfficiency()", "efficiency tool is not initialized for "
          "%s (tagger), %s (jet author), %s (working point), and %s (efficiency scheme)!",
          btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.wp.c_str(), btc.scheme.c_str());
    return 0.0;
  }
} // getMCEfficiency

float BTaggingTool::getScaleFactor (const xAOD::Jet &jet, std::string btc_name)
{
  auto  btc = m_btc_map.at(btc_name);

  if (auto tool = m_efficiencyTools[btc.tagger][btc.jetAuthor][btc.wp]){

    if (m_MCIndex == 9999 || !tool->setMapIndex(getFlavorLabel(jet), m_MCIndex)) {
      Error("BTaggingTool::getScaleFactor",
	    "Couldn't set MC/MC index properly. Results will be biasd. Flavor: %s, Index: %u",
	    getFlavorLabel(jet).c_str(), m_MCIndex);
    }
    // check eta of jet
    // the tool will initiate a core dump (!) if we feed a jet with
    // |eta| > 2.5 to it... it's a design choice, like it not...
    if (fabs(jet.eta()) > 2.5) return -1.0;

    // get scale factor
    float sf;
    auto status = CorrectionCode::OutOfValidityRange;
    if(btc.useContinuous){
      Analysis::CalibrationDataVariables m_variables;
      m_variables.jetAuthor = btc.jetAuthor;
      m_variables.jetPt = jet.pt(); // in MeV
      m_variables.jetEta = jet.eta();
      float tagger_weight = getTaggerWeight(jet, btc_name);

      m_variables.jetTagWeight = tagger_weight; // here you specify the MV1 or MV1c value of the jet
      status = tool->getScaleFactor(getFlavor(jet), m_variables, sf);
    }
    else
      status = tool->getScaleFactor(jet, sf);

    // check return code
    if (status == CorrectionCode::OutOfValidityRange && m_printOutOfRangeWarnings) {
      // out of extrapolation range of tool (high jet pt) - result should not be used
      Warning("BTaggingTool::getScaleFactor()", "Jet is out of the validity range of the BTaggingEfficiencyTool! "
              "Please exercise caution using this scale factor. Contact expert if unsure how to proceed. "
              "jet details: eta %f, pt %f, sf %f", jet.eta(), jet.pt(), sf);
    }
    else if (status == CP::CorrectionCode::Error) {
      // something went wrong - print error message
      Error("BTaggingTool::getScaleFactor()", "tool returned CP::CorrectionCode::Error");
      return 0.0;
    }

    // All is okay
    return sf;
  }
  else {
    Error("BTaggingTool::getScaleFactor()", "efficiency tool is not initialized for "
          "%s (tagger), %s (jet author), %s (working point), and %s (efficiency scheme)!",
          btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.wp.c_str(), btc.scheme.c_str());
    return 0.0;
  }
} // getScaleFactor

float BTaggingTool::getInefficiencyScaleFactor (const xAOD::Jet &jet, std::string btc_name)
{
  auto  btc = m_btc_map.at(btc_name);

  if (auto tool = m_efficiencyTools[btc.tagger][btc.jetAuthor][btc.wp]){

    if (m_MCIndex == 9999 || !tool->setMapIndex(getFlavorLabel(jet), m_MCIndex)) {
      Error("BTaggingTool::getInefficiencyScaleFactor",
	    "Couldn't set MC/MC index properly. Results will be biasd. Flavor: %s, Index: %u",
	    getFlavorLabel(jet).c_str(), m_MCIndex);
    }

    // check eta of jet
    // the tool will initiate a core dump (!) if we feed a jet with
    // |eta| > 2.5 to it... it's a design choice, like it not...
    if (fabs(jet.eta()) > 2.5) return -1.0;
    // get scale factor
    float sf;
    auto status = CorrectionCode::OutOfValidityRange;
    if(btc.useContinuous){
      Analysis::CalibrationDataVariables m_variables;
      m_variables.jetAuthor = btc.jetAuthor;
      m_variables.jetPt = jet.pt(); // in MeV
      m_variables.jetEta = jet.eta();
      float tagger_weight = getTaggerWeight(jet, btc_name);

      m_variables.jetTagWeight = tagger_weight; // here you specify the MV1 or MV1c value of the jet
      status = tool->getScaleFactor(getFlavor(jet), m_variables, sf); //The Continuous SF are modified, no need to use IneffSF
    }
    else
      status = tool->getInefficiencyScaleFactor(jet, sf);
    // check return code
    if (status == CorrectionCode::OutOfValidityRange && m_printOutOfRangeWarnings) {
      // out of extrapolation range of tool (high jet pt) - result should not be used
      Warning("BTaggingTool::getInefficiencyScaleFactor()", "Jet is out of the validity range of the BTaggingEfficiencyTool! "
              "Please exercise caution using this scale factor. Contact expert if unsure how to proceed. "
              "jet details: eta %f, pt %f, sf %f", jet.eta(), jet.pt(), sf);
    }
    else if (status == CP::CorrectionCode::Error) {
      // something went wrong - print error message
      Error("BTaggingTool::getInefficiencyScaleFactor()", "tool returned CP::CorrectionCode::Error");
      return 0.0;
    }

    // All is okay
    return sf;
  }
  else {
    Error("BTaggingTool::getInefficiencyScaleFactor()", "efficiency tool is not initialized for "
          "%s (tagger), %s (jet author), %s (working point), and %s (efficiency scheme)!",
          btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.wp.c_str(), btc.scheme.c_str());
    return 0.0;
  }
} // getInefficiencyScaleFactor

float BTaggingTool::getFullEfficiency (const xAOD::Jet &jet, bool tagged, std::string btc_name)
{
  float eff = 0.0;

  if (tagged) getEfficiency(jet, eff, btc_name);
  else getInefficiency(jet, eff, btc_name);
  return eff;
} // getFullEfficiency

float BTaggingTool::getFullScaleFactor (const xAOD::Jet &jet, bool tagged, std::string btc_name)
{
  float sf = 1.0;

  if (tagged) getScaleFactor(jet, sf, btc_name);
  else getInefficiencyScaleFactor(jet, sf, btc_name);
  return sf;
} // getFullScaleFactor

void BTaggingTool::getScaleFactor (const xAOD::JetContainer &jets, std::string btc_name)
{
  // loop over jets and attach scale factor as decoration
  for (const xAOD::Jet *jet : jets) {
    // attach scale factor
    BTagProps::SF.set(jet, getScaleFactor(*jet, btc_name));
  }
} // getScaleFactor

int BTaggingTool::getQuantile (const xAOD::Jet &jet, std::string btc_name) 
{

  //////////////////////
  // Cheatsheet:
  // returns 5 if between 60% and 0%
  // returns 4 if between 70% and 60%
  // returns 3 if between 77% and 70%
  // returns 2 if between 85% and 77%
  // returns 1 if between 100% and 85%
  //////////////////////
  auto  btc = m_btc_map.at(btc_name);
  auto seltool = m_selectionTools[btc.tagger][btc.jetAuthor][btc.wp];

  if(!seltool){
    Warning("BTaggingTool::getQuantile()", "selection tool is not initialized for "
	    "%s (tagger), %s (jet author), and %s (working point)!",
	    btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.wp.c_str());
  }
  
  float tagweight = getTaggerWeight(jet, btc_name);
  
  if(btc.scheme.find("FixedCut")!= string::npos ){
    for(unsigned int i=0; i< btc.binEdges.size(); i++){
      if(btc.binEdges[i] > tagweight){
	return i+1;
      }
    }
    //if we got here, the jet is tagged in the 60% WP
    return 5;
  }
  else if(btc.scheme.find("Continuous")!= string::npos){
    return seltool->getQuantile(jet.pt(), jet.eta(), tagweight);
  }else{
    //only fixed cut and continuous WP supported in getQuantile
    return -1;
  }

  Error("BTaggingTool::getQuantile()", "problem retrieving the selection tool. Exiting.");
  return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//MC Index functions:

BTaggingTool& BTaggingTool::setMCIndex (const char *fileName, XSectionProvider &xSectionProvider)
{
  for (auto &p : m_flavorDSID) {
    auto itoken = 0;
    auto tokens = Analysis::tokenize(p.second, ';');
    for (auto &token : tokens) {
      auto index = this->indexMCEfficiencyFromChannel(std::stoul(token), xSectionProvider);
      if (index != m_defaultMCIndex) m_validMCIndicies.insert(index);
      m_actualMC2MCIndex[index] = itoken;
      ++itoken;
    }
  }
  auto index = this->indexMCEfficiencyFromFileName(fileName);

  if (m_validMCIndicies.count(index) == 0)
    Warning("BTaggingTool::setMCIndex", "unable to locate corresponding MC index %u"
	    " - will likely cause a segmentation violation", index);
  m_MCIndex = m_actualMC2MCIndex[index];
  return *this;
}

BTaggingTool& BTaggingTool::setMCIndex (const int &mcChannel, XSectionProvider &xSectionProvider)
{
  if(m_reference_file_type == CDI_FileType::VHcc_Custom_CDI ||
     m_reference_file_type == CDI_FileType::OFFICIAL_JUL_2019 ||
     m_reference_file_type == CDI_FileType::OFFICIAL_OCT_2019 ||
     m_reference_file_type == CDI_FileType::OFFICIAL_MAR_2020 ||
     m_reference_file_type == CDI_FileType::OFFICIAL_DEC_2020 ||
     m_reference_file_type == CDI_FileType::OFFICIAL_APR_2021) {
    if (!xSectionProvider.hasMCchannel(mcChannel)){
      m_MCIndex = 0;
    }else{
      auto description = xSectionProvider.getSampleDetailName(mcChannel);
      m_MCIndex = get_MC_index_from_string(description, m_defaultMCIndex);
    }
  }
  else{
    Warning("BTaggingTool::setMCIndex", "Reference file type is UNKNOWN "
	    " - Please use a valid CDI file.");
    }

  return *this;
}


unsigned BTaggingTool::indexMCEfficiencyFromFileName (const std::string &name)
{
  return get_MC_index_from_string(name, m_defaultMCIndex);
} // indexMCEfficiencyFromFileName


unsigned BTaggingTool::indexMCEfficiencyFromChannel (const int &mcChannel, XSectionProvider &xSectionProvider)
{
  if (!xSectionProvider.hasMCchannel(mcChannel)) return m_defaultMCIndex;
  auto description = xSectionProvider.getSampleDetailName(mcChannel);

  return get_MC_index_from_string(description, m_defaultMCIndex);
}

// local helper function that returns an (arbitrary) integer to represent an MC
// generator given a string description
unsigned BTaggingTool::get_MC_index_from_string (const std::string &name,
						 const unsigned &defaultMCIndex)
{
  if(m_reference_file_type == CDI_FileType::VHcc_Custom_CDI
     || m_reference_file_type == CDI_FileType::OFFICIAL_JUL_2019
     || m_reference_file_type == CDI_FileType::OFFICIAL_OCT_2019
     || m_reference_file_type == CDI_FileType::OFFICIAL_MAR_2020
     || m_reference_file_type == CDI_FileType::OFFICIAL_DEC_2020
     || m_reference_file_type == CDI_FileType::OFFICIAL_APR_2021){

    //the avaliable maps are 410501;410250;410558 or 410501;364156;410558
    // Pythia 8
    if ((name.find("Pythia8_") != string::npos) ||
        (name.find("Pythia8B_") != string::npos) ||
        (name.find("MGPy8EG_") != string::npos) ||
        (name.find("MadGraphPythia8_") != string::npos) ||
        (name.find("MadGraphPythia8EvtGen_") != string::npos) ||
        (name.find("PowhegPythia8EvtGen_CT10_") != string::npos) ||
        (name.find("PowhegPythia8EvtGen_") != string::npos) ||
        (name.find("Pythia8EvtGen_") != string::npos) ||
        (name.find("Pythia8EG_") != string::npos) ||
        (name.find("PwPy8EG_CT10_") != string::npos) ||
        (name.find("PowhegPythia8_") != string::npos) ||
        (name.find("PowhegPythia8EG_") != string::npos) ||
        (name.find("PhPy8EG_CT10_") != string::npos) ||
        (name.find("Py8EG_") != string::npos) ||
	(name.find("Py8") != string::npos)) { return 0; }

    // Sherpa 2.2.1 or 2.2.2
    if ((name.find("Sh_221_") != string::npos) ||
        (name.find("Sherpa_NNPDF30NNLO") != string::npos) ||
        (name.find("Sherpa_222_NNPDF30NNLO") != string::npos) ||
        (name.find("Sherpa_221_NNPDF30NNLO") != string::npos)){ return 1; }

    //Herwig 7.0.4
    if ((name.find("Herwig") != string::npos) &&
       !(name.find("PowhegHerwig7EvtGen_tt") != string::npos) &&
       !(name.find("PowhegHerwig7EvtGen_NNPDF3") != string::npos)){ return 2; }

    //aMCAtNLO
    if ((name.find("aMcAtNloPy8EvtGen_") != string::npos) ){ return 3; }

    //Herwig 7.1.3
    if ((name.find("PowhegHerwig7EvtGen_tt") != string::npos) ||
        (name.find("PowhegHerwig7EvtGen_NNPDF3") != string::npos)){
       if(m_reference_file_type == CDI_FileType::OFFICIAL_APR_2021) return 4;
       else return 2;
    }

    //Sherpa 2.2.8
    if ((name.find("Sh_228_") != string::npos)  ||
        (name.find("Sherpa_228_") != string::npos)){
       if(m_reference_file_type == CDI_FileType::OFFICIAL_APR_2021) return 5;
       else return 1;
    }

    //Sherpa 2.2.10 and Sherpa 2.2.11
    //(Apply Sherpa 2.2.10 maps to both)
    if ((name.find("Sh_2210_") != string::npos)  ||
        (name.find("Sherpa_2210_") != string::npos) ||
	(name.find("Sh_2211_") != string::npos)  ||
        (name.find("Sherpa_2211_") != string::npos)){
       if(m_reference_file_type == CDI_FileType::OFFICIAL_APR_2021) return 6;
       else return 1;
    }

    //Herwig 7.2.1
    if ((name.find("PhH7EG_H7UE") != string::npos) ){
       if(m_reference_file_type == CDI_FileType::OFFICIAL_APR_2021) return 7;
       else return 2;
    }

    //for all other samples, use the default map (pythia8).
    return 0;
  }
  else{
    Error("setMCIndex()", "This CDI is NOT supported. Please update it."); 
  }

  return defaultMCIndex;
}

/////////////////////////////////////////////////////////////////////////////////////
//functions to initialize the Tools:

StatusCode BTaggingTool::initializeEffTool (std::shared_ptr<BTaggingEfficiencyTool> &eff_ptr, const BTagConfig& btc, bool is_veto)
{
  // TODO CDI ignores this
  MSG::Level m_msgLevel = MSG::WARNING;
  std::string tagger = is_veto ? btc.taggerName_veto : btc.tagger;
  std::string wp = is_veto ? btc.wp_veto : btc.wp;
  std::string jetAuthor = btc.jetAuthor;
  //add here some other settings if needed

  eff_ptr = std::make_shared<BTaggingEfficiencyTool>(
						     Form("BTaggingEfficiencyTool.%s.%s.%s", tagger.c_str(), jetAuthor.c_str(), wp.c_str()));

  TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("TaggerName", tagger.c_str()));
  TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("OperatingPoint", wp.c_str()));
  TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("JetAuthor", jetAuthor.c_str()));
  TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("ScaleFactorFileName", m_fileNameCDI.c_str()));
  TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("ConeFlavourLabel", true));
  TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("OldConeFlavourLabel", false));
  if (m_flavorDSID.count("B") != 0) TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("EfficiencyBCalibrations", m_flavorDSID["B"]));
  if (m_flavorDSID.count("C") != 0) TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("EfficiencyCCalibrations", m_flavorDSID["C"]));
  if (m_flavorDSID.count("T") != 0) TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("EfficiencyTCalibrations", m_flavorDSID["T"]));
  if (m_flavorDSID.count("Light") != 0) TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("EfficiencyLightCalibrations", m_flavorDSID["Light"]));

  TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("EigenvectorReductionB", btc.rScheme));
  TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("EigenvectorReductionC", btc.rScheme));
  TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("EigenvectorReductionLight", btc.rScheme));

  //Add minPt requirements for Jets https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTagCalibrationRecommendationsRelease21
  if(jetAuthor.find("EMPFlow")!= std::string::npos){
    TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("MinPt", 20e3));
  }

  if(jetAuthor.find("EMTopo")!= std::string::npos){
    TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("MinPt", 20e3));
  }

  if(jetAuthor.find("VRTrack")  != std::string::npos ||
     // for AntiKtVR30Rmax4Rmin02TrackJets
     (jetAuthor.find("AntiKtVR")  != std::string::npos &&
      jetAuthor.find("TrackJets") != std::string::npos)) {
    TOOL_CHECK("BTaggingTool::initializeEffTool()", eff_ptr->setProperty("MinPt", 10e3));
  }

  Info("BTaggingTool::initializeEffTool", "Initializing efficiency tool: %s, %s, %s", tagger.c_str(), jetAuthor.c_str(), wp.c_str());
  eff_ptr->msg().setLevel(m_msgLevel);  

  EL_CHECK("BTaggingTool::initialize()", eff_ptr->initialize());

  return StatusCode::SUCCESS;
} // initializeEffTool

StatusCode BTaggingTool::initializeTruthTagTool (std::shared_ptr<BTaggingTruthTaggingTool> &tt_ptr, const BTagConfig& btc, bool is_veto)
{

  // TODO CDI ignores this
  MSG::Level m_msgLevel = MSG::WARNING;

  std::string tagger = is_veto ? btc.taggerName_veto : btc.tagger;
  std::string wp = is_veto ? btc.wp_veto : btc.wp;
  std::string jetAuthor = btc.jetAuthor;

  //add here some other settings if needed

  tt_ptr = std::make_shared<BTaggingTruthTaggingTool>(
						      Form("BTaggingTruthTaggingTool.%s.%s.%s", tagger.c_str(), jetAuthor.c_str(), wp.c_str()));

  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("TaggerName", tagger.c_str()));
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("OperatingPoint", wp.c_str()));
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("JetAuthor", jetAuthor.c_str()));
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("ScaleFactorFileName", m_fileNameCDI.c_str()));
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("ConeFlavourLabel", true));
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("OldConeFlavourLabel", false));
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("EigenvectorReductionB", btc.rScheme));
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("EigenvectorReductionC", btc.rScheme));
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("EigenvectorReductionLight", btc.rScheme));
  //TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("EigenvectorReductionLight", m_rScheme));

  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("IgnoreScaleFactors",false));
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("UsePermutations", true));
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("UseQuantile", btc.useQuantile ));
    
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("UseSystematics", true ));
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("MaxNtagged", btc.maxTruthTag ) );
  TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("ExcludeSpecificEigens", btc.ExcludeEV));

  if(btc.wp == "Continuous"){ //working point for the selection
    TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("CutBenchmark", Form("FixedCutBEff_%s", btc.btagcuteff.c_str())));
    TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("StoreOnlyUpVariations", btc.useReducedSetOfBTagEV));
  }
  if (m_flavorDSID.count("B") != 0) TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("EfficiencyBCalibrations", m_flavorDSID["B"]));
  if (m_flavorDSID.count("C") != 0) TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("EfficiencyCCalibrations", m_flavorDSID["C"]));
  if (m_flavorDSID.count("T") != 0) TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("EfficiencyTCalibrations", m_flavorDSID["T"]));
  if (m_flavorDSID.count("Light") != 0) TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("EfficiencyLightCalibrations", m_flavorDSID["Light"]));


  //Add minPt requirements for Jets https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTagCalibrationRecommendationsRelease21
  if(jetAuthor.find("EMPFlow")!= std::string::npos){
    TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("MinPt", 20e3));
  }

  if(jetAuthor.find("EMTopo")!= std::string::npos){
    TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("MinPt", 20e3));
  }

  if(jetAuthor.find("VRTrack")  != std::string::npos ||
     // for AntiKtVR30Rmax4Rmin02TrackJets
     (jetAuthor.find("AntiKtVR")  != std::string::npos &&
      jetAuthor.find("TrackJets") != std::string::npos)) {
    TOOL_CHECK("BTaggingTool::initializeTruthTagTool()", tt_ptr->setProperty("MinPt", 10e3));
  }

  Info("BTaggingTool::initializeTruthTagTool()", "Initializing truthtagging tool: %s, %s, %s", tagger.c_str(), jetAuthor.c_str(), wp.c_str());

  tt_ptr->msg().setLevel(m_msgLevel);
  EL_CHECK("BTaggingTool::initialize()", tt_ptr->initialize());

  return StatusCode::SUCCESS;
} // initializeTruthTagTool



StatusCode BTaggingTool::initializeSelectTool (std::shared_ptr<BTaggingSelectionTool> &select_ptr, const BTagConfig& btc, bool is_veto)
{
  // TODO CDI ignores this
  MSG::Level m_msgLevel = MSG::WARNING;

  std::string tagger = is_veto ? btc.taggerName_veto : btc.tagger;
  std::string wp = is_veto ? btc.wp_veto : btc.wp;
  std::string jetAuthor = btc.jetAuthor;
  //add here some other settings if needed

  select_ptr = std::make_shared<BTaggingSelectionTool>(
						       Form("BTaggingSelectionTool.%s.%s.%s", tagger.c_str(), jetAuthor.c_str(), wp.c_str()));

  // auto GeV = 1000.0;

  TOOL_CHECK("BTaggingTool::initializeSelectTool()", select_ptr->setProperty("TaggerName", tagger.c_str()));
  TOOL_CHECK("BTaggingTool::initializeSelectTool()", select_ptr->setProperty("OperatingPoint", wp.c_str()));
  TOOL_CHECK("BTaggingTool::initializeSelectTool()", select_ptr->setProperty("JetAuthor", jetAuthor.c_str()));
  TOOL_CHECK("BTaggingTool::initializeSelectTool()", select_ptr->setProperty("FlvTagCutDefinitionsFileName", m_fileNameCDI.c_str()));

  //Add minPt requirements for Jets https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTagCalibrationRecommendationsRelease21
  if(jetAuthor.find("EMPFlow")!= std::string::npos){
    TOOL_CHECK("BTaggingTool::initializeSelectTool()", select_ptr->setProperty("MinPt", 20e3));
    TOOL_CHECK("BTaggingTool::initializeSelectTool()", select_ptr->setProperty("MaxEta", 2.5));
  }

  if(jetAuthor.find("EMTopo")!= std::string::npos){
    TOOL_CHECK("BTaggingTool::initializeSelectTool()", select_ptr->setProperty("MinPt", 20e3));
    TOOL_CHECK("BTaggingTool::initializeSelectTool()", select_ptr->setProperty("MaxEta", 2.5));
  }

  if(jetAuthor.find("VRTrack")  != std::string::npos ||
     // for AntiKtVR30Rmax4Rmin02TrackJets
     (jetAuthor.find("AntiKtVR")  != std::string::npos &&
      jetAuthor.find("TrackJets") != std::string::npos) ){
    TOOL_CHECK("BTaggingTool::initializeSelectTool()", select_ptr->setProperty("MinPt", 10e3));
    TOOL_CHECK("BTaggingTool::initializeSelectTool()", select_ptr->setProperty("MaxEta", 2.5));
  }

  Info("BTaggingTool::initializeSelectTool", "Initializing selection tool: %s, %s, %s", tagger.c_str(), jetAuthor.c_str(), wp.c_str());

  select_ptr->msg().setLevel(m_msgLevel);
  EL_CHECK("BTaggingTool::initializeSelectTool()", select_ptr->initialize());

  return StatusCode::SUCCESS;
} // initializeSelectTool


void BTaggingTool::selectTools (const BTagConfig& btc, bool is_for_veto)
{

  std::string tagger = is_for_veto ? btc.taggerName_veto : btc.tagger;
  std::string wp = is_for_veto ? btc.wp_veto : btc.wp;  
  std::string jetAuthor = btc.jetAuthor;

  //Efficiency tool
  if(!m_efficiencyTools[tagger][jetAuthor][wp]){
    auto status = initializeEffTool(m_efficiencyTools[tagger][jetAuthor][wp], btc, is_for_veto);
    if (status != StatusCode::SUCCESS) {
      Error("BTaggingTool::selectTools()", "couldn't initialize requested efficiency tool");
      PrintConfiguration(btc);
    }
  }

  //Selection Tool
  if(!m_selectionTools[tagger][jetAuthor][wp]){
    auto status = initializeSelectTool(m_selectionTools[tagger][jetAuthor][wp],btc, is_for_veto);
    if (status != StatusCode::SUCCESS) {
      Error("BTaggingTool::selectTools()", "couldn't initialize requested selection tool");
      PrintConfiguration(btc);
    }
  }

    //Truth Tagging Tool
  if(btc.doTruthTagging && !m_TruthTaggingTools[tagger][jetAuthor][wp]){
    auto status = initializeTruthTagTool(m_TruthTaggingTools[tagger][jetAuthor][wp],btc, is_for_veto);
    if (status != StatusCode::SUCCESS) {
      Error("BTaggingTool::selectTools()", "couldn't initialize requested truthtagging tool");
      PrintConfiguration(btc);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////
//Truth Tagging Functions:

void BTaggingTool::truth_tag_jets(unsigned long long eventNumber,const xAOD::JetContainer &signalJets, bool doSyst, std::string btc_name){
  std::vector<const xAOD::Jet*> temp;
  for (auto jet : signalJets) temp.push_back(jet);
  return truth_tag_jets(eventNumber,temp, doSyst, btc_name);
}

void BTaggingTool::truth_tag_jets(unsigned long long eventNumber,const std::vector<const xAOD::Jet*> &signalJets, bool doSyst, std::string btc_name){
  
  auto  btc = m_btc_map.at(btc_name);

  if(m_debug)
    btc.print();

  std::string tagger = btc.tagger;
  std::string jetAuthor = btc.jetAuthor;
  std::string wp = btc.wp;
  bool exclusive = btc.exclusive; ; //exactly n-required tags, or inclusive, n or more tags.

  if (auto tool = m_TruthTaggingTools[tagger][jetAuthor][wp]) {
    if (m_MCIndex == 9999 || !tool->setEffMapIndex("B", m_MCIndex) || !tool->setEffMapIndex("C", m_MCIndex)
	|| !tool->setEffMapIndex("Light", m_MCIndex) || !tool->setEffMapIndex("T", m_MCIndex)) {
      Error("BTaggingTool::truth_tag_jets",
	    "Couldn't set MC/MC index properly. Results will be biasd. Index: %u", m_MCIndex);
    }

    if (doSyst) {
      tool->setUseSystematics(true);
    }else{
      tool->setUseSystematics(false);
    }

    //Set the jets to be used in the tool.
    m_truthtag_jets[jetAuthor].clear();
    m_truthtag_jets_groupB[jetAuthor].clear();

    SelectJetsForTruthTagging(m_truthtag_jets[jetAuthor], m_truthtag_jets_groupB[jetAuthor],
			      signalJets, btc);

    int randomseed = (int)eventNumber;
   
    StatusCode status = tool->CalculateResults( m_truthtag_jets[jetAuthor].pt,m_truthtag_jets[jetAuthor].eta,
						m_truthtag_jets[jetAuthor].flav,m_truthtag_jets[jetAuthor].tagw, m_truthtag_jets[jetAuthor].tt_results,randomseed);

    if(status != StatusCode::SUCCESS){
      Error("BTaggingTool::truth_tag_jets","failed to compute truth tag results");
    }

    if(m_truthtag_jets_groupB[jetAuthor].size() > 0){
      status = tool->CalculateResults( m_truthtag_jets_groupB[jetAuthor].pt,m_truthtag_jets_groupB[jetAuthor].eta,
				       m_truthtag_jets_groupB[jetAuthor].flav,m_truthtag_jets_groupB[jetAuthor].tagw, m_truthtag_jets_groupB[jetAuthor].tt_results,randomseed);

      if(status != StatusCode::SUCCESS){
	Error("BTaggingTool::truth_tag_jets","failed to compute truth tag results");
      }
    }

    //decorate the jets with the results of the chosen permutations and randomly generated tag weights
    m_hybrid_jets[btc.jetAuthor].clear();
    for(unsigned int ijet=0; ijet< signalJets.size();ijet++){
      BTagProps::eff.set(signalJets[ijet],getEfficiency(*signalJets[ijet], btc_name));

      bool hybridjet = false;
      bool istagged = false;
      float tagweight = -99;
      int quantile = -1;

      if(m_truthtag_jets[jetAuthor].hasJetIndex(ijet)){
	istagged = m_truthtag_jets[jetAuthor].istagged(ijet,exclusive);
	if(btc.useQuantile){
	  tagweight = m_truthtag_jets[jetAuthor].tagweight(ijet,exclusive);
	  quantile = m_truthtag_jets[jetAuthor].quantile(ijet,exclusive);
	}
      }else if(m_truthtag_jets_groupB[jetAuthor].hasJetIndex(ijet)){
	istagged = m_truthtag_jets_groupB[jetAuthor].istagged(ijet,exclusive);
	if(btc.useQuantile){
	  tagweight = m_truthtag_jets_groupB[jetAuthor].tagweight(ijet,exclusive);
	  quantile = m_truthtag_jets_groupB[jetAuthor].quantile(ijet,exclusive);
	}
      }else{
	//if we got here, these must be b-jets and we must be in hybrid mode
	//so just give the jets their "real" properties
	istagged = isTagged(*signalJets[ijet], btc_name);
	if(btc.useQuantile){
	  tagweight = getTaggerWeight(*signalJets[ijet], btc_name);
	  quantile = getQuantile(*signalJets[ijet], btc_name);
	}
	
	hybridjet = true;
      }

      BTagProps::isTagged.set(signalJets[ijet],istagged);
      BTagProps::tagWeight.set(signalJets[ijet],tagweight);
      BTagProps::Quantile.set(signalJets[ijet],quantile);

      if(hybridjet){  m_hybrid_jets[jetAuthor].push_back(signalJets[ijet]); }

    }

  }else {
    Error("BTaggingTool::truth_tag_jets()", "truth tag tool is not initialized for "
          "%s (tagger), %s (jet author), %s (working point), and %s (efficiency scheme)!",
          tagger.c_str(), jetAuthor.c_str(), wp.c_str(), btc.scheme.c_str());
  }
}


void BTaggingTool::SelectJetsForTruthTagging(TruthTagJets& tt_jets, TruthTagJets& tt_jets_groupB,
                                             const std::vector<const xAOD::Jet*> &signalJets, const BTagConfig& btc) const {
  
  int n_RequiredTruthTags= btc.requiredTT;
  std::string strategy= btc.strategy; //"AllSignalJets";
  bool doHybridTruthTagging = btc.doHybridTagging; //in hybrid mode b-jets are kept "as they are" without applying truth tagging.
  bool exclusive = btc.exclusive; //true; //exactly n-required tags, or inclusive, n or more tags.
  TaggerType taggerEnum = btc.taggerEnum;
  bool useContinuous = btc.useContinuous;
  std::string trsh = btc.btagcuteff;

  std::string tagger = btc.tagger;
  std::string jetAuthor = btc.jetAuthor;
  std::string wp = btc.wp;


  auto seltool = m_selectionTools.at(tagger).at(jetAuthor).at(wp);
  if(!seltool){
    Error("BTaggingTool::isTagged()", "selection tool is not initialized for "
	    "%s (tagger), %s (jet author), and %s (working point)!",
            tagger.c_str(), jetAuthor.c_str(), wp.c_str());
  }

  if(strategy=="AllSignalJets"){
    int n_tagged_bjets=0;

    for(unsigned int ijet=0; ijet< signalJets.size();ijet++){
      int label = getFlavor(*signalJets[ijet]);

      if( doHybridTruthTagging && fabs(label)==5 ){
        if(isJetTagged(*signalJets[ijet], seltool, taggerEnum, useContinuous, trsh)){ n_tagged_bjets++; }
        continue;
      }

      tt_jets.pt.push_back(signalJets[ijet]->pt());
      tt_jets.eta.push_back(signalJets[ijet]->eta());
      tt_jets.flav.push_back(label);
      tt_jets.tagw.push_back(getJetTaggerWeight(*signalJets[ijet], seltool, taggerEnum)); //the tag weight is not needed for truth tagging, just for direct tagging.
      tt_jets.signaljet_index.push_back(ijet);
    }

    if(!doHybridTruthTagging){
      tt_jets.n_requiredTags =   n_RequiredTruthTags<= tt_jets.size() ? n_RequiredTruthTags : tt_jets.size();
    }

    if(doHybridTruthTagging){
      int required = n_RequiredTruthTags-n_tagged_bjets;
      if( required < 0 ){ required = 0; }
      tt_jets.n_requiredTags =   required<= tt_jets.size() ? required : tt_jets.size();
    }

  } //AllSignalJets

  if(strategy=="Leading2SignalJets"){
    //deal with the 2 leading pt jets
    int ntags_leading2 = 0;

    for (unsigned int i = 0; i < signalJets.size(); i++)
      {
	if(i > 1){ break; }
	if( !(doHybridTruthTagging && getFlavorLabel(*signalJets[i])=="B") ){
	  int label = getFlavor(*signalJets[i]);
	  tt_jets.pt.push_back(signalJets[i]->pt());
	  tt_jets.eta.push_back(signalJets[i]->eta());
	  tt_jets.flav.push_back(label);
	  tt_jets.tagw.push_back(getJetTaggerWeight(*signalJets[i], seltool, taggerEnum)); //the tag weight is not needed for truth tagging, just for direct tagging.
	  tt_jets.signaljet_index.push_back(i);
	}else{
	  if( isJetTagged(*signalJets[i], seltool, taggerEnum, useContinuous, trsh) ){ ntags_leading2++; }
	}
      }

    int required = n_RequiredTruthTags;
    if(required > 2){ required =2; }
    required = required - ntags_leading2;
    if(required < 0){ required =0; }

    tt_jets.n_requiredTags =   required<= tt_jets.size() ? required : tt_jets.size();
    //if we are in the inclusive case, we don't care whats up with the rest of the jets, we can leave them as they are.
    if(!exclusive){ return; }
    if(signalJets.size() <=2 ){ return; }

    ntags_leading2 = ntags_leading2+ tt_jets.n_requiredTags;
    //in the exclusive case, we require the rest of the jets to have n_RequiredTruthTags-ntags_leading2
    //in hybrid mode, require those tags - n_tagged_bjets

    int n_tagged_bjets=0;

    for(unsigned int ijet=2; ijet< signalJets.size();ijet++){
      int label = getFlavor(*signalJets[ijet]);

      if( doHybridTruthTagging && fabs(label)==5 ){

        if(isJetTagged(*signalJets[ijet], seltool, taggerEnum, useContinuous, trsh)){ n_tagged_bjets++; }
        continue;
      }

      tt_jets_groupB.pt.push_back(signalJets[ijet]->pt());
      tt_jets_groupB.eta.push_back(signalJets[ijet]->eta());
      tt_jets_groupB.flav.push_back(label);
      tt_jets_groupB.tagw.push_back(getJetTaggerWeight(*signalJets[ijet], seltool, taggerEnum)); //the tag weight is not needed for truth tagging, just for direct tagging.
      tt_jets_groupB.signaljet_index.push_back(ijet);

    }

    required = n_RequiredTruthTags-ntags_leading2;
    if(doHybridTruthTagging){ required = required - n_tagged_bjets; }
    if(required < 0){ required = 0; }

    tt_jets_groupB.n_requiredTags =   required<= tt_jets_groupB.size() ? required : tt_jets_groupB.size();

  } //Leading2SignalJets
}

///////////////////////////////////////////////////////////////////////////////////////
// Retrieve Event Weight:

//DEPRECATED//
//will be soon be removed
std::map<std::string, float> BTaggingTool::computeEventWeight_old(const std::vector<const xAOD::Jet*> &xaodjets, bool doSyst, std::string btc_name){
  return computeEventWeight(xaodjets, doSyst, btc_name);
}

StatusCode BTaggingTool::computeEventWeight (xAOD::JetContainer &xaodjets, map<string, float>& btageffweights, bool doSyst, std::string btc_name)
{
  std::vector<const xAOD::Jet*> temp;
  //  map<string, float> btageffweights;

  for (auto jet : xaodjets) temp.push_back(jet);

  btageffweights = computeEventWeight(temp, doSyst, btc_name);

  if(btageffweights.empty())
    return StatusCode::FAILURE;
  
  return StatusCode::SUCCESS;
} // computeEventWeight

std::map<std::string, float> BTaggingTool::computeEventWeight (const vector<const xAOD::Jet*> &xaodjets, bool doSyst, std::string btc_name) 
{

  map<string, float> btageffweights;
  
  // Add nominal
  btageffweights["Nominal"] = 1.0;
  
  // always compute "Nominal" SF
  float nominal_sf = 1.0;
  
  for (auto jet : xaodjets) {
    float sf(1.0);
    bool tagged = BTagProps::isTagged.exists(jet) ?
      static_cast<bool>(BTagProps::isTagged.get(jet)) :
      isTagged(*jet, btc_name);
    
    sf = getFullScaleFactor(*jet, tagged, btc_name);
    if (sf == -1.0) continue;
    nominal_sf*= sf;
  }
  
  btageffweights["Nominal"] = nominal_sf;
  // optionally do systematics
  if (doSyst) {
    auto  btc = m_btc_map.at(btc_name);    
    auto efftool = m_efficiencyTools.at(btc.tagger).at(btc.jetAuthor).at(btc.wp);
    if(!efftool){
      Error("BTaggingTool::computeEventWeight()", "efficiency tool is not initialized for "
	    "%s (tagger), %s (jet author), and %s (working point)!",
	    btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.wp.c_str());
    }

    SetMCIndex(efftool); //Make sure the MC Index is set correctly.
    
    SystematicSet affectingSystematics = efftool->affectingSystematics();
    
    for (auto& var : affectingSystematics) {
      if(btc.useReducedSetOfBTagEV){
	if(var.name().find("__1down") !=string::npos) continue;
	if(btc.ExcludeEV.find(var.name())!=string::npos) continue;
      }
      
      auto name = Analysis::replace_all(var.name(), " ", "_");
      SystematicSet set;
      
      set.insert(var);
      auto sresult = efftool->applySystematicVariation(set);
      if (sresult != StatusCode::SUCCESS) continue;
      
      // Set all weights to 1
      btageffweights[name] = 1.0;
      float tmp_sf = 1.0;
      
      for (auto jet : xaodjets) {
	float sf(1.0);
	
	bool tagged = BTagProps::isTagged.exists(jet) ?
	  static_cast<bool>(BTagProps::isTagged.get(jet)) :
	  isTagged(*jet, btc_name);
	
	sf = getFullScaleFactor(*jet, tagged, btc_name);
	if (sf == -1.0) continue;
	tmp_sf *= sf;
      }
      btageffweights[name] = tmp_sf;
    }

    // revert to nominal
    SystematicSet default_set;
    auto result = efftool->applySystematicVariation(default_set);
    if (result != StatusCode::SUCCESS){
      Error("failed to restore nominal setting in efficiency tool: ","compute_EventWeight");
    }
  }
  return btageffweights;
} // computeEventWeight


StatusCode BTaggingTool::compute_TruthTag_EventWeight (std::map<std::string, float> btageffweights, bool doSyst, std::string btc_name){
  btageffweights.clear();
  btageffweights = compute_TruthTag_EventWeight(doSyst, btc_name);

  if(btageffweights.empty())
    return StatusCode::FAILURE;
  
  return StatusCode::SUCCESS;
}

std::map<std::string, float> BTaggingTool::compute_TruthTag_EventWeight (bool doSyst, std::string btc_name){

  auto btc = m_btc_map.at(btc_name);

  bool exclusive = btc.exclusive; //exactly n-required tags, or inclusive, n or more tags.
  std::string tagger = btc.tagger;
  std::string jetAuthor = btc.jetAuthor;
  std::string wp = btc.wp;
  std::map<std::string, float> btageffweights;

  map<string, float> DirectTagEffweights; //for use with hybrid truth tagging

  if(m_hybrid_jets[jetAuthor].size() > 0){ // remove if(doHybridTruthTagging) for boosted TT
    DirectTagEffweights = computeEventWeight(m_hybrid_jets[jetAuthor], doSyst, btc_name);
  }
  // Add nominal
  btageffweights["Nominal"] = m_truthtag_jets[jetAuthor].getEventWeight(exclusive,"Nominal")*m_truthtag_jets_groupB[jetAuthor].getEventWeight(exclusive,"Nominal");

  if(m_hybrid_jets[jetAuthor].size() > 0){
    btageffweights["Nominal"] = btageffweights["Nominal"]*DirectTagEffweights["Nominal"];
  }

  // optionally do systematics
  if(doSyst){

    auto efftool = m_efficiencyTools.at(tagger).at(jetAuthor).at(wp);
    if(!efftool){
      Error("BTaggingTool::computeEventWeight()", "efficiency tool is not initialized for "
            "%s (tagger), %s (jet author), and %s (working point)!",
            tagger.c_str(), jetAuthor.c_str(), wp.c_str());
    }

    SetMCIndex(efftool); //Make sure the MC Index is set correctly.

    SystematicSet affectingSystematics = efftool->affectingSystematics();
      for (auto& var : affectingSystematics) {
        if(btc.useReducedSetOfBTagEV){
          if(var.name().find("__1down") !=string::npos) continue;
          if(btc.ExcludeEV.find(var.name())!=string::npos) continue;
        }
	
	//fetch the systematic
        SystematicSet set;
        auto name = Analysis::replace_all(var.name(), " ", "_");
        set.insert(var);
        auto sresult = efftool->applySystematicVariation(set);

	//if not retrieved properly, set the weight to Nominal (an okay default.....)
	//remember, this is both event weight and scale factors
	if (sresult != StatusCode::SUCCESS){
	  btageffweights[name] = btageffweights["Nominal"];
	  continue;
	}
        btageffweights[name] = m_truthtag_jets[jetAuthor].getEventWeight(exclusive,name)*m_truthtag_jets_groupB[jetAuthor].getEventWeight(exclusive,name);
        if(m_hybrid_jets[jetAuthor].size() > 0){
          btageffweights[name] = btageffweights[name]*DirectTagEffweights[name];
        }
      }
      
      // revert to nominal
      SystematicSet default_set;
      auto sresult = efftool->applySystematicVariation(default_set);
      if (sresult != StatusCode::SUCCESS){
	Error("failed to restore nominal setting in efficiency tool: ","compute_EventWeight_TruthTag");
      }
  }
  return btageffweights;
} // computeEventWeight_truthTag


///////////////////////////////////////////////////////////////////////////////////////
//VHbb related functions:
// function to set the btagging decision and decorate the jets
// shared among Merged and resolved regimes
StatusCode BTaggingTool::compute_btag_info(unsigned long long eventNumber, 
					   std::vector<const xAOD::Jet *> &signalJets,
					   bool isMC, bool doSyst, std::string btc_name) {
  
  auto btc = m_btc_map.at(btc_name);
  int nSignalJet = signalJets.size();

  //First Tag the jets:  do DT or TT tagging?
  if (isMC && btc.doTruthTagging) { //includes Hybrid tag
    truth_tag_jets(eventNumber, signalJets, doSyst, btc_name);
  }

  else{ //Direct Tag all jets if TT is not specified
    for (int idx=0; idx<nSignalJet; ++idx) {
      const xAOD::Jet *Jet = signalJets.at(idx);
      
      if (isMC && (btc.tagAlgorithm == "FlavLabel")){
	isTagged_UsingTruthFlavLabel(Jet);}
      else{
	BTagProps::isTagged.set( Jet, isTagged(*Jet, btc_name));
	BTagProps::tagWeight.set(Jet, getTaggerWeight(*Jet, btc_name));
	BTagProps::Quantile.set( Jet, getQuantile(*Jet, btc_name));
	if (isMC)
	  BTagProps::eff.set(Jet, getEfficiency(*Jet, btc_name));
	else
	  BTagProps::eff.set(Jet, -999.);
      }
    }
  }
  return StatusCode::SUCCESS;
}

// supported strategies -->  std::string strategy: "AllSignalJets" "Leading2SignalJets"
// function to set the selected jets based on the btagging decision
// shared between merged and resolved + merged has an additional tagging outside the fatjet 
// handled in AnalysisReader_VHQQ::ComputeMergedBTagSelection
StatusCode BTaggingTool::get_tagjet_selection_vhbb(std::vector<const xAOD::Jet *> &signalJets,std::vector<const xAOD::Jet *> &selectedJets, 
						   int &tagcatExcl, std::string btc_name, const std::vector<const xAOD::Jet *> &forwardJets) {

  std::string strategy = m_btc_map.at(btc_name).strategy;
  int nSignalJet = signalJets.size();
  int nForwardJet = forwardJets.size();

  // no point in going further if there are no jets
  // (eg. for VR jets inside the FJ: they can be zero)
  if (nSignalJet == 0)
    return StatusCode::SUCCESS;

  selectedJets.clear();
  int nbtag = 0;
  tagcatExcl = -1;
  std::vector<int> tagIndex, untagIndex; //vector of tagged indices and untagged indices
  
  const xAOD::Jet *Jet1 = nullptr;
  const xAOD::Jet *Jet2 = nullptr;
  const xAOD::Jet *Jet3 = nullptr;
  const xAOD::Jet *Jet4 = nullptr;

  // jets that will be used to fill Jet3 and Jet4
  // the 3rd and 4th signal jets (if present) are included
  // + 1st and 2nd forward jets (if present)
  // leading two jets among the four will be Jet3 and Jet4
  std::vector<const xAOD::Jet*> jetsToSort;

  if (strategy == "AllSignalJets" || strategy == "Leading3SignalJets") {

    //first loop over all the jets and save the index as tagged or untagged
    //useful to compute also nbtag.
    if (strategy == "AllSignalJets"){
      for (int idx=0; idx<nSignalJet; ++idx) {
		const xAOD::Jet *Jet = signalJets.at(idx);
		if (BTagProps::isTagged.get(Jet)){
		  tagIndex.push_back(idx);
		  nbtag++;}
		else{
		  untagIndex.push_back(idx);}
      }//loop in signal jet
    } else {
      // leading3SignalJets
      for (int idx=0; idx<nSignalJet; ++idx) {
		const xAOD::Jet *Jet = signalJets.at(idx);
		if (BTagProps::isTagged.get(Jet) && idx < 3){
		  tagIndex.push_back(idx);
		  nbtag++;}
		else{
		  untagIndex.push_back(idx);}
      }//loop in signal jet      
    }
    tagcatExcl = nbtag;
    //create a vector of tagIndex: fill it first with tag and then untag
    std::vector<int> ti;
    ti.clear();
    // NB: vector selectedJets is built so that its first two jets are
    // the two jets we use to reconstruct the H candidate, ordered in pT
    if(tagcatExcl == 1 && !(BTagProps::isTagged.get(signalJets.at(0))) ){
      // for 1tag we store leading and then tagged jet IF they do not coincide
      if(nSignalJet >= 1) ti.push_back(untagIndex.at(0));
      if(nSignalJet >= 2) ti.push_back(tagIndex.at(0));
      if(nSignalJet >= 3) ti.push_back(untagIndex.at(1));
      if(nSignalJet >= 4) ti.push_back(untagIndex.at(2));
    }
    else { //fill it first with tag and then untag
      ti = tagIndex; 
      ti.insert(ti.end(), untagIndex.begin(), untagIndex.end()); 
	  if(int(ti.size()) != nSignalJet)
	Error("get_tagjet_selection_vhbb()", "AllIndexVector does not match nSignalJet. This should not happen.");
    }

    if (nSignalJet >= 1){
      Jet1 = signalJets.at(ti[0]);
      selectedJets.push_back(Jet1);
      BTagProps::forBTagSF.set(Jet1, true);
    }
    if (nSignalJet >= 2){
      Jet2 = signalJets.at(ti[1]);
      selectedJets.push_back(Jet2);
      BTagProps::forBTagSF.set(Jet2, true);
    }
    
    if (nSignalJet >= 3){
      BTagProps::forBTagSF.set(signalJets.at(ti[2]), true);
      jetsToSort.push_back(signalJets.at(ti[2]));
      if (nSignalJet >= 4){
		jetsToSort.push_back(signalJets.at(ti[3]));
		if (strategy == "AllSignalJets")
		  BTagProps::forBTagSF.set(signalJets.at(ti[3]), true);
		else // Leading3SignalJets
		  BTagProps::forBTagSF.set(signalJets.at(ti[3]), false);
	  }
	}
  } // end tag strategy "AllSignalJets" || "Leading3SignalJets"

  /////////////////////////////////////////////////////////////
  if (strategy == "Leading2SignalJets") {

    bool lead_tag = (nSignalJet >= 1) ? BTagProps::isTagged.get(signalJets.at(0)) : false;
    bool sublead_tag = (nSignalJet >= 2) ? BTagProps::isTagged.get(signalJets.at(1)) : false;

    if (!lead_tag && !sublead_tag) tagcatExcl = 0;
    if ((lead_tag && !sublead_tag) || (!lead_tag && sublead_tag)) tagcatExcl = 1;
    if (lead_tag && sublead_tag) tagcatExcl = 2;

    // NB: vector selectedJets is built so that its first two jets are
    // the two jets we use to reconstruct the H candidate, ordered in pT
    if (nSignalJet >= 1){
      Jet1 = signalJets.at(0);
      selectedJets.push_back(Jet1);
      BTagProps::forBTagSF.set(Jet1, true);
    }
    if (nSignalJet >= 2){
      Jet2 = signalJets.at(1);
      selectedJets.push_back(Jet2);
      BTagProps::forBTagSF.set(Jet2, true);
    }

    if (nSignalJet >= 3){
      BTagProps::forBTagSF.set(signalJets.at(2), false);
      jetsToSort.push_back(signalJets.at(2));
      if (nSignalJet >= 4){
	BTagProps::forBTagSF.set(signalJets.at(3), false);
	jetsToSort.push_back(signalJets.at(3));
      }
    }

  } //"leading2signaljet"

  // NB. THIS IS ONLY FOR RESOLVED - In Merged nForwardJets always equal 0.    
  if (nSignalJet >= 2)
    {
      if( nForwardJet >= 1){
	BTagProps::forBTagSF.set(forwardJets.at(0), false);
	jetsToSort.push_back(forwardJets.at(0));
	if (nForwardJet >= 2){
	  BTagProps::forBTagSF.set(forwardJets.at(1), false);
	  jetsToSort.push_back(forwardJets.at(1));
	}
      }
    }

  if( jetsToSort.size() > 0)
    {
      // If Leading3SignalJets, keep the first jet in jetToSort as jet3 no matter what.
      // Because the btagging SF is calculated by the leading3signaljets.
      if (strategy == "Leading3SignalJets" && jetsToSort.size() > 1){
	std::sort(jetsToSort.begin()+1, jetsToSort.end(), sort_pt );
      }else{
	// sort by pt and assign first and second to Jet3 and Jet4
	std::sort(jetsToSort.begin(), jetsToSort.end(), sort_pt );
      }

      // For "AllSignalJets" strategy tagged jets go first, 
      // for "Leading2SignalJets" we only look at the first two jets tagging
      if (strategy == "AllSignalJets") 
	std::stable_sort(jetsToSort.begin(), jetsToSort.end(), [] (const xAOD::Jet* jetA, const xAOD::Jet* jetB)
		  {return BTagProps::isTagged.get(jetA) > BTagProps::isTagged.get(jetB);} );

      Jet3 = jetsToSort.at(0);
      if( jetsToSort.size() > 1)
	Jet4 = jetsToSort.at(1);
    }

  if (Jet3) selectedJets.push_back(Jet3);
  if (Jet4) selectedJets.push_back(Jet4);

  return StatusCode::SUCCESS;
}  // get_tagjet_selection_vhbb


// This function counts the number of b-tagged track jets outside the fat jet
// as well as the number of b-tagged track jets inside the fat jet, regardless
// the tag strategy
StatusCode BTaggingTool::set_BtagProperties_fatJets(const xAOD::Jet *fatJet, std::vector<const xAOD::Jet *> trackJetsInFatJet,
					      std::vector<const xAOD::Jet *> trackJetsNotInFatJet, bool isMC) {

  auto nTags = 0;
  auto nAddbtag = 0;
  for (const xAOD::Jet *trackjet : trackJetsNotInFatJet) {
    if (BTagProps::isTagged.get(trackjet)) nAddbtag++;
  }
  for (const xAOD::Jet *trackjet : trackJetsInFatJet) {
    if (BTagProps::isTagged.get(trackjet)) ++nTags;
  }
  BTagProps::nAddBTags.set(fatJet, nAddbtag);  // count number of b-tagged track
                                           // jets outside the fat jet
  BTagProps::nAllBTagsTrkJets.set(fatJet, nTags);  // count number of b-tagged track jets
                                        // inside the fat jet regardless the tag strategy

  // determine number of true b-jets in a fat jet
  int nBJets = 0;
  if (isMC) {
    for (const xAOD::Jet *trackjet : trackJetsInFatJet) {
      if (Props::HadronConeExclTruthLabelID.get(trackjet) == 5) ++nBJets;
    }
  } else {
    nBJets = -1;  // Data
  }
  BTagProps::nTrueBJets.set(fatJet, nBJets);

  return StatusCode::SUCCESS;
}  // set_BtagProperties_fatJets

StatusCode BTaggingTool::computeEventWeight_vhbb(const std::vector<const xAOD::Jet *> &signalJets,
						 std::map<std::string, float>& btagEffSFs, float& nominalSF, bool doSyst, std::string btc_name){
  
  // no point in going further if there are no jets
  // (eg. for VR jets inside the FJ: they can be zero)
  if (signalJets.size() == 0)
    return StatusCode::SUCCESS;

  auto btc = m_btc_map.at(btc_name);

  if(m_debug){
    Info("computeEventWeight_vhbb()", "Called.");
    btc.print();
  }

  nominalSF = 1.;
  bool truth_tag = btc.doTruthTagging;
  std::string strategy = btc.strategy;
  
  if(truth_tag){ //includes Hybrid Tag as well
    btagEffSFs = compute_TruthTag_EventWeight (doSyst, btc_name);
  }
  else if (strategy == "Leading2SignalJets"){ //compute weights just for the first two jets
    std::vector<const xAOD::Jet *> leadingjets;
    if(signalJets.size() >= 1) leadingjets.push_back(signalJets.at(0));
    if(signalJets.size() >=2) leadingjets.push_back(signalJets.at(1));
    
    btagEffSFs = computeEventWeight(leadingjets, doSyst, btc_name);
  }
  else if (strategy == "Leading3SignalJets"){
    std::vector<const xAOD::Jet *> leadingjets;
    if(signalJets.size() >= 1) leadingjets.push_back(signalJets.at(0));
    if(signalJets.size() >= 2) leadingjets.push_back(signalJets.at(1));
    if(signalJets.size() >= 3) leadingjets.push_back(signalJets.at(2));
    
    btagEffSFs = computeEventWeight(leadingjets, doSyst, btc_name);    
  }
  else{ //all other Direct Tagged strategies
    btagEffSFs = computeEventWeight(signalJets, doSyst, btc_name);
  }    

  nominalSF = btagEffSFs["Nominal"];

  if (m_debug) Info("computeEventWeight_vhbb()", "Ended. SF: %.3f", nominalSF);

  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////////
//VHcc related functions:

void BTaggingTool::tagjet_selection_vhcc(std::vector<const xAOD::Jet *> &signalJets,
					 std::vector<const xAOD::Jet *> &selectedJets,
					 int &tagcatExcl, int &nSubleadingTags, std::string btc_name) {

  selectedJets.clear();
  auto btc = m_btc_map.at(btc_name);
  auto seltool_veto = m_selectionTools[btc.taggerName_veto][btc.jetAuthor][btc.wp_veto];

  if(!seltool_veto){ //auto status = initializeSelectTool(m_selectionTools[btc.taggerName_veto][btc.jetAuthor][btc.wp_veto],btc);
    //if (status != StatusCode::SUCCESS) { Error("VHcc","error in loading selection tool "); }
    //seltool_veto = m_selectionTools[btc.taggerName_veto][btc.jetAuthor][btc.wp_veto];
    Error("VHcc","error in loading selection tool. This should not happen. Please check btagconfig settings. ");
  }
  /////////////////////////////////////////////////////////////
  // **C-Tagging Selection**

  int nSignalJets = signalJets.size();
  int nctag = 0;
  nSubleadingTags = 0;

  const xAOD::Jet *Jet1 = nullptr;
  const xAOD::Jet *Jet2 = nullptr;
  const xAOD::Jet *Jet3 = nullptr;

  if (nSignalJets >= 1) Jet1 = signalJets.at(0);
  if (nSignalJets >= 2) Jet2 = signalJets.at(1);
  if (nSignalJets >= 3) Jet3 = signalJets.at(2);

  // fill selected jets
  //------------------
  if (Jet1) {
    selectedJets.push_back(Jet1);
    BTagProps::isTagged.set( Jet1, isTagged(*Jet1, btc_name) );
    if (BTagProps::isTagged.get(Jet1)) nctag++;
  }
  if (Jet2) {
    selectedJets.push_back(Jet2);
    BTagProps::isTagged.set( Jet2, isTagged(*Jet2, btc_name) );
    if (BTagProps::isTagged.get(Jet2)) nctag++;
  }
  if (Jet3) {
    selectedJets.push_back(Jet3);
  }

  tagcatExcl = nctag;
  // In case the additional jets are b-tagged we reject the event
  for (int i = 2; i < nSignalJets; i++) {
    const xAOD::Jet *jet = signalJets.at(i);
    float jMV2c10 = Props::MV2c10.get(jet);


    if (seltool_veto->accept(jet->pt(), jet->eta(), jMV2c10)){
      nSubleadingTags++;
    }
  }
}

void BTaggingTool::tagjet_selection_vhcc(std::vector<const xAOD::Jet *> &signalJets, std::vector<const xAOD::Jet *> &fwdJets,
					 std::vector<const xAOD::Jet *> &selectedJets,
					 int &tagcatExcl, int &nSubleadingTags, std::string btc_name) {

  selectedJets.clear();
  auto btc = m_btc_map.at(btc_name);
  auto seltool_veto = m_selectionTools[btc.taggerName_veto][btc.jetAuthor][btc.wp_veto];
  if(!seltool_veto){ //auto status = initializeSelectTool(m_selectionTools[btc.taggerName_veto][btc.jetAuthor][btc.wp_veto],btc);
    //if (status != StatusCode::SUCCESS) { 
    Error("VHcc","Error in loading selection tool. This should not happen. Please check btagconfig settings. "); }
    //   seltool_veto = m_selectionTools[btc.taggerName_veto][btc.jetAuthor][btc.wp_veto];
  //}

  /////////////////////////////////////////////////////////////
  // **C-Tagging Selection**

  int nSignalJets = signalJets.size();
  int nFwdJets = fwdJets.size();
  int nctag = 0;
  nSubleadingTags = 0;

  const xAOD::Jet *Jet1 = nullptr;
  const xAOD::Jet *Jet2 = nullptr;
  const xAOD::Jet *Jet3 = nullptr;

  if (nSignalJets >= 1) Jet1 = signalJets.at(0);
  if (nSignalJets >= 2) Jet2 = signalJets.at(1);
  if (nSignalJets >= 3){ Jet3 = signalJets.at(2);}
  else if(nSignalJets==2 && nFwdJets>0){ Jet3 =fwdJets.at(0);}

  // fill selected jets
  //------------------
  if (Jet1) {
    selectedJets.push_back(Jet1);
    BTagProps::isTagged.set( Jet1, isTagged(*Jet1, btc_name) );
    if (BTagProps::isTagged.get(Jet1)) nctag++;
  }
  if (Jet2) {
    selectedJets.push_back(Jet2);
    BTagProps::isTagged.set( Jet2, isTagged(*Jet2, btc_name) );
    if (BTagProps::isTagged.get(Jet2)) nctag++;
  }
  if (Jet3) {
    selectedJets.push_back(Jet3);
  }
  tagcatExcl = nctag;
  // In case the additional jets are b-tagged we reject the event
  for (int i = 2; i < nSignalJets; i++) {
    const xAOD::Jet *jet = signalJets.at(i);
    float jMV2c10 = Props::MV2c10.get(jet);


    if (seltool_veto->accept(jet->pt(), jet->eta(), jMV2c10)){
      nSubleadingTags++;
    }
  }
}

std::map< std::string, std::map<int, float> >  BTaggingTool::compute_TruthTag_EventWeight_VHcc(std::vector<const xAOD::Jet*> &xaodjets, std::string btc_name, bool doSyst, 
   std::string description, const float& correction_dR_jet1, const float& correction_dR_jet2, bool decorrelateTaus){

  //this function computes the event weights (direct tag and truth tag) for the VHcc style of doing truth tagging 
  //(filling each event to multiple histograms, 0, 1 and 2 tag, and whatever direct tag histograms )
  //it fill the map eventweights with event weights for 0,1,2 truth tags, and the -1 entry is for the direct tag histgoram (whatever that is)

  std::map< std::string, std::map<int, float> > eventweights;

  //set up the tools we need:
  auto btc = m_btc_map.at(btc_name);
  auto efftool_nominal = m_efficiencyTools[btc.tagger][btc.jetAuthor][btc.wp];
  auto efftool_veto = m_efficiencyTools[btc.taggerName_veto][btc.jetAuthor][btc.wp_veto];

  if(!efftool_nominal){//auto status = initializeEffTool(m_efficiencyTools[m_tagger][m_jetAuthor][nominal_wp]
    //,m_tagger,m_jetAuthor,nominal_wp); 
    //if (status != StatusCode::SUCCESS) { 
      Error("VHcc truth tagging","error in loading efficiency tool "); 
      // efftool_nominal = m_efficiencyTools[m_tagger][m_jetAuthor][m_workingPoint];}
  }
  if(!efftool_veto){ //auto status = initializeEffTool(m_efficiencyTools[m_taggerName_Veto][m_jetAuthor][m_OP_Veto],m_taggerName_Veto,m_jetAuthor,m_OP_Veto); 
    //if (status != StatusCode::SUCCESS) { 
    Error("VHcc truth tagging","error in loading efficiency tool "); 
    //    efftool_veto = m_efficiencyTools[m_taggerName_Veto][m_jetAuthor][m_OP_Veto];
  }
  
  int njets = xaodjets.size();
  
  if(njets<2){
    return eventweights;
  }

  int jet1flav = abs(getFlavor(*xaodjets.at(0)));
  int jet2flav = abs(getFlavor(*xaodjets.at(1)));

  bool taus_in_event = false;
  if(jet1flav==15 || jet2flav==15){
    taus_in_event = true;
  }
  // CDI setup for ttbar based MC/MC scale factors and inefficiency scale factors
  // In order to use custom maps, please replace m_MCIndex by m_MCIndex_TruthTag 
  efftool_nominal->setMapIndex("B", m_MCIndex);
  efftool_nominal->setMapIndex("C", m_MCIndex);
  efftool_nominal->setMapIndex("Light", m_MCIndex);
  efftool_nominal->setMapIndex("T", m_MCIndex);

  efftool_veto->setMapIndex("B", m_MCIndex);
  efftool_veto->setMapIndex("C", m_MCIndex);
  efftool_veto->setMapIndex("Light", m_MCIndex);
  efftool_veto->setMapIndex("T", m_MCIndex);

  std::vector<float> jet_effs;
  std::vector<float> jet_ineffs;

  std::vector<float> jet_directtag_SFs;

  GetVhccEffs(jet_effs, jet_ineffs, jet_directtag_SFs, efftool_nominal, efftool_veto, xaodjets, btc_name, description);
      
  float direct_tag_weight_2leading = jet_directtag_SFs.at(0)*jet_directtag_SFs.at(1);
  float veto_ineff_sf = 1.0;
  if(njets > 2){
    veto_ineff_sf = jet_directtag_SFs.at(2);
  }

  // apply min dR jet specific corrections
  // correction_dR_jet1/2 = 1 by default 
  float eff1, eff2, ineff1, ineff2;

  eff1 = ( jet_effs[0] * correction_dR_jet1  );
  eff2 = ( jet_effs[1] * correction_dR_jet2  );

  ineff1 = (1 - correction_dR_jet1  * (1. - jet_ineffs[0])); // Ineff weight becomes new_ineff = (1 - c * eff )
  ineff2 = (1 - correction_dR_jet2  * (1. - jet_ineffs[1]));

  // compute event weights for direct and truth tagging
  // treat truth tagging and direct tagging in the same way: b-tag veto ineff scale factor on 3+jets applied directly 
  eventweights["Nominal"][-1] = direct_tag_weight_2leading*veto_ineff_sf; // Direct tagging weight
  eventweights["Nominal"][0]  = ineff1 * ineff2 * veto_ineff_sf;  // 0 c-tag Truth Tagging weight
  eventweights["Nominal"][1]  = (eff1 * ineff2 + eff2 * ineff1) * veto_ineff_sf; // 1 c-tag Truth Tagging weight
  eventweights["Nominal"][2]  = eff1 * eff2 * veto_ineff_sf; // 2 c-tag Truth Tagging weight
  eventweights["Nominal"][3]  = (eff1 * ineff2 + eff2 * ineff1 + eff1 * eff2) * veto_ineff_sf;// 1+ c-tag = 1 c-tag + 2 c-tag  Truth Tagging weight

  // optionally do systematics
  if (doSyst) {
         
    std::vector<float> sys_jet_effs;
    std::vector<float> sys_jet_ineffs;
    std::vector<float> sys_jet_directtag_SFs;

    float sys_direct_tag_weight = 1.0;
    float sys_veto_ineff_sf = 1.0;

    SystematicSet affectingSystematics = efftool_nominal->affectingSystematics();

    for (auto& var : affectingSystematics) {

      auto name = Analysis::replace_all(var.name(), " ", "_");
      auto name_bveto = Analysis::replace_all(var.name(), " ", "_");
            
      int suffix = 5; 
      if (name.find("__1down") != string::npos) { suffix = 7;}

      name_bveto = name_bveto.substr(0,name_bveto.size()-suffix);
      std::string var_str = name.substr(name.size()-suffix,name.size());

      SystematicSet set;

      set.insert(var);
      auto sresult = efftool_nominal->applySystematicVariation(set);
      if (sresult != StatusCode::SUCCESS) continue;
      sresult = efftool_veto->applySystematicVariation(set);
      if (sresult != StatusCode::SUCCESS) continue;

      sys_jet_effs.clear();
      sys_jet_ineffs.clear();
      sys_jet_directtag_SFs.clear();
              
      GetVhccEffs(sys_jet_effs, sys_jet_ineffs, sys_jet_directtag_SFs, efftool_nominal, efftool_veto, xaodjets, btc_name, description);
      sys_direct_tag_weight = sys_jet_directtag_SFs.at(0)*sys_jet_directtag_SFs.at(1);
      if(njets > 2){
	sys_veto_ineff_sf = sys_jet_directtag_SFs.at(2);
      }
              
      float temp_eff1 = sys_jet_effs[0];
      float temp_eff2 = sys_jet_effs[1];
      float temp_ineff1 = sys_jet_ineffs[0];
      float temp_ineff2 = sys_jet_ineffs[1];

      if(taus_in_event && name.find("extrapolation_from_charm")== string::npos && decorrelateTaus){
	//if we have taus, and its NOT the extrapolation from charm syst (which applies only to taus), 
	// we want to set taus back to their nominal SF/eff values
	sys_direct_tag_weight = 1.0;
                  
	if(jet1flav==15){ 
	  temp_eff1 = jet_effs.at(0); temp_ineff1 = jet_ineffs.at(0); 
	  sys_direct_tag_weight*=jet_directtag_SFs.at(0);
	}
	if(jet2flav==15){ 
	  temp_eff2 = jet_effs.at(1); temp_ineff2 = jet_ineffs.at(1); 
	  sys_direct_tag_weight*=jet_directtag_SFs.at(1);
	}
      }

      // apply min dR jet specific corrections
      // correction_dR_jet1/2 = 1 by default 
      temp_ineff1 = (1 - correction_dR_jet1 * (1. - temp_ineff1)); // Ineff weight becomes new_ineff = (1 - c * eff )
      temp_ineff2 = (1 - correction_dR_jet2 * (1. - temp_ineff2)); 

      temp_eff1 = temp_eff1 * correction_dR_jet1;
      temp_eff2 = temp_eff2 * correction_dR_jet2;
      // compute event weights for direct and truth tagging
      // no sys variation on the veto
      eventweights[name][-1] = sys_direct_tag_weight*veto_ineff_sf; // Direct tagging weight, 
      eventweights[name][0] = temp_ineff1 * temp_ineff2 * veto_ineff_sf; //  0 c-tag Truth Tagging weight
      eventweights[name][1] = (temp_eff1 * temp_ineff2 + temp_eff2 * temp_ineff1) * veto_ineff_sf; // 1 c-tag Truth Tagging weight
      eventweights[name][2] = temp_eff1 * temp_eff2 * veto_ineff_sf; // 2 c-tag Truth Tagging weight
      eventweights[name][3] = eventweights[name][1]+eventweights[name][2]; // 1+ c-tag = 1 c-tag + 2 c-tag Truth Tagging weight

      // no sys variation on the c-tagger
      eventweights[name_bveto+"_bveto"+var_str][-1] = direct_tag_weight_2leading*sys_veto_ineff_sf; // Direct tagging weight, note: veto ineff already applied in direct tag weight
      eventweights[name_bveto+"_bveto"+var_str][0] = ineff1 * ineff2 * sys_veto_ineff_sf; //  0 c-tag Truth Tagging weight
      eventweights[name_bveto+"_bveto"+var_str][1] = (eff1 * ineff2 + eff2 * ineff1) * sys_veto_ineff_sf; // 1 c-tag Truth Tagging weight
      eventweights[name_bveto+"_bveto"+var_str][2] = eff1 * eff2 * sys_veto_ineff_sf; // 2 c-tag Truth Tagging weight
      eventweights[name_bveto+"_bveto"+var_str][3] = (eff1 * ineff2 + eff2 * ineff1 + eff1 * eff2) * sys_veto_ineff_sf; // 1+ c-tag = 1 c-tag + 2 c-tag Truth Tagging weight

      if (name.find("Eigen_C")!= string::npos && decorrelateTaus){
	// for systematics that apply to tau jets and c jets, 
	// in this IF block we want to remove the sys variation for all jets that are NOT taus (c jets, specifically)
	// and then fill an additional systematic with Eigen_C replaced by Eigen_T
	sys_direct_tag_weight = 1.0;

	if(jet1flav==15){ 
	  temp_eff1 = sys_jet_effs.at(0); temp_ineff1 = sys_jet_ineffs.at(0); 
	  sys_direct_tag_weight*=sys_jet_directtag_SFs.at(0);
	}else{
	  temp_eff1 = jet_effs.at(0); temp_ineff1 = jet_ineffs.at(0); 
	  sys_direct_tag_weight*= jet_directtag_SFs.at(0);
	}
    
	if(jet2flav==15){ 
	  temp_eff2 = sys_jet_effs.at(1); temp_ineff2 = sys_jet_ineffs.at(1); 
	  sys_direct_tag_weight*=sys_jet_directtag_SFs.at(1);
	}else{
	  temp_eff2 = jet_effs.at(1); temp_ineff2 = jet_ineffs.at(1); 
	  sys_direct_tag_weight*=jet_directtag_SFs.at(1);
	}

	temp_ineff1 = (1 - correction_dR_jet1 * (1. - temp_ineff1)); // Ineff weight becomes new_ineff = (1 - c * eff )
	temp_ineff2 = (1 - correction_dR_jet2 * (1. - temp_ineff2)); 

	temp_eff1 = temp_eff1 * correction_dR_jet1;
	temp_eff2 = temp_eff2 * correction_dR_jet2;


	std::string tau_var_name = name;
       
	size_t start_pos = tau_var_name.find("Eigen_C");
	tau_var_name.replace(start_pos,7,"Eigen_T");

	// compute event weights for direct and truth tagging
	// no sys variation on the veto
	eventweights[tau_var_name][-1]= sys_direct_tag_weight*veto_ineff_sf; // Direct tagging weight, 
	eventweights[tau_var_name][0] = temp_ineff1 * temp_ineff2 * veto_ineff_sf; //  0 c-tag Truth Tagging weight
	eventweights[tau_var_name][1] = (temp_eff1 * temp_ineff2 + temp_eff2 * temp_ineff1) * veto_ineff_sf; // 1 c-tag Truth Tagging weight
	eventweights[tau_var_name][2] = temp_eff1 * temp_eff2 * veto_ineff_sf; // 2 c-tag Truth Tagging weight
	eventweights[tau_var_name][3] = eventweights[name][1]+eventweights[name][2]; // 1+ c-tag = 1 c-tag + 2 c-tag Truth Tagging weight
      }

    } // end of loop over systematics 
    
    // dR uncertainties
    // down variation is the uncorrected weight (no dR correction applied)
    eventweights["TT_dR__1down"][-1] = direct_tag_weight_2leading*veto_ineff_sf; // Direct tagging weight
    eventweights["TT_dR__1down"][0]  = jet_ineffs[0] * jet_ineffs[1] * veto_ineff_sf; //  0 c-tag Truth Tagging weight
    eventweights["TT_dR__1down"][1]  = (jet_effs[0] * jet_ineffs[1] + jet_effs[1] * jet_ineffs[0]) * veto_ineff_sf; // 1 c-tag Truth Tagging weight
    eventweights["TT_dR__1down"][2]  = jet_effs[0] * jet_effs[1] * veto_ineff_sf; // 2 c-tag Truth Tagging weight
    eventweights["TT_dR__1down"][3]  = (jet_effs[0] * jet_ineffs[1] + jet_effs[1] * jet_ineffs[0] + jet_effs[0] * jet_effs[1]) * veto_ineff_sf; // 1+ c-tag = 1 c-tag + 2 c-tag Truth Tagging weight

    //up variation
    // apply the correction ---squared----
         
    eff1 = ( jet_effs[0] * correction_dR_jet1 * correction_dR_jet1 );
    eff2 = ( jet_effs[1] * correction_dR_jet2 * correction_dR_jet2 );

    ineff1 = (1 - correction_dR_jet1 * correction_dR_jet1 * (1. - jet_ineffs[0])); // Ineff weight becomes new_ineff = (1 - c * eff )
    ineff2 = (1 - correction_dR_jet2 * correction_dR_jet2 * (1. - jet_ineffs[1]));

    // compute event weights for direct and truth tagging
    eventweights["TT_dR__1up"][-1] = direct_tag_weight_2leading*veto_ineff_sf; // Direct tagging weight, note: veto ineff already applied in direct tag weight
    eventweights["TT_dR__1up"][0]  = ineff1 * ineff2 * veto_ineff_sf;  // 0 c-tag Truth Tagging weight
    eventweights["TT_dR__1up"][1]  = (eff1 * ineff2 + eff2 * ineff1) * veto_ineff_sf; // 1 c-tag Truth Tagging weight
    eventweights["TT_dR__1up"][2]  = eff1 * eff2 * veto_ineff_sf; // 2 c-tag Truth Tagging weight
    eventweights["TT_dR__1up"][3]  = (eff1 * ineff2 + eff2 * ineff1 + eff1 * eff2) * veto_ineff_sf;// 1+ c-tag = 1 c-tag + 2 c-tag  Truth Tagging weight

    // revert to nominal
    SystematicSet default_set;
    auto sresult = efftool_nominal->applySystematicVariation(default_set);
    if (sresult != StatusCode::SUCCESS) Error("failed to restore nominal setting in efficiency tool: ","compute_TruthTag_EventWeight_VHcc");
    sresult = efftool_veto->applySystematicVariation(default_set);
    if (sresult != StatusCode::SUCCESS) Error("failed to restore nominal setting in efficiency tool: ","compute_TruthTag_EventWeight_VHcc");

  }
      
  return eventweights;

}

void BTaggingTool::GetVhccEffs(std::vector<float> &jet_effs, std::vector<float> &jet_ineffs,
                               std::vector<float> &jet_directtag_SFs,std::shared_ptr<BTaggingEfficiencyTool> efftool_nominal, 
			       std::shared_ptr<BTaggingEfficiencyTool> efftool_veto,std::vector<const xAOD::Jet*> &xaodjets, std::string btc_name, std::string description){

  int njets = xaodjets.size();
  float eff = 1.0;
  float ineff = 1.0;
  float sf = 1.0;

  // compute eff, ineff, scale factor and ineff scale factors on 2 leading jets
  for(int jet_i=0;jet_i<2;jet_i++){
    const xAOD::Jet* jet= xaodjets.at(jet_i);
    auto status = efftool_nominal->getEfficiency(*jet,eff);
    if (status == CorrectionCode::Error){ Error("VHcc truth tagging","error in getEfficiency");  }
        
    status =  efftool_nominal->getInefficiency(*jet,ineff);
    if (status == CorrectionCode::Error){ Error("VHcc truth tagging","error in getInefficiency");  }


    // store efficiencies and inefficiencies for 2 leading jets
    jet_effs.push_back(eff);
    jet_ineffs.push_back(ineff);

    if(isTagged(*jet, btc_name)){
      status = efftool_nominal->getScaleFactor(*jet,sf);
      if (status == CorrectionCode::Error){ Error("VHcc truth tagging","error in getScaleFactor");  }
    }else{
      status = efftool_nominal->getInefficiencyScaleFactor(*jet,sf);
      if (status == CorrectionCode::Error){ Error("VHcc truth tagging","error in getInefficiencyScaleFactor");  }
    }
    jet_directtag_SFs.push_back(sf);
  }

      
  // compute b-tag veto inefficiency scale factors on 3+jets
  if(njets > 2){
    for(int jet_i=2;jet_i<njets;jet_i++){
      const xAOD::Jet* jet= xaodjets.at(jet_i);
          
      if (description == "topCR"){
	auto status =  efftool_veto->getScaleFactor(*jet,sf);
	if (status == CorrectionCode::Error){ Error("VHcc truth tagging","error in getScaleFactor");  }
            
	jet_directtag_SFs.push_back(sf);
      }else{
	auto status =  efftool_veto->getInefficiencyScaleFactor(*jet,sf);
	if (status == CorrectionCode::Error){ Error("VHcc truth tagging","error in getInefficiencyScaleFactor");  }
            
	jet_directtag_SFs.push_back(sf);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////
//Utils:

CP::SystematicSet BTaggingTool::affectingSystematics (std::string btc_name) const
{
  auto btc = m_btc_map.at(btc_name);
  auto efftool = m_efficiencyTools.at(btc.tagger).at(btc.jetAuthor).at(btc.wp);

  if(!efftool){
    Error("BTaggingTool::computeEventWeight()", "efficiency tool is not initialized for "
	  "%s (tagger), %s (jet author), and %s (working point)!",
	  btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.wp.c_str());
    return SystematicSet();
  }
    return efftool->affectingSystematics();
} // affectingSystematics


int BTaggingTool::getFlavor (const xAOD::Jet &jet) const
{
  int label = -1;

  if (Props::HadronConeExclTruthLabelID.exists(&jet)) Props::HadronConeExclTruthLabelID.get(&jet, label);
  else if (Props::TruthLabelID.exists(&jet)) Props::TruthLabelID.get(&jet, label);
  else if (jet.getAttribute("HadronConeExclTruthLabelID", label)) {}
  else if (jet.getAttribute("TruthLabelID", label)) {}

  return label;
} // getFlavor


string BTaggingTool::getFlavorLabel (const xAOD::Jet &jet) const
{
  int label = getFlavor(jet);

  if ((label == 5) || (label == -5)) return "B";

  if ((label == 4) || (label == -4)) return "C";

  if ((label == 15) || (label == -15)) return "T";
  else return "Light";
} // getFlavorLabel


void BTaggingTool::getBinFromEfficiency(std::string& btagcuteff, int &effbin) const{
  if(btagcuteff == "60") effbin = 5;
  else if(btagcuteff == "70")  effbin = 4;
  else if(btagcuteff == "77")  effbin = 3;
  else if(btagcuteff == "85")  effbin = 2;
  else if(btagcuteff == "100") effbin = 1;
  else{
    std::cout << "Unknown efficiency case, unable to get the quantile" << std::endl;
    effbin = -1;
  }
}

void BTaggingTool::SetMCIndex(std::shared_ptr<BTaggingEfficiencyTool> efftool){
  // CDI setup for ttbar based MC/MC scale factors and inefficiency scale factors
  // In order to use custom maps, please replace m_MCIndex by m_MCIndex_TruthTag
  efftool->setMapIndex("B", m_MCIndex);
  efftool->setMapIndex("C", m_MCIndex);
  efftool->setMapIndex("Light", m_MCIndex);
  efftool->setMapIndex("T", m_MCIndex);
  if(m_debug)
    Info("BTaggingTool::SetMCIndex()", "setting MCIndex to %d ", m_MCIndex);
}
