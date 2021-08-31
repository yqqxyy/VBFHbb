#include "EventLoop/Job.h"
#include "EventLoop/StatusCode.h"
#include "EventLoop/Worker.h"
#include "EventLoop/OutputStream.h"

#include "CxAODMaker_VBFHbb/AnalysisBase_VBFHbb.h"

#include "CxAODMaker_VHbb/ElectronHandler_VHbb.h"
#include "CxAODMaker_VHbb/MuonHandler_VHbb.h"
#include "CxAODMaker_VBFHbb/FatJetHandler_VBFHbb.h"
#include "CxAODMaker_VBFHbb/JetHandler_VBFHbb.h"
#include "CxAODMaker/TauHandler.h"
#include "CxAODMaker_VHbb/PhotonHandler_VHbb.h"
#include "CxAODMaker_VHbb/PhotonHandler_VGamma.h"
#include "CxAODMaker/TrackJetHandler.h"
#include "CxAODMaker/TruthJetHandler.h"
#include "CxAODMaker/METHandler.h"
#include "CxAODMaker_VBFHbb/EventInfoHandler_VBFHbb.h"
#include "CxAODMaker/EventSelector.h"
#include "CxAODMaker/TruthEventHandler.h"
#include "CxAODMaker/TruthProcessor.h"

#include "CxAODTools/OverlapRemoval.h"
#include "CxAODTools_VBFHbb/VBFHbbInclEvtSelection.h"

#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"

// this is needed to distribute the algorithm to the workers
ClassImp(AnalysisBase_VBFHbb)

EL::StatusCode AnalysisBase_VBFHbb::initializeSampleInfo() {
  std::cout << "Inside initializeSampleInfo" << std::endl;

  std::ifstream file;
  std::string sampleInfo_File = gSystem->Getenv("WorkDir_DIR");
  sampleInfo_File += "/data/CxAODOperations_VHbb/DxAOD/info/sample_info.txt";

  std::cout << sampleInfo_File << std::endl;

  file.open(sampleInfo_File.c_str());

  while (!file.eof()) {
    // read line
    std::string lineString;
    getline(file, lineString);
    // std::cout << lineString << std::endl;

    // skip empty lines
    // TODO - is there a better way to implement this check?
    // if (lineString.find(".") > 1000) {
    //   continue;
    // }

    // skip lines starting with #
    if (lineString.find("#") == 0) {
      continue;
    }

    // store in map
    std::stringstream line(lineString);
    int dsid;
    std::string shortname;
    std::string longname;
    std::string nominal;
    std::string veto;

    line >> dsid >> shortname >> longname >> nominal >> veto;

    if (m_NominalDSIDs.count(dsid) != 0) {
      Warning("initializeSampleInfo()", "Skipping duplicated mc_channel_number for line '%s'.", lineString.c_str());
      continue;
    }
    m_NominalDSIDs[dsid] = (nominal == "nominal");
  }
  file.close();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode AnalysisBase_VBFHbb::initializeHandlers() {
  
  Info("AnalysisBase_VHbb::initializeHandlers()","Initialize handlers.");
  m_isEmbedding=(dynamic_cast<TTree*>(wk()->inputFile()->Get("CollectionTree")))->GetBranch("MuonsEmb"); 

  // initialize EventInfoHandler
  m_eventInfoHandler = new EventInfoHandler_VBFHbb(*m_config, m_event);
  //pass information if running on derivation and MC / data to EventInfoHandler in order to have it available in the other object handlers
  m_eventInfoHandler->set_isMC(m_isMC);
  ((EventInfoHandler_VBFHbb*)m_eventInfoHandler)->set_isEmbedding(m_isEmbedding);
  m_eventInfoHandler->set_isDerivation(m_isDerivation);
  m_eventInfoHandler->set_derivationName(m_derivationName);
  m_eventInfoHandler->set_isAFII(m_isAFII);
  m_eventInfoHandler->set_pTag(m_pTag);
  m_eventInfoHandler->set_mcChanNr(m_mcChanNr);
  
  // check pile up reweight Files contain MC dataset before initializing PU reweight in EventInfoHandler
  if(m_isMC) EL_CHECK("AnalysisBase_VHbb::initializeHandlers()", checkPileupRwFiles());

  EL_CHECK("AnalysisBase_VHbb::initializeHandlers()",m_eventInfoHandler->initialize());

  std::string selectionName = "";
  m_config->getif<std::string>("selectionName", selectionName);

  // note: the names of the handlers determine which collections are
  //       read, see also CxAODMaker-job.cfg
  
  // these have global pointer to be used e.g. in event selection
  if (m_isMC) {
    m_truthjetHandler = registerHandler<TruthJetHandler>("truthJet");
    m_truthfatjetHandler = registerHandler<TruthJetHandler>("truthfatJet");
    m_truthWZjetHandler = registerHandler<TruthJetHandler>("truthWZJet");
    m_truthWZfatjetHandler = registerHandler<TruthJetHandler>("truthWZfatJet");
  }
  m_muonHandler     = registerHandler<MuonHandler_VHbb>("muon");
  m_tauHandler      = registerHandler<TauHandler>("tau");
  m_electronHandler = registerHandler<ElectronHandler_VHbb>("electron");
  if (selectionName == "vgamma") m_photonHandler   = registerHandler<PhotonHandler_VGamma>("photon");
  else m_photonHandler   = registerHandler<PhotonHandler_VHbb>("photon");
  m_jetHandler      = registerHandler<JetHandler_VBFHbb>("jet");
  ((JetHandler_VBFHbb*)m_jetHandler)->set_isEmbedding(m_isEmbedding);
  m_fatjetHandler   = registerHandler<FatJetHandler_VBFHbb>("fatJet");
  m_trackjetHandler = registerHandler<TrackJetHandler>("trackJet");
  m_vrtrackjetHandler = registerHandler<TrackJetHandler>("vrtrackJet");
  m_METHandler      = registerHandler<METHandler>("MET");
  m_METHandlerMJTight= registerHandler<METHandler>("METMJTight"); 
  m_METHandlerMJMUTight= registerHandler<METHandler>("METMJMUTight"); 
  m_METHandlerMJMiddle= registerHandler<METHandler>("METMJMiddle"); 
  m_METHandlerMJLoose= registerHandler<METHandler>("METMJLoose"); 
  
  // these are spectators: they are calibrated and written to output,
  //                       but are not used in the event selection
   
  registerHandler<JetHandler_VBFHbb>("jetSpectator");
  m_METTrackHandler = registerHandler<METHandler>("METTrack");

  if (m_isMC) {
    registerHandler<METHandler>("METTruth");
    registerHandler<TruthEventHandler>("truthEvent");
  }
   
  // alternative: manual handler initialization (e.g. with different constructor)
  //  std::string containerName;
  //  std::string name = "muon";
  //  m_config->getif<std::string>(name + "Container", containerName);
  //  if ( ! containerName.empty() ) {
  //    m_muonHandler = new MuonHandler(name, *m_config, m_event, *m_eventInfoHandler);
  //    m_objectHandler.push_back( m_muonHandler );
  //  }

  // for tau truth matching
  if (m_tauHandler) {
    m_tauHandler->setTruthProcessor(m_truthProcessor);
  }
  

  // Set trigger stream info if data
  // need to catch embedding once the time has come
  if( !m_isMC && !m_isEmbedding ){
    TString sampleName = wk()->metaData()->castString("sample_name");
    if(sampleName.Contains("Egamma")) m_eventInfoHandler->set_TriggerStream(1);
    if(sampleName.Contains("Muons")) m_eventInfoHandler->set_TriggerStream(2);
    if(sampleName.Contains("Jet")) m_eventInfoHandler->set_TriggerStream(3);
  }
  
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode AnalysisBase_VBFHbb::initializeSelection() {

  Info("initializeSelection()","...");

  // determine selection name
  
  std::string selectionName = "";
  m_config->getif<std::string>("selectionName", selectionName);

  bool applySelection = false;
  m_config->getif<bool>("applyEventSelection", applySelection);
  if (applySelection) {
    Info("initialize()", "Applying selection: %s", selectionName.c_str());
    if (selectionName == "vbf" ) m_selector->setSelection(new VBFHbbInclEvtSelection());
    else {
      Error("initialize()", "Unknown selection requested!");
      return EL::StatusCode::FAILURE;
    }
  }

  // initialize overlap removal (possibly analysis specific)

  OverlapRemoval* overlapRemoval = new OverlapRemoval(*m_config);
  EL_CHECK("initializeSelection()",overlapRemoval->initialize());
  m_selector -> setOverlapRemoval(overlapRemoval);
  ((VBFHbbInclEvtSelection*)m_selector->getSelection())->setIsMC(m_isMC);
  
  if (applySelection && (!m_muonHandler || !m_electronHandler || !m_jetHandler)) {
    Error("initialize()", "Not all collections for event selection are defined!");
    return EL::StatusCode::FAILURE;
  }

  return EL::StatusCode::SUCCESS;
}

