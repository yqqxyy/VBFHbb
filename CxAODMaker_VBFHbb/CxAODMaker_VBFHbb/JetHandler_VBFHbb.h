// Dear emacs, this is -*-c++-*-
#ifndef CxAODMaker_JetHandler_VBFHbb_H
#define CxAODMaker_JetHandler_VBFHbb_H

#include "CxAODMaker/JetHandler.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
//#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "TrigDecisionTool/Feature.h"

namespace Trig {
  class TrigDecisionTool;
}


class JetHandler_VBFHbb : public JetHandler {

public:

  JetHandler_VBFHbb(const std::string& name, ConfigStore & config, xAOD::TEvent * event,
             EventInfoHandler & eventInfoHandler);

  void BJetTriggerMatch(xAOD::Jet * recoJet, std::string trig_type);
  void JetTriggerMatch(xAOD::Jet * recoJet);

  template<class Object, class Collection>
  const Object* getTrigObject(Trig::Feature<Collection>& feature){

    const Collection* trigCol = feature.cptr();
    if ( !trigCol ) {
      std::cout << "ERROR: No Trig Collection pointer" << std::endl;
      return 0;
    }

    if(trigCol->size() != 1){
      std::cout << "ERROR Trig Collection size " << trigCol->size() << std::endl;
      return 0;;
    }

    return trigCol->at(0);
  }

  void  set_isEmbedding(int isEmbedding)    { m_isEmbedding = isEmbedding                                                    ; } 
  int   get_isEmbedding()            { return m_isEmbedding                                                    ; }

protected:
  
  // selection functions
  bool passVetoJet(xAOD::Jet* jet);
  bool passSignalJet(xAOD::Jet* jet);
  bool checkCentralJet(xAOD::Jet* jet,bool isInCutFlow=0);

  float m_btagCut;
  bool m_isEmbedding;//!

  virtual EL::StatusCode writeCustomVariables(xAOD::Jet* inJet, xAOD::Jet* outJet, bool isKinVar, bool isWeightVar, const TString& sysName) override;
  virtual EL::StatusCode decorateOriginParticles(xAOD::JetContainer * jets) override;
  //
};

#endif
