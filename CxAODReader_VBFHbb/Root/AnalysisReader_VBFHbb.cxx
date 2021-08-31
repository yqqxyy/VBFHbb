#include <type_traits>

#include <EventLoop/Worker.h>

#include "TSystem.h"

#include <CxAODReader_VBFHbb/AnalysisReader_VBFHbb.h>

#include "CxAODTools_VBFHbb/VBFHbbInclEvtSelection.h"

#include "CxAODTools/CommonProperties.h"
#include "PathResolver/PathResolver.h"
#include "xAODTruth/TruthEventContainer.h"
//#include "CxAODTools_VBFHbb/VBFHbbJetAssignment.h"

PROPERTY( Props , int , isMatchedToLeadFatJet )


#define length(array) (sizeof(array) / sizeof(*(array)))

ClassImp(AnalysisReader_VBFHbb)

AnalysisReader_VBFHbb::AnalysisReader_VBFHbb () :
  AnalysisReader(),
  m_analysisType(""),
  m_analysisStrategy("SimpleMerge500"),
  m_selection("none"),
  m_doTruthTagging(false),
  m_doOnlyInputs(false),
  m_doReduceFillHistos(false),
  m_tree(nullptr),
  m_jetCorrType("OneMu"),
  m_fatJetCorrType("OneMu"),
  m_tagStrategy("AllSignalJets"),
  m_doBlindAnalysis(false),
  m_doFillHistograms(false),
  m_doMergeCR(false),
  m_doCutflow(false),
  m_fatJetHistos(),
  m_jetAssigner(nullptr),
  m_eventCountPassed_VBFHbb(0),
  m_TrigEffHist_offline70_online70_data(nullptr),
  m_TrigEffHist_offline70_online70_mc(nullptr),
  m_TrigEffHist_offline70_online60_data(nullptr),
  m_TrigEffHist_offline70_online60_mc(nullptr),
  m_TrigEffHist_offline85_online85_data(nullptr),
  m_TrigEffHist_offline85_online85_mc(nullptr),

  m_Eff_Event_leadingJet_jetEta(nullptr),
  m_SF_Off85_match_hlt85_jetPt16(nullptr),
  m_SF_Off85_match_hlt70_jetPt16(nullptr),
  m_SF_Off85_match_hlt60_jetPt16(nullptr),
  m_SF_Off77_match_hlt85_jetPt16(nullptr),
  m_SF_Off77_match_hlt70_jetPt16(nullptr),
  m_SF_Off77_match_hlt60_jetPt16(nullptr),
  m_SF_Off70_match_hlt85_jetPt16(nullptr),
  m_SF_Off70_match_hlt70_jetPt16(nullptr),
  m_SF_Off70_match_hlt60_jetPt16(nullptr),
  m_SF_Off60_match_hlt85_jetPt16(nullptr),
  m_SF_Off60_match_hlt70_jetPt16(nullptr),
  m_SF_Off60_match_hlt60_jetPt16(nullptr),
  m_Eff_Off85_match_hlt85_jetPt16(nullptr),
  m_Eff_Off85_match_hlt70_jetPt16(nullptr),
  m_Eff_Off85_match_hlt60_jetPt16(nullptr),
  m_Eff_Off85_match_hlt40_jetPt16(nullptr),
  m_Eff_Off77_match_hlt85_jetPt16(nullptr),
  m_Eff_Off77_match_hlt70_jetPt16(nullptr),
  m_Eff_Off77_match_hlt60_jetPt16(nullptr),
  m_Eff_Off77_match_hlt40_jetPt16(nullptr),
  m_Eff_Off70_match_hlt85_jetPt16(nullptr),
  m_Eff_Off70_match_hlt70_jetPt16(nullptr),
  m_Eff_Off70_match_hlt60_jetPt16(nullptr),
  m_Eff_Off70_match_hlt40_jetPt16(nullptr),
  m_Eff_Off60_match_hlt85_jetPt16(nullptr),
  m_Eff_Off60_match_hlt70_jetPt16(nullptr),
  m_Eff_Off60_match_hlt60_jetPt16(nullptr),
  m_Eff_Off60_match_hlt40_jetPt16(nullptr),
  m_SF_Off85_match_hlt85_jetPt(nullptr),
  m_SF_Off85_match_hlt70_jetPt(nullptr),
  m_SF_Off85_match_hlt60_jetPt(nullptr),
  m_SF_Off77_match_hlt85_jetPt(nullptr),
  m_SF_Off77_match_hlt70_jetPt(nullptr),
  m_SF_Off77_match_hlt60_jetPt(nullptr),
  m_SF_Off70_match_hlt85_jetPt(nullptr),
  m_SF_Off70_match_hlt70_jetPt(nullptr),
  m_SF_Off70_match_hlt60_jetPt(nullptr),
  m_SF_Off60_match_hlt85_jetPt(nullptr),
  m_SF_Off60_match_hlt70_jetPt(nullptr),
  m_SF_Off60_match_hlt60_jetPt(nullptr),
  m_Eff_Off85_match_hlt85_jetPt(nullptr),
  m_Eff_Off85_match_hlt70_jetPt(nullptr),
  m_Eff_Off85_match_hlt60_jetPt(nullptr),
  m_Eff_Off85_match_hlt40_jetPt(nullptr),
  m_Eff_Off77_match_hlt85_jetPt(nullptr),
  m_Eff_Off77_match_hlt70_jetPt(nullptr),
  m_Eff_Off77_match_hlt60_jetPt(nullptr),
  m_Eff_Off77_match_hlt40_jetPt(nullptr),
  m_Eff_Off70_match_hlt85_jetPt(nullptr),
  m_Eff_Off70_match_hlt70_jetPt(nullptr),
  m_Eff_Off70_match_hlt60_jetPt(nullptr),
  m_Eff_Off70_match_hlt40_jetPt(nullptr),
  m_Eff_Off60_match_hlt85_jetPt(nullptr),
  m_Eff_Off60_match_hlt70_jetPt(nullptr),
  m_Eff_Off60_match_hlt60_jetPt(nullptr),
  m_Eff_Off60_match_hlt40_jetPt(nullptr),
  m_inFile(nullptr),
  m_inFile16(nullptr)
{
  m_csCorrections.clear();
  m_csVariations.clear();
}

AnalysisReader_VBFHbb::~AnalysisReader_VBFHbb ()
{
  if (m_jetAssigner != nullptr) delete m_jetAssigner;
}

EL::StatusCode AnalysisReader_VBFHbb::initializeSelection ()
{
  xAOD::SetMessageSourceWidth(100);
  m_config->getif<string>("analysisType", m_analysisType);
  m_config->getif<string>("analysisStrategy", m_analysisStrategy);
  m_config->getif<string>("JetAssignmentStrategy", m_JetAssignmentStrategy);
  m_config->getif<string>("fJVTCut", m_fJVTCut);
  m_config->getif<string>("selection", m_selection);
  m_config->getif<float>("luminosity_mc16a", m_luminosity_a);
  m_config->getif<float>("luminosity_mc16d", m_luminosity_d);
  m_config->getif<float>("luminosity_mc16e", m_luminosity_e);

  Info("initializeSelection()", "Initialize analysis '%s %s'.", m_analysisType.c_str(), m_analysisStrategy.c_str());
  Info("initializeSelection()", "JetAssignment type '%s'", m_JetAssignmentStrategy.c_str());

  m_eventSelection = new VBFHbbInclEvtSelection();

  ((VBFHbbInclEvtSelection*)m_eventSelection)->setIsMC(m_isMC);

  m_fillFunction   = std::bind(&AnalysisReader_VBFHbb::fill_VBF, this);

  bool writeEasyTree = false;
  m_config->getif<bool>("writeEasyTree", writeEasyTree);

  // TODO move to base class?
  bool writeMVATree = false;
  bool readMVA      = false;
  bool onlyFitInputs = false;
  m_config->getif<bool>("writeMVATree", writeMVATree);
  m_config->getif<bool>("readMVA", readMVA);
  m_config->getif<bool>("onlyFitInputs", onlyFitInputs);

  if (writeEasyTree && writeMVATree) writeMVATree = false;

  if ((m_analysisType == "vbf") || (m_analysisType == "vbfa")) {
    m_tree_vbf = new MVATree_VBFHbb(writeMVATree, readMVA, m_analysisType, wk(), m_variations, false,onlyFitInputs,m_isMC);
  }

  // initialize EasyTree
  m_etree = new EasyTree(writeEasyTree, m_analysisType, wk(), m_variations, false);

  // Fill reduced set of histograms
  m_config->getif<bool>("doReduceFillHistos", m_doReduceFillHistos);

  // Fill only histograms for fit inputs
  m_config->getif<bool>("doOnlyInputs", m_doOnlyInputs);

  // do truth tagging?
  m_config->getif<bool>("doTruthTagging", m_doTruthTagging);

  m_config->getif<bool>("fillCr", m_doFillCR);

  m_config->getif<string>("jetCorrType",m_jetCorrType);
  m_config->getif<string>("fatjetCorrType",m_fatJetCorrType);

  m_config->getif<std::string>("tagStrategy", m_tagStrategy);   // AllSignalJets,Leading2SignalJets,LeadingSignalJets

  m_config->getif<std::vector<std::string> >("csCorrections", m_csCorrections);
  m_config->getif<std::vector<std::string> >("csVariations", m_csVariations);

  m_config->getif<bool>("doMergeJetBins", m_doMergeJetBins);
  m_config->getif<bool>("doMergeCR", m_doMergeCR);


  // b-tagging
  // ---------
  bool use2DbTagCut = false;
  m_config->getif<bool>("use2DbTagCut", use2DbTagCut);
  std::vector<std::string> bTagToolConfigs;
  if (use2DbTagCut) m_config->getif<std::vector<std::string> >("bTagToolConfigs2D", bTagToolConfigs);
  else m_config->getif<std::vector<std::string> >("bTagToolConfigs", bTagToolConfigs);
  std::vector<std::string> bTagVariationConfigs;
  m_config->getif<std::vector<std::string> >("weightVariations", bTagVariationConfigs);
  bool doWeightVar            = false;
  
  for (auto var : bTagVariationConfigs) {
    if (var.find("truth") == std::string::npos) {
      doWeightVar = true;
    }
  }
  
  BTaggingTool::Config_t args_loose {
    { "TaggerName", bTagToolConfigs[0] },
      { "OperatingPoint", "85" },
	{ "JetAuthor", bTagToolConfigs[2] },
	  { "Scheme", bTagToolConfigs[3] },
	    { "rScheme", "Medium" }
  };
  
  BTaggingTool::Config_t args_medium {
    { "TaggerName", bTagToolConfigs[0] },
      { "OperatingPoint", "77" },
	{ "JetAuthor", bTagToolConfigs[2] },
	  { "Scheme", bTagToolConfigs[3] },
	    { "rScheme", "Medium"}
  };

  std::string btaggingCDIfilename;
  m_config->getif<std::string>("btaggingCDIfilename", btaggingCDIfilename);
  
  m_bTagTool_Medium = new BTaggingTool();
  EL_CHECK("initializeTools()", m_bTagTool_Medium->initialize(args_medium, use2DbTagCut,false,false,2,btaggingCDIfilename));
  m_bTagTool_Medium->setWeightVar(doWeightVar);
  
  m_bTagTool_Loose = new BTaggingTool();
  EL_CHECK("initializeTools()", m_bTagTool_Loose->initialize(args_loose, use2DbTagCut,false,false,2,btaggingCDIfilename));
  m_bTagTool_Loose->setWeightVar(doWeightVar);

  return EL::StatusCode::SUCCESS;
} // initializeSelection

EL::StatusCode AnalysisReader_VBFHbb::initializeCorrsAndSysts ()
{
  if (!m_isMC) return EL::StatusCode::SUCCESS;

  std::string comEnergy    = m_config->get<std::string>("COMEnergy");

  if ((comEnergy != "8TeV") || (comEnergy != "7TeV")) comEnergy = "8TeV";
  TString csname;
  std::string debugname;

  if (m_analysisType == "vbf") { csname = comEnergy + "_Vbf"; debugname =  comEnergy + "_Vbf"; }

  Info("initializeCorrsAndSysts()", "Initializing CorrsAndSysts for %s", debugname.c_str());
  Info("initializeCorrsAndSysts()", "Skipping CorrsAndSysts because it gave an error previously");

  return EL::StatusCode::SUCCESS;
} // initializeCorrsAndSysts

EL::StatusCode AnalysisReader_VBFHbb::initializeTools ()
{
  EL_CHECK("AnalysisReader_VBFHbb::initializeTools()", AnalysisReader::initializeTools());
  EL_CHECK("AnalysisReader_VBFHbb::initializeTools() initializeCorrsAndSysts()",
           initializeCorrsAndSysts());

  // trigger tool
  // -------------
  //m_triggerTool = new TriggerTool(*m_config, m_config->get<std::string>("analysisType"));
  //m_triggerTool = new TriggerTool(*m_config);
  //EL_CHECK("AnalysisReader::initializeTools()", m_triggerTool->initialize());
  //m_triggerSystList = m_triggerTool->getTriggerSystList();

  m_KF=new KF::KinematicFit();
  //std::string datadir_KF=gSystem->Getenv("ROOTCOREBIN");
  std::string datadir_KF=gSystem->Getenv("WorkDir_DIR");
  datadir_KF+="/data/KinematicFit/";
  m_KF->Init(datadir_KF,"2012",m_isMC);

  // Jet Assignment Tool
  // -------------

  m_jetAssigner = new jetAssignmentTool();
  EL_CHECK( "AnalysisReader::jetAssignmentTool()",m_jetAssigner->setSelection(m_selection));
  EL_CHECK( "AnalysisReader::jetAssignmentTool()",m_jetAssigner->setIsMC(m_isMC));

  EL_CHECK( "AnalysisReader::jetAssignmentTool()",m_jetAssigner->setAssignmentType( m_JetAssignmentStrategy ) );

  //TString FileName  = gSystem->Getenv("ROOTCOREBIN");
  //FileName  += "/data/CxAODReader_VBFHbb/BJetTriggerEfficiencies-00-02-01.root";
  //TString FileName = PathResolverFindCalibFile("CxAODReader_VBFHbb/BJetTriggerEfficiencies-00-02-01.root");
  TString FileName = PathResolverFindCalibFile("CxAODReader_VBFHbb/BJetTriggerEfficiencies-Data2017-GRL-410470-RenamedGraphs-18Feb2019.root");
  TString FileName16 = PathResolverFindCalibFile("CxAODReader_VBFHbb/BJetTriggerEfficiencies-00-02-01.root");
  
  m_inFile = new TFile(FileName, "READ");
  m_inFile16 = new TFile(FileName16, "READ");

  m_SF_Off85_match_hlt85_jetPt = (TGraph*)m_inFile->Get("g_SF_Off85_match_hlt85_jetPt");
  m_SF_Off85_match_hlt70_jetPt = (TGraph*)m_inFile->Get("g_SF_Off85_match_hlt70_jetPt");
  m_SF_Off85_match_hlt60_jetPt = (TGraph*)m_inFile->Get("g_SF_Off85_match_hlt60_jetPt");
  m_SF_Off77_match_hlt85_jetPt = (TGraph*)m_inFile->Get("g_SF_Off77_match_hlt85_jetPt");
  m_SF_Off77_match_hlt70_jetPt = (TGraph*)m_inFile->Get("g_SF_Off77_match_hlt70_jetPt");
  m_SF_Off77_match_hlt60_jetPt = (TGraph*)m_inFile->Get("g_SF_Off77_match_hlt60_jetPt");
  m_SF_Off70_match_hlt85_jetPt = (TGraph*)m_inFile->Get("g_SF_Off70_match_hlt85_jetPt");
  m_SF_Off70_match_hlt70_jetPt = (TGraph*)m_inFile->Get("g_SF_Off70_match_hlt70_jetPt");
  m_SF_Off70_match_hlt60_jetPt = (TGraph*)m_inFile->Get("g_SF_Off70_match_hlt60_jetPt");
  m_SF_Off60_match_hlt85_jetPt = (TGraph*)m_inFile->Get("g_SF_Off60_match_hlt85_jetPt");
  m_SF_Off60_match_hlt70_jetPt = (TGraph*)m_inFile->Get("g_SF_Off60_match_hlt70_jetPt");
  m_SF_Off60_match_hlt60_jetPt = (TGraph*)m_inFile->Get("g_SF_Off60_match_hlt60_jetPt");
  m_Eff_Off85_match_hlt85_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off85_match_hlt85_jetPt");
  m_Eff_Off85_match_hlt70_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off85_match_hlt70_jetPt");
  m_Eff_Off85_match_hlt60_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off85_match_hlt60_jetPt");
  m_Eff_Off77_match_hlt85_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off77_match_hlt85_jetPt");
  m_Eff_Off77_match_hlt70_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off77_match_hlt70_jetPt");
  m_Eff_Off77_match_hlt60_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off77_match_hlt60_jetPt");
  m_Eff_Off70_match_hlt85_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off70_match_hlt85_jetPt");
  m_Eff_Off70_match_hlt70_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off70_match_hlt70_jetPt");
  m_Eff_Off70_match_hlt60_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off70_match_hlt60_jetPt");
  m_Eff_Off60_match_hlt85_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off60_match_hlt85_jetPt");
  m_Eff_Off60_match_hlt70_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off60_match_hlt70_jetPt");
  m_Eff_Off60_match_hlt60_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off60_match_hlt60_jetPt");
  m_Eff_Off60_match_hlt40_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off60_match_hlt40_jetPt");
  m_Eff_Off70_match_hlt40_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off60_match_hlt70_jetPt");
  m_Eff_Off77_match_hlt40_jetPt = (TGraph*)m_inFile->Get("g_Eff_Off60_match_hlt77_jetPt");

  m_SF_Off85_match_hlt85_jetPt16 = (TGraph*)m_inFile16->Get("g_SF_offJets85_match_hlt_match_hlt85_jetPt");
  m_SF_Off85_match_hlt70_jetPt16 = (TGraph*)m_inFile16->Get("g_SF_offJets85_match_hlt_match_hlt70_jetPt");
  m_SF_Off85_match_hlt60_jetPt16 = (TGraph*)m_inFile16->Get("g_SF_offJets85_match_hlt_match_hlt60_jetPt");
  m_SF_Off77_match_hlt85_jetPt16 = (TGraph*)m_inFile16->Get("g_SF_offJets77_match_hlt_match_hlt85_jetPt");
  m_SF_Off77_match_hlt70_jetPt16 = (TGraph*)m_inFile16->Get("g_SF_offJets77_match_hlt_match_hlt70_jetPt");
  m_SF_Off77_match_hlt60_jetPt16 = (TGraph*)m_inFile16->Get("g_SF_offJets77_match_hlt_match_hlt60_jetPt");
  m_SF_Off70_match_hlt85_jetPt16 = (TGraph*)m_inFile16->Get("g_SF_offJets70_match_hlt_match_hlt85_jetPt");
  m_SF_Off70_match_hlt70_jetPt16 = (TGraph*)m_inFile16->Get("g_SF_offJets70_match_hlt_match_hlt70_jetPt");
  m_SF_Off70_match_hlt60_jetPt16 = (TGraph*)m_inFile16->Get("g_SF_offJets70_match_hlt_match_hlt60_jetPt");
  m_SF_Off60_match_hlt85_jetPt16 = (TGraph*)m_inFile16->Get("g_SF_offJets60_match_hlt_match_hlt85_jetPt");
  m_SF_Off60_match_hlt70_jetPt16 = (TGraph*)m_inFile16->Get("g_SF_offJets60_match_hlt_match_hlt70_jetPt");
  m_SF_Off60_match_hlt60_jetPt16 = (TGraph*)m_inFile16->Get("g_SF_offJets60_match_hlt_match_hlt60_jetPt");
  m_Eff_Off85_match_hlt85_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets85_match_hlt_match_hlt85_jetPt");
  m_Eff_Off85_match_hlt70_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets85_match_hlt_match_hlt70_jetPt");
  m_Eff_Off85_match_hlt60_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets85_match_hlt_match_hlt60_jetPt");
  m_Eff_Off77_match_hlt85_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets77_match_hlt_match_hlt85_jetPt");
  m_Eff_Off77_match_hlt70_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets77_match_hlt_match_hlt70_jetPt");
  m_Eff_Off77_match_hlt60_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets77_match_hlt_match_hlt60_jetPt");
  m_Eff_Off70_match_hlt85_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets70_match_hlt_match_hlt85_jetPt");
  m_Eff_Off70_match_hlt70_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets70_match_hlt_match_hlt70_jetPt");
  m_Eff_Off70_match_hlt60_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets70_match_hlt_match_hlt60_jetPt");
  m_Eff_Off60_match_hlt85_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets60_match_hlt_match_hlt85_jetPt");
  m_Eff_Off60_match_hlt70_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets60_match_hlt_match_hlt70_jetPt");
  m_Eff_Off60_match_hlt60_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets60_match_hlt_match_hlt60_jetPt");
  m_Eff_Off60_match_hlt40_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets60_match_hlt_match_hlt40_jetPt");
  m_Eff_Off70_match_hlt40_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets60_match_hlt_match_hlt70_jetPt");
  m_Eff_Off77_match_hlt40_jetPt16 = (TGraph*)m_inFile16->Get("g_Eff_offJets60_match_hlt_match_hlt77_jetPt");

  m_Eff_Event_leadingJet_jetEta    = (TGraph*)m_inFile16->Get("g_Eff_Event_leadingJet_jetEta");

  TString FileNameNN_adv_e100_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_adv_e100_l0.json");
  std::ifstream ifs_adv_e100_l0(FileNameNN_adv_e100_l0.Data(), std::ifstream::in);
  auto config_adv_e100_l0 = lwt::parse_json_graph(ifs_adv_e100_l0);
  lwtgraph_adv_e100_l0 = new lwt::LightweightGraph(config_adv_e100_l0);

  TString FileNameNN_nunez26 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_nunez26.json");
  std::ifstream ifs_nunez26(FileNameNN_nunez26.Data(), std::ifstream::in);
  auto config_nunez26 = lwt::parse_json_graph(ifs_nunez26);
  lwtgraph_nunez26 = new lwt::LightweightGraph(config_nunez26);

  TString FileNameNN_RE29 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_RE29.json");
  std::ifstream ifs_RE29(FileNameNN_RE29.Data(), std::ifstream::in);
  auto config_RE29 = lwt::parse_json_graph(ifs_RE29);
  lwtgraph_RE29 = new lwt::LightweightGraph(config_RE29);

  TString FileNameNN_RE34 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_RE34.json");
  std::ifstream ifs_RE34(FileNameNN_RE34.Data(), std::ifstream::in);
  auto config_RE34 = lwt::parse_json_graph(ifs_RE34);
  lwtgraph_RE34 = new lwt::LightweightGraph(config_RE34);

  TString FileNameNN_dataM_e100_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_dataM_e100_l0.json");
  std::ifstream ifs_dataM_e100_l0(FileNameNN_dataM_e100_l0.Data(), std::ifstream::in);
  auto config_dataM_e100_l0 = lwt::parse_json_graph(ifs_dataM_e100_l0);
  lwtgraph_dataM_e100_l0 = new lwt::LightweightGraph(config_dataM_e100_l0);

  TString FileNameNN_dataM_e100_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_dataM_e100_l10.json");
  std::ifstream ifs_dataM_e100_l10(FileNameNN_dataM_e100_l10.Data(), std::ifstream::in);
  auto config_dataM_e100_l10 = lwt::parse_json_graph(ifs_dataM_e100_l10);
  lwtgraph_dataM_e100_l10 = new lwt::LightweightGraph(config_dataM_e100_l10);

  TString FileNameNN_2025odd_e1000_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_2025odd_e1000_l10.json");
  std::ifstream ifs_2025odd_e1000_l10(FileNameNN_2025odd_e1000_l10.Data(), std::ifstream::in);
  auto config_2025odd_e1000_l10 = lwt::parse_json_graph(ifs_2025odd_e1000_l10);
  lwtgraph_2025odd_e1000_l10 = new lwt::LightweightGraph(config_2025odd_e1000_l10);

  TString FileNameNN_3c_2025odd_e1000_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_2025odd_e1000_l10.json");
  std::ifstream ifs_3c_2025odd_e1000_l10(FileNameNN_3c_2025odd_e1000_l10.Data(), std::ifstream::in);
  auto config_3c_2025odd_e1000_l10 = lwt::parse_json_graph(ifs_3c_2025odd_e1000_l10);
  lwtgraph_3c_2025odd_e1000_l10 = new lwt::LightweightGraph(config_3c_2025odd_e1000_l10);

  TString FileNameNN_2024odd_e1000_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_2024odd_e1000_l0.json");
  std::ifstream ifs_2024odd_e1000_l0(FileNameNN_2024odd_e1000_l0.Data(), std::ifstream::in);
  auto config_2024odd_e1000_l0 = lwt::parse_json_graph(ifs_2024odd_e1000_l0);
  lwtgraph_2024odd_e1000_l0 = new lwt::LightweightGraph(config_2024odd_e1000_l0);

  TString FileNameNN_3c_2024odd_e1000_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_2024odd_e1000_l0.json");
  std::ifstream ifs_3c_2024odd_e1000_l0(FileNameNN_3c_2024odd_e1000_l0.Data(), std::ifstream::in);
  auto config_3c_2024odd_e1000_l0 = lwt::parse_json_graph(ifs_3c_2024odd_e1000_l0);
  lwtgraph_3c_2024odd_e1000_l0 = new lwt::LightweightGraph(config_3c_2024odd_e1000_l0);

  TString FileNameNN_4100w2k0_e1000_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4100w2k0_e1000_l10.json");
  std::ifstream ifs_4100w2k0_e1000_l10(FileNameNN_4100w2k0_e1000_l10.Data(), std::ifstream::in);
  auto config_4100w2k0_e1000_l10 = lwt::parse_json_graph(ifs_4100w2k0_e1000_l10);
  lwtgraph_4100w2k0_e1000_l10 = new lwt::LightweightGraph(config_4100w2k0_e1000_l10);

  TString FileNameNN_4100w2k1_e1000_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4100w2k1_e1000_l10.json");
  std::ifstream ifs_4100w2k1_e1000_l10(FileNameNN_4100w2k1_e1000_l10.Data(), std::ifstream::in);
  auto config_4100w2k1_e1000_l10 = lwt::parse_json_graph(ifs_4100w2k1_e1000_l10);
  lwtgraph_4100w2k1_e1000_l10 = new lwt::LightweightGraph(config_4100w2k1_e1000_l10);

  TString FileNameNN_4100w4k0_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4100w4k0_l10.json");
  std::ifstream ifs_4100w4k0_l10(FileNameNN_4100w4k0_l10.Data(), std::ifstream::in);
  auto config_4100w4k0_l10 = lwt::parse_json_graph(ifs_4100w4k0_l10);
  lwtgraph_4100w4k0_l10 = new lwt::LightweightGraph(config_4100w4k0_l10);

  TString FileNameNN_4100w4k1_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4100w4k1_l10.json");
  std::ifstream ifs_4100w4k1_l10(FileNameNN_4100w4k1_l10.Data(), std::ifstream::in);
  auto config_4100w4k1_l10 = lwt::parse_json_graph(ifs_4100w4k1_l10);
  lwtgraph_4100w4k1_l10 = new lwt::LightweightGraph(config_4100w4k1_l10);

  TString FileNameNN_4100w4k2_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4100w4k2_l10.json");
  std::ifstream ifs_4100w4k2_l10(FileNameNN_4100w4k2_l10.Data(), std::ifstream::in);
  auto config_4100w4k2_l10 = lwt::parse_json_graph(ifs_4100w4k2_l10);
  lwtgraph_4100w4k2_l10 = new lwt::LightweightGraph(config_4100w4k2_l10);

  TString FileNameNN_4100w4k3_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4100w4k3_l10.json");
  std::ifstream ifs_4100w4k3_l10(FileNameNN_4100w4k3_l10.Data(), std::ifstream::in);
  auto config_4100w4k3_l10 = lwt::parse_json_graph(ifs_4100w4k3_l10);
  lwtgraph_4100w4k3_l10 = new lwt::LightweightGraph(config_4100w4k3_l10);

  TString FileNameNN_4100w4k0_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4100w4k0_l0.json");
  std::ifstream ifs_4100w4k0_l0(FileNameNN_4100w4k0_l0.Data(), std::ifstream::in);
  auto config_4100w4k0_l0 = lwt::parse_json_graph(ifs_4100w4k0_l0);
  lwtgraph_4100w4k0_l0 = new lwt::LightweightGraph(config_4100w4k0_l0);

  TString FileNameNN_4100w4k1_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4100w4k1_l0.json");
  std::ifstream ifs_4100w4k1_l0(FileNameNN_4100w4k1_l0.Data(), std::ifstream::in);
  auto config_4100w4k1_l0 = lwt::parse_json_graph(ifs_4100w4k1_l0);
  lwtgraph_4100w4k1_l0 = new lwt::LightweightGraph(config_4100w4k1_l0);

  TString FileNameNN_4100w4k2_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4100w4k2_l0.json");
  std::ifstream ifs_4100w4k2_l0(FileNameNN_4100w4k2_l0.Data(), std::ifstream::in);
  auto config_4100w4k2_l0 = lwt::parse_json_graph(ifs_4100w4k2_l0);
  lwtgraph_4100w4k2_l0 = new lwt::LightweightGraph(config_4100w4k2_l0);

  TString FileNameNN_4100w4k3_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4100w4k3_l0.json");
  std::ifstream ifs_4100w4k3_l0(FileNameNN_4100w4k3_l0.Data(), std::ifstream::in);
  auto config_4100w4k3_l0 = lwt::parse_json_graph(ifs_4100w4k3_l0);
  lwtgraph_4100w4k3_l0 = new lwt::LightweightGraph(config_4100w4k3_l0);

  TString FileNameNN_4200w4k0_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4200w4k0_l10.json");
  std::ifstream ifs_4200w4k0_l10(FileNameNN_4200w4k0_l10.Data(), std::ifstream::in);
  auto config_4200w4k0_l10 = lwt::parse_json_graph(ifs_4200w4k0_l10);
  lwtgraph_4200w4k0_l10 = new lwt::LightweightGraph(config_4200w4k0_l10);

  TString FileNameNN_4200w4k1_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4200w4k1_l10.json");
  std::ifstream ifs_4200w4k1_l10(FileNameNN_4200w4k1_l10.Data(), std::ifstream::in);
  auto config_4200w4k1_l10 = lwt::parse_json_graph(ifs_4200w4k1_l10);
  lwtgraph_4200w4k1_l10 = new lwt::LightweightGraph(config_4200w4k1_l10);

  TString FileNameNN_4200w4k2_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4200w4k2_l10.json");
  std::ifstream ifs_4200w4k2_l10(FileNameNN_4200w4k2_l10.Data(), std::ifstream::in);
  auto config_4200w4k2_l10 = lwt::parse_json_graph(ifs_4200w4k2_l10);
  lwtgraph_4200w4k2_l10 = new lwt::LightweightGraph(config_4200w4k2_l10);

  TString FileNameNN_4200w4k3_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4200w4k3_l10.json");
  std::ifstream ifs_4200w4k3_l10(FileNameNN_4200w4k3_l10.Data(), std::ifstream::in);
  auto config_4200w4k3_l10 = lwt::parse_json_graph(ifs_4200w4k3_l10);
  lwtgraph_4200w4k3_l10 = new lwt::LightweightGraph(config_4200w4k3_l10);

  TString FileNameNN_4200w2k0_e1000_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4200w2k0_e1000_l10.json");
  std::ifstream ifs_4200w2k0_e1000_l10(FileNameNN_4200w2k0_e1000_l10.Data(), std::ifstream::in);
  auto config_4200w2k0_e1000_l10 = lwt::parse_json_graph(ifs_4200w2k0_e1000_l10);
  lwtgraph_4200w2k0_e1000_l10 = new lwt::LightweightGraph(config_4200w2k0_e1000_l10);

  TString FileNameNN_4200w2k1_e1000_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_4200w2k1_e1000_l10.json");
  std::ifstream ifs_4200w2k1_e1000_l10(FileNameNN_4200w2k1_e1000_l10.Data(), std::ifstream::in);
  auto config_4200w2k1_e1000_l10 = lwt::parse_json_graph(ifs_4200w2k1_e1000_l10);
  lwtgraph_4200w2k1_e1000_l10 = new lwt::LightweightGraph(config_4200w2k1_e1000_l10);

  TString FileNameNN_3000odd_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000odd_l0.json");
  std::ifstream ifs_3000odd_l0(FileNameNN_3000odd_l0.Data(), std::ifstream::in);
  auto config_3000odd_l0 = lwt::parse_json_graph(ifs_3000odd_l0);
  lwtgraph_3000odd_l0 = new lwt::LightweightGraph(config_3000odd_l0);

  TString FileNameNN_3000even_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000even_l0.json");
  std::ifstream ifs_3000even_l0(FileNameNN_3000even_l0.Data(), std::ifstream::in);
  auto config_3000even_l0 = lwt::parse_json_graph(ifs_3000even_l0);
  lwtgraph_3000even_l0 = new lwt::LightweightGraph(config_3000even_l0);

  TString FileNameNN_3000odd_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000odd_l10.json");
  std::ifstream ifs_3000odd_l10(FileNameNN_3000odd_l10.Data(), std::ifstream::in);
  auto config_3000odd_l10 = lwt::parse_json_graph(ifs_3000odd_l10);
  lwtgraph_3000odd_l10 = new lwt::LightweightGraph(config_3000odd_l10);

  TString FileNameNN_3000even_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000even_l10.json");
  std::ifstream ifs_3000even_l10(FileNameNN_3000even_l10.Data(), std::ifstream::in);
  auto config_3000even_l10 = lwt::parse_json_graph(ifs_3000even_l10);
  lwtgraph_3000even_l10 = new lwt::LightweightGraph(config_3000even_l10);

  TString FileNameNN_3000odd_l20 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000odd_l20.json");
  std::ifstream ifs_3000odd_l20(FileNameNN_3000odd_l20.Data(), std::ifstream::in);
  auto config_3000odd_l20 = lwt::parse_json_graph(ifs_3000odd_l20);
  lwtgraph_3000odd_l20 = new lwt::LightweightGraph(config_3000odd_l20);

  TString FileNameNN_3000even_l20 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000even_l20.json");
  std::ifstream ifs_3000even_l20(FileNameNN_3000even_l20.Data(), std::ifstream::in);
  auto config_3000even_l20 = lwt::parse_json_graph(ifs_3000even_l20);
  lwtgraph_3000even_l20 = new lwt::LightweightGraph(config_3000even_l20);

  TString FileNameNN_3000odd_l40 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000odd_l40.json");
  std::ifstream ifs_3000odd_l40(FileNameNN_3000odd_l40.Data(), std::ifstream::in);
  auto config_3000odd_l40 = lwt::parse_json_graph(ifs_3000odd_l40);
  lwtgraph_3000odd_l40 = new lwt::LightweightGraph(config_3000odd_l40);

  TString FileNameNN_3000even_l40 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000even_l40.json");
  std::ifstream ifs_3000even_l40(FileNameNN_3000even_l40.Data(), std::ifstream::in);
  auto config_3000even_l40 = lwt::parse_json_graph(ifs_3000even_l40);
  lwtgraph_3000even_l40 = new lwt::LightweightGraph(config_3000even_l40);

  TString FileNameNN_3000odd_l60 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000odd_l60.json");
  std::ifstream ifs_3000odd_l60(FileNameNN_3000odd_l60.Data(), std::ifstream::in);
  auto config_3000odd_l60 = lwt::parse_json_graph(ifs_3000odd_l60);
  lwtgraph_3000odd_l60 = new lwt::LightweightGraph(config_3000odd_l60);

  TString FileNameNN_3000even_l60 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000even_l60.json");
  std::ifstream ifs_3000even_l60(FileNameNN_3000even_l60.Data(), std::ifstream::in);
  auto config_3000even_l60 = lwt::parse_json_graph(ifs_3000even_l60);
  lwtgraph_3000even_l60 = new lwt::LightweightGraph(config_3000even_l60);

  TString FileNameNN_3000odd_l80 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000odd_l80.json");
  std::ifstream ifs_3000odd_l80(FileNameNN_3000odd_l80.Data(), std::ifstream::in);
  auto config_3000odd_l80 = lwt::parse_json_graph(ifs_3000odd_l80);
  lwtgraph_3000odd_l80 = new lwt::LightweightGraph(config_3000odd_l80);

  TString FileNameNN_3000even_l80 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000even_l80.json");
  std::ifstream ifs_3000even_l80(FileNameNN_3000even_l80.Data(), std::ifstream::in);
  auto config_3000even_l80 = lwt::parse_json_graph(ifs_3000even_l80);
  lwtgraph_3000even_l80 = new lwt::LightweightGraph(config_3000even_l80);

  TString FileNameNN_3000rodd_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000rodd_l0.json");
  std::ifstream ifs_3000rodd_l0(FileNameNN_3000rodd_l0.Data(), std::ifstream::in);
  auto config_3000rodd_l0 = lwt::parse_json_graph(ifs_3000rodd_l0);
  lwtgraph_3000rodd_l0 = new lwt::LightweightGraph(config_3000rodd_l0);

  TString FileNameNN_3000reven_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000reven_l0.json");
  std::ifstream ifs_3000reven_l0(FileNameNN_3000reven_l0.Data(), std::ifstream::in);
  auto config_3000reven_l0 = lwt::parse_json_graph(ifs_3000reven_l0);
  lwtgraph_3000reven_l0 = new lwt::LightweightGraph(config_3000reven_l0);

  TString FileNameNN_3000rodd_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000rodd_l10.json");
  std::ifstream ifs_3000rodd_l10(FileNameNN_3000rodd_l10.Data(), std::ifstream::in);
  auto config_3000rodd_l10 = lwt::parse_json_graph(ifs_3000rodd_l10);
  lwtgraph_3000rodd_l10 = new lwt::LightweightGraph(config_3000rodd_l10);

  TString FileNameNN_3000reven_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000reven_l10.json");
  std::ifstream ifs_3000reven_l10(FileNameNN_3000reven_l10.Data(), std::ifstream::in);
  auto config_3000reven_l10 = lwt::parse_json_graph(ifs_3000reven_l10);
  lwtgraph_3000reven_l10 = new lwt::LightweightGraph(config_3000reven_l10);

  TString FileNameNN_3000rodd_l20 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000rodd_l20.json");
  std::ifstream ifs_3000rodd_l20(FileNameNN_3000rodd_l20.Data(), std::ifstream::in);
  auto config_3000rodd_l20 = lwt::parse_json_graph(ifs_3000rodd_l20);
  lwtgraph_3000rodd_l20 = new lwt::LightweightGraph(config_3000rodd_l20);

  TString FileNameNN_3000reven_l20 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3000reven_l20.json");
  std::ifstream ifs_3000reven_l20(FileNameNN_3000reven_l20.Data(), std::ifstream::in);
  auto config_3000reven_l20 = lwt::parse_json_graph(ifs_3000reven_l20);
  lwtgraph_3000reven_l20 = new lwt::LightweightGraph(config_3000reven_l20);

  TString FileNameNN_3c_3000rodd_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000rodd_l0.json");
  std::ifstream ifs_3c_3000rodd_l0(FileNameNN_3c_3000rodd_l0.Data(), std::ifstream::in);
  auto config_3c_3000rodd_l0 = lwt::parse_json_graph(ifs_3c_3000rodd_l0);
  lwtgraph_3c_3000rodd_l0 = new lwt::LightweightGraph(config_3c_3000rodd_l0);

  TString FileNameNN_3c_3000reven_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000reven_l0.json");
  std::ifstream ifs_3c_3000reven_l0(FileNameNN_3c_3000reven_l0.Data(), std::ifstream::in);
  auto config_3c_3000reven_l0 = lwt::parse_json_graph(ifs_3c_3000reven_l0);
  lwtgraph_3c_3000reven_l0 = new lwt::LightweightGraph(config_3c_3000reven_l0);

  TString FileNameNN_3c_3000rodd_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000rodd_l10.json");
  std::ifstream ifs_3c_3000rodd_l10(FileNameNN_3c_3000rodd_l10.Data(), std::ifstream::in);
  auto config_3c_3000rodd_l10 = lwt::parse_json_graph(ifs_3c_3000rodd_l10);
  lwtgraph_3c_3000rodd_l10 = new lwt::LightweightGraph(config_3c_3000rodd_l10);

  TString FileNameNN_3c_3000reven_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000reven_l10.json");
  std::ifstream ifs_3c_3000reven_l10(FileNameNN_3c_3000reven_l10.Data(), std::ifstream::in);
  auto config_3c_3000reven_l10 = lwt::parse_json_graph(ifs_3c_3000reven_l10);
  lwtgraph_3c_3000reven_l10 = new lwt::LightweightGraph(config_3c_3000reven_l10);

  TString FileNameNN_3c_3000rodd_l20 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000rodd_l20.json");
  std::ifstream ifs_3c_3000rodd_l20(FileNameNN_3c_3000rodd_l20.Data(), std::ifstream::in);
  auto config_3c_3000rodd_l20 = lwt::parse_json_graph(ifs_3c_3000rodd_l20);
  lwtgraph_3c_3000rodd_l20 = new lwt::LightweightGraph(config_3c_3000rodd_l20);

  TString FileNameNN_3c_3000reven_l20 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000reven_l20.json");
  std::ifstream ifs_3c_3000reven_l20(FileNameNN_3c_3000reven_l20.Data(), std::ifstream::in);
  auto config_3c_3000reven_l20 = lwt::parse_json_graph(ifs_3c_3000reven_l20);
  lwtgraph_3c_3000reven_l20 = new lwt::LightweightGraph(config_3c_3000reven_l20);

  TString FileNameNN_3c_3000odd_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000odd_l0.json");
  std::ifstream ifs_3c_3000odd_l0(FileNameNN_3c_3000odd_l0.Data(), std::ifstream::in);
  auto config_3c_3000odd_l0 = lwt::parse_json_graph(ifs_3c_3000odd_l0);
  lwtgraph_3c_3000odd_l0 = new lwt::LightweightGraph(config_3c_3000odd_l0);

  TString FileNameNN_3c_3000even_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000even_l0.json");
  std::ifstream ifs_3c_3000even_l0(FileNameNN_3c_3000even_l0.Data(), std::ifstream::in);
  auto config_3c_3000even_l0 = lwt::parse_json_graph(ifs_3c_3000even_l0);
  lwtgraph_3c_3000even_l0 = new lwt::LightweightGraph(config_3c_3000even_l0);

  TString FileNameNN_3c_3000odd_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000odd_l10.json");
  std::ifstream ifs_3c_3000odd_l10(FileNameNN_3c_3000odd_l10.Data(), std::ifstream::in);
  auto config_3c_3000odd_l10 = lwt::parse_json_graph(ifs_3c_3000odd_l10);
  lwtgraph_3c_3000odd_l10 = new lwt::LightweightGraph(config_3c_3000odd_l10);

  TString FileNameNN_3c_3000even_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000even_l10.json");
  std::ifstream ifs_3c_3000even_l10(FileNameNN_3c_3000even_l10.Data(), std::ifstream::in);
  auto config_3c_3000even_l10 = lwt::parse_json_graph(ifs_3c_3000even_l10);
  lwtgraph_3c_3000even_l10 = new lwt::LightweightGraph(config_3c_3000even_l10);

  TString FileNameNN_3c_3000odd_l20 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000odd_l20.json");
  std::ifstream ifs_3c_3000odd_l20(FileNameNN_3c_3000odd_l20.Data(), std::ifstream::in);
  auto config_3c_3000odd_l20 = lwt::parse_json_graph(ifs_3c_3000odd_l20);
  lwtgraph_3c_3000odd_l20 = new lwt::LightweightGraph(config_3c_3000odd_l20);

  TString FileNameNN_3c_3000even_l20 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000even_l20.json");
  std::ifstream ifs_3c_3000even_l20(FileNameNN_3c_3000even_l20.Data(), std::ifstream::in);
  auto config_3c_3000even_l20 = lwt::parse_json_graph(ifs_3c_3000even_l20);
  lwtgraph_3c_3000even_l20 = new lwt::LightweightGraph(config_3c_3000even_l20);

  TString FileNameNN_3c_3000odd_l40 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000odd_l40.json");
  std::ifstream ifs_3c_3000odd_l40(FileNameNN_3c_3000odd_l40.Data(), std::ifstream::in);
  auto config_3c_3000odd_l40 = lwt::parse_json_graph(ifs_3c_3000odd_l40);
  lwtgraph_3c_3000odd_l40 = new lwt::LightweightGraph(config_3c_3000odd_l40);

  TString FileNameNN_3c_3000even_l40 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000even_l40.json");
  std::ifstream ifs_3c_3000even_l40(FileNameNN_3c_3000even_l40.Data(), std::ifstream::in);
  auto config_3c_3000even_l40 = lwt::parse_json_graph(ifs_3c_3000even_l40);
  lwtgraph_3c_3000even_l40 = new lwt::LightweightGraph(config_3c_3000even_l40);

  TString FileNameNN_3c_3000odd_l100 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000odd_l100.json");
  std::ifstream ifs_3c_3000odd_l100(FileNameNN_3c_3000odd_l100.Data(), std::ifstream::in);
  auto config_3c_3000odd_l100 = lwt::parse_json_graph(ifs_3c_3000odd_l100);
  lwtgraph_3c_3000odd_l100 = new lwt::LightweightGraph(config_3c_3000odd_l100);

  TString FileNameNN_3c_3000even_l100 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_3000even_l100.json");
  std::ifstream ifs_3c_3000even_l100(FileNameNN_3c_3000even_l100.Data(), std::ifstream::in);
  auto config_3c_3000even_l100 = lwt::parse_json_graph(ifs_3c_3000even_l100);
  lwtgraph_3c_3000even_l100 = new lwt::LightweightGraph(config_3c_3000even_l100);



  TString FileNameNN_3c_4100w2k0_e1000_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4100w2k0_e1000_l10.json");
  std::ifstream ifs_3c_4100w2k0_e1000_l10(FileNameNN_3c_4100w2k0_e1000_l10.Data(), std::ifstream::in);
  auto config_3c_4100w2k0_e1000_l10 = lwt::parse_json_graph(ifs_3c_4100w2k0_e1000_l10);
  lwtgraph_3c_4100w2k0_e1000_l10 = new lwt::LightweightGraph(config_3c_4100w2k0_e1000_l10);

  TString FileNameNN_3c_4100w4k0_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4100w4k0_l10.json");
  std::ifstream ifs_3c_4100w4k0_l10(FileNameNN_3c_4100w4k0_l10.Data(), std::ifstream::in);
  auto config_3c_4100w4k0_l10 = lwt::parse_json_graph(ifs_3c_4100w4k0_l10);
  lwtgraph_3c_4100w4k0_l10 = new lwt::LightweightGraph(config_3c_4100w4k0_l10);

  TString FileNameNN_3c_4100w4k1_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4100w4k1_l10.json");
  std::ifstream ifs_3c_4100w4k1_l10(FileNameNN_3c_4100w4k1_l10.Data(), std::ifstream::in);
  auto config_3c_4100w4k1_l10 = lwt::parse_json_graph(ifs_3c_4100w4k1_l10);
  lwtgraph_3c_4100w4k1_l10 = new lwt::LightweightGraph(config_3c_4100w4k1_l10);

  TString FileNameNN_3c_4100w4k2_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4100w4k2_l10.json");
  std::ifstream ifs_3c_4100w4k2_l10(FileNameNN_3c_4100w4k2_l10.Data(), std::ifstream::in);
  auto config_3c_4100w4k2_l10 = lwt::parse_json_graph(ifs_3c_4100w4k2_l10);
  lwtgraph_3c_4100w4k2_l10 = new lwt::LightweightGraph(config_3c_4100w4k2_l10);

  TString FileNameNN_3c_4100w4k3_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4100w4k3_l10.json");
  std::ifstream ifs_3c_4100w4k3_l10(FileNameNN_3c_4100w4k3_l10.Data(), std::ifstream::in);
  auto config_3c_4100w4k3_l10 = lwt::parse_json_graph(ifs_3c_4100w4k3_l10);
  lwtgraph_3c_4100w4k3_l10 = new lwt::LightweightGraph(config_3c_4100w4k3_l10);

  TString FileNameNN_3c_4100w4k0_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4100w4k0_l0.json");
  std::ifstream ifs_3c_4100w4k0_l0(FileNameNN_3c_4100w4k0_l0.Data(), std::ifstream::in);
  auto config_3c_4100w4k0_l0 = lwt::parse_json_graph(ifs_3c_4100w4k0_l0);
  lwtgraph_3c_4100w4k0_l0 = new lwt::LightweightGraph(config_3c_4100w4k0_l0);

  TString FileNameNN_3c_4100w4k1_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4100w4k1_l0.json");
  std::ifstream ifs_3c_4100w4k1_l0(FileNameNN_3c_4100w4k1_l0.Data(), std::ifstream::in);
  auto config_3c_4100w4k1_l0 = lwt::parse_json_graph(ifs_3c_4100w4k1_l0);
  lwtgraph_3c_4100w4k1_l0 = new lwt::LightweightGraph(config_3c_4100w4k1_l0);

  TString FileNameNN_3c_4100w4k2_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4100w4k2_l0.json");
  std::ifstream ifs_3c_4100w4k2_l0(FileNameNN_3c_4100w4k2_l0.Data(), std::ifstream::in);
  auto config_3c_4100w4k2_l0 = lwt::parse_json_graph(ifs_3c_4100w4k2_l0);
  lwtgraph_3c_4100w4k2_l0 = new lwt::LightweightGraph(config_3c_4100w4k2_l0);

  TString FileNameNN_3c_4100w4k3_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4100w4k3_l0.json");
  std::ifstream ifs_3c_4100w4k3_l0(FileNameNN_3c_4100w4k3_l0.Data(), std::ifstream::in);
  auto config_3c_4100w4k3_l0 = lwt::parse_json_graph(ifs_3c_4100w4k3_l0);
  lwtgraph_3c_4100w4k3_l0 = new lwt::LightweightGraph(config_3c_4100w4k3_l0);

  TString FileNameNN_3c_4200w4k0_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4200w4k0_l10.json");
  std::ifstream ifs_3c_4200w4k0_l10(FileNameNN_3c_4200w4k0_l10.Data(), std::ifstream::in);
  auto config_3c_4200w4k0_l10 = lwt::parse_json_graph(ifs_3c_4200w4k0_l10);
  lwtgraph_3c_4200w4k0_l10 = new lwt::LightweightGraph(config_3c_4200w4k0_l10);

  TString FileNameNN_3c_4200w4k1_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4200w4k1_l10.json");
  std::ifstream ifs_3c_4200w4k1_l10(FileNameNN_3c_4200w4k1_l10.Data(), std::ifstream::in);
  auto config_3c_4200w4k1_l10 = lwt::parse_json_graph(ifs_3c_4200w4k1_l10);
  lwtgraph_3c_4200w4k1_l10 = new lwt::LightweightGraph(config_3c_4200w4k1_l10);

  TString FileNameNN_3c_4200w4k2_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4200w4k2_l10.json");
  std::ifstream ifs_3c_4200w4k2_l10(FileNameNN_3c_4200w4k2_l10.Data(), std::ifstream::in);
  auto config_3c_4200w4k2_l10 = lwt::parse_json_graph(ifs_3c_4200w4k2_l10);
  lwtgraph_3c_4200w4k2_l10 = new lwt::LightweightGraph(config_3c_4200w4k2_l10);

  TString FileNameNN_3c_4200w4k3_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4200w4k3_l10.json");
  std::ifstream ifs_3c_4200w4k3_l10(FileNameNN_3c_4200w4k3_l10.Data(), std::ifstream::in);
  auto config_3c_4200w4k3_l10 = lwt::parse_json_graph(ifs_3c_4200w4k3_l10);
  lwtgraph_3c_4200w4k3_l10 = new lwt::LightweightGraph(config_3c_4200w4k3_l10);

  TString FileNameNN_3c_4100w2k1_e1000_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4100w2k1_e1000_l10.json");
  std::ifstream ifs_3c_4100w2k1_e1000_l10(FileNameNN_3c_4100w2k1_e1000_l10.Data(), std::ifstream::in);
  auto config_3c_4100w2k1_e1000_l10 = lwt::parse_json_graph(ifs_3c_4100w2k1_e1000_l10);
  lwtgraph_3c_4100w2k1_e1000_l10 = new lwt::LightweightGraph(config_3c_4100w2k1_e1000_l10);

  TString FileNameNN_3c_4200w2k0_e1000_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4200w2k0_e1000_l10.json");
  std::ifstream ifs_3c_4200w2k0_e1000_l10(FileNameNN_3c_4200w2k0_e1000_l10.Data(), std::ifstream::in);
  auto config_3c_4200w2k0_e1000_l10 = lwt::parse_json_graph(ifs_3c_4200w2k0_e1000_l10);
  lwtgraph_3c_4200w2k0_e1000_l10 = new lwt::LightweightGraph(config_3c_4200w2k0_e1000_l10);

  TString FileNameNN_3c_4200w2k1_e1000_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_4200w2k1_e1000_l10.json");
  std::ifstream ifs_3c_4200w2k1_e1000_l10(FileNameNN_3c_4200w2k1_e1000_l10.Data(), std::ifstream::in);
  auto config_3c_4200w2k1_e1000_l10 = lwt::parse_json_graph(ifs_3c_4200w2k1_e1000_l10);
  lwtgraph_3c_4200w2k1_e1000_l10 = new lwt::LightweightGraph(config_3c_4200w2k1_e1000_l10);

  TString FileNameNN_3c_dataM800_e100_l0 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_dataM800_e100_l0.json");
  std::ifstream ifs_3c_dataM800_e100_l0(FileNameNN_3c_dataM800_e100_l0.Data(), std::ifstream::in);
  auto config_3c_dataM800_e100_l0 = lwt::parse_json_graph(ifs_3c_dataM800_e100_l0);
  lwtgraph_3c_dataM800_e100_l0 = new lwt::LightweightGraph(config_3c_dataM800_e100_l0);

  TString FileNameNN_3c_dataM800_e100_l10 = PathResolverFindCalibFile("CxAODReader_VBFHbb/lwtnn_3c_dataM800_e100_l10.json");
  std::ifstream ifs_3c_dataM800_e100_l10(FileNameNN_3c_dataM800_e100_l10.Data(), std::ifstream::in);
  auto config_3c_dataM800_e100_l10 = lwt::parse_json_graph(ifs_3c_dataM800_e100_l10);
  lwtgraph_3c_dataM800_e100_l10 = new lwt::LightweightGraph(config_3c_dataM800_e100_l10);

   reader2cen = new TMVA::Reader( "!Color:!Silent" );
   reader2cen->AddVariable( "mJJ", &m_bdt_mJJ );
   reader2cen->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader2cen->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader2cen->AddVariable( "mindRJ1_Ex", &m_bdt_mindRJ1_Ex );
   reader2cen->AddVariable( "mindRJ2_Ex", &m_bdt_mindRJ2_Ex );
   reader2cen->AddVariable( "eta_J_star", &m_bdt_eta_J_star );
   reader2cen->AddVariable( "cosTheta_boost", &m_bdt_cosTheta_boost );
   reader2cen->AddVariable( "deltaMJJ", &m_bdt_deltaMJJ );
   reader2cen->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader2cen->AddSpectator( "mJJ", &m_bdt_mJJ );
   reader2cen->AddSpectator( "pTJJ", &m_bdt_pTJJ );
   reader2cen->AddSpectator( "pT_balance", &m_bdt_pT_balance );
   reader2cen->AddSpectator( "mindRJ1_Ex", &m_bdt_mindRJ1_Ex );
   reader2cen->AddSpectator( "mindRJ2_Ex", &m_bdt_mindRJ2_Ex );
   reader2cen->AddSpectator( "eta_J_star", &m_bdt_eta_J_star );
   reader2cen->AddSpectator( "cosTheta_boost", &m_bdt_cosTheta_boost );
   reader2cen->AddSpectator( "deltaMJJ", &m_bdt_deltaMJJ );
   reader2cen->AddSpectator( "max_J1J2", &m_bdt_max_J1J2 );
   reader2cen->AddSpectator( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader2cen->AddSpectator( "mBB", &m_bdt_mBB );
   TString FileName2cen = PathResolverFindCalibFile("CxAODReader_VBFHbb/vbf_bdt_2cen/weights/events_BDT.weights.xml");
   reader2cen->BookMVA( "BDT method", FileName2cen );

   reader2cenWeight = new TMVA::Reader( "!Color:!Silent" );
   reader2cenWeight->AddVariable( "mJJ", &m_bdt_mJJ );
   reader2cenWeight->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader2cenWeight->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader2cenWeight->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader2cenWeight->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader2cenWeight->AddVariable( "deltaMJJ", &m_bdt_deltaMJJ );
   reader2cenWeight->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   TString FileName2cenWeight = PathResolverFindCalibFile("CxAODReader_VBFHbb/vbf_bdt_2cenWeight/weights/events_BDT.weights.xml");
   reader2cenWeight->BookMVA( "BDT method", FileName2cenWeight );

   reader2cenOldExt = new TMVA::Reader( "!Color:!Silent" );
   reader2cenOldExt->AddVariable( "mJJ", &m_bdt_mJJ );
   reader2cenOldExt->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader2cenOldExt->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader2cenOldExt->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader2cenOldExt->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader2cenOldExt->AddVariable( "deltaMJJ", &m_bdt_deltaMJJ );
   reader2cenOldExt->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader2cenOldExt->AddVariable( "centralMaxJ_Ex", &m_bdt_centralMaxJ_Ex );
   TString FileName2cenOldExt = PathResolverFindCalibFile("CxAODReader_VBFHbb/vbf_bdt_2cenOldExt/weights/events_BDT.weights.xml");
   reader2cenOldExt->BookMVA( "BDT method", FileName2cenOldExt );

   reader3cen = new TMVA::Reader( "!Color:!Silent" );
   reader3cen->AddVariable( "mJJ", &m_bdt_mJJ );
   reader3cen->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader3cen->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader3cen->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader3cen->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader3cen->AddVariable( "eta_J_star", &m_bdt_eta_J_star );
   reader3cen->AddVariable( "deltaMJJ", &m_bdt_deltaMJJ );
   reader3cen->AddVariable( "NTrk500PVJ1", &m_bdt_NTrk500PVJ1 );
   reader3cen->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   TString FileName3cen = PathResolverFindCalibFile("CxAODReader_VBFHbb/vbf_bdt_3cen/weights/events_BDT.weights.xml");
   reader3cen->BookMVA( "BDT method", FileName3cen );

   readerHighpt = new TMVA::Reader( "!Color:!Silent" );
   readerHighpt->AddVariable( "mJJ", &m_bdt_mJJ );
   readerHighpt->AddVariable( "pTJJ", &m_bdt_pTJJ );
   readerHighpt->AddVariable( "pT_balance", &m_bdt_pT_balance );
   readerHighpt->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   readerHighpt->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   readerHighpt->AddVariable( "eta_J_star", &m_bdt_eta_J_star );
   readerHighpt->AddVariable( "deltaMJJ", &m_bdt_deltaMJJ );
   readerHighpt->AddVariable( "NTrk500PVJ1", &m_bdt_NTrk500PVJ1 );
   readerHighpt->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   TString FileNameHighpt = PathResolverFindCalibFile("CxAODReader_VBFHbb/vbf_bdt_highpt/weights/events_BDT.weights.xml");
   readerHighpt->BookMVA( "BDT method", FileNameHighpt );

   readertopoInc = new TMVA::Reader( "!Color:!Silent" );
   readertopoInc->AddVariable( "mJJ", &m_bdt_mJJ );
   readertopoInc->AddVariable( "pTJJ", &m_bdt_pTJJ );
   readertopoInc->AddVariable( "pT_balance", &m_bdt_pT_balance );
   readertopoInc->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   readertopoInc->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   readertopoInc->AddVariable( "eta_J_star", &m_bdt_eta_J_star );
   readertopoInc->AddVariable( "deltaMJJ", &m_bdt_deltaMJJ );
   readertopoInc->AddVariable( "NTrk500PVJ1", &m_bdt_NTrk500PVJ1 );
   readertopoInc->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   TString FileNametopoInc = PathResolverFindCalibFile("CxAODReader_VBFHbb/vbf_bdt_topoInc/weights/events_BDT.weights.xml");
   readertopoInc->BookMVA( "BDT method", FileNametopoInc );

   reader2cenOld = new TMVA::Reader( "!Color:!Silent" );
   reader2cenOld->AddVariable( "mJJ", &m_bdt_mJJ );
   reader2cenOld->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader2cenOld->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader2cenOld->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader2cenOld->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader2cenOld->AddVariable( "deltaMJJ", &m_bdt_deltaMJJ );
   reader2cenOld->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   TString FileName2cenOld = PathResolverFindCalibFile("CxAODReader_VBFHbb/vbf_bdt_2cenOld/weights/events_BDT.weights.xml");
   reader2cenOld->BookMVA( "BDT method", FileName2cenOld );

   reader_mcmj = new TMVA::Reader( "!Color:!Silent" );
   reader_mcmj->AddVariable( "mJJ", &m_bdt_mJJ );
   reader_mcmj->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader_mcmj->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader_mcmj->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader_mcmj->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader_mcmj->AddVariable( "deltaMJJ", &m_bdt_deltaMJJ );
   reader_mcmj->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader_mcmj->AddVariable( "centralMaxJ_Ex", &m_bdt_centralMaxJ_Ex );
   reader_mcmj->AddVariable( "(pTJ1-pTJ2)/(pTJ1+pTJ2)", &m_bdt_asymJJ );
   reader_mcmj->AddVariable( "nJets", &m_bdt_nJets );
   reader_mcmj->AddVariable( "(fabs(dEtaBB)-fabs(dPhiBB))/(fabs(dEtaBB)+fabs(dPhiBB))", &m_bdt_angleAsymBB );
   //reader_mcmj->AddVariable( "dEtaJJ", &m_bdt_dEtaJJ );
   reader_mcmj->AddSpectator( "mBB", &m_bdt_mBB );
   reader_mcmj->AddSpectator( "pTJ1", &m_bdt_pTJ1 );
   TString FileName_mcmj = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmva_mcmj.xml");
   reader_mcmj->BookMVA( "BDT method", FileName_mcmj );

   reader_1for2cen_final = new TMVA::Reader( "!Color:!Silent" );
   reader_1for2cen_final->AddVariable( "mJJ", &m_bdt_mJJ );
   reader_1for2cen_final->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader_1for2cen_final->AddVariable( "asymJJ", &m_bdt_asymJJ );
   reader_1for2cen_final->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader_1for2cen_final->AddVariable( "dEtaBBJJ", &m_bdt_dEtaBBJJ );
   reader_1for2cen_final->AddVariable( "dPhiBBJJ", &m_bdt_dPhiBBJJ );
   reader_1for2cen_final->AddVariable( "angleAsymBB", &m_bdt_angleAsymBB );
   reader_1for2cen_final->AddVariable( "nJets20pt", &m_bdt_nJets20pt );
   reader_1for2cen_final->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader_1for2cen_final->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader_1for2cen_final->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader_1for2cen_final->AddSpectator( "mBB", &m_bdt_mBB );
   reader_1for2cen_final->AddSpectator( "pTJ1", &m_bdt_pTJ1 );
   reader_1for2cen_final->AddSpectator( "mBB_no_corr", &m_bdt_mBB_no_corr );
   TString FileName_1for2cen_final = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmva_1for2cen_final.xml");
   reader_1for2cen_final->BookMVA( "BDT method", FileName_1for2cen_final );

   reader_1for2cen_full = new TMVA::Reader( "!Color:!Silent" );
   reader_1for2cen_full->AddVariable( "mJJ", &m_bdt_mJJ );
   reader_1for2cen_full->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader_1for2cen_full->AddVariable( "asymJJ", &m_bdt_asymJJ );
   reader_1for2cen_full->AddVariable( "dEtaBBJJ", &m_bdt_dEtaBBJJ );
   reader_1for2cen_full->AddVariable( "dPhiBBJJ", &m_bdt_dPhiBBJJ );
   reader_1for2cen_full->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader_1for2cen_full->AddVariable( "angleAsymBB", &m_bdt_angleAsymBB );
   reader_1for2cen_full->AddVariable( "nJets20pt", &m_bdt_nJets20pt );
   reader_1for2cen_full->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader_1for2cen_full->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader_1for2cen_full->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader_1for2cen_full->AddSpectator( "mBB", &m_bdt_mBB );
   TString FileName_1for2cen_full = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmva_1for2cen_full.xml");
   reader_1for2cen_full->BookMVA( "BDT method", FileName_1for2cen_full );

   reader_1for2cen_trueasym = new TMVA::Reader( "!Color:!Silent" );
   reader_1for2cen_trueasym->AddVariable( "mJJ", &m_bdt_mJJ );
   reader_1for2cen_trueasym->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader_1for2cen_trueasym->AddVariable( "asymJJ", &m_bdt_asymJJ );
   reader_1for2cen_trueasym->AddVariable( "dEtaBBJJ", &m_bdt_dEtaBBJJ );
   reader_1for2cen_trueasym->AddVariable( "dPhiBBJJ", &m_bdt_dPhiBBJJ );
   reader_1for2cen_trueasym->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader_1for2cen_trueasym->AddVariable( "nJets20pt", &m_bdt_nJets20pt );
   reader_1for2cen_trueasym->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader_1for2cen_trueasym->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader_1for2cen_trueasym->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader_1for2cen_trueasym->AddVariable( "(cosh(dEtaBB)+cos(dPhiBB)-2)/(cosh(dEtaBB)-cos(dPhiBB))", &m_bdt_trueangleAsymBB );
   reader_1for2cen_trueasym->AddSpectator( "mBB", &m_bdt_mBB );
   TString FileName_1for2cen_trueasym = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmva_1for2cen_trueasym.xml");
   reader_1for2cen_trueasym->BookMVA( "BDT method", FileName_1for2cen_trueasym );

   reader_1for2cen_angle = new TMVA::Reader( "!Color:!Silent" );
   reader_1for2cen_angle->AddVariable( "mJJ", &m_bdt_mJJ );
   reader_1for2cen_angle->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader_1for2cen_angle->AddVariable( "asymJJ", &m_bdt_asymJJ );
   reader_1for2cen_angle->AddVariable( "dEtaBBJJ", &m_bdt_dEtaBBJJ );
   reader_1for2cen_angle->AddVariable( "dPhiBBJJ", &m_bdt_dPhiBBJJ );
   reader_1for2cen_angle->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader_1for2cen_angle->AddVariable( "nJets20pt", &m_bdt_nJets20pt );
   reader_1for2cen_angle->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader_1for2cen_angle->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader_1for2cen_angle->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader_1for2cen_angle->AddVariable( "atan(tan(dPhiBB/2)/tanh(dEtaBB/2))", &m_bdt_mBB_angle );
   reader_1for2cen_angle->AddSpectator( "mBB", &m_bdt_mBB );
   TString FileName_1for2cen_angle = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmva_1for2cen_angle.xml");
   reader_1for2cen_angle->BookMVA( "BDT method", FileName_1for2cen_angle );

   reader_1for2cen_nojets = new TMVA::Reader( "!Color:!Silent" );
   reader_1for2cen_nojets->AddVariable( "mJJ", &m_bdt_mJJ );
   reader_1for2cen_nojets->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader_1for2cen_nojets->AddVariable( "asymJJ", &m_bdt_asymJJ );
   reader_1for2cen_nojets->AddVariable( "dEtaBBJJ", &m_bdt_dEtaBBJJ );
   reader_1for2cen_nojets->AddVariable( "dPhiBBJJ", &m_bdt_dPhiBBJJ );
   reader_1for2cen_nojets->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader_1for2cen_nojets->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader_1for2cen_nojets->AddVariable( "mBB_angle", &m_bdt_mBB_angle );
   reader_1for2cen_nojets->AddSpectator( "mBB", &m_bdt_mBB );
   TString FileName_1for2cen_nojets = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmva_1for2cen_nojets.xml");
   reader_1for2cen_nojets->BookMVA( "BDT method", FileName_1for2cen_nojets );

   reader_1for2cen_nojetsrap = new TMVA::Reader( "!Color:!Silent" );
   reader_1for2cen_nojetsrap->AddVariable( "mJJ", &m_bdt_mJJ );
   reader_1for2cen_nojetsrap->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader_1for2cen_nojetsrap->AddVariable( "asymJJ", &m_bdt_asymJJ );
   reader_1for2cen_nojetsrap->AddVariable( "dEtaBBJJ", &m_bdt_dEtaBBJJ );
   reader_1for2cen_nojetsrap->AddVariable( "dPhiBBJJ", &m_bdt_dPhiBBJJ );
   reader_1for2cen_nojetsrap->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader_1for2cen_nojetsrap->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader_1for2cen_nojetsrap->AddVariable( "mBB_angle", &m_bdt_mBB_angle );
   reader_1for2cen_nojetsrap->AddSpectator( "mBB", &m_bdt_mBB );
   TString FileName_1for2cen_nojetsrap = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmva_1for2cen_nojetsrap.xml");
   reader_1for2cen_nojetsrap->BookMVA( "BDT method", FileName_1for2cen_nojetsrap );

   reader_1for2cen_pflow_5jet = new TMVA::Reader( "!Color:!Silent" );
   reader_1for2cen_pflow_5jet->AddVariable( "mJJ", &m_bdt_mJJ );
   reader_1for2cen_pflow_5jet->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader_1for2cen_pflow_5jet->AddVariable( "asymJJ", &m_bdt_asymJJ );
   reader_1for2cen_pflow_5jet->AddVariable( "dEtaBBJJ", &m_bdt_dEtaBBJJ );
   reader_1for2cen_pflow_5jet->AddVariable( "dPhiBBJJ", &m_bdt_dPhiBBJJ );
   reader_1for2cen_pflow_5jet->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader_1for2cen_pflow_5jet->AddVariable( "inclusiveMaxJ_Ex", &m_bdt_inclusiveMaxJ_Ex );
   reader_1for2cen_pflow_5jet->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader_1for2cen_pflow_5jet->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader_1for2cen_pflow_5jet->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader_1for2cen_pflow_5jet->AddVariable( "mBB_angle", &m_bdt_mBB_angle );
   reader_1for2cen_pflow_5jet->AddSpectator( "mBB", &m_bdt_mBB );
   TString FileName_1for2cen_pflow_5jet = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmva_1for2cen_pflow_5jet.xml");
   reader_1for2cen_pflow_5jet->BookMVA( "BDT method", FileName_1for2cen_pflow_5jet );

   reader_1for2cen_pflow = new TMVA::Reader( "!Color:!Silent" );
   reader_1for2cen_pflow->AddVariable( "mJJ", &m_bdt_mJJ );
   reader_1for2cen_pflow->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader_1for2cen_pflow->AddVariable( "asymJJ", &m_bdt_asymJJ );
   reader_1for2cen_pflow->AddVariable( "dEtaBBJJ", &m_bdt_dEtaBBJJ );
   reader_1for2cen_pflow->AddVariable( "dPhiBBJJ", &m_bdt_dPhiBBJJ );
   reader_1for2cen_pflow->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader_1for2cen_pflow->AddVariable( "nJets20pt", &m_bdt_nJets20pt );
   reader_1for2cen_pflow->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader_1for2cen_pflow->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader_1for2cen_pflow->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader_1for2cen_pflow->AddVariable( "mBB_angle", &m_bdt_mBB_angle );
   reader_1for2cen_pflow->AddSpectator( "mBB", &m_bdt_mBB );
   TString FileName_1for2cen_pflow = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmva_1for2cen_pflow.xml");
   reader_1for2cen_pflow->BookMVA( "BDT method", FileName_1for2cen_pflow );

   reader_1for2cen_pflow_data = new TMVA::Reader( "!Color:!Silent" );
   reader_1for2cen_pflow_data->AddVariable( "mJJ", &m_bdt_mJJ );
   reader_1for2cen_pflow_data->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader_1for2cen_pflow_data->AddVariable( "asymJJ", &m_bdt_asymJJ );
   reader_1for2cen_pflow_data->AddVariable( "dEtaBBJJ", &m_bdt_dEtaBBJJ );
   reader_1for2cen_pflow_data->AddVariable( "dPhiBBJJ", &m_bdt_dPhiBBJJ );
   reader_1for2cen_pflow_data->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader_1for2cen_pflow_data->AddVariable( "nJets20pt", &m_bdt_nJets20pt );
   reader_1for2cen_pflow_data->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader_1for2cen_pflow_data->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader_1for2cen_pflow_data->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader_1for2cen_pflow_data->AddVariable( "mBB_angle", &m_bdt_mBB_angle );
   reader_1for2cen_pflow_data->AddSpectator( "mBB", &m_bdt_mBB );
   TString FileName_1for2cen_pflow_data = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmva_1for2cen_pflow_data.xml");
   reader_1for2cen_pflow_data->BookMVA( "BDT method", FileName_1for2cen_pflow_data );

   reader_walrus_pflow_data = new TMVA::Reader( "!Color:!Silent" );
   reader_walrus_pflow_data->AddVariable( "mJJ", &m_bdt_mJJ );
   reader_walrus_pflow_data->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader_walrus_pflow_data->AddVariable( "asymJJ", &m_bdt_asymJJ );
   reader_walrus_pflow_data->AddVariable( "dEtaBBJJ", &m_bdt_dEtaBBJJ );
   reader_walrus_pflow_data->AddVariable( "dPhiBBJJ", &m_bdt_dPhiBBJJ );
   reader_walrus_pflow_data->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader_walrus_pflow_data->AddVariable( "nJets20pt", &m_bdt_nJets20pt );
   reader_walrus_pflow_data->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader_walrus_pflow_data->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader_walrus_pflow_data->AddVariable( "NTrk500PVJ1", &m_bdt_NTrk500PVJ1 );
   reader_walrus_pflow_data->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader_walrus_pflow_data->AddVariable( "mBB_angle", &m_bdt_mBB_angle );
   reader_walrus_pflow_data->AddSpectator( "mBB", &m_bdt_mBB );
   TString FileName_walrus_pflow_data = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmva_walrus_pflow_data.xml");
   reader_walrus_pflow_data->BookMVA( "BDT method", FileName_walrus_pflow_data );

   reader_walrus_final = new TMVA::Reader( "!Color:!Silent" );
   reader_walrus_final->AddVariable( "mJJ", &m_bdt_mJJ );
   reader_walrus_final->AddVariable( "pTJJ", &m_bdt_pTJJ );
   reader_walrus_final->AddVariable( "asymJJ", &m_bdt_asymJJ );
   reader_walrus_final->AddVariable( "pT_balance", &m_bdt_pT_balance );
   reader_walrus_final->AddVariable( "dEtaBBJJ", &m_bdt_dEtaBBJJ );
   reader_walrus_final->AddVariable( "dPhiBBJJ", &m_bdt_dPhiBBJJ );
   reader_walrus_final->AddVariable( "angleAsymBB", &m_bdt_angleAsymBB );
   reader_walrus_final->AddVariable( "nJets20pt", &m_bdt_nJets20pt );
   reader_walrus_final->AddVariable( "min(mindRJ1_Ex,10)", &m_bdt_min_mindRJ1_Ex );
   reader_walrus_final->AddVariable( "min(mindRJ2_Ex,10)", &m_bdt_min_mindRJ2_Ex );
   reader_walrus_final->AddVariable( "NTrk500PVJ1", &m_bdt_NTrk500PVJ1 );
   reader_walrus_final->AddVariable( "NTrk500PVJ2", &m_bdt_NTrk500PVJ2 );
   reader_walrus_final->AddSpectator( "mBB", &m_bdt_mBB );
   reader_walrus_final->AddSpectator( "bdt_walrus_final", &m_bdt_mBB );
   TString FileName_walrus_final = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmva_walrus_final.xml");
   reader_walrus_final->BookMVA( "BDT method", FileName_walrus_final );

   TString FileNameForwardSF2016 = PathResolverFindCalibFile("CxAODReader_VBFHbb/SF_1for2cen_2016J1.root");
   TFile *forwardSF2016_file = new TFile(FileNameForwardSF2016.Data(), "READ");
   m_forwardSF2016 = (TH2D*)forwardSF2016_file->Get("SF_1for2cen_2016J1");
   m_forwardSF2016->SetDirectory(0);
   TString FileNameForwardSF2017 = PathResolverFindCalibFile("CxAODReader_VBFHbb/SF_1for2cen_2017J1.root");
   TFile *forwardSF2017_file = new TFile(FileNameForwardSF2017.Data(), "READ");
   m_forwardSF2017 = (TH2D*)forwardSF2017_file->Get("SF_1for2cen_2017J1");
   m_forwardSF2017->SetDirectory(0);
   TString FileNameForwardSF2018 = PathResolverFindCalibFile("CxAODReader_VBFHbb/SF_1for2cen_2018J1.root");
   TFile *forwardSF2018_file = new TFile(FileNameForwardSF2018.Data(), "READ");
   m_forwardSF2018 = (TH2D*)forwardSF2018_file->Get("SF_1for2cen_2018J1");
   m_forwardSF2018->SetDirectory(0);

   TString FileNameCentralSF2016 = PathResolverFindCalibFile("CxAODReader_VBFHbb/SF_3cen_2016J1.root");
   TFile *centralSF2016_file = new TFile(FileNameCentralSF2016.Data(), "READ");
   m_centralSF2016 = (TH2D*)centralSF2016_file->Get("SF_3cen_2016J1");
   m_centralSF2016->SetDirectory(0);
   TString FileNameCentralSF2017 = PathResolverFindCalibFile("CxAODReader_VBFHbb/SF_3cen_2017J1.root");
   TFile *centralSF2017_file = new TFile(FileNameCentralSF2017.Data(), "READ");
   m_centralSF2017 = (TH2D*)centralSF2017_file->Get("SF_3cen_2017J1");
   m_centralSF2017->SetDirectory(0);
   TString FileNameCentralSF2018 = PathResolverFindCalibFile("CxAODReader_VBFHbb/SF_3cen_2018J1.root");
   TFile *centralSF2018_file = new TFile(FileNameCentralSF2018.Data(), "READ");
   m_centralSF2018 = (TH2D*)centralSF2018_file->Get("SF_3cen_2018J1");
   m_centralSF2018->SetDirectory(0);

   TString FileNameForwardEff2016 = PathResolverFindCalibFile("CxAODReader_VBFHbb/Eff_1for2cen_2016J1.root");
   TFile *forwardEff2016_file = new TFile(FileNameForwardEff2016.Data(), "READ");
   m_forwardEff2016 = (TH2D*)forwardEff2016_file->Get("Eff_1for2cen_2016J1");
   m_forwardEff2016->SetDirectory(0);
   TString FileNameForwardEff2017 = PathResolverFindCalibFile("CxAODReader_VBFHbb/Eff_1for2cen_2017J1.root");
   TFile *forwardEff2017_file = new TFile(FileNameForwardEff2017.Data(), "READ");
   m_forwardEff2017 = (TH2D*)forwardEff2017_file->Get("Eff_1for2cen_2017J1");
   m_forwardEff2017->SetDirectory(0);
   TString FileNameForwardEff2018 = PathResolverFindCalibFile("CxAODReader_VBFHbb/Eff_1for2cen_2018J1.root");
   TFile *forwardEff2018_file = new TFile(FileNameForwardEff2018.Data(), "READ");
   m_forwardEff2018 = (TH2D*)forwardEff2018_file->Get("Eff_1for2cen_2018J1");
   m_forwardEff2018->SetDirectory(0);

   TString FileNameCentralEff2016 = PathResolverFindCalibFile("CxAODReader_VBFHbb/Eff_3cen_2016J1.root");
   TFile *centralEff2016_file = new TFile(FileNameCentralEff2016.Data(), "READ");
   m_centralEff2016 = (TH2D*)centralEff2016_file->Get("Eff_3cen_2016J1");
   m_centralEff2016->SetDirectory(0);
   TString FileNameCentralEff2017 = PathResolverFindCalibFile("CxAODReader_VBFHbb/Eff_3cen_2017J1.root");
   TFile *centralEff2017_file = new TFile(FileNameCentralEff2017.Data(), "READ");
   m_centralEff2017 = (TH2D*)centralEff2017_file->Get("Eff_3cen_2017J1");
   m_centralEff2017->SetDirectory(0);
   TString FileNameCentralEff2018 = PathResolverFindCalibFile("CxAODReader_VBFHbb/Eff_3cen_2018J1.root");
   TFile *centralEff2018_file = new TFile(FileNameCentralEff2018.Data(), "READ");
   m_centralEff2018 = (TH2D*)centralEff2018_file->Get("Eff_3cen_2018J1");
   m_centralEff2018->SetDirectory(0);

   TString FileNameB2_1for2cenEff2016 = PathResolverFindCalibFile("CxAODReader_VBFHbb/Eff_1for2cen_2016B2.root");
   TFile *B2_1for2cenEff2016_file = new TFile(FileNameB2_1for2cenEff2016.Data(), "READ");
   m_B2_1for2cenEff2016 = (TH1D*)B2_1for2cenEff2016_file->Get("Eff_1for2cen_2016B2");
   m_B2_1for2cenEff2016->SetDirectory(0);
   TString FileNameB2_1for2cenEff2017 = PathResolverFindCalibFile("CxAODReader_VBFHbb/Eff_1for2cen_2017B2.root");
   TFile *B2_1for2cenEff2017_file = new TFile(FileNameB2_1for2cenEff2017.Data(), "READ");
   m_B2_1for2cenEff2017 = (TH1D*)B2_1for2cenEff2017_file->Get("Eff_1for2cen_2017B2");
   m_B2_1for2cenEff2017->SetDirectory(0);
   TString FileNameB2_1for2cenEff2018 = PathResolverFindCalibFile("CxAODReader_VBFHbb/Eff_1for2cen_2018B2.root");
   TFile *B2_1for2cenEff2018_file = new TFile(FileNameB2_1for2cenEff2018.Data(), "READ");
   m_B2_1for2cenEff2018 = (TH1D*)B2_1for2cenEff2018_file->Get("Eff_1for2cen_2018B2");
   m_B2_1for2cenEff2018->SetDirectory(0);

   TString FileNameB2_3cenEff2016 = PathResolverFindCalibFile("CxAODReader_VBFHbb/Eff_3cen_2016B2.root");
   TFile *B2_3cenEff2016_file = new TFile(FileNameB2_3cenEff2016.Data(), "READ");
   m_B2_3cenEff2016 = (TH1D*)B2_3cenEff2016_file->Get("Eff_3cen_2016B2");
   m_B2_3cenEff2016->SetDirectory(0);
   TString FileNameB2_3cenEff2017 = PathResolverFindCalibFile("CxAODReader_VBFHbb/Eff_3cen_2017B2.root");
   TFile *B2_3cenEff2017_file = new TFile(FileNameB2_3cenEff2017.Data(), "READ");
   m_B2_3cenEff2017 = (TH1D*)B2_3cenEff2017_file->Get("Eff_3cen_2017B2");
   m_B2_3cenEff2017->SetDirectory(0);
   TString FileNameB2_3cenEff2018 = PathResolverFindCalibFile("CxAODReader_VBFHbb/Eff_3cen_2018B2.root");
   TFile *B2_3cenEff2018_file = new TFile(FileNameB2_3cenEff2018.Data(), "READ");
   m_B2_3cenEff2018 = (TH1D*)B2_3cenEff2018_file->Get("Eff_3cen_2018B2");
   m_B2_3cenEff2018->SetDirectory(0);





   TString herwigMbbSF = PathResolverFindCalibFile("CxAODReader_VBFHbb/herwigMbbSF.root");
   TFile *herwigMbb_file = new TFile(herwigMbbSF.Data(), "READ");
   m_herwigMbb = (TH1D*)herwigMbb_file->Get("herwigMbb");
   m_herwigMbb->SetDirectory(0);

   TString Shower_mBBSF = PathResolverFindCalibFile(m_selection == ("1forward2central")?"CxAODReader_VBFHbb/Shower_mBB_1for2cen.root":"CxAODReader_VBFHbb/Shower_mBB_3cen.root");
   TFile *Shower_mBB_file = new TFile(Shower_mBBSF.Data(), "READ");
   m_Shower_mBB = (TH1D*)Shower_mBB_file->Get("histMad5");
   m_Shower_mBB->SetName("Shower_mBB");
   m_Shower_mBB->SetDirectory(0);

   TString Shower_nJets30ptSF = PathResolverFindCalibFile(m_selection == ("1forward2central")?"CxAODReader_VBFHbb/Shower_nJets30pt_1for2cen.root":"CxAODReader_VBFHbb/Shower_nJets30pt_3cen.root");
   TFile *Shower_nJets30pt_file = new TFile(Shower_nJets30ptSF.Data(), "READ");
   m_Shower_nJets30pt = (TH1D*)Shower_nJets30pt_file->Get("histMad5");
   m_Shower_nJets30pt->SetName("Shower_nJets30pt");
   m_Shower_nJets30pt->SetDirectory(0);


   TString s_rescale_4100w4f_l10SF = PathResolverFindCalibFile("CxAODReader_VBFHbb/rescale_4100w4f_l10.root");
   TFile *f_rescale_4100w4f_l10_file = new TFile(s_rescale_4100w4f_l10SF.Data(), "READ");
   h_rescale_4100w4fold0_l10 = (TH1F*)f_rescale_4100w4f_l10_file->Get("fold0");
   h_rescale_4100w4fold1_l10 = (TH1F*)f_rescale_4100w4f_l10_file->Get("fold1");
   h_rescale_4100w4fold2_l10 = (TH1F*)f_rescale_4100w4f_l10_file->Get("fold2");
   h_rescale_4100w4fold3_l10 = (TH1F*)f_rescale_4100w4f_l10_file->Get("fold3");
   h_rescale_4100w4fold0_l10->SetDirectory(0);
   h_rescale_4100w4fold1_l10->SetDirectory(0);
   h_rescale_4100w4fold2_l10->SetDirectory(0);
   h_rescale_4100w4fold3_l10->SetDirectory(0);
   h_rescale_4100w4fold0_l10->SetName("h_rescale_4100w4fold0_l10");
   h_rescale_4100w4fold1_l10->SetName("h_rescale_4100w4fold1_l10");
   h_rescale_4100w4fold2_l10->SetName("h_rescale_4100w4fold2_l10");
   h_rescale_4100w4fold3_l10->SetName("h_rescale_4100w4fold3_l10");

   TString s_rescale_4100w4f_l0SF = PathResolverFindCalibFile("CxAODReader_VBFHbb/rescale_4100w4f_l0.root");
   TFile *f_rescale_4100w4f_l0_file = new TFile(s_rescale_4100w4f_l0SF.Data(), "READ");
   h_rescale_4100w4fold0_l0 = (TH1F*)f_rescale_4100w4f_l0_file->Get("fold0");
   h_rescale_4100w4fold1_l0 = (TH1F*)f_rescale_4100w4f_l0_file->Get("fold1");
   h_rescale_4100w4fold2_l0 = (TH1F*)f_rescale_4100w4f_l0_file->Get("fold2");
   h_rescale_4100w4fold3_l0 = (TH1F*)f_rescale_4100w4f_l0_file->Get("fold3");
   h_rescale_4100w4fold0_l0->SetDirectory(0);
   h_rescale_4100w4fold1_l0->SetDirectory(0);
   h_rescale_4100w4fold2_l0->SetDirectory(0);
   h_rescale_4100w4fold3_l0->SetDirectory(0);
   h_rescale_4100w4fold0_l0->SetName("h_rescale_4100w4fold0_l0");
   h_rescale_4100w4fold1_l0->SetName("h_rescale_4100w4fold1_l0");
   h_rescale_4100w4fold2_l0->SetName("h_rescale_4100w4fold2_l0");
   h_rescale_4100w4fold3_l0->SetName("h_rescale_4100w4fold3_l0");

   TString s_rescale_4100w2f_l10SF = PathResolverFindCalibFile("CxAODReader_VBFHbb/rescale_4100w2f_l10.root");
   TFile *f_rescale_4100w2f_l10_file = new TFile(s_rescale_4100w2f_l10SF.Data(), "READ");
   h_rescale_4100w2fold0_l10 = (TH1F*)f_rescale_4100w2f_l10_file->Get("fold2f0");
   h_rescale_4100w2fold1_l10 = (TH1F*)f_rescale_4100w2f_l10_file->Get("fold2f1");
   h_rescale_4100w2fold0_l10->SetDirectory(0);
   h_rescale_4100w2fold1_l10->SetDirectory(0);
   h_rescale_4100w2fold0_l10->SetName("h_rescale_4100w2fold0_l10");
   h_rescale_4100w2fold1_l10->SetName("h_rescale_4100w2fold1_l10");

   TString s_rescale_3c_4100w4f_l10SF = PathResolverFindCalibFile("CxAODReader_VBFHbb/rescale_3c_4100w4f_l10.root");
   TFile *f_rescale_3c_4100w4f_l10_file = new TFile(s_rescale_3c_4100w4f_l10SF.Data(), "READ");
   h_rescale_3c_4100w4fold0_l10 = (TH1F*)f_rescale_3c_4100w4f_l10_file->Get("fold0");
   h_rescale_3c_4100w4fold1_l10 = (TH1F*)f_rescale_3c_4100w4f_l10_file->Get("fold1");
   h_rescale_3c_4100w4fold2_l10 = (TH1F*)f_rescale_3c_4100w4f_l10_file->Get("fold2");
   h_rescale_3c_4100w4fold3_l10 = (TH1F*)f_rescale_3c_4100w4f_l10_file->Get("fold3");
   h_rescale_3c_4100w4fold0_l10->SetDirectory(0);
   h_rescale_3c_4100w4fold1_l10->SetDirectory(0);
   h_rescale_3c_4100w4fold2_l10->SetDirectory(0);
   h_rescale_3c_4100w4fold3_l10->SetDirectory(0);
   h_rescale_3c_4100w4fold0_l10->SetName("h_rescale_3c_4100w4fold0_l10");
   h_rescale_3c_4100w4fold1_l10->SetName("h_rescale_3c_4100w4fold1_l10");
   h_rescale_3c_4100w4fold2_l10->SetName("h_rescale_3c_4100w4fold2_l10");
   h_rescale_3c_4100w4fold3_l10->SetName("h_rescale_3c_4100w4fold3_l10");

   TString s_rescale_3c_4100w4f_l0SF = PathResolverFindCalibFile("CxAODReader_VBFHbb/rescale_3c_4100w4f_l0.root");
   TFile *f_rescale_3c_4100w4f_l0_file = new TFile(s_rescale_3c_4100w4f_l0SF.Data(), "READ");
   h_rescale_3c_4100w4fold0_l0 = (TH1F*)f_rescale_3c_4100w4f_l0_file->Get("fold0");
   h_rescale_3c_4100w4fold1_l0 = (TH1F*)f_rescale_3c_4100w4f_l0_file->Get("fold1");
   h_rescale_3c_4100w4fold2_l0 = (TH1F*)f_rescale_3c_4100w4f_l0_file->Get("fold2");
   h_rescale_3c_4100w4fold3_l0 = (TH1F*)f_rescale_3c_4100w4f_l0_file->Get("fold3");
   h_rescale_3c_4100w4fold0_l0->SetDirectory(0);
   h_rescale_3c_4100w4fold1_l0->SetDirectory(0);
   h_rescale_3c_4100w4fold2_l0->SetDirectory(0);
   h_rescale_3c_4100w4fold3_l0->SetDirectory(0);
   h_rescale_3c_4100w4fold0_l0->SetName("h_rescale_3c_4100w4fold0_l0");
   h_rescale_3c_4100w4fold1_l0->SetName("h_rescale_3c_4100w4fold1_l0");
   h_rescale_3c_4100w4fold2_l0->SetName("h_rescale_3c_4100w4fold2_l0");
   h_rescale_3c_4100w4fold3_l0->SetName("h_rescale_3c_4100w4fold3_l0");

   TString s_rescale_3c_4100w2f_l10SF = PathResolverFindCalibFile("CxAODReader_VBFHbb/rescale_3c_4100w2f_l10.root");
   TFile *f_rescale_3c_4100w2f_l10_file = new TFile(s_rescale_3c_4100w2f_l10SF.Data(), "READ");
   h_rescale_3c_4100w2fold0_l10 = (TH1F*)f_rescale_3c_4100w2f_l10_file->Get("fold2f0");
   h_rescale_3c_4100w2fold1_l10 = (TH1F*)f_rescale_3c_4100w2f_l10_file->Get("fold2f1");
   h_rescale_3c_4100w2fold0_l10->SetDirectory(0);
   h_rescale_3c_4100w2fold1_l10->SetDirectory(0);
   h_rescale_3c_4100w2fold0_l10->SetName("h_rescale_3c_4100w2fold0_l10");
   h_rescale_3c_4100w2fold1_l10->SetName("h_rescale_3c_4100w2fold1_l10");

   TString FileNametmp2d = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmp2dmjj.root");
   //TString FileNametmp2d = PathResolverFindCalibFile("CxAODReader_VBFHbb/tmp2d.root");
   TFile *tmp2d_file = new TFile(FileNametmp2d.Data(), "READ");
   m_tmp2d = (TH2F*)tmp2d_file->Get("histMad5");
   m_tmp2d->SetDirectory(0);

  return EL::StatusCode::SUCCESS;
} // initializeTools

EL::StatusCode AnalysisReader_VBFHbb::setObjectsForOR (const xAOD::ElectronContainer *electrons,
                                                     const xAOD::PhotonContainer   *photons,
                                                     const xAOD::MuonContainer     *muons,
                                                     const xAOD::TauJetContainer   *taus,
                                                     const xAOD::JetContainer      *jets,
                                                     const xAOD::JetContainer      *fatJets) {
  if (electrons) {
    for (const xAOD::Electron *elec : *electrons) {
      Props::passPreSel.set(elec, Props::isVHLooseElectron.get(elec));
    }
  }

  if (muons) {
    for (const xAOD::Muon *muon : *muons) {
      Props::passPreSel.set(muon, Props::isVHLooseMuon.get(muon));
    }
  }

  if (jets) {
    for (const xAOD::Jet *jet : *jets) {
      Props::passPreSel.set(jet, Props::isVetoJet.get(jet));
    }
  }

  if (taus) {
    for (const xAOD::TauJet *tau : *taus) {
      Props::passPreSel.set(tau, Props::passTauSelector.get(tau));
    }
  }

  if (photons) {
    for (const xAOD::Photon *photon : *photons) {
      Props::passPreSel.set(photon, Props::isVBFLoosePhoton.get(photon));
    }
  }

  return EL::StatusCode::SUCCESS;
} // setObjectsForOR

EL::StatusCode AnalysisReader_VBFHbb::fill_bTaggerHists (const xAOD::Jet *jet)
{

  return EL::StatusCode::SUCCESS;
} // fill_bTaggerHists


EL::StatusCode AnalysisReader_VBFHbb::fill_VBFCutFlow (std::string label)
{
  std::string dir = "CutFlow/Nominal/";

  static std::string cuts[78] = {
    "All","initial_heta","initial_hpt0_150","initial_hpt150","initial_hpt0_200","initial_hpt200","initial_2jet","Pre-Selection", "Lepton/photon veto","Trigger","B-jet selection","Forward veto","Jet mult","VBF jet selection","pTBB","mBB","mJJ","Jet-Assignment","final_heta","final_hpt0_150","final_hpt150","final_hpt0_200","final_hpt200","final_2jet",
"stxs0",
"stxs200",
"stxs201",
"stxs202",
"stxs203",
"stxs204",
"stxs205",
"stxs206",
"stxs207",
"stxs208",
"stxs209",
"stxs210",
"stxs211",
"stxs212",
"stxs213",
"stxs214",
"stxs215",
"stxs216",
"stxs217",
"stxs218",
"stxs219",
"stxs220",
"stxs221",
"stxs222",
"stxs223",
"stxs224",
"stxs100",
"stxs101",
"stxs102",
"stxs103",
"stxs104",
"stxs105",
"stxs106",
"stxs107",
"stxs108",
"stxs109",
"stxs110",
"stxs111",
"stxs112",
"stxs113",
"stxs114",
"stxs115",
"stxs116",
"stxs117",
"stxs118",
"stxs119",
"stxs120",
"stxs121",
"stxs122",
"stxs123",
"stxs124",
"stxs125",
"stxs126",
"stxs127"
  };

  float ewkcorr = 1;
  if (m_mcChannel == 345949){
    float hpt = Props::hpt.get(m_eventInfo);
    if (hpt>50e3 && hpt<450e3) ewkcorr *=(0+1.0352055860847063*TMath::Power(hpt/1e3,0)+-0.0003323650501359826*TMath::Power(hpt/1e3,1)+-1.1680829307844583e-07*TMath::Power(hpt/1e3,2));
    if (hpt>450e3) ewkcorr *= 0.868501072;
  }


  m_histSvc->BookFillCutHist(dir + "Cuts", length(cuts), cuts, label, m_weight*ewkcorr);
  m_histSvc->BookFillCutHist(dir + "CutsNoWeight", length(cuts), cuts, label, 1.);

  return EL::StatusCode::SUCCESS;
} // fill_VBFCutFlow


void AnalysisReader_VBFHbb::compute_btagging ()
{
  if (m_jets) {
    if (m_debug) Info("compute_btagging()", "We got jets...");
    m_bTagTool->setJetAuthor(m_jetReader->getContainerName());

    for (auto jet : *m_jets) {
      BTagProps::isTagged.set(jet, static_cast<decltype(BTagProps::isTagged.get(jet))>(m_bTagTool->isTagged(*jet)));
      //BTagProps::tagWeight.set(jet, Props::MV2c20.get(jet));

      if (m_isMC) BTagProps::eff.set(jet, m_bTagTool->getEfficiency(*jet));
    }
  }

} // compute_btagging

EL::StatusCode AnalysisReader_VBFHbb::fill_nJetHistos (const std::vector<const xAOD::Jet*> &jets, const string &jetType) {
  int  nJet    = jets.size();
  bool isSig   = (jetType == "Sig");

  m_histSvc->BookFillHist("N" + jetType + "Jets", 11, -0.5, 10.5, nJet, m_weight);

  if (!m_doReduceFillHistos) {
    for (const xAOD::Jet *jet : jets) {
      m_histSvc->BookFillHist("Pt" + jetType + "Jets", 100,  0, 100, jet->pt() / 1e3, m_weight);
      m_histSvc->BookFillHist("Eta" + jetType + "Jets", 100, -5,   5, jet->eta(),  m_weight);
    }

//    // TODO: we may want to allow track-jets to be handled by fill_bTaggerHists - some changed would be needed there
//    if (isSig) {
//      for (const xAOD::Jet *jet : jets) {
//        EL_CHECK("AnalysisReader_VBFHbb::fill_nJetHistos", fill_bTaggerHists(jet));
//      }
//    }
  }

  return EL::StatusCode::SUCCESS;
} // fill_nJetHistos


void AnalysisReader_VBFHbb::setevent_nJets   (const std::vector<const xAOD::Jet*> &signalJets,
                                           const std::vector<const xAOD::Jet*> &forwardJets)
{
  auto nSignalJet  = signalJets.size();
  auto nForwardJet = forwardJets.size();
  auto nJet        = nSignalJet + nForwardJet;

  m_physicsMeta.nJets       = nJet;
  m_physicsMeta.nSigJet     = nSignalJet;
  m_physicsMeta.nForwardJet = nForwardJet;
} // setevent_nJets


EL::StatusCode AnalysisReader_VBFHbb::fill_VBF ()
{

  EL_CHECK("fill_VBF ()", fill_VBFCutFlow("All"));

  int njettruth = 0;

  if (m_truthWZJets) for (const xAOD::Jet *jet : *m_truthWZJets) {
    if (jet->pt()>20e3) njettruth++;
    //if (jet->auxdata<float>("TruthWZ_pt")>20e3) njettruth++;
  }

  if (Props::heta.exists(m_eventInfo)) {
     TLorentzVector tlvh;
     tlvh.SetPtEtaPhiM(Props::hpt.get(m_eventInfo),Props::heta.get(m_eventInfo),Props::hphi.get(m_eventInfo),Props::hm.get(m_eventInfo));
     if (fabs(tlvh.Rapidity())<2.5) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("initial_heta"));
     if (fabs(tlvh.Rapidity())<2.5 && Props::hpt.get(m_eventInfo)<150e3) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("initial_hpt0_150"));
     if (fabs(tlvh.Rapidity())<2.5 && Props::hpt.get(m_eventInfo)>150e3) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("initial_hpt150"));
     if (fabs(tlvh.Rapidity())<2.5 && Props::hpt.get(m_eventInfo)<200e3) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("initial_hpt0_200"));
     if (fabs(tlvh.Rapidity())<2.5 && Props::hpt.get(m_eventInfo)>200e3) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("initial_hpt200"));
     if (fabs(tlvh.Rapidity())<2.5 && njettruth>=4) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("initial_2jet"));

     int newstxs = getSTXS();
     TString stxslabel = "stxs";
     stxslabel += newstxs;
     fill_VBFCutFlow(stxslabel.Data());



    //"All","heta25","Pre-Selection", "Lepton/photon veto","Trigger","B-jet selection","Forward veto","Jet mult","VBF jet selection","pTBB","mBB","mJJ","Jet-Assignment","final_heta","final_hpt0_150","final_hpt150","final_hpt0_200","final_hpt200"
     //if (Props::hpt.get(m_eventInfo)>200e3 && fabs(tlvh.Rapidity())<2.5) {EL_CHECK("fill_VBF ()", fill_VBFCutFlow("hpt"));}
     //if (Props::hpt.get(m_eventInfo)>150e3 && fabs(tlvh.Rapidity())<2.5) {EL_CHECK("fill_VBF ()", fill_VBFCutFlow("hpt"));}
     //else return EL::StatusCode::SUCCESS;
  }

  if (m_debug) Info("fill_VBF()", "Beginning of the function...");
  if (m_isMC) m_bTagTool->setMCIndex(m_mcChannel, *m_xSectionProvider);
  if (m_isMC) m_bTagTool_Medium->setMCIndex(m_mcChannel, *m_xSectionProvider);
  if (m_isMC) m_bTagTool_Loose->setMCIndex(m_mcChannel, *m_xSectionProvider);
  m_tree_vbf->SetVariation(m_currentVar);
  m_tree_vbf->Reset();
  ResultVBFHbbIncl selectionResult = ((VBFHbbInclEvtSelection*)m_eventSelection)->result();

  UInt_t runNumber = (m_isMC?m_eventInfo->auxdata<UInt_t>("RandomRunNumber"):m_eventInfo->runNumber());
  if (m_isMC && runNumber<=311481) m_weight *= m_luminosity_a;
  else if (m_isMC && runNumber<=340453) m_weight *= m_luminosity_d;
  else if (m_isMC) m_weight *= m_luminosity_e;

  if( !selectionResult.pass )
    return EL::StatusCode::SUCCESS;


  m_tree_vbf->nElectrons = 0;
  m_tree_vbf->nMuons = 0;
  m_tree_vbf->nPhotons = 0;
  m_tree_vbf->met = 0;

  if (m_electrons) {
    for (const xAOD::Electron *elec : *m_electrons) {
      if (Props::isVHLooseElectron.get(elec)) m_tree_vbf->nElectrons++;
    }
  }

  if (m_muons) {
    for (const xAOD::Muon *muon : *m_muons) {
      if (Props::isVHLooseMuon.get(muon)) m_tree_vbf->nMuons++;
    }
  }

  if (m_photons) {
    for (const xAOD::Photon *photon : *m_photons) {
      if (Props::isVBFLoosePhoton.get(photon)) m_tree_vbf->nPhotons++;
    }
  }

  m_tree_vbf->batman = (runNumber<=311481 && Props::DFCommonJets_isBadBatman.get(m_eventInfo));
  if (m_selection.find("all") == std::string::npos && m_tree_vbf->batman) return EL::StatusCode::SUCCESS;

  if (m_debug) Info("fill_VBF()", "pass pre-selection");

  EL_CHECK("fill_VBF ()", fill_VBFCutFlow("Pre-Selection"));

  if (m_selection.find("all") == std::string::npos && (m_tree_vbf->nPhotons || m_tree_vbf->nMuons || m_tree_vbf->nElectrons)) return EL::StatusCode::SUCCESS;

  EL_CHECK("fill_VBF ()", fill_VBFCutFlow("Lepton/photon veto"));

  if (m_met) {
    m_tree_vbf->met = m_met->met();
  }
  if (m_met_soft) {
    m_tree_vbf->metsoft = m_met_soft->met();
    m_tree_vbf->sumsoft = m_met_soft->sumet();
  }
  //if (m_metCont && (*m_metCont)["RefJet"]) {
  //  m_tree_vbf->metjet = (*m_metCont)["RefJet"]->met();
  //}


  std::vector<const xAOD::Jet*> jets = selectionResult.jets;

  // In case we apply no selection, we are done
//  if (m_selection == "none") {
//    EL_CHECK("AnalysisReader_VBFHbb::save_meta_info" , save_meta_info(jets, jets) );
//    for (unsigned int iJet(0); iJet < jets.size(); iJet++) {
//      m_tree_vbf->pT.push_back( jets.at(iJet)->pt() );
//      m_tree_vbf->phi.push_back( jets.at(iJet)->phi() );
//      m_tree_vbf->eta.push_back( jets.at(iJet)->eta() );
//    }
//    if (m_histNameSvc->get_isNominal()) {
//      m_tree_vbf->Fill();
//      m_eventCountPassed_VBFHbb++;
//    }
//    return EL::StatusCode::SUCCESS;
//  }

  std::vector<const xAOD::Jet*> signalJets, vbfJets;
  std::vector<const xAOD::Jet*> forwardJets = selectionResult.forwardJets;
  std::vector<const xAOD::Jet*> trackJets = selectionResult.trackJets;

  // Jet Assignment Tool
  // -------------

  m_jetAssigner->setRunNumber(runNumber);
  if (m_debug) Info("fill_VBF()", "entering jet assignment");
  int returnval = m_jetAssigner->assignJets(jets, m_eventInfo);
  if (returnval>=1) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("Trigger"));
  if (returnval>=2) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("B-jet selection"));
  if (returnval>=3) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("Forward veto"));
  if (returnval>=4) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("Jet mult"));
  if (returnval>=5) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("VBF jet selection"));
  if (returnval<10) return EL::StatusCode::SUCCESS;
  if (m_debug) Info("fill_VBF()", "passed jet assignment");

  EL_CHECK("fill_VBF ()", fill_VBFCutFlow("Jet-Assignment"));





  std::string assType = m_jetAssigner->assignmentType();
  signalJets = m_jetAssigner->signalJets();
  vbfJets = m_jetAssigner->vbfJets();


  if(!m_doTruthTagging){
    compute_btagging();
  }

  // ===============
  //
  //
  signalJets.at(0)->p4();
  signalJets.at(1)->p4();
  vbfJets.at(0)->p4();
  vbfJets.at(1)->p4();
  TLorentzVector b1Vec(signalJets.at(0)->p4()), b2Vec(signalJets.at(1)->p4()),j1Vec(vbfJets.at(0)->p4()), j2Vec(vbfJets.at(1)->p4());
  //j1Vec.SetPtEtaPhiM(vbfJets.at(0)->pt(),vbfJets.at(0)->eta(),vbfJets.at(0)->phi(),vbfJets.at(0)->m());
  //j2Vec.SetPtEtaPhiM(vbfJets.at(1)->pt(),vbfJets.at(1)->eta(),vbfJets.at(1)->phi(),vbfJets.at(1)->m());
  j1Vec.SetPtEtaPhiM(vbfJets.at(0)->pt(),vbfJets.at(0)->eta()*vbfJets.at(0)->auxdata<float>("randfactor"),vbfJets.at(0)->phi(),vbfJets.at(0)->m());
  j2Vec.SetPtEtaPhiM(vbfJets.at(1)->pt(),vbfJets.at(1)->eta()*vbfJets.at(1)->auxdata<float>("randfactor"),vbfJets.at(1)->phi(),vbfJets.at(1)->m());


  TLorentzVector HVec(b1Vec+b2Vec);


  // ====================
  // apply weights
  // ====================
  float btagSF = 1.;
  if(m_isMC){
    if (m_JetAssignmentStrategy.find("85") != std::string::npos){
      btagSF = computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "85");
    }
    if (m_JetAssignmentStrategy.find("77") != std::string::npos){
      btagSF = computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "77");
    }
    if (m_JetAssignmentStrategy.find("70") != std::string::npos){
      btagSF = computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "70");
    }
    if (m_JetAssignmentStrategy.find("asym") != std::string::npos){
      btagSF = computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "70");
      btagSF *= computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "85");
    }


    if (m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos && m_selection == "3central") {
      btagSF = computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "77");
    }
    if (m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos && m_selection == "1forward2central") {
      //std::cout << computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "77") << " " << computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "85") << '\n';
      //std::cout << computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "77") << " " << computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "85") << " " << (computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "87")/(computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "85")*computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "77")+1e-6)) << '\n';
      btagSF = computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "continuous");
      //btagSF = computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "77");
      //btagSF *= computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "85");
    }
    if (m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos && m_selection == "0forward") {
      btagSF = computeBTagSFWeightPerJet (signalJets, m_jetReader->getContainerName(), "77");
    }

  }

  // b-tagging weights
  m_weight *= btagSF;
  m_tree_vbf->BJetSF = btagSF;

  // apply jvt weights
  m_weight *= (Props::JvtSF.exists(m_eventInfo)?Props::JvtSF.get(m_eventInfo):1);

  // apply qg tagging weights
  //m_weight *= !Props::QGTaggerWeight.exists(vbfJets.at(0))?1:Props::QGTaggerWeight.get(vbfJets.at(0));
  //m_weight *= !Props::QGTaggerWeight.exists(vbfJets.at(1))?1:Props::QGTaggerWeight.get(vbfJets.at(1));

  //temporary hack for missing ttH. Scale up mc16d lumi.
  if (m_isMC && m_mcChannel>=345873 && m_mcChannel<=345874) m_weight *= ((m_luminosity_a+m_luminosity_d+m_luminosity_e)/m_luminosity_d);

  m_histNameSvc->set_analysisType(HistNameSvc::AnalysisType::VBF); // use VBF naming convention
  m_histNameSvc->set_nTag(2); // set histogram name
  m_histNameSvc->set_nJet(4); // more histogram naming

  // ====================
  // fill event variables
  // ====================

  EL_CHECK("AnalysisReader_VBFHbb::save_trigger_info" , save_trigger_info() );

  // ====================
  // fill jet variables
  // ====================

  EL_CHECK("AnalysisReader_VBFHbb::fill_VBF", fill_nJetHistos(signalJets,  "Sig"));
  EL_CHECK("AnalysisReader_VBFHbb::fill_VBF", fill_nJetHistos(forwardJets, "Fwd"));
  EL_CHECK("AnalysisReader_VBFHbb::fill_VBF", fill_jetHistos(jets, signalJets,forwardJets));
  // We save all the information we need, for the events and the jets
  EL_CHECK("AnalysisReader_VBFHbb::save_jet_info" , save_jet_info(jets,signalJets,vbfJets,trackJets) );
  EL_CHECK("AnalysisReader_VBFHbb::save_meta_info" , save_meta_info(jets, signalJets) );

  bool passCutspTBB = true;
  bool passCutsmBB = true;
  bool passCutsmJJ = true;
  if (m_tree_vbf->pTBB<150e3) passCutspTBB = false;
  if (m_tree_vbf->mBB>200e3) passCutsmBB = false;
  if (m_tree_vbf->mJJ<800e3) passCutsmJJ = false;
  //if (m_tree_vbf->mBB<300e3) return EL::StatusCode::SUCCESS;

  if(!passCutspTBB && m_selection.find("trigger") == std::string::npos && m_selection.find("inclusive") == std::string::npos && m_selection.find("all") == std::string::npos) {
    //std::printf("Failed kinematic cuts! Skipping this event...\n");
    return EL::StatusCode::SUCCESS;
  }

  EL_CHECK("fill_VBF ()", fill_VBFCutFlow("pTBB"));

  if(!passCutsmBB && m_selection.find("trigger") == std::string::npos && m_selection.find("inclusive") == std::string::npos && m_selection.find("all") == std::string::npos) {
    //std::printf("Failed kinematic cuts! Skipping this event...\n");
    return EL::StatusCode::SUCCESS;
  }

  EL_CHECK("fill_VBF ()", fill_VBFCutFlow("mBB"));

  if(!passCutsmJJ && m_selection.find("trigger") == std::string::npos && m_selection.find("inclusive") == std::string::npos && m_selection.find("all") == std::string::npos && m_selection.find("3cen") != std::string::npos) {
    //std::printf("Failed kinematic cuts! Skipping this event...\n");
    return EL::StatusCode::SUCCESS;
  }

  EL_CHECK("fill_VBF ()", fill_VBFCutFlow("mJJ"));

  if (Props::heta.exists(m_eventInfo)) {
     TLorentzVector tlvh;
     tlvh.SetPtEtaPhiM(Props::hpt.get(m_eventInfo),Props::heta.get(m_eventInfo),Props::hphi.get(m_eventInfo),Props::hm.get(m_eventInfo));
     if (fabs(tlvh.Rapidity())<2.5) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("final_heta"));
     if (fabs(tlvh.Rapidity())<2.5 && Props::hpt.get(m_eventInfo)<150e3) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("final_hpt0_150"));
     if (fabs(tlvh.Rapidity())<2.5 && Props::hpt.get(m_eventInfo)>150e3) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("final_hpt150"));
     if (fabs(tlvh.Rapidity())<2.5 && Props::hpt.get(m_eventInfo)<200e3) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("final_hpt0_200"));
     if (fabs(tlvh.Rapidity())<2.5 && Props::hpt.get(m_eventInfo)>200e3) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("final_hpt200"));
     if (fabs(tlvh.Rapidity())<2.5 && njettruth>=4) EL_CHECK("fill_VBF ()", fill_VBFCutFlow("final_2jet"));
     //else return EL::StatusCode::SUCCESS;
  }

  // ====================
  // fill jet histograms
  // ====================

  if (jets.size() > 0) m_histSvc -> BookFillHist("pTj1", 30, 0, 700, jets.at(0)->pt()*0.001, m_weight);
  if (jets.size() > 1) m_histSvc -> BookFillHist("pTj2", 30, 0, 700, jets.at(1)->pt()*0.001, m_weight);
  if (jets.size() > 2) m_histSvc -> BookFillHist("pTj3", 30, 0, 700, jets.at(2)->pt()*0.001, m_weight);
  if (jets.size() > 3) m_histSvc -> BookFillHist("pTj4", 30, 0, 700, jets.at(3)->pt()*0.001, m_weight);

  m_histSvc -> BookFillHist("pTB1", 30, 0, 600, b1Vec.Pt()*0.001, m_weight);
  m_histSvc -> BookFillHist("pTB2", 30, 0, 500, b2Vec.Pt()*0.001, m_weight);
  m_histSvc -> BookFillHist("pTJ1", 30, 0, 700, j1Vec.Pt()*0.001, m_weight);
  m_histSvc -> BookFillHist("pTJ2", 30, 0, 500, j2Vec.Pt()*0.001, m_weight);

  m_histSvc -> BookFillHist("etaB1", 30, -3., 3., b1Vec.Eta(), m_weight);
  m_histSvc -> BookFillHist("etaB2", 30, -3., 3., b2Vec.Eta(), m_weight);
  m_histSvc -> BookFillHist("etaJ1", 30, -5., 5., j1Vec.Eta(), m_weight);
  m_histSvc -> BookFillHist("etaJ2", 30, -5., 5., j2Vec.Eta(), m_weight);

  m_histSvc -> BookFillHist("mBB", 100, 0, 1000, HVec.M()*0.001, m_weight);
  m_histSvc -> BookFillHist("mJJ", 30, 0, 3500, (j1Vec+j2Vec).M()*0.001, m_weight);
  m_histSvc -> BookFillHist("dEtaJJ", 30, 0, 9, fabs(j1Vec.Eta()-j2Vec.Eta()), m_weight);

  //fk added from fill_VBF in AnalysisReader_VBFHbb.cxx
  static const float GeV = 1000.;
  const xAOD::Jet* j1 = vbfJets.at(0);
  const xAOD::Jet* b1 = signalJets.at(0);
  const xAOD::Jet* b2 = signalJets.at(1);
  const xAOD::Jet* j2 = vbfJets.at(1);
  const TLorentzVector bbSystem = b1->p4() + b2->p4();
  const TLorentzVector jjSystem = j1->p4() + j2->p4();
  m_histSvc -> BookFillHist("phiB1", 30, -1.*TMath::Pi(), TMath::Pi(), b1->phi(), m_weight);
  m_histSvc -> BookFillHist("phiB2", 30, -1.*TMath::Pi(), TMath::Pi(), b2->phi(), m_weight);
  m_histSvc -> BookFillHist("phiJ1", 30, -1.*TMath::Pi(), TMath::Pi(), j1->phi(), m_weight);
  m_histSvc -> BookFillHist("phiJ2", 30, -1.*TMath::Pi(), TMath::Pi(), j2->phi(), m_weight);

  m_histSvc -> BookFillHist("pTBB", 30, 0, 500, bbSystem.Pt()/GeV, m_weight);
  m_histSvc -> BookFillHist("dPhiJJ", 30, 0, TMath::Pi(), fabs( j1->p4().DeltaPhi(j2->p4()) ), m_weight);
  m_histSvc -> BookFillHist("dEtaBB", 30, 0, 9, fabs(b1->eta()-b2->eta()), m_weight);
  m_histSvc -> BookFillHist("dPhiBB", 30, 0, TMath::Pi(), fabs( b1->p4().DeltaPhi(b2->p4()) ), m_weight);


  if (m_tree_vbf->pTBB>150e3 && m_tree_vbf->bdt2cenOldExt>0.62) m_histSvc -> BookFillHist("mBB_SR1", 400, 0, 200, m_tree_vbf->mBB*0.001, m_weight);
  else if (m_tree_vbf->pTBB>150e3) m_histSvc -> BookFillHist("mBB_SR2", 400, 0, 200, m_tree_vbf->mBB*0.001,m_weight);
  if (m_tree_vbf->pTBB>150e3) m_histSvc -> BookFillHist("mBB_bdt2cenOldExt", 200, 0, 200, 200,-1,1,float(m_tree_vbf->mBB*0.001), m_tree_vbf->bdt2cenOldExt, m_weight);
  if (m_tree_vbf->pTBB>120e3) m_histSvc -> BookFillHist("mBB_bdt2cen", 200, 0, 200, 200,-1,1,      float(m_tree_vbf->mBB*0.001), m_tree_vbf->bdt2cen, m_weight);

  // fill weight systematics
  for(unsigned int i=0; i<m_weightSysts.size(); i++){
    //m_tree_vbf->weightSysts.push_back(m_weightSysts.at(i).factor);
    //std::cout << "NAMER " << m_weightSysts.at(i).name << " " << m_weightSysts.at(i).factor << '\n';
    if (m_debug) Info("fillVBF()", "sysname %s",m_weightSysts.at(i).name.c_str());
  }

  // ====================
  // fill output ntup
  // ====================

  m_tree_vbf->Fill();// filling for all variations
  if (m_histNameSvc->get_isNominal()) {
    m_eventCountPassed_VBFHbb++;
  }

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode AnalysisReader_VBFHbb::save_meta_info( std::vector<const xAOD::Jet*>  const& Jets,
						      std::vector<const xAOD::Jet*>  const& signalJets){

  // ------------------------
  // compute branch variables
  // ------------------------
  // Event Metadata
  // ------------------------
  /// apply the trigger weight scale factor 

  TLorentzVector b1Vec(signalJets.at(0)->p4()), b2Vec(signalJets.at(1)->p4());
  TLorentzVector LeadJetVec(Jets.at(0)->p4());
  std::vector<float> weights_leadBEta = {1,0};
  std::vector<float> weights_jet0 = {1,0};
  std::vector<float> weights_jet1 = {1,0};

  UInt_t runNumber = (m_isMC?m_eventInfo->auxdata<UInt_t>("RandomRunNumber"):m_eventInfo->runNumber());
  if (m_isMC && !Props::embZm.exists(m_eventInfo)){
    if (b1Vec == LeadJetVec or b2Vec == LeadJetVec){
      weights_leadBEta = ReadOfflineBJetTriggerWeight(m_Eff_Event_leadingJet_jetEta, LeadJetVec.Eta(), false);
    }
    if (m_selection == "0forward") {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt60_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt60_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt60_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt60_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt60_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt60_jetPt, b2Vec.Pt());
      }
    }
    if ((m_selection == "3central" || m_selection == "fake3" || m_selection == "fake4") && runNumber<=311481) {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt60_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt60_jetPt16, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt60_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt60_jetPt16, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt60_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt60_jetPt16, b2Vec.Pt());
      }
    }
    if ((m_selection == "3central" || m_selection == "fake3" || m_selection == "fake4") && runNumber>311481) {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt70_jetPt, b2Vec.Pt());
      }
    }
    if (m_selection == "1forward2central" && runNumber<=311481) {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt85_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt70_jetPt16, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt85_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt70_jetPt16, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt85_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt70_jetPt16, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("asym") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt70_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off85_match_hlt85_jetPt16, b2Vec.Pt());
      }
    }
    if (m_selection == "1forward2central" && runNumber>311481) {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt85_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt85_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt85_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("asym") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off85_match_hlt85_jetPt, b2Vec.Pt());
      }
    }
    if (m_selection == "fake") {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt85_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt85_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt60_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt60_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("asym") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        //weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt60_jetPt, b1Vec.Pt());
        //weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt60_jetPt, b2Vec.Pt());
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt60_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off85_match_hlt60_jetPt, b2Vec.Pt());
      }
    }
    if (m_selection == "1forward3central") {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt70_jetPt, b2Vec.Pt());
      }
    }
    if (m_selection == "2forward") {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off60_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = {1,0};
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos || m_JetAssignmentStrategy.find("asym") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = {1,0};
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 ={1,0};
      }
    }
/*
    if (m_selection == "4central") {
      if (runNumber<=303943){
	if (m_JetAssignmentStrategy.find("70") != std::string::npos){
 	  weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt70_jetPt, b1Vec.Pt());
	  weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt70_jetPt, b2Vec.Pt());
	}
	if (m_JetAssignmentStrategy.find("77") != std::string::npos){
	  weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt70_jetPt, b1Vec.Pt());
	  weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt70_jetPt, b2Vec.Pt());
	}
	if (m_JetAssignmentStrategy.find("85") != std::string::npos){
	  weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off85_match_hlt70_jetPt, b1Vec.Pt());
	  weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off85_match_hlt70_jetPt, b2Vec.Pt());
	}
      }
      else {
	if (m_JetAssignmentStrategy.find("70") != std::string::npos){
 	  weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt60_jetPt, b1Vec.Pt());
	  weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt60_jetPt, b2Vec.Pt());
	}
	if (m_JetAssignmentStrategy.find("77") != std::string::npos){
	  weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt60_jetPt, b1Vec.Pt());
	  weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt60_jetPt, b2Vec.Pt());
	}
	if (m_JetAssignmentStrategy.find("85") != std::string::npos){
	  weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off85_match_hlt60_jetPt, b1Vec.Pt());
	  weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off85_match_hlt60_jetPt, b2Vec.Pt());
	}
      }
    }
    if (m_selection == "2central1forward") {
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
	weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt70_jetPt, b1Vec.Pt());
	weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt85_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos){
	weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt70_jetPt, b1Vec.Pt());
	weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off77_match_hlt85_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("85") != std::string::npos){
	weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off85_match_hlt70_jetPt, b1Vec.Pt());
	weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off85_match_hlt85_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("asym") != std::string::npos){
	weights_jet0 = ReadOfflineBJetTriggerWeight(m_SF_Off70_match_hlt70_jetPt, b1Vec.Pt());
	weights_jet1 = ReadOfflineBJetTriggerWeight(m_SF_Off85_match_hlt85_jetPt, b2Vec.Pt());
      }
    }
*/

    if (weights_leadBEta[0]>10) std::cout << "TABITHA 1 " << weights_leadBEta[0] << '\n';
    if (weights_jet0[0]>10) std::cout << "TABITHA 2 " << weights_jet0[0] << '\n';
    if (weights_jet1[0]>10) std::cout << "TABITHA 3 " << weights_jet1[0] << '\n';
    m_weightSysts.push_back({"BTrig_Eta_EFF__1up" ,  weights_leadBEta[0]==0?0:(weights_leadBEta[0]+weights_leadBEta[1])/weights_leadBEta[0]   });
    m_weightSysts.push_back({"BTrig_Eta_EFF__1down" ,  weights_leadBEta[0]==0?0:(weights_leadBEta[0]-weights_leadBEta[1])/weights_leadBEta[0]  });
    m_weightSysts.push_back({"BTrig_SF__1up" ,   (weights_jet0[0]==0?0:(weights_jet0[0]+weights_jet0[1])/weights_jet0[0])*(weights_jet1[0]==0?0:(weights_jet1[0]+weights_jet1[1])/weights_jet1[0]) });
    m_weightSysts.push_back({"BTrig_SF__1down" , (weights_jet0[0]==0?0:(weights_jet0[0]-weights_jet0[1])/weights_jet0[0])*(weights_jet1[0]==0?0:(weights_jet1[0]-weights_jet1[1])/weights_jet1[0]) });
  }
  if (Props::embZm.exists(m_eventInfo)){
    if (m_selection == "0forward") {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt60_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt60_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt60_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt60_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt60_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt60_jetPt, b2Vec.Pt());
      }
    }
    if ((m_selection == "3central" || m_selection == "fake3" || m_selection == "fake4") && runNumber<=311481) {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt60_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt60_jetPt16, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt60_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt60_jetPt16, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt60_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt60_jetPt16, b2Vec.Pt());
      }
    }
    if ((m_selection == "3central" || m_selection == "fake3" || m_selection == "fake4") && runNumber>311481) {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt70_jetPt, b2Vec.Pt());
      }
    }
    if (m_selection == "fake" && runNumber>311481) {
      if (m_JetAssignmentStrategy.find("asym") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt60_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off85_match_hlt60_jetPt, b2Vec.Pt());
      }
    }
    if (m_selection == "fake" && runNumber<=303943) {
      if (m_JetAssignmentStrategy.find("asym") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt70_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off85_match_hlt70_jetPt16, b2Vec.Pt());
      }
    }
    if (m_selection == "fake" && runNumber<=311481 && runNumber>303943) {
      if (m_JetAssignmentStrategy.find("asym") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt60_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off85_match_hlt60_jetPt16, b2Vec.Pt());
      }
    }
    if (m_selection == "fake" && runNumber>311481) {
      if (m_JetAssignmentStrategy.find("77")){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt60_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off85_match_hlt60_jetPt, b2Vec.Pt());
      }
    }
    if (m_selection == "fake" && runNumber<=303943) {
      if (m_JetAssignmentStrategy.find("77")){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt70_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off85_match_hlt70_jetPt16, b2Vec.Pt());
      }
    }
    if (m_selection == "fake" && runNumber<=311481 && runNumber>303943) {
      if (m_JetAssignmentStrategy.find("77")){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt60_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off85_match_hlt60_jetPt16, b2Vec.Pt());
      }
    }
    if (m_selection == "1forward2central" && runNumber<=311481) {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt85_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt70_jetPt16, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt85_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt70_jetPt16, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt85_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt70_jetPt16, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("asym") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt70_jetPt16, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off85_match_hlt85_jetPt16, b2Vec.Pt());
      }
    }
    if (m_selection == "1forward2central" && runNumber>311481) {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt85_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt85_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt85_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("asym") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off85_match_hlt85_jetPt, b2Vec.Pt());
      }
    }
    if (m_selection == "1forward3central") {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt70_jetPt, b2Vec.Pt());
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt70_jetPt, b2Vec.Pt());
      }
    }
    if (m_selection == "2forward") {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = {1,0};
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos || m_JetAssignmentStrategy.find("asym") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = {1,0};
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt70_jetPt, b1Vec.Pt());
        weights_jet1 = {1,0};
      }
    }
    if (m_selection == "single") {
      if (m_JetAssignmentStrategy.find("60") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off60_match_hlt40_jetPt, b1Vec.Pt());
        weights_jet1 = {1,0};
      }
      if (m_JetAssignmentStrategy.find("70") != std::string::npos || m_JetAssignmentStrategy.find("DEFAULT") != std::string::npos || m_JetAssignmentStrategy.find("asym") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off70_match_hlt40_jetPt, b1Vec.Pt());
        weights_jet1 = {1,0};
      }
      if (m_JetAssignmentStrategy.find("77") != std::string::npos){
        weights_jet0 = ReadOfflineBJetTriggerWeight(m_Eff_Off77_match_hlt40_jetPt, b1Vec.Pt());
        weights_jet1 = {1,0};
      }
    }
    
    m_weightSysts.push_back({"BTrig_SF__1up" ,   (weights_jet0[0]==0?0:(weights_jet0[0]+weights_jet0[1])/weights_jet0[0])*(weights_jet1[0]==0?0:(weights_jet1[0]+weights_jet1[1])/weights_jet1[0]) });
    m_weightSysts.push_back({"BTrig_SF__1down" , (weights_jet0[0]==0?0:(weights_jet0[0]-weights_jet0[1])/weights_jet0[0])*(weights_jet1[0]==0?0:(weights_jet1[0]-weights_jet1[1])/weights_jet1[0]) });
  }

  m_tree_vbf->RunNumber = (m_isMC?m_eventInfo->auxdata<UInt_t>("RandomRunNumber"):m_eventInfo->runNumber());
  m_tree_vbf->LumiBlock = m_eventInfo->lumiBlock();
  m_tree_vbf->EventNumber = m_eventInfo->eventNumber();
  m_tree_vbf->BJetTriggerWeight = weights_jet0[0] * weights_jet1[0];
  if (m_tree_vbf->RunNumber<=311481) m_tree_vbf->BJetTriggerWeight = weights_jet0[0] * weights_jet1[0] *weights_leadBEta[0];
  if (Props::embZm.exists(m_eventInfo) || m_isMC) m_weight *= m_tree_vbf->forwardSF;
  if (Props::embZm.exists(m_eventInfo) || m_isMC) m_weight *= m_tree_vbf->centralSF;
  if (Props::embZm.exists(m_eventInfo)) m_weight *= m_tree_vbf->B2SF;
  if (m_isMC && m_tree_vbf->QGTaggerWeightJ1>0 && m_tree_vbf->pTJ1>50e3 && fabs(m_tree_vbf->etaJ1)<2.1) m_weight *= m_tree_vbf->QGTaggerWeightJ1;
  if (m_isMC && m_tree_vbf->QGTaggerWeightJ2>0 && m_tree_vbf->pTJ2>50e3 && fabs(m_tree_vbf->etaJ2)<2.1) m_weight *= m_tree_vbf->QGTaggerWeightJ2;

  if (Props::muonEff.exists(m_eventInfo) && Props::muonEff.get(m_eventInfo)>0) m_weight *= (1./Props::muonEff.get(m_eventInfo));
  if (Props::muonTrigEff_cvmfs.exists(m_eventInfo) && Props::muonTrigEff_cvmfs.get(m_eventInfo)>0) m_weight *= (1./Props::muonTrigEff_cvmfs.get(m_eventInfo));
  if (Props::muonEff.exists(m_eventInfo)) m_weight *= (15.6/3.366);
  //if (Props::muonEff.exists(m_eventInfo) && Props::muonEff.get(m_eventInfo)>0) m_tree_vbf->EventWeight *= (1./Props::muonEff.get(m_eventInfo));
  //if (Props::muonTrigEff_cvmfs.exists(m_eventInfo) && Props::muonTrigEff_cvmfs.get(m_eventInfo)>0) m_tree_vbf->EventWeight *= (1./Props::muonTrigEff_cvmfs.get(m_eventInfo));
  //if (Props::muonEff.exists(m_eventInfo)) m_tree_vbf->EventWeight *= (15.6/3.366);
  if (m_isMC) {
    m_tree_vbf->hpt = (Props::hpt.exists(m_eventInfo)?Props::hpt.get(m_eventInfo):-1);
    m_tree_vbf->heta = (Props::heta.exists(m_eventInfo)?Props::heta.get(m_eventInfo):-1);
    m_tree_vbf->hphi = (Props::hphi.exists(m_eventInfo)?Props::hphi.get(m_eventInfo):-1);
    m_tree_vbf->hm = (Props::hm.exists(m_eventInfo)?Props::hm.get(m_eventInfo):-1);

    m_tree_vbf->HTXS_Higgs_pt = (Props::HTXS_Higgs_pt.exists(m_eventInfo)?Props::HTXS_Higgs_pt.get(m_eventInfo):-1);
    m_tree_vbf->HTXS_Higgs_eta = (Props::HTXS_Higgs_eta.exists(m_eventInfo)?Props::HTXS_Higgs_eta.get(m_eventInfo):-1);
    m_tree_vbf->HTXS_Higgs_phi = (Props::HTXS_Higgs_phi.exists(m_eventInfo)?Props::HTXS_Higgs_phi.get(m_eventInfo):-1);
    m_tree_vbf->HTXS_Higgs_m = (Props::HTXS_Higgs_m.exists(m_eventInfo)?Props::HTXS_Higgs_m.get(m_eventInfo):-1);

    m_tree_vbf->HTXS_Njets_pTjet25 = m_eventInfo->auxdata<int>("HTXS_Njets_pTjet25");
    m_tree_vbf->HTXS_Njets_pTjet30 = m_eventInfo->auxdata<int>("HTXS_Njets_pTjet30");
    m_tree_vbf->HTXS_Stage1_1_Category_pTjet25 = m_eventInfo->auxdata<int>("HTXS_Stage1_1_Category_pTjet25");
    m_tree_vbf->HTXS_Stage1_1_Category_pTjet30 = m_eventInfo->auxdata<int>("HTXS_Stage1_1_Category_pTjet30");
    m_tree_vbf->HTXS_Stage1_1_FineIndex_pTjet25 = m_eventInfo->auxdata<int>("HTXS_Stage1_1_FineIndex_pTjet25");
    m_tree_vbf->HTXS_Stage1_1_FineIndex_pTjet30 = m_eventInfo->auxdata<int>("HTXS_Stage1_1_FineIndex_pTjet30");
    m_tree_vbf->HTXS_Stage1_1_Fine_Category_pTjet25 = m_eventInfo->auxdata<int>("HTXS_Stage1_1_Fine_Category_pTjet25");
    m_tree_vbf->HTXS_Stage1_1_Fine_Category_pTjet30 = m_eventInfo->auxdata<int>("HTXS_Stage1_1_Fine_Category_pTjet30");
    m_tree_vbf->HTXS_Stage1_1_Fine_FineIndex_pTjet25 = m_eventInfo->auxdata<int>("HTXS_Stage1_1_Fine_FineIndex_pTjet25");
    m_tree_vbf->HTXS_Stage1_1_Fine_FineIndex_pTjet30 = m_eventInfo->auxdata<int>("HTXS_Stage1_1_Fine_FineIndex_pTjet30");
    m_tree_vbf->HTXS_Stage1_Category_pTjet25 = m_eventInfo->auxdata<int>("HTXS_Stage1_Category_pTjet25");
    m_tree_vbf->HTXS_Stage1_Category_pTjet30 = m_eventInfo->auxdata<int>("HTXS_Stage1_Category_pTjet30");
    m_tree_vbf->HTXS_Stage1_FineIndex_pTjet25 = m_eventInfo->auxdata<int>("HTXS_Stage1_FineIndex_pTjet25");
    m_tree_vbf->HTXS_Stage1_FineIndex_pTjet30 = m_eventInfo->auxdata<int>("HTXS_Stage1_FineIndex_pTjet30");


    m_tree_vbf->hy = 0;
    if (Props::heta.exists(m_eventInfo)) {
      TLorentzVector tlvh;
      tlvh.SetPtEtaPhiM(Props::hpt.get(m_eventInfo),Props::heta.get(m_eventInfo),Props::hphi.get(m_eventInfo),Props::hm.get(m_eventInfo));
      m_tree_vbf->hy = tlvh.Rapidity();
    }
  }
  m_tree_vbf->ewkWeight = 1;
  m_tree_vbf->ewkWeightOff = 1;
  if (m_mcChannel == 345949){
    m_tree_vbf->STXS_bin = getSTXS();

    if (m_tree_vbf->mJJ_truth<60e3) m_tree_vbf->ewkWeightOff = 1.055966*0.981;
    if (60e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<120e3) m_tree_vbf->ewkWeightOff = 1.055966*0.938;
    if (120e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<350e3) m_tree_vbf->ewkWeightOff = 1.055966*0.981;
    if (m_tree_vbf->hpt<200e3 && 350e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<700e3) m_tree_vbf->ewkWeightOff = 1.055966*0.955;
    if (m_tree_vbf->hpt<200e3 && 700e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<1000e3) m_tree_vbf->ewkWeightOff = 1.055966*0.937;
    if (m_tree_vbf->hpt<200e3 && 1000e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<1500e3) m_tree_vbf->ewkWeightOff = 1.055966*0.921;
    if (m_tree_vbf->hpt<200e3 && 1500e3<m_tree_vbf->mJJ_truth) m_tree_vbf->ewkWeightOff = 1.055966*0.899;
    if (m_tree_vbf->hpt>200e3 && 350e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<700e3) m_tree_vbf->ewkWeightOff = 1.055966*0.927;
    if (m_tree_vbf->hpt>200e3 && 700e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<1000e3) m_tree_vbf->ewkWeightOff = 1.055966*0.907;
    if (m_tree_vbf->hpt>200e3 && 1000e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<1500e3) m_tree_vbf->ewkWeightOff = 1.055966*0.883;
    if (m_tree_vbf->hpt>200e3 && 1500e3<m_tree_vbf->mJJ_truth) m_tree_vbf->ewkWeightOff = 1.055966*0.851;

    if (m_tree_vbf->hpt>50e3 && m_tree_vbf->hpt<450e3) m_tree_vbf->ewkWeight *=(0+1.0352055860847063*TMath::Power(m_tree_vbf->hpt/1e3,0)+-0.0003323650501359826*TMath::Power(m_tree_vbf->hpt/1e3,1)+-1.1680829307844583e-07*TMath::Power(m_tree_vbf->hpt/1e3,2));
    if (m_tree_vbf->hpt>450e3) m_tree_vbf->ewkWeight *= 0.868501072;
    m_weight *= m_tree_vbf->ewkWeight;

  }

  if (m_isMC || Props::embZm.exists(m_eventInfo)){
    m_weight *= m_tree_vbf->BJetTriggerWeight;
    m_tree_vbf->EventWeight = m_weight;
    m_tree_vbf->EventWeightforwardSF__1up = m_weight/(m_tree_vbf->forwardSF>0?m_tree_vbf->forwardSF:1);
    m_tree_vbf->EventWeightforwardSF__1down = m_weight/(m_tree_vbf->forwardSF>0?m_tree_vbf->forwardSF:1)*std::max<float>(0.,-1+2*m_tree_vbf->forwardSF);
    m_tree_vbf->EventWeightcentralSF__1up = m_weight/(m_tree_vbf->centralSF>0?m_tree_vbf->centralSF:1);
    m_tree_vbf->EventWeightcentralSF__1down = m_weight/(m_tree_vbf->centralSF>0?m_tree_vbf->centralSF:1)*std::max<float>(0.,-1+2*m_tree_vbf->centralSF);
    m_tree_vbf->EventWeightB2SF__1up = m_weight/(m_tree_vbf->B2SF>0?m_tree_vbf->B2SF:1);
    m_tree_vbf->EventWeightB2SF__1down = m_weight/(m_tree_vbf->B2SF>0?m_tree_vbf->B2SF:1)*std::max<float>(0.,-1+2*m_tree_vbf->B2SF);
    m_tree_vbf->EventWeightBTrig_Eta_EFF__1up = m_weight;
    m_tree_vbf->EventWeightBTrig_Eta_EFF__1down = m_weight;
    m_tree_vbf->EventWeightBTrig_SF__1up = m_weight;
    m_tree_vbf->EventWeightBTrig_SF__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_extrapolation_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_extrapolation_AntiKt4EMPFlowJets__1down = m_weight;
    m_tree_vbf->EventWeightFT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1up = m_weight;
    m_tree_vbf->EventWeightFT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1down = m_weight;
    for(unsigned int i=0; i<m_weightSysts.size(); i++){
      if (m_weightSysts.at(i).name=="BTrig_Eta_EFF__1up") m_tree_vbf->EventWeightBTrig_Eta_EFF__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="BTrig_Eta_EFF__1down") m_tree_vbf->EventWeightBTrig_Eta_EFF__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="BTrig_SF__1up") m_tree_vbf->EventWeightBTrig_SF__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="BTrig_SF__1down") m_tree_vbf->EventWeightBTrig_SF__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_extrapolation_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_extrapolation_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_extrapolation_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_extrapolation_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1down") m_tree_vbf->EventWeightFT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1down *= m_weightSysts.at(i).factor;
      if (m_weightSysts.at(i).name=="FT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1up") m_tree_vbf->EventWeightFT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1up *= m_weightSysts.at(i).factor;
    }
    m_tree_vbf->eventWeightHerwigMbb = m_weight*(m_tree_vbf->mBB_truth<100e3 || m_tree_vbf->mBB_truth>140e3 ? 1 : m_herwigMbb->GetBinContent(m_herwigMbb->FindBin(m_tree_vbf->mBB_truth/1e3)));
    m_tree_vbf->EventWeightShower_mBB__1up = m_weight*(m_tree_vbf->mBB_truth<100e3 || m_tree_vbf->mBB_truth>140e3 ? 1 : m_Shower_mBB->GetBinContent(m_Shower_mBB->FindBin(m_tree_vbf->mBB_truth/1e3)));
    m_tree_vbf->EventWeightShower_mBB__1down = m_weight*(m_tree_vbf->mBB_truth<100e3 || m_tree_vbf->mBB_truth>140e3 || m_Shower_mBB->GetBinContent(m_Shower_mBB->FindBin(m_tree_vbf->mBB_truth/1e3))<1e-3 ? 1 : (1/m_Shower_mBB->GetBinContent(m_Shower_mBB->FindBin(m_tree_vbf->mBB_truth/1e3))));
    m_tree_vbf->EventWeightShower_nJets30pt__1up = m_weight*(m_tree_vbf->nJets_truth<2 || m_tree_vbf->mBB_truth<100e3 || m_tree_vbf->mBB_truth>140e3 ? 1 : m_Shower_nJets30pt->GetBinContent(m_Shower_nJets30pt->FindBin(m_tree_vbf->nJets_truth+2)));
    m_tree_vbf->EventWeightShower_nJets30pt__1down = m_weight*(m_tree_vbf->nJets_truth<2 || m_tree_vbf->mBB_truth<100e3 || m_tree_vbf->mBB_truth>140e3 || m_Shower_nJets30pt->GetBinContent(m_Shower_nJets30pt->FindBin(m_tree_vbf->nJets_truth+2))<1e-3 ? 1 : (1/m_Shower_nJets30pt->GetBinContent(m_Shower_nJets30pt->FindBin(m_tree_vbf->nJets_truth+2))));


  } else {
    m_tree_vbf->EventWeight = 1;
    if (Props::muonEff.exists(m_eventInfo)) m_tree_vbf->EventWeight *= m_tree_vbf->BJetTriggerWeight;
  }


  m_tree_vbf->npv = Props::npv.get(m_eventInfo);
  m_tree_vbf->ActualMu = m_actualMu;
  m_tree_vbf->AverageMu = m_averageMu;
  m_tree_vbf->ActualMuScaled = m_actualMuScaled;
  m_tree_vbf->AverageMuScaled = m_averageMuScaled;
  m_tree_vbf->AverageMuRaw = Props::averageInteractionsPerCrossing.get(m_eventInfo);
  m_tree_vbf->ActualMuRaw = Props::actualInteractionsPerCrossing.get(m_eventInfo);


  m_tree_vbf->embMu1pt = (Props::embMu1pt.exists(m_eventInfo)?Props::embMu1pt.get(m_eventInfo):1);
  m_tree_vbf->embMu1eta = (Props::embMu1eta.exists(m_eventInfo)?Props::embMu1eta.get(m_eventInfo):1);
  m_tree_vbf->embMu1phi = (Props::embMu1phi.exists(m_eventInfo)?Props::embMu1phi.get(m_eventInfo):1);
  m_tree_vbf->embMu1ptvarcone30 = (Props::embMu1ptvarcone30.exists(m_eventInfo)?Props::embMu1ptvarcone30.get(m_eventInfo):1);
  m_tree_vbf->embMu1topoetcone20 = (Props::embMu1topoetcone20.exists(m_eventInfo)?Props::embMu1topoetcone20.get(m_eventInfo):1);
  m_tree_vbf->embMu2pt = (Props::embMu2pt.exists(m_eventInfo)?Props::embMu2pt.get(m_eventInfo):1);
  m_tree_vbf->embMu2eta = (Props::embMu2eta.exists(m_eventInfo)?Props::embMu2eta.get(m_eventInfo):1);
  m_tree_vbf->embMu2phi = (Props::embMu2phi.exists(m_eventInfo)?Props::embMu2phi.get(m_eventInfo):1);
  m_tree_vbf->embMu2ptvarcone30 = (Props::embMu2ptvarcone30.exists(m_eventInfo)?Props::embMu2ptvarcone30.get(m_eventInfo):1);
  m_tree_vbf->embMu2topoetcone20 = (Props::embMu2topoetcone20.exists(m_eventInfo)?Props::embMu2topoetcone20.get(m_eventInfo):1);
  m_tree_vbf->embZpt = (Props::embZpt.exists(m_eventInfo)?Props::embZpt.get(m_eventInfo):1);
  m_tree_vbf->embZeta = (Props::embZeta.exists(m_eventInfo)?Props::embZeta.get(m_eventInfo):1);
  m_tree_vbf->embZphi = (Props::embZphi.exists(m_eventInfo)?Props::embZphi.get(m_eventInfo):1);
  m_tree_vbf->embZm = (Props::embZm.exists(m_eventInfo)?Props::embZm.get(m_eventInfo):1);
  m_tree_vbf->embMuonWeight = 1./(Props::muonEff.exists(m_eventInfo)?Props::muonEff.get(m_eventInfo):1);
  m_tree_vbf->embMuonTrigWeight = 1./(Props::muonTrigEff.exists(m_eventInfo)?Props::muonTrigEff.get(m_eventInfo):1);
  m_tree_vbf->embMuonWeight_cvmfs = 1./(Props::muonEff_cvmfs.exists(m_eventInfo)?Props::muonEff_cvmfs.get(m_eventInfo):1);
  m_tree_vbf->embMuonTrigWeight_cvmfs = 1./(Props::muonTrigEff_cvmfs.exists(m_eventInfo) && Props::muonTrigEff_cvmfs.get(m_eventInfo)?Props::muonTrigEff_cvmfs.get(m_eventInfo):1);
  m_tree_vbf->embMuonTrigWeight_cvmfs_14 = 1./(Props::muonTrigEff_cvmfs_14.exists(m_eventInfo) && Props::muonTrigEff_cvmfs_14.get(m_eventInfo)?Props::muonTrigEff_cvmfs_14.get(m_eventInfo):1);
  m_tree_vbf->embMuonTrigWeight_cvmfs_50 = 1./(Props::muonTrigEff_cvmfs_50.exists(m_eventInfo) && Props::muonTrigEff_cvmfs_50.get(m_eventInfo)?Props::muonTrigEff_cvmfs_50.get(m_eventInfo):1);
  m_tree_vbf->embMet = m_met?m_met->met():0;


  if (m_isMC){
    m_tree_vbf->MCChannelNumber = m_mcChannel;
    m_tree_vbf->MCWeight    = Props::MCEventWeight.get(m_eventInfo);
    m_tree_vbf->PUWeight    = m_pileupReweight;

    m_tree_vbf->EventWeightewkCorrection__1up = m_weight;
    m_tree_vbf->EventWeightewkCorrection__1down = m_weight;
    m_tree_vbf->EventWeightewkCorrectionNorm__1up = m_weight;
    m_tree_vbf->EventWeightewkCorrectionNorm__1down = m_weight;

    m_tree_vbf->EventWeightalpha_s__1up = m_weight;
    m_tree_vbf->EventWeightalpha_s__1down = m_weight;
    m_tree_vbf->EventWeightBR_bb__1up = m_weight;
    m_tree_vbf->EventWeightBR_bb__1down = m_weight;
    m_tree_vbf->EventWeightPDF_VBF__1up = m_weight;
    m_tree_vbf->EventWeightPDF_VBF__1down = m_weight;
    m_tree_vbf->EventWeightQCDscale_VBF__1up = m_weight;
    m_tree_vbf->EventWeightQCDscale_VBF__1down = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF__1up = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF__1down = m_weight;
    m_tree_vbf->EventWeightmuR_0p5_muF_0p5 = m_weight;
    m_tree_vbf->EventWeightmuRmuF__1down = m_weight;
    m_tree_vbf->EventWeightmuR_0p5_muF_1p0 = m_weight;
    m_tree_vbf->EventWeightmuR_0p5_muF_2p0 = m_weight;
    m_tree_vbf->EventWeightmuR_1p0_muF_0p5 = m_weight;
    m_tree_vbf->EventWeightmuR_1p0_muF_1p0 = m_weight;
    m_tree_vbf->EventWeightmuR_1p0_muF_2p0 = m_weight;
    m_tree_vbf->EventWeightmuR_2p0_muF_0p5 = m_weight;
    m_tree_vbf->EventWeightmuR_2p0_muF_1p0 = m_weight;
    m_tree_vbf->EventWeightmuR_2p0_muF_2p0 = m_weight;
    m_tree_vbf->EventWeightmuRmuF__1up = m_weight;
    m_tree_vbf->EventWeightPDF_ggF__1up = m_weight;
    m_tree_vbf->EventWeightPDF_ggF__1down = m_weight;

    m_tree_vbf->EventWeightQCDscale_ggF_mu__1up = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_mu__1down = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_res__1up = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_res__1down = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_mig01__1up = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_mig01__1down = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_mig12__1up = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_mig12__1down = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1up = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1down = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1up = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1down = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1up = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1down = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1up = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1down = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1up = m_weight;
    m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1down = m_weight;

    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV1__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV1__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV2__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV2__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV3__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV3__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV4__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV4__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV5__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV5__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV6__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV6__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV7__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV7__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV8__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV8__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV9__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV9__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV10__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV10__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV11__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV11__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV12__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV12__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV13__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV13__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV14__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV14__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV15__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV15__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV16__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV16__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV17__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV17__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV18__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV18__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV19__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV19__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV20__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV20__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV21__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV21__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV22__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV22__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV23__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV23__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV24__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV24__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV25__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV25__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV26__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV26__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV27__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV27__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV28__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV28__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV29__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV29__1down = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV30__1up = m_weight;
    m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV30__1down = m_weight;

    if (m_mcChannel == 345949){

    if (m_tree_vbf->mJJ_truth<60e3) m_tree_vbf->EventWeightewkCorrection__1up *= 1.012;
    if (60e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<120e3) m_tree_vbf->EventWeightewkCorrection__1up *= 1.012;
    if (120e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<350e3) m_tree_vbf->EventWeightewkCorrection__1up *= 1.012;
    if (m_tree_vbf->hpt<200e3 && 350e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<700e3) m_tree_vbf->EventWeightewkCorrection__1up *= 1.014;
    if (m_tree_vbf->hpt<200e3 && 700e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<1000e3) m_tree_vbf->EventWeightewkCorrection__1up *= 1.015;
    if (m_tree_vbf->hpt<200e3 && 1000e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<1500e3) m_tree_vbf->EventWeightewkCorrection__1up *= 1.016;
    if (m_tree_vbf->hpt<200e3 && 1500e3<m_tree_vbf->mJJ_truth) m_tree_vbf->EventWeightewkCorrection__1up *= 1.019;
    if (m_tree_vbf->hpt>200e3 && 350e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<700e3) m_tree_vbf->EventWeightewkCorrection__1up *= 1.018;
    if (m_tree_vbf->hpt>200e3 && 700e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<1000e3) m_tree_vbf->EventWeightewkCorrection__1up *= 1.017;
    if (m_tree_vbf->hpt>200e3 && 1000e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<1500e3) m_tree_vbf->EventWeightewkCorrection__1up *= 1.017;
    if (m_tree_vbf->hpt>200e3 && 1500e3<m_tree_vbf->mJJ_truth) m_tree_vbf->EventWeightewkCorrection__1up *= 1.022;

    if (m_tree_vbf->mJJ_truth<60e3) m_tree_vbf->EventWeightewkCorrection__1down *= 0.988;
    if (60e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<120e3) m_tree_vbf->EventWeightewkCorrection__1down *= 0.988;
    if (120e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<350e3) m_tree_vbf->EventWeightewkCorrection__1down *= 0.988;
    if (m_tree_vbf->hpt<200e3 && 350e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<700e3) m_tree_vbf->EventWeightewkCorrection__1down *= 0.986;
    if (m_tree_vbf->hpt<200e3 && 700e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<1000e3) m_tree_vbf->EventWeightewkCorrection__1down *= 0.985;
    if (m_tree_vbf->hpt<200e3 && 1000e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<1500e3) m_tree_vbf->EventWeightewkCorrection__1down *= 0.984;
    if (m_tree_vbf->hpt<200e3 && 1500e3<m_tree_vbf->mJJ_truth) m_tree_vbf->EventWeightewkCorrection__1down *= 0.981;
    if (m_tree_vbf->hpt>200e3 && 350e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<700e3) m_tree_vbf->EventWeightewkCorrection__1down *= 0.982;
    if (m_tree_vbf->hpt>200e3 && 700e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<1000e3) m_tree_vbf->EventWeightewkCorrection__1down *= 0.983;
    if (m_tree_vbf->hpt>200e3 && 1000e3<m_tree_vbf->mJJ_truth && m_tree_vbf->mJJ_truth<1500e3) m_tree_vbf->EventWeightewkCorrection__1down *= 0.983;
    if (m_tree_vbf->hpt>200e3 && 1500e3<m_tree_vbf->mJJ_truth) m_tree_vbf->EventWeightewkCorrection__1down *= 0.978;
    m_tree_vbf->EventWeightewkCorrection__1up /= 1.015;
    m_tree_vbf->EventWeightewkCorrection__1down /= 0.985;
    m_tree_vbf->EventWeightewkCorrectionNorm__1up *= 1.015;
    m_tree_vbf->EventWeightewkCorrectionNorm__1down *= 0.985;

/*
      m_tree_vbf->EventWeightalpha_s__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[140]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.986502;
      m_tree_vbf->EventWeightalpha_s__1down *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[141]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00939;
      m_tree_vbf->EventWeightBR_bb__1up *= 1.017;
      m_tree_vbf->EventWeightBR_bb__1down *= 0.983;
      m_tree_vbf->EventWeightPDF_VBF__1up *= 1.021;
      m_tree_vbf->EventWeightPDF_VBF__1down *= 0.979;
      m_tree_vbf->EventWeightQCDscale_VBF__1up *= 1.021;
      m_tree_vbf->EventWeightQCDscale_VBF__1down *= 0.979;
      m_tree_vbf->EventWeightmuR_0p5_muF_0p5 *= Props::MCEventWeightSys.get(m_eventInfo)[1]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.990539;
      m_tree_vbf->EventWeightmuRmuF__1down *= Props::MCEventWeightSys.get(m_eventInfo)[1]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.990539;
      m_tree_vbf->EventWeightmuR_0p5_muF_1p0 *= Props::MCEventWeightSys.get(m_eventInfo)[7]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.995687;
      m_tree_vbf->EventWeightmuR_0p5_muF_2p0 *= Props::MCEventWeightSys.get(m_eventInfo)[4]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.0085;
      m_tree_vbf->EventWeightmuR_1p0_muF_0p5 *= Props::MCEventWeightSys.get(m_eventInfo)[2]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.995336;
      m_tree_vbf->EventWeightmuR_1p0_muF_1p0 *= Props::MCEventWeightSys.get(m_eventInfo)[0]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1;
      m_tree_vbf->EventWeightmuR_1p0_muF_2p0 *= Props::MCEventWeightSys.get(m_eventInfo)[5]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.0077;
      m_tree_vbf->EventWeightmuR_2p0_muF_0p5 *= Props::MCEventWeightSys.get(m_eventInfo)[3]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.00036;
      m_tree_vbf->EventWeightmuR_2p0_muF_1p0 *= Props::MCEventWeightSys.get(m_eventInfo)[8]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.997717;
      m_tree_vbf->EventWeightmuR_2p0_muF_2p0 *= Props::MCEventWeightSys.get(m_eventInfo)[6]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.00414;
      m_tree_vbf->EventWeightmuRmuF__1up *= Props::MCEventWeightSys.get(m_eventInfo)[6]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.00414;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV1__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+1]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.99898;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV1__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+1]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999011;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV2__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+2]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00286;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV2__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+2]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.993544;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV3__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+3]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999519;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV3__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+3]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00021;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV4__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+4]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.01216;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV4__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+4]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.984255;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV5__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+5]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.995955;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV5__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+5]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00312;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV6__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+6]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.01253;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV6__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+6]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.984583;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV7__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+7]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.998877;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV7__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+7]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00068;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV8__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+8]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00366;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV8__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+8]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.99581;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV9__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+9]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.995444;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV9__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+9]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00384;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV10__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+10]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00264;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV10__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+10]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.996927;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV11__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+11]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00339;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV11__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+11]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.995101;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV12__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+12]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00596;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV12__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+12]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.992641;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV13__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+13]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999785;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV13__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+13]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999951;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV14__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+14]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.994908;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV14__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+14]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00401;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV15__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+15]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.997169;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV15__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+15]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00206;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV16__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+16]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00105;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV16__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+16]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.998375;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV17__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+17]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.998271;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV17__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+17]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00109;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV18__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+18]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.994432;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV18__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+18]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00471;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV19__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+19]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00478;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV19__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+19]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.994472;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV20__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+20]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00768;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV20__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+20]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9911;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV21__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+21]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00255;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV21__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+21]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.996866;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV22__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+22]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.997851;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV22__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+22]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00179;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV23__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+23]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999857;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV23__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+23]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999936;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV24__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+24]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.997292;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV24__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+24]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00206;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV25__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+25]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00205;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV25__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+25]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.997623;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV26__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+26]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.99199;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV26__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+26]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00676;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV27__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+27]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.998291;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV27__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+27]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00111;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV28__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+28]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.998919;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV28__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+28]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00074;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV29__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+29]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00264;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV29__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+29]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.996769;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV30__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+30]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.997477;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV30__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+30]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.0021;

*/












/*
      m_tree_vbf->EventWeightalpha_s__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[140]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999448;
      m_tree_vbf->EventWeightalpha_s__1down *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[141]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00074;
      m_tree_vbf->EventWeightBR_bb__1up *= 1.017;
      m_tree_vbf->EventWeightBR_bb__1down *= 0.983;
      m_tree_vbf->EventWeightPDF_VBF__1up *= 1.021;
      m_tree_vbf->EventWeightPDF_VBF__1down *= 0.979;
      m_tree_vbf->EventWeightQCDscale_VBF__1up *= 1.021;
      m_tree_vbf->EventWeightQCDscale_VBF__1down *= 0.979;
      m_tree_vbf->EventWeightmuR_0p5_muF_0p5 *= Props::MCEventWeightSys.get(m_eventInfo)[1]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.992497;
      m_tree_vbf->EventWeightmuRmuF__1down *= Props::MCEventWeightSys.get(m_eventInfo)[1]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.992497;
      m_tree_vbf->EventWeightmuR_0p5_muF_1p0 *= Props::MCEventWeightSys.get(m_eventInfo)[7]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.994263;
      m_tree_vbf->EventWeightmuR_0p5_muF_2p0 *= Props::MCEventWeightSys.get(m_eventInfo)[4]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.998262;
      m_tree_vbf->EventWeightmuR_1p0_muF_0p5 *= Props::MCEventWeightSys.get(m_eventInfo)[2]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.999575;
      m_tree_vbf->EventWeightmuR_1p0_muF_1p0 *= Props::MCEventWeightSys.get(m_eventInfo)[0]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1;
      m_tree_vbf->EventWeightmuR_1p0_muF_2p0 *= Props::MCEventWeightSys.get(m_eventInfo)[5]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.00185;
      m_tree_vbf->EventWeightmuR_2p0_muF_0p5 *= Props::MCEventWeightSys.get(m_eventInfo)[3]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.00203;
      m_tree_vbf->EventWeightmuR_2p0_muF_1p0 *= Props::MCEventWeightSys.get(m_eventInfo)[8]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.00397;
      m_tree_vbf->EventWeightmuR_2p0_muF_2p0 *= Props::MCEventWeightSys.get(m_eventInfo)[6]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.00655;
      m_tree_vbf->EventWeightmuRmuF__1up *= Props::MCEventWeightSys.get(m_eventInfo)[6]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.00655;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV1__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+1]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00074;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV1__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+1]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999258;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV2__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+2]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00992;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV2__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+2]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.989833;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV3__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+3]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999833;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV3__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+3]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00017;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV4__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+4]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.998081;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV4__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+4]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00191;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV5__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+5]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999752;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV5__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+5]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00025;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV6__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+6]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00037;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV6__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+6]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999619;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV7__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+7]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00047;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV7__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+7]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999525;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV8__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+8]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00029;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV8__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+8]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999708;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV9__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+9]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999809;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV9__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+9]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00019;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV10__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+10]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00027;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV10__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+10]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999726;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV11__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+11]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00083;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV11__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+11]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999165;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV12__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+12]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.0001;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV12__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+12]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999899;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV13__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+13]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.99982;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV13__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+13]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00018;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV14__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+14]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999857;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV14__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+14]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00014;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV15__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+15]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999432;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV15__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+15]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00057;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV16__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+16]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999939;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV16__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+16]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00006;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV17__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+17]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00009;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV17__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+17]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999909;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV18__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+18]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00002;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV18__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+18]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999981;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV19__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+19]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00047;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV19__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+19]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999532;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV20__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+20]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00036;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV20__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+20]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999634;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV21__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+21]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00117;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV21__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+21]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.998827;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV22__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+22]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00024;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV22__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+22]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999757;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV23__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+23]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00007;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV23__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+23]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999927;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV24__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+24]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.99944;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV24__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+24]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00056;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV25__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+25]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00023;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV25__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+25]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999766;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV26__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+26]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999917;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV26__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+26]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00008;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV27__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+27]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.99986;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV27__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+27]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00014;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV28__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+28]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV28__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+28]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV29__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+29]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00024;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV29__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+29]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.999756;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV30__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+30]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.998918;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV30__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+30]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00108;
*/


      m_tree_vbf->EventWeightalpha_s__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[140]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9933153009;
      m_tree_vbf->EventWeightalpha_s__1down *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[141]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.005762721;
      m_tree_vbf->EventWeightmuRmuF__1down *= Props::MCEventWeightSys.get(m_eventInfo)[1]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.9958050091;
      m_tree_vbf->EventWeightmuRmuF__1up *= Props::MCEventWeightSys.get(m_eventInfo)[6]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.005300248;
      m_tree_vbf->EventWeightBR_bb__1up *= 1.017;
      m_tree_vbf->EventWeightBR_bb__1down *= 0.983;
      m_tree_vbf->EventWeightPDF_VBF__1up *= 1.021;
      m_tree_vbf->EventWeightPDF_VBF__1down *= 0.979;
      m_tree_vbf->EventWeightQCDscale_VBF__1up *= 1.004;
      m_tree_vbf->EventWeightQCDscale_VBF__1down *= 0.997;
      m_tree_vbf->EventWeightmuR_0p5_muF_0p5 *= Props::MCEventWeightSys.get(m_eventInfo)[1]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.9958050091;
      m_tree_vbf->EventWeightmuR_0p5_muF_1p0 *= Props::MCEventWeightSys.get(m_eventInfo)[7]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.9981424566;
      m_tree_vbf->EventWeightmuR_0p5_muF_2p0 *= Props::MCEventWeightSys.get(m_eventInfo)[4]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.006660624;
      m_tree_vbf->EventWeightmuR_1p0_muF_0p5 *= Props::MCEventWeightSys.get(m_eventInfo)[2]/Props::MCEventWeightSys.get(m_eventInfo)[0]/0.9992182395;
      m_tree_vbf->EventWeightmuR_1p0_muF_1p0 *= Props::MCEventWeightSys.get(m_eventInfo)[0]/Props::MCEventWeightSys.get(m_eventInfo)[0];
      m_tree_vbf->EventWeightmuR_1p0_muF_2p0 *= Props::MCEventWeightSys.get(m_eventInfo)[5]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.005905676;
      m_tree_vbf->EventWeightmuR_2p0_muF_0p5 *= Props::MCEventWeightSys.get(m_eventInfo)[3]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.001955453;
      m_tree_vbf->EventWeightmuR_2p0_muF_1p0 *= Props::MCEventWeightSys.get(m_eventInfo)[8]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.001499174;
      m_tree_vbf->EventWeightmuR_2p0_muF_2p0 *= Props::MCEventWeightSys.get(m_eventInfo)[6]/Props::MCEventWeightSys.get(m_eventInfo)[0]/1.005300248;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV1__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+1]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000857666;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV1__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+1]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9991423332;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV2__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+2]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00962393;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV2__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+2]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9903760699;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV3__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+3]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9997841456;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV3__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+3]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000215849;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV4__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+4]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.006020664;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV4__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+4]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9939793352;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV5__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+5]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9979021235;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV5__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+5]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.002097876;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV6__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+6]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.007155673;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV6__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+6]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9928443268;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV7__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+7]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9998269206;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV7__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+7]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000173074;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV8__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+8]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.002283708;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV8__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+8]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9977162906;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV9__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+9]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9977159235;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV9__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+9]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.002284075;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV10__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+10]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.001692685;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV10__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+10]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.998307313;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV11__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+11]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.002823922;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV11__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+11]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9971760763;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV12__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+12]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00347933;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV12__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+12]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9965206698;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV13__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+13]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9998623203;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV13__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+13]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000137676;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV14__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+14]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9975822838;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV14__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+14]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.002417715;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV15__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+15]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9981628226;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV15__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+15]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.001837177;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV16__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+16]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000377861;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV16__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+16]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9996221372;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV17__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+17]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.99945246;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV17__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+17]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000547539;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV18__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+18]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9972421896;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV18__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+18]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.002757809;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV19__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+19]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.003033748;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV19__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+19]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9969662511;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV20__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+20]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.004399188;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV20__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+20]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9956008112;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV21__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+21]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.002287273;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV21__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+21]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9977127257;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV22__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+22]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9991816026;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV22__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+22]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000818396;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV23__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+23]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000027604;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV23__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+23]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9999723905;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV24__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+24]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.998372715;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV24__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+24]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.001627284;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV25__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+25]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.001240373;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV25__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+25]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9987596249;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV26__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+26]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9963222317;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV26__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+26]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.003677767;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV27__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+27]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9992306595;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV27__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+27]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000769339;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV28__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+28]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9995404948;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV28__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+28]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000459503;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV29__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+29]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.001702618;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV29__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+29]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9982973762;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV30__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[109+30]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9980385409;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV30__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[109+30]-Props::MCEventWeightSys.get(m_eventInfo)[109])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.001961454;

    } else if (m_mcChannel == 345342){
      m_tree_vbf->STXS_bin = getSTXS();
      m_tree_vbf->EventWeightalpha_s__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[142]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9769759194;
      m_tree_vbf->EventWeightalpha_s__1down *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[143]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.021981452;
      m_tree_vbf->EventWeightBR_bb__1up *= 1.017;
      m_tree_vbf->EventWeightBR_bb__1down *= 0.983;
      m_tree_vbf->EventWeightPDF_ggF__1up *= 1.032;
      m_tree_vbf->EventWeightPDF_ggF__1down *= 0.968;
      m_tree_vbf->EventWeightQCDscale_ggF__1up *= (1+0.039);
      m_tree_vbf->EventWeightQCDscale_ggF__1down *= (1-0.039);

      if (m_tree_vbf->nJets_truth==0) {
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1up *= (1+0.038);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1up *= (1+0.001);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1up *= (1-0.041);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1down *= (1-0.038);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1down *= (1-0.001);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1down *= (1+0.041);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1down *= (1-0);
      }
      if (m_tree_vbf->nJets_truth==1 && m_tree_vbf->hpt<60e3) {
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1up *= (1+0.052);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1up *= (1+0.045);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1up *= (1+0.079);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1up *= (1-0.068);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1up *= (1-0.047);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1up *= (1-0.016);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1down *= (1-0.052);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1down *= (1-0.045);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1down *= (1-0.079);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1down *= (1+0.068);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1down *= (1+0.047);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1down *= (1+0.016);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1down *= (1-0);
      }
      if (m_tree_vbf->nJets_truth==1 && 60e3<m_tree_vbf->hpt && m_tree_vbf->hpt<120e3) {
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1up *= (1+0.052);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1up *= (1+0.045);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1up *= (1+0.079);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1up *= (1-0.068);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1up *= (1+0.051);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1up *= (1-0.008);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1down *= (1-0.052);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1down *= (1-0.045);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1down *= (1-0.079);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1down *= (1+0.068);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1down *= (1-0.051);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1down *= (1+0.008);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1down *= (1-0);
      }
      if (m_tree_vbf->nJets_truth==1 && 120e3<m_tree_vbf->hpt && m_tree_vbf->hpt<200e3) {
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1up *= (1+0.052);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1up *= (1+0.045);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1up *= (1+0.079);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1up *= (1-0.068);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1up *= (1+0.10);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1up *= (1+0.102);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1up *= (1+0.005);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1down *= (1-0.052);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1down *= (1-0.045);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1down *= (1-0.079);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1down *= (1+0.068);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1down *= (1-0.10);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1down *= (1-0.102);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1down *= (1-0.005);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1down *= (1-0);
      }
      if (m_tree_vbf->nJets_truth==1 && 200e3<m_tree_vbf->hpt) {
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1up *= (1+0.052);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1up *= (1+0.045);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1up *= (1+0.079);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1up *= (1-0.068);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1up *= (1+0.10);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1up *= (1+0.14);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1up *= (1+0.104);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1down *= (1-0.052);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1down *= (1-0.045);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1down *= (1-0.079);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1down *= (1+0.068);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1down *= (1-0.10);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1down *= (1-0.14);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1down *= (1-0.104);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1down *= (1-0);
      }
      if (m_tree_vbf->nJets_truth==2 && m_tree_vbf->mJJ_truth>400e3 && m_tree_vbf->dEtaJJ_truth>2.8) {
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1up *= (1-0.01);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1up *= (1+0.017);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1up *= (1+0.001);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1up *= (1+0.20);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1up *= (1-0.32);
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1down *= (1+0.01);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1down *= (1-0.017);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1down *= (1-0.001);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1down *= (1-0.20);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1down *= (1+0.32);
      }
      else if (m_tree_vbf->nJets_truth>2 && m_tree_vbf->mJJ_truth>400e3 && m_tree_vbf->dEtaJJ_truth>2.8) {
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1up *= (1+0.005);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1up *= (1+0.032);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1up *= (1+0.003);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1up *= (1+0.20);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1up *= (1+0.235);
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1down *= (1-0.005);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1down *= (1-0.032);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1down *= (1-0.003);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1down *= (1-0.20);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1down *= (1-0.235);
      }
      else if (m_tree_vbf->nJets_truth>=2 && m_tree_vbf->hpt<60e3) {
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1up *= (1+0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1up *= (1+0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1up *= (1+0.044);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1up *= (1+0.182);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1up *= (1-0.058);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1up *= (1-0.016);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1down *= (1-0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1down *= (1-0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1down *= (1-0.044);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1down *= (1-0.182);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1down *= (1+0.058);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1down *= (1+0.016);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1down *= (1-0);
      }
      else if (m_tree_vbf->nJets_truth>=2 && 60e3<m_tree_vbf->hpt && m_tree_vbf->hpt<120e3) {
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1up *= (1+0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1up *= (1+0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1up *= (1+0.044);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1up *= (1+0.182);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1up *= (1-0.001);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1up *= (1-0.001);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1down *= (1-0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1down *= (1-0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1down *= (1-0.044);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1down *= (1-0.182);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1down *= (1+0.001);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1down *= (1+0.001);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1down *= (1-0);
      }
      else if (m_tree_vbf->nJets_truth>=2 && 120e3<m_tree_vbf->hpt && m_tree_vbf->hpt<200e3) {
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1up *= (1+0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1up *= (1+0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1up *= (1+0.044);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1up *= (1+0.182);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1up *= (1+0.067);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1up *= (1+0.108);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1up *= (1+0.007);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1down *= (1-0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1down *= (1-0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1down *= (1-0.044);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1down *= (1-0.182);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1down *= (1-0.067);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1down *= (1-0.108);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1down *= (1-0.007);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1down *= (1-0);
      }
      else if (m_tree_vbf->nJets_truth>=2 && 200e3<m_tree_vbf->hpt) {
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1up *= (1+0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1up *= (1+0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1up *= (1+0.044);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1up *= (1+0.182);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1up *= (1+0.10);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1up *= (1+0.14);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1up *= (1+0.116);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1up *= (1+0);
        m_tree_vbf->EventWeightQCDscale_ggF_mu__1down *= (1-0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_res__1down *= (1-0.089);
        m_tree_vbf->EventWeightQCDscale_ggF_mig01__1down *= (1-0.044);
        m_tree_vbf->EventWeightQCDscale_ggF_mig12__1down *= (1-0.182);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH60__1down *= (1-0.10);
        m_tree_vbf->EventWeightQCDscale_ggF_pTH120__1down *= (1-0.14);
        m_tree_vbf->EventWeightQCDscale_ggF_qm_t__1down *= (1-0.116);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf2j__1down *= (1-0);
        m_tree_vbf->EventWeightQCDscale_ggF_vbf3j__1down *= (1-0);
      }





      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV1__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+1]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.002882397;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV1__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+1]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9971168309;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV2__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+2]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9961612331;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV2__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+2]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.003837995;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV3__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+3]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9996681402;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV3__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+3]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000331088;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV4__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+4]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9982722624;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV4__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+4]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.001726966;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV5__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+5]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.016085143;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV5__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+5]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9839140857;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV6__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+6]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9967662504;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV6__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+6]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.003232978;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV7__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+7]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9996402741;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV7__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+7]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000358954;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV8__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+8]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000620239;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV8__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+8]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9993789887;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV9__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+9]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9974695544;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV9__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+9]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.002529674;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV10__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+10]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000570223;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV10__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+10]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9994290055;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV11__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+11]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9974254861;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV11__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+11]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.002573742;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV12__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+12]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9974377671;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV12__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+12]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.002561461;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV13__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+13]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000239976;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV13__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+13]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9997592517;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV14__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+14]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9998977682;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV14__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+14]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00010146;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV15__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+15]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00141417;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV15__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+15]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9985850576;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV16__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+16]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9993928408;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV16__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+16]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000606387;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV17__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+17]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9957701198;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV17__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+17]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.004229108;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV18__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+18]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.001808303;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV18__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+18]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9981909254;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV19__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+19]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9955074539;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV19__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+19]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.004491774;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV20__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+20]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9994119517;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV20__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+20]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000587276;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV21__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+21]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9994766842;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV21__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+21]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000522544;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV22__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+22]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.001602555;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV22__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+22]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9983966732;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV23__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+23]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000552744;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV23__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+23]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9994464845;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV24__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+24]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9995119296;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV24__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+24]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000487299;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV25__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+25]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9993462506;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV25__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+25]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000652978;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV26__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+26]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00143642;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV26__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+26]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9985628078;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV27__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+27]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000855178;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV27__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+27]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9991440499;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV28__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+28]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.005839009;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV28__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+28]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9941602187;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV29__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+29]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.000982342;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV29__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+29]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9990168866;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV30__1up *= (1+(Props::MCEventWeightSys.get(m_eventInfo)[111+30]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/0.9995923184;
      m_tree_vbf->EventWeightPDF4LHC_NLO_30_EV30__1down *= (1-(Props::MCEventWeightSys.get(m_eventInfo)[111+30]-Props::MCEventWeightSys.get(m_eventInfo)[111])/Props::MCEventWeightSys.get(m_eventInfo)[0])/1.00040691;

    }
      
   
    if (false && m_mcChannel == 445338 or m_mcChannel == 445342){
      
      m_tree_vbf->alpha_s_up = Props::alpha_s_up.get(m_eventInfo);
      m_tree_vbf->alpha_s_dn = Props::alpha_s_dn.get(m_eventInfo);
      
      for(unsigned int i=0; i< Props::weight_pdf4lhc.get(m_eventInfo).size() ; i++){
	m_tree_vbf->weightspdf4lhc.push_back( Props::weight_pdf4lhc.get(m_eventInfo)[i]);
      }
      for(unsigned int i=0; i< Props::weight_pdfnnpdf30.get(m_eventInfo).size() ; i++){
	m_tree_vbf->weightspdfnnpdf30.push_back( Props::weight_pdfnnpdf30.get(m_eventInfo)[i]);
      }
      for(unsigned int i=0; i< Props::weight_qcd_nnlops.get(m_eventInfo).size() ; i++){
	m_tree_vbf->weightsqcdnnlops.push_back( Props::weight_qcd_nnlops.get(m_eventInfo)[i]);
      }
      for(unsigned int i=0; i< Props::weight_qcd_nnlops_2np.get(m_eventInfo).size() ; i++){
	m_tree_vbf->weightsqcdnnlops2np.push_back( Props::weight_qcd_nnlops_2np.get(m_eventInfo)[i]);
      }
      for(unsigned int i=0; i< Props::weight_qcd_WG1.get(m_eventInfo).size() ; i++){
	m_tree_vbf->weightsqcdwg1.push_back( Props::weight_qcd_WG1.get(m_eventInfo)[i]);
      }
      for(unsigned int i=0; i< Props::weight_alternative_pdf.get(m_eventInfo).size() ; i++){
	m_tree_vbf->weightsalternativepdf.push_back( Props::weight_alternative_pdf.get(m_eventInfo)[i]);
      }
    }
  }
  return EL::StatusCode::SUCCESS;

}

EL::StatusCode AnalysisReader_VBFHbb::save_trigger_info(){

  // ------------------------
  // Collecting Trigger Information
  // ------------------------

  // trigger decisions

  m_tree_vbf->passHLT_2mu14 = Props::passHLT_2mu14.get(m_eventInfo);
  m_tree_vbf->passHLT_mu50 = Props::passHLT_mu50.get(m_eventInfo);
  m_tree_vbf->passL1_J40_0ETA25_2J25_J20_31ETA49 = Props::passL1_J40_0ETA25_2J25_J20_31ETA49.get(m_eventInfo);
  m_tree_vbf->passL1_J40_0ETA25_2J25 = Props::passL1_J40_0ETA25_2J25.get(m_eventInfo);
  m_tree_vbf->passL1_J20_31ETA49 = Props::passL1_J20_31ETA49.get(m_eventInfo);
  m_tree_vbf->passL1_J15_31ETA49 = Props::passL1_J15_31ETA49.exists(m_eventInfo)?Props::passL1_J15_31ETA49.get(m_eventInfo):0;
  m_tree_vbf->passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490 = Props::passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490.get(m_eventInfo);
  m_tree_vbf->passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 = Props::passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->passHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 = Props::passHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->passHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 = Props::passHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->passHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 = Props::passHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25 = Props::passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.exists(m_eventInfo)?Props::passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.get(m_eventInfo):0;
  m_tree_vbf->passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25 = Props::passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.exists(m_eventInfo)?Props::passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.get(m_eventInfo):0;
  m_tree_vbf->passHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25 = Props::passHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25.exists(m_eventInfo)?Props::passHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25.get(m_eventInfo):0;
  m_tree_vbf->passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30 = Props::passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30.exists(m_eventInfo)?Props::passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30.get(m_eventInfo):0;
  m_tree_vbf->passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30 = Props::passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30.exists(m_eventInfo)?Props::passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30.get(m_eventInfo):0;
  m_tree_vbf->passHLT_j100_2j55_bmv2c2060_split = Props::passHLT_j100_2j55_bmv2c2060_split.exists(m_eventInfo)?Props::passHLT_j100_2j55_bmv2c2060_split.get(m_eventInfo):0;
  m_tree_vbf->passHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 = Props::passHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->passHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 = Props::passHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->passHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 = Props::passHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->passHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 = Props::passHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->passHLT_j175_gsc225_bmv2c1040_split = Props::passHLT_j175_gsc225_bmv2c1040_split.get(m_eventInfo);
  m_tree_vbf->passHLT_j175_bmv2c2040_split = Props::passHLT_j175_bmv2c2040_split.exists(m_eventInfo)?Props::passHLT_j175_bmv2c2040_split.get(m_eventInfo):0;
  m_tree_vbf->passHLT_j225_gsc275_bmv2c1060_split = Props::passHLT_j225_gsc275_bmv2c1060_split.get(m_eventInfo);
  m_tree_vbf->passHLT_j225_gsc300_bmv2c1070_split = Props::passHLT_j225_gsc300_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->passHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490 = Props::passHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490.get(m_eventInfo);

  m_tree_vbf->passHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split = Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split.exists(m_eventInfo)?Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split.get(m_eventInfo):0;

  m_tree_vbf->passHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split = Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split.exists(m_eventInfo)?Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split.get(m_eventInfo):0;
  m_tree_vbf->passHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split = Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->passHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = Props::passHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.get(m_eventInfo);
  m_tree_vbf->passHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = Props::passHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.exists(m_eventInfo)?Props::passHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.get(m_eventInfo):0;
  //m_tree_vbf->passHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = Props::passHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.get(m_eventInfo);
  m_tree_vbf->passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split = Props::passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split = Props::passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF = Props::passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF.get(m_eventInfo);
  m_tree_vbf->passHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split = Props::passHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->passHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF = Props::passHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF.get(m_eventInfo);
  m_tree_vbf->passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split = Props::passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split.get(m_eventInfo);
  m_tree_vbf->passHLT_j45_320eta490 = Props::passHLT_j45_320eta490.get(m_eventInfo);
  m_tree_vbf->passHLT_j80_0eta240_j60_j45_320eta490 = Props::passHLT_j80_0eta240_j60_j45_320eta490.get(m_eventInfo);
  m_tree_vbf->passHLT_j80_0eta240_j60 = Props::passHLT_j80_0eta240_j60.get(m_eventInfo);

  m_tree_vbf->passL1_J40_0ETA25_2J15_31ETA49 = Props::passL1_J40_0ETA25_2J15_31ETA49.get(m_eventInfo);
  m_tree_vbf->passL1_J40_0ETA25 = Props::passL1_J40_0ETA25.get(m_eventInfo);
  m_tree_vbf->passL1_2J15_31ETA49 = Props::passL1_2J15_31ETA49.get(m_eventInfo);
  m_tree_vbf->passHLT_j80_bmv2c2085_split_2j60_320eta490 = Props::passHLT_j80_bmv2c2085_split_2j60_320eta490.get(m_eventInfo);
  m_tree_vbf->passHLT_j80_bmv2c2085_split = Props::passHLT_j80_bmv2c2085_split.get(m_eventInfo);
  m_tree_vbf->passHLT_2j60_320eta490 = Props::passHLT_2j60_320eta490.get(m_eventInfo);
  m_tree_vbf->passHLT_j80_0eta240_2j60_320eta490 = Props::passHLT_j80_0eta240_2j60_320eta490.get(m_eventInfo);
  m_tree_vbf->passHLT_j80_0eta240 = Props::passHLT_j80_0eta240.get(m_eventInfo);

  m_tree_vbf->passL1_4J20 = Props::passL1_4J20.get(m_eventInfo);
  m_tree_vbf->passHLT_2j55_bmv2c2077_split_2j55 = Props::passHLT_2j55_bmv2c2077_split_2j55.get(m_eventInfo);
  m_tree_vbf->passHLT_2j45_bmv2c2070_split_2j45 = Props::passHLT_2j45_bmv2c2070_split_2j45.get(m_eventInfo);

  m_tree_vbf->passL1_4J15 = Props::passL1_4J15.get(m_eventInfo);
  m_tree_vbf->passHLT_2j35_bmv2c2070_split_2j35_L14J15 = Props::passHLT_2j35_bmv2c2070_split_2j35_L14J15.get(m_eventInfo);
  m_tree_vbf->passHLT_2j35_bmv2c2060_split_2j35_L14J15 = Props::passHLT_2j35_bmv2c2060_split_2j35_L14J15.get(m_eventInfo);
  m_tree_vbf->passHLT_2j35_bmv2c2050_split_2j35_L14J15 = Props::passHLT_2j35_bmv2c2050_split_2j35_L14J15.get(m_eventInfo);
  m_tree_vbf->passHLT_j75_bmv2c2060_split_3j75_L14J15 = Props::passHLT_j75_bmv2c2060_split_3j75_L14J15.get(m_eventInfo);
  m_tree_vbf->passHLT_j75_bmv2c2070_split_3j75_L14J15 = Props::passHLT_j75_bmv2c2070_split_3j75_L14J15.get(m_eventInfo);

  m_tree_vbf->passHLT_2j45_bmv2c2077_split_2j45_L14J15 = Props::passHLT_2j45_bmv2c2077_split_2j45_L14J15.get(m_eventInfo);
  m_tree_vbf->passHLT_2j45_bmv2c2070_split_2j45_L14J15 = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(m_eventInfo);
  m_tree_vbf->passHLT_4j45 = Props::passHLT_4j45.get(m_eventInfo);

  // trigger activity
  m_tree_vbf->isActiveL1_J40_0ETA25_2J25_J20_31ETA49 = Props::isActiveL1_J40_0ETA25_2J25_J20_31ETA49.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490 = Props::isActiveHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 = Props::isActiveHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 = Props::isActiveHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 = Props::isActiveHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 = Props::isActiveHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25 = Props::isActiveHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.exists(m_eventInfo)?Props::isActiveHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.get(m_eventInfo):0;
  m_tree_vbf->isActiveHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25 = Props::isActiveHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.exists(m_eventInfo)?Props::isActiveHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.get(m_eventInfo):0;
  m_tree_vbf->isActiveHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25 = Props::isActiveHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25.exists(m_eventInfo)?Props::isActiveHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25.get(m_eventInfo):0;
  m_tree_vbf->isActiveHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30 = Props::isActiveHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30.exists(m_eventInfo)?Props::isActiveHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30.get(m_eventInfo):0;
  m_tree_vbf->isActiveHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30 = Props::isActiveHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30.exists(m_eventInfo)?Props::isActiveHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30.get(m_eventInfo):0;
  m_tree_vbf->isActiveHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 = Props::isActiveHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 = Props::isActiveHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 = Props::isActiveHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 = Props::isActiveHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j175_gsc225_bmv2c1040_split = Props::isActiveHLT_j175_gsc225_bmv2c1040_split.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j175_bmv2c2040_split = Props::isActiveHLT_j175_bmv2c2040_split.exists(m_eventInfo)?Props::isActiveHLT_j175_bmv2c2040_split.get(m_eventInfo):0;
  m_tree_vbf->isActiveHLT_j225_gsc275_bmv2c1060_split = Props::isActiveHLT_j225_gsc275_bmv2c1060_split.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j225_gsc300_bmv2c1070_split = Props::isActiveHLT_j225_gsc300_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490 = Props::isActiveHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split = Props::isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split.exists(m_eventInfo)?Props::isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split.get(m_eventInfo):0;
  m_tree_vbf->isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split = Props::isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split.exists(m_eventInfo)?Props::isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split.get(m_eventInfo):0;
  m_tree_vbf->isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split = Props::isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = Props::isActiveHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = Props::isActiveHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.exists(m_eventInfo)?Props::isActiveHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.get(m_eventInfo):0;
  //m_tree_vbf->isActiveHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = Props::isActiveHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split = Props::isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split = Props::isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF = Props::isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split = Props::isActiveHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF = Props::isActiveHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j80_0eta240_j60_j45_320eta490 = Props::isActiveHLT_j80_0eta240_j60_j45_320eta490.get(m_eventInfo);

  m_tree_vbf->isActiveL1_J40_0ETA25_2J15_31ETA49 = Props::isActiveL1_J40_0ETA25_2J15_31ETA49.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j80_bmv2c2085_split_2j60_320eta490 = Props::isActiveHLT_j80_bmv2c2085_split_2j60_320eta490.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j80_0eta240_2j60_320eta490 = Props::isActiveHLT_j80_0eta240_2j60_320eta490.get(m_eventInfo);

  m_tree_vbf->isActiveL1_4J20 = Props::isActiveL1_4J20.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j55_bmv2c2077_split_2j55 = Props::isActiveHLT_2j55_bmv2c2077_split_2j55.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j45_bmv2c2070_split_2j45 = Props::isActiveHLT_2j45_bmv2c2070_split_2j45.get(m_eventInfo);

  m_tree_vbf->isActiveL1_4J15 = Props::isActiveL1_4J15.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j35_bmv2c2070_split_2j35_L14J15 = Props::isActiveHLT_2j35_bmv2c2070_split_2j35_L14J15.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j35_bmv2c2060_split_2j35_L14J15 = Props::isActiveHLT_2j35_bmv2c2060_split_2j35_L14J15.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j35_bmv2c2050_split_2j35_L14J15 = Props::isActiveHLT_2j35_bmv2c2050_split_2j35_L14J15.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j75_bmv2c2060_split_3j75_L14J15 = Props::isActiveHLT_j75_bmv2c2060_split_3j75_L14J15.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_j75_bmv2c2070_split_3j75_L14J15 = Props::isActiveHLT_j75_bmv2c2070_split_3j75_L14J15.get(m_eventInfo);

  m_tree_vbf->isActiveHLT_2j45_bmv2c2077_split_2j45_L14J15 = Props::isActiveHLT_2j45_bmv2c2077_split_2j45_L14J15.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_2j45_bmv2c2070_split_2j45_L14J15 = Props::isActiveHLT_2j45_bmv2c2070_split_2j45_L14J15.get(m_eventInfo);
  m_tree_vbf->isActiveHLT_4j45 = Props::isActiveHLT_4j45.get(m_eventInfo);

  m_tree_vbf->prescaleL1_J40_0ETA25_2J25_J20_31ETA49 = Props::prescaleL1_J40_0ETA25_2J25_J20_31ETA49.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490 = Props::prescaleHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 = Props::prescaleHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 = Props::prescaleHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 = Props::prescaleHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 = Props::prescaleHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25 = Props::prescaleHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.exists(m_eventInfo)?Props::prescaleHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.get(m_eventInfo):0;
  m_tree_vbf->prescaleHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25 = Props::prescaleHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.exists(m_eventInfo)?Props::prescaleHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.get(m_eventInfo):0;
  m_tree_vbf->prescaleHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25 = Props::prescaleHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25.exists(m_eventInfo)?Props::prescaleHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25.get(m_eventInfo):0;
  m_tree_vbf->prescaleHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30 = Props::prescaleHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30.exists(m_eventInfo)?Props::prescaleHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30.get(m_eventInfo):0;
  m_tree_vbf->prescaleHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30 = Props::prescaleHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30.exists(m_eventInfo)?Props::prescaleHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30.get(m_eventInfo):0;
  m_tree_vbf->prescaleHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 = Props::prescaleHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 = Props::prescaleHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 = Props::prescaleHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 = Props::prescaleHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j175_gsc225_bmv2c1040_split = Props::prescaleHLT_j175_gsc225_bmv2c1040_split.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j175_bmv2c2040_split = Props::prescaleHLT_j175_bmv2c2040_split.exists(m_eventInfo)?Props::prescaleHLT_j175_bmv2c2040_split.get(m_eventInfo):0;
  m_tree_vbf->prescaleHLT_j225_gsc275_bmv2c1060_split = Props::prescaleHLT_j225_gsc275_bmv2c1060_split.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j225_gsc300_bmv2c1070_split = Props::prescaleHLT_j225_gsc300_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490 = Props::prescaleHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split = Props::prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split.exists(m_eventInfo)?Props::prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split.get(m_eventInfo):0;
  m_tree_vbf->prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split = Props::prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split.exists(m_eventInfo)?Props::prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split.get(m_eventInfo):0;
  m_tree_vbf->prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split = Props::prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = Props::prescaleHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = Props::prescaleHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.exists(m_eventInfo)?Props::prescaleHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.get(m_eventInfo):0;
  //m_tree_vbf->prescaleHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = Props::prescaleHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split = Props::prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split = Props::prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF = Props::prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split = Props::prescaleHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF = Props::prescaleHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j80_0eta240_j60_j45_320eta490 = Props::prescaleHLT_j80_0eta240_j60_j45_320eta490.get(m_eventInfo);

  m_tree_vbf->prescaleL1_J40_0ETA25_2J15_31ETA49 = Props::prescaleL1_J40_0ETA25_2J15_31ETA49.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j80_bmv2c2085_split_2j60_320eta490 = Props::prescaleHLT_j80_bmv2c2085_split_2j60_320eta490.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j80_0eta240_2j60_320eta490 = Props::prescaleHLT_j80_0eta240_2j60_320eta490.get(m_eventInfo);

  m_tree_vbf->prescaleL1_4J20 = Props::prescaleL1_4J20.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j55_bmv2c2077_split_2j55 = Props::prescaleHLT_2j55_bmv2c2077_split_2j55.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j45_bmv2c2070_split_2j45 = Props::prescaleHLT_2j45_bmv2c2070_split_2j45.get(m_eventInfo);

  m_tree_vbf->prescaleL1_4J15 = Props::prescaleL1_4J15.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j35_bmv2c2070_split_2j35_L14J15 = Props::prescaleHLT_2j35_bmv2c2070_split_2j35_L14J15.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j35_bmv2c2060_split_2j35_L14J15 = Props::prescaleHLT_2j35_bmv2c2060_split_2j35_L14J15.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j35_bmv2c2050_split_2j35_L14J15 = Props::prescaleHLT_2j35_bmv2c2050_split_2j35_L14J15.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j75_bmv2c2060_split_3j75_L14J15 = Props::prescaleHLT_j75_bmv2c2060_split_3j75_L14J15.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_j75_bmv2c2070_split_3j75_L14J15 = Props::prescaleHLT_j75_bmv2c2070_split_3j75_L14J15.get(m_eventInfo);

  m_tree_vbf->prescaleHLT_2j45_bmv2c2077_split_2j45_L14J15 = Props::prescaleHLT_2j45_bmv2c2077_split_2j45_L14J15.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_2j45_bmv2c2070_split_2j45_L14J15 = Props::prescaleHLT_2j45_bmv2c2070_split_2j45_L14J15.get(m_eventInfo);
  m_tree_vbf->prescaleHLT_4j45 = Props::prescaleHLT_4j45.get(m_eventInfo);

  return EL::StatusCode::SUCCESS;

}



EL::StatusCode AnalysisReader_VBFHbb::save_jet_info(
						    std::vector<const xAOD::Jet*>  const& inputJets,
						    std::vector<const xAOD::Jet*>  const& signalJets,
						    std::vector<const xAOD::Jet*>  const& vbfJets,
						    std::vector<const xAOD::Jet*>  const& trackJets) {


  m_tree_vbf->nJets = inputJets.size();

  TLorentzVector b1Vec(signalJets.at(0)->p4()), b2Vec(signalJets.at(1)->p4());
  TLorentzVector j1Vec(vbfJets.at(0)->p4()), j2Vec(vbfJets.at(1)->p4());
  TLorentzVector j1VecRaw(vbfJets.at(0)->p4()), j2VecRaw(vbfJets.at(1)->p4());
  TLorentzVector b1Vec_no_corr(signalJets.at(0)->p4()), b2Vec_no_corr(signalJets.at(1)->p4());
  TLorentzVector b1Vec_onlymu(signalJets.at(0)->p4()), b2Vec_onlymu(signalJets.at(1)->p4());
  //j1Vec.SetPtEtaPhiM(vbfJets.at(0)->pt(),vbfJets.at(0)->eta(),vbfJets.at(0)->phi(),vbfJets.at(0)->m());
  //j2Vec.SetPtEtaPhiM(vbfJets.at(1)->pt(),vbfJets.at(1)->eta(),vbfJets.at(1)->phi(),vbfJets.at(1)->m());
  //j1Vec.SetPtEtaPhiM(vbfJets.at(0)->pt(),vbfJets.at(0)->eta()*1.8,vbfJets.at(0)->phi(),vbfJets.at(0)->m());
  //j2Vec.SetPtEtaPhiM(vbfJets.at(1)->pt(),vbfJets.at(1)->eta()*1.8,vbfJets.at(1)->phi(),vbfJets.at(1)->m());
  j1Vec.SetPtEtaPhiM(vbfJets.at(0)->pt(),vbfJets.at(0)->eta()*vbfJets.at(0)->auxdata<float>("randfactor"),vbfJets.at(0)->phi(),vbfJets.at(0)->m());
  j2Vec.SetPtEtaPhiM(vbfJets.at(1)->pt(),vbfJets.at(1)->eta()*vbfJets.at(1)->auxdata<float>("randfactor"),vbfJets.at(1)->phi(),vbfJets.at(1)->m());

  // apply b-jet energy correction to signal jets
  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/HSG5BJetEnergyCorr#Context
  
  if (m_jetCorrType != "NoCorr"){
    EL_CHECK("AnalysisReader_VBFHbb::Fill() ",getBJetEnergyCorrTLV(signalJets.at(0), b1Vec, false, m_jetCorrType));
    EL_CHECK("AnalysisReader_VBFHbb::Fill() ",getBJetEnergyCorrTLV(signalJets.at(1), b2Vec, false, m_jetCorrType));
    EL_CHECK("AnalysisReader_VBFHbb::Fill() ",getBJetEnergyCorrTLV(signalJets.at(0), b1Vec_onlymu, false, "OneMu"));
    EL_CHECK("AnalysisReader_VBFHbb::Fill() ",getBJetEnergyCorrTLV(signalJets.at(1), b2Vec_onlymu, false, "OneMu"));
  }
  TLorentzVector b1Vec_nom,b2Vec_nom;
  b1Vec_nom.SetPtEtaPhiM(b1Vec.Pt(),b1Vec.Eta(),b1Vec.Phi(),0);
  b2Vec_nom.SetPtEtaPhiM(b2Vec.Pt(),b2Vec.Eta(),b2Vec.Phi(),0);
  TLorentzVector b1Vec_trm,b2Vec_trm;
  b1Vec_trm.SetPtEtaPhiM(b1Vec.Pt(),b1Vec.Eta(),b1Vec.Phi(),signalJets.at(0)->auxdata<float>("TruthWZ_m")>0?signalJets.at(0)->auxdata<float>("TruthWZ_m"):b1Vec.M());
  b2Vec_trm.SetPtEtaPhiM(b2Vec.Pt(),b2Vec.Eta(),b2Vec.Phi(),signalJets.at(1)->auxdata<float>("TruthWZ_m")>0?signalJets.at(1)->auxdata<float>("TruthWZ_m"):b2Vec.M());

  // ------------------------
  // Collecting Jets' Info
  // ------------------------
  m_tree_vbf->nJets20pt = 0;
  m_tree_vbf->nJets20pt_central = 0;
  m_tree_vbf->nJets20pt_centraljvt = 0;
  m_tree_vbf->nJets20pt_rap = 0;
  m_tree_vbf->nJets30pt = 0;
  m_tree_vbf->nJets30pt_central = 0;
  m_tree_vbf->nJets30pt_rap = 0;
  m_tree_vbf->nJets_truth = 0;


  TLorentzVector embmu1,embmu2;
  embmu1.SetPtEtaPhiM(m_tree_vbf->embMu1pt,m_tree_vbf->embMu1eta,m_tree_vbf->embMu1phi,105.7);
  embmu2.SetPtEtaPhiM(m_tree_vbf->embMu2pt,m_tree_vbf->embMu2eta,m_tree_vbf->embMu2phi,105.7);

  //if (m_isMC && m_truthWZJets) for (const xAOD::Jet *jet : *m_truthWZJets) {
    //if (jet->pt()>30e3) m_tree_vbf->nJets_truth++;
    //if (jet->pt()>30e3) m_tree_vbf->nJets_truth++;
    //if (jet->auxdata<float>("TruthWZ_pt")>20e3) njettruth++;
  //}
  m_tree_vbf->nJets_truth = m_eventInfo->auxdata<int>("HTXS_Njets_pTjet30");
  //std::cout << " NJET " << m_tree_vbf->nJets_truth << " " << m_eventInfo->auxdata<int>("HTXS_Njets_pTjet30") << '\n';

  for (unsigned int iJet(0) ; iJet < inputJets.size(); ++iJet) {
    const xAOD::Jet * jet = inputJets.at(iJet);
    if (m_isMC) {
      //if (jet->auxdata<float>("TruthWZ_pt")>30e3) m_tree_vbf->nJets_truth++;
    }


    bool eventJet = false;
    if (jet==signalJets.at(0) || jet==signalJets.at(1) || jet==vbfJets.at(0) || jet==vbfJets.at(1)) {
      eventJet = true;
    }
    if (fabs(jet->eta())<2.5 || !Props::Timing.exists(jet) || (Props::PassFJvtLoose.get( jet ) && fabs(Props::Timing.get( jet ))<10.))  {
      if (jet->pt() > 20e3) m_tree_vbf->nJets20pt++;
      if (fabs(jet->eta())<2.5 && jet->pt() > 20e3 && !eventJet) m_tree_vbf->nJets20pt_central++;
      bool passtightjvt = true;
      if (jet->pt()>20e3 && jet->pt()<120e3 && fabs(jet->eta())<2.4 && Props::Jvt.get(jet)<0.59) passtightjvt = false;
      if (jet->pt()>20e3 && jet->pt()<120e3 && fabs(jet->eta())<2.5 && Props::Jvt.get(jet)<0.11) passtightjvt = false;
      if (fabs(jet->eta())<2.5 && jet->pt() > 20e3 && !eventJet && passtightjvt) m_tree_vbf->nJets20pt_centraljvt++;
      if ((jet->eta()-j1Vec.Eta())*(jet->eta()-j2Vec.Eta())<0 && jet->pt() > 20e3 && !eventJet) m_tree_vbf->nJets20pt_rap++;
      if (jet->pt() > 30e3) m_tree_vbf->nJets30pt++;
      if (fabs(jet->eta())<2.5 && jet->pt() > 30e3 && !eventJet) m_tree_vbf->nJets30pt_central++;
      if ((jet->eta()-j1Vec.Eta())*(jet->eta()-j2Vec.Eta())<0 && jet->pt() > 30e3 && !eventJet) m_tree_vbf->nJets30pt_rap++;
    }

    if (Props::MV2c10.get(jet)>m_b_85_weight) m_tree_vbf->nLooseMv2c10++;
    if (Props::MV2c10.get(jet)>m_b_77_weight) m_tree_vbf->nMediumMv2c10++;
    if (Props::MV2c10.get(jet)>m_b_70_weight) m_tree_vbf->nTightMv2c10++;

    if (jet->p4().DeltaR(embmu1)>0.4 && jet->p4().DeltaR(embmu2)>0.4) {
      if (Props::MV2c10.get(jet)>m_b_85_weight) m_tree_vbf->nLooseEmbMv2c10++;
      if (Props::MV2c10.get(jet)>m_b_77_weight) m_tree_vbf->nMediumEmbMv2c10++;
      if (Props::MV2c10.get(jet)>m_b_70_weight) m_tree_vbf->nTightEmbMv2c10++;
    }

    m_tree_vbf->pT.push_back( jet->pt() );
    m_tree_vbf->phi.push_back( jet->phi() );
    m_tree_vbf->eta.push_back( jet->eta() );
    m_tree_vbf->mv2c10.push_back( Props::MV2c10.get(jet) );
    m_tree_vbf->HLTJetMatchedPt.push_back( Props::HLTJetMatchedPt.exists(jet)?Props::HLTJetMatchedPt.get(jet):0 );
    m_tree_vbf->L1JetMatchedPt.push_back( Props::L1JetMatchedPt.exists(jet)?Props::L1JetMatchedPt.get(jet):0 );
    m_tree_vbf->L1JetMatchedEta.push_back( Props::L1JetMatchedEta.exists(jet)?Props::L1JetMatchedEta.get(jet):0 );
    m_tree_vbf->L1JetMatchedPhi.push_back( Props::L1JetMatchedPhi.exists(jet)?Props::L1JetMatchedPhi.get(jet):0 );
    m_tree_vbf->TruthJetMatchedPt.push_back( Props::TruthWZ_pt.exists(jet)?Props::TruthWZ_pt.get(jet):0 );
    m_tree_vbf->TruthJetMatchedEta.push_back( Props::TruthWZ_eta.exists(jet)?Props::TruthWZ_eta.get(jet):0 );

    int partonTruthLabelID = -999;
    if (m_isMC) partonTruthLabelID = Props::PartonTruthLabelID.get(jet);
    m_tree_vbf->partonTruthLabelID.push_back(partonTruthLabelID);
  }

  xAOD::TStore *store = xAOD::TActiveStore::store();
  const xAOD::TruthEventContainer *truthevents = NULL;
  if (m_isMC) {
    if (!m_event->retrieve(truthevents, "TruthEvents___Nominal").isSuccess()) {}
    const xAOD::TruthEvent *truthevent = truthevents->at(0);
    m_tree_vbf->TE_Q             = truthevent->auxdata<float>("Q");
    m_tree_vbf->TE_X1             = truthevent->auxdata<float>("X1");
    m_tree_vbf->TE_X2             = truthevent->auxdata<float>("X2");
    m_tree_vbf->TE_XF1             = truthevent->auxdata<float>("XF1");
    m_tree_vbf->TE_XF2             = truthevent->auxdata<float>("XF2");
  }
  
  m_tree_vbf->J1FJVTLoose = (Props::PassFJvtLoose.get( vbfJets.at(0) ) && fabs(!Props::Timing.exists(vbfJets.at(0))?0:Props::Timing.get( vbfJets.at(0) ))<10.);
  m_tree_vbf->J2FJVTLoose = (Props::PassFJvtLoose.get( vbfJets.at(1) ) && fabs(!Props::Timing.exists(vbfJets.at(1))?0:Props::Timing.get( vbfJets.at(1) ))<10.);
  m_tree_vbf->J1FJVTTight = (Props::PassFJvtTight.get( vbfJets.at(0) ) && fabs(!Props::Timing.exists(vbfJets.at(0))?0:Props::Timing.get( vbfJets.at(0) ))<10.);
  m_tree_vbf->J2FJVTTight = (Props::PassFJvtTight.get( vbfJets.at(1) ) && fabs(!Props::Timing.exists(vbfJets.at(1))?0:Props::Timing.get( vbfJets.at(1) ))<10.);
  m_tree_vbf->J1Timing = fabs(!Props::Timing.exists(vbfJets.at(0))?0:Props::Timing.get( vbfJets.at(0) ));
  m_tree_vbf->J2Timing = fabs(!Props::Timing.exists(vbfJets.at(1))?0:Props::Timing.get( vbfJets.at(1) ));
  m_tree_vbf->J1JVT = Props::Jvt.get( vbfJets.at(0) );
  m_tree_vbf->J2JVT = Props::Jvt.get( vbfJets.at(1) );
  m_tree_vbf->J1FJVT = Props::PassFJvtLoose.get( vbfJets.at(0) );
  m_tree_vbf->J2FJVT = Props::PassFJvtLoose.get( vbfJets.at(1) );

  // === Get Index and mindR of J1, J2, B1 and B2
  double mindRB1_Ex = 9999;
  double mindRB2_Ex = 9999;
  double mindRJ1_Ex = 9999;
  double mindRJ1_Ex_base = 9999;
  double mindRJ1_Ex_closest = 9999;
  double mindRJ1_Ex_dRB1 = 9999;
  double mindRJ1_ExPt = 0;
  double mindRJ1_ExPt_closest = 0;
  double mindRJ1_ExPt_dRB1 = 0;
  double mindRJ2_Ex = 9999;
  double mindRJ2_Ex_base = 9999;
  double mindRJ2_Ex_closest = 9999;
  double mindRJ2_Ex_dRB1 = 9999;
  double mindRJ2_ExPt = 0;
  double mindRJ2_ExPt_closest = 0;
  double mindRJ2_ExPt_dRB1 = 0;

  double dRB1J = 9999;
  double dRB2J = 9999;
  double dRJ1J = 9999;
  double dRJ2J = 9999;
  double dRJ1JRaw = 9999;
  double dRJ2JRaw = 9999;
  TLorentzVector tlv5,tlv_ex10,tlv_ex12,tlv_ex14,tlv_ex16,tlv_ex18,tlv_ex20,tlv_exmin08,tlv_exmin10,tlv_exmin12;
  float tlvsum_exmin08=0;
  float tlvsum_exmin10=0;
  float tlvsum_exmin12=0;
  float tlvsum_ex10=0;
  float tlvsum_ex12=0;
  float tlvsum_ex14=0;
  float tlvsum_ex16=0;
  float tlvsum_ex18=0;
  float tlvsum_ex20=0;
  int tlvnum_exmin08=0;
  int tlvnum_exmin10=0;
  int tlvnum_exmin12=0;
  int tlvnum_ex10=0;
  int tlvnum_ex12=0;
  int tlvnum_ex14=0;
  int tlvnum_ex16=0;
  int tlvnum_ex18=0;
  int tlvnum_ex20=0;
  int njets_ex = 0;

  TLorentzVector j1VecJ = j1Vec;
  TLorentzVector j2VecJ = j2Vec;
  TLorentzVector b1VecJ = b1Vec;
  TLorentzVector b2VecJ = b2Vec;
  for (unsigned int iJet(0); iJet < inputJets.size(); iJet++) {
    if (fabs(inputJets[iJet]->eta())>2.5 && Props::Timing.exists(inputJets[iJet]) && (!Props::PassFJvtLoose.get( inputJets[iJet] ) || fabs(Props::Timing.get( inputJets[iJet] ))>10.)) continue;
    TLorentzVector tmpVect( inputJets.at(iJet)->p4() );
    dRJ1J = tmpVect.DeltaR(j1Vec);
    dRJ2J = tmpVect.DeltaR(j2Vec);
    dRJ1JRaw = tmpVect.DeltaR(j1VecRaw);
    dRJ2JRaw = tmpVect.DeltaR(j2VecRaw);
    dRB1J = tmpVect.DeltaR(b1Vec_no_corr);
    dRB2J = tmpVect.DeltaR(b2Vec_no_corr);

    bool eventJet = false;
    if (inputJets.at(iJet)==signalJets[0] || inputJets.at(iJet)==signalJets[1] || inputJets.at(iJet)==vbfJets[0] || inputJets.at(iJet)==vbfJets[1]){
      eventJet = true;
    }

    if (!eventJet && dRB1J<1. && dRB1J<dRB2J && dRB1J<dRJ1J && dRB1J<dRJ2J) b1VecJ += tmpVect;
    if (!eventJet && dRB2J<dRB1J && dRB2J<1. && dRB2J<dRJ1J && dRB2J<dRJ2J) b2VecJ += tmpVect;
    if (!eventJet && dRJ1J<dRB1J && dRJ1J<dRB2J && dRJ1J<1. && dRJ1J<dRJ2J) j1VecJ += tmpVect;
    if (!eventJet && dRJ2J<dRB1J && dRJ2J<dRB2J && dRJ2J<dRJ1J && dRJ2J<1.) j2VecJ += tmpVect;

    if (!eventJet && dRJ1J<dRB1J && dRJ1J<dRB2J && dRJ1J<mindRJ1_Ex_closest && dRJ1J<dRJ2J) {mindRJ1_Ex_closest = dRJ1J; mindRJ1_ExPt_closest = tmpVect.Pt();}
    if (!eventJet && dRJ2J<dRB1J && dRJ2J<dRB2J && dRJ2J<dRJ1J && dRJ2J<mindRJ2_Ex_closest) {mindRJ2_Ex_closest = dRJ2J; mindRJ2_ExPt_closest = tmpVect.Pt();}
    if (!eventJet && dRB1J>1. && dRB2J>1. && dRJ1J<mindRJ1_Ex_dRB1) {mindRJ1_Ex_dRB1 = dRJ1J; mindRJ1_ExPt_dRB1 = tmpVect.Pt();}
    if (!eventJet && dRB1J>1. && dRB2J>1. && dRJ2J<mindRJ2_Ex_dRB1) {mindRJ2_Ex_dRB1 = dRJ2J; mindRJ2_ExPt_dRB1 = tmpVect.Pt();}
    if (!eventJet && dRJ1J<mindRJ1_Ex) {mindRJ1_Ex = dRJ1J; mindRJ1_ExPt = tmpVect.Pt();}
    if (!eventJet && dRJ2J<mindRJ2_Ex) {mindRJ2_Ex = dRJ2J; mindRJ2_ExPt = tmpVect.Pt();}
    if (!eventJet && dRJ1JRaw<mindRJ1_Ex_base) {mindRJ1_Ex_base = dRJ1JRaw;}
    if (!eventJet && dRJ2JRaw<mindRJ2_Ex_base) {mindRJ2_Ex_base = dRJ2JRaw;}

    if (!eventJet) njets_ex++;
    if (!eventJet) tlv5 = tmpVect;
    if (!eventJet && std::min(b1Vec_no_corr.DeltaR(tmpVect),b2Vec_no_corr.DeltaR(tmpVect))<0.8) tlv_exmin08 += tmpVect; 
    if (!eventJet && std::min(b1Vec_no_corr.DeltaR(tmpVect),b2Vec_no_corr.DeltaR(tmpVect))<1.0) tlv_exmin10 += tmpVect; 
    if (!eventJet && std::min(b1Vec_no_corr.DeltaR(tmpVect),b2Vec_no_corr.DeltaR(tmpVect))<1.2) tlv_exmin12 += tmpVect; 
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.0) tlv_ex10 += tmpVect; 
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.2) tlv_ex12 += tmpVect; 
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.4) tlv_ex14 += tmpVect; 
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.6) tlv_ex16 += tmpVect; 
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.8) tlv_ex18 += tmpVect; 
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<2.0) tlv_ex20 += tmpVect; 
    if (!eventJet && std::min(b1Vec_no_corr.DeltaR(tmpVect),b2Vec_no_corr.DeltaR(tmpVect))<0.8) tlvsum_exmin08 += tmpVect.Pt(); 
    if (!eventJet && std::min(b1Vec_no_corr.DeltaR(tmpVect),b2Vec_no_corr.DeltaR(tmpVect))<1.0) tlvsum_exmin10 += tmpVect.Pt(); 
    if (!eventJet && std::min(b1Vec_no_corr.DeltaR(tmpVect),b2Vec_no_corr.DeltaR(tmpVect))<1.2) tlvsum_exmin12 += tmpVect.Pt(); 
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.0) tlvsum_ex10 += tmpVect.Pt(); 
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.2) tlvsum_ex12 += tmpVect.Pt(); 
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.4) tlvsum_ex14 += tmpVect.Pt(); 
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.6) tlvsum_ex16 += tmpVect.Pt(); 
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.8) tlvsum_ex18 += tmpVect.Pt(); 
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<2.0) tlvsum_ex20 += tmpVect.Pt(); 
    if (!eventJet && std::min(b1Vec_no_corr.DeltaR(tmpVect),b2Vec_no_corr.DeltaR(tmpVect))<0.8) tlvnum_exmin08 ++; 
    if (!eventJet && std::min(b1Vec_no_corr.DeltaR(tmpVect),b2Vec_no_corr.DeltaR(tmpVect))<1.0) tlvnum_exmin10 ++;
    if (!eventJet && std::min(b1Vec_no_corr.DeltaR(tmpVect),b2Vec_no_corr.DeltaR(tmpVect))<1.2) tlvnum_exmin12 ++;
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.0) tlvnum_ex10++;
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.2) tlvnum_ex12++;
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.4) tlvnum_ex14++;
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.6) tlvnum_ex16++;
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<1.8) tlvnum_ex18++;
    if (!eventJet && b1Vec_no_corr.DeltaR(tmpVect)+b2Vec_no_corr.DeltaR(tmpVect)<2.0) tlvnum_ex20++;

    if (tmpVect == j1Vec) { m_tree_vbf->whoIsJ1 = iJet;}
    if (tmpVect == j2Vec) { m_tree_vbf->whoIsJ2 = iJet;}

    if (tmpVect == b1Vec_no_corr) { m_tree_vbf->whoIsB1 = iJet;}
    else { 
      if(not eventJet and dRB1J<mindRB1_Ex) mindRB1_Ex = dRB1J; 
    }

    if (tmpVect == b2Vec_no_corr) { m_tree_vbf->whoIsB2 = iJet;}
    else {
      if(not eventJet and dRB2J<mindRB2_Ex) mindRB2_Ex = dRB2J; 
    }
  }
  m_tree_vbf->nJets20pt_fsrmin08 = m_tree_vbf->nJets20pt-tlvnum_exmin08;
  m_tree_vbf->nJets20pt_fsrmin10 = m_tree_vbf->nJets20pt-tlvnum_exmin10;
  m_tree_vbf->nJets20pt_fsrmin12 = m_tree_vbf->nJets20pt-tlvnum_exmin12;
  m_tree_vbf->nJets20pt_fsr10 = m_tree_vbf->nJets20pt-tlvnum_ex10;
  m_tree_vbf->nJets20pt_fsr12 = m_tree_vbf->nJets20pt-tlvnum_ex12;
  m_tree_vbf->nJets20pt_fsr14 = m_tree_vbf->nJets20pt-tlvnum_ex14;
  m_tree_vbf->nJets20pt_fsr16 = m_tree_vbf->nJets20pt-tlvnum_ex16;
  m_tree_vbf->nJets20pt_fsr18 = m_tree_vbf->nJets20pt-tlvnum_ex18;
  m_tree_vbf->nJets20pt_fsr20 = m_tree_vbf->nJets20pt-tlvnum_ex20;

  m_tree_vbf->mBB    =  (b1Vec + b2Vec).M();
  m_tree_vbf->mBB_nom    =  (b1Vec_nom + b2Vec_nom).M();
  m_tree_vbf->mBB_trm    =  (b1Vec_trm + b2Vec_trm).M();
  m_tree_vbf->blind    =  ((b1Vec + b2Vec).M()<100e3 || (b1Vec + b2Vec).M()>140e3)?1:0;
  m_tree_vbf->dRBB   =  b1Vec.DeltaR(b2Vec);
  m_tree_vbf->dPhiBB =  fabs(b1Vec.DeltaPhi(b2Vec));
  m_tree_vbf->dEtaBB =  fabs(b1Vec.Eta() - b2Vec.Eta());
  m_tree_vbf->pTBB   =  (b1Vec + b2Vec).Pt();
  m_tree_vbf->EtaBB   =  (b1Vec + b2Vec).Eta();
  m_tree_vbf->PhiBB   =  (b1Vec + b2Vec).Phi();

  m_tree_vbf->mBB_no_corr    =  (b1Vec_no_corr + b2Vec_no_corr).M();
  m_tree_vbf->mBB_onlymu    =  (b1Vec_onlymu + b2Vec_onlymu).M();
  TLorentzVector b1_truth,b2_truth;
  if (Props::TruthWZ_pt.exists(signalJets.at(0)) && Props::TruthWZ_pt.get(signalJets.at(0))>0) b1_truth.SetPtEtaPhiM( Props::TruthWZ_pt.get(signalJets.at(0)),Props::TruthWZ_eta.get(signalJets.at(0)),Props::TruthWZ_phi.get(signalJets.at(0)),Props::TruthWZ_m.get(signalJets.at(0)));
  if (Props::TruthWZ_pt.exists(signalJets.at(1)) && Props::TruthWZ_pt.get(signalJets.at(1))>0) b2_truth.SetPtEtaPhiM( Props::TruthWZ_pt.get(signalJets.at(1)),Props::TruthWZ_eta.get(signalJets.at(1)),Props::TruthWZ_phi.get(signalJets.at(1)),Props::TruthWZ_m.get(signalJets.at(1)));
  m_tree_vbf->mBB_truth    =  (b1_truth+b2_truth).M();
  m_tree_vbf->mBB_fsrmin08    =  (b1Vec + b2Vec+tlv_exmin08).M();
  m_tree_vbf->mBB_fsrmin10    =  (b1Vec + b2Vec+tlv_exmin10).M();
  m_tree_vbf->mBB_fsrmin12    =  (b1Vec + b2Vec+tlv_exmin12).M();
  m_tree_vbf->mBB_fsr10    =  (b1Vec + b2Vec+tlv_ex10).M();
  m_tree_vbf->mBB_fsr12    =  (b1Vec + b2Vec+tlv_ex12).M();
  m_tree_vbf->mBB_fsr14    =  (b1Vec + b2Vec+tlv_ex14).M();
  m_tree_vbf->mBB_fsr16    =  (b1Vec + b2Vec+tlv_ex16).M();
  m_tree_vbf->mBB_fsr18    =  (b1Vec + b2Vec+tlv_ex18).M();
  m_tree_vbf->mBB_fsr20    =  (b1Vec + b2Vec+tlv_ex20).M();
  m_tree_vbf->dRBB_no_corr   =  b1Vec_no_corr.DeltaR(b2Vec_no_corr);
  m_tree_vbf->dRBB_onlymu   =  b1Vec_onlymu.DeltaR(b2Vec_onlymu);
  m_tree_vbf->pTBB_no_corr   =  (b1Vec_no_corr + b2Vec_no_corr).Pt();
  m_tree_vbf->pTBB_onlymu   =  (b1Vec_onlymu + b2Vec_onlymu).Pt();
  m_tree_vbf->pTBB_fsrmin08    =  (b1Vec + b2Vec+tlv_exmin08).Pt();
  m_tree_vbf->pTBB_fsrmin10    =  (b1Vec + b2Vec+tlv_exmin10).Pt();
  m_tree_vbf->pTBB_fsrmin12    =  (b1Vec + b2Vec+tlv_exmin12).Pt();
  m_tree_vbf->pTBB_fsr10    =  (b1Vec + b2Vec+tlv_ex10).Pt();
  m_tree_vbf->pTBB_fsr12    =  (b1Vec + b2Vec+tlv_ex12).Pt();
  m_tree_vbf->pTBB_fsr14    =  (b1Vec + b2Vec+tlv_ex14).Pt();
  m_tree_vbf->pTBB_fsr16    =  (b1Vec + b2Vec+tlv_ex16).Pt();
  m_tree_vbf->pTBB_fsr18    =  (b1Vec + b2Vec+tlv_ex18).Pt();
  m_tree_vbf->pTBB_fsr20    =  (b1Vec + b2Vec+tlv_ex20).Pt();
  m_tree_vbf->EtaBB_no_corr   =  (b1Vec_no_corr + b2Vec_no_corr).Eta();
  m_tree_vbf->EtaBB_onlymu   =  (b1Vec_onlymu + b2Vec_onlymu).Eta();
  m_tree_vbf->PhiBB_no_corr   =  (b1Vec_no_corr + b2Vec_no_corr).Phi();
  m_tree_vbf->PhiBB_onlymu   =  (b1Vec_onlymu + b2Vec_onlymu).Phi();

  m_tree_vbf->mJJ     =  (j1Vec + j2Vec).M();
  //m_tree_vbf->mJJ_base     =  (j1VecRaw + j2VecRaw).M();
  if (m_eventInfo->isAvailable<float>("mJJ_base")) m_tree_vbf->mJJ_base = m_eventInfo->auxdata<float>("mJJ_base");
  if (m_isMC) {
    TLorentzVector tlv_j1truth,tlv_j2truth;
    tlv_j1truth.SetPtEtaPhiM(vbfJets.at(0)->auxdata<float>("TruthWZ_pt"),vbfJets.at(0)->auxdata<float>("TruthWZ_eta"),vbfJets.at(0)->auxdata<float>("TruthWZ_phi"),vbfJets.at(0)->auxdata<float>("TruthWZ_m"));
    tlv_j2truth.SetPtEtaPhiM(vbfJets.at(1)->auxdata<float>("TruthWZ_pt"),vbfJets.at(1)->auxdata<float>("TruthWZ_eta"),vbfJets.at(1)->auxdata<float>("TruthWZ_phi"),vbfJets.at(1)->auxdata<float>("TruthWZ_m"));
    m_tree_vbf->mJJ_truth = m_eventInfo->auxdata<float>("HTXS_MJJ_jets30");
    std::vector<float> tjetpt = m_eventInfo->auxdata<std::vector<float> >("HTXS_V_jets25_pt");
    std::vector<float> tjeteta = m_eventInfo->auxdata<std::vector<float> >("HTXS_V_jets25_eta");
    std::vector<float> tjetphi = m_eventInfo->auxdata<std::vector<float> >("HTXS_V_jets25_phi");
    std::vector<float> tjetm = m_eventInfo->auxdata<std::vector<float> >("HTXS_V_jets25_m");
    float maxor = 1e9;
    m_tree_vbf->mJJ_truth_new = 0;
    m_tree_vbf->pTHJJ_truth_new = 0;
    float newpthjj = 0;
    TLorentzVector tlvh;
    tlvh.SetPtEtaPhiM(Props::hpt.get(m_eventInfo),Props::heta.get(m_eventInfo),Props::hphi.get(m_eventInfo),Props::hm.get(m_eventInfo));
    for (size_t i = 0; i < tjetpt.size(); i++) for (size_t j = 0; j < i; j++) {
      if (tjetpt[i]<30e3) continue;
      if (tjetpt[j]<30e3) continue;
      TLorentzVector tlv1,tlv2;
      tlv1.SetPtEtaPhiM(tjetpt[i],tjeteta[i],tjetphi[i],tjetm[i]);
      tlv2.SetPtEtaPhiM(tjetpt[j],tjeteta[j],tjetphi[j],tjetm[j]);
      //if (tjetpt[i]+tjetpt[j]<maxor) continue;
      //maxor = tjetpt[i]+tjetpt[j];
      if (fabs((tlv1+tlv2).M()-m_tree_vbf->mJJ_truth)>maxor) continue;
      maxor = fabs((tlv1+tlv2).M()-m_tree_vbf->mJJ_truth);
      m_tree_vbf->mJJ_truth_new = (tlv1+tlv2).M();
      m_tree_vbf->pTHJJ_truth_new = (tlv1+tlv2+tlvh).Pt();
    }
    /*std::cout << "NEWEV\n";
    if (fabs(m_tree_vbf->mJJ_truth-m_tree_vbf->mJJ_truth_new)>1e-3) {
      for (size_t i = 0; i < tjetpt.size(); i++) for (size_t j = 0; j < i; j++) {
        if (tjetpt[i]<30e3) continue;
        if (tjetpt[j]<30e3) continue;
        //if (tjetpt[i]+tjetpt[j]<maxor) continue;
        TLorentzVector tlv1,tlv2;
        tlv1.SetPtEtaPhiM(tjetpt[i],tjeteta[i],tjetphi[i],tjetm[i]);
        tlv2.SetPtEtaPhiM(tjetpt[j],tjeteta[j],tjetphi[j],tjetm[j]);
        std::cout << "   CORN " << tjetpt[i]+tjetpt[j] << " " << tjetpt[i] << " " << tjetpt[j] << " " << m_tree_vbf->mJJ_truth << " " << m_tree_vbf->mJJ_truth_new << " " << (tlv1+tlv2).M() << " " << m_eventInfo->auxdata<int>("HTXS_Njets_pTjet30") << '\n';
        //m_tree_vbf->pTHJJ_truth_new = (tlv1+tlv2+tlvh).Pt();
      }
    }*/


    //m_tree_vbf->mJJ_truth = (tlv_j1truth+tlv_j2truth).M();
    //std::cout << "TEST " << m_tree_vbf->mJJ_truth << " " << m_eventInfo->auxdata<float>("HTXS_MJJ_jets30") << " " << m_tree_vbf->mJJ << '\n';
    m_tree_vbf->dEtaJJ_truth = fabs(tlv_j1truth.Eta()-tlv_j2truth.Eta());

    m_tree_vbf->mJJ_truth_abs = (tlv_j1truth+tlv_j2truth).M();
    m_tree_vbf->dEtaJJ_truth_abs = fabs(tlv_j1truth.Eta()-tlv_j2truth.Eta());
  }
  m_tree_vbf->etaJ1_base     =  (j1VecRaw).Eta();
  m_tree_vbf->etaJ2_base     =  (j2VecRaw).Eta();
  m_tree_vbf->mJJJ     =  (j1VecJ + j2VecJ).M();
  m_tree_vbf->deltaMJJ   = std::max(double(m_tree_vbf->mJJ),std::max((j1Vec + b1Vec).M(),(j1Vec + b2Vec).M()))-m_tree_vbf->mJJ;
  m_tree_vbf->deltaMJJ2   = std::max(double(m_tree_vbf->mJJ),std::max((j2Vec + b1Vec).M(),(j2Vec + b2Vec).M()))-m_tree_vbf->mJJ;
  m_tree_vbf->deltaMJJJ   = std::max(double(m_tree_vbf->mJJJ),std::max((j1VecJ + b1VecJ).M(),(j1VecJ + b2VecJ).M()))-m_tree_vbf->mJJJ;
  TLorentzVector bb_nomBB;
  bb_nomBB.SetPtEtaPhiM((b1Vec+b2Vec).Pt(),(b1Vec+b2Vec).Eta(),(b1Vec+b2Vec).Phi(),0);
  m_tree_vbf->deltaMJJ_pTBB_nomBB   = std::max(double(m_tree_vbf->mJJ),(j1Vec + bb_nomBB).M())-m_tree_vbf->mJJ;
  m_tree_vbf->deltaMJJ_pTBB   = std::max(double(m_tree_vbf->mJJ),(j1Vec + b1Vec + b2Vec).M())-m_tree_vbf->mJJ;

  m_tree_vbf->dRJJ   =  j1Vec.DeltaR(j2Vec);
  m_tree_vbf->dPhiJJ =  fabs(j1Vec.DeltaPhi(j2Vec));
  m_tree_vbf->dPhiBBJJ =  fabs((j1Vec+j2Vec).DeltaPhi(b1Vec+b2Vec));
  m_tree_vbf->dEtaJJ =  fabs(j1Vec.Eta() - j2Vec.Eta());
  m_tree_vbf->dEtaBBJJ =  fabs((j1Vec+j2Vec).Eta()-(b1Vec+b2Vec).Eta());
  m_tree_vbf->EtaJJ =  (j1Vec+j2Vec).Eta();
  m_tree_vbf->PhiJJ =  (j1Vec+j2Vec).Phi();
  m_tree_vbf->pTJJ   =  (j1Vec + j2Vec).Pt();
  m_tree_vbf->asymJJ   =  (j1Vec.Pt()-j2Vec.Pt())/(j1Vec.Pt()+j2Vec.Pt());
  m_tree_vbf->angleAsymBB   =  (m_tree_vbf->dEtaBB-m_tree_vbf->dPhiBB)/(m_tree_vbf->dEtaBB+m_tree_vbf->dPhiBB);
  m_tree_vbf->mBB_angle   =  atan(tan(m_tree_vbf->dPhiBB/2.)/tanh(m_tree_vbf->dEtaBB/2.));
  m_tree_vbf->pTJJJ   =  (j1VecJ + j2VecJ).Pt();

  m_tree_vbf->mindRB1_Ex    =  mindRB1_Ex;
  m_tree_vbf->mindRB2_Ex    =  mindRB2_Ex;
  m_tree_vbf->mindRJ1_Ex    =  mindRJ1_Ex;
  m_tree_vbf->mindRJ1_Ex_base    =  mindRJ1_Ex_base;
  m_tree_vbf->mindRJ1_Ex_closest    =  mindRJ1_Ex_closest;
  m_tree_vbf->mindRJ1_Ex_dRB1    =  mindRJ1_Ex_dRB1;
  m_tree_vbf->mindRJ1_ExPt    =  mindRJ1_ExPt;
  m_tree_vbf->mindRJ1_ExPt_closest    =  mindRJ1_ExPt_closest;
  m_tree_vbf->mindRJ1_ExPt_dRB1    =  mindRJ1_ExPt_dRB1;
  m_tree_vbf->mindRJ2_Ex    =  mindRJ2_Ex;
  m_tree_vbf->mindRJ2_Ex_base    =  mindRJ2_Ex_base;
  m_tree_vbf->mindRJ2_Ex_closest    =  mindRJ2_Ex_closest;
  m_tree_vbf->mindRJ2_Ex_dRB1    =  mindRJ2_Ex_dRB1;
  m_tree_vbf->mindRJ2_ExPt    =  mindRJ2_ExPt;
  m_tree_vbf->mindRJ2_ExPt_closest    =  mindRJ2_ExPt_closest;
  m_tree_vbf->mindRJ2_ExPt_dRB1    =  mindRJ2_ExPt_dRB1;


  // ======

  m_tree_vbf->pTJ1 = j1Vec.Pt();
  m_tree_vbf->mJ1 = j1Vec.M();
  m_tree_vbf->pTJ1J = j1VecJ.Pt();
  m_tree_vbf->pTJ2 = j2Vec.Pt();
  m_tree_vbf->mJ2 = j2Vec.M();
  m_tree_vbf->pTJ2J = j2VecJ.Pt();
  m_tree_vbf->centralSFJ1 = 1;
  m_tree_vbf->centralSFJ2 = 1;
  m_tree_vbf->centralSF = 1;
  m_tree_vbf->forwardSFJ1 = 1;
  m_tree_vbf->forwardSFJ2 = 1;
  m_tree_vbf->forwardSF = 1;
  m_tree_vbf->B2SF = 1;
  UInt_t runNumber = (m_isMC?m_eventInfo->auxdata<UInt_t>("RandomRunNumber"):m_eventInfo->runNumber());

  if (!Props::embZm.exists(m_eventInfo) && fabs(j1VecRaw.Eta())>3.2 && fabs(j1VecRaw.Eta())<4.5 && j1VecRaw.Pt()>60e3 && j1VecRaw.Pt()<100e3) {
    if (runNumber<=311481) m_tree_vbf->forwardSFJ1 = reducedSF(m_forwardSF2016,fabs(j1VecRaw.Eta()),j1VecRaw.Pt()/1e3);
    else if (runNumber<=340453) m_tree_vbf->forwardSFJ1 = reducedSF(m_forwardSF2017,fabs(j1VecRaw.Eta()),j1VecRaw.Pt()/1e3);
    else m_tree_vbf->forwardSFJ1 = reducedSF(m_forwardSF2018,fabs(j1VecRaw.Eta()),j1VecRaw.Pt()/1e3);
  }
  if (!Props::embZm.exists(m_eventInfo) && fabs(j2VecRaw.Eta())>3.2 && fabs(j2VecRaw.Eta())<4.5 && j2VecRaw.Pt()>60e3 && j2VecRaw.Pt()<100e3) {
    if (runNumber<=311481) m_tree_vbf->forwardSFJ2 = reducedSF(m_forwardSF2016,fabs(j2VecRaw.Eta()),j2VecRaw.Pt()/1e3);
    else if (runNumber<=340453) m_tree_vbf->forwardSFJ2 = reducedSF(m_forwardSF2017,fabs(j2VecRaw.Eta()),j2VecRaw.Pt()/1e3);
    else m_tree_vbf->forwardSFJ2 = reducedSF(m_forwardSF2018,fabs(j2VecRaw.Eta()),j2VecRaw.Pt()/1e3);
  }
  m_tree_vbf->forwardSF = m_tree_vbf->forwardSFJ1;
  if (!Props::embZm.exists(m_eventInfo) && fabs(j2VecRaw.Eta())>3.2 && fabs(j2VecRaw.Eta())<4.5 && j2VecRaw.Pt()>60e3) {
    m_tree_vbf->forwardSF = 1;
  }
  if (!Props::embZm.exists(m_eventInfo) && fabs(j1VecRaw.Eta())<3.1 && j1VecRaw.Pt()>160e3 && j1VecRaw.Pt()<200e3) {
    if (runNumber<=311481) m_tree_vbf->centralSFJ1 = reducedSF(m_centralSF2016,fabs(j1VecRaw.Eta()),j1VecRaw.Pt()/1e3);
    else if (runNumber<=340453) m_tree_vbf->centralSFJ1 = reducedSF(m_centralSF2017,fabs(j1VecRaw.Eta()),j1VecRaw.Pt()/1e3);
    else m_tree_vbf->centralSFJ1 = reducedSF(m_centralSF2018,fabs(j1VecRaw.Eta()),j1VecRaw.Pt()/1e3);
  }
  if (!Props::embZm.exists(m_eventInfo) && fabs(j2VecRaw.Eta())<3.1 && j2VecRaw.Pt()>160e3 && j2VecRaw.Pt()<200e3) {
    if (runNumber<=311481) m_tree_vbf->centralSFJ2 = reducedSF(m_centralSF2016,fabs(j2VecRaw.Eta()),j2VecRaw.Pt()/1e3);
    else if (runNumber<=340453) m_tree_vbf->centralSFJ2 = reducedSF(m_centralSF2017,fabs(j2VecRaw.Eta()),j2VecRaw.Pt()/1e3);
    else m_tree_vbf->centralSFJ2 = reducedSF(m_centralSF2018,fabs(j2VecRaw.Eta()),j2VecRaw.Pt()/1e3);
  }
  m_tree_vbf->centralSF = m_tree_vbf->centralSFJ1;
  if (!Props::embZm.exists(m_eventInfo) && fabs(j2VecRaw.Eta())<3.1 && j2VecRaw.Pt()>160e3) {
    m_tree_vbf->centralSF = 1;
  }

//////
  if (Props::embZm.exists(m_eventInfo) && b2Vec_no_corr.Pt()<120e3 && (m_selection == "1forward2central" || m_selection == "fake")) {
    if (runNumber<=311481) m_tree_vbf->B2SF = m_B2_1for2cenEff2016->GetBinContent(m_B2_1for2cenEff2016->FindBin(b2Vec_no_corr.Pt()));
    else if (runNumber<=340453) m_tree_vbf->B2SF = m_B2_1for2cenEff2017->GetBinContent(m_B2_1for2cenEff2017->FindBin(b2Vec_no_corr.Pt()));
    else m_tree_vbf->B2SF = m_B2_1for2cenEff2018->GetBinContent(m_B2_1for2cenEff2018->FindBin(b2Vec_no_corr.Pt()));
  }

  if (Props::embZm.exists(m_eventInfo) && b2Vec_no_corr.Pt()<120e3 && (m_selection == "3central" || m_selection == "fake3")) {
    if (runNumber<=311481) m_tree_vbf->B2SF = m_B2_3cenEff2016->GetBinContent(m_B2_3cenEff2016->FindBin(b2Vec_no_corr.Pt()));
    else if (runNumber<=340453) m_tree_vbf->B2SF = m_B2_3cenEff2017->GetBinContent(m_B2_3cenEff2017->FindBin(b2Vec_no_corr.Pt()));
    else m_tree_vbf->B2SF = m_B2_3cenEff2018->GetBinContent(m_B2_3cenEff2018->FindBin(b2Vec_no_corr.Pt()));
  }


  m_tree_vbf->pTB1 = b1Vec.Pt();
  m_tree_vbf->mB1 = b1Vec.M();
  m_tree_vbf->pTB1J = b1VecJ.Pt();
  m_tree_vbf->pTB2 = b2Vec.Pt();
  m_tree_vbf->mB2 = b2Vec.M();
  m_tree_vbf->pTB2J = b2VecJ.Pt();
  m_tree_vbf->pTB1_no_corr = b1Vec_no_corr.Pt();
  m_tree_vbf->pTB2_no_corr = b2Vec_no_corr.Pt();
  m_tree_vbf->pTB1_onlymu = b1Vec_onlymu.Pt();
  m_tree_vbf->pTB2_onlymu = b2Vec_onlymu.Pt();
  m_tree_vbf->HLTB1 = ( Props::HLTJetMatchedPt.exists(signalJets.at(0))?Props::HLTJetMatchedPt.get(signalJets.at(0)):0 );
  m_tree_vbf->HLTB2 = ( Props::HLTJetMatchedPt.exists(signalJets.at(1))?Props::HLTJetMatchedPt.get(signalJets.at(1)):0 );
  m_tree_vbf->HLTJ1 = ( Props::HLTJetMatchedPt.exists(vbfJets.at(0))?Props::HLTJetMatchedPt.get(vbfJets.at(0)):0 );
  m_tree_vbf->HLTJ2 = ( Props::HLTJetMatchedPt.exists(vbfJets.at(1))?Props::HLTJetMatchedPt.get(vbfJets.at(1)):0 );
  m_tree_vbf->L1B1 = ( Props::L1JetMatchedPt.exists(signalJets.at(0))?Props::L1JetMatchedPt.get(signalJets.at(0)):0 );
  m_tree_vbf->L1B2 = ( Props::L1JetMatchedPt.exists(signalJets.at(1))?Props::L1JetMatchedPt.get(signalJets.at(1)):0 );
  m_tree_vbf->L1J1 = ( Props::L1JetMatchedPt.exists(vbfJets.at(0))?Props::L1JetMatchedPt.get(vbfJets.at(0)):0 );
  m_tree_vbf->L1J2 = ( Props::L1JetMatchedPt.exists(vbfJets.at(1))?Props::L1JetMatchedPt.get(vbfJets.at(1)):0 );

  m_tree_vbf->etaJ1 = j1Vec.Eta();
  m_tree_vbf->etaJ2 = j2Vec.Eta();
  m_tree_vbf->etaB1 = b1Vec.Eta();
  m_tree_vbf->etaB2 = b2Vec.Eta();
  m_tree_vbf->etaB1_no_corr = b1Vec_no_corr.Eta();
  m_tree_vbf->etaB2_no_corr = b2Vec_no_corr.Eta();
  m_tree_vbf->etaB1_onlymu = b1Vec_onlymu.Eta();
  m_tree_vbf->etaB2_onlymu = b2Vec_onlymu.Eta();

  m_tree_vbf->dRB1J1 = b1Vec.DeltaR(j1Vec);
  m_tree_vbf->dRB1J2 = b1Vec.DeltaR(j2Vec);
  m_tree_vbf->dRB2J1 = b2Vec.DeltaR(j1Vec);
  m_tree_vbf->dRB2J2 = b2Vec.DeltaR(j2Vec);
  m_tree_vbf->dRMin   = m_tree_vbf->dRJJ;
  if (m_tree_vbf->dRBB<m_tree_vbf->dRMin)  m_tree_vbf->dRMin = m_tree_vbf->dRBB;
  if (m_tree_vbf->dRB1J1<m_tree_vbf->dRMin)  m_tree_vbf->dRMin = m_tree_vbf->dRB1J1;
  if (m_tree_vbf->dRB1J2<m_tree_vbf->dRMin)  m_tree_vbf->dRMin = m_tree_vbf->dRB1J2;
  if (m_tree_vbf->dRB2J1<m_tree_vbf->dRMin)  m_tree_vbf->dRMin = m_tree_vbf->dRB2J1;
  if (m_tree_vbf->dRB2J2<m_tree_vbf->dRMin)  m_tree_vbf->dRMin = m_tree_vbf->dRB2J2;
  m_tree_vbf->pTMin   = m_tree_vbf->pTB1;
  if (m_tree_vbf->pTB2<m_tree_vbf->pTMin)  m_tree_vbf->pTMin = m_tree_vbf->pTB2;
  if (m_tree_vbf->pTJ1<m_tree_vbf->pTMin)  m_tree_vbf->pTMin = m_tree_vbf->pTJ1;
  if (m_tree_vbf->pTJ2<m_tree_vbf->pTMin)  m_tree_vbf->pTMin = m_tree_vbf->pTJ2;

  if (m_isMC) {
    //m_tree_vbf->TruthLabelB1 = ( Props::TruthLabelID.get( signalJets.at(0) ));
    //m_tree_vbf->TruthLabelB2 = ( Props::TruthLabelID.get( signalJets.at(1) ));
    m_tree_vbf->TruthLabelPartonB1 = ( Props::PartonTruthLabelID.get( signalJets.at(0) ));
    m_tree_vbf->TruthLabelPartonB2 = ( Props::PartonTruthLabelID.get( signalJets.at(1) ));
    m_tree_vbf->HadronConeExclTruthLabelB1 = ( Props::HadronConeExclTruthLabelID.get(signalJets.at(0) ));
    m_tree_vbf->HadronConeExclTruthLabelB2 = ( Props::HadronConeExclTruthLabelID.get(signalJets.at(1) ));
    
  }

  m_tree_vbf->MV2c10B1 = Props::MV2c10.get(signalJets.at(0));
  m_tree_vbf->MV2c10B2 = Props::MV2c10.get(signalJets.at(1));
  m_tree_vbf->MV2c10J1 = Props::MV2c10.get(vbfJets.at(0));
  m_tree_vbf->MV2c10J2 = Props::MV2c10.get(vbfJets.at(1));

  m_tree_vbf-> NTrk500PVJ1 = Props::NumTrkPt500PV.get(vbfJets.at(0));
  m_tree_vbf-> NTrk500PVJ2 = Props::NumTrkPt500PV.get(vbfJets.at(1));

  m_tree_vbf-> QGTagger_NTracksJ1 = !Props::DFCommonJets_QGTagger_NTracks.exists(vbfJets.at(0))?-1:Props::DFCommonJets_QGTagger_NTracks.get(vbfJets.at(0));
  m_tree_vbf-> QGTagger_NTracksJ2 = !Props::DFCommonJets_QGTagger_NTracks.exists(vbfJets.at(1))?-1:Props::DFCommonJets_QGTagger_NTracks.get(vbfJets.at(1));
  m_tree_vbf-> QGTagger_TracksWidthJ1 = !Props::DFCommonJets_QGTagger_TracksWidth.exists(vbfJets.at(0))?-1:Props::DFCommonJets_QGTagger_TracksWidth.get(vbfJets.at(0));
  m_tree_vbf-> QGTagger_TracksWidthJ2 = !Props::DFCommonJets_QGTagger_TracksWidth.exists(vbfJets.at(1))?-1:Props::DFCommonJets_QGTagger_TracksWidth.get(vbfJets.at(1));
  m_tree_vbf-> QGTagger_TracksC1J1 = !Props::DFCommonJets_QGTagger_TracksC1.exists(vbfJets.at(0))?-1:Props::DFCommonJets_QGTagger_TracksC1.get(vbfJets.at(0));
  m_tree_vbf-> QGTagger_TracksC1J2 = !Props::DFCommonJets_QGTagger_TracksC1.exists(vbfJets.at(1))?-1:Props::DFCommonJets_QGTagger_TracksC1.get(vbfJets.at(1));

  m_tree_vbf-> QGTaggerWeightJ1 = !Props::QGTaggerWeight.exists(vbfJets.at(0))?1:Props::QGTaggerWeight.get(vbfJets.at(0));
  m_tree_vbf-> QGTaggerWeightJ2 = !Props::QGTaggerWeight.exists(vbfJets.at(1))?1:Props::QGTaggerWeight.get(vbfJets.at(1));


  // ===== pT_balance
  TLorentzVector sumSelectedJets = j1Vec + j2Vec + b1Vec + b2Vec;
  TLorentzVector sumSelectedJetsJ = j1VecJ + j2VecJ + b1VecJ + b2VecJ;
  double sumPt = j1Vec.Pt() + j2Vec.Pt() + b1Vec.Pt() + b2Vec.Pt();
  double sumPtJ = j1VecJ.Pt() + j2VecJ.Pt() + b1VecJ.Pt() + b2VecJ.Pt();
  double sumPt_bb = j1Vec.Pt() + j2Vec.Pt() + (b1Vec + b2Vec).Pt();
  m_tree_vbf->pT_balance = sumSelectedJets.Pt() / sumPt;
  m_tree_vbf->pT_balanceJ = sumSelectedJetsJ.Pt() / sumPtJ;
  m_tree_vbf->pT_balance_pTBB = sumSelectedJets.Pt() / sumPt_bb;
  m_tree_vbf->pT_balance_fsrmin08    = (sumSelectedJets+tlv_exmin08).Pt()/(sumPt+tlvsum_exmin08);
  m_tree_vbf->pT_balance_fsrmin10    = (sumSelectedJets+tlv_exmin10).Pt()/(sumPt+tlvsum_exmin10);
  m_tree_vbf->pT_balance_fsrmin12    = (sumSelectedJets+tlv_exmin12).Pt()/(sumPt+tlvsum_exmin12);
  m_tree_vbf->pT_balance_fsr10    = (sumSelectedJets+tlv_ex10).Pt()/(sumPt+tlvsum_ex10);
  m_tree_vbf->pT_balance_fsr12    = (sumSelectedJets+tlv_ex12).Pt()/(sumPt+tlvsum_ex12);
  m_tree_vbf->pT_balance_fsr14    = (sumSelectedJets+tlv_ex14).Pt()/(sumPt+tlvsum_ex14);
  m_tree_vbf->pT_balance_fsr16    = (sumSelectedJets+tlv_ex16).Pt()/(sumPt+tlvsum_ex16);
  m_tree_vbf->pT_balance_fsr18    = (sumSelectedJets+tlv_ex18).Pt()/(sumPt+tlvsum_ex18);
  m_tree_vbf->pT_balance_fsr20    = (sumSelectedJets+tlv_ex20).Pt()/(sumPt+tlvsum_ex20);


  // ======
  m_tree_vbf->max_J1J2 = std::max( fabs(j1Vec.Eta()), fabs(j2Vec.Eta()) );
  m_tree_vbf->eta_J_star = ( fabs(j1Vec.Eta()) + fabs(j2Vec.Eta()) )/2 - ( fabs(b1Vec.Eta()) + fabs(b2Vec.Eta()) )/2;


  // Computation of HT_soft (taken from VBF+gamma) -- trunk version

  m_tree_vbf->HT_soft = 0;
  std::pair<std::pair<float, float>, float> El(getCenterSlopeOfEllipse(signalJets.at(0), signalJets.at(1)));

  for (int iJet(0); iJet < (int)trackJets.size(); iJet++) {
    if ( trackJetIsInEllipse(El, trackJets.at(iJet), m_tree_vbf->dRBB, 0.4) ) continue;
    if (trackJets.at(iJet)->p4().DeltaR(vbfJets.at(0)->p4()) < 0.4) continue;
    if (trackJets.at(iJet)->p4().DeltaR(vbfJets.at(1)->p4()) < 0.4) continue;

    if (trackJets.at(iJet)->pt() * 0.001 < 2) continue;
    m_tree_vbf->HT_soft += trackJets.at(iJet)->pt();
  }

  float deta =  fabs(j1Vec.Eta() - j2Vec.Eta());
  float aveta =  (j1Vec.Eta() + j2Vec.Eta())/2.;

  m_tree_vbf->HT_MVA  = 0;
  m_tree_vbf->centralMaxJ_Ex = 0;
  m_tree_vbf->nonforwardMaxJ_Ex = 0;
  m_tree_vbf->inclusiveMaxJ_Ex = 0;
  m_tree_vbf->centralMaxJ_Ex_dRB1 = 0;
  m_tree_vbf->nonforwardMaxJ_Ex_dRB1 = 0;
  m_tree_vbf->inclusiveMaxJ_Ex_dRB1 = 0;
  m_tree_vbf->centralMaxJ_zeppenfeld = 0;
  m_tree_vbf->nonforwardMaxJ_zeppenfeld = 0;
  m_tree_vbf->inclusiveMaxJ_zeppenfeld = 0;
  m_tree_vbf->centralMaxJ_eta = 0;
  m_tree_vbf->nonforwardMaxJ_eta = 0;
  m_tree_vbf->inclusiveMaxJ_eta = 0;
  m_tree_vbf->centralMaxJ_NTrk500PV = 0;
  m_tree_vbf->nonforwardMaxJ_NTrk500PV = 0;
  m_tree_vbf->inclusiveMaxJ_NTrk500PV = 0;
  m_tree_vbf->centralMaxJ_dEtaJ1 = 0;
  m_tree_vbf->nonforwardMaxJ_dEtaJ1 = 0;
  m_tree_vbf->inclusiveMaxJ_dEtaJ1 = 0;
  for (int iJet(0); iJet < (int)inputJets.size(); iJet++) {
    const xAOD::Jet *jet = inputJets.at(iJet);
    // Check if the selected jet has not been selected as signal or vbf
    if (iJet == m_tree_vbf->whoIsJ1) continue;
    if (iJet == m_tree_vbf->whoIsJ2) continue;
    if (iJet == m_tree_vbf->whoIsB1) continue;
    if (iJet == m_tree_vbf->whoIsB2) continue;
    if (inputJets.at(iJet)->pt() * 0.001 < 20) continue;
    if (fabs(jet->eta())>2.5 && Props::Timing.exists(jet) && (!Props::PassFJvtLoose.get( jet ) || fabs(Props::Timing.get( jet ))>10.)) continue;
    //if (fabs(jet->eta())>2.5 && (!Props::PassFJvtLoose.get( jet ) || fabs(Props::Timing.get( jet ))>10.)) continue;
    if (fabs(jet->eta())<2.5 && jet->pt()>m_tree_vbf->centralMaxJ_Ex) {m_tree_vbf->centralMaxJ_Ex = jet->pt(); m_tree_vbf->centralMaxJ_zeppenfeld = fabs(jet->eta()-aveta)/deta;; m_tree_vbf->centralMaxJ_dEtaJ1 = fabs(jet->eta()-j1Vec.Eta());m_tree_vbf->centralMaxJ_NTrk500PV = Props::NumTrkPt500PV.get(jet);m_tree_vbf->centralMaxJ_eta = jet->eta();}
    if (fabs(jet->eta())<3.2 && jet->pt()>m_tree_vbf->nonforwardMaxJ_Ex) {m_tree_vbf->nonforwardMaxJ_Ex = jet->pt(); m_tree_vbf->nonforwardMaxJ_zeppenfeld = fabs(jet->eta()-aveta)/deta;; m_tree_vbf->nonforwardMaxJ_dEtaJ1 = fabs(jet->eta()-j1Vec.Eta());m_tree_vbf->nonforwardMaxJ_NTrk500PV = Props::NumTrkPt500PV.get(jet);m_tree_vbf->nonforwardMaxJ_eta = jet->eta();}
    if (fabs(jet->eta())<4.5 && jet->pt()>m_tree_vbf->inclusiveMaxJ_Ex) {m_tree_vbf->inclusiveMaxJ_Ex = jet->pt(); m_tree_vbf->inclusiveMaxJ_zeppenfeld = fabs(jet->eta()-aveta)/deta;; m_tree_vbf->inclusiveMaxJ_dEtaJ1 = fabs(jet->eta()-j1Vec.Eta());m_tree_vbf->inclusiveMaxJ_NTrk500PV = Props::NumTrkPt500PV.get(jet);m_tree_vbf->inclusiveMaxJ_eta = jet->eta();}

    if (fabs(jet->eta())<2.5 && jet->pt()>m_tree_vbf->centralMaxJ_Ex_dRB1 && jet->p4().DeltaR(inputJets.at(m_tree_vbf->whoIsB1)->p4())>1 && jet->p4().DeltaR(inputJets.at(m_tree_vbf->whoIsB2)->p4())>1) m_tree_vbf->centralMaxJ_Ex_dRB1 = jet->pt();
    if (fabs(jet->eta())<3.2 && jet->pt()>m_tree_vbf->nonforwardMaxJ_Ex_dRB1 && jet->p4().DeltaR(inputJets.at(m_tree_vbf->whoIsB1)->p4())>1 && jet->p4().DeltaR(inputJets.at(m_tree_vbf->whoIsB2)->p4())>1) m_tree_vbf->nonforwardMaxJ_Ex_dRB1 = jet->pt();
    if (fabs(jet->eta())<4.5 && jet->pt()>m_tree_vbf->inclusiveMaxJ_Ex_dRB1 && jet->p4().DeltaR(inputJets.at(m_tree_vbf->whoIsB1)->p4())>1 && jet->p4().DeltaR(inputJets.at(m_tree_vbf->whoIsB2)->p4())>1) m_tree_vbf->inclusiveMaxJ_Ex_dRB1 = jet->pt();

    // Remove the region between the two b-jets
    double etaJet = inputJets.at(iJet)->eta();
    double etaJetBMin = inputJets.at(m_tree_vbf->whoIsB1)->eta() < inputJets.at(m_tree_vbf->whoIsB2)->eta() ? inputJets.at(m_tree_vbf->whoIsB1)->eta() : inputJets.at(m_tree_vbf->whoIsB2)->eta() ;
    double etaJetBMax = inputJets.at(m_tree_vbf->whoIsB1)->eta() < inputJets.at(m_tree_vbf->whoIsB2)->eta() ? inputJets.at(m_tree_vbf->whoIsB2)->eta() : inputJets.at(m_tree_vbf->whoIsB1)->eta() ;
    if ( etaJet > etaJetBMin && etaJet < etaJetBMax) continue;

    m_tree_vbf->HT_MVA += inputJets.at(iJet)->pt();
  }


  TLorentzVector Higgs = b1Vec + b2Vec;
  TVector3 boostingVector = Higgs.BoostVector();
  TVector3 CrossProduct = j1Vec.Vect().Cross(j2Vec.Vect());
  TLorentzVector CrossBoosted(CrossProduct,sqrt(CrossProduct*CrossProduct));
  CrossBoosted.Boost(boostingVector);
  m_tree_vbf->cosTheta_MVA = CrossBoosted.CosTheta();


  // cosTheta CMS def
  TLorentzVector rest4j = b1Vec + b2Vec + j1Vec + j2Vec;
  TVector3 boostVec4j = -(rest4j.BoostVector());
  TLorentzVector b1Vec4j = b1Vec;
  TLorentzVector b2Vec4j = b2Vec;
  TLorentzVector j1Vec4j = j1Vec;
  TLorentzVector j2Vec4j = j2Vec;
  b1Vec4j.Boost(boostVec4j);
  b2Vec4j.Boost(boostVec4j);
  j1Vec4j.Boost(boostVec4j);
  j2Vec4j.Boost(boostVec4j);
  TVector3 b1Vec3 = b1Vec4j.Vect();
  TVector3 b2Vec3 = b2Vec4j.Vect();
  TVector3 j1Vec3 = j1Vec4j.Vect();
  TVector3 j2Vec3 = j2Vec4j.Vect();
  TVector3 b1xb2 = b1Vec3.Cross(b2Vec3);
  TVector3 j1xj2 = j1Vec3.Cross(j2Vec3);
  float cosTheta_CMS = b1xb2 * j1xj2 / (b1xb2.Mag() * j1xj2.Mag());

  m_tree_vbf->cosTheta_CMS = cosTheta_CMS;

  TLorentzVector j1VecH = j1Vec;
  TLorentzVector j2VecH = j2Vec;
  TLorentzVector b1VecH = b1Vec;
  TLorentzVector HVec(b1Vec+b2Vec);
  TVector3 boostVecH = -(HVec.BoostVector());
  j1VecH.Boost(boostVecH);
  j2VecH.Boost(boostVecH);
  b1VecH.Boost(boostVecH);
  TVector3 j1VecH3 = j1VecH.Vect();
  TVector3 j2VecH3 = j2VecH.Vect();
  TVector3 j1xj2p = j1VecH3.Cross(j2VecH3);
  TVector3 HVec3 = HVec.Vect();
  float cosTheta_boost = j1xj2p.Dot(HVec3)/ (j1xj2p.Mag()*HVec3.Mag());

  m_tree_vbf->cosTheta_boost = cosTheta_boost;

  m_tree_vbf->hframe_cosTheta_h_b1 = cos(b1VecH.Angle(HVec3));

  m_bdt_pTBB = m_tree_vbf->pTBB;
  m_bdt_mJJ = m_tree_vbf->mJJ;
  m_bdt_pTJJ = m_tree_vbf->pTJJ;
  m_bdt_pT_balance = m_tree_vbf->pT_balance;
  m_bdt_mindRJ1_Ex = m_tree_vbf->mindRJ1_Ex;
  m_bdt_mindRJ2_Ex = m_tree_vbf->mindRJ2_Ex;
  m_bdt_min_mindRJ1_Ex = std::min(float(10.),m_tree_vbf->mindRJ1_Ex);
  m_bdt_min_mindRJ2_Ex = std::min(float(10.),m_tree_vbf->mindRJ2_Ex);
  m_bdt_centralMaxJ_Ex_dRB1 = m_tree_vbf->centralMaxJ_Ex_dRB1;
  m_bdt_centralMaxJ_Ex = m_tree_vbf->centralMaxJ_Ex;
  m_bdt_inclusiveMaxJ_Ex = m_tree_vbf->inclusiveMaxJ_Ex;
  m_bdt_inclusiveMaxJ_zeppenfeld = std::min<float>(m_tree_vbf->inclusiveMaxJ_zeppenfeld,2.);
  m_bdt_mismatchJJ = m_tree_vbf->pTJ2-m_tree_vbf->inclusiveMaxJ_Ex;
  m_bdt_hframe_cosTheta_h_b1 = m_tree_vbf->hframe_cosTheta_h_b1;
  m_bdt_asymJJ = m_tree_vbf->asymJJ;
  m_bdt_nJets = m_tree_vbf->nJets;
  m_bdt_dEtaJJ = m_tree_vbf->dEtaJJ;
  m_bdt_angleAsymBB = m_tree_vbf->angleAsymBB;
  m_bdt_trueangleAsymBB = (cosh(m_tree_vbf->dEtaBB)+cos(m_tree_vbf->dPhiBB)-2)/(cosh(m_tree_vbf->dEtaBB)-cos(m_tree_vbf->dPhiBB));
  m_bdt_mBB_angle = m_tree_vbf->mBB_angle;

  m_bdt_MV2c10B2 = m_tree_vbf->MV2c10B2;
  m_bdt_nLooseMv2c10 = m_tree_vbf->nLooseMv2c10;
  m_bdt_eta_J_star = m_tree_vbf->eta_J_star;
  m_bdt_cosTheta_boost = m_tree_vbf->cosTheta_boost;
  m_bdt_deltaMJJ = m_tree_vbf->deltaMJJ;
  m_bdt_deltaMJJ2 = m_tree_vbf->deltaMJJ2;
  m_bdt_deltaMJJ_pTBB_nomBB = m_tree_vbf->deltaMJJ_pTBB_nomBB;
  m_bdt_deltaMJJMax = std::max(m_tree_vbf->deltaMJJ,m_tree_vbf->deltaMJJ2);
  m_bdt_max_J1J2 = m_tree_vbf->max_J1J2;
  m_bdt_NTrk500PVJ1 = m_tree_vbf->NTrk500PVJ1;
  m_bdt_NTrk500PVJ2 = m_tree_vbf->NTrk500PVJ2;
  m_bdt_dPhiBBJJ = m_tree_vbf->dPhiBBJJ;
  m_bdt_dEtaBBJJ_dEtaJJ = m_tree_vbf->dEtaBBJJ/m_tree_vbf->dEtaJJ;
  m_bdt_dEtaBBJJ = m_tree_vbf->dEtaBBJJ;
  m_bdt_nJets20pt = m_tree_vbf->nJets20pt;
  //m_bdt_NTrk500PVJ1 = m_tree_vbf->NTrk500PVJ1+(fabs(m_tree_vbf->etaJ1)<2.5 && gRandom->Uniform(1)>0.5 ? 1 : 0);
  //m_bdt_NTrk500PVJ2 = m_tree_vbf->NTrk500PVJ2+(fabs(m_tree_vbf->etaJ2)<2.5 && gRandom->Uniform(1)>0.5 ? 1 : 0);
  //m_bdt_NTrk500PVJ1 = m_tree_vbf->NTrk500PVJ1;
  //m_bdt_NTrk500PVJ2 = m_tree_vbf->NTrk500PVJ2;
  m_tree_vbf->bdt_mcmj = (reader_mcmj->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt_1for2cen_final = (reader_1for2cen_final->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt_1for2cen_full = (reader_1for2cen_full->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt_1for2cen_trueasym = (reader_1for2cen_trueasym->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt_1for2cen_angle = (reader_1for2cen_angle->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt_1for2cen_nojets = (reader_1for2cen_nojets->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt_1for2cen_nojetsrap = (reader_1for2cen_nojetsrap->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt_1for2cen_pflow_5jet = (reader_1for2cen_pflow_5jet->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt_1for2cen_pflow_data = (reader_1for2cen_pflow_data->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt_walrus_pflow_data = (reader_walrus_pflow_data->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt_1for2cen_pflow = (reader_1for2cen_pflow->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt_walrus_final = (reader_walrus_final->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt2cen = (reader2cen->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt2cenWeight = (reader2cenWeight->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt2cenOldExt = (reader2cenOldExt->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt3cen = (reader3cen->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdtHighpt = (readerHighpt->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdttopoInc = (readertopoInc->EvaluateMVA( "BDT method" ));
  m_tree_vbf->bdt2cenOld = (reader2cenOld->EvaluateMVA( "BDT method" ));
 
  //auto outputs = conehead->compute(inputs);

  std::map<std::string, std::map<std::string, double> > inputs,inputsNoPTJJ,inputs1for,inputs_nunez,inputs_nojets,inputs_flow,inputs_flowmass,inputs_flowj,inputs_flowj3,inputs_nunez0,inputs_1for2cen,inputs_3cen,inputs2029_1for2cen,inputs2029_3cen;
  inputs_nunez["node_0"] = {{"variable_0", m_tree_vbf->mJJ/1e3},{"variable_1", m_tree_vbf->pTJJ/1e3},{"variable_2", m_tree_vbf->asymJJ},{"variable_3", m_tree_vbf->pT_balance},{"variable_4", m_tree_vbf->dEtaBBJJ/m_tree_vbf->dEtaJJ},{"variable_5", m_tree_vbf->dPhiBBJJ},{"variable_6", m_tree_vbf->angleAsymBB},{"variable_7", m_tree_vbf->nJets20pt},{"variable_8", std::min<float>(m_tree_vbf->mindRJ1_Ex,10.)},{"variable_9", std::min<float>(m_tree_vbf->mindRJ2_Ex,10.)},{"variable_10", m_tree_vbf->NTrk500PVJ2}};
  inputs_nunez0["node_0"] = {{"variable_0", m_tree_vbf->mJJ/1e3},{"variable_1", m_tree_vbf->pTJJ/1e3},{"variable_2", m_tree_vbf->asymJJ},{"variable_3", m_tree_vbf->pT_balance},{"variable_4", m_tree_vbf->dEtaBBJJ/m_tree_vbf->dEtaJJ},{"variable_5", m_tree_vbf->dPhiBBJJ},{"variable_6", m_tree_vbf->mBB_angle},{"variable_7", m_tree_vbf->NTrk500PVJ2}};
  inputs["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->deltaMJJ},{"variable_3", m_tree_vbf->mindRJ1_Ex},{"variable_4", m_tree_vbf->mindRJ2_Ex},{"variable_5", m_tree_vbf->eta_J_star},{"variable_6", m_tree_vbf->NTrk500PVJ1},{"variable_7", m_tree_vbf->NTrk500PVJ2},{"variable_8", m_tree_vbf->pT_balance},{"variable_9", m_tree_vbf->max_J1J2}};
  inputsNoPTJJ["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->deltaMJJ},{"variable_2", m_tree_vbf->mindRJ1_Ex},{"variable_3", m_tree_vbf->mindRJ2_Ex},{"variable_4", m_tree_vbf->eta_J_star},{"variable_5", m_tree_vbf->NTrk500PVJ1},{"variable_6", m_tree_vbf->NTrk500PVJ2},{"variable_7", m_tree_vbf->pT_balance},{"variable_8", m_tree_vbf->max_J1J2}};

  inputs1for["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->deltaMJJ},{"variable_3", m_tree_vbf->mindRJ1_Ex},{"variable_4", m_tree_vbf->mindRJ2_Ex},{"variable_5", m_tree_vbf->eta_J_star},{"variable_6", m_tree_vbf->NTrk500PVJ2},{"variable_7", m_tree_vbf->pT_balance}};


  std::map<std::string, double> outputs;

  std::map<std::string, std::map<std::string, double> > inputs_20190905;

  inputs_20190905["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->pT_balance},{"variable_3", m_tree_vbf->dEtaBBJJ},{"variable_4", m_tree_vbf->dPhiBBJJ},{"variable_5", m_tree_vbf->asymJJ},{"variable_6", m_tree_vbf->angleAsymBB},{"variable_7", m_tree_vbf->nJets20pt},{"variable_8", std::min<float>(m_tree_vbf->mindRJ1_Ex,10.)},{"variable_9", std::min<float>(m_tree_vbf->mindRJ2_Ex,10.)},{"variable_10", m_tree_vbf->NTrk500PVJ2}};
  inputs_nojets["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->pT_balance},{"variable_3", m_tree_vbf->dEtaBBJJ},{"variable_4", m_tree_vbf->dPhiBBJJ},{"variable_5", m_tree_vbf->asymJJ},{"variable_6", m_tree_vbf->mBB_angle},{"variable_7", m_tree_vbf->NTrk500PVJ2}};
  inputs_flow["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->pT_balance},{"variable_3", m_tree_vbf->dEtaBBJJ},{"variable_4", m_tree_vbf->dPhiBBJJ},{"variable_5", m_tree_vbf->asymJJ},{"variable_6", std::min<float>(m_tree_vbf->mindRJ1_Ex,10.)},{"variable_7", std::min<float>(m_tree_vbf->mindRJ2_Ex,10.)},{"variable_8", m_tree_vbf->mBB_angle},{"variable_9", m_tree_vbf->NTrk500PVJ2}};
  inputs_flowj["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->pT_balance},{"variable_3", m_tree_vbf->dEtaBBJJ},{"variable_4", m_tree_vbf->dPhiBBJJ},{"variable_5", m_tree_vbf->asymJJ},{"variable_6",m_tree_vbf->nJets20pt},{"variable_7", std::min<float>(m_tree_vbf->mindRJ1_Ex,10.)},{"variable_8", std::min<float>(m_tree_vbf->mindRJ2_Ex,10.)},{"variable_9", m_tree_vbf->mBB_angle},{"variable_10", m_tree_vbf->NTrk500PVJ2}};
  inputs_flowj3["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->pT_balance},{"variable_3", m_tree_vbf->dEtaBBJJ},{"variable_4", m_tree_vbf->dPhiBBJJ},{"variable_5", m_tree_vbf->asymJJ},{"variable_6",m_tree_vbf->nJets20pt},{"variable_7", std::min<float>(m_tree_vbf->mindRJ1_Ex,10.)},{"variable_8", std::min<float>(m_tree_vbf->mindRJ2_Ex,10.)},{"variable_9", m_tree_vbf->mBB_angle},{"variable_10", m_tree_vbf->NTrk500PVJ1},{"variable_11", m_tree_vbf->NTrk500PVJ2}};
  inputs_flowmass["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->pT_balance},{"variable_3", m_tree_vbf->dEtaBBJJ},{"variable_4", m_tree_vbf->dPhiBBJJ},{"variable_5", m_tree_vbf->asymJJ},{"variable_6", std::min<float>(m_tree_vbf->mindRJ1_Ex,10.)},{"variable_7", std::min<float>(m_tree_vbf->mindRJ2_Ex,10.)},{"variable_8", m_tree_vbf->dEtaBB},{"variable_9", m_tree_vbf->dPhiBB},{"variable_10", m_tree_vbf->pTB1},{"variable_11", m_tree_vbf->pTB2},{"variable_12", m_tree_vbf->NTrk500PVJ2}};

  float NTrk500PVJ1_mod = m_tree_vbf->NTrk500PVJ1;
  if (fabs(m_tree_vbf->etaJ1)>2.1 && m_tree_vbf->pTJ1<50e3) NTrk500PVJ1_mod = -1;
  if (fabs(m_tree_vbf->etaJ1)>2.1 && m_tree_vbf->pTJ1>50e3) NTrk500PVJ1_mod = -2;
  if (fabs(m_tree_vbf->etaJ1)<2.1 && m_tree_vbf->pTJ1<50e3) NTrk500PVJ1_mod = -3;
  float NTrk500PVJ2_mod = m_tree_vbf->NTrk500PVJ2;
  if (fabs(m_tree_vbf->etaJ2)>2.1 && m_tree_vbf->pTJ2<50e3) NTrk500PVJ2_mod = -1;
  if (fabs(m_tree_vbf->etaJ2)>2.1 && m_tree_vbf->pTJ2>50e3) NTrk500PVJ2_mod = -2;
  if (fabs(m_tree_vbf->etaJ2)<2.1 && m_tree_vbf->pTJ2<50e3) NTrk500PVJ2_mod = -3;
  inputs_1for2cen["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->pT_balance},{"variable_3", m_tree_vbf->dEtaBBJJ},{"variable_4", m_tree_vbf->dPhiBBJJ},{"variable_5", m_tree_vbf->asymJJ},{"variable_6",m_tree_vbf->nJets20pt},{"variable_7", std::min<float>(m_tree_vbf->mindRJ1_Ex,10.)},{"variable_8", std::min<float>(m_tree_vbf->mindRJ2_Ex,10.)},{"variable_9", m_tree_vbf->mBB_angle},{"variable_10",NTrk500PVJ2_mod}};
  //inputs_1for2cen["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->pT_balance},{"variable_3", m_tree_vbf->dEtaBBJJ},{"variable_4", m_tree_vbf->dPhiBBJJ},{"variable_5", m_tree_vbf->asymJJ},{"variable_6",m_tree_vbf->nJets20pt},{"variable_7", std::min<float>(m_tree_vbf->mindRJ1_Ex_base,10.)},{"variable_8", std::min<float>(m_tree_vbf->mindRJ2_Ex_base,10.)},{"variable_9", m_tree_vbf->mBB_angle},{"variable_10",NTrk500PVJ2_mod}};
  inputs_3cen["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->pT_balance},{"variable_3", m_tree_vbf->dEtaBBJJ},{"variable_4", m_tree_vbf->dPhiBBJJ},{"variable_5", m_tree_vbf->asymJJ},{"variable_6",m_tree_vbf->nJets20pt},{"variable_7", std::min<float>(m_tree_vbf->mindRJ1_Ex,10.)},{"variable_8", std::min<float>(m_tree_vbf->mindRJ2_Ex,10.)},{"variable_9", m_tree_vbf->mBB_angle},{"variable_10",NTrk500PVJ1_mod},{"variable_11",NTrk500PVJ2_mod}};
  //inputs_3cen["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->pT_balance},{"variable_3", m_tree_vbf->dEtaBBJJ},{"variable_4", m_tree_vbf->dPhiBBJJ},{"variable_5", m_tree_vbf->asymJJ},{"variable_6",m_tree_vbf->nJets20pt},{"variable_7", std::min<float>(m_tree_vbf->mindRJ1_Ex_base,10.)},{"variable_8", std::min<float>(m_tree_vbf->mindRJ2_Ex_base,10.)},{"variable_9", m_tree_vbf->mBB_angle},{"variable_10",NTrk500PVJ1_mod},{"variable_11",NTrk500PVJ2_mod}};
  inputs2029_1for2cen["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->pT_balance},{"variable_3", m_tree_vbf->dEtaBBJJ},{"variable_4", m_tree_vbf->dPhiBBJJ},{"variable_5", m_tree_vbf->asymJJ},{"variable_6", std::min<float>(m_tree_vbf->mindRJ1_Ex,10.)},{"variable_7", std::min<float>(m_tree_vbf->mindRJ2_Ex,10.)},{"variable_8", m_tree_vbf->mBB_angle},{"variable_9",NTrk500PVJ2_mod}};
  inputs2029_3cen["node_0"] = {{"variable_0", m_tree_vbf->mJJ},{"variable_1", m_tree_vbf->pTJJ},{"variable_2", m_tree_vbf->pT_balance},{"variable_3", m_tree_vbf->dEtaBBJJ},{"variable_4", m_tree_vbf->dPhiBBJJ},{"variable_5", m_tree_vbf->asymJJ},{"variable_6", std::min<float>(m_tree_vbf->mindRJ1_Ex,10.)},{"variable_7", std::min<float>(m_tree_vbf->mindRJ2_Ex,10.)},{"variable_8", m_tree_vbf->mBB_angle},{"variable_9",NTrk500PVJ1_mod},{"variable_10",NTrk500PVJ2_mod}};

  outputs = lwtgraph_nunez26->compute(inputs_nunez);
  m_tree_vbf->nn_nunez26 = outputs["out_0"];

  outputs = lwtgraph_RE29->compute(inputs_nunez);
  m_tree_vbf->nn_RE29 = outputs["out_0"];

  outputs = lwtgraph_RE34->compute(inputs_nunez);
  m_tree_vbf->nn_RE34 = outputs["out_0"];

  outputs = lwtgraph_dataM_e100_l0->compute(inputs_flowj);
  m_tree_vbf->nn_dataM_e100_l0 = outputs["out_0"];
  outputs = lwtgraph_dataM_e100_l10->compute(inputs_flowj);
  m_tree_vbf->nn_dataM_e100_l10 = outputs["out_0"];
  outputs = lwtgraph_2025odd_e1000_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_2025odd_e1000_l10 = outputs["out_0"];
  outputs = lwtgraph_3c_2025odd_e1000_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_2025odd_e1000_l10 = outputs["out_0"];
  outputs = lwtgraph_2024odd_e1000_l0->compute(inputs_1for2cen);
  m_tree_vbf->nn_2024odd_e1000_l0 = outputs["out_0"];
  outputs = lwtgraph_3c_2024odd_e1000_l0->compute(inputs_3cen);
  m_tree_vbf->nn_3c_2024odd_e1000_l0 = outputs["out_0"];

  outputs = lwtgraph_4100w2k0_e1000_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_4100w2k0_e1000_l10 = outputs["out_0"];
  outputs = lwtgraph_4100w2k1_e1000_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_4100w2k1_e1000_l10 = outputs["out_0"];

  outputs = lwtgraph_4100w4k0_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_4100w4k0_l10 = outputs["out_0"];
  outputs = lwtgraph_4100w4k1_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_4100w4k1_l10 = outputs["out_0"];
  outputs = lwtgraph_4100w4k2_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_4100w4k2_l10 = outputs["out_0"];
  outputs = lwtgraph_4100w4k3_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_4100w4k3_l10 = outputs["out_0"];

  outputs = lwtgraph_4100w4k0_l0->compute(inputs_1for2cen);
  m_tree_vbf->nn_4100w4k0_l0 = outputs["out_0"];
  outputs = lwtgraph_4100w4k1_l0->compute(inputs_1for2cen);
  m_tree_vbf->nn_4100w4k1_l0 = outputs["out_0"];
  outputs = lwtgraph_4100w4k2_l0->compute(inputs_1for2cen);
  m_tree_vbf->nn_4100w4k2_l0 = outputs["out_0"];
  outputs = lwtgraph_4100w4k3_l0->compute(inputs_1for2cen);
  m_tree_vbf->nn_4100w4k3_l0 = outputs["out_0"];

  outputs = lwtgraph_4200w4k0_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_4200w4k0_l10 = outputs["out_0"];
  outputs = lwtgraph_4200w4k1_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_4200w4k1_l10 = outputs["out_0"];
  outputs = lwtgraph_4200w4k2_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_4200w4k2_l10 = outputs["out_0"];
  outputs = lwtgraph_4200w4k3_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_4200w4k3_l10 = outputs["out_0"];

  outputs = lwtgraph_4200w2k0_e1000_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_4200w2k0_e1000_l10 = outputs["out_0"];
  outputs = lwtgraph_4200w2k1_e1000_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_4200w2k1_e1000_l10 = outputs["out_0"];

  outputs = lwtgraph_3000odd_l0->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000odd_l0 = outputs["out_0"];
  outputs = lwtgraph_3000even_l0->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000even_l0 = outputs["out_0"];

  outputs = lwtgraph_3000odd_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000odd_l10 = outputs["out_0"];
  outputs = lwtgraph_3000even_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000even_l10 = outputs["out_0"];

  outputs = lwtgraph_3000odd_l20->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000odd_l20 = outputs["out_0"];
  outputs = lwtgraph_3000even_l20->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000even_l20 = outputs["out_0"];

  outputs = lwtgraph_3000odd_l40->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000odd_l40 = outputs["out_0"];
  outputs = lwtgraph_3000even_l40->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000even_l40 = outputs["out_0"];

  outputs = lwtgraph_3000odd_l60->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000odd_l60 = outputs["out_0"];
  outputs = lwtgraph_3000even_l60->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000even_l60 = outputs["out_0"];

  outputs = lwtgraph_3000odd_l80->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000odd_l80 = outputs["out_0"];
  outputs = lwtgraph_3000even_l80->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000even_l80 = outputs["out_0"];

  outputs = lwtgraph_3000rodd_l0->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000rodd_l0 = outputs["out_0"];
  outputs = lwtgraph_3000reven_l0->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000reven_l0 = outputs["out_0"];

  outputs = lwtgraph_3000rodd_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000rodd_l10 = outputs["out_0"];
  outputs = lwtgraph_3000reven_l10->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000reven_l10 = outputs["out_0"];

  outputs = lwtgraph_3000rodd_l20->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000rodd_l20 = outputs["out_0"];
  outputs = lwtgraph_3000reven_l20->compute(inputs_1for2cen);
  m_tree_vbf->nn_3000reven_l20 = outputs["out_0"];

  outputs = lwtgraph_3c_3000rodd_l0->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000rodd_l0 = outputs["out_0"];
  outputs = lwtgraph_3c_3000reven_l0->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000reven_l0 = outputs["out_0"];

  outputs = lwtgraph_3c_3000rodd_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000rodd_l10 = outputs["out_0"];
  outputs = lwtgraph_3c_3000reven_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000reven_l10 = outputs["out_0"];

  outputs = lwtgraph_3c_3000rodd_l20->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000rodd_l20 = outputs["out_0"];
  outputs = lwtgraph_3c_3000reven_l20->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000reven_l20 = outputs["out_0"];

  outputs = lwtgraph_3c_3000odd_l0->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000odd_l0 = outputs["out_0"];
  outputs = lwtgraph_3c_3000even_l0->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000even_l0 = outputs["out_0"];

  outputs = lwtgraph_3c_3000odd_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000odd_l10 = outputs["out_0"];
  outputs = lwtgraph_3c_3000even_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000even_l10 = outputs["out_0"];

  outputs = lwtgraph_3c_3000odd_l20->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000odd_l20 = outputs["out_0"];
  outputs = lwtgraph_3c_3000even_l20->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000even_l20 = outputs["out_0"];

  outputs = lwtgraph_3c_3000odd_l40->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000odd_l40 = outputs["out_0"];
  outputs = lwtgraph_3c_3000even_l40->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000even_l40 = outputs["out_0"];

  outputs = lwtgraph_3c_3000odd_l100->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000odd_l100 = outputs["out_0"];
  outputs = lwtgraph_3c_3000even_l100->compute(inputs_3cen);
  m_tree_vbf->nn_3c_3000even_l100 = outputs["out_0"];

  outputs = lwtgraph_3c_4100w2k0_e1000_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4100w2k0_e1000_l10 = outputs["out_0"];
  outputs = lwtgraph_3c_4100w2k1_e1000_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4100w2k1_e1000_l10 = outputs["out_0"];

  outputs = lwtgraph_3c_4100w4k0_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4100w4k0_l10 = outputs["out_0"];
  outputs = lwtgraph_3c_4100w4k1_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4100w4k1_l10 = outputs["out_0"];
  outputs = lwtgraph_3c_4100w4k2_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4100w4k2_l10 = outputs["out_0"];
  outputs = lwtgraph_3c_4100w4k3_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4100w4k3_l10 = outputs["out_0"];

  outputs = lwtgraph_3c_4100w4k0_l0->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4100w4k0_l0 = outputs["out_0"];
  outputs = lwtgraph_3c_4100w4k1_l0->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4100w4k1_l0 = outputs["out_0"];
  outputs = lwtgraph_3c_4100w4k2_l0->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4100w4k2_l0 = outputs["out_0"];
  outputs = lwtgraph_3c_4100w4k3_l0->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4100w4k3_l0 = outputs["out_0"];

  outputs = lwtgraph_3c_4200w4k0_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4200w4k0_l10 = outputs["out_0"];
  outputs = lwtgraph_3c_4200w4k1_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4200w4k1_l10 = outputs["out_0"];
  outputs = lwtgraph_3c_4200w4k2_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4200w4k2_l10 = outputs["out_0"];
  outputs = lwtgraph_3c_4200w4k3_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4200w4k3_l10 = outputs["out_0"];

  outputs = lwtgraph_3c_4200w2k0_e1000_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4200w2k0_e1000_l10 = outputs["out_0"];
  outputs = lwtgraph_3c_4200w2k1_e1000_l10->compute(inputs_3cen);
  m_tree_vbf->nn_3c_4200w2k1_e1000_l10 = outputs["out_0"];

  m_tree_vbf->nn_4100w2kfold_e1000_l10 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_4100w2k0_e1000_l10 : m_tree_vbf->nn_4100w2k1_e1000_l10);
  m_tree_vbf->nn_4100w2kmerge_e1000_l10 = ((m_eventInfo->eventNumber()%2==1)*m_tree_vbf->nn_4100w2k1_e1000_l10*1+(m_eventInfo->eventNumber()%2==0)*m_tree_vbf->nn_4100w2k0_e1000_l10*0.893055);
  m_tree_vbf->nn_4100w4kfold_l10 = (m_eventInfo->eventNumber()%4==0)*m_tree_vbf->nn_4100w4k0_l10+(m_eventInfo->eventNumber()%4==1)*m_tree_vbf->nn_4100w4k1_l10+(m_eventInfo->eventNumber()%4==2)*m_tree_vbf->nn_4100w4k2_l10+(m_eventInfo->eventNumber()%4==3)*m_tree_vbf->nn_4100w4k3_l10;
  m_tree_vbf->nn_4100w4kmerge_l10 = ((m_eventInfo->eventNumber()%4==0)*m_tree_vbf->nn_4100w4k0_l10*1.06826+(m_eventInfo->eventNumber()%4==1)*m_tree_vbf->nn_4100w4k1_l10*0.986567+(m_eventInfo->eventNumber()%4==2)*m_tree_vbf->nn_4100w4k2_l10*0.941505+(m_eventInfo->eventNumber()%4==3)*m_tree_vbf->nn_4100w4k3_l10*1.01199);
  m_tree_vbf->nn_4200w4kfold_l10 = (m_eventInfo->eventNumber()%4==0)*m_tree_vbf->nn_4200w4k0_l10+(m_eventInfo->eventNumber()%4==1)*m_tree_vbf->nn_4200w4k1_l10+(m_eventInfo->eventNumber()%4==2)*m_tree_vbf->nn_4200w4k2_l10+(m_eventInfo->eventNumber()%4==3)*m_tree_vbf->nn_4200w4k3_l10;
  m_tree_vbf->nn_4200w4kmerge_l10 = ((m_eventInfo->eventNumber()%4==0)*m_tree_vbf->nn_4200w4k0_l10*0.955113+(m_eventInfo->eventNumber()%4==1)*m_tree_vbf->nn_4200w4k1_l10*1.0214+(m_eventInfo->eventNumber()%4==2)*m_tree_vbf->nn_4200w4k2_l10*1.01109+(m_eventInfo->eventNumber()%4==3)*m_tree_vbf->nn_4200w4k3_l10*1.01531);
  m_tree_vbf->nn_4200w2kfold_e1000_l10 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_4200w2k0_e1000_l10 : m_tree_vbf->nn_4200w2k1_e1000_l10);
  m_tree_vbf->nn_4200w2kmerge_e1000_l10 = ((m_eventInfo->eventNumber()%2==1)*m_tree_vbf->nn_4200w2k1_e1000_l10*0.838645+(m_eventInfo->eventNumber()%2==0)*m_tree_vbf->nn_4200w2k0_e1000_l10*1);
  m_tree_vbf->nn_3000shuffle_l0 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3000odd_l0 : m_tree_vbf->nn_3000even_l0);
  m_tree_vbf->nn_3000shuffle_l10 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3000odd_l10 : m_tree_vbf->nn_3000even_l10);
  m_tree_vbf->nn_3000shuffle_l20 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3000odd_l20 : m_tree_vbf->nn_3000even_l20);
  m_tree_vbf->nn_3000shuffle_l40 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3000odd_l40 : m_tree_vbf->nn_3000even_l40);
  m_tree_vbf->nn_3000shuffle_l60 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3000odd_l60 : m_tree_vbf->nn_3000even_l60);
  m_tree_vbf->nn_3000shuffle_l80 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3000odd_l80 : m_tree_vbf->nn_3000even_l80);
  m_tree_vbf->nn_3000rshuffle_l0 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3000rodd_l0 : m_tree_vbf->nn_3000reven_l0);
  m_tree_vbf->nn_3000rshuffle_l10 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3000rodd_l10 : m_tree_vbf->nn_3000reven_l10);
  m_tree_vbf->nn_3000rshuffle_l20 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3000rodd_l20 : m_tree_vbf->nn_3000reven_l20);
  m_tree_vbf->nn_3c_3000rshuffle_l0 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3c_3000rodd_l0 : m_tree_vbf->nn_3c_3000reven_l0);
  m_tree_vbf->nn_3c_3000rshuffle_l10 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3c_3000rodd_l10 : m_tree_vbf->nn_3c_3000reven_l10);
  m_tree_vbf->nn_3c_3000rshuffle_l20 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3c_3000rodd_l20 : m_tree_vbf->nn_3c_3000reven_l20);
  m_tree_vbf->nn_3c_3000shuffle_l0 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3c_3000odd_l0 : m_tree_vbf->nn_3c_3000even_l0);
  m_tree_vbf->nn_3c_3000shuffle_l10 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3c_3000odd_l10 : m_tree_vbf->nn_3c_3000even_l10);
  m_tree_vbf->nn_3c_3000shuffle_l20 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3c_3000odd_l20 : m_tree_vbf->nn_3c_3000even_l20);
  m_tree_vbf->nn_3c_3000shuffle_l40 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3c_3000odd_l40 : m_tree_vbf->nn_3c_3000even_l40);
  m_tree_vbf->nn_3c_3000shuffle_l100 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3c_3000odd_l100 : m_tree_vbf->nn_3c_3000even_l100);
  m_tree_vbf->nn_3c_4100w2kfold_e1000_l10 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3c_4100w2k0_e1000_l10 : m_tree_vbf->nn_3c_4100w2k1_e1000_l10);
  m_tree_vbf->nn_3c_4100w2kmerge_e1000_l10 = ((m_eventInfo->eventNumber()%2==1)*m_tree_vbf->nn_3c_4100w2k1_e1000_l10*1+(m_eventInfo->eventNumber()%2==0)*m_tree_vbf->nn_3c_4100w2k0_e1000_l10*0.878825);
  m_tree_vbf->nn_3c_4100w4kfold_l10 = (m_eventInfo->eventNumber()%4==0)*m_tree_vbf->nn_3c_4100w4k0_l10+(m_eventInfo->eventNumber()%4==1)*m_tree_vbf->nn_3c_4100w4k1_l10+(m_eventInfo->eventNumber()%4==2)*m_tree_vbf->nn_3c_4100w4k2_l10+(m_eventInfo->eventNumber()%4==3)*m_tree_vbf->nn_3c_4100w4k3_l10;
  m_tree_vbf->nn_3c_4100w4kmerge_l10 = ((m_eventInfo->eventNumber()%4==0)*m_tree_vbf->nn_3c_4100w4k0_l10*0.869092+(m_eventInfo->eventNumber()%4==1)*m_tree_vbf->nn_3c_4100w4k1_l10*1.26219+(m_eventInfo->eventNumber()%4==2)*m_tree_vbf->nn_3c_4100w4k2_l10*0.906698+(m_eventInfo->eventNumber()%4==3)*m_tree_vbf->nn_3c_4100w4k3_l10*1.048);
  m_tree_vbf->nn_3c_4200w4kfold_l10 = (m_eventInfo->eventNumber()%4==0)*m_tree_vbf->nn_3c_4200w4k0_l10+(m_eventInfo->eventNumber()%4==1)*m_tree_vbf->nn_3c_4200w4k1_l10+(m_eventInfo->eventNumber()%4==2)*m_tree_vbf->nn_3c_4200w4k2_l10+(m_eventInfo->eventNumber()%4==3)*m_tree_vbf->nn_3c_4200w4k3_l10;
  m_tree_vbf->nn_3c_4200w4kmerge_l10 = ((m_eventInfo->eventNumber()%4==0)*m_tree_vbf->nn_3c_4200w4k0_l10*0.922452+(m_eventInfo->eventNumber()%4==1)*m_tree_vbf->nn_3c_4200w4k1_l10*1.05268+(m_eventInfo->eventNumber()%4==2)*m_tree_vbf->nn_3c_4200w4k2_l10*0.950306+(m_eventInfo->eventNumber()%4==3)*m_tree_vbf->nn_3c_4200w4k3_l10*1.09447);
  m_tree_vbf->nn_3c_4200w2kfold_e1000_l10 = (m_eventInfo->eventNumber()%2==0 ? m_tree_vbf->nn_3c_4200w2k0_e1000_l10 : m_tree_vbf->nn_3c_4200w2k1_e1000_l10);
  m_tree_vbf->nn_3c_4200w2kmerge_e1000_l10 = ((m_eventInfo->eventNumber()%2==1)*m_tree_vbf->nn_3c_4200w2k1_e1000_l10*1+(m_eventInfo->eventNumber()%2==0)*m_tree_vbf->nn_3c_4200w2k0_e1000_l10*0.891916);

  outputs = lwtgraph_3c_dataM800_e100_l0->compute(inputs_flowj3);
  m_tree_vbf->nn_3c_dataM800_e100_l0 = outputs["out_0"];
  outputs = lwtgraph_3c_dataM800_e100_l10->compute(inputs_flowj3);
  m_tree_vbf->nn_3c_dataM800_e100_l10 = outputs["out_0"];

  m_tree_vbf->nn_4100w4f0_l10 = h_rescale_4100w4fold0_l10->FindBin(m_tree_vbf->nn_4100w4k0_l10)/1000.;
  m_tree_vbf->nn_4100w4f1_l10 = h_rescale_4100w4fold1_l10->FindBin(m_tree_vbf->nn_4100w4k1_l10)/1000.;
  m_tree_vbf->nn_4100w4f2_l10 = h_rescale_4100w4fold2_l10->FindBin(m_tree_vbf->nn_4100w4k2_l10)/1000.;
  m_tree_vbf->nn_4100w4f3_l10 = h_rescale_4100w4fold3_l10->FindBin(m_tree_vbf->nn_4100w4k3_l10)/1000.;
  if ((m_eventInfo->eventNumber()%4)==0) m_tree_vbf->nn_4100w4fmerge_l10 = m_tree_vbf->nn_4100w4f0_l10;
  if ((m_eventInfo->eventNumber()%4)==1) m_tree_vbf->nn_4100w4fmerge_l10 = m_tree_vbf->nn_4100w4f1_l10;
  if ((m_eventInfo->eventNumber()%4)==2) m_tree_vbf->nn_4100w4fmerge_l10 = m_tree_vbf->nn_4100w4f2_l10;
  if ((m_eventInfo->eventNumber()%4)==3) m_tree_vbf->nn_4100w4fmerge_l10 = m_tree_vbf->nn_4100w4f3_l10;

  m_tree_vbf->nn_4100w4f0_l0 = h_rescale_4100w4fold0_l0->FindBin(m_tree_vbf->nn_4100w4k0_l0)/1000.;
  m_tree_vbf->nn_4100w4f1_l0 = h_rescale_4100w4fold1_l0->FindBin(m_tree_vbf->nn_4100w4k1_l0)/1000.;
  m_tree_vbf->nn_4100w4f2_l0 = h_rescale_4100w4fold2_l0->FindBin(m_tree_vbf->nn_4100w4k2_l0)/1000.;
  m_tree_vbf->nn_4100w4f3_l0 = h_rescale_4100w4fold3_l0->FindBin(m_tree_vbf->nn_4100w4k3_l0)/1000.;
  if ((m_eventInfo->eventNumber()%4)==0) m_tree_vbf->nn_4100w4fmerge_l0 = m_tree_vbf->nn_4100w4f0_l0;
  if ((m_eventInfo->eventNumber()%4)==1) m_tree_vbf->nn_4100w4fmerge_l0 = m_tree_vbf->nn_4100w4f1_l0;
  if ((m_eventInfo->eventNumber()%4)==2) m_tree_vbf->nn_4100w4fmerge_l0 = m_tree_vbf->nn_4100w4f2_l0;
  if ((m_eventInfo->eventNumber()%4)==3) m_tree_vbf->nn_4100w4fmerge_l0 = m_tree_vbf->nn_4100w4f3_l0;

  m_tree_vbf->nn_4100w2f0_e1000_l10 = h_rescale_4100w2fold0_l10->FindBin(m_tree_vbf->nn_4100w2k0_e1000_l10)/1000.;
  m_tree_vbf->nn_4100w2f1_e1000_l10 = h_rescale_4100w2fold1_l10->FindBin(m_tree_vbf->nn_4100w2k1_e1000_l10)/1000.;
  if ((m_eventInfo->eventNumber()%2)==0) m_tree_vbf->nn_4100w2fmerge_e1000_l10 = m_tree_vbf->nn_4100w2f0_e1000_l10;
  if ((m_eventInfo->eventNumber()%2)==1) m_tree_vbf->nn_4100w2fmerge_e1000_l10 = m_tree_vbf->nn_4100w2f1_e1000_l10;

  m_tree_vbf->nn_3c_4100w4f0_l10 = h_rescale_3c_4100w4fold0_l10->FindBin(m_tree_vbf->nn_3c_4100w4k0_l10)/1000.;
  m_tree_vbf->nn_3c_4100w4f1_l10 = h_rescale_3c_4100w4fold1_l10->FindBin(m_tree_vbf->nn_3c_4100w4k1_l10)/1000.;
  m_tree_vbf->nn_3c_4100w4f2_l10 = h_rescale_3c_4100w4fold2_l10->FindBin(m_tree_vbf->nn_3c_4100w4k2_l10)/1000.;
  m_tree_vbf->nn_3c_4100w4f3_l10 = h_rescale_3c_4100w4fold3_l10->FindBin(m_tree_vbf->nn_3c_4100w4k3_l10)/1000.;
  if ((m_eventInfo->eventNumber()%4)==0) m_tree_vbf->nn_3c_4100w4fmerge_l10 = m_tree_vbf->nn_3c_4100w4f0_l10;
  if ((m_eventInfo->eventNumber()%4)==1) m_tree_vbf->nn_3c_4100w4fmerge_l10 = m_tree_vbf->nn_3c_4100w4f1_l10;
  if ((m_eventInfo->eventNumber()%4)==2) m_tree_vbf->nn_3c_4100w4fmerge_l10 = m_tree_vbf->nn_3c_4100w4f2_l10;
  if ((m_eventInfo->eventNumber()%4)==3) m_tree_vbf->nn_3c_4100w4fmerge_l10 = m_tree_vbf->nn_3c_4100w4f3_l10;

  m_tree_vbf->nn_3c_4100w4f0_l0 = h_rescale_3c_4100w4fold0_l0->FindBin(m_tree_vbf->nn_3c_4100w4k0_l0)/1000.;
  m_tree_vbf->nn_3c_4100w4f1_l0 = h_rescale_3c_4100w4fold1_l0->FindBin(m_tree_vbf->nn_3c_4100w4k1_l0)/1000.;
  m_tree_vbf->nn_3c_4100w4f2_l0 = h_rescale_3c_4100w4fold2_l0->FindBin(m_tree_vbf->nn_3c_4100w4k2_l0)/1000.;
  m_tree_vbf->nn_3c_4100w4f3_l0 = h_rescale_3c_4100w4fold3_l0->FindBin(m_tree_vbf->nn_3c_4100w4k3_l0)/1000.;
  if ((m_eventInfo->eventNumber()%4)==0) m_tree_vbf->nn_3c_4100w4fmerge_l0 = m_tree_vbf->nn_3c_4100w4f0_l0;
  if ((m_eventInfo->eventNumber()%4)==1) m_tree_vbf->nn_3c_4100w4fmerge_l0 = m_tree_vbf->nn_3c_4100w4f1_l0;
  if ((m_eventInfo->eventNumber()%4)==2) m_tree_vbf->nn_3c_4100w4fmerge_l0 = m_tree_vbf->nn_3c_4100w4f2_l0;
  if ((m_eventInfo->eventNumber()%4)==3) m_tree_vbf->nn_3c_4100w4fmerge_l0 = m_tree_vbf->nn_3c_4100w4f3_l0;

  m_tree_vbf->nn_3c_4100w2f0_e1000_l10 = h_rescale_3c_4100w2fold0_l10->FindBin(m_tree_vbf->nn_3c_4100w2k0_e1000_l10)/1000.;
  m_tree_vbf->nn_3c_4100w2f1_e1000_l10 = h_rescale_3c_4100w2fold1_l10->FindBin(m_tree_vbf->nn_3c_4100w2k1_e1000_l10)/1000.;
  if ((m_eventInfo->eventNumber()%2)==0) m_tree_vbf->nn_3c_4100w2fmerge_e1000_l10 = m_tree_vbf->nn_3c_4100w2f0_e1000_l10;
  if ((m_eventInfo->eventNumber()%2)==1) m_tree_vbf->nn_3c_4100w2fmerge_e1000_l10 = m_tree_vbf->nn_3c_4100w2f1_e1000_l10;

  // after filling the MVA tree, start to evaluate MVA
  m_tree_vbf->ReadMVA();

  return EL::StatusCode::SUCCESS;

}

// a function to check specifig cuts
bool AnalysisReader_VBFHbb::passSpecificCuts(const unsigned long int flag, const std::vector<unsigned int> &cuts) {
  unsigned int mask = 0;
  // Make the bit mask
  for (auto cut : cuts) mask = mask | 1 << cut;
  // Check if the flag matches the bit mask
  return (flag & mask) == mask;
}

EL::StatusCode AnalysisReader_VBFHbb::fill_jetHistos (std::vector<const xAOD::Jet*> jets, std::vector<const xAOD::Jet*> signalJets, std::vector<const xAOD::Jet*> forwardJets) {

  m_histSvc->BookFillHist("NJets", 11, -0.5, 10.5, jets.size(), m_weight);
  m_histSvc->BookFillHist("NbJets", 11, -0.5, 10.5, signalJets.size(), m_weight);

  if (!m_doReduceFillHistos) {
    m_histSvc->BookFillHist("NFwdJets", 11, -0.5, 10.5, forwardJets.size(), m_weight);
    for (const xAOD::Jet *jet : jets) {
      m_histSvc->BookFillHist("PtJets", 100,  0, 100, jet->pt() / 1e3, m_weight);
      m_histSvc->BookFillHist("EtaJets", 100, -5,   5, jet->eta(),    m_weight);
    }

    for (const xAOD::Jet *jet : forwardJets) {
      m_histSvc->BookFillHist("PtJets", 100,  0, 100, jet->pt() / 1e3, m_weight);
      m_histSvc->BookFillHist("EtaJets", 100, -5,   5, jet->eta(),    m_weight);
    }
  }

  return EL::StatusCode::SUCCESS;
} // fill_jetHistos



// ADDED FROM VBF+GAMMA

std::pair<std::pair<float, float>, float>
AnalysisReader_VBFHbb::getCenterSlopeOfEllipse(const xAOD::Jet* jet1, const xAOD::Jet* jet2)
{
  float eta0;
  float eta1;
  float eta2;
  float phi0;
  float phi1;
  float phi2;
  float m;

  if(jet1->phi() > jet2->phi()){
    phi1 = jet1->phi();
    phi2 = jet2->phi();
    eta1 = jet1->eta();
    eta2 = jet2->eta();
  }
  else{
    phi1 = jet2->phi();
    phi2 = jet1->phi();
    eta1 = jet2->eta();
    eta2 = jet1->eta();
  }

  eta0 = (eta1 + eta2)/2;
  float dphi = phi1 - phi2;
  if(dphi > TMath::Pi()) phi2 = phi2 + 2.*TMath::Pi();

  phi0 = (phi1 + phi2)/2;
  m = (phi1 - phi0)/(eta1 - eta0);
  if (phi0 > TMath::Pi()) phi0 = phi0 - 2.*TMath::Pi();

  std::pair<float, float> C(eta0, phi0);
  std::pair<std::pair<float, float>, float> El(C, m);

  return El;
}


bool AnalysisReader_VBFHbb::trackJetIsInEllipse(std::pair<std::pair<float, float>, float> El,
						const xAOD::Jet* jet, float dRBB, float r){

  float eta0 = El.first.first;
  float phi0 = El.first.second;
  float m = El.second;
  float m_p = -1./m;

  float etaj = jet->eta();
  float phij = jet->phi();

  if ( fabs(phi0 - phij) > TMath::Pi() ){
    phij = (phi0 > phij)? phij + 2*TMath::Pi() : phij - 2*TMath::Pi();
  }

  float eta_I1 = ( 1. / (m - m_p) ) * ( m*eta0 - m_p*etaj - phi0 + phij);
  float phi_I1 = m * (eta_I1 - eta0) + phi0;

  float eta_I2 = ( 1. / (m_p - m) ) * ( m_p*eta0 - m*etaj - phi0 + phij);
  float phi_I2 = m_p * (eta_I2 - eta0) + phi0;


  // not sure below here!!!
  float dEta1 = eta_I1 - etaj;
  float dPhi1 = phi_I1 - phij;

  float dEta2 = eta_I2 - etaj;
  float dPhi2 = phi_I2 - phij;

  float d1sq = dPhi1*dPhi1 + dEta1*dEta1;
  float d2sq = dPhi2*dPhi2 + dEta2*dEta2;

  float b = r;
  float a = 0.5 * dRBB + r;

  bool inEllipse = ( (d1sq / (b*b)) + (d2sq / (a*a)) ) < 1;

  return inEllipse;
}

std::vector<float> AnalysisReader_VBFHbb::ReadOfflineBJetTriggerWeight(TGraph *TrigSF, double var, bool ispT){

  if (ispT) var = var/1000.;

  int NPoints = TrigSF->GetN();
  double weight = 1;
  double error = 0;
  Double_t x_mc[100], y_mc[100], x_data[100], y_data[100], x[100], y[100];
  x_data[0] = 0;
  y_data[0] = 0;
  x_mc[0] = 0;
  y_mc[0] = 0;
  x[0] = 0;
  y[0] = 0;
  std::vector<float> weights ={1,0};
  
  for (unsigned int iPoint(0); iPoint<NPoints; iPoint++) {
    TrigSF-> GetPoint(iPoint+1,x[iPoint+1],y[iPoint+1]);
    error = TrigSF->GetErrorY(iPoint+1);
    if ( var>x[iPoint] and var<x[iPoint+1]){
      weights[0]=y[iPoint+1];
      weights[1]=error;
    }
  }

  return weights;
}


float AnalysisReader_VBFHbb::computeBTagSFWeightPerJet (const std::vector<const xAOD::Jet*> &signalJets, const std::string &authorName , TString WP)
{
  if (m_debug) Info("computeBTagSFWeight()", "Called.");

  if (!m_isMC) {
    return 1;
  }
  BTaggingTool *bTagTool;
  if      (WP == "85") bTagTool = m_bTagTool_Loose;
  else if (WP == "77") bTagTool = m_bTagTool_Medium;
  else                 bTagTool = m_bTagTool;
   
  bool truth_tag=false;
  m_config->getif<bool>("doTruthTagging", truth_tag);
  float weight = 1.0;
  bTagTool->setJetAuthor(authorName);
  bool doWeightVar{bTagTool->doWeightVar()},
    isNominal{(m_currentVar == "Nominal")};

    if (doWeightVar && !isNominal) bTagTool->setWeightVar(false);
    auto btagEffSFs = (truth_tag?bTagTool->computeEventWeight_truthTag(m_eventInfo->eventNumber(),signalJets,m_config):bTagTool->computeEventWeight(signalJets));
    bTagTool->setWeightVar(doWeightVar);

    weight = btagEffSFs["Nominal"];

    //only write syst histos if nominalOnly is set to false
    bool nominalOnly = false;
    m_config->getif<bool>("nominalOnly", nominalOnly);
    if (!nominalOnly && isNominal && doWeightVar) {
      for (auto effSF : btagEffSFs) {
        std::string varName = effSF.first;
        if (varName == "Nominal") continue;
        if (authorName != "") {
          auto n = varName.rfind("__1up");
          if (n == std::string::npos) n = varName.rfind("__1down");
          if (n == std::string::npos) varName += "_" + WP+ authorName;
          else varName.insert(n, "_" + authorName);
        }

        m_weightSysts.push_back({ varName, effSF.second / weight });
        if (m_debug) Info("computeBTagSFWeight", "Relative weight for %s = %f", effSF.first.c_str(), effSF.second/weight);
      }
    }

    if (m_debug) Info("computeBTagSFWeight()", "Event weight = %f", weight);
    return weight;
} // computeBTagSFWeight

EL::StatusCode AnalysisReader_VBFHbb::initializeSumOfWeights() {
  Info("initializeSumOfWeights()", "Initialize sum of weights.");

  m_config->getif<std::string>("period", m_period);

  const xAOD::EventInfo *eventInfo = m_eventInfoReader->getObjects("Nominal");
  int runNumber = (int)eventInfo->runNumber();
  if (runNumber<300000) m_period="a";
  if (runNumber==300000) m_period="d";
  if (runNumber>300000) m_period="e";

  if (!m_isMC) {
    return EL::StatusCode::SUCCESS;
  }

  std::string sumOfWeightsFile;
  bool generateYieldFile = false;
  std::string CxAODYieldDir = "CxAODOperations_VHbb/CxAOD/info";
  std::string analysisType = m_config->get<std::string>("analysisType");
  std::string comEnergy = m_config->get<std::string>("COMEnergy");
  m_config->getif<bool>("generateYieldFile", generateYieldFile);
  m_config->getif<std::string>("CxAODYieldDir", CxAODYieldDir);

  if (generateYieldFile && (m_sumOfWeightsFile != "")) {
    sumOfWeightsFile = m_sumOfWeightsFile;
  } else {
    sumOfWeightsFile = gSystem->Getenv("WorkDir_DIR");
    int releaseSeries = atoi(getenv("ROOTCORE_RELEASE_SERIES"));
    if (releaseSeries >= 25) sumOfWeightsFile = gSystem->Getenv("WorkDir_DIR");
    sumOfWeightsFile += "/data/" + CxAODYieldDir + "/yields.";
    sumOfWeightsFile += analysisType;
    sumOfWeightsFile += ".";
    sumOfWeightsFile += comEnergy;
    sumOfWeightsFile += "_mc16" + m_period;
    sumOfWeightsFile += ".txt";
  }
  // if yieldFile is given in config file, replace all we just did
  m_config->getif<string>("yieldFile", sumOfWeightsFile);
  m_sumOfWeightsProvider = new sumOfWeightsProvider(sumOfWeightsFile);

  if (!m_sumOfWeightsProvider) {
    Error("initializeSumOfWeights()", "sumOfWeightsProvider not initialized!");
    return EL::StatusCode::FAILURE;
  }
  return EL::StatusCode::SUCCESS;
}  // initializeSumOfWeights

float AnalysisReader_VBFHbb::reducedSF(TH2D *hist,float eta,float pt) {
  float centralval = hist->GetBinContent(hist->FindBin(eta,pt));
  if (fabs(centralval-1)>0.02) return centralval;
  for (int i = -1; i <= 1; i++) for (int j = -1; j <= 1; j++) {
    float newval = hist->GetBinContent(hist->FindBin(eta+hist->GetXaxis()->GetBinWidth(1)*i,pt+hist->GetYaxis()->GetBinWidth(1)*j));
    if (fabs(centralval-1)>0.02) return centralval;
  }
  return 1;
}

int AnalysisReader_VBFHbb::getSTXS() {
  TLorentzVector tlvh;
  tlvh.SetPtEtaPhiM(Props::hpt.get(m_eventInfo),Props::heta.get(m_eventInfo),Props::hphi.get(m_eventInfo),Props::hm.get(m_eventInfo));
  int nJets_truth = m_eventInfo->auxdata<int>("HTXS_Njets_pTjet30");

  std::vector<float> tjetpt = m_eventInfo->auxdata<std::vector<float> >("HTXS_V_jets25_pt");
  std::vector<float> tjeteta = m_eventInfo->auxdata<std::vector<float> >("HTXS_V_jets25_eta");
  std::vector<float> tjetphi = m_eventInfo->auxdata<std::vector<float> >("HTXS_V_jets25_phi");
  std::vector<float> tjetm = m_eventInfo->auxdata<std::vector<float> >("HTXS_V_jets25_m");
  float maxor = 0;
  float mJJ_truth = 0;
  float pTHJJ_truth_new = 0;
  float newpthjj = 0;
  for (size_t i = 0; i < tjetpt.size(); i++) for (size_t j = 0; j < i; j++) {
    if (tjetpt[i]<30e3) continue;
    if (tjetpt[j]<30e3) continue;
    if (tjetpt[i]+tjetpt[j]<maxor) continue;
    maxor = tjetpt[i]+tjetpt[j];
    TLorentzVector tlv1,tlv2;
    tlv1.SetPtEtaPhiM(tjetpt[i],tjeteta[i],tjetphi[i],tjetm[i]);
    tlv2.SetPtEtaPhiM(tjetpt[j],tjeteta[j],tjetphi[j],tjetm[j]);
    mJJ_truth = (tlv1+tlv2).M();
    pTHJJ_truth_new = (tlv1+tlv2+tlvh).Pt();
  }
  maxor = 0;
  float pTHJ_truth_new = 0;
  for (size_t i = 0; i < tjetpt.size(); i++) {
    if (tjetpt[i]<30e3) continue;
    if (tjetpt[i]<maxor) continue;
    maxor = tjetpt[i];
    TLorentzVector tlv1;
    tlv1.SetPtEtaPhiM(tjetpt[i],tjeteta[i],tjetphi[i],tjetm[i]);
    pTHJ_truth_new = (tlv1+tlvh).Pt();
  }
  if (m_mcChannel == 345342){
    if (fabs(tlvh.Rapidity())>2.5) return 100;
    else if (tlvh.Pt()>200e3 && tlvh.Pt()<300e3 && pTHJ_truth_new/tlvh.Pt()<0.15) return 101;
    else if (tlvh.Pt()>300e3 && tlvh.Pt()<450e3 && pTHJ_truth_new/tlvh.Pt()<0.15) return 102;
    else if (tlvh.Pt()>450e3 && tlvh.Pt()<650e3 && pTHJ_truth_new/tlvh.Pt()<0.15) return 103;
    else if (tlvh.Pt()>650e3 && pTHJ_truth_new/tlvh.Pt()<0.15) return 104;
    else if (tlvh.Pt()>200e3 && tlvh.Pt()<300e3) return 105;
    else if (tlvh.Pt()>300e3 && tlvh.Pt()<450e3) return 106;
    else if (tlvh.Pt()>450e3 && tlvh.Pt()<650e3) return 107;
    else if (tlvh.Pt()>650e3) return 108;
    else if (nJets_truth==0 && tlvh.Pt()<10e3) return 109;
    else if (nJets_truth==0) return 110;
    else if (nJets_truth==1 && tlvh.Pt()<60e3) return 111;
    else if (nJets_truth==1 && tlvh.Pt()<120e3) return 112;
    else if (nJets_truth==1) return 113;
    else if (mJJ_truth<350e3 && tlvh.Pt()<60e3 && pTHJJ_truth_new<25e3) return 114;
    else if (mJJ_truth<350e3 && tlvh.Pt()<120e3 && pTHJJ_truth_new<25e3) return 115;
    else if (mJJ_truth<350e3 && pTHJJ_truth_new<25e3) return 116;
    else if (mJJ_truth<350e3 && tlvh.Pt()<60e3) return 117;
    else if (mJJ_truth<350e3 && tlvh.Pt()<120e3) return 118;
    else if (mJJ_truth<350e3) return 119;
    else if (mJJ_truth<700e3 && pTHJJ_truth_new<25e3) return 120;
    else if (mJJ_truth<700e3) return 121;
    else if (mJJ_truth<1000e3 && pTHJJ_truth_new<25e3) return 122;
    else if (mJJ_truth<1000e3) return 123;
    else if (mJJ_truth<1500e3 && pTHJJ_truth_new<25e3) return 124;
    else if (mJJ_truth<1500e3) return 125;
    else if (pTHJJ_truth_new<25e3) return 126;
    else return 127;
  }
  if (m_mcChannel == 345949){
    if (fabs(tlvh.Rapidity())>2.5) return 200;
    else if (nJets_truth == 0) return 201;
    else if (nJets_truth == 1) return 202;
    else if (pTHJJ_truth_new<25e3 && mJJ_truth<60e3) return 203;
    else if (pTHJJ_truth_new<25e3 && mJJ_truth<120e3) return 204;
    else if (pTHJJ_truth_new<25e3 && mJJ_truth<350e3) return 205;
    else if (mJJ_truth<60e3) return 206;
    else if (mJJ_truth<120e3) return 207;
    else if (mJJ_truth<350e3) return 208;
    else if (pTHJJ_truth_new<25e3 && tlvh.Pt()<200e3 && mJJ_truth<700e3) return 209;
    else if (tlvh.Pt()<200e3 && mJJ_truth<700e3) return 210;
    else if (pTHJJ_truth_new<25e3 && tlvh.Pt()<200e3 && mJJ_truth<1000e3) return 211;
    else if (tlvh.Pt()<200e3 && mJJ_truth<1000e3) return 212;
    else if (pTHJJ_truth_new<25e3 && tlvh.Pt()<200e3 && mJJ_truth<1500e3) return 213;
    else if (tlvh.Pt()<200e3 && mJJ_truth<1500e3) return 214;
    else if (pTHJJ_truth_new<25e3 && tlvh.Pt()<200e3) return 215;
    else if (tlvh.Pt()<200e3) return 216;

    else if (pTHJJ_truth_new<25e3 && mJJ_truth<700e3) return 217;
    else if (mJJ_truth<700e3) return 218;
    else if (pTHJJ_truth_new<25e3 && mJJ_truth<1000e3) return 219;
    else if (mJJ_truth<1000e3) return 220;
    else if (pTHJJ_truth_new<25e3 && mJJ_truth<1500e3) return 221;
    else if (mJJ_truth<1500e3) return 222;
    else if (pTHJJ_truth_new<25e3) return 223;
    else return 224;
 } 
 return 0;

}
