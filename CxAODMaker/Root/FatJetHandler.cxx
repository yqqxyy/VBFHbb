#include <iostream>
#include <typeinfo>

#include "CxAODMaker/FatJetHandler.h"
#include "CxAODMaker/JetSemileptonic.h"
#include "CxAODMaker/MuonHandler.h"
#include "CxAODMaker/EventInfoHandler.h"
#include "CxAODMaker/JetSemileptonic.h"

#include "JetUncertainties/JetUncertaintiesTool.h"
#include "JetUncertainties/FFJetSmearingTool.h"
#include "JetCalibTools/JetCalibrationTool.h"

SG::AuxElement::ConstAccessor<int> FatJetHandler::GhostTrackCount("GhostTrackCount");
SG::AuxElement::Accessor<std::vector<float>> FatJetHandler::TrackWidthPt500("TrackWidthPt500");
SG::AuxElement::ConstAccessor<int> FatJetHandler::GhostMuonSegmentCount("GhostMuonSegmentCount");
SG::AuxElement::ConstAccessor<int> FatJetHandler::NSubjet("NSubjet");
SG::AuxElement::ConstAccessor<float> FatJetHandler::JetConstitScaleMomentum_eta("JetConstitScaleMomentum_eta");

//for Hbb muon-in-jet correction
static const SG::AuxElement::ConstAccessor<ElementLink<xAOD::JetContainer>> acc_parent("Parent");

//for retrieving the jet type
SG::AuxElement::ConstAccessor<int> FatJetHandler::GhostHBosonsCount("GhostHBosonsCount");
SG::AuxElement::ConstAccessor<int> FatJetHandler::GhostTQuarksFinalCount("GhostTQuarksFinalCount");
SG::AuxElement::ConstAccessor<int> FatJetHandler::GhostWBosonsCount("GhostWBosonsCount");
SG::AuxElement::ConstAccessor<int> FatJetHandler::GhostZBosonsCount("GhostZBosonsCount");


FatJetHandler::FatJetHandler(const std::string& name, ConfigStore & config, xAOD::TEvent * event,
    EventInfoHandler & eventInfoHandler) :
  ObjectHandler(name, config, event, eventInfoHandler),
  m_isLCTOPO(true),
  m_isTCC(false),
  m_isUFOVSD(false),
  m_matchTruthJet(false),
  m_storeGAParticlesInFatJets(true),
  m_saveqgvariables(false),
  m_isMC(false),
  m_hasNoFJDetectorEta(false),
  m_JetTruthLabelingTool(nullptr),
  m_doCalib(false),
  m_doMassCalib(true),
  m_jetCalibTool(nullptr),
  m_decMuonsInTrackJetLink( "MuonsInTrackJet" ),
  m_acc_ghostMatchedTrackJets("GhostVR30Rmax4Rmin02TrackJet"),
  m_name_ghostMatchedTrackJets("GhostVR30Rmax4Rmin02TrackJet"),
  m_muonSelectionTool(nullptr),
  m_muonCalibrationPeriodTool(nullptr),
  m_calibratedMassDecorator(""),
  m_trackJetPtMin(10000),
  m_trackJetEtaMax(2.5),
  m_trackJetNConst(2),
  m_muonPtMin(10000),
  m_muonEtaMax(2.5),
  m_muonDrMax(0.4),
  m_smearLRPD(false)
{
  using std::placeholders::_1;
  m_selectFcns.clear();
  m_selectFcns.push_back(std::bind(&FatJetHandler::passPreSelection, this, _1));

  setParameters();

  m_config.getif<bool>("matchTruthJet",m_matchTruthJet);

  // set switch for storing Ghost associated heavy flavour hadrons
  m_config.getif<bool>("storeGAParticlesInFatJets", m_storeGAParticlesInFatJets);
  //keep qg variables?
  m_config.getif<bool>("saveqgvariables",m_saveqgvariables);

  //do pseudo-data JER smearing?
  m_config.getif<bool>("smearLRPD", m_smearLRPD);

  // btag time stamp is required from p4xxx derivation
  if ( m_btagTimeStamp!="none" ){
    m_name_ghostMatchedTrackJets+=m_btagTimeStamp;
    Info("FatJetHandler::FatJetHandler()","Combined ghostMatchedTrackJets name: %s", m_name_ghostMatchedTrackJets.c_str());
    SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::IParticleContainer> > > acc_ghostMatchedTrackJets(m_name_ghostMatchedTrackJets);
    m_acc_ghostMatchedTrackJets = acc_ghostMatchedTrackJets;
  }

  // keep element links to track jets ?
  m_config.getif<std::vector<std::string>>("FatJet::TrackLinkNames", m_linkNames);
  if ( m_btagTimeStamp!="none") {
    for (long unsigned int i=0; i<m_linkNames.size(); i++) {
      if (m_linkNames[i]=="GhostVR30Rmax4Rmin02TrackJet" ){
	m_linkNames[i]="GhostVR30Rmax4Rmin02TrackJet"+m_btagTimeStamp;
	Info("FatJetHandler::FatJetHandler()","Combined linkName: %s", m_linkNames[i].c_str());
      }
    }
  }

}


FatJetHandler::~FatJetHandler()
{

}


EL::StatusCode FatJetHandler::initializeTools()
{

  m_hasNoFJDetectorEta = m_eventInfoHandler.get_hasNoFJDetectorEta();
  m_isMC = m_eventInfoHandler.get_isMC();

  m_isLCTOPO = (m_containerName == "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets");
  m_isTCC = (m_containerName == "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets");

  //New precision large-R jet collection
  //https://twiki.cern.ch/twiki/bin/view/AtlasProtected/LargeRPrecisionJets
  //Caveat:
  //No available uncertainty files here: /cvmfs/atlas.cern.ch/repo/sw/database/GroupData/JetUncertainties/CalibArea-08/rel21/Summer2019/  
  //The boosted object taggers of the new UFO jet collection have not been defined yet
  //The truth labels have not been defined yet properly which are needed to derive systematic uncertainties

  m_isUFOVSD = (m_containerName == "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets");

  if (m_isLCTOPO ||  m_isTCC || m_isUFOVSD){
    ////////////////////////////////////////////////////
    //Initialize the calibration tool:
    std::string jetAlg = "AntiKt10LCTopoTrimmedPtFrac5SmallR20";
    std::string confFileMC = "JES_MC16recommendation_FatJet_Trimmed_JMS_comb_17Oct2018.config"; // MC recommendation for comb mass
    std::string confFileData = "JES_MC16recommendation_FatJet_Trimmed_JMS_comb_March2021.config"; // Data recommendation for comb mass
    
    //alternatives for TTC (no in-situ for TTC available)
    std::string jetAlgTCC = "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20";
    std::string confFileTCC = "JES_MC16recommendation_FatJet_TCC_JMS_calo_30Oct2018.config";
    //end alternatives for TTC

    //alternatives for UFO (the in situ claibration is not availaible yet. only FullSim is supported)
    std::string jetAlgUFO = "AntiKt10UFOCSSKSoftDropBeta100Zcut10";
    std::string confFileUFO = "JES_MC16recommendation_R10_UFO_CSSK_SoftDrop_JMS_01April2020.config";

    std::string calibSeqMC = "EtaJES_JMS";
    std::string calibSeqData = "EtaJES_JMS_Insitu_InsituCombinedMass";

    std::string calibName = "FatJetCalibTool";
    //std::string calibArea = "00-04-81";
    m_doCalib = true;

    // increase msg level to error to silence tool when jets are not decorated with 'TrackSumMass'
    // m_msgLevel = MSG::ERROR;

    // calibration tool
    m_jetCalibTool = new JetCalibrationTool(calibName);
    m_jetCalibTool->msg().setLevel(m_msgLevel);

    if( m_isLCTOPO ){
      TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("JetCollection", jetAlg));
      if (m_isMC){
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("ConfigFile", confFileMC));
      }else{
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("ConfigFile", confFileData));
      }
      if(m_isMC){
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("CalibSequence", calibSeqMC));
      }else{
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("CalibSequence", calibSeqData));
      }
    }else if( m_isTCC ){
      //TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("DEVmode", true));
      TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("JetCollection", jetAlgTCC));
      TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("ConfigFile", confFileTCC));
      TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("CalibSequence", calibSeqMC));
    }else if( m_isUFOVSD){ 
      //Caveat: In situ calibration is not available for UFO, i.e. no calib seq for Data
      TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("JetCollection", jetAlgUFO));
      TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("ConfigFile", confFileUFO));
      TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("CalibSequence", calibSeqMC));
    }

    //TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("CalibArea", calibArea)); //no longer needs to be set
    TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetCalibTool->setProperty("IsData", !m_isMC));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_jetCalibTool->initializeTool(calibName));


    ////////////////////////////////////////////////////
    //Initialize the uncertainty tool:
    //Reference: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetUncertaintiesRel21ConsolidatedLargeRScaleRes
    std::string jetUnc_FJ_base("");
    m_config.getif<std::string>("jetUnc_FJ_base", jetUnc_FJ_base);
    std::vector<std::string> jetUnc_FJ_prefixes;
    m_config.getif<std::vector<std::string> >("jetUnc_FJ_prefixes", jetUnc_FJ_prefixes);
    std::vector<std::string> jetUnc_FJ_configs;
    m_config.getif<std::vector<std::string> >("jetUnc_FJ_configs", jetUnc_FJ_configs);

    //sanity checks if everything has been specified in the config file
    if(jetUnc_FJ_base.size()==0 || jetUnc_FJ_prefixes.size()==0 || jetUnc_FJ_configs.size()==0){
      Error("FatJetHandler::initializeTools()","Please specify jetUnc_FJ_base, jetUnc_FJ_prefixes and jetUnc_FJ_configs in the config file");
      return EL::StatusCode::FAILURE;
    }
    if(jetUnc_FJ_prefixes.size()!=jetUnc_FJ_configs.size()){
      Error("FatJetHandler::initializeTools()","The sizes of jetUnc_FJ_prefixes and jetUnc_FJ_configs do not match");
      return EL::StatusCode::FAILURE;
    }

    for (unsigned idx = 0; idx < jetUnc_FJ_configs.size(); ++idx){
      std::string jetUnc_FJ_config = jetUnc_FJ_configs[idx];
      std::string jetUnc_FJ_prefix = jetUnc_FJ_prefixes[idx];
      std::string MCType = "MC16";

      Info("FatJetHandler::initializeTools()","FatJet uncertainty configuration discovered: %s", jetUnc_FJ_config.c_str());

      //m_jetUncTools[jetUnc_FJ_prefix]->msg().setLevel( m_msgLevel ); Corresponds to question above
      if( m_isLCTOPO ){
        m_jetUncTools[jetUnc_FJ_prefix] = new JetUncertaintiesTool(("JetUncProvider_"+jetUnc_FJ_config+jetAlg).c_str());
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncTools[jetUnc_FJ_prefix]->setProperty("JetDefinition", jetAlg ));
      }else if( m_isTCC ){
        m_jetUncTools[jetUnc_FJ_prefix] = new JetUncertaintiesTool(("JetUncProvider_"+jetUnc_FJ_config+jetAlgTCC).c_str());
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncTools[jetUnc_FJ_prefix]->setProperty("JetDefinition", jetAlgTCC ));
      }else if( m_isUFOVSD ){ 
	//no availaible config files for UFO jet, still using TCC jet
        m_jetUncTools[jetUnc_FJ_prefix] = new JetUncertaintiesTool(("JetUncProvider_"+jetUnc_FJ_config+jetAlgTCC).c_str());
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncTools[jetUnc_FJ_prefix]->setProperty("JetDefinition", jetAlgTCC ));
      }
  
      //TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncTools[jetUnc_FJ_prefix]->setProperty("CalibArea", "CalibArea-05" ));
      TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncTools[jetUnc_FJ_prefix]->setProperty("ConfigFile", (jetUnc_FJ_base+jetUnc_FJ_config+".config").c_str()));
      TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncTools[jetUnc_FJ_prefix]->setProperty("MCType", MCType));
      if( m_isLCTOPO ){
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncTools[jetUnc_FJ_prefix]->setProperty("IsData", !m_isMC)); 
      }else{
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncTools[jetUnc_FJ_prefix]->setProperty("IsData", false)); 
      }
      TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncTools[jetUnc_FJ_prefix]->initialize());

      //pseudo data smearing?
      if( m_isLCTOPO && m_smearLRPD && m_isMC){
        m_jetUncToolsPD[jetUnc_FJ_prefix] = new JetUncertaintiesTool(("JetUncProvider_PD_"+jetUnc_FJ_config+jetAlg).c_str());
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncToolsPD[jetUnc_FJ_prefix]->setProperty("JetDefinition", jetAlg ));
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncToolsPD[jetUnc_FJ_prefix]->setProperty("ConfigFile", (jetUnc_FJ_base+jetUnc_FJ_config+".config").c_str()));
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncToolsPD[jetUnc_FJ_prefix]->setProperty("MCType", MCType));
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncToolsPD[jetUnc_FJ_prefix]->setProperty("IsData", true)); //pseudo-data 
	TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetUncToolsPD[jetUnc_FJ_prefix]->initialize());
      }

      std::vector<std::string> pulls = {"__1down", "__1up"};
      CP::SystematicSet jetUnc_sysSet = m_jetUncTools[jetUnc_FJ_prefix]->recommendedSystematics();
      const std::set<std::string> sysNames = jetUnc_sysSet.getBaseNames();
      for (auto sysName: sysNames) {
        for (auto pull : pulls) {
          std::string sysPulled = sysName + pull;
          m_jetUnc_sysSets.insert({jetUnc_FJ_prefix+sysPulled, CP::SystematicSet(sysPulled)});
          m_jetUnc_prefixes.insert({jetUnc_FJ_prefix+sysPulled, jetUnc_FJ_prefix});
        }
      }
    }

    ////////////////////////////////////////////////////
    //Initialize the forward folding jet smearing tool:
    //Reference: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/FFJetSmearingTool
    std::string jetUnc_FJJMR_base("");
    m_config.getif<std::string>("jetUnc_FJJMR_base", jetUnc_FJJMR_base);
    std::vector<std::string> jetUnc_FJJMR_prefixes;
    m_config.getif<std::vector<std::string> >("jetUnc_FJJMR_prefixes", jetUnc_FJJMR_prefixes);
    std::vector<std::string> jetUnc_FJJMR_configs;
    m_config.getif<std::vector<std::string> >("jetUnc_FJJMR_configs", jetUnc_FJJMR_configs);
    std::string jetUnc_JMR_mass = "Comb";
    m_config.getif<std::string>("jetUnc_JMR_mass", jetUnc_JMR_mass); 

    //sanity checks if everything has been specified in the config file
    if(jetUnc_FJJMR_base.size()==0 || jetUnc_FJJMR_prefixes.size()==0 || jetUnc_FJJMR_configs.size()==0){
      Error("FatJetHandler::initializeTools()","Please specify jetUnc_FJJMR_base, jetUnc_FJJMR_prefixes and jetUnc_FJJMR_configs in the config file");
      return EL::StatusCode::FAILURE;
    }
    if(jetUnc_FJJMR_prefixes.size()!=jetUnc_FJJMR_configs.size()){
      Error("FatJetHandler::initializeTools()","The sizes of jetUnc_FJJMR_prefixes and jetUnc_FJJMR_configs do not match");
      return EL::StatusCode::FAILURE;
    }

    for (unsigned idx = 0; idx < jetUnc_FJJMR_configs.size(); ++idx){
      std::string jetUnc_FJJMR_config = jetUnc_FJJMR_configs[idx];
      std::string jetUnc_FJJMR_prefix = jetUnc_FJJMR_prefixes[idx];

      Info("FatJetHandler::initializeTools()","FatJet FF smearing uncertainty configuration discovered: %s", jetUnc_FJJMR_config.c_str());

      m_jetSmearTools[jetUnc_FJJMR_prefix] = new CP::FFJetSmearingTool(("JetFFSmear_"+jetUnc_FJJMR_config+jetAlg).c_str());
      TOOL_CHECK("FatJetHandler()::initializeTools()",  m_jetSmearTools[jetUnc_FJJMR_prefix]->setProperty("MassDef", jetUnc_JMR_mass ));
      TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetSmearTools[jetUnc_FJJMR_prefix]->setProperty("ConfigFile", (jetUnc_FJJMR_base+jetUnc_FJJMR_config+".config").c_str()));
      TOOL_CHECK("FatJetHandler()::initializeTools()", m_jetSmearTools[jetUnc_FJJMR_prefix]->initialize());

      std::vector<std::string> pulls = {"__1down", "__1up"};
      CP::SystematicSet jetUnc_sysSet = m_jetSmearTools[jetUnc_FJJMR_prefix]->recommendedSystematics();
      const std::set<std::string> sysNames = jetUnc_sysSet.getBaseNames();
      for (auto sysName: sysNames) {
	for (auto pull : pulls) {
	  std::string sysPulled = sysName + pull;
	  m_jetUnc_sysSets.insert({jetUnc_FJJMR_prefix+sysPulled, CP::SystematicSet(sysPulled)});
	  m_jetUnc_prefixes.insert({jetUnc_FJJMR_prefix+sysPulled, jetUnc_FJJMR_prefix});
	  m_jetMRUnc_prefixes.insert({jetUnc_FJJMR_prefix+sysPulled, jetUnc_FJJMR_prefix});
	}
      }
    }
    
    ////////////////////////////////////////////////////
    // initialize boson tagging uncertainty tools
    if(m_bosonTag){
      std::string jetTagUnc_FJ_base("");
      m_config.getif<std::string>("jetTagUnc_FJ_base", jetTagUnc_FJ_base);
      std::vector<std::string> jetTagUnc_FJ_prefixes;
      m_config.getif<std::vector<std::string> >("jetTagUnc_FJ_prefixes", jetTagUnc_FJ_prefixes);
      std::vector<std::string> jetTagUnc_FJ_configs;
      m_config.getif<std::vector<std::string> >("jetTagUnc_FJ_configs", jetTagUnc_FJ_configs);

      //sanity checks if everything has been specified in the config file
      if(jetTagUnc_FJ_base.size()==0 || jetTagUnc_FJ_prefixes.size()==0 || jetTagUnc_FJ_configs.size()==0){
	Error("FatJetHandler::initializeTools()","Please specify jetTagUnc_FJ_base, jetTagUnc_FJ_prefixes and jetTagUnc_FJ_configs in the config file");
	return EL::StatusCode::FAILURE;
      }
      if(jetTagUnc_FJ_prefixes.size()!=jetTagUnc_FJ_configs.size()){
	Error("FatJetHandler::initializeTools()","The sizes of jetTagUnc_FJ_prefixes and jetTagUnc_FJ_configs do not match");
	return EL::StatusCode::FAILURE;
      }
      //outsourced initialize function, because it's very lengthy
      EL_CHECK("FatJetHandler::initializeBosonTagUncertaintyTools()", initializeBosonTagUncertaintyTools(jetTagUnc_FJ_base, jetTagUnc_FJ_configs.at(0), jetTagUnc_FJ_prefixes.at(0), jetAlg));
    }  
  }else {
    Warning("initializeTools()", "FatJets not any of them: AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets, AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets, AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets. No Calibration available");
    m_doCalib = false;
  }

  ////////////////////////////////////////////////////
  //Initialize the truth labelling tool
  //Under investigation for UFO
  m_JetTruthLabelingTool = std::unique_ptr<JetTruthLabelingTool>( new JetTruthLabelingTool( "JetTruthLabelingTool" ) );

  TOOL_CHECK("FatJetHandler::initializeTools()", m_JetTruthLabelingTool->setProperty("TruthLabelName",  "R10TruthLabel_R21Consolidated"));
  TOOL_CHECK("FatJetHandler::initializeTools()", m_JetTruthLabelingTool->setProperty("UseTRUTH3",  true));

  // The name of the truth Boson container changed over time (approx. from _p4XXX)
  // "TruthBosonWithDecayParticles" -> "TruthBosonsWithDecayParticles";
  // Check which one is in the event and use that name
  if ( m_event->contains<xAOD::TruthParticleContainer>("TruthBosonWithDecayParticles") ) {
    TOOL_CHECK("FatJetHandler::initializeTools()", m_JetTruthLabelingTool->setProperty("TruthBosonContainerName", "TruthBosonWithDecayParticles"));
  } else if ( m_event->contains<xAOD::TruthParticleContainer>("TruthBosonsWithDecayParticles") ) {
    TOOL_CHECK("FatJetHandler::initializeTools()", m_JetTruthLabelingTool->setProperty("TruthBosonContainerName", "TruthBosonsWithDecayParticles"));
  } else {    
    std::cout << "Event does not contain the TruthBosons" << std::endl;
    Warning("FatJetHandler::initializeTools()","The truth boson container is not found");
  }

  if ( m_event->contains<xAOD::TruthParticleContainer>("TruthTopQuarkWithDecayParticles") ) {
    TOOL_CHECK("FatJetHandler::initializeTools()", m_JetTruthLabelingTool->setProperty("TruthTopQuarkContainerName", "TruthTopQuarkWithDecayParticles"));
  } else if ( m_event->contains<xAOD::TruthParticleContainer>("TruthTop") ){
    TOOL_CHECK("FatJetHandler::initializeTools()", m_JetTruthLabelingTool->setProperty("TruthTopQuarkContainerName", "TruthTop"));
  } else {
    std::cout << "Event does not contain the TruthTop" << std::endl;
    Warning("FatJetHandler::initializeTools()","The truth top container is not found");
  }

  TOOL_CHECK("FatJetHandler::initializeTools()", m_JetTruthLabelingTool->initialize());

  ////////////////////////////////////////////////////
  //Initialize the taggers:

  //W/Z taggers:
  m_smoothedWTagger50 = nullptr;
  m_smoothedWTagger80 = nullptr;
  m_smoothedZTagger50 = nullptr;
  m_smoothedZTagger80 = nullptr;
  if ( m_isLCTOPO ) {
    m_smoothedWTagger50 = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "SmoothWTagger50" ) );
    m_smoothedWTagger80 = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "SmoothWTagger80" ) );
    m_smoothedZTagger50 = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "SmoothZTagger50" ) );
    m_smoothedZTagger80 = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "SmoothZTagger80" ) );
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedWTagger50->setProperty( "CalibArea",  "SmoothedWZTaggers/Rel21"));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedWTagger80->setProperty( "CalibArea",  "SmoothedWZTaggers/Rel21"));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedZTagger50->setProperty( "CalibArea",  "SmoothedWZTaggers/Rel21"));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedZTagger80->setProperty( "CalibArea",  "SmoothedWZTaggers/Rel21"));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedWTagger50->setProperty( "ConfigFile", "SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16_20201216.dat"));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedWTagger80->setProperty( "ConfigFile", "SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC16_20201216.dat"));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedZTagger50->setProperty( "ConfigFile", "SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC16_20201216.dat"));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedZTagger80->setProperty( "ConfigFile", "SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC16_20201216.dat"));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedWTagger50->setProperty("DecorateJet",true));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedWTagger80->setProperty("DecorateJet",true));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedZTagger50->setProperty("DecorateJet",true));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedZTagger80->setProperty("DecorateJet",true));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedWTagger50->setProperty("IsMC", m_isMC));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedWTagger80->setProperty("IsMC", m_isMC));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedZTagger50->setProperty("IsMC", m_isMC));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedZTagger80->setProperty("IsMC", m_isMC));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedWTagger50->initialize());
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedWTagger80->initialize());
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedZTagger50->initialize());
    TOOL_CHECK("FatJetHandler::initializeTools()", m_smoothedZTagger80->initialize());
  }
  if ( m_isTCC ) {
    m_TCC_WTagger_2var = nullptr;
    m_TCC_WTagger_3var = nullptr;

    m_TCC_ZTagger_2var = nullptr;
    m_TCC_ZTagger_3var = nullptr;

    m_TCC_WTagger_2var = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "TCC_WTagger_2var" ) );
    m_TCC_WTagger_3var = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "TCC_WTagger_3var" ) );

    m_TCC_ZTagger_2var = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "TCC_ZTagger_2var" ) );
    m_TCC_ZTagger_3var = std::unique_ptr<SmoothedWZTagger>( new SmoothedWZTagger( "TCC_ZTagger_3var" ) );

    TOOL_CHECK("FatJetHandler::initializeTools()", m_TCC_WTagger_2var->setProperty( "ConfigFile", "SmoothedWZTaggers/Rel21/SmoothedWTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_2Var_MC16d_20190410.dat"));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_TCC_WTagger_3var->setProperty( "ConfigFile", "SmoothedWZTaggers/Rel21/SmoothedContainedWTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_3Var_MC16d_20190410.dat"));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_TCC_ZTagger_2var->setProperty( "ConfigFile", "SmoothedWZTaggers/Rel21/SmoothedZTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_2Var_MC16d_20190410.dat"));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_TCC_ZTagger_3var->setProperty( "ConfigFile", "SmoothedWZTaggers/Rel21/SmoothedContainedZTagger_AntiKt10TrackCaloClusterTrimmed_MaxSignificance_3Var_MC16d_20190410.dat"));

    TOOL_CHECK("FatJetHandler::initializeTools()", m_TCC_WTagger_2var->setProperty("DecorateJet",true));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_TCC_WTagger_3var->setProperty("DecorateJet",true));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_TCC_ZTagger_2var->setProperty("DecorateJet",true));
    TOOL_CHECK("FatJetHandler::initializeTools()", m_TCC_ZTagger_3var->setProperty("DecorateJet",true));
    
    TOOL_CHECK("FatJetHandler::initializeTools()", m_TCC_WTagger_2var->initialize());
    TOOL_CHECK("FatJetHandler::initializeTools()", m_TCC_WTagger_3var->initialize());
    TOOL_CHECK("FatJetHandler::initializeTools()", m_TCC_ZTagger_2var->initialize());
    TOOL_CHECK("FatJetHandler::initializeTools()", m_TCC_ZTagger_3var->initialize());
  }

  //register ISystematicsTools
  ////////////////////////////////
  m_sysToolList.clear();
  if (m_doCalib){
    for (const auto& kv: m_jetUncTools){
      m_sysToolList.push_back(kv.second);
    }
    for (const auto& kv: m_jetUncToolsPD){
      m_sysToolList.push_back(kv.second);
    }
    for (const auto& kv: m_jetSmearTools){
      m_sysToolList.push_back(kv.second);
    }
    if(m_bosonTag) {
      m_sysToolList.push_back(m_smoothedWTagger50_uncTool.get());
      m_sysToolList.push_back(m_smoothedWTagger80_uncTool.get());
      m_sysToolList.push_back(m_smoothedZTagger50_uncTool.get());
      m_sysToolList.push_back(m_smoothedZTagger80_uncTool.get());
    }
  }

  //for Hbb muon-in-jet correction
  m_muonSelectionTool = new CP::MuonSelectionTool("MuonSelectionToolForHbb");
  m_muonSelectionTool->msg().setLevel(m_msgLevel);
  TOOL_CHECK("FatjetHandler::initializeTools()",m_muonSelectionTool->initialize());
  m_muonCalibrationPeriodTool = new CP::MuonCalibrationPeriodTool("MuonCalibrationPeriodToolForHbb");
  m_muonCalibrationPeriodTool->msg().setLevel(m_msgLevel);
  TOOL_CHECK("FatjetHandler::initializeTools()",m_muonCalibrationPeriodTool->initialize());
  m_decMuonsInTrackJetLink = SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::IParticleContainer>>>("MounsInTrackJet");
  m_calibratedMassDecorator = m_isMC?"JetJMSScaleMomentum":"JetInsituScaleMomentum";

  m_HbbTagConfig202006 = new FlavorTagDiscriminants::HbbTagConfig( "BTagging/202006/Xbb/GhostVR30Rmax4Rmin02TrackJet_BTagging201903/network.json" );
  m_HbbTagConfig2020v3 = new FlavorTagDiscriminants::HbbTagConfig( "dev/BTagging/2020v3/Xbb/GhostVR30Rmax4Rmin02TrackJet_BTagging201903/network.json" );
  m_HbbTagger202006 = new FlavorTagDiscriminants::HbbTag( *m_HbbTagConfig202006 );
  m_HbbTagger2020v3 = new FlavorTagDiscriminants::HbbTag( *m_HbbTagConfig2020v3 );

  return EL::StatusCode::SUCCESS;

}

void FatJetHandler::setParameters()
{
  //Setup default cut parameters
  m_bosonTag = false;

  m_ptcut = 250000;
  m_etacut = 2.5;

  //Get setting for decoration
  m_config.getif<bool>("FatJet::DoBosonTag", m_bosonTag);
  m_config.getif<bool>("FatJet::DoMassCalib", m_doMassCalib);
  m_config.getif<float>("FatJet::Pt", m_ptcut);
  m_config.getif<float>("FatJet::Eta", m_etacut);

  return;
}

bool FatJetHandler::passPreSelection(xAOD::Jet* jet)
{
  //Check the jet passes required eta and pt cuts
  bool passCut = true;

  if (fabs(jet->pt()) < m_ptcut) {passCut = false;}
  else { m_cutflow->count("pT cut"); }

  if (fabs(jet->eta()) > m_etacut) {passCut = false;}
  else { m_cutflow->count("eta cut"); }

  Props::passPreSel.set(jet, passCut);

  return passCut;
}

void FatJetHandler::doVTag(xAOD::Jet* jet)
{
  if ( m_isLCTOPO) {
    auto result_WTag50 = m_smoothedWTagger50->tag(*jet);
    auto result_WTag80 = m_smoothedWTagger80->tag(*jet);
    auto result_ZTag50 = m_smoothedZTagger50->tag(*jet);
    auto result_ZTag80 = m_smoothedZTagger80->tag(*jet);

    std::string selectionName = "";
    m_config.getif<std::string>("selectionName", selectionName);

    bool WTag50_PassMassLow = result_WTag50.getCutResult("PassMassLow");
    bool ZTag50_PassMassLow = result_ZTag50.getCutResult("PassMassLow");
    bool WTag80_PassMassLow = result_WTag80.getCutResult("PassMassLow");
    bool ZTag80_PassMassLow = result_ZTag80.getCutResult("PassMassLow");

    bool WTag50_PassMassHigh = result_WTag50.getCutResult("PassMassHigh");
    bool ZTag50_PassMassHigh = result_ZTag50.getCutResult("PassMassHigh");
    bool WTag80_PassMassHigh = result_WTag80.getCutResult("PassMassHigh");
    bool ZTag80_PassMassHigh = result_ZTag80.getCutResult("PassMassHigh");
    
    bool WTag50_PassMass = (result_WTag50.getCutResult("PassMassLow") & result_WTag50.getCutResult("PassMassHigh"));
    bool ZTag50_PassMass = (result_ZTag50.getCutResult("PassMassLow") & result_ZTag50.getCutResult("PassMassHigh"));
    bool WTag80_PassMass = (result_WTag80.getCutResult("PassMassLow") & result_WTag80.getCutResult("PassMassHigh"));
    bool ZTag80_PassMass = (result_ZTag80.getCutResult("PassMassLow") & result_ZTag80.getCutResult("PassMassHigh"));
    
    bool WTag50_PassD2 = result_WTag50.getCutResult("PassD2");
    bool ZTag50_PassD2 = result_ZTag50.getCutResult("PassD2");
    bool WTag80_PassD2 = result_WTag80.getCutResult("PassD2");
    bool ZTag80_PassD2 = result_ZTag80.getCutResult("PassD2");
    
    bool WTag50_PassNtrk = result_WTag50.getCutResult("PassNtrk");
    bool ZTag50_PassNtrk = result_ZTag50.getCutResult("PassNtrk");
    bool WTag80_PassNtrk = result_WTag80.getCutResult("PassNtrk");
    bool ZTag80_PassNtrk = result_ZTag80.getCutResult("PassNtrk");
    
    bool WTag50_PassAll = WTag50_PassMass & WTag50_PassD2 & WTag50_PassNtrk;
    bool ZTag50_PassAll = ZTag50_PassMass & ZTag50_PassD2 & ZTag50_PassNtrk;
    bool WTag80_PassAll = WTag80_PassMass & WTag80_PassD2 & WTag80_PassNtrk;
    bool ZTag80_PassAll = ZTag80_PassMass & ZTag80_PassD2 & ZTag80_PassNtrk;

    if ( WTag50_PassMass ) m_cutflow->count("WMassCut");
    if ( ZTag50_PassMass ) m_cutflow->count("ZMassCut");
    if ( WTag50_PassD2   ) m_cutflow->count("WSubCut");
    if ( ZTag50_PassD2   ) m_cutflow->count("ZSubCut");
    if ( WTag50_PassNtrk ) m_cutflow->count("WNtrkCut");
    if ( ZTag50_PassNtrk ) m_cutflow->count("ZNtrkCut");
    if ( WTag50_PassAll  ) m_cutflow->count("isWJet");
    if ( ZTag50_PassAll  ) m_cutflow->count("isZJet");
    //if(passMassCut(bin, jet, m_HMassShift)) { m_cutflow->count("HMassCut"); }

    Props::passWMassCut50.set(jet, WTag50_PassMass );
    Props::passWMassCut80.set(jet, WTag80_PassMass );
    Props::passZMassCut50.set(jet, ZTag50_PassMass ) ;
    Props::passZMassCut80.set(jet, ZTag80_PassMass ) ;

    Props::passWLowMassCut50.set(jet, WTag50_PassMassLow );
    Props::passWLowMassCut80.set(jet, WTag80_PassMassLow );
    Props::passZLowMassCut50.set(jet, ZTag50_PassMassLow );
    Props::passZLowMassCut80.set(jet, ZTag80_PassMassLow );
    
    Props::passWHighMassCut50.set(jet, WTag50_PassMassHigh );
    Props::passWHighMassCut80.set(jet, WTag80_PassMassHigh );
    Props::passZHighMassCut50.set(jet, ZTag50_PassMassHigh );
    Props::passZHighMassCut80.set(jet, ZTag80_PassMassHigh );

    Props::passWSubstructure50.set(jet, WTag50_PassD2 );
    Props::passWSubstructure80.set(jet, WTag80_PassD2 );
    Props::passZSubstructure50.set(jet, ZTag50_PassD2 );
    Props::passZSubstructure80.set(jet, ZTag80_PassD2 );

    Props::passWNtrk50.set(jet, WTag50_PassNtrk );
    Props::passWNtrk80.set(jet, WTag80_PassNtrk );
    Props::passZNtrk50.set(jet, ZTag50_PassNtrk );
    Props::passZNtrk80.set(jet, ZTag80_PassNtrk );

    Props::isWJet50.set(jet, WTag50_PassAll );
    Props::isWJet80.set(jet, WTag80_PassAll );
    Props::isZJet50.set(jet, ZTag50_PassAll );
    Props::isZJet80.set(jet, ZTag80_PassAll );

  }

  if ( m_isTCC ) {
    auto result_TCC_WTag_2var= m_TCC_WTagger_2var->tag(*jet);
    auto result_TCC_WTag_3var= m_TCC_WTagger_3var->tag(*jet);
    auto result_TCC_ZTag_2var= m_TCC_ZTagger_2var->tag(*jet);
    auto result_TCC_ZTag_3var= m_TCC_ZTagger_3var->tag(*jet);
    
    bool WTag_TCC_2var_PassMassLow = result_TCC_WTag_2var.getCutResult("PassMassLow");
    bool WTag_TCC_3var_PassMassLow = result_TCC_WTag_3var.getCutResult("PassMassLow");
    bool ZTag_TCC_2var_PassMassLow = result_TCC_ZTag_2var.getCutResult("PassMassLow");
    bool ZTag_TCC_3var_PassMassLow = result_TCC_ZTag_3var.getCutResult("PassMassLow");

    bool WTag_TCC_2var_PassMassHigh = result_TCC_WTag_2var.getCutResult("PassMassHigh");
    bool WTag_TCC_3var_PassMassHigh = result_TCC_WTag_3var.getCutResult("PassMassHigh");
    bool ZTag_TCC_2var_PassMassHigh = result_TCC_ZTag_2var.getCutResult("PassMassHigh");
    bool ZTag_TCC_3var_PassMassHigh = result_TCC_ZTag_3var.getCutResult("PassMassHigh");

    bool WTag_TCC_2var_PassMass = (result_TCC_WTag_2var.getCutResult("PassMassLow") & result_TCC_WTag_2var.getCutResult("PassMassHigh"));
    bool WTag_TCC_3var_PassMass = (result_TCC_WTag_3var.getCutResult("PassMassLow") & result_TCC_WTag_3var.getCutResult("PassMassHigh"));
    bool ZTag_TCC_2var_PassMass = (result_TCC_ZTag_2var.getCutResult("PassMassLow") & result_TCC_ZTag_2var.getCutResult("PassMassHigh"));
    bool ZTag_TCC_3var_PassMass = (result_TCC_ZTag_3var.getCutResult("PassMassLow") & result_TCC_ZTag_3var.getCutResult("PassMassHigh"));
    
    bool WTag_TCC_2var_PassD2 = result_TCC_WTag_2var.getCutResult("PassD2");
    bool WTag_TCC_3var_PassD2 = result_TCC_WTag_3var.getCutResult("PassD2");
    bool ZTag_TCC_2var_PassD2 = result_TCC_ZTag_2var.getCutResult("PassD2");
    bool ZTag_TCC_3var_PassD2 = result_TCC_ZTag_3var.getCutResult("PassD2");

    bool WTag_TCC_3var_PassNtrk =  result_TCC_WTag_3var.getCutResult("PassNtrk");
    bool ZTag_TCC_3var_PassNtrk =  result_TCC_ZTag_3var.getCutResult("PassNtrk");
    
    bool WTag_TCC_2var_all = WTag_TCC_2var_PassMass & WTag_TCC_2var_PassD2;
    bool WTag_TCC_3var_all = WTag_TCC_3var_PassMass & WTag_TCC_3var_PassD2 & WTag_TCC_3var_PassNtrk;
    bool ZTag_TCC_2var_all = ZTag_TCC_2var_PassMass & ZTag_TCC_2var_PassD2;
    bool ZTag_TCC_3var_all = ZTag_TCC_3var_PassMass & ZTag_TCC_3var_PassD2 & ZTag_TCC_3var_PassNtrk;

    Props::passWMassCut_TCC_2var.set(jet, WTag_TCC_2var_PassMass);
    Props::passWMassCut_TCC_3var.set(jet, WTag_TCC_3var_PassMass);
    Props::passZMassCut_TCC_2var.set(jet, ZTag_TCC_2var_PassMass);
    Props::passZMassCut_TCC_3var.set(jet, ZTag_TCC_3var_PassMass);
    
    Props::passWHighMassCut_TCC_2var.set(jet, WTag_TCC_2var_PassMassHigh);
    Props::passWHighMassCut_TCC_3var.set(jet, WTag_TCC_3var_PassMassHigh);
    Props::passZHighMassCut_TCC_2var.set(jet, ZTag_TCC_2var_PassMassHigh);
    Props::passZHighMassCut_TCC_3var.set(jet, ZTag_TCC_3var_PassMassHigh);
    
    Props::passWLowMassCut_TCC_2var.set(jet, WTag_TCC_2var_PassMassLow);
    Props::passWLowMassCut_TCC_3var.set(jet, WTag_TCC_3var_PassMassLow);
    Props::passZLowMassCut_TCC_2var.set(jet, ZTag_TCC_2var_PassMassLow);
    Props::passZLowMassCut_TCC_3var.set(jet, ZTag_TCC_3var_PassMassLow);

    Props::passWSubstructure_TCC_2var.set(jet, WTag_TCC_2var_PassD2);
    Props::passWSubstructure_TCC_3var.set(jet, WTag_TCC_3var_PassD2);
    Props::passZSubstructure_TCC_2var.set(jet, ZTag_TCC_2var_PassD2);
    Props::passZSubstructure_TCC_3var.set(jet, ZTag_TCC_3var_PassD2);
    
    Props::passWNtrk_TCC_3var.set(jet, WTag_TCC_3var_PassNtrk);
    Props::passZNtrk_TCC_3var.set(jet, ZTag_TCC_3var_PassNtrk);
    
    Props::isWJet_TCC_2var.set(jet, WTag_TCC_2var_all);
    Props::isWJet_TCC_3var.set(jet, WTag_TCC_3var_all);
    Props::isZJet_TCC_2var.set(jet, ZTag_TCC_2var_all);
    Props::isZJet_TCC_3var.set(jet, ZTag_TCC_3var_all);

  }
  return;
}


EL::StatusCode FatJetHandler::addCPVariations(const std::vector<TString> &variations,
    const bool filterByTools, const bool /*skipWeightVar*/) {
      
  // add the usual variations, filtered by registered tools if requested
  EL_CHECK("FatJetHandler::addCPVariations()", ObjectHandlerBase::addCPVariations(variations, filterByTools));
  // see if we want some additional ones
  std::vector<TString> addVariations;
  // for (TString variation : variations) {
  //   bool allow = false;
  //   allow |= variation.EqualTo("FATJET_JER__1up");
  //   allow |= variation.EqualTo("FATJET_JMR__1up");
  //   allow |= variation.EqualTo("FATJET_SubR__1up");
  //   if (!allow) continue;
  //   addVariations.push_back(variation);
  // }
  // add them to the list w/o further filtering
  return ObjectHandlerBase::addCPVariations(addVariations, false);

}


EL::StatusCode FatJetHandler::decorateOriginParticle(const xAOD::Jet * jet)
{

  // substructure moments: n-subjettiness
  float tau1(-1.), tau2(-1.), tau3(-1.);
  float tau21(-1.), tau32(-1.);

  jet->getAttribute("Tau1_wta", tau1);
  jet->getAttribute("Tau2_wta", tau2);
  jet->getAttribute("Tau3_wta", tau3);

  if (tau2 > 0.) tau21 = tau2 / tau1;
  if (tau3 > 0.) tau32 = tau3 / tau2;

  Props::Tau21.set(jet, tau21);
  Props::Tau32.set(jet, tau32);

  // D2Beta1
  const float& ecf1 = jet->getAttribute<float>("ECF1");
  const float& ecf2 = jet->getAttribute<float>("ECF2");
  const float& ecf3 = jet->getAttribute<float>("ECF3");

  float d2 = 0;
  float c2 = 0;
  if (fabs(ecf2) > 0) {
    d2 = ecf3 * pow(ecf1, 3) / pow(ecf2, 3);
    c2 = ecf3 * ecf1 / pow(ecf2, 2);
  }
  Props::D2.set(jet, d2);
  Props::C2.set(jet, c2);

  Props::y.set(jet, jet->rapidity());
  Props::nConstituents.set(jet, jet->auxdata<std::vector<ElementLink<DataVector<xAOD::IParticle>>>>("constituentLinks").size());
  try {
    Props::nMuonSegments.set(jet, GhostMuonSegmentCount(*jet));
  } catch (...) {
    Props::nMuonSegments.set(jet, 0);
  }
  try {
    Props::nGroomedTracks.set(jet, GhostTrackCount(*jet));
  } catch (...) {
    Props::nGroomedTracks.set(jet, 0);
  }

  static SG::AuxElement::Accessor<ElementLink<xAOD::JetContainer> > ungroomedLink("Parent");
  // check if track has link to truth particle and set truth particle pointer
  const xAOD::Jet * ungroomedJet = 0;
  if( ungroomedLink.isAvailable( *jet ) ) {
    ElementLink<xAOD::JetContainer> linkToUngroomed = ungroomedLink( *jet );
    if (  linkToUngroomed.isValid() ) ungroomedJet = *linkToUngroomed;
    std::vector<int> NTrkPt500, NTrkPt1000;
    std::vector<float> TrkWidthPt500, TrkWidthPt1000, SumPtTrkPt500, SumPtTrkPt1000;
    ungroomedJet->getAttribute(xAOD::JetAttribute::NumTrkPt500, NTrkPt500);
    if (NTrkPt500.size()==0) {
      Props::NumTrkPt500PV.set(jet, -99.9);
    } else {
      Props::NumTrkPt500PV.set(jet, NTrkPt500.at(0));
    }
    ungroomedJet->getAttribute(xAOD::JetAttribute::NumTrkPt1000, NTrkPt1000);
    if (NTrkPt1000.size()==0) {
      Props::NumTrkPt1000PV.set(jet, -99.9);
    } else {
      Props::NumTrkPt1000PV.set(jet, NTrkPt1000.at(0));
    }
    ungroomedJet->getAttribute(xAOD::JetAttribute::TrackWidthPt500, TrkWidthPt500);
    if (TrkWidthPt500.size()==0) {
      Props::UngroomedTrackWidth500.set(jet, -99.9);
    } else {
      Props::UngroomedTrackWidth500.set(jet, TrkWidthPt500.at(0));
    }
    ungroomedJet->getAttribute(xAOD::JetAttribute::TrackWidthPt1000, TrkWidthPt1000);
    if (TrkWidthPt1000.size()==0) {
      Props::UngroomedTrackWidth1000.set(jet, -99.9);
    } else {
      Props::UngroomedTrackWidth1000.set(jet, TrkWidthPt1000.at(0));
    }
    ungroomedJet->getAttribute(xAOD::JetAttribute::SumPtTrkPt500, SumPtTrkPt500);
    if (SumPtTrkPt500.size()==0) {
      Props::UngroomedSumTrackPt500.set(jet, -99.9);
    } else {
      Props::UngroomedSumTrackPt500.set(jet, SumPtTrkPt500.at(0));
    }
    ungroomedJet->getAttribute(xAOD::JetAttribute::SumPtTrkPt1000, SumPtTrkPt1000);
    if(SumPtTrkPt1000.size()==0){
      Props::UngroomedSumTrackPt1000.set(jet, -99.9);
    } else {
      Props::UngroomedSumTrackPt1000.set(jet, SumPtTrkPt1000.at(0));
    }
  }

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode FatJetHandler::decorate(xAOD::Jet * jet)
{

  if( m_isLCTOPO ){
    static SG::AuxElement::Accessor<float> caloPtAcc (m_calibratedMassDecorator+"Calo_pt");
    if ( caloPtAcc.isAvailable(*jet) ) {
      Props::caloPt.set(jet, jet->getAttribute<float>(m_calibratedMassDecorator+"Calo_pt"));
    }
    static SG::AuxElement::Accessor<float> caloMassAcc(m_calibratedMassDecorator+"Calo_m");
    if ( caloMassAcc.isAvailable(*jet) ) {
      Props::caloMass.set(jet, jet->getAttribute<float>(m_calibratedMassDecorator+"Calo_m"));
    }
    static SG::AuxElement::Accessor<float> TAmassAcc(m_calibratedMassDecorator+"TA_m");
    if ( TAmassAcc.isAvailable(*jet) ) {
      Props::TAmass.set(jet, jet->getAttribute<float>(m_calibratedMassDecorator+"TA_m"));
    }
  }

  //hbb muon-in-jet correction
  if( m_isLCTOPO ){
    TLorentzVector correctedVectorVR;
    TLorentzVector correctionVectorVR;
    if ( getHbbCorrectedVector(*jet, correctedVectorVR) != EL::StatusCode::SUCCESS ){
      Error("FatJetHandler::decorate","getHbbCorrectedVector() failed");
      return EL::StatusCode::FAILURE;
    }
    correctionVectorVR = correctedVectorVR - jet->p4();
    if( std::isnan(correctionVectorVR.Pt()) || std::isnan(correctionVectorVR.E()) ){
      Warning("FatJetHandler::decorate", "correctionVectorVR is NaN. Setting output default 0 vector");
      correctionVectorVR = TLorentzVector();
    }
    jet->setJetP4("hbbVR", xAOD::JetFourMom_t(correctionVectorVR.Pt(), correctionVectorVR.Eta(), correctionVectorVR.Phi(), correctionVectorVR.M()));
  }

  //HbbTool
  float xbb202006_higgs = -1.0;
  float xbb202006_qcd = -1.0;
  float xbb202006_top = -1.0;
  float xbb2020v3_higgs = -1.0;
  float xbb2020v3_qcd = -1.0;
  float xbb2020v3_top = -1.0;

  bool hbbTagged = true;

  try{
       m_HbbTagger202006 -> decorate( *jet );
       m_HbbTagger2020v3 -> decorate( *jet );
  }

  catch (const std::exception& e) {
    Error("FatJetHandler_VHbb::decorate()","HbbTag decoration failed!");
    hbbTagged = false;
  }

 if(hbbTagged){
    xbb202006_higgs = jet-> getAttribute<float>("Xbb202006_Higgs") ;
    xbb202006_top = jet-> getAttribute<float>("Xbb202006_Top") ;
    xbb202006_qcd = jet-> getAttribute<float>("Xbb202006_QCD") ;
    xbb2020v3_higgs = jet-> getAttribute<float>("Xbb2020v3_Higgs") ;
    xbb2020v3_top = jet-> getAttribute<float>("Xbb2020v3_Top") ;
    xbb2020v3_qcd = jet-> getAttribute<float>("Xbb2020v3_QCD") ;
 }
  Props::Xbb202006_Higgs.set(jet, xbb202006_higgs);
  Props::Xbb202006_Top.set(jet, xbb202006_top);
  Props::Xbb202006_QCD.set(jet, xbb202006_qcd);
  Props::Xbb2020v3_Higgs.set(jet, xbb2020v3_higgs);
  Props::Xbb2020v3_Top.set(jet, xbb2020v3_top);
  Props::Xbb2020v3_QCD.set(jet, xbb2020v3_qcd);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode FatJetHandler::fillOutputLinks()
{
  if(m_linkNames.size() == 0) { return EL::StatusCode::SUCCESS; }
  if(m_outContainer.size() == 0) { return EL::StatusCode::SUCCESS; }

  // always use the nominal as the input container
  const auto& inCont = *m_inContainer["Nominal"];

  // don't iterator over output container, only write to nominal
  {
    auto& outCont = *m_outContainer["Nominal"];

    unsigned int k = 0;
    for(unsigned int j = 0; j < inCont.size(); ++j) {
      const ElementLink<xAOD::JetContainer> &el = inCont.at(j)->auxdata<ElementLink<xAOD::JetContainer> >("Parent");
      if (!(el.isValid())) continue;
      bool passAnyVar = passAny(m_inContainer, j, Props::passSel);
      if (passAnyVar) {
         for(auto& linkName : m_linkNames) {
	   m_linker->copyParticleLinks(**el, *(outCont.at(k)), linkName, m_handlerName);
	 }
	 ++k;
	 if (k == outCont.size()) break;
      }
    }
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode FatJetHandler::flagOutputLinks()
{
  if(m_linkNames.size() == 0) { return EL::StatusCode::SUCCESS; }

  // always use the nominal as the input container
  const auto& inCont = *m_inContainer["Nominal"];

  for(unsigned int j = 0; j < inCont.size(); ++j) {
    const ElementLink<xAOD::JetContainer> &el = inCont.at(j)->auxdata<ElementLink<xAOD::JetContainer> >("Parent");
    if (!(el.isValid())) continue;
    bool passAnyVar = passAny(m_inContainer, j, Props::passSel);
    if (passAnyVar) {
      for(auto& linkName : m_linkNames) {
	m_linker->flagParticleLinks(**el, linkName);
      }
    }
  }

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode FatJetHandler::matchTruthJet(JetSemileptonic* m_jetSemileptonic, const xAOD::JetContainer* truthjets, const std::string& name, double DRCut) {

  if (!m_matchTruthJet) { return EL::StatusCode::SUCCESS;}
  // loop over all jet variations
  for(auto& x: *getInParticles()) { //std::map<TString, partContainer*>* getInParticles();
    //const TString& variation=x.first;
    //loop over the jets in the container belonging to this variation
    for (xAOD::Jet * jet : *x.second) {
      unsigned counter=0;
      unsigned index_smallestDR=99;
      double smallestDR=99.9;
      const xAOD::Jet* truthjetbest=nullptr;
      //loop over all truth jets to do the dR matching
      for (unsigned i=0; i!=truthjets->size(); i++) {
        const xAOD::Jet* truthjet = (*truthjets)[i];
        double DR=jet->p4().DeltaR(truthjet->p4());
        if(!(DR<DRCut)) continue;
        counter++;
        if(DR<smallestDR) {
          index_smallestDR=i;
          smallestDR=DR;
        }
      }//end loop over truth jets
      TLorentzVector truthjettlv;
      if (counter>0) {
        //we will set the real value of truthjettlv
        truthjetbest = (*truthjets)[index_smallestDR];
        truthjettlv.SetPtEtaPhiE(truthjetbest->pt(),truthjetbest->eta(),truthjetbest->phi(),truthjetbest->e());
      }//end if counter>0
      //if not it remains with its dummy value
      //add as Truth or TruthWZ 4-vector as signal state of the nominal je
      EL_CHECK("FatJetHandler::matchTruthJet",m_jetSemileptonic->setJetTLV(jet,name,truthjettlv));

    }//end loop over jets in the container for this variation
  }// end loop over variations
  return EL::StatusCode::SUCCESS;
}

double FatJetHandler::getSmearFactor(double sigma, long int seed) {
  // Set the seed
  m_rand.SetSeed(seed);
  // Calculate the smearing factor
  double smear = -1;
  while (smear <= 0) {
    // at least 50% change to get smear > 0 => runtime ok
    smear = m_rand.Gaus(1.0, sigma);
  }
  return smear;
}

CP::CorrectionCode FatJetHandler::applySmearingJER(xAOD::Jet& jet) {
  // seed similar to JERSmearingTool
  long int seed = 1.1e+5 * std::abs(jet.phi());
  // smearing by 2%
  double smear = getSmearFactor(0.02, seed);
  // energy smearing: scale pt only
  xAOD::JetFourMom_t p4 = jet.jetP4();
  jet.setJetP4(xAOD::JetFourMom_t(smear * p4.Pt(), p4.Eta(), p4.Phi(), p4.M()));
  return CP::CorrectionCode::Ok;
}

CP::CorrectionCode FatJetHandler::applySmearingJMR(xAOD::Jet& jet) {
  // seed similar to JERSmearingTool
  long int seed = 1.2e+5 * std::abs(jet.phi());
  // smearing by 20%
  double smear = getSmearFactor(0.20, seed);
  // mass smearing: scale mass only
  xAOD::JetFourMom_t p4 = jet.jetP4();
  jet.setJetP4(xAOD::JetFourMom_t(p4.Pt(), p4.Eta(), p4.Phi(), smear * p4.M()));
  return CP::CorrectionCode::Ok;
}

CP::CorrectionCode FatJetHandler::applySmearingSubR(xAOD::Jet& jet) {
  // seed similar to JERSmearingTool
  long int seed = 1.3e+5 * std::abs(jet.phi());
  // smearing by 15%
  double smear = getSmearFactor(0.15, seed);
  // D2 smearing:
  float D2 = Props::D2.get(&jet);
  Props::D2.set(&jet, smear * D2);
  // C2 smearing:
  float C2 = Props::C2.get(&jet);
  Props::C2.set(&jet, smear * C2);
  // Tau21 smearing:
  float Tau21 = Props::Tau21.get(&jet);
  Props::Tau21.set(&jet, smear * Tau21);
  return CP::CorrectionCode::Ok;
}

EL::StatusCode FatJetHandler::calibrateCopies(xAOD::JetContainer * particles, const CP::SystematicSet & sysSet)
{

  std::string sysName = sysSet.name();
  std::string uncKey;
  bool isVTagUnc = (m_jetTagUnc_prefixes.find(sysName) != m_jetTagUnc_prefixes.end() && !m_jetTagUnc_prefixes.empty());
  bool isFFSmearUnc = (m_jetMRUnc_prefixes.find(sysName) != m_jetMRUnc_prefixes.end() && !m_jetMRUnc_prefixes.empty());

  if(sysName == ""){
    sysName = "Nominal";
  }

  if (m_doCalib && !isVTagUnc && !isFFSmearUnc) {
    if ((m_jetUnc_prefixes.find(sysName) != m_jetUnc_prefixes.end() && !m_jetUnc_prefixes.empty()) || (m_smearLRPD && (sysName.find("FATJET_CR_JET_JERPD") != std::string::npos))){ //TODO: this is very hardcoded atm
      if(m_isMC && m_smearLRPD && (sysName.find("FATJET_CR_JET_JERPD") != std::string::npos)){ //PD added by us, need to remove it
	  //pseudo data smearing for JER 
	  TString jerSysName(sysName);
	  jerSysName.ReplaceAll("FATJET_CR_JET_JERPD", "FATJET_CR_JET_JER");
	  uncKey = m_jetUnc_prefixes.at(jerSysName.Data());
	  CP_CHECK("FatJetHandler::calibrateCopies()",m_jetUncToolsPD.at(uncKey)->applySystematicVariation(m_jetUnc_sysSets.at(jerSysName.Data())),m_debug);
	}else{
	  //JES, JER JMS uncertainty
	  uncKey = m_jetUnc_prefixes.at(sysName);
	  CP_CHECK("FatJetHandler::calibrateCopies()",m_jetUncTools.at(uncKey)->applySystematicVariation(m_jetUnc_sysSets.at(sysName)),m_debug);
	}
    }else{
      //systematic not known
      uncKey = m_jetUncTools.begin()->first;
      CP_CHECK("FatJetHandler::calibrateCopies()",m_jetUncTools.at(uncKey)->applySystematicVariation(sysSet),m_debug);
    }
  }else if(m_doCalib && isFFSmearUnc){
    //JMR uncertainties
    uncKey = m_jetMRUnc_prefixes.at(sysName);
    CP_CHECK("FatJetHandler::calibrateCopies()", m_jetSmearTools.at(uncKey)->applySystematicVariation(m_jetUnc_sysSets.at(sysName)),m_debug);  
  }

  //do the jet labelling
  if(m_isMC){
    m_JetTruthLabelingTool->modify(*particles);
  }

  //in case of boosted jet taggers, if it's a common JES NP and correlate
  bool affectsJES = false;
  bool affectsW50(false), affectsW80(false), affectsZ50(false), affectsZ80(false);
  if(m_bosonTag) { 
    if(isVTagUnc){
      EL_CHECK("FatJetHandler::getAffectedVBosonTaggers()", getAffectedVBosonTaggers(sysName, affectsW50, affectsW80, affectsZ50, affectsZ80) );
      if(affectsW50){
	CP_CHECK("FatJetHandler::calibrateCopies()", m_smoothedWTagger50_uncTool->applySystematicVariation(m_jetUnc_sysSets.at(sysName)),m_debug);
      }
      if (affectsW80){
	CP_CHECK("FatJetHandler::calibrateCopies()", m_smoothedWTagger80_uncTool->applySystematicVariation(m_jetUnc_sysSets.at(sysName)),m_debug);
      }
      if (affectsZ50){
	CP_CHECK("FatJetHandler::calibrateCopies()", m_smoothedZTagger50_uncTool->applySystematicVariation(m_jetUnc_sysSets.at(sysName)),m_debug);
      }
      if (affectsZ80){
	CP_CHECK("FatJetHandler::calibrateCopies()", m_smoothedZTagger80_uncTool->applySystematicVariation(m_jetUnc_sysSets.at(sysName)),m_debug);
      }
      //it is one of the boson uncertainties
      //change name to the JES name
      TString jesSysName(sysName);
      TString tagKey = m_jetTagUnc_prefixes.at(sysName);
      jesSysName.ReplaceAll(tagKey, "FATJET_CR_"); //this is for the moment hardcoded, do we need other schemes than the CR one?
      //see if the changed name is known to the JES tool
      if (m_jetUnc_prefixes.find(jesSysName.Data()) != m_jetUnc_prefixes.end() && !m_jetUnc_prefixes.empty()) {
	//variation is known, apply it
	affectsJES = true;
	CP_CHECK("FatJetHandler::calibrateCopies()",m_jetUncTools.at("FATJET_CR_")->applySystematicVariation(m_jetUnc_sysSets.at(jesSysName.Data())),m_debug);	  
      }
    }
  }

  // loop over the particles in the container
  for (xAOD::Jet * jet : *particles) {
    //this ensures that the DynAux container has the same number of entries as the original one
    setP4( jet , jet );

    // print particle (pt,eta,phi,e,m)
    //std::cout << "===> Fat jet <===" << std::endl;
    //printParticle(jet);

    //Detector Eta fix
    //In derivations up to cache 21.2.18 (pTags 3503/3504 and p3506) there is no DetectorEta information available -> replace it with JetConstituentEta
    //For newer derivations (see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DerivationProductionTeam for pTag-Cache matching) the DetectorEta has been added again
    if (m_hasNoFJDetectorEta){
      jet->setAttribute<float>("DetectorEta", JetConstitScaleMomentum_eta(*jet));
    }

    // apply calibrations
    if(m_doCalib) {
      CP_CHECK("FatJetHandler::calibrateCopies()", m_jetCalibTool->applyCalibration(*jet), m_debug);
    }

    // Boson tag
    if(m_bosonTag) {
      doVTag(jet);
    }

    // Uncertainty treatment
    if(m_isMC){
      // Check first the validity range
      // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetUncertaintiesRel21ConsolidatedLargeRScaleRes and https://twiki.cern.ch/twiki/bin/view/AtlasProtected/FFJetSmearingTool
      bool validForUncTool = (jet->pt() > 200e3 && jet->pt() < 3000e3);
      validForUncTool &= (jet->m() < 600e3);
      validForUncTool &= (fabs(jet->eta()) < 2);

      //apply correction
      if (validForUncTool && m_doCalib && !isVTagUnc && !isFFSmearUnc) {
	//JES, JMS, JER
	if(m_smearLRPD && sysName.find("FATJET_CR_JET_JERPD") != std::string::npos){
	  CP_CHECK("FatJetHandler::calibrateCopies()",m_jetUncToolsPD.at(uncKey)->applyCorrection(*jet),m_debug);
	}else{
	  CP_CHECK("FatJetHandler::calibrateCopies()",m_jetUncTools.at(uncKey)->applyCorrection(*jet),m_debug);
	}
      }else if (validForUncTool && m_doCalib && isFFSmearUnc){
	//JMR
	CP_CHECK("FatJetHandler::calibrateCopies()",m_jetSmearTools.at(uncKey)->applyCorrection(*jet),m_debug);
      }

      if(validForUncTool && m_doCalib && affectsJES){
	CP_CHECK("FatJetHandler::calibrateCopies()", m_jetUncTools.at("FATJET_CR_")->applyCorrection(*jet),m_debug); 
      }
      if(affectsW50){
	CP_CHECK("FatJetHandler::calibrateCopies()", m_smoothedWTagger50_uncTool->applyCorrection(*jet),m_debug); 
      }
      if(affectsW80){
	CP_CHECK("FatJetHandler::calibrateCopies()", m_smoothedWTagger80_uncTool->applyCorrection(*jet),m_debug); 
      }
      if(affectsZ50){
	CP_CHECK("FatJetHandler::calibrateCopies()", m_smoothedZTagger50_uncTool->applyCorrection(*jet),m_debug); 
      }
      if(affectsZ80){
	CP_CHECK("FatJetHandler::calibrateCopies()", m_smoothedZTagger80_uncTool->applyCorrection(*jet),m_debug); 
      }  

    }

    // decorate jet
    if ( decorate( jet ) != EL::StatusCode::SUCCESS ) return EL::StatusCode::FAILURE;

  }

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode FatJetHandler::writeOutputVariables(xAOD::Jet* inJet, xAOD::Jet* outJet, bool isKinVar, bool isWeightVar, const TString& sysName)
{

  if (isKinVar || isWeightVar) {
    // Nominal Not, Kinematic Yes, Weight Yes
    EL_CHECK("FatJetHandler::writeOutputVariables",writeAllVariations(inJet, outJet, sysName));
  }
  if (isKinVar && !isWeightVar) {
    // Nominal Not, Kinematic Yes, Weight Not
    EL_CHECK("FatJetHandler::writeOutputVariables",writeKinematicVariations(inJet, outJet, sysName));
  } else if (!isKinVar && isWeightVar) {
    // Nominal Not, Kinematic Not, Weight Yes
    EL_CHECK("FatJetHandler::writeOutputVariables",writeWeightVariations(inJet, outJet, sysName));
  } else if (!isKinVar && !isWeightVar) {
    // Nominal Yes, Kinematic Not, Weight Not
    // for nominal we save all of them
    // in order to not copy paste code in both up and here in Nominal
    // and later to update one and forget the other
    // the functions were created to be easily read by name and edited
    EL_CHECK("FatJetHandler::writeOutputVariables",writeAllVariations(inJet, outJet, sysName));
    EL_CHECK("FatJetHandler::writeOutputVariables",writeKinematicVariations(inJet, outJet, sysName));
    EL_CHECK("FatJetHandler::writeOutputVariables",writeWeightVariations(inJet, outJet, sysName));
    EL_CHECK("FatJetHandler::writeOutputVariables",writeNominal(inJet, outJet, sysName));
  } else assert(false);

  return writeCustomVariables(inJet, outJet, isKinVar, isWeightVar, sysName);
}

EL::StatusCode FatJetHandler::writeAllVariations(xAOD::Jet*, xAOD::Jet*, const TString&)
{
  //
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode FatJetHandler::writeKinematicVariations(xAOD::Jet* inJet, xAOD::Jet* outJet, const TString& sysName)
{
  // bool isJEVar = (sysName == "Nominal" || sysName.Contains("_JER") || sysName.Contains("_Kin") || sysName.Contains("_pT"));
  // bool isJMVar = (sysName == "Nominal" || sysName.Contains("_JMR") || sysName.Contains("_Kin") || sysName.Contains("_mass"));
  bool isSubVar = (sysName == "Nominal" || sysName.Contains("_D2") || sysName.Contains("_C2") || sysName.Contains("_Tau21") || sysName.Contains("_Sub"));

  // bool isMassRes = (sysName == "Nominal" || sysName.Contains("_MassRes"));

  // set four momentum -> only pt/m are affected by systs (if pT or m is changed here, e will be recalculated)
  //there is currently no easy way to do this smarter w/o hardcoding all the uncertainty names
  Props::pt.copy(inJet, outJet);
  Props::m.copy(inJet, outJet);

  // substructure moments: ECFs and n-subjettiness
  if (isSubVar) {
    Props::D2.copy(inJet, outJet);
    Props::C2.copy(inJet, outJet);
    Props::Tau21.copy(inJet, outJet);
  }

  // boson tagging
  if(m_bosonTag) {

    if ( m_isLCTOPO ) {
      Props::isWJet50.copy(inJet, outJet);
      Props::isWJet80.copy(inJet, outJet);
      Props::isZJet50.copy(inJet, outJet);
      Props::isZJet80.copy(inJet, outJet);
      Props::passWMassCut50.copy(inJet, outJet);
      Props::passWMassCut80.copy(inJet, outJet);
      Props::passZMassCut50.copy(inJet, outJet);
      Props::passZMassCut80.copy(inJet, outJet);
      Props::passWSubstructure50.copy(inJet, outJet);
      Props::passWSubstructure80.copy(inJet, outJet);
      Props::passZSubstructure50.copy(inJet, outJet);
      Props::passZSubstructure80.copy(inJet, outJet);
      Props::passWNtrk50.copy(inJet, outJet);
      Props::passWNtrk80.copy(inJet, outJet);
      Props::passZNtrk50.copy(inJet, outJet);
      Props::passZNtrk80.copy(inJet, outJet);
      Props::passWHighMassCut50.copy(inJet, outJet);
      Props::passWHighMassCut80.copy(inJet, outJet);
      Props::passZHighMassCut50.copy(inJet, outJet);
      Props::passZHighMassCut80.copy(inJet, outJet);
      Props::passWLowMassCut50.copy(inJet, outJet);
      Props::passWLowMassCut80.copy(inJet, outJet);
      Props::passZLowMassCut50.copy(inJet, outJet);
      Props::passZLowMassCut80.copy(inJet, outJet);
      Props::SmoothWContained50_SF.copy(inJet, outJet);
      Props::SmoothWContained50_effSF.copy(inJet, outJet);
      Props::SmoothWContained80_SF.copy(inJet, outJet);
      Props::SmoothWContained80_effSF.copy(inJet, outJet);
      Props::SmoothZContained50_SF.copy(inJet, outJet);
      Props::SmoothZContained50_effSF.copy(inJet, outJet);
      Props::SmoothZContained80_SF.copy(inJet, outJet);
      Props::SmoothZContained80_effSF.copy(inJet, outJet);

    }
    if ( m_isTCC ){
      Props::passWMassCut_TCC_2var.copy(inJet, outJet);
      Props::passWMassCut_TCC_3var.copy(inJet, outJet);
      Props::passZMassCut_TCC_2var.copy(inJet, outJet);
      Props::passZMassCut_TCC_3var.copy(inJet, outJet);
      
      Props::passWHighMassCut_TCC_2var.copy(inJet, outJet);
      Props::passWHighMassCut_TCC_3var.copy(inJet, outJet);
      Props::passZHighMassCut_TCC_2var.copy(inJet, outJet);
      Props::passZHighMassCut_TCC_3var.copy(inJet, outJet);
      
      Props::passWLowMassCut_TCC_2var.copy(inJet, outJet);
      Props::passWLowMassCut_TCC_3var.copy(inJet, outJet);
      Props::passZLowMassCut_TCC_2var.copy(inJet, outJet);
      Props::passZLowMassCut_TCC_3var.copy(inJet, outJet);
      
      Props::passWSubstructure_TCC_2var.copy(inJet, outJet);
      Props::passWSubstructure_TCC_3var.copy(inJet, outJet);
      Props::passZSubstructure_TCC_2var.copy(inJet, outJet);
      Props::passZSubstructure_TCC_3var.copy(inJet, outJet);
      
      
      Props::passWNtrk_TCC_3var.copy(inJet, outJet);
      Props::passZNtrk_TCC_3var.copy(inJet, outJet);
      
      Props::isWJet_TCC_2var.copy(inJet, outJet);
      Props::isWJet_TCC_3var.copy(inJet, outJet);
      Props::isZJet_TCC_2var.copy(inJet, outJet);
      Props::isZJet_TCC_3var.copy(inJet, outJet);
    }
  }

  //
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode FatJetHandler::writeWeightVariations(xAOD::Jet* inJet, xAOD::Jet* outJet, const TString& sysName)
{

  if(m_bosonTag) {
    if ( m_isLCTOPO ) {
      if( (m_jetTagUnc_prefixes.find(sysName.Data()) != m_jetTagUnc_prefixes.end() && !m_jetTagUnc_prefixes.empty()) ){
	Props::SmoothWContained50_SF.copy(inJet, outJet);
	Props::SmoothWContained50_effSF.copy(inJet, outJet);
	Props::SmoothWContained80_SF.copy(inJet, outJet);
	Props::SmoothWContained80_effSF.copy(inJet, outJet);
	Props::SmoothZContained50_SF.copy(inJet, outJet);
	Props::SmoothZContained50_effSF.copy(inJet, outJet);
	Props::SmoothZContained80_SF.copy(inJet, outJet);
	Props::SmoothZContained80_effSF.copy(inJet, outJet);
      }
    }
  }
  
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode FatJetHandler::writeNominal(xAOD::Jet* inJet, xAOD::Jet* outJet, const TString& /*sysName*/)
{

  // set four momentum -> eta/phi are not affected by systs
  Props::eta.copy(inJet, outJet);
  Props::phi.copy(inJet, outJet);

  //save the truth label from jet truth labelling tool
  if(m_isMC) Props::R10TruthLabel_R21Consolidated.copy(inJet, outJet);

  if( m_isLCTOPO ){
    //calo mass + pt
    Props::caloPt.copyIfExists(inJet, outJet);
    Props::caloMass.copyIfExists(inJet, outJet);

    // track assisted mass
    Props::TAmass.copyIfExists(inJet, outJet);
    //Props::UncalibTAmass.copyIfExists(inJet, outJet); // not necessary?
  }

  Props::C2.copy(inJet, outJet);
  Props::Tau21.copy(inJet, outJet);
  if(m_saveNonCrucialVariables){
    Props::Tau32.copy(inJet, outJet);
  }

  if( m_isLCTOPO ){
    setP4( inJet , "hbbVR", outJet );
  }

  Props::Angularity.copyIfExists(inJet, outJet);
  Props::KtDR.copyIfExists(inJet, outJet);
  Props::NumTrkPt1000PV.copy(inJet, outJet);
  Props::NumTrkPt500PV.copy(inJet, outJet);

  if(m_saveqgvariables){
    Props::TrackWidthPt500PV.copyIfExists(inJet,outJet);
    Props::Width.copyIfExists(inJet,outJet);
    Props::DFCommonJets_QGTagger_NTracks.copyIfExists(inJet,outJet);
    Props::DFCommonJets_QGTagger_TracksWidth.copyIfExists(inJet,outJet);
    Props::DFCommonJets_QGTagger_TracksC1.copyIfExists(inJet,outJet);
  }
  // truth jets as signal states
  if(m_isMC && m_matchTruthJet) {
    //setP4( inJet , "Truth", outJet );
    setP4( inJet , "TruthWZ", outJet );
  }

  // ghost associated hadron labels
  if ( m_isMC && m_storeGAParticlesInFatJets ) {

    const ElementLink<xAOD::JetContainer> &el = inJet->auxdata<ElementLink<xAOD::JetContainer> >("Parent");
    if (!(el.isValid())) {
      Error("FatJetHandler::writeNominal()","Couldn't find parent (ungroomed) jet! (You can turn off storing these hadrons via 'storeGAParticlesInFatJets = false' in the config file)");
      return EL::StatusCode::FAILURE;
    } else {
      const std::string labelB   = "ConeExclBHadronsFinal";//"GhostBHadronsFinal";
      const std::string labelC   = "ConeExclCHadronsFinal";//"GhostCHadronsFinal";
      const std::string labelTau = "GhostTausFinal";
      const std::string labelH   = "GhostHBosons";
      std::vector<const xAOD::TruthParticle *> ghostB;
      std::vector<const xAOD::TruthParticle *> ghostC;
      std::vector<const xAOD::TruthParticle *> ghostTau;
      std::vector<const xAOD::TruthParticle *> ghostH;
      // get input vectors from ghost-associated parent jet element link
      if ( ! (*el)->getAssociatedObjects<xAOD::TruthParticle>( labelB   , ghostB   ) ||
          ! (*el)->getAssociatedObjects<xAOD::TruthParticle>( labelC   , ghostC   ) ||
          ! (*el)->getAssociatedObjects<xAOD::TruthParticle>( labelTau , ghostTau ) ||
          ! (*el)->getAssociatedObjects<xAOD::TruthParticle>( labelH , ghostH ) )
      {
        Error("FatJetHandler::writeNominal()","Couldn't access ghost associated heavy flavour hadrons! (You can turn off storing these hadrons via 'storeGAParticlesInFatJets = false' in the config file)");
        return EL::StatusCode::FAILURE;
      }
      // make sure all IParticle pointers are valid (i.e. remove invalid pointers)
      // (are the invalid pointers a bug?)
      for (const xAOD::TruthParticle * part : ghostB  ) if ( ! part ) ghostB.erase( std::remove( std::begin(ghostB), std::end(ghostB), part ), std::end(ghostB) );
      for (const xAOD::TruthParticle * part : ghostC  ) if ( ! part ) ghostC.erase( std::remove( std::begin(ghostC), std::end(ghostC), part ), std::end(ghostC) );
      for (const xAOD::TruthParticle * part : ghostTau) if ( ! part ) ghostTau.erase( std::remove( std::begin(ghostTau), std::end(ghostTau), part ), std::end(ghostTau) );
      for (const xAOD::TruthParticle * part : ghostH  ) if ( ! part ) ghostH.erase( std::remove( std::begin(ghostH), std::end(ghostH), part ), std::end(ghostH) );

      // set output vectors
      // element links are broken in output; see JIRA CXAOD-211; removing for the moment
      // outJet->setAssociatedObjects<xAOD::TruthParticle>( labelB   , ghostB   );
      // outJet->setAssociatedObjects<xAOD::TruthParticle>( labelC   , ghostC   );
      // outJet->setAssociatedObjects<xAOD::TruthParticle>( labelTau , ghostTau );
      // outJet->setAssociatedObjects<xAOD::TruthParticle>( labelH   , ghostH   );

      // count number of B- and C-Hadrons, excluding non-prompt decays
      int nGhostMatchedHBosons = ghostH.size();
      int nGhostMatchedBHadrons = ghostB.size();
      int nGhostMatchedPromptCHadrons = 0;
      // only counting prompt C-Hadrons
      for (const xAOD::TruthParticle * part : ghostC ) {
        if (!IsChild(part, ghostB)) nGhostMatchedPromptCHadrons++;
      }
      int nGhostMatchedTaus = ghostTau.size();


      Props::nGhostMatchedBHadrons.set(outJet, nGhostMatchedBHadrons);
      Props::nGhostMatchedPromptCHadrons.set(outJet, nGhostMatchedPromptCHadrons);
      Props::nGhostMatchedHBosons.set(outJet, nGhostMatchedHBosons);
      if(m_saveNonCrucialVariables){
	Props::nGhostMatchedTaus.set(outJet, nGhostMatchedTaus);
      }
    }
  }


  if(m_bosonTag) {
    if ( m_isLCTOPO ) {
      Props::SmoothWContained50_efficiency.copy(inJet, outJet);
      Props::SmoothWContained80_efficiency.copy(inJet, outJet);
      Props::SmoothZContained50_efficiency.copy(inJet, outJet);
      Props::SmoothZContained80_efficiency.copy(inJet, outJet);
    }
  }

  Props::Xbb202006_Higgs.copyIfExists(inJet, outJet);
  Props::Xbb202006_Top.copyIfExists(inJet, outJet);
  Props::Xbb202006_QCD.copyIfExists(inJet, outJet);
  Props::Xbb2020v3_Higgs.copyIfExists(inJet, outJet);
  Props::Xbb2020v3_Top.copyIfExists(inJet, outJet);
  Props::Xbb2020v3_QCD.copyIfExists(inJet, outJet);

  //
  return EL::StatusCode::SUCCESS;
}

bool FatJetHandler::IsChild(const xAOD::TruthParticle* child, std::vector<const xAOD::TruthParticle *> parents, std::vector<const xAOD::TruthParticle*> ancestors) {
  // child:     particle to be checked; starting point
  // parents:   list of parent particles
  // ancestors: lineage, starting from child, to avoid loops in some W+b samples
  // TODO:      move to CxAODTools / look if something like this exists in RootCore truth utils
  if (parents.size() == 0) return false;
  if (std::count(ancestors.begin(), ancestors.end(), child) > 0) return false; // check if child already exists in lineage
  if (std::find(parents.begin(), parents.end(), child) != parents.end()) return true;
  ancestors.push_back(child);
  if (!child) return false;
  for (unsigned int j = 0; j < child->nParents(); j++) {
    if (IsChild(child->parent(j), parents, ancestors)) return true; // recursive call
  }
  return false;
}


EL::StatusCode FatJetHandler::writeCustomVariables(xAOD::Jet*, xAOD::Jet*, bool, bool, const TString&)
{
  // This method is meant to be overridden in derived handlers for writing
  // additional decorations. Argument names are omitted here to avoid warnings.
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode FatJetHandler::clearEvent()
{

  EL_CHECK("FatJetHandler::clearEvent()", ObjectHandler::clearEvent());
  return EL::StatusCode::SUCCESS;

}


EL::StatusCode FatJetHandler::getHbbCorrectedVector(const xAOD::Jet& jet, TLorentzVector& correctedVector)
{
  /* Steps:
     1. Get all AntiKt2TrackJets/AntiKtVR30Rmax4Rmin02TrackJets asssociated with the ungroomed jet
     2. Match muons to these b-tagged track-jets
     - if more than 1 muon matches a track jet, only use the muon closest in DR
     3. Correct the fat-jet mass by putting the matched muon back
  */

  // no associated muons available at the moment in DxAOD
  // decorate track jets with muons using simple dR matching
  if (decorateWithMuons(jet) == StatusCode::FAILURE) {
    Error("FatJetHandler::getHbbCorrectedVector()","No muon decoration for muon-in-jet correction available.");
  }

  // Step 1

  // std::vector<const xAOD::Jet*> associated_trackJets;
  std::vector<ElementLink<xAOD::IParticleContainer> > associated_trackJets;

  // get the track jets from the parent
  bool problemWithParent = false;
  ElementLink<xAOD::JetContainer> parentEL;
  if(!acc_parent.isAvailable(jet)) problemWithParent = true;
  else parentEL = acc_parent(jet);
  if(problemWithParent || !parentEL.isValid()){
    if(problemWithParent) Error("FatJetHandler::getHbbCorrectedVector()","Parent decoration does not exist.");
    if(!parentEL.isValid()) Error("FatJetHandler::getHbbCorrectedVector()","Parent link is not valid.");
  }
  else {
    const xAOD::Jet* parentJet = *parentEL;
    // use accessor instead of getAssociatedObject in order to have EL
    if (!m_acc_ghostMatchedTrackJets.isAvailable(*parentJet)) {
      Error("FatJetHandler::getHbbCorrectedVector()","Ghostmatched jet collection does not exist.");
    }
    associated_trackJets = m_acc_ghostMatchedTrackJets(*parentJet);
  }

  // decorate all trackjets by default
  // filter out the track jets we do not want (pT > 10 GeV and |eta| < 2.5 and at least 2 constituents)
  std::vector<ElementLink<xAOD::IParticleContainer> > associated_trackJets_filtered;
  for(const auto& trackJetEL: associated_trackJets) {
    if(!trackJetEL.isValid()) {
      Error("FatJetHandler::getHbbCorrectedVector()","Track jet link is not valid.");
    }
    const xAOD::Jet *trackJet(static_cast<const xAOD::Jet*>(*trackJetEL));
    if (trackJet->pt() < m_trackJetPtMin) continue;
    if (fabs(trackJet->eta()) > m_trackJetEtaMax) continue;
    if (trackJet->numConstituents() < m_trackJetNConst) continue;
    associated_trackJets_filtered.push_back(trackJetEL);
  }
  std::sort(associated_trackJets_filtered.begin(), associated_trackJets_filtered.end(), [](ElementLink<xAOD::IParticleContainer> lhs, ElementLink<xAOD::IParticleContainer> rhs) -> bool { return ((static_cast<const xAOD::IParticle*>(*lhs))->pt() > (static_cast<const xAOD::IParticle*>(*rhs))->pt()); });

  // Step 2

  std::vector<xAOD::Muon*> calibratedMuons;
  std::vector<const xAOD::Muon*> matched_muons;
  for (const auto& trackJetEL: associated_trackJets_filtered) {
    const xAOD::Jet *trackJet(static_cast<const xAOD::Jet*>(*trackJetEL));
    float maxDR(m_muonDrMax);
    // trackJet->getAttribute("SizeParameter", maxDR);
    const xAOD::Muon *closest_muon(nullptr);
    // get muons from jet decoration
    if( ! m_decMuonsInTrackJetLink.isAvailable( *trackJet ) ) {
      Error("FatJetHandler::getHbbCorrectedVector()","No muons link found for jet.");
    }
    for (auto muonLink : m_decMuonsInTrackJetLink( *trackJet)) {
      if(!muonLink.isValid()) {
	if(m_debug) std::cout<<"Muon link not valid."<<std::endl;
	continue;
      }
      const xAOD::Muon *muon(static_cast<const xAOD::Muon*>(*muonLink));
      // apply muon correction if not calibrated yet
      static SG::AuxElement::Accessor<float> acc_muonSpectrometerPt ("MuonSpectrometerPt");
      static SG::AuxElement::Accessor<float> acc_innerDetectorPt ("InnerDetectorPt");
      if (!(acc_innerDetectorPt.isAvailable(*muon) && acc_muonSpectrometerPt.isAvailable(*muon))) {
	if(m_debug) std::cout<<"No decorators for MuonSpectrometerPt or InnerDetectorPt found. Calibrate muons on-the-fly."<<std::endl;
	xAOD::Muon *muon_calib(nullptr);
	if( m_muonCalibrationPeriodTool->correctedCopy( *muon, muon_calib) != CP::CorrectionCode::Ok ){
	  Error("FatJetHandler::getHbbCorrectedVector()","Could not get calibrated copy of muon.");
	}
	// save the pointers for deletion later
	calibratedMuons.push_back(muon_calib);
	// work with calibrated muon
	muon = muon_calib;
      }
      // muon quality selection
      if (muon->pt() < m_muonPtMin) continue;
      if (m_muonSelectionTool->getQuality(*muon) > xAOD::Muon::Medium) continue;
      if (fabs(muon->eta()) > m_muonEtaMax) continue;
      // find clostest muon
      float DR( trackJet->p4().DeltaR(muon->p4()) );
      float cutDR=std::min(0.4,0.04+10000.0/muon->pt());
      if (DR > cutDR) continue;
      if (DR > maxDR) continue;
      maxDR = DR;
      closest_muon = muon;
    }

    // check if the closest muon was already selected
    for (const auto muon: matched_muons){
      if(closest_muon == muon){
	closest_muon = nullptr;
	if(m_debug) std::cout<<"Muon duplicate found! Skipping."<<std::endl;
	break;
      }
    }
    if (closest_muon) {
      matched_muons.push_back(closest_muon);
    }
  }

  // Step 3

  xAOD::JetFourMom_t corrected_jet_p4 = getMuonCorrectedJetFourMom(jet, matched_muons, Combined);
  TLorentzVector corrected_jet(corrected_jet_p4.x(), corrected_jet_p4.y(), corrected_jet_p4.z(), corrected_jet_p4.t());
  correctedVector = corrected_jet;

  // delete the vector of pointers to muons after you have used them to correct the four vector
  for(xAOD::Muon * muonPointer : calibratedMuons){
    delete muonPointer;
  }

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode FatJetHandler::decorateWithMuons(const xAOD::Jet& jet) const
{
  // retrieve muons from StoreGate
  const xAOD::MuonContainer* muons;
  if (!m_event->retrieve(muons, "Muons")) {
    Error("FatJetHandler::decorateWithMuons()","Cannot retrieve muon collection Muons from event store.");
    return StatusCode::FAILURE;
  }
  // retrieve ghost-associated track jets from large-R jet
  std::vector<const xAOD::Jet*> associated_trackJets;
  // get the element links to the parent, ungroomed jet
  if(!acc_parent.isAvailable(jet)) {
    Error("FatJetHandler::decorateWithMuons()","Parent (ungroomed) jet collection does not exist.");
    return StatusCode::FAILURE;
  }
  ElementLink<xAOD::JetContainer> parentEL = acc_parent(jet);
  if(!parentEL.isValid()) {
    Error("FatJetHandler::decorateWithMuons()","Parent link is not valid.");
    return StatusCode::FAILURE;
  }
  // access the track jets
  const xAOD::Jet* parentJet = *parentEL;
  if(!parentJet->getAssociatedObjects<xAOD::Jet>(m_name_ghostMatchedTrackJets, associated_trackJets)) {
    if(m_debug) std::cout<<"No associated track jets found on parent jet."<<std::endl;
    return StatusCode::SUCCESS;
  }
  // decorate all track jets by default, no selection, no muon overlap removal (will be done later)
  float maxDR(m_muonDrMax);
  for(auto trackJet: associated_trackJets) {
    std::vector<ElementLink<xAOD::IParticleContainer> > muons_in_jet;
    for(const auto& muon: *muons) {
      float DR( trackJet->p4().DeltaR(muon->p4()) );
      if (DR > maxDR) continue;
      ElementLink<xAOD::IParticleContainer> muonEL( *muons, muon->index() );
      muons_in_jet.push_back(muonEL);
    }
    m_decMuonsInTrackJetLink(*trackJet) = muons_in_jet;
  }
  return StatusCode::SUCCESS;
}


const xAOD::JetFourMom_t FatJetHandler::getMuonCorrectedJetFourMom(const xAOD::Jet& jet, std::vector<const xAOD::Muon*> muons, MuonCorrectionScheme scheme, bool useJMSScale) const
{
  xAOD::JetFourMom_t JetCorr_tlv = jet.jetP4();
  if(muons.size()==0){
    return JetCorr_tlv;
  }
  if( !(jet.m()>0 && jet.jetP4(m_calibratedMassDecorator+"Calo").M()>0 && jet.jetP4(m_calibratedMassDecorator+"TA").M()>0) ){
    return JetCorr_tlv;
  }
  if(m_debug) std::cout<<"Derive muon-in-jet correction: nMuons=" << (int) muons.size() << "\tMuonCorrectionScheme=" << scheme << "\tuseJMSScale=" << useJMSScale<<std::endl;
  if (scheme == Calorimeter) {
    // muon-in-jet correction for jets calibrated using calorimeter mass
    xAOD::JetFourMom_t CaloJet_tlv = jet.jetP4();
    if (useJMSScale) CaloJet_tlv = jet.jetP4(m_calibratedMassDecorator+"Calo");
    for(auto muon : muons) {
      // get energy loss of muon in the calorimeter
      float eLoss(0.0);
      muon->parameter(eLoss, xAOD::Muon::EnergyLoss);
      if(m_debug) std::cout<<"Energy loss in calorimter=" << eLoss<<std::endl;
      // use muon tlv to get x/y/z compontent of energy loss
      auto muon_tlv = muon->p4();
      double eLossX = eLoss * sin(muon_tlv.Theta()) * cos(muon_tlv.Phi());
      double eLossY = eLoss * sin(muon_tlv.Theta()) * sin(muon_tlv.Phi());
      double eLossZ = eLoss * cos(muon_tlv.Theta());
      TLorentzVector mLoss(eLossX, eLossY, eLossZ, eLoss);
      auto muonCorr_tlv = muon_tlv - mLoss;
      // apply muon-in-jet correction for track-assisted mass calibration to output jet TLV
      CaloJet_tlv += xAOD::JetFourMom_t(muonCorr_tlv.Pt(), muonCorr_tlv.Eta(), muonCorr_tlv.Phi(), muonCorr_tlv.M());
    }
    JetCorr_tlv = CaloJet_tlv;
  } else if (scheme == TrackAssisted) {
    // muon-in-jet correction for jets calibrated using track-assisted mass
    xAOD::JetFourMom_t TAJet_tlv = jet.jetP4();
    if (useJMSScale) TAJet_tlv = jet.jetP4(m_calibratedMassDecorator+"TA");
    xAOD::JetFourMom_t CaloJet_tlv = jet.jetP4(m_calibratedMassDecorator+"Calo");
    xAOD::JetFourMom_t CaloJetCorr_tlv =  getMuonCorrectedJetFourMom(jet, muons, Calorimeter, true);
    float TAJetCorr_m = TAJet_tlv.M() / CaloJet_tlv.Pt() * CaloJetCorr_tlv.Pt() ;
    float TAJetCorr_pt = sqrt((CaloJetCorr_tlv.E() * CaloJetCorr_tlv.E()) - (TAJetCorr_m * TAJetCorr_m)) / cosh( CaloJetCorr_tlv.Eta() );
    // apply muon-in-jet correction for track-assisted mass calibration to output jet TLV
    JetCorr_tlv = xAOD::JetFourMom_t(TAJetCorr_pt, CaloJetCorr_tlv.Eta(), CaloJetCorr_tlv.Phi(), TAJetCorr_m);
  } else if (scheme == Combined) {
    // muon-in-jet correction for jets calibrated using combined mass
    xAOD::JetFourMom_t TAJet_tlv = jet.jetP4(m_calibratedMassDecorator+"TA");
    xAOD::JetFourMom_t TAJetCorr_tlv = getMuonCorrectedJetFourMom(jet, muons, TrackAssisted, true);
    xAOD::JetFourMom_t CaloJet_tlv = jet.jetP4(m_calibratedMassDecorator+"Calo");
    xAOD::JetFourMom_t CaloJetCorr_tlv = getMuonCorrectedJetFourMom(jet, muons, Calorimeter, true);
    xAOD::JetFourMom_t CombJet_tlv = jet.jetP4();
    float CaloWeight = (CombJet_tlv.M() -   TAJet_tlv.M()) / (CaloJet_tlv.M() - TAJet_tlv.M());
    float TAWeight   = (CaloJet_tlv.M() - CombJet_tlv.M()) / (CaloJet_tlv.M() - TAJet_tlv.M());
    if(m_debug) std::cout<<"CaloWeight=" << CaloWeight << "\tTAWeight=" << TAWeight<<std::endl;
    float CombJetCorr_m = CaloWeight * CaloJetCorr_tlv.M() + TAWeight * TAJetCorr_tlv.M();
    float CombJetCorr_pt = sqrt((CaloJetCorr_tlv.E() * CaloJetCorr_tlv.E()) - (CombJetCorr_m * CombJetCorr_m)) / cosh( CaloJetCorr_tlv.Eta() );
    // apply muon-in-jet correction for track-assisted mass calibration to output jet TLV
    JetCorr_tlv = xAOD::JetFourMom_t(CombJetCorr_pt, CaloJetCorr_tlv.Eta(), CaloJetCorr_tlv.Phi(), CombJetCorr_m);
  } else {
    Error("FatJetHandler::getMuonCorrectedJetFourMom()","Unknown muon correction scheme.");
  }
  return JetCorr_tlv;
}



EL::StatusCode FatJetHandler::initializeBosonTagUncertaintyTools(const std::string& base, const std::string& config, const std::string& prefix, const std::string& jetAlg){

  std::string jetTagUnc_FJ_base = base;
  std::string jetTagUnc_FJ_config = config; 
  std::string jetTagUnc_FJ_prefix = prefix;
  std::string MCType = "MC16";

  Info("FatJetHandler::initializeTools()","FatJet tagger uncertainty configuration discovered: %s", jetTagUnc_FJ_config.c_str());

  Info("FatJetHandler::initializeTools()","Initializing uncertainty tool for smoothedWTagger50");
  jetTagUnc_FJ_config += "WTag_SigEff50";

  m_smoothedWTagger50_uncTool = std::unique_ptr<JetUncertaintiesTool>( new JetUncertaintiesTool(("JetUncProvider_"+jetTagUnc_FJ_config+jetAlg).c_str()) );
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedWTagger50_uncTool->setProperty("JetDefinition", jetAlg ));
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedWTagger50_uncTool->setProperty("ConfigFile", (jetTagUnc_FJ_base+jetTagUnc_FJ_config+".config").c_str()));
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedWTagger50_uncTool->setProperty("MCType", MCType));
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedWTagger50_uncTool->setProperty("IsData", false)); 
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedWTagger50_uncTool->initialize());

  std::vector<std::string> pulls = {"__1down", "__1up"};
  CP::SystematicSet jetUnc_sysSet = m_smoothedWTagger50_uncTool->recommendedSystematics();
  const std::set<std::string> sysNamesW50 = jetUnc_sysSet.getBaseNames();
  for (auto sysName: sysNamesW50) {
    for (auto pull : pulls) {
      std::string sysPulled = sysName + pull;
      m_jetUnc_sysSets.insert({jetTagUnc_FJ_prefix+sysPulled, CP::SystematicSet(sysPulled)});
      m_jetUnc_prefixes.insert({jetTagUnc_FJ_prefix+sysPulled, jetTagUnc_FJ_prefix});
      m_jetTagUnc_prefixes.insert({jetTagUnc_FJ_prefix+sysPulled, jetTagUnc_FJ_prefix}); //needed for proper correlation later
    }
  }


  Info("FatJetHandler::initializeTools()","Initializing uncertainty tool for smoothedWTagger80");
  jetTagUnc_FJ_config = config;
  jetTagUnc_FJ_config += "WTag_SigEff80";

  m_smoothedWTagger80_uncTool = std::unique_ptr<JetUncertaintiesTool>( new JetUncertaintiesTool(("JetUncProvider_"+jetTagUnc_FJ_config+jetAlg).c_str()) );
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedWTagger80_uncTool->setProperty("JetDefinition", jetAlg ));
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedWTagger80_uncTool->setProperty("ConfigFile", (jetTagUnc_FJ_base+jetTagUnc_FJ_config+".config").c_str()));
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedWTagger80_uncTool->setProperty("MCType", MCType));
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedWTagger80_uncTool->setProperty("IsData", false)); 
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedWTagger80_uncTool->initialize());

  jetUnc_sysSet = m_smoothedWTagger80_uncTool->recommendedSystematics();
  const std::set<std::string> sysNamesW80 = jetUnc_sysSet.getBaseNames();
  for (auto sysName: sysNamesW80) {
    for (auto pull : pulls) {
      std::string sysPulled = sysName + pull;
      m_jetUnc_sysSets.insert({jetTagUnc_FJ_prefix+sysPulled, CP::SystematicSet(sysPulled)});
      m_jetUnc_prefixes.insert({jetTagUnc_FJ_prefix+sysPulled, jetTagUnc_FJ_prefix});
      m_jetTagUnc_prefixes.insert({jetTagUnc_FJ_prefix+sysPulled, jetTagUnc_FJ_prefix}); //needed for proper correlation later
    }
  }


  Info("FatJetHandler::initializeTools()","Initializing uncertainty tool for smoothedZTagger50");
  jetTagUnc_FJ_config = config;
  jetTagUnc_FJ_config += "ZTag_SigEff50";

  m_smoothedZTagger50_uncTool = std::unique_ptr<JetUncertaintiesTool>( new JetUncertaintiesTool(("JetUncProvider_"+jetTagUnc_FJ_config+jetAlg).c_str()) );
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedZTagger50_uncTool->setProperty("JetDefinition", jetAlg ));
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedZTagger50_uncTool->setProperty("ConfigFile", (jetTagUnc_FJ_base+jetTagUnc_FJ_config+".config").c_str()));
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedZTagger50_uncTool->setProperty("MCType", MCType));
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedZTagger50_uncTool->setProperty("IsData", false)); 
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedZTagger50_uncTool->initialize());

  jetUnc_sysSet = m_smoothedZTagger50_uncTool->recommendedSystematics();
  const std::set<std::string> sysNamesZ50 = jetUnc_sysSet.getBaseNames();
  for (auto sysName: sysNamesZ50) {
    for (auto pull : pulls) {
      std::string sysPulled = sysName + pull;
      m_jetUnc_sysSets.insert({jetTagUnc_FJ_prefix+sysPulled, CP::SystematicSet(sysPulled)});
      m_jetUnc_prefixes.insert({jetTagUnc_FJ_prefix+sysPulled, jetTagUnc_FJ_prefix});
      m_jetTagUnc_prefixes.insert({jetTagUnc_FJ_prefix+sysPulled, jetTagUnc_FJ_prefix}); //needed for proper correlation later
    }
  }


  Info("FatJetHandler::initializeTools()","Initializing uncertainty tool for smoothedZTagger80");
  jetTagUnc_FJ_config = config;
  jetTagUnc_FJ_config += "ZTag_SigEff80";

  m_smoothedZTagger80_uncTool = std::unique_ptr<JetUncertaintiesTool>( new JetUncertaintiesTool(("JetUncProvider_"+jetTagUnc_FJ_config+jetAlg).c_str()) );
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedZTagger80_uncTool->setProperty("JetDefinition", jetAlg ));
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedZTagger80_uncTool->setProperty("ConfigFile", (jetTagUnc_FJ_base+jetTagUnc_FJ_config+".config").c_str()));
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedZTagger80_uncTool->setProperty("MCType", MCType));
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedZTagger80_uncTool->setProperty("IsData", false)); 
  TOOL_CHECK("FatJetHandler()::initializeTools()", m_smoothedZTagger80_uncTool->initialize());

  jetUnc_sysSet = m_smoothedZTagger80_uncTool->recommendedSystematics();
  const std::set<std::string> sysNamesZ80 = jetUnc_sysSet.getBaseNames();
  for (auto sysName: sysNamesZ80) {
    for (auto pull : pulls) {
      std::string sysPulled = sysName + pull;
      m_jetUnc_sysSets.insert({jetTagUnc_FJ_prefix+sysPulled, CP::SystematicSet(sysPulled)});
      m_jetUnc_prefixes.insert({jetTagUnc_FJ_prefix+sysPulled, jetTagUnc_FJ_prefix});
      m_jetTagUnc_prefixes.insert({jetTagUnc_FJ_prefix+sysPulled, jetTagUnc_FJ_prefix}); //needed for proper correlation later
    }
  }

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode FatJetHandler::getAffectedVBosonTaggers(const TString& name, bool& w50, bool& w80, bool& z50, bool& z80){

  if( name.Contains("SigEff50") ){
    if( name.Contains("ExtrapolationZ") ){
      z50 = true;
    }else{
      w50 = true;
      z50 = true;
    }
  }else if ( name.Contains("SigEff80") ){
    if( name.Contains("ExtrapolationZ") ){
      z80 = true;
    }else{
      w80 = true;
      z80 = true;
    }
  }else{
    w50 = true;
    w80 = true;
    z50 = true;
    z80 = true;
  }

  return EL::StatusCode::SUCCESS;

}




