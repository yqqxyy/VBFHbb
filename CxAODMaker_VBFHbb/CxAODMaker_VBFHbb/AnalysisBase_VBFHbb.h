#ifndef CxAODMaker_AnalysisBase_VBFHbb_H
#define CxAODMaker_AnalysisBase_VBFHbb_H

#include "CxAODMaker/AnalysisBase.h"
#include "CxAODMaker_VBFHbb/EventInfoHandler_VBFHbb.h"

class AnalysisBase_VBFHbb : public AnalysisBase {

protected:
  virtual EL::StatusCode initializeSampleInfo() override;
  virtual EL::StatusCode initializeHandlers() override;
  virtual EL::StatusCode initializeSelection() override;
  
public: 
  AnalysisBase_VBFHbb()
    : AnalysisBase() {}

  ~AnalysisBase_VBFHbb() {}

  ClassDefOverride(AnalysisBase_VBFHbb, 1);
  bool m_isEmbedding;//!
};

#endif
