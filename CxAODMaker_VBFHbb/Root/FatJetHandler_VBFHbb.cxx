#include <iostream>

#include "CxAODMaker_VBFHbb/FatJetHandler_VBFHbb.h"

#include "CxAODTools_VBFHbb/CommonProperties_VBFHbb.h"

static SG::AuxElement::ConstAccessor<ElementLink<xAOD::JetContainer> > ungroomedLink("Parent");
static SG::AuxElement::ConstAccessor<int> partonTruthLabelID("PartonTruthLabelID");

FatJetHandler_VBFHbb::FatJetHandler_VBFHbb(const std::string& name, ConfigStore & config, xAOD::TEvent * event,
                             EventInfoHandler & eventInfoHandler) : 
  FatJetHandler(name, config, event, eventInfoHandler),
  m_writePartonTruthLabel(false)
{
  using std::placeholders::_1;
  // clear to get rid of ObjectHandler::passPreSel (all jets pass)
  m_selectFcns.clear();
  // add back in the functions from base class. passPreSel is set there with configurable pT and eta cuts !
  m_selectFcns.push_back(std::bind(&FatJetHandler::passPreSelection, this, _1));

  if (m_isMC) {
    m_config.getif<bool>("writePartonTruthIDInFatJets", m_writePartonTruthLabel);
  }
}

EL::StatusCode FatJetHandler_VBFHbb::decorate(xAOD::Jet* jet)
{
  if (m_writePartonTruthLabel && m_isMC) {
    int ptlID = 0;
    auto parent = ungroomedLink(*jet);
    if (parent.isValid()) ptlID = partonTruthLabelID(**parent);
    Props::PartonTruthLabelIDFromUngrJet.set(jet, ptlID);
  }
  if (m_xbbTag) {
    //could get detailed result from xbb tagger; satisfied with pure pass/fail decision for now
    const Root::TAccept& xbb_result = m_xbbDoubleTagger->tag(*jet); 
    Props::xbbDoubleResult.set(jet, (bool)xbb_result);
  }

  return FatJetHandler::decorate(jet);
}

EL::StatusCode FatJetHandler_VBFHbb::writeCustomVariables(xAOD::Jet* inJet, xAOD::Jet* outJet, bool isKinVar, bool isWeightVar, const TString& /*sysName*/) 
{
  if( !isKinVar && !isWeightVar ){
    Props::nGroomedTracks.copy(inJet, outJet);
    Props::NumTrkPt500PV.copy(inJet, outJet);
    Props::NumTrkPt1000PV.copy(inJet, outJet);
    if(m_saveNonCrucialVariables){
      Props::UngroomedTrackWidth500.copy(inJet, outJet);
      Props::UngroomedTrackWidth1000.copy(inJet, outJet);
    }

    if (m_writePartonTruthLabel) {
      Props::PartonTruthLabelIDFromUngrJet.copy(inJet, outJet);
    }
    if (m_xbbTag) {
      Props::xbbDoubleResult.copy(inJet, outJet);
    }
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode FatJetHandler_VBFHbb::initializeTools()
{
  FatJetHandler::initializeTools();
  if (m_xbbTag){
    ASG_SET_ANA_TOOL_TYPE( m_xbbDoubleTagger, BoostedXbbTagger);
    m_xbbDoubleTagger.setName("xbbDouble");
    //TOOL_CHECK("FatJetHandler_VBFHbb::initializeTools()", m_xbbDoubleTagger.setProperty("ConfigFile", "XbbTagger/XbbTagger_AntiKt10LCTopoTrimmed_2BTag_MC15c_20161118.dat"));
    //TOOL_CHECK("FatJetHandler_VBFHbb::initializeTools()", m_xbbDoubleTagger.setProperty("DecorationName","XbbTagTwoTag"));
    TOOL_CHECK("FatJetHandler_VBFHbb::initializeTools()", m_xbbDoubleTagger.setProperty("NumBTags",2));
    TOOL_CHECK("FatJetHandler_VBFHbb::initializeTools()", m_xbbDoubleTagger.setProperty("BTagCut",0.83));
    TOOL_CHECK("FatJetHandler_VBFHbb::initializeTools()", m_xbbDoubleTagger.setProperty("JetMassMin",0));
    TOOL_CHECK("FatJetHandler_VBFHbb::initializeTools()", m_xbbDoubleTagger.setProperty("JetMassMax",7000));
    //TOOL_CHECK("FatJetHandler_VBFHbb::initializeTools()", m_xbbDoubleTagger.setProperty("MuonCorrectionScheme","Combined"));
    TOOL_CHECK("FatJetHandler_VBFHbb::initializeTools()", m_xbbDoubleTagger.setProperty("DecorateJet",true));
    TOOL_CHECK("FatJetHandler_VBFHbb::initializeTools()", m_xbbDoubleTagger.retrieve());
  }
  return EL::StatusCode::SUCCESS;
}

