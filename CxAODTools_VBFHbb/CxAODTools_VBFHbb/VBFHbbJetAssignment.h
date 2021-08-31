
#ifndef CxAODTools__VBFHbb_JetAssignment_H
#define CxAODTools__VBFHbb_JetAssignment_H

#include "vector"
#include "xAODJet/Jet.h"
#include "xAODEventInfo/EventInfo.h"
#include "EventLoop/Algorithm.h"
#include "TRandom3.h"

class jetAssignmentTool {
 public:
  jetAssignmentTool();
  jetAssignmentTool(jetAssignmentTool const&);
  ~jetAssignmentTool();

  EL::StatusCode setAssignmentType(std::string);
  EL::StatusCode setIsMC(bool);
  void setRunNumber(UInt_t runNumber) {m_runNumber = runNumber;}
  EL::StatusCode setSelection(std::string selection);
  void setDebugMode();

  int assignJets(std::vector<xAOD::Jet const*> const&, const xAOD::EventInfo*);

  const std::string assignmentType() const;
  std::vector<xAOD::Jet const*> signalJets() const;
  std::vector<xAOD::Jet const*> vbfJets() const;

 protected:
  void clear();
  void printResults();
  int assignCMS(std::vector<xAOD::Jet const*>&);
  int assignBtag_singleTagForward(std::vector<xAOD::Jet const*> &inputJets, std::string WP);
  int assignBtag_0for(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo*,bool dotrig = true);
  int assignBtag_fake(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo*);
  int assignBtag_fake3(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo*);
  int assignBtag_fake4(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo*);
  int assignBtag_1for3cen(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo*);
  int assignBtag_1forAlt3cen(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo*);
  int assignBtag_1for2cen(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo*, bool allowFF = true,bool dotrig = true);
  int assignBtag_2for(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo*);
  int assignBtag_topoCF(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info);
  int assignBtag_topoInc(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info);
  int assignBtag_inclusive(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info);
  int assignBtag_3cen(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info,bool dotrig = true);
  int assignBtag_3cen_maxptjj(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info,bool dotrig = true);
  int assignBtag_3jet(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info);
  int assignBtag_single(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info);
  int assignBtag_trigger(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info);
  int assignBtag_1for2cen_triggerJ1(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info, bool allowFF = true,bool dotrig = true);
  int assignBtag_1for2cen_triggerB2(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info, bool allowFF = true,bool dotrig = true);
  int assignBtag_3cen_triggerJ1(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info,bool dotrig = true);
  int assignBtag_3cen_triggerB2(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info,bool dotrig = true);

 private:
  std::string _type;
  bool _debugMode;

  std::vector<xAOD::Jet const*> _signalJets;
  std::vector<xAOD::Jet const*> _vbfJets;
  UInt_t m_runNumber;
  std::string _Selection;
  double m_b_85_weight = 0.07;
  double m_b_77_weight = 0.63;
  double m_b_70_weight = 0.83;
  double m_b_60_weight = 0.94;
  TRandom3 *m_rand;
  //double m_b_85_weight = 0.11;
  //double m_b_77_weight = 0.64;
  //double m_b_70_weight = 0.83;
  //double m_b_60_weight = 0.94;
  bool m_isMC; 


};

#include "VBFHbbJetAssignment.icc"

#endif 

