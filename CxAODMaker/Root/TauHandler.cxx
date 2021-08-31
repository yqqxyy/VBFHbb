#include <iostream>
//tau includes
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

#include "CxAODMaker/TauHandler.h"
#include "CxAODMaker/TruthProcessor.h"
#include "CxAODMaker/EventInfoHandler.h"
#include "CxAODTools/PUReweightingTool.h"

#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauTruthMatchingTool.h"
#include "TrigTauMatching/TrigTauMatching.h"
//#include "AsgTools/ToolHandle.h"

TauHandler::TauHandler(const std::string& name, ConfigStore & config, xAOD::TEvent *event,
                         EventInfoHandler & eventInfoHandler) :
  ObjectHandler(name, config, event, eventInfoHandler),
  m_antitau(false),
  m_truthproc(0),
  m_SearchedTrueTaus(false),
  m_runTruthMatchTool(false),
  m_useTausInMET(false),
  m_trigDecTool(nullptr),
  m_tauIDWP("medium"),
  m_writeRNNVars(false),
  m_useRNNTaus(false),
  m_noLowerTauIDCut(false),
  m_lowerTauIDCut(0.05),
  m_suppressTauCalibration(false),
  m_doTrigMatch(false),
  m_AllowMatchingResurrectedTrigger(false),
  m_SkipTruthMatch(false),
  m_triggersToMatch(),
  m_match_Tool(nullptr)
{
  using std::placeholders::_1;
  m_selectFcns.clear();
  m_selectFcns.push_back(std::bind(&TauHandler::passLooseTau, this, _1));

  m_config.getif<std::string>("TauIDWP", m_tauIDWP);
  m_config.getif<bool>("runTauTruthMatchTool",m_runTruthMatchTool);
  m_config.getif<bool>("useTausInMET",m_useTausInMET);
  m_config.getif<bool>("writeRNNVars",m_writeRNNVars);
  m_config.getif<bool>("useRNNTaus",m_useRNNTaus);

  m_config.getif<bool>("AntiTau",m_antitau);

  // Options to adjust the lower Tau-ID cut on anti-taus
  m_config.getif<float>("LowerTauIDCut",m_lowerTauIDCut);
  m_config.getif<bool>("NoLowerTauIDCut",m_noLowerTauIDCut);
  // BDT / RNN-score is in [0, 1] so 'id_score > -1' is always true
  if (m_noLowerTauIDCut) { m_lowerTauIDCut = -1; }
  if (m_antitau) {
    Info("TauHandler::TauHandler()", "Lower ID cut for anti-taus set to %f", m_lowerTauIDCut);
  }

  m_config.getif<bool>("suppressTauCalibration",m_suppressTauCalibration);
  m_config.getif<bool>("SkipTruthMatch",m_SkipTruthMatch);
  m_config.getif<bool>("AllowMatchingResurrectedTrigger",m_AllowMatchingResurrectedTrigger);
}

TauHandler::~TauHandler() {
}


EL::StatusCode TauHandler::initializeTools()
{
  // ===================
  // Setup Tau-ID levels
  // ===================

  // ID threshold for signal taus
  if (m_tauIDWP == "tight") {
    if (m_useRNNTaus) {
      m_TAT_IDLevel = TauAnalysisTools::JETIDRNNTIGHT;
      m_xAOD_IDLevel = xAOD::TauJetParameters::IsTauFlag::JetRNNSigTight;
      Info("TauHandler::initializeTools()", "Setting up TauHandler with RNN tight Tau-ID");
    } else {
      m_TAT_IDLevel = TauAnalysisTools::JETIDBDTTIGHT;
      m_xAOD_IDLevel = xAOD::TauJetParameters::IsTauFlag::JetBDTSigTight;
      Info("TauHandler::initializeTools()", "Setting up TauHandler with BDT tight Tau-ID");
    }
  } else if (m_tauIDWP == "medium") {
    if (m_useRNNTaus) {
      m_TAT_IDLevel = TauAnalysisTools::JETIDRNNMEDIUM;
      m_xAOD_IDLevel = xAOD::TauJetParameters::IsTauFlag::JetRNNSigMedium;
      Info("TauHandler::initializeTools()", "Setting up TauHandler with RNN medium Tau-ID");
    } else {
      m_TAT_IDLevel = TauAnalysisTools::JETIDBDTMEDIUM;
      m_xAOD_IDLevel = xAOD::TauJetParameters::IsTauFlag::JetBDTSigMedium;
      Info("TauHandler::initializeTools()", "Setting up TauHandler with BDT medium Tau-ID");
    }
  } else if (m_tauIDWP == "loose") {
    if (m_useRNNTaus) {
      m_TAT_IDLevel = TauAnalysisTools::JETIDRNNLOOSE;
      m_xAOD_IDLevel = xAOD::TauJetParameters::IsTauFlag::JetRNNSigLoose;
      Info("TauHandler::initializeTools()", "Setting up TauHandler with RNN loose Tau-ID");
    } else {
      m_TAT_IDLevel = TauAnalysisTools::JETIDBDTLOOSE;
      m_xAOD_IDLevel = xAOD::TauJetParameters::IsTauFlag::JetBDTSigLoose;
      Info("TauHandler::initializeTools()", "Setting up TauHandler with BDT loose Tau-ID");
    }
  } else {
    Error("TauHandler::initializeTools()", "TauIDWP property is only supporting 'tight', 'medium' and 'loose'");
    return EL::StatusCode::FAILURE;
  }

  // Init Calibrationtool
  // _____________________________________________
  m_tauSmearingTool.reset(new TauAnalysisTools ::TauSmearingTool("TauSmearTool"));
  m_tauSmearingTool->msg().setLevel( m_msgLevel );
  if(m_SkipTruthMatch) TOOL_CHECK("TauHandler::initializeTools()",m_tauSmearingTool->setProperty("SkipTruthMatchCheck",true));
  TOOL_CHECK("TauHandler::initializeTools()",m_tauSmearingTool->setProperty("RecommendationTag","2019-summer"));
  TOOL_CHECK("TauHandler::initializeTools()",m_tauSmearingTool->initialize());

  // Init TauSelector
  // _____________________________________________

  // Raise an error if we set fake since this shouldn't be used anymore
  bool fake = false;
  m_config.getif<bool>("Fake", fake);
  if (fake) {
    Error("TauHandler::initializeTools()", "The 'fake' flag is deprecated and should always be false.");
    return EL::StatusCode::FAILURE;
  }

  const int ptag=std::stoi(m_eventInfoHandler.get_pTag());
  // TauSelectionTool configuration
  // Following tau selection recommended by TauCP
  const double ptMin = 20.;
  const std::vector<double> absEtaRegion = {0, 1.37, 1.52, 2.5};
  const int absCharge = 1;
  const std::vector<size_t> nTracks = {1, 3};
  bool eleOLR = false;
  int eleBDTWP = -1;
  if (ptag >= 3759) {
    eleBDTWP = TauAnalysisTools::ELEIDBDTLOOSE;
  } else {
    // SFs for ptag < 3759 provided only for eleOLR or eleOLR + old eVeto together
    eleBDTWP = TauAnalysisTools::ELEIDBDTOLDLOOSE;
    eleOLR = true;
  }

  m_tauSelectionTool.reset(new TauAnalysisTools::TauSelectionTool("TauSelectTool"));
  m_tauSelectionTool->msg().setLevel( m_msgLevel );

  int selectionCuts = TauAnalysisTools::CutPt
    | TauAnalysisTools::CutAbsEta
    | TauAnalysisTools::CutAbsCharge
    | TauAnalysisTools::CutNTrack
    | TauAnalysisTools::CutEleOLR
    | TauAnalysisTools::CutEleBDTWP;

  TOOL_CHECK("TauHandler::initializeTools()",
             m_tauSelectionTool->setProperty("PtMin", ptMin));
  TOOL_CHECK("TauHandler::initializeTools()",
             m_tauSelectionTool->setProperty("AbsEtaRegion", absEtaRegion));
  TOOL_CHECK("TauHandler::initializeTools()",
             m_tauSelectionTool->setProperty("AbsCharge", absCharge));
  TOOL_CHECK("TauHandler::initializeTools()",
             m_tauSelectionTool->setProperty("NTracks", nTracks));
  TOOL_CHECK("TauHandler::initializeTools()",
             m_tauSelectionTool->setProperty("EleOLR", eleOLR));
  TOOL_CHECK("TauHandler::initializeTools()",
             m_tauSelectionTool->setProperty("EleBDTWP", eleBDTWP));

  if (m_antitau) {
    // Lower Tau-ID cut for antitau selection
    std::string propertyName;

    if (m_useRNNTaus) {
      selectionCuts |= TauAnalysisTools::CutJetRNNScoreSigTrans;
      propertyName = "JetRNNSigTransMin";
    } else {
      selectionCuts |= TauAnalysisTools::CutJetBDTScoreSigTrans;
      propertyName = "JetBDTSigTransMin";
    }
    TOOL_CHECK("TauHandler::initializeTools()",
               m_tauSelectionTool->setProperty(propertyName.c_str(),
                                               static_cast<double>(m_lowerTauIDCut)));
  } else {
    // WP for signal tau selection
    selectionCuts |= TauAnalysisTools::CutJetIDWP;
    TOOL_CHECK("TauHandler::initializeTools()",
               m_tauSelectionTool->setProperty("JetIDWP", m_TAT_IDLevel));
  }

  TOOL_CHECK("TauHandler::initializeTools()",
             m_tauSelectionTool->setProperty("SelectionCuts", selectionCuts));

  // Disable default config file
  TOOL_CHECK("TauHandler::initializeTools()",
             m_tauSelectionTool->setProperty("ConfigPath", ""));

  TOOL_CHECK("TauHandler::initializeTools()",
             m_tauSelectionTool->initialize());

  // Init ID Correction
  // _____________________________________________
  ToolHandle<TauAnalysisTools::ITauSelectionTool> TauSelToolHandle = m_tauSelectionTool.get();
  PUReweightingTool* puTool = m_eventInfoHandler.get_PUReweightingTool();
  ToolHandle<CP::IPileupReweightingTool> m_tPRWToolHandle = puTool->getPileupReweightingTool().getHandle();

  m_tauEffCorr.reset(new TauAnalysisTools::TauEfficiencyCorrectionsTool("TauEfficiencyTool"));
  m_tauEffCorr->msg().setLevel( m_msgLevel );
  TOOL_CHECK("TauHandler::initializeTools()",m_tauEffCorr->setProperty("IDLevel", m_TAT_IDLevel));
  if (ptag >= 3759) {
    TOOL_CHECK("TauHandler::initializeTools()",m_tauEffCorr->setProperty("OLRLevel", (int)TauAnalysisTools::ELEBDTLOOSE));
  } else {
    TOOL_CHECK("TauHandler::initializeTools()",m_tauEffCorr->setProperty("OLRLevel", (int)TauAnalysisTools::ELEBDTLOOSEPLUSVETO));
  }
  TOOL_CHECK("TauHandler::initializeTools()",m_tauEffCorr->setProperty("RecommendationTag","2019-summer"));

  //This overwrites the IDLevel
  //TOOL_CHECK("TauHandler::initializeTools()",m_tauEffCorrLoose->setProperty("TauSelectionTool", TauSelToolHandle));
  TOOL_CHECK("TauHandler::initializeTools()",m_tauEffCorr->setProperty("PileupReweightingTool", m_tPRWToolHandle ));
  TOOL_CHECK("TauHandler::initializeTools()",m_tauEffCorr->initialize());

  // =======================
  // Tau truth-matching tool
  // =======================
  if (m_runTruthMatchTool && m_eventInfoHandler.get_isMC()) {
    m_tauTruthMatchingTool.reset(new TauAnalysisTools::TauTruthMatchingTool("T2MT"));
    m_tauTruthMatchingTool->msg().setLevel( m_msgLevel );
    TOOL_CHECK("TauHandler::initializeTools()",m_tauTruthMatchingTool->setProperty("MaxDeltaR",0.2)); // TODO make this configurable
    TOOL_CHECK("TauHandler::initializeTools()",m_tauTruthMatchingTool->setProperty("WriteTruthTaus",true));
    TOOL_CHECK("TauHandler::initializeTools()",m_tauTruthMatchingTool->initialize());
  }

  //retrieve TrigDecTool from EventInfo
  //------------------------------------
  m_trigDecTool = m_eventInfoHandler.get_TrigDecTool();

  //initialize tau trigger matching
  //--------------------------------
  if(m_trigDecTool){
    ToolHandle<Trig::TrigDecisionTool> m_trigDec(m_trigDecTool);

    m_match_Tool = new Trig::TrigTauMatchingTool("TauMatch");
    m_match_Tool->msg().setLevel( m_msgLevel );
    if(m_AllowMatchingResurrectedTrigger) TOOL_CHECK("TauHandler::initializeTools()",m_match_Tool->setProperty("AllowResurrectedDecision",true));
    TOOL_CHECK("TauHandler::initializeTools()",m_match_Tool->setProperty("TrigDecisionTool",m_trigDec));
    TOOL_CHECK("TauHandler::initializeTools()",m_match_Tool->initialize());
  }

  std::unordered_map<std::string, PROP<int>*> triggersToMatch;
  m_triggersToMatch = triggersToMatch;

  // register ISystematicsTools
  // _____________________________________________
  m_sysToolList.clear();
  m_sysToolList.push_back(m_tauEffCorr.get());
  m_sysToolList.push_back(m_tauSmearingTool.get());

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode TauHandler::writeOutputVariables(xAOD::TauJet * inTau, xAOD::TauJet * outTau, bool isKinVar, bool isWeightVar, const TString& sysName)
{

  if (isKinVar || isWeightVar) {
    // Nominal Not, Kinematic Yes, Weight Yes
    EL_CHECK("TauHandler::writeOutputVariables",writeAllVariations(inTau, outTau, sysName));
  }
  if (isKinVar && !isWeightVar) {
    // Nominal Not, Kinematic Yes, Weight Not
    EL_CHECK("TauHandler::writeOutputVariables",writeKinematicVariations(inTau, outTau, sysName));
  } else if (!isKinVar && isWeightVar) {
    // Nominal Not, Kinematic Not, Weight Yes
    EL_CHECK("TauHandler::writeOutputVariables",writeWeightVariations(inTau, outTau, sysName));
  } else if (!isKinVar && !isWeightVar) {
    // Nominal Yes, Kinematic Not, Weight Not
    // for nominal we save all of them
    // in order to not copy paste code in both up and here in Nominal
    // and later to update one and forget the other
    // the functions were created to be easily read by name and edited
    EL_CHECK("TauHandler::writeOutputVariables",writeAllVariations(inTau, outTau, sysName));
    EL_CHECK("TauHandler::writeOutputVariables",writeKinematicVariations(inTau, outTau, sysName));
    EL_CHECK("TauHandler::writeOutputVariables",writeWeightVariations(inTau, outTau, sysName));
    EL_CHECK("TauHandler::writeOutputVariables",writeNominal(inTau, outTau, sysName));
  } else assert(false);

  return writeCustomVariables(inTau, outTau, isKinVar, isWeightVar, sysName);

}

EL::StatusCode TauHandler::writeAllVariations(xAOD::TauJet*, xAOD::TauJet*, const TString&)
{
  //
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TauHandler::writeKinematicVariations(xAOD::TauJet* inTau, xAOD::TauJet* outTau, const TString& /*sysName*/)
{
  // set four momentum
  setP4( inTau , outTau );

  Props::isAntiTau.copy(inTau, outTau);
  Props::passTauSelector.copy(inTau, outTau);
  Props::TruthMatch.copy(inTau, outTau);
  Props::LeptonTruthMatch.copy(inTau, outTau);
  if(m_runTruthMatchTool) {
    Props::TATTruthMatch.copy(inTau, outTau);
  }
  //
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TauHandler::writeWeightVariations(xAOD::TauJet* inTau, xAOD::TauJet* outTau, const TString& /*sysName*/)
{
  if (m_eventInfoHandler.get_isMC()) {
    Props::effSF.copy(inTau, outTau);
  }
  //
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TauHandler::writeNominal(xAOD::TauJet* inTau, xAOD::TauJet* outTau, const TString& /*sysName*/)
{
  outTau->setCharge(inTau->charge());
  //
  Props::nTracks.copy(inTau, outTau);
  Props::tauTJVAIndex.copy(inTau, outTau);
  Props::isBDTVeryLoose.copy(inTau, outTau);
  Props::isBDTLoose.copy(inTau, outTau);
  Props::isBDTMedium.copy(inTau, outTau);
  Props::isBDTTight.copy(inTau, outTau);
  Props::charge.copy(inTau, outTau); // TODO: remove this? It's redundant as charge is already a member of the class
  Props::BDTScoreDeprecated.copy(inTau, outTau);
  Props::BDTScore.copy(inTau, outTau);

  Props::isRNNVeryLoose.copyIfExists(inTau, outTau);
  Props::isRNNLoose.copyIfExists(inTau, outTau);
  Props::isRNNMedium.copyIfExists(inTau, outTau);
  Props::isRNNTight.copyIfExists(inTau, outTau);
  Props::RNNScoreDeprecated.copyIfExists(inTau, outTau);
  Props::RNNScore.copyIfExists(inTau, outTau);

  //
  Props::EleBDTScore.copy(inTau, outTau);
  Props::EleORBDTscore.copy(inTau, outTau);
  Props::EleORLH.copy(inTau, outTau);
  Props::TrackEta.copy(inTau, outTau);
  //
  // trigger Props are independent of energy scale (at least for now)
  if ( m_doTrigMatch ) {
    const auto& trigDecorators = m_eventInfoHandler.get_trigDecorators();
    for(auto& trig : m_triggersToMatch) {
      if(trigDecorators.count(trig.first)) {
        trig.second->copy(inTau, outTau);
      }
    }
  }

  //Write Truth Match property
  Props::TruthMatch.copyIfExists(inTau, outTau);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TauHandler::writeCustomVariables(xAOD::TauJet*, xAOD::TauJet*, bool, bool, const TString&)
{
  // This method is meant to be overridden in derived handlers for writing
  // additional decorations. Argument names are omitted here to avoid warnings.
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TauHandler::decorateOriginParticle(const xAOD::TauJet * tau)
{
  // This method should only be applied to things that don't depend on the calibration

  //bool medium = tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose);

  m_doTrigMatch = true;
  if (m_event->contains<xAOD::TauJetContainer>("HLT_xAOD__TauJetContainer_TrigTauRecMerged")) {
    m_doTrigMatch = true;
  }

  // tau trigger matching is independent of pT -> independent of calib
  if ( m_doTrigMatch ) {
    const auto& trigDecorators = m_eventInfoHandler.get_trigDecorators();
    std::string trig_name;

    // only call the trigger matching for the triggers that really need it !
    // loop over the triggers we declared as match-able for taus
    for (auto& trig : m_triggersToMatch) {
      // check if it is in the trigDecorators. Maybe we don't want this trigger.

      if(trigDecorators.count(trig.first)) {
        trig_name = trig.first;

        int is_matched = 0;
        // Do not do trigger matching if the tau is not accepted -> now need to for antitaus
        //if(medium) {
  if (m_match_Tool->match(tau,trig_name)) is_matched = 1;
        //}
        trig.second->set(tau, is_matched);
      }
    }
  }

  // needed in case decorations are not in the derivation
  if (m_runTruthMatchTool && m_eventInfoHandler.get_isMC()) {
    m_tauTruthMatchingTool->applyTruthMatch(*tau);
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TauHandler::addCPVariations(const std::vector<TString> &variations,
             const bool filterByTools, const bool /*skipWeightVar*/) {

  // add the usual variations, filtered by registered tools if requested
  EL_CHECK("TauHandler::addCPVariations()", ObjectHandlerBase::addCPVariations(variations, filterByTools));
  // see if we want some additional ones
  std::vector<TString> addVariations;  for (const TString &variation : variations) {
    bool allow = false;
    allow |= variation.EqualTo("ANTITAU_BDT_CUT");
    if (!allow) continue;
    addVariations.push_back(variation);
  }

  // add them to the list w/o further filtering
  return ObjectHandlerBase::addCPVariations(addVariations, false);
}

EL::StatusCode TauHandler::decorate(xAOD::TauJet * tau)
{

  /* Set tau variables - derived ones*/

  Props::nTracks.set(tau, tau->nTracks());

  float leadtrketa=-100;
  if (tau->nTracks() > 0){
    leadtrketa = tau->track(0)->eta();
  }
  Props::TrackEta.set(tau, leadtrketa );

  // BDT-based ID
  const bool isBDTVeryLoose = tau->isTau(xAOD::TauJetParameters::JetBDTSigVeryLoose);
  const bool isBDTLoose = tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose);
  const bool isBDTMedium = tau->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
  const bool isBDTTight = tau->isTau(xAOD::TauJetParameters::JetBDTSigTight);

  const float bdtscore_deprecated = tau->discriminant(xAOD::TauJetParameters::BDTJetScore);
  const float bdtscore = tau->discriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans);

  Props::isBDTVeryLoose.set(tau, isBDTVeryLoose);
  Props::isBDTLoose.set(tau, isBDTLoose);
  Props::isBDTMedium.set(tau, isBDTMedium);
  Props::isBDTTight.set(tau, isBDTTight);

  Props::BDTScoreDeprecated.set(tau, bdtscore_deprecated);
  Props::BDTScore.set(tau, bdtscore);

  // RNN-based ID (only available in newer derivations)
  static SG::AuxElement::Accessor<float> rnn_acc("RNNJetScore");
  if ((m_writeRNNVars || m_useRNNTaus) && rnn_acc.isAvailable(*tau)) {
    const bool isRNNVeryLoose = tau->isTau(xAOD::TauJetParameters::JetRNNSigVeryLoose);
    const bool isRNNLoose = tau->isTau(xAOD::TauJetParameters::JetRNNSigLoose);
    const bool isRNNMedium = tau->isTau(xAOD::TauJetParameters::JetRNNSigMedium);
    const bool isRNNTight = tau->isTau(xAOD::TauJetParameters::JetRNNSigTight);

    float rnnscore_deprecated = tau->discriminant(xAOD::TauJetParameters::RNNJetScore);
    float rnnscore = tau->discriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans);

    Props::isRNNVeryLoose.set(tau, isRNNVeryLoose);
    Props::isRNNLoose.set(tau, isRNNLoose);
    Props::isRNNMedium.set(tau, isRNNMedium);
    Props::isRNNTight.set(tau, isRNNTight);

    Props::RNNScoreDeprecated.set(tau, rnnscore_deprecated);
    Props::RNNScore.set(tau, rnnscore);
  }

  float bdtelescore= tau->discriminant(xAOD::TauJetParameters::BDTEleScore);
  Props::EleBDTScore.set(tau, bdtelescore);

  float eleORBDTscore= tau->auxdata<float>("BDTEleScoreSigTrans");
  Props::EleORBDTscore.set(tau, eleORBDTscore);

  float eleORLH= tau->auxdata<float>("EleMatchLikelihoodScore");
  Props::EleORLH.set(tau,eleORLH);

  bool accept = static_cast<bool>(m_tauSelectionTool->accept( *tau ));
  Props::passTauSelector.set(tau, accept);

  int tauVertIndex = tau->vertex()->index();
  Props::tauTJVAIndex.set(tau, tauVertIndex);

  float charge=tau->charge();
  Props::charge.set(tau, charge); // TODO: Remove this? It's redundant...

  if( !Props::effSF.exists(tau) ){
    Props::effSF.set(tau, 1);
   }

  // Antitau selection
  bool isAntiTau = false;
  if (!tau->isTau(m_xAOD_IDLevel)) {
    isAntiTau = true;
  }
  Props::isAntiTau.set(tau, isAntiTau);

  EL_CHECK("TauHandler::decorate",TruthMatch(tau));
  if(m_runTruthMatchTool) {
    EL_CHECK("TauHandler::decorate",TruthMatchTAT(tau));
  }

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode TauHandler::FindTrueTaus(){
  if(! m_truthproc ) {
    Error("TauHandler::TruthMatch()", "Cannot find truthparticle handler");
    return EL::StatusCode::FAILURE;
  }

  // thats how it usually done, working on the Nominal InParticleVariation
  // Build a vector of true taus/leptons
  if (m_truthproc->isNewStyle()) {
    const xAOD::TruthParticleContainer* truthTaus =  m_truthproc -> getTaus();
    const xAOD::TruthParticleContainer* truthEle =  m_truthproc -> getElectrons();
    const xAOD::TruthParticleContainer* truthMu =  m_truthproc -> getMuons();
    const xAOD::TruthParticleContainer* truthHS =  m_truthproc -> getInParticle("HardScatterParticles");

    if (!truthTaus || !truthEle || !truthMu || !truthHS) {
      Error("TauHandler::TruthMatch()", "Cannot find nominal outParticle container for mctruth");
      return EL::StatusCode::FAILURE;
    }

    for (const xAOD::TruthParticle* part : *truthTaus) {
      if (!part) continue;
      m_truetaus.push_back(part);
      if (part->auxdata<char>("IsHadronicTau")) {
        m_truetaus_had.push_back(part);
      } else {
        m_truetaus_lep.push_back(part);
      }
    }
    for (const xAOD::TruthParticle* part : *truthEle) {
      if (!part) continue;
      m_trueeles.push_back(part);
    }
    for (const xAOD::TruthParticle* part : *truthMu) {
      if (!part) continue;
      m_truemuons.push_back(part);
    }
    // now collect quarks
    for (const xAOD::TruthParticle* part : *truthHS) {
      if (!part) continue;
      int pdgId = (int) fabs(part->pdgId()); // get it only once for the if below for speed
      // also register partons above 4 gev (pt cut to speed up processing time..)
      if ((part->pt() > 10e3) && (std::fabs(part->eta()) < 3.5) && ((pdgId > 0 && pdgId < 5) || pdgId == 21)) {
        //std::cout<<"pdg:"<< part->pdgId()<<" pt=" << part->pt() << " eta="<< part->eta() <<" status:"<<part->status()<<"children:"<<part->nChildren()<<std::endl;
        //if ( part->status() == 2 ||  part->status() == 11 ||  part->status() == 3)
        m_truepartons.push_back(part);
      }
    }
  } else {
    const xAOD::TruthParticleContainer* truthPart =  m_truthproc -> getInParticle("TruthParticles");
    if (!truthPart) {
      Error("TauHandler::TruthMatch()", "Cannot find nominal outParticle container for mctruth");
      return EL::StatusCode::FAILURE;
    }
    for (const xAOD::TruthParticle* part : *truthPart) {
      if (!part) continue;

      int pdgId = (int) fabs(part->pdgId()); // get it only once for the if below for speed
      // also register partons above 4 gev (pt cut to speed up processing time..)
      if ((part->pt() > 10e3) && (std::fabs(part->eta()) < 3.5) && ((pdgId > 0 && pdgId < 5) || pdgId == 21)) {
        //std::cout<<"pdg:"<< part->pdgId()<<" pt=" << part->pt() << " eta="<< part->eta() <<" status:"<<part->status()<<"children:"<<part->nChildren()<<std::endl;
        //if ( part->status() == 2 ||  part->status() == 11 ||  part->status() == 3)
        m_truepartons.push_back(part);
      }

      // Leptons only
      if (!(pdgId == 15 || pdgId == 11 || pdgId == 13)) continue;

      bool isEW = m_truthproc->isEWLepton(part);
      if (isEW) {
        if (pdgId == 11) m_trueeles.push_back(part);
        if (pdgId == 13) m_truemuons.push_back(part);
        if (pdgId == 15) m_truetaus.push_back(part);
      }
    }

    // Classify tau decays
    for (const xAOD::TruthParticle* part : m_truetaus) {
      if (!part) continue;
      bool ishad = true;
      for (unsigned int iChild=0; iChild< part->nChildren(); ++iChild) {
        const xAOD::TruthParticle* child = (const xAOD::TruthParticle*) part->child(iChild);
        if (!child) continue;
        int pdgId = (int) fabs(part->pdgId()); // get it only once for the if below for speed
        //if (child->status() == 3 ) continue;
        if (pdgId == 11 || pdgId == 13) {
      m_truetaus_lep.push_back(part);
    ishad = false;
    break;
        }
      }

      if(ishad) {
        m_truetaus_had.push_back(part);
      }
    }
  }

  m_SearchedTrueTaus = true;

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TauHandler::TruthMatch(xAOD::TauJet * tau){

  Props::TruthMatch.set(tau, 0);
  Props::LeptonTruthMatch.set(tau, 0);
  bool hasmatch=false;

  int isMC = m_eventInfoHandler.get_isMC();
  if (!isMC) {
    return EL::StatusCode::SUCCESS;
  }

  // only perform time consuming truth matching for tau candidates passing some basic criteria:
  if ((tau->pt() < 15e3) || (std::fabs(tau->eta()) > 2.5) || fabs(tau->charge()) != 1)
    return EL::StatusCode::SUCCESS;

  if(!m_SearchedTrueTaus){
    if(FindTrueTaus() != EL::StatusCode::SUCCESS){
      Error("in TauHandler::TruthMatch()","Failed to FindTrueTaus. Exiting.");
      return EL::StatusCode::FAILURE;
    }
  }

  // Check hadronic taus:
  float mindr = 0.2;
  for (unsigned int iPart = 0; iPart < m_truetaus_had.size(); ++iPart) {
    if ( (m_truetaus_had[iPart]->p4()).DeltaR(tau->p4()) < mindr) {
      Props::TruthMatch.set(tau, 1);
      hasmatch = true;
    }
  }// thads

  for (unsigned int iPart = 0; iPart < m_truetaus_lep.size(); ++iPart) {
    if ( (m_truetaus_lep[iPart]->p4()).DeltaR(tau->p4()) < mindr) {
      Props::TruthMatch.set(tau, 2);
      hasmatch = true;
    }
  }// tleps

  for (unsigned int iPart = 0; iPart < m_trueeles.size(); ++iPart) {
    if ( (m_trueeles[iPart]->p4()).DeltaR(tau->p4()) < mindr) {
      Props::LeptonTruthMatch.set(tau, 1); // matched to true electron
      hasmatch=true;
    }
  }// true eles

  for (unsigned int iPart = 0; iPart < m_truemuons.size(); ++iPart) {
    if ( (m_truemuons[iPart]->p4()).DeltaR(tau->p4()) < mindr) {
      Props::LeptonTruthMatch.set(tau, 2); // matched to true muon
      hasmatch=true;
    }
  }// true muons


  // look whether it has a parton match
  if (!hasmatch) {
    float highestpt=1.; // at least 1 mev please
    int partonindex=-1;
    for(unsigned int iPart = 0; iPart < m_truepartons.size(); ++iPart){

      if ( m_truepartons[iPart]->pt() >highestpt && (m_truepartons[iPart]->p4()).DeltaR(tau->p4())< 0.4 ){
  partonindex = iPart;
  highestpt = m_truepartons[iPart]->pt();
      }
    }
    if (partonindex>=0){
      int partonpdgid=fabs(m_truepartons[partonindex]->pdgId());
      Props::TruthMatch.set(tau, -(partonpdgid));
    }
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TauHandler::TruthMatchTAT(xAOD::TauJet * tau){

  int isMC = m_eventInfoHandler.get_isMC();
  if (!isMC) {
    Props::TATTruthMatch.set(tau, 0);
    return EL::StatusCode::SUCCESS;
  }


  // Fix jet link which is missing in some derivations
  if(!tau->isAvailable<ElementLink<xAOD::JetContainer>>("truthJetLink") ||
     !tau->auxdecor<ElementLink<xAOD::JetContainer>>("truthJetLink").isValid()) {
    const xAOD::JetContainer* jetContainer;
    EL_CHECK("TauHandler::TruthMatchTAT()", m_event->retrieve(jetContainer , "AntiKt4TruthJets") );
    double dPtMax = 0;
    const xAOD::Jet* xTruthJetMatch = nullptr;
    for (auto xTruthJetIt : *jetContainer) {
      if (tau->p4().DeltaR(xTruthJetIt->p4()) <= 0.2) {
  if (xTruthJetIt->pt()<dPtMax)
    continue;
  xTruthJetMatch = xTruthJetIt;
  dPtMax = xTruthJetIt->pt();
      }
    }

    if (xTruthJetMatch) {
      ElementLink < xAOD::JetContainer > lTruthParticleLink(xTruthJetMatch, *jetContainer);
      tau->auxdecor< ElementLink< xAOD::JetContainer > >("truthJetLink") = lTruthParticleLink;
    } else {
      ElementLink < xAOD::JetContainer > lTruthParticleLink;
      tau->auxdecor< ElementLink< xAOD::JetContainer > >("truthJetLink") = lTruthParticleLink;
    }
  }

  if (tau->auxdata<char>("IsTruthMatched")) {
    auto truthPart = (*tau->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink"));

    if (truthPart->isTau()) {
      bool isHadTau(false);
      try {
  isHadTau = (bool)truthPart->auxdata<char>("IsHadronicTau");
      } catch (const SG::ExcAuxTypeMismatch& e) {
  isHadTau = truthPart->auxdata<bool>("IsHadronicTau");
      }

      if (isHadTau) {
  Props::TATTruthMatch.set(tau, 15);
  return EL::StatusCode::SUCCESS;
      } else {
  if (m_tauTruthMatchingTool->getNTauDecayParticles(*tau, 11, true) > 0) {
    //tau->enunu
    Props::TATTruthMatch.set(tau, -11);
    return EL::StatusCode::SUCCESS;
  } else if (m_tauTruthMatchingTool->getNTauDecayParticles(*tau, 13, true) > 0) {
    //tau->mununu
    Props::TATTruthMatch.set(tau, -13);
    return EL::StatusCode::SUCCESS;
  }
      }
    }

    if (truthPart->isMuon()) {
      Props::TATTruthMatch.set(tau, 13);
      return EL::StatusCode::SUCCESS;
    }

    if (truthPart->isElectron()) {
      Props::TATTruthMatch.set(tau, 11);
      return EL::StatusCode::SUCCESS;
    }
  } else {

    // Jet
    const ElementLink< xAOD::JetContainer>  jet = tau->auxdecor<ElementLink<xAOD::JetContainer>>("truthJetLink");
    if (!jet.isValid()) {
      Props::TATTruthMatch.set(tau, 0); // Not matched
      return EL::StatusCode::SUCCESS;
    }

    int pdgid;
    (*jet)->getAttribute("PartonTruthLabelID", pdgid);
    Props::TATTruthMatch.set(tau, pdgid);
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TauHandler::calibrateCopies(xAOD::TauJetContainer * particles, const CP::SystematicSet & sysSet) {
  int isMC = m_eventInfoHandler.get_isMC();

  // tell tool to apply systematic variation
  CP_CHECK("TauHandler::calibrateCopies()",m_tauSmearingTool->applySystematicVariation(sysSet),m_debug);
  CP_CHECK("TauHandler::calibrateCopies()",m_tauEffCorr->applySystematicVariation(sysSet),m_debug);

  for (xAOD::TauJet * tau : *particles) {

    //calibration
    //------------
    setP4( tau , tau );

    if (!m_suppressTauCalibration) {
      CP_CHECK("TauHandler::calibrateCopies()", m_tauSmearingTool->applyCorrection(*tau), m_debug);
    }

    //props initialised just in case the decorate is not run.
    Props::forMETRebuild.set(tau, false);

    double effSF = 1.0;

    const bool accepted = static_cast<bool>(m_tauSelectionTool->accept( *tau ));
    const bool passTauID = tau->isTau(m_xAOD_IDLevel);

    if(!m_suppressTauCalibration && accepted && passTauID && isMC) {
      CP_CHECK("TauHandler::calibrateCopies()",m_tauEffCorr->getEfficiencyScaleFactor(*tau, effSF),m_debug);
    }

    Props::effSF.set(tau, effSF);

    // decorate tau
    EL_CHECK("TauHandler::calibrateCopies", decorate(tau));
  }
  return EL::StatusCode::SUCCESS;
}

bool TauHandler::passLooseTau(xAOD::TauJet * tau) {
  bool passSel = true;
  passSel &= Props::passTauSelector.get(tau);

  //useTausInMET is set in the config file for MET rebuilding. Then it includes
  //taus that have passed the default tau selection
  if (m_useTausInMET) { Props::forMETRebuild.set(tau,passSel); }

  Props::passPreSel.set(tau, passSel);
  return passSel;
}

EL::StatusCode TauHandler::clearEvent()
{
  m_truetaus_had.clear();
  m_truetaus_lep.clear();
  m_truetaus.clear();
  m_trueeles.clear();
  m_truemuons.clear();
  m_truepartons.clear();

  m_SearchedTrueTaus = false;
  EL_CHECK("TauHandler::clearEvent()",ObjectHandler::clearEvent());

  return EL::StatusCode::SUCCESS;
}

//Overload of ObjectHandlerBase link copier
EL::StatusCode TauHandler::fillOutputLinks(){

  if(m_debug){
    Info("TauHandler::fillOutputLinks", " Copying input container element links to output container (%s)", m_handlerName.c_str());
  }

  //Check that the input container is non-zero
  if( m_inContainer.size() == 0 ){ return EL::StatusCode::SUCCESS; }

  //Check that the output container is non-zero in size
  if( m_outContainer.size() == 0 ){ return EL::StatusCode::SUCCESS; }

  //Now use only the nominal input container to copy partile links
  const auto &inCont = *m_inContainer["Nominal"];

  //Write to only the nominal output container
  auto &outCont = *m_outContainer["Nominal"];

  //Create an accessor
  std::string RecoToTruthLinkName = "TruthPart";
  m_config.getif<std::string>("RecoToTruthLinkName", RecoToTruthLinkName);
  SG::AuxElement::Accessor< ElementLink< const xAOD::TruthParticleContainer > > truthPartElAcc (RecoToTruthLinkName);

  //Loop through the electrons in the output container
  for( unsigned int j = 0; j < outCont.size(); ++j){

    //Get the particle index from the input container
    unsigned int index = Props::partIndex.get(outCont.at(j));

    //Check that the electron is truth matched
    if ( !Props::TruthMatch.exists(inCont.at(index)) || !Props::TruthMatch.get(inCont.at(index))
   || !(truthPartElAcc.isAvailable( *(inCont.at(index)) )) ) continue;

    //Extract the element link for the current electron
    ElementLink< const xAOD::TruthParticleContainer> el = truthPartElAcc(*(inCont.at(index)));

    //Now assign to the output particle
    truthPartElAcc( *(outCont.at(j)) ) = el;


    //=============== Alterantively use the methods in ParticleLinker.cxx class =================
    //Otherwise copy particle link to output container
    //m_linker->copyParticleLinks(*(inCont.at(j)), *(outCont.at(j)), "TruthPart", m_handlerName);
    //===========================================================================================

  }

  return EL::StatusCode::SUCCESS;

}
