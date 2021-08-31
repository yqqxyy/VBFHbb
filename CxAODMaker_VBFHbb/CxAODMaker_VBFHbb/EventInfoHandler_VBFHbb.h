// Dear emacs, this is -*-c++-*-
#ifndef CxAODMaker_EventInfoHandler_VBFHbb_H
#define CxAODMaker_EventInfoHandler_VBFHbb_H

#include "CxAODMaker/EventInfoHandler.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/JetRoIAuxContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
//#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TruthWeightTools/HiggsWeightTool.h"
#include <TH2.h>

namespace Trig {
  class TrigDecisionTool;
}


// Trigger Emulation Tool
#include "TrigBtagEmulationTool/TrigBtagEmulationTool.h"

class EventInfoHandler_VBFHbb : public EventInfoHandler {

public:

  EventInfoHandler_VBFHbb(ConfigStore & config, xAOD::TEvent * event);
  
  ~EventInfoHandler_VBFHbb() {}

  virtual EL::StatusCode initialize() override;
  virtual EL::StatusCode executeEvent() override;
  virtual void writeOutputVariables(xAOD::EventInfo* eventInfoIn, xAOD::EventInfo* eventInfoOut) override;

  void  set_isEmbedding(int isEmbedding)    { m_isEmbedding = isEmbedding                                                    ; } 
  int   get_isEmbedding()            { return m_isEmbedding                                                    ; }

protected:

  Trig::TrigBtagEmulationTool *m_triggerEmulation; //! 
  TruthWeightTools::HiggsWeightTool *m_higgsMCtool_VBF;
  TruthWeightTools::HiggsWeightTool *m_higgsMCtool_ggF;

  std::unordered_map<std::string,PROP<float> *> m_trigPrescales; //!
  std::unordered_map<std::string,PROP<char> *> m_trigIsActive; //!

  bool m_doTrigEmul; //!
  bool m_isEmbedding; //!
  TH2D *m_muonEff;//!
  TH2D *m_muonTrigEff;//!
  TH2F *m_cvmfs_muonEff2016;//!
  TH2F *m_cvmfs_muonEff2017;//!
  TH2F *m_cvmfs_muonEff2018;//!

  TH2F *m_cvmfs_mu14Eff2016A_barrel;//!
  TH2F *m_cvmfs_mu14Eff2016B_barrel;//!
  TH2F *m_cvmfs_mu14Eff2016C_barrel;//!
  TH2F *m_cvmfs_mu14Eff2016D1D3_barrel;//!
  TH2F *m_cvmfs_mu14Eff2016D4D8_barrel;//!
  TH2F *m_cvmfs_mu14Eff2016E_barrel;//!
  TH2F *m_cvmfs_mu14Eff2016F_barrel;//!
  TH2F *m_cvmfs_mu14Eff2016G_barrel;//!
  TH2F *m_cvmfs_mu14Eff2016I_barrel;//!
  TH2F *m_cvmfs_mu14Eff2016K_barrel;//!
  TH2F *m_cvmfs_mu14Eff2016L_barrel;//!
  TH2F *m_cvmfs_mu14Eff2016A_endcap;//!
  TH2F *m_cvmfs_mu14Eff2016B_endcap;//!
  TH2F *m_cvmfs_mu14Eff2016C_endcap;//!
  TH2F *m_cvmfs_mu14Eff2016D1D3_endcap;//!
  TH2F *m_cvmfs_mu14Eff2016D4D8_endcap;//!
  TH2F *m_cvmfs_mu14Eff2016E_endcap;//!
  TH2F *m_cvmfs_mu14Eff2016F_endcap;//!
  TH2F *m_cvmfs_mu14Eff2016G_endcap;//!
  TH2F *m_cvmfs_mu14Eff2016I_endcap;//!
  TH2F *m_cvmfs_mu14Eff2016K_endcap;//!
  TH2F *m_cvmfs_mu14Eff2016L_endcap;//!
  TH2F *m_cvmfs_mu50Eff2016A_barrel;//!
  TH2F *m_cvmfs_mu50Eff2016B_barrel;//!
  TH2F *m_cvmfs_mu50Eff2016C_barrel;//!
  TH2F *m_cvmfs_mu50Eff2016D1D3_barrel;//!
  TH2F *m_cvmfs_mu50Eff2016D4D8_barrel;//!
  TH2F *m_cvmfs_mu50Eff2016E_barrel;//!
  TH2F *m_cvmfs_mu50Eff2016F_barrel;//!
  TH2F *m_cvmfs_mu50Eff2016G_barrel;//!
  TH2F *m_cvmfs_mu50Eff2016I_barrel;//!
  TH2F *m_cvmfs_mu50Eff2016K_barrel;//!
  TH2F *m_cvmfs_mu50Eff2016L_barrel;//!
  TH2F *m_cvmfs_mu50Eff2016A_endcap;//!
  TH2F *m_cvmfs_mu50Eff2016B_endcap;//!
  TH2F *m_cvmfs_mu50Eff2016C_endcap;//!
  TH2F *m_cvmfs_mu50Eff2016D1D3_endcap;//!
  TH2F *m_cvmfs_mu50Eff2016D4D8_endcap;//!
  TH2F *m_cvmfs_mu50Eff2016E_endcap;//!
  TH2F *m_cvmfs_mu50Eff2016F_endcap;//!
  TH2F *m_cvmfs_mu50Eff2016G_endcap;//!
  TH2F *m_cvmfs_mu50Eff2016I_endcap;//!
  TH2F *m_cvmfs_mu50Eff2016K_endcap;//!
  TH2F *m_cvmfs_mu50Eff2016L_endcap;//!

  //  xAOD::JetRoIContainer *uniql1jets; //= new xAOD::JetRoIContainer();
  //  xAOD::JetRoIAuxContainer *uniql1jetsaux; //= new xAOD::JetRoIAuxContainer();


};

#endif
