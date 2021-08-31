// Dear emacs, this is -*-c++-*-
#ifndef CxAODMaker_FatJetHandler_VBFHbb_H
#define CxAODMaker_FatJetHandler_VBFHbb_H

#include "CxAODMaker/FatJetHandler.h"

class FatJetHandler_VBFHbb : public FatJetHandler {

public:

  FatJetHandler_VBFHbb(const std::string& name, ConfigStore & config, xAOD::TEvent * event,
                EventInfoHandler & eventInfoHandler);

private:
  bool m_writePartonTruthLabel;
  
  // selection functions
  bool passFatJet(xAOD::Jet* jet);

  virtual EL::StatusCode decorate(xAOD::Jet* jet) override;
  
  virtual EL::StatusCode writeCustomVariables(xAOD::Jet * inPart, xAOD::Jet * outPart, bool isKinVar, bool isWeightVar, const TString& sysName) override;

  EL::StatusCode initializeTools() override;

  asg::AnaToolHandle<IJetSelectorTool> m_xbbDoubleTagger; //!
};

#endif
