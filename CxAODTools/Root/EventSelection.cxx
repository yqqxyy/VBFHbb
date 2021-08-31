#include "CxAODTools/EventSelection.h"

EventSelection::EventSelection(const TString cutFlowName ) : 
  m_cutFlow(cutFlowName),
  m_metMJCalc(0),
  m_jetPtCut(25.0e3),
  m_jetRapidityCut(2.5),
  m_lepPtCut(25.0e3),
  m_config(nullptr),
  m_RecoToTruthLinkName("")
{}

EventSelection::EventSelection( ConfigStore *config, const TString cutFlowName ) : 
  m_cutFlow(cutFlowName),
  m_metMJCalc(0),
  m_jetPtCut(25.0e3),
  m_jetRapidityCut(2.5),
  m_lepPtCut(25.0e3),
  m_RecoToTruthLinkName("")
{
  if( !config )
  {
    Error("EventSelection()", "config is a null pointer"); 
  }
  //Initialise configuration store member variable
  m_config = config;
  m_config->getif<std::string>("RecoToTruthLinkName", m_RecoToTruthLinkName); 
}

bool EventSelection::passJetCleaning(const xAOD::JetContainer* /*jets*/, const xAOD::EventInfo* evtinfo){

  //https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/HowToCleanJetsR21

  if(!Props::DFCommonJets_eventClean_LooseBad.get(evtinfo)){
    return false;
  }
  if(Props::isCrackVetoClean.exists(evtinfo)){
    if(Props::isCrackVetoClean.get(evtinfo)==0){
      return false;
    }
  }

  return true;
}
