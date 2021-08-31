#include "CxAODTools_VBFHbb/VBFHbbJetAssignment.h"

#include "EventLoop/Worker.h"
#include "TLorentzVector.h"
#include "cmath"
#include "TRandom.h"

jetAssignmentTool::jetAssignmentTool()
  : _debugMode(false)
{
  this->setAssignmentType("0for");
  this->setIsMC(false);
  this->setSelection("0forward");
  m_rand = new TRandom3();
}
jetAssignmentTool::jetAssignmentTool(jetAssignmentTool const& other)
  : _type(other._type),
    _debugMode(other._debugMode), 
    _signalJets(other._signalJets),
    _vbfJets(other._vbfJets) {} 
jetAssignmentTool::~jetAssignmentTool() {}

EL::StatusCode jetAssignmentTool::setAssignmentType(std::string type) 
{ 
  _type = type;
  if (_debugMode) Info("jetAssignmentTool()","Setting Jet Assignment type to %15s",type.c_str());
  return EL::StatusCode::SUCCESS; 
}

EL::StatusCode jetAssignmentTool::setSelection(std::string selection){
  _Selection = selection;
  return EL::StatusCode::SUCCESS; 
}

EL::StatusCode jetAssignmentTool::setIsMC(bool isMC){
  m_isMC = isMC;
  return EL::StatusCode::SUCCESS; 
}

void jetAssignmentTool::setDebugMode() { _debugMode = true; }

int jetAssignmentTool::assignJets(std::vector<xAOD::Jet const*> const& inputJets, const xAOD::EventInfo* info)
{
  // ** Common to all methods ** //

  std::vector<const xAOD::Jet*> shortListedInputJets;
  this->clear();

  /*if (_Selection.find("0forward") != std::string::npos){
    for (unsigned int iJet(0); iJet < inputJets.size(); iJet++) {
      if (inputJets.at(iJet)->pt() > 52.5e3 && fabs(inputJets.at(iJet)->eta())<2.5) shortListedInputJets.push_back( inputJets.at(iJet) );
    }
    if (shortListedInputJets.size() < 4) return 0;
  } else {
    for (unsigned int iJet(0); iJet < inputJets.size(); iJet++) {
      shortListedInputJets.push_back( inputJets.at(iJet) );
    }
  }*/

  for (unsigned int iJet(0); iJet < inputJets.size(); iJet++) {
    shortListedInputJets.push_back( inputJets.at(iJet) );
  }

  // ** End common part ** //

  if (_Selection.find("0forward_notrig") != std::string::npos){
    return this->assignBtag_0for(shortListedInputJets, _type, info,false);
  } else if (_Selection.find("0forward") != std::string::npos){
    return this->assignBtag_0for(shortListedInputJets, _type, info);
  } else if (_Selection.find("fake3") != std::string::npos){
    return this->assignBtag_fake3(shortListedInputJets, _type, info);
  } else if (_Selection.find("fake4") != std::string::npos){
    return this->assignBtag_fake4(shortListedInputJets, _type, info);
  } else if (_Selection.find("fake") != std::string::npos){
    return this->assignBtag_fake(shortListedInputJets, _type, info);
  } else if (_Selection.find("singleTagForward") != std::string::npos){
    return this->assignBtag_singleTagForward(shortListedInputJets, _type);
  } else if (_Selection.find("single") != std::string::npos){
    return this->assignBtag_single(shortListedInputJets, _type,info);
  } else if (_Selection.find("2forward") != std::string::npos){
    return this->assignBtag_2for(shortListedInputJets, _type,info);
  } else if (_Selection.find("topoCF") != std::string::npos){
    return this->assignBtag_topoCF(shortListedInputJets, _type,info);
  } else if (_Selection.find("inclusive") != std::string::npos){
    return this->assignBtag_inclusive(shortListedInputJets, _type,info);
  } else if (_Selection.find("topoInc") != std::string::npos){
    return this->assignBtag_topoInc(shortListedInputJets, _type,info);
  } else if (_Selection.find("1forward3central") != std::string::npos){
    return this->assignBtag_1for3cen(shortListedInputJets, _type, info);
  } else if (_Selection.find("1forwardAlt3central") != std::string::npos){
    return this->assignBtag_1forAlt3cen(shortListedInputJets, _type, info);
  } else if (_Selection.find("3central_maxptjj") != std::string::npos){
    return this->assignBtag_3cen_maxptjj(shortListedInputJets, _type,info);
  } else if (_Selection.find("3central_triggerJ1") != std::string::npos){
    return this->assignBtag_3cen_triggerJ1(shortListedInputJets, _type,info);
  } else if (_Selection.find("3central_triggerB2") != std::string::npos){
    return this->assignBtag_3cen_triggerB2(shortListedInputJets, _type,info);
  } else if (_Selection.find("3central") != std::string::npos){
    return this->assignBtag_3cen(shortListedInputJets, _type,info);
  } else if (_Selection.find("3jet") != std::string::npos){
    return this->assignBtag_3jet(shortListedInputJets, _type,info);
  } else if (_Selection.find("1forward2central_notrig") != std::string::npos){
    return this->assignBtag_1for2cen(shortListedInputJets, _type, info,true,false);
  } else if (_Selection.find("1forward2central_triggerJ1") != std::string::npos){
    return this->assignBtag_1for2cen_triggerJ1(shortListedInputJets, _type, info);
  } else if (_Selection.find("1forward2central_triggerB2") != std::string::npos){
    return this->assignBtag_1for2cen_triggerB2(shortListedInputJets, _type, info);
  } else if (_Selection.find("1forward2central") != std::string::npos){
    return this->assignBtag_1for2cen(shortListedInputJets, _type, info);
  } else if (_Selection.find("trigger") != std::string::npos){
    return this->assignBtag_trigger(shortListedInputJets, _type,info);
  }
  
  return 0;
}

void jetAssignmentTool::clear()
{
  _signalJets.clear();
  _vbfJets.clear();
}

void jetAssignmentTool::printResults()
{
  Info( "jetAssignmentTool()", "Results of assignment [%15s]", this->_type.c_str());
  Info( "jetAssignmentTool()", "signal Jet n. 1 [pt,eta,mv2c20] = [%.4f,%.4f,%.4f]", _signalJets.at(0)->pt(), _signalJets.at(0)->eta(), Props::MV2c10.get(_signalJets.at(0)) );
  Info( "jetAssignmentTool()", "signal Jet n. 2 [pt,eta,mv2c20] = [%.4f,%.4f,%.4f]", _signalJets.at(1)->pt(), _signalJets.at(1)->eta(), Props::MV2c10.get(_signalJets.at(1)) );
  Info( "jetAssignmentTool()", "   vbf Jet n. 1 [pt,eta,mv2c20] = [%.4f,%.4f,%.4f]", _vbfJets.at(0)->pt(), _vbfJets.at(0)->eta(), Props::MV2c10.get(_vbfJets.at(0)) );
  Info( "jetAssignmentTool()", "   vbf Jet n. 2 [pt,eta,mv2c20] = [%.4f,%.4f,%.4f]", _vbfJets.at(1)->pt(), _vbfJets.at(1)->eta(), Props::MV2c10.get(_vbfJets.at(1)) );
}

int jetAssignmentTool::assignCMS(std::vector<xAOD::Jet const*> &inputJets)
{
  // be sure jets are sorted by pt
  std::sort(inputJets.begin(),inputJets.end(),JetSort::sortJetsByPt);

  // take only the top 6 jets in pt
  std::vector<xAOD::Jet const*> candidates(inputJets.begin(),inputJets.begin()+4);
  if ( inputJets.size() > 4 ) candidates.push_back( inputJets.at(4) );
  if ( inputJets.size() > 5 ) candidates.push_back( inputJets.at(5) );

  // signal jets are the two most b-tagged jets 
  std::sort(candidates.begin(),candidates.end(),JetSort::sortJetsByBtag);
  _signalJets.push_back( candidates.at(0) );
  _signalJets.push_back( candidates.at(1) );

  candidates.erase( candidates.begin() );
  candidates.erase( candidates.begin() );

  // vbf jets as the top pt jets
  std::sort(candidates.begin(),candidates.end(),JetSort::sortJetsByPt);
  _vbfJets.push_back( candidates.at(0) );
  _vbfJets.push_back( candidates.at(1) );

  // sort results in pt
  std::sort(_signalJets.begin(),_signalJets.end(),JetSort::sortJetsByPt);
  std::sort(_vbfJets.begin(),_vbfJets.end(),JetSort::sortJetsByPt);

  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_singleTagForward(std::vector<xAOD::Jet const*> &inputJets, std::string WP)
{

  double btagweight =0;
  if (WP == "85"){
    btagweight = m_b_77_weight;
  }
  if (WP == "77"){
    btagweight = m_b_77_weight;
  }
  if (WP == "70" || WP=="DEFAULT"){
    btagweight = m_b_70_weight;
  }
  if (WP == "truth"){
    btagweight = -1;
  }

  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;

  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);

  for (unsigned int i(0); i < inputJets.size(); i++) {
    const xAOD::Jet *jet = inputJets.at(i);
    if (fabs(jet->eta())>2.5) continue;
    //if (fabs(jet->pt())<225e3) continue;
    //if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet)!=5 : Props::MV2c10.get( jet ) < btagweight) continue;
    TLorentzVector HiggsVect = inputJets.at(i)->p4();
    if ( HiggsVect.Pt() > MaxPt ) {
      MaxPt = HiggsVect.Pt();
      _signalJets.at(0) = inputJets.at(i);
      _signalJets.at(1) = inputJets.at(i);
    }
  }

  if ( _signalJets.at(0) == nullptr or _signalJets.at(1) == nullptr ) {
    return 0;
  }

  // find the pair of jets that yields the highest mjj                                                                                                                     
  std::vector<xAOD::Jet const*> candidates;
  std::vector<xAOD::Jet const*> candidatesLoose;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (fabs(inputJets.at(i)->eta())>3.0 && inputJets.at(i)->pt()>45e3) candidates.push_back( inputJets.at(i) );
    if (fabs(inputJets.at(i)->eta())>2.5 && inputJets.at(i)->pt()>30e3) candidatesLoose.push_back( inputJets.at(i) );
  }


  if (candidates.size() != 1) return 0;
  if (candidatesLoose.size() != 1) return 0;

  double maxPt(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  for (unsigned int i(0); i < candidates.size(); i++) {
    TLorentzVector jet1(candidates.at(i)->p4());
    if(jet1.Pt()<maxPt) continue;
    maxPt = jet1.Pt();
    vbfjet1 = candidates.at(i);
    vbfjet2 = candidates.at(i);
  }

  if (vbfjet1 == nullptr or vbfjet2 == nullptr) {
    return 0;
  }

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );

  // sort results in pt
  std::sort(_signalJets.begin(),_signalJets.end(),JetSort::sortJetsByPt);
  std::sort(_vbfJets.begin(),_vbfJets.end(),JetSort::sortJetsByPt);

  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_0for(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info, bool dotrig)
{

  double btagweight =0;
  if (WP == "85"){
    btagweight = m_b_77_weight;
  }
  if (WP == "77" || WP == "DEFAULT"){
    btagweight = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight = m_b_70_weight;
  }
  if (WP == "truth"){
    btagweight = -1;
  }

  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber>340453 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.get(info);
  else if (runNumber>=338675 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  else if (!Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  //if (runNumber<=303943 && !m_isMC) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  if (runNumber<=303943 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  else if (runNumber<=311481 && !Props::embZpt.exists(info)) passesHLT = ((Props::passHLT_2j35_bmv2c2060_split_2j35_L14J15.get(info)));

  if (dotrig && btagweight>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0;

  if (Props::embZpt.exists(info)) {
    //if (Props::embZm.get(info)<66e3 || Props::embZm.get(info)>116e3) return 0;
    //if (Props::embZm.get(info)<76e3 || Props::embZm.get(info)>106e3) return 0;
    if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;
  }

  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;

  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);

  for (unsigned int i(0); i < inputJets.size(); i++)
    {
      const xAOD::Jet *jet = inputJets.at(i);
      if (fabs(jet->eta())>2.5) continue;
      if (jet->pt()<50e3) continue;
      if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet)!=5 : Props::MV2c10.get( jet ) < btagweight) continue;
      for (unsigned int j(i+1); j < inputJets.size(); j++) {
        const xAOD::Jet *jet2 = inputJets.at(j);
        if (fabs(jet2->eta())>2.5) continue;
        if (jet2->pt()<50e3) continue;
        if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet2)!=5 : Props::MV2c10.get( jet2 ) < btagweight) continue;
	  
	  //if (m_isMC and m_runNumber<=303943 and Props::HLTBJetMatched_2J45_MV2c20_70.get(inputJets.at(i)) == Props::HLTBJetMatched_2J45_MV2c20_70.get(inputJets.at(j)))
	  //  continue;

	  //if (m_isMC and m_runNumber>303943 and m_runNumber<=311481 and Props::HLTBJetMatched_2J35_MV2c20_60.get(inputJets.at(i)) == Props::HLTBJetMatched_2J35_MV2c20_60.get(inputJets.at(j)))
	  //  continue;

	  //if (m_isMC and m_runNumber>311481 and Props::HLTBJetMatched_2J45_MV2c10_60.get(inputJets.at(i)) == Props::HLTBJetMatched_2J45_MV2c10_60.get(inputJets.at(j)))
	  //  continue;
	      
	  TLorentzVector HiggsVect = inputJets.at(i)->p4() + inputJets.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = inputJets.at(i);
	    _signalJets.at(1) = inputJets.at(j);
	  }
	}
    }

  if ( _signalJets.at(0) == nullptr or _signalJets.at(1) == nullptr ) {
    return 0;
  }

  // find the pair of jets that yields the highest mjj                                                                                                                     
  std::vector<xAOD::Jet const*> candidates;

  int nforward = 0;
  for (unsigned int i(0); i < inputJets.size(); i++) {
    //if (fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if (fabs(inputJets.at(i)->eta())>3.2 && inputJets.at(i)->pt()>60e3) nforward++;
    if (fabs(inputJets.at(i)->eta())>2.5 && inputJets.at(i)->pt()>160e3) nforward++;
    if (fabs(inputJets.at(i)->eta())<2.5 && (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))!=5 : Props::MV2c10.get( inputJets.at(i) ) < btagweight) && inputJets.at(i)->pt()>160e3) nforward++;
    //if (fabs(inputJets.at(i)->eta())>3.2 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>60e3) nforward++;
    if (fabs(inputJets.at(i)->eta())>2.5) continue;
    if (inputJets.at(i)->pt()<50e3) continue;
    if (fabs(inputJets.at(i)->eta())>2.5 || (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))!=5 : Props::MV2c10.get( inputJets.at(i) ) < btagweight)) {
      candidates.push_back( inputJets.at(i) );
    }
  }
  if (nforward) return 0;


  if (candidates.size() < 2) return 0;

  double maxMjj(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  for (unsigned int i(0); i < candidates.size(); i++)
    {
      for (unsigned int j(i+1); j < candidates.size(); j++)
	{
	  TLorentzVector jet1(candidates.at(i)->p4());
	  TLorentzVector jet2(candidates.at(j)->p4());
	  if((jet1+jet2).M() > maxMjj){
	    maxMjj = (jet1+jet2).M();
	    vbfjet1 = candidates.at(i);
	    vbfjet2 = candidates.at(j);
	  }
	}
    }

  if (vbfjet1 == nullptr or vbfjet2 == nullptr) {
    return 0;
  }
  if (maxMjj<800e3) return 0;

  vbfjet1->auxdecor<float>("randfactor") = 1;
  vbfjet2->auxdecor<float>("randfactor") = 1;
  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );

  // sort results in pt
  std::sort(_signalJets.begin(),_signalJets.end(),JetSort::sortJetsByPt);
  std::sort(_vbfJets.begin(),_vbfJets.end(),JetSort::sortJetsByPt);

  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_2for(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info)
{

  double btagweight_lead =0;
  double btagweight_sub =0;

  if (WP == "85"){
    btagweight_lead = m_b_85_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "77"){
    btagweight_lead = m_b_77_weight;
    btagweight_sub = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_70_weight;
  }
  if (WP == "asym" || WP == "DEFAULT"){
    btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "truth"){
    btagweight_lead = -1;
    btagweight_sub = -1;
  }
  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber<=328393) passesHLT = Props::passHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.get(info);
  else if (runNumber<=340453) passesHLT = Props::passHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.get(info);
  else passesHLT = Props::passHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49.get(info);
  if (runNumber<=311481) passesHLT = true;
  if (btagweight_lead>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0; 

  if (Props::embZpt.exists(info)) {
    //if (Props::embZm.get(info)<81e3 || Props::embZm.get(info)>101e3) return 0;
    //if (Props::embZm.get(info)<66e3 || Props::embZm.get(info)>116e3) return 0;
    if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;
  }

  std::vector<xAOD::Jet const*> candidates_leading_b;
  std::vector<xAOD::Jet const*> candidates_sub_b;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.4 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
        candidates_leading_b.push_back( inputJets.at(i) );
    }
    if (inputJets.at(i)->pt()>20e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
        candidates_sub_b.push_back( inputJets.at(i) );
    }
  }

  
  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;

  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);

  for (unsigned int i(0); i < candidates_leading_b.size(); i++) {
    for (unsigned int j(0); j < candidates_sub_b.size(); j++) {
      if (candidates_leading_b.at(i) == candidates_sub_b.at(j)) continue;
      TLorentzVector HiggsVect = candidates_leading_b.at(i)->p4() + candidates_sub_b.at(j)->p4();
      if ( HiggsVect.Pt() > MaxPt ) {
        MaxPt = HiggsVect.Pt();
        _signalJets.at(0) = candidates_leading_b.at(i);
        _signalJets.at(1) = candidates_sub_b.at(j);
      }
    }
  }

  if ( _signalJets.at(0) == nullptr ) return 0;
  if ( _signalJets.at(1) == nullptr ) return 0;

  std::vector<xAOD::Jet const*> candidates_forward;

  double maxMjj(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  // forward jets
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if( fabs(inputJets.at(i)->eta())>3.2 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>60e3){
      candidates_forward.push_back(inputJets.at(i));
    }
  }

  if (candidates_forward.size()<2) return 0;

  for (unsigned int i(0); i < candidates_forward.size(); i++) {
    for (unsigned int j(0); j < candidates_forward.size(); j++) {
      if (candidates_forward[i] == candidates_forward[j]) continue;
      if (candidates_forward[i]->eta()*candidates_forward[j]->eta()>0) continue;
      if (candidates_forward[i]->pt()<candidates_forward[j]->pt()) continue;
      TLorentzVector jet1(candidates_forward.at(i)->p4());
      TLorentzVector jet2(candidates_forward.at(j)->p4());
      if((jet1+jet2).M() > maxMjj){
        maxMjj = (jet1+jet2).M();
        vbfjet1 = candidates_forward[i];
        vbfjet2 = candidates_forward[j];
      }
    }
  }

  if (vbfjet1 == nullptr) return 0;
  if (vbfjet2 == nullptr) return 0;

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );
  
  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_3jet(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info)
{

  double btagweight =0;

  if (WP == "85"){
    btagweight = m_b_85_weight;
  }
  if (WP == "77" || WP == "DEFAULT"){
    btagweight = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight = m_b_70_weight;
  }
  if (WP == "truth"){
    btagweight = -1;
  }
  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber<=340453) passesHLT = Props::passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30.get(info);
  else passesHLT = Props::passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30.get(info);
  if (!Props::embZpt.exists(info) && runNumber>=296939 && runNumber<=311481) passesHLT = Props::passHLT_j100_2j55_bmv2c2060_split.get(info);
  if (btagweight>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0; 

  if (Props::embZpt.exists(info)) {
    //if (Props::embZm.get(info)<66e3 || Props::embZm.get(info)>116e3) return 0;
    //if (Props::embZm.get(info)<76e3 || Props::embZm.get(info)>106e3) return 0;
    if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;
  }

  double MaxPt = -1;

  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);

  for (unsigned int i(0); i < inputJets.size(); i++)
    {
      const xAOD::Jet *jet = inputJets.at(i);
      if (fabs(jet->eta())>2.5 || jet->pt()<65e3) continue;
      if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet)!=5 : Props::MV2c10.get( jet ) < btagweight) continue;
      for (unsigned int j(i+1); j < inputJets.size(); j++) {
        const xAOD::Jet *jet2 = inputJets.at(j);
        if (fabs(jet2->eta())>2.5 || jet2->pt()<65e3) continue;
        if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet2)!=5 : Props::MV2c10.get( jet2 ) < btagweight) continue;
	      
	  TLorentzVector HiggsVect = inputJets.at(i)->p4() + inputJets.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = inputJets.at(i);
	    _signalJets.at(1) = inputJets.at(j);
	  }
	}
    }

  if ( _signalJets.at(0) == nullptr or _signalJets.at(1) == nullptr ) {
    return 0;
  }

  // find the pair of jets that yields the highest mjj                                                                                                                     
  std::vector<xAOD::Jet const*> candidates_central;

  int nforward = 0;
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)==_signalJets.at(0)) continue;
    if (inputJets.at(i)==_signalJets.at(1)) continue;
    if (fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if (fabs(inputJets.at(i)->eta())>2.8 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>30e3) nforward++;
    if (fabs(inputJets.at(i)->eta())>2.5 && fabs(inputJets.at(i)->eta())<2.8 && inputJets.at(i)->pt()>30e3 && inputJets.at(i)->pt()<160e3) nforward++;
    if (fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>50e3 && inputJets.at(i)->pt()<160e3) nforward++;
    //if ((btagweight<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))!=5 : Props::MV2c10.get( inputJets.at(i) ) < btagweight) && fabs(inputJets.at(i)->eta())<2.8 && inputJets.at(i)->pt()>160e3) {
    if (fabs(inputJets.at(i)->eta())<2.8 && inputJets.at(i)->pt()>160e3) {
      candidates_central.push_back( inputJets.at(i) );
    }
  }
  if (nforward) return 0;
  if (candidates_central.size()!=1) return 0;

  double maxMjj(-1);//min mJJ from trigger
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  for (unsigned int j(0); j < candidates_central.size(); j++) {
    TLorentzVector jet2(candidates_central.at(j)->p4());
    if((jet2).Pt() > maxMjj){
      vbfjet1 = candidates_central.at(j);
      vbfjet2 = candidates_central.at(j);
      maxMjj = jet2.Pt();
    }
  }

  if (vbfjet1 == nullptr or vbfjet2 == nullptr) {
    return 0;
  }

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );

  // sort results in pt
  std::sort(_signalJets.begin(),_signalJets.end(),JetSort::sortJetsByPt);

  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_3cen_maxptjj(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info,bool dotrig)
{

  double btagweight =0;

  if (WP == "85"){
    btagweight = m_b_85_weight;
  }
  if (WP == "77" || WP == "DEFAULT"){
    btagweight = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight = m_b_70_weight;
  }
  if (WP == "truth"){
    btagweight = -1;
  }
  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber<=340453 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30.get(info);
  else if (!Props::embZpt.exists(info)) passesHLT = Props::passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30.get(info);
  if (!Props::embZpt.exists(info) && runNumber>=296939 && runNumber<=311481) passesHLT = Props::passHLT_j100_2j55_bmv2c2060_split.get(info);
  if (dotrig && btagweight>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0; 

  if (Props::embZpt.exists(info)) {
    //if (Props::embZm.get(info)<66e3 || Props::embZm.get(info)>116e3) return 0;
    //if (Props::embZm.get(info)<76e3 || Props::embZm.get(info)>106e3) return 0;
    if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;
  }

  double MaxPt = -1;

  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);

  for (unsigned int i(0); i < inputJets.size(); i++) {
      const xAOD::Jet *jet = inputJets.at(i);
      if (fabs(jet->eta())>2.5 || jet->pt()<65e3) continue;
      if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet)!=5 : Props::MV2c10.get( jet ) < btagweight) continue;
      for (unsigned int j(i+1); j < inputJets.size(); j++) {
        const xAOD::Jet *jet2 = inputJets.at(j);
        if (fabs(jet2->eta())>2.5 || jet2->pt()<65e3) continue;
        if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet2)!=5 : Props::MV2c10.get( jet2 ) < btagweight) continue;
	      
	  TLorentzVector HiggsVect = inputJets.at(i)->p4() + inputJets.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = inputJets.at(i);
	    _signalJets.at(1) = inputJets.at(j);
	  }
	}
  }

  if ( _signalJets.at(0) == nullptr or _signalJets.at(1) == nullptr ) {
    return 0;
  }

  // find the pair of jets that yields the highest mjj                                                                                                                     
  std::vector<xAOD::Jet const*> candidates_all;
  std::vector<xAOD::Jet const*> candidates_central;

  int nforward = 0;
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets[i]==_signalJets.at(0) || inputJets[i]==_signalJets.at(1)) continue;
    if (fabs(inputJets.at(i)->eta())>3.2 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>60e3) nforward++;
    if (fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if (fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>30e3) {
      candidates_all.push_back( inputJets.at(i) );
    }
    if (fabs(inputJets.at(i)->eta())<2.8 && inputJets.at(i)->pt()>160e3) {
      candidates_central.push_back( inputJets.at(i) );
    }
  }
  if (nforward) return 0;


  if (candidates_all.size() < 1 || candidates_central.size()<1) return 0;

  double maxMjj(-1);//min mJJ from trigger
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  for (unsigned int i(0); i < candidates_all.size(); i++) {
    for (unsigned int j(0); j < candidates_central.size(); j++) {
      if (candidates_all.at(i)==candidates_central.at(j)) continue;
      TLorentzVector jet1(candidates_all.at(i)->p4());
      TLorentzVector jet2(candidates_central.at(j)->p4());
      if (jet1.Pt()>jet2.Pt() && fabs(jet1.Eta())<2.8) continue;
      if((jet1+jet2).Pt() > maxMjj){
        maxMjj = (jet1+jet2).Pt();
        vbfjet1 = candidates_central.at(j);
        vbfjet2 = candidates_all.at(i);
      }
    }
  }

  if (vbfjet1 == nullptr or vbfjet2 == nullptr) {
    return 0;
  }

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );

  // sort results in pt
  std::sort(_signalJets.begin(),_signalJets.end(),JetSort::sortJetsByPt);

  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_3cen(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info,bool dotrig)
{

  double btagweight =0;

  if (WP == "85"){
    btagweight = m_b_85_weight;
  }
  if (WP == "77" || WP == "DEFAULT"){
    btagweight = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight = m_b_70_weight;
  }
  if (WP == "truth"){
    btagweight = -1;
  }
  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber<=340453 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30.get(info);
  else if (!Props::embZpt.exists(info)) passesHLT = Props::passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30.get(info);
  if (!Props::embZpt.exists(info) && runNumber<=311481) passesHLT = Props::passHLT_j100_2j55_bmv2c2060_split.get(info);
  //if (!Props::embZpt.exists(info) && runNumber>=296939 && runNumber<=311481) passesHLT = Props::passHLT_j100_2j55_bmv2c2060_split.get(info);
  if (dotrig && btagweight>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0; 

  if (Props::embZpt.exists(info)) {
    if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;
  }

  double MaxPt = -1;

  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);

  for (unsigned int i(0); i < inputJets.size(); i++) {
      const xAOD::Jet *jet = inputJets.at(i);
      if (fabs(jet->eta())>2.5 || jet->pt()<65e3) continue;
      if (!Props::embZpt.exists(info) && (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : (!Props::HLTGSCJetMatchedPt.exists(inputJets.at(i)) ? 61e3 : Props::HLTGSCJetMatchedPt.get(inputJets.at(i))))<60e3) continue;
      //if (!Props::embZpt.exists(info) && (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))<60e3) continue;
      if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet)!=5 : Props::MV2c10.get( jet ) < btagweight) continue;
      for (unsigned int j(i+1); j < inputJets.size(); j++) {
        const xAOD::Jet *jet2 = inputJets.at(j);
        if (fabs(jet2->eta())>2.5 || jet2->pt()<65e3) continue;
        if (!Props::embZpt.exists(info) && (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(j)) : (!Props::HLTGSCJetMatchedPt.exists(inputJets.at(j)) ? 61e3 : Props::HLTGSCJetMatchedPt.get(inputJets.at(j))))<60e3) continue;
        //if (!Props::embZpt.exists(info) && (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(j)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(j)))<60e3) continue;
        if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet2)!=5 : Props::MV2c10.get( jet2 ) < btagweight) continue;
	  TLorentzVector HiggsVect = inputJets.at(i)->p4() + inputJets.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = inputJets.at(i);
	    _signalJets.at(1) = inputJets.at(j);
	  }
	}
  }

  if ( _signalJets.at(0) == nullptr or _signalJets.at(1) == nullptr ) {
    return 1;
  }

  // find the pair of jets that yields the highest mjj                                                                                                                     
  std::vector<xAOD::Jet const*> candidates_all;
  std::vector<xAOD::Jet const*> candidates_central;

  int nforward = 0;
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets[i]==_signalJets.at(0) || inputJets[i]==_signalJets.at(1)) continue;
    if (fabs(inputJets.at(i)->eta())>3.2 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>60e3) nforward++;
    if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if (fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>30e3) {
      candidates_all.push_back( inputJets.at(i) );
    }
    if (fabs(inputJets.at(i)->eta())<3.1 && inputJets.at(i)->pt()>160e3 && (Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : (!Props::HLTGSCJetMatchedPt.exists(inputJets.at(i)) ? 151e3 : Props::HLTGSCJetMatchedPt.get(inputJets.at(i))) )>150e3)) {
    //if (fabs(inputJets.at(i)->eta())<3.1 && inputJets.at(i)->pt()>160e3) {
      //if (!Props::embZpt.exists(info) && (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))<60e3) continue;
      candidates_central.push_back( inputJets.at(i) );
    }
  }
  if (nforward) return 2;


  if (candidates_all.size() < 1 || candidates_central.size()<1) return 3;

  double maxMjj(-1);//min mJJ from trigger
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  for (unsigned int i(0); i < candidates_all.size(); i++) {
    for (unsigned int j(0); j < candidates_central.size(); j++) {
      if (candidates_all.at(i)==candidates_central.at(j)) continue;
      TLorentzVector jet1(candidates_all.at(i)->p4());
      TLorentzVector jet2(candidates_central.at(j)->p4());
      if (jet1.Pt()>jet2.Pt() && fabs(jet1.Eta())<3.1) continue;
      if((jet1+jet2).M() > maxMjj){
        maxMjj = (jet1+jet2).M();
        vbfjet1 = candidates_central.at(j);
        vbfjet2 = candidates_all.at(i);
      }
    }
  }

  if (vbfjet1 == nullptr or vbfjet2 == nullptr) {
    return 4;
  }

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );

  // sort results in pt
  std::sort(_signalJets.begin(),_signalJets.end(),JetSort::sortJetsByPt);
  vbfjet1->auxdecor<float>("randfactor") = 1;
  vbfjet2->auxdecor<float>("randfactor") = 1;

  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_3cen_triggerJ1(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info,bool dotrig)
{

  double btagweight =0;

  if (WP == "85"){
    btagweight = m_b_85_weight;
  }
  if (WP == "77" || WP == "DEFAULT"){
    btagweight = m_b_60_weight;
  }
  if (WP == "70"){
    btagweight = m_b_70_weight;
  }
  if (WP == "truth"){
    btagweight = -1;
  }

  //bool passesHLT = false;
  //UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  //passesHLT = Props::passHLT_j175_gsc225_bmv2c1040_split.get(info);
  //if (runNumber>=296939 && runNumber<=311481) passesHLT = Props::passHLT_j175_bmv2c2040_split.get(info);
  //if (!passesHLT && !Props::embZpt.exists(info)) return 0;
  //if (!passesHLT && !Props::embZpt.exists(info) && !m_isMC) return 0;

  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber>340453 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.get(info);
  else if (runNumber>=338675 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  else if (!Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  //if (runNumber<=303943 && !m_isMC) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  if (runNumber<=303943 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  else if (runNumber<=311481 && !Props::embZpt.exists(info)) passesHLT = ((Props::passHLT_2j35_bmv2c2060_split_2j35_L14J15.get(info)));
  if (btagweight>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0;
  if (Props::embZpt.exists(info)) if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;

  double MaxPt = -1;

  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);

  for (unsigned int i(0); i < inputJets.size(); i++) {
    const xAOD::Jet *jet = inputJets.at(i);
    if (fabs(jet->eta())>2.5 || jet->pt()<80e3) continue;
    if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet)!=5 : Props::MV2c10.get( jet ) < btagweight) continue;
    for (unsigned int j(i+1); j < inputJets.size(); j++) {
      const xAOD::Jet *jet2 = inputJets.at(j);
      if (fabs(jet2->eta())>2.5 || jet2->pt()<80e3) continue;
      if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet2)!=5 : Props::MV2c10.get( jet2 ) < btagweight) continue;
      TLorentzVector HiggsVect = inputJets.at(i)->p4() + inputJets.at(j)->p4();
      if ( HiggsVect.Pt() > MaxPt ) {
        MaxPt = HiggsVect.Pt();
        _signalJets.at(0) = inputJets.at(i);
        _signalJets.at(1) = inputJets.at(j);
      }
    }
  }

  if ( _signalJets.at(0) == nullptr or _signalJets.at(1) == nullptr ) {
    return 0;
  }

  // find the pair of jets that yields the highest mjj                                                                                                                     
  std::vector<xAOD::Jet const*> candidates_all;
  std::vector<xAOD::Jet const*> candidates_central;

  int nforward = 0;
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets[i]==_signalJets.at(0) || inputJets[i]==_signalJets.at(1)) continue;
    if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if (fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>30e3) {
      candidates_all.push_back( inputJets.at(i) );
    }
    if (fabs(inputJets.at(i)->eta())<3.5 && inputJets.at(i)->pt()>120e3) {
      candidates_central.push_back( inputJets.at(i) );
    }
  }

  if (candidates_all.size() < 1 || candidates_central.size()!=1) return 0;

  double maxMjj(-1);//min mJJ from trigger
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  for (unsigned int i(0); i < candidates_all.size(); i++) {
    for (unsigned int j(0); j < candidates_central.size(); j++) {
      if (candidates_all.at(i)==candidates_central.at(j)) continue;
      TLorentzVector jet1(candidates_all.at(i)->p4());
      TLorentzVector jet2(candidates_central.at(j)->p4());
      if (jet1.Pt()>jet2.Pt() && fabs(jet1.Eta())<3.5) continue;
      //if(jet1.Pt()+jet2.Pt() > maxMjj){
      //  maxMjj = jet1.Pt()+jet2.Pt();
      if((jet1+jet2).M() > maxMjj){
        maxMjj = (jet1+jet2).M();
        vbfjet1 = candidates_central.at(j);
        vbfjet2 = candidates_all.at(i);
      }
    }
  }

  if (vbfjet1 == nullptr or vbfjet2 == nullptr) {
    return 0;
  }

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );

  // sort results in pt
  std::sort(_signalJets.begin(),_signalJets.end(),JetSort::sortJetsByPt);
  vbfjet1->auxdecor<float>("randfactor") = 1;
  vbfjet2->auxdecor<float>("randfactor") = 1;

  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_3cen_triggerB2(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info,bool dotrig)
{

  double btagweight =0;

  if (WP == "85"){
    btagweight = m_b_85_weight;
  }
  if (WP == "77" || WP == "DEFAULT"){
    btagweight = m_b_60_weight;
  }
  if (WP == "70"){
    btagweight = m_b_70_weight;
  }
  if (WP == "truth"){
    btagweight = -1;
  }

  //bool passesHLT = false;
  //UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  //passesHLT = Props::passHLT_j175_gsc225_bmv2c1040_split.get(info);
  //if (runNumber>=296939 && runNumber<=311481) passesHLT = Props::passHLT_j175_bmv2c2040_split.get(info);
  //if (!passesHLT && !Props::embZpt.exists(info) && !m_isMC) return 0;
  //if (Props::embZpt.exists(info)) if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;

  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber>340453 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.get(info);
  else if (runNumber>=338675 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  else if (!Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  //if (runNumber<=303943 && !m_isMC) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  if (runNumber<=303943 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  else if (runNumber<=311481 && !Props::embZpt.exists(info)) passesHLT = ((Props::passHLT_2j35_bmv2c2060_split_2j35_L14J15.get(info)));
  if (btagweight>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0;
  if (Props::embZpt.exists(info)) if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;

  double MaxPt = -1;

  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);
  int nbjets = 0;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    const xAOD::Jet *jet = inputJets.at(i);

    if (fabs(jet->eta())<2.5 && jet->pt()>40e3 && Props::MV2c10.get( jet ) > m_b_85_weight) nbjets++;

    if (fabs(jet->eta())>2.5 || jet->pt()<100e3) continue;
    if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet)!=5 : Props::MV2c10.get( jet ) < btagweight) continue;
    for (unsigned int j(i+1); j < inputJets.size(); j++) {
      const xAOD::Jet *jet2 = inputJets.at(j);
      if (fabs(jet2->eta())>2.5 || jet2->pt()<40e3) continue;
      if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet2)!=5 : Props::MV2c10.get( jet2 ) < btagweight) continue;
      float HiggsVect = inputJets.at(i)->pt() + inputJets.at(j)->pt();
      if ( HiggsVect > MaxPt ) {
        MaxPt = inputJets.at(i)->pt() + inputJets.at(j)->pt();
        _signalJets.at(0) = inputJets.at(jet->pt()>jet2->pt()?i:j);
        _signalJets.at(1) = inputJets.at(jet->pt()>jet2->pt()?j:i);
      }
    }
  }

  if (nbjets!=2 || _signalJets.at(0) == nullptr || _signalJets.at(1) == nullptr ) {
    return 0;
  }

  // find the pair of jets that yields the highest mjj                                                                                                                     
  std::vector<xAOD::Jet const*> candidates_all;
  std::vector<xAOD::Jet const*> candidates_central;

  int nforward = 0;
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets[i]==_signalJets.at(0) || inputJets[i]==_signalJets.at(1)) continue;
    if (fabs(inputJets.at(i)->eta())>3.2 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>60e3) nforward++;
    if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if (fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>30e3) {
      candidates_all.push_back( inputJets.at(i) );
    }
    if (fabs(inputJets.at(i)->eta())<3.1 && inputJets.at(i)->pt()>180e3) {
      candidates_central.push_back( inputJets.at(i) );
    }
  }
  if (nforward) return 0;


  if (candidates_all.size() < 1 || candidates_central.size()<1) return 0;

  double maxMjj(-1);//min mJJ from trigger
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  for (unsigned int i(0); i < candidates_all.size(); i++) {
    for (unsigned int j(0); j < candidates_central.size(); j++) {
      if (candidates_all.at(i)==candidates_central.at(j)) continue;
      TLorentzVector jet1(candidates_all.at(i)->p4());
      TLorentzVector jet2(candidates_central.at(j)->p4());
      if (jet1.Pt()>jet2.Pt() && fabs(jet1.Eta())<3.1) continue;
      //if(jet1.Pt()+jet2.Pt() > maxMjj){
      //  maxMjj = jet1.Pt()+jet2.Pt();
      if(jet1.Pt()+jet2.Pt() > maxMjj){
        maxMjj = jet1.Pt()+jet2.Pt();
        vbfjet1 = candidates_central.at(j);
        vbfjet2 = candidates_all.at(i);
      }
    }
  }

  if (vbfjet1 == nullptr or vbfjet2 == nullptr) {
    return 0;
  }

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );

  // sort results in pt
  std::sort(_signalJets.begin(),_signalJets.end(),JetSort::sortJetsByPt);
  vbfjet1->auxdecor<float>("randfactor") = 1;
  vbfjet2->auxdecor<float>("randfactor") = 1;

  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_topoInc(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info)
{

  double btagweight =0;

  if (WP == "85"){
    btagweight = m_b_85_weight;
  }
  if (WP == "77" || WP == "DEFAULT"){
    btagweight = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight = m_b_70_weight;
  }
  if (WP == "truth"){
    btagweight = -1;
  }
  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  passesHLT = Props::passHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF.get(info);
  if (runNumber<355529) passesHLT = false;
  if (btagweight>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0; 

  double MaxPt = -1;

  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);

  for (unsigned int i(0); i < inputJets.size(); i++)
    {
      const xAOD::Jet *jet = inputJets.at(i);
      if (fabs(jet->eta())>2.5 || jet->pt()<20e3) continue;
      if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet)!=5 : Props::MV2c10.get( jet ) < btagweight) continue;
      for (unsigned int j(i+1); j < inputJets.size(); j++) {
        const xAOD::Jet *jet2 = inputJets.at(j);
        if (fabs(jet2->eta())>2.5 || jet2->pt()<20e3) continue;
        if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet2)!=5 : Props::MV2c10.get( jet2 ) < btagweight) continue;
	      
	  TLorentzVector HiggsVect = inputJets.at(i)->p4() + inputJets.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = inputJets.at(i);
	    _signalJets.at(1) = inputJets.at(j);
	  }
	}
    }

  if ( _signalJets.at(0) == nullptr or _signalJets.at(1) == nullptr ) {
    return 0;
  }

  // find the pair of jets that yields the highest mjj                                                                                                                     
  std::vector<xAOD::Jet const*> candidates_all;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()<85e3) continue;
    if (fabs(inputJets.at(i)->eta())>4.5) continue;
    if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if ( inputJets.at(i) != _signalJets.at(0) and inputJets.at(i) != _signalJets.at(1)){
      candidates_all.push_back( inputJets.at(i) );
    }
  }


  if (candidates_all.size()<1) return 0;

  double maxMjj(1250e3);//min mJJ from trigger
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  for (unsigned int i(0); i < candidates_all.size(); i++) {
      for (unsigned int j(0); j < candidates_all.size(); j++) {
          if (i==j) continue;
	  TLorentzVector jet1(candidates_all.at(i)->p4());
	  TLorentzVector jet2(candidates_all.at(j)->p4());
          if (fabs(jet1.Eta())>3.2 && fabs(jet2.Eta())>3.2) continue;
          if (fabs(jet1.Eta()-jet2.Eta())<4.) continue;
          if (fabs(jet1.DeltaPhi(jet2))>2.) continue;
	  if((jet1+jet2).M() > maxMjj){
	    maxMjj = (jet1+jet2).M();
	    vbfjet1 = candidates_all.at(i);
	    vbfjet2 = candidates_all.at(j);
	  }
	}
    }

  if (vbfjet1 == nullptr or vbfjet2 == nullptr) {
    return 0;
  }

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );

  // sort results in pt
  std::sort(_signalJets.begin(),_signalJets.end(),JetSort::sortJetsByPt);

  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_inclusive(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info)
{

  double btagweight =0;

  if (WP == "85"){
    btagweight = m_b_85_weight;
  }
  if (WP == "77"){
    btagweight = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight = m_b_70_weight;
  }
  if (WP == "truth" || WP == "DEFAULT"){
    btagweight = -1;
  }

  double MaxPt = -1;

  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);

  for (unsigned int i(0); i < inputJets.size(); i++)
    {
      const xAOD::Jet *jet = inputJets.at(i);
      if (fabs(jet->eta())>4.5 || jet->pt()<20e3) continue;
      if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet)!=5 : Props::MV2c10.get( jet ) < btagweight) continue;
      for (unsigned int j(i+1); j < inputJets.size(); j++) {
        const xAOD::Jet *jet2 = inputJets.at(j);
        if (fabs(jet2->eta())>4.5 || jet2->pt()<20e3) continue;
        if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet2)!=5 : Props::MV2c10.get( jet2 ) < btagweight) continue;
	      
	  TLorentzVector HiggsVect = inputJets.at(i)->p4() + inputJets.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = inputJets.at(i);
	    _signalJets.at(1) = inputJets.at(j);
	  }
	}
    }

  if ( _signalJets.at(0) == nullptr or _signalJets.at(1) == nullptr ) {
    return 0;
  }

  // find the pair of jets that yields the highest mjj                                                                                                                     
  std::vector<xAOD::Jet const*> candidates_all;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()<20e3) continue;
    if (fabs(inputJets.at(i)->eta())>4.5) continue;
    if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if ( inputJets.at(i) != _signalJets.at(0) and inputJets.at(i) != _signalJets.at(1)){
      candidates_all.push_back( inputJets.at(i) );
    }
  }


  if (candidates_all.size()<1) return 0;

  double maxMjj(0);//min mJJ from trigger
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  for (unsigned int i(0); i < candidates_all.size(); i++) {
      for (unsigned int j(0); j < candidates_all.size(); j++) {
          if (i==j) continue;
	  TLorentzVector jet1(candidates_all.at(i)->p4());
	  TLorentzVector jet2(candidates_all.at(j)->p4());
	  if((jet1+jet2).M() > maxMjj){
	    maxMjj = (jet1+jet2).M();
	    vbfjet1 = candidates_all.at(i);
	    vbfjet2 = candidates_all.at(j);
	  }
	}
    }

  if (vbfjet1 == nullptr or vbfjet2 == nullptr) {
    return 0;
  }

  vbfjet1->auxdecor<float>("randfactor") = 1;
  vbfjet2->auxdecor<float>("randfactor") = 1;
  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );


  // sort results in pt
  std::sort(_signalJets.begin(),_signalJets.end(),JetSort::sortJetsByPt);

  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_topoCF(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info)
{

  double btagweight =0;

  if (WP == "85"){
    btagweight = m_b_85_weight;
  }
  if (WP == "77" || WP == "DEFAULT"){
    btagweight = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight = m_b_70_weight;
  }
  if (WP == "truth"){
    btagweight = -1;
  }
  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber<=340453) passesHLT = Props::passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split.get(info);
  else passesHLT = Props::passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split.get(info);
  if (runNumber<=311481) passesHLT = true;
  if (btagweight>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0; 

  double MaxPt = -1;

  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);

  for (unsigned int i(0); i < inputJets.size(); i++)
    {
      const xAOD::Jet *jet = inputJets.at(i);
      if (fabs(jet->eta())>2.5 || jet->pt()<50e3) continue;
      if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet)!=5 : Props::MV2c10.get( jet ) < btagweight) continue;
      for (unsigned int j(i+1); j < inputJets.size(); j++) {
        const xAOD::Jet *jet2 = inputJets.at(j);
        if (fabs(jet2->eta())>2.5 || jet2->pt()<50e3) continue;
        if (btagweight<0 ? Props::HadronConeExclTruthLabelID.get(jet2)!=5 : Props::MV2c10.get( jet2 ) < btagweight) continue;
	      
	  TLorentzVector HiggsVect = inputJets.at(i)->p4() + inputJets.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = inputJets.at(i);
	    _signalJets.at(1) = inputJets.at(j);
	  }
	}
    }

  if ( _signalJets.at(0) == nullptr or _signalJets.at(1) == nullptr ) {
    return 0;
  }

  // find the pair of jets that yields the highest mjj                                                                                                                     
  std::vector<xAOD::Jet const*> candidates_forward;
  std::vector<xAOD::Jet const*> candidates_central;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (fabs(inputJets.at(i)->eta())>3.2 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>60e3) {
      candidates_forward.push_back( inputJets.at(i) );
    }
    if ((btagweight<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))!=5 : Props::MV2c10.get( inputJets.at(i) ) < btagweight) && fabs(inputJets.at(i)->eta())<2.8 && inputJets.at(i)->pt()>60e3) {
      candidates_central.push_back( inputJets.at(i) );
    }
  }


  if (candidates_forward.size() < 1 || candidates_central.size()<1) return 0;

  double maxMjj(750e3);//min mJJ from trigger
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  for (unsigned int i(0); i < candidates_forward.size(); i++)
    {
      for (unsigned int j(0); j < candidates_central.size(); j++)
	{
	  TLorentzVector jet1(candidates_forward.at(i)->p4());
	  TLorentzVector jet2(candidates_central.at(j)->p4());
	  if((jet1+jet2).M() > maxMjj){
	    maxMjj = (jet1+jet2).M();
	    vbfjet1 = candidates_forward.at(i);
	    vbfjet2 = candidates_central.at(j);
	  }
	}
    }

  if (vbfjet1 == nullptr or vbfjet2 == nullptr) {
    return 0;
  }

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );

  // sort results in pt
  std::sort(_signalJets.begin(),_signalJets.end(),JetSort::sortJetsByPt);

  double simpleHT(0);
  for (unsigned int i(0); i < inputJets.size(); i++) {
    const xAOD::Jet *jet = inputJets.at(i);
    if (fabs(jet->eta())>3.2) continue;
    if (jet->pt()<vbfjet2->pt() && jet->pt()<_signalJets.at(1)->pt()) continue;
    simpleHT += jet->pt(); //keep all central jets with pT >= any of the central tag jets (including the tag jets)
  }
  if (simpleHT<300e3) return 0;

  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_trigger(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info)
{
  double btagweight =0;
  if (WP == "85") btagweight = m_b_85_weight;
  if (WP == "77") btagweight = m_b_77_weight;
  if (WP == "70") btagweight = m_b_70_weight;
  if (WP == "60" || WP == "DEFAULT") btagweight = m_b_60_weight;

  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  passesHLT = Props::passHLT_j175_gsc225_bmv2c1040_split.get(info);
  if (runNumber>=296939 && runNumber<=311481) passesHLT = Props::passHLT_j175_bmv2c2040_split.get(info);
  //if (!passesHLT && !Props::embZpt.exists(info)) return 0;
  if (!passesHLT && !Props::embZpt.exists(info) && !m_isMC) return 0;

  std::vector<xAOD::Jet const*> candidates_b;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()>30e3 && fabs(inputJets.at(i)->eta())<2.5 && Props::MV2c10.get(inputJets.at(i))>btagweight)  candidates_b.push_back( inputJets.at(i) );
  }
  
  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;
  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);
  for (unsigned int i(0); i < candidates_b.size(); i++) {
      for (unsigned int j(0); j < candidates_b.size(); j++) {
	  if (candidates_b.at(i) == candidates_b.at(j)) continue; // no same jet
	  if (candidates_b.at(i)->pt() < candidates_b.at(j)->pt()) continue; // no same jet
	  if ( candidates_b[i]->pt()+candidates_b[j]->pt() > MaxPt ) {
	    MaxPt = candidates_b[i]->pt()+candidates_b[j]->pt();
	    _signalJets.at(0) = candidates_b.at(i);
	    _signalJets.at(1) = candidates_b.at(j);
	  }
	}
    }

  if ( _signalJets.at(0) == nullptr ) return 0;

  std::vector<xAOD::Jet const*> candidates_all;

  double maxMjj(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()<30e3) continue;
    if (fabs(inputJets.at(i)->eta())>4.5) continue;
    if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if ( inputJets.at(i) != _signalJets.at(0) and inputJets.at(i) != _signalJets.at(1)){
      candidates_all.push_back( inputJets.at(i) );
    }
  }
  
  //if (candidates_all.size() < 2) return 0;

  for (unsigned int i(0); i < candidates_all.size(); i++) {
	  TLorentzVector jet1(candidates_all.at(i)->p4());
          //if (fabs(candidates_all.at(i)->eta())<3.2) continue;
          //if (jet1.Pt()<jet2.Pt()) continue;
	  if(jet1.Pt() > maxMjj){
	    maxMjj = jet1.Pt();
	    vbfjet1 = candidates_all.at(i);
	    vbfjet2 = candidates_all.at(i);
	  }
    }

  /*for (unsigned int i(0); i < candidates_all.size(); i++) {
      for (unsigned int j(0); j < candidates_all.size(); j++) {
	  TLorentzVector jet1(candidates_all.at(i)->p4());
	  TLorentzVector jet2(candidates_all.at(j)->p4());
	  if (candidates_all.at(i) == candidates_all.at(j)) continue;
          if (fabs(candidates_all.at(i)->eta())<3.2) continue;
          //if (jet1.Pt()<jet2.Pt()) continue;
	  if(jet1.Pt()+jet2.Pt() > maxMjj){
	    maxMjj = jet1.Pt()+jet2.Pt();
	    vbfjet1 = candidates_all.at(i);
	    vbfjet2 = candidates_all.at(j);
	  }
	}
    }*/

  if (vbfjet1 == nullptr) return 0;
  if (vbfjet2 == nullptr) return 0;

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );
  
  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_single(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info)
{
  double btagweight_lead =0;
  double btagweight_sub =0;
  if (WP == "85"){
    btagweight_lead = m_b_85_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "77"){
    btagweight_lead = m_b_77_weight;
    btagweight_sub = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_70_weight;
  }
  if (WP == "asym" || WP == "DEFAULT"){
    btagweight_lead = m_b_60_weight;
    //btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "truth"){
    btagweight_lead = -1;
    btagweight_sub = -1;
  }
  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  passesHLT = Props::passHLT_j175_gsc225_bmv2c1040_split.get(info);
  if (runNumber<=311481) passesHLT = Props::passHLT_j225_bmv2c2060_split.get(info);
  if (btagweight_lead>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0;

  int nforward = 0;
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (fabs(inputJets.at(i)->eta())>3.2 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>60e3) nforward++;
  }
  if (nforward) return 0;

  std::vector<xAOD::Jet const*> candidates_leading_b;
  std::vector<xAOD::Jet const*> candidates_sub_b;


  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()>240e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
      candidates_leading_b.push_back( inputJets.at(i) );
    }
    if (inputJets.at(i)->pt()>20e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
      candidates_sub_b.push_back( inputJets.at(i) );
    }
  }
  
  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;
  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);
  for (unsigned int i(0); i < candidates_leading_b.size(); i++) {
      for (unsigned int j(0); j < candidates_sub_b.size(); j++) {
	  if (candidates_leading_b.at(i) == candidates_sub_b.at(j)) continue; // no same jet
	  //if (m_runNumber<=311481 && Props::HLTBJetMatched_J80_MV2c20_70.get(candidates_leading_b.at(i)) == Props::HLTBJetMatched_J60_MV2c20_85.get(candidates_sub_b.at(j)))// not matched to same HLT jet
	    //continue;
	  //if (m_runNumber>311481 && Props::HLTBJetMatched_J80_MV2c10_70.get(candidates_leading_b.at(i)) == Props::HLTBJetMatched_J60_MV2c10_85.get(candidates_sub_b.at(j)))// not matched to same HLT jet
	  //  continue;
	  TLorentzVector HiggsVect = candidates_leading_b.at(i)->p4() + candidates_sub_b.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = candidates_leading_b.at(i);
	    _signalJets.at(1) = candidates_sub_b.at(j);
	  }
	}
    }

  if ( _signalJets.at(0) == nullptr ) return 0;

  std::vector<xAOD::Jet const*> candidates_all;

  double maxMjj(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()<30e3) continue;
    if (fabs(inputJets.at(i)->eta())>3.2) continue;
    if (fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if ( inputJets.at(i) != _signalJets.at(0) and inputJets.at(i) != _signalJets.at(1)){
      candidates_all.push_back( inputJets.at(i) );
    }
  }
  
  if (candidates_all.size() < 2) return 0;

  for (unsigned int i(0); i < candidates_all.size(); i++) {
      for (unsigned int j(0); j < candidates_all.size(); j++) {
	  TLorentzVector jet1(candidates_all.at(i)->p4());
	  TLorentzVector jet2(candidates_all.at(j)->p4());
	  if (candidates_all.at(i) == candidates_all.at(j)) continue;
          if (jet1.Pt()<jet2.Pt()) continue;
	  if((jet1+jet2).M() > maxMjj){
	    maxMjj = (jet1+jet2).M();
	    vbfjet1 = candidates_all.at(i);
	    vbfjet2 = candidates_all.at(j);
	  }
	}
    }

  if (vbfjet1 == nullptr) return 0;
  if (vbfjet2 == nullptr) return 0;

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );
  
  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_fake4(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info)
{
  double btagweight_lead =0;
  double btagweight_sub =0;
  if (WP == "85"){
    btagweight_lead = m_b_85_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "77"){
    btagweight_lead = m_b_77_weight;
    btagweight_sub = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_70_weight;
  }
  if (WP == "asym" or WP == "overlap" || WP == "DEFAULT"){
    btagweight_lead = m_b_77_weight;
    //btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_77_weight;
  }
  if (WP == "truth"){
    btagweight_lead = -1;
    btagweight_sub = -1;
  }

  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber<=340453 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30.get(info);
  else if (!Props::embZpt.exists(info)) passesHLT = Props::passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30.get(info);
  if (!Props::embZpt.exists(info) && runNumber<=311481) passesHLT = Props::passHLT_j100_2j55_bmv2c2060_split.get(info);
  //if (!Props::embZpt.exists(info) && runNumber>=296939 && runNumber<=311481) passesHLT = Props::passHLT_j100_2j55_bmv2c2060_split.get(info);
  if (btagweight_lead>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0;




  if (Props::embZpt.exists(info)) {
    //if (Props::embZm.get(info)<81e3 || Props::embZm.get(info)>101e3) return 0;
    //if (Props::embZm.get(info)<66e3 || Props::embZm.get(info)>116e3) return 0;
    if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;
  }

  std::vector<xAOD::Jet const*> candidates_leading_b;
  std::vector<xAOD::Jet const*> candidates_sub_b;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    //if (inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
    if ((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))>60e3) && inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
    //if (inputJets.at(i)->pt()>85e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
      candidates_leading_b.push_back( inputJets.at(i) );
    }
    //if (inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
    if ((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))>60e3) && inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
    //if (inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
      candidates_sub_b.push_back( inputJets.at(i) );
    }
  }
  
  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;
  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);
  for (unsigned int i(0); i < candidates_leading_b.size(); i++) {
      for (unsigned int j(0); j < candidates_sub_b.size(); j++) {
	  if (candidates_leading_b.at(i) == candidates_sub_b.at(j)) continue; // no same jet
	  //if (m_runNumber<=311481 && Props::HLTBJetMatched_J80_MV2c20_70.get(candidates_leading_b.at(i)) == Props::HLTBJetMatched_J60_MV2c20_85.get(candidates_sub_b.at(j)))// not matched to same HLT jet
	    //continue;
	  //if (m_runNumber>311481 && Props::HLTBJetMatched_J80_MV2c10_70.get(candidates_leading_b.at(i)) == Props::HLTBJetMatched_J60_MV2c10_85.get(candidates_sub_b.at(j)))// not matched to same HLT jet
	  //  continue;
	  TLorentzVector HiggsVect = candidates_leading_b.at(i)->p4() + candidates_sub_b.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = candidates_leading_b.at(i);
	    _signalJets.at(1) = candidates_sub_b.at(j);
	  }
	}
    }

  if ( _signalJets.at(0) == nullptr ) return 0;

  std::vector<xAOD::Jet const*> candidates_forward;
  std::vector<xAOD::Jet const*> candidates_all;
  std::vector<xAOD::Jet const*> candidates_real;

  double maxMjj(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  bool finalforward = false;
  int njets = 0;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    inputJets.at(i)->auxdecor<float>("randfactor") = 1;
  }

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i) == _signalJets.at(0) || inputJets.at(i) == _signalJets.at(1)) continue;
    if (fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>160e3) finalforward = true;
    if (fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>30e3) njets++;
    //if (fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>160e3) finalforward = true;
    //if (fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>30e3) njets++;
  }
  if (njets<2) finalforward = false;

  int ntries = 0;
  while (finalforward && ntries<1000) {
    ntries++;
    for (unsigned int i(0); i < inputJets.size(); i++) {
      if (inputJets.at(i) == _signalJets.at(0) || inputJets.at(i) == _signalJets.at(1)) continue;
      float eta = inputJets.at(i)->eta();
      inputJets.at(i)->auxdecor<float>("randfactor") = m_rand->Gaus(eta,100e3/inputJets.at(i)->pt())/(eta!=0?eta:1);
    }
    for (unsigned int i(0); i < inputJets.size(); i++) {
      if (inputJets.at(i) == _signalJets.at(0) || inputJets.at(i) == _signalJets.at(1)) continue;
      if (fabs(inputJets.at(i)->eta())>2.5) continue;
      //if (fabs(inputJets.at(i)->eta())>2.5) continue;
      if (inputJets.at(i)->pt()<160e3 || fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))>3.1) continue;
      for (unsigned int j(0); j < inputJets.size(); j++) {
        if (inputJets.at(j) == _signalJets.at(0) || inputJets.at(j) == _signalJets.at(1)) continue;
        if (fabs(inputJets.at(j)->eta())>2.5) continue;
        //if (fabs(inputJets.at(j)->eta())>2.5) continue;
        if (inputJets.at(j)->pt()<30e3 || fabs(inputJets.at(j)->eta()*inputJets.at(j)->auxdata<float>("randfactor"))>4.5) continue;
        TLorentzVector tlv1,tlv2;
        tlv1.SetPtEtaPhiM(inputJets.at(i)->pt(),inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"),inputJets.at(i)->phi(),inputJets.at(i)->m());
        tlv2.SetPtEtaPhiM(inputJets.at(j)->pt(),inputJets.at(j)->eta()*inputJets.at(j)->auxdata<float>("randfactor"),inputJets.at(j)->phi(),inputJets.at(j)->m());
        if ((tlv1+tlv2).M()<800e3) continue;
        finalforward = false;
      }
    }
  }

  for (unsigned int i(0); i < inputJets.size(); i++) {
    //if(fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<3.1 && fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>160e3 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    if((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))>150e3) && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<3.1 && fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>160e3 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    //if((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))>150e3) && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<3.1 && fabs(inputJets.at(i)->eta())<3.1 && inputJets.at(i)->pt()>160e3 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    //if((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))>150e3) && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<3.1 && fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>160e3 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    //if (fabs(inputJets.at(i)->eta())<3.1 && inputJets.at(i)->pt()>160e3 && (Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))>150e3)) {
      candidates_forward.push_back(inputJets.at(i));
    }
  }

  // not signal candidates
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()<30e3) continue;
     
    //if (fabs(inputJets.at(i)->eta())<4.5 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    if (fabs(inputJets.at(i)->eta())<2.5 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    //if (fabs(inputJets.at(i)->eta())<3.1 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    //if (fabs(inputJets.at(i)->eta())<2.5 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
      candidates_all.push_back( inputJets.at(i) );
    }
    if (fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
      candidates_real.push_back( inputJets.at(i) );
    }
  }
  float maxmJJ = 0;
  float maxeta = 0;
  for (unsigned int i(0); i < candidates_real.size(); i++) {
    if (candidates_real.at(i)->pt()>60e3 && fabs(candidates_real.at(i)->eta())>maxeta) maxeta = fabs(candidates_real.at(i)->eta());
    for (unsigned int j(0); j < i; j++) {
      if (candidates_real.at(i)->pt()<160e3 && candidates_real.at(j)->pt()<160e3) continue;
      float newmJJ = (candidates_real.at(i)->p4()+candidates_real.at(j)->p4()).M();
      if (newmJJ>maxmJJ) maxmJJ = newmJJ;
    }
  }
  
  if (candidates_forward.size()<1 or candidates_all.size() < 1) return 0;
  if (maxmJJ>800e3) return 0;
  if (maxmJJ<200e3) return 0;
  if (maxeta>3.2) return 0;

  for (unsigned int i(0); i < candidates_forward.size(); i++) {
      for (unsigned int j(0); j < candidates_all.size(); j++) {
	  TLorentzVector jet1;
	  TLorentzVector jet2;
          //jet1.SetPtEtaPhiM(candidates_forward.at(i)->pt(),candidates_forward.at(i)->eta(),candidates_forward.at(i)->phi(),candidates_forward.at(i)->m());
          //jet2.SetPtEtaPhiM(candidates_all.at(j)->pt(),candidates_all.at(j)->eta(),candidates_all.at(j)->phi(),candidates_all.at(j)->m());
          jet1.SetPtEtaPhiM(candidates_forward.at(i)->pt(),candidates_forward.at(i)->eta()*candidates_forward.at(i)->auxdata<float>("randfactor"),candidates_forward.at(i)->phi(),candidates_forward.at(i)->m());
          jet2.SetPtEtaPhiM(candidates_all.at(j)->pt(),candidates_all.at(j)->eta()*candidates_all.at(j)->auxdata<float>("randfactor"),candidates_all.at(j)->phi(),candidates_all.at(j)->m());
          if (jet1.Pt()<jet2.Pt() && fabs(jet2.Eta())<3.1) continue;
	  if (candidates_forward.at(i) == candidates_all.at(j)) continue;
	  if((jet1+jet2).M() > maxMjj){
	    maxMjj = (jet1+jet2).M();
	    vbfjet1 = candidates_forward.at(i);
	    vbfjet2 = candidates_all.at(j);
	  }
	}
    }

  if (vbfjet1 == nullptr) return 0;
  if (vbfjet2 == nullptr) return 0;

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );
  
  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_fake3(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info)
{

  m_rand->SetSeed(info->eventNumber()*2);

  double btagweight_lead =0;
  double btagweight_sub =0;
  if (WP == "85"){
    btagweight_lead = m_b_85_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "77"){
    btagweight_lead = m_b_77_weight;
    btagweight_sub = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_70_weight;
  }
  if (WP == "asym" or WP == "overlap" || WP == "DEFAULT"){
    btagweight_lead = m_b_77_weight;
    //btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_77_weight;
  }
  if (WP == "truth"){
    btagweight_lead = -1;
    btagweight_sub = -1;
  }

  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber<=340453 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30.get(info);
  else if (!Props::embZpt.exists(info)) passesHLT = Props::passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30.get(info);
  if (!Props::embZpt.exists(info) && runNumber<=311481) passesHLT = Props::passHLT_j100_2j55_bmv2c2060_split.get(info);
  //if (!Props::embZpt.exists(info) && runNumber>=296939 && runNumber<=311481) passesHLT = Props::passHLT_j100_2j55_bmv2c2060_split.get(info);
  if (btagweight_lead>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0;




  if (Props::embZpt.exists(info)) {
    //if (Props::embZm.get(info)<81e3 || Props::embZm.get(info)>101e3) return 0;
    //if (Props::embZm.get(info)<66e3 || Props::embZm.get(info)>116e3) return 0;
    if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;
  }

  std::vector<xAOD::Jet const*> candidates_leading_b;
  std::vector<xAOD::Jet const*> candidates_sub_b;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    //if (inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
    if ((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : (!Props::HLTGSCJetMatchedPt.exists(inputJets.at(i)) ? 61e3 : Props::HLTGSCJetMatchedPt.get(inputJets.at(i))) )>60e3) && inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
      candidates_leading_b.push_back( inputJets.at(i) );
    }
    //if (inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
    //if ((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))>60e3) && inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
    if ((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : (!Props::HLTGSCJetMatchedPt.exists(inputJets.at(i)) ? 61e3 : Props::HLTGSCJetMatchedPt.get(inputJets.at(i))) )>60e3) && inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
    //if (inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
      candidates_sub_b.push_back( inputJets.at(i) );
    }
  }
  
  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;
  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);
  for (unsigned int i(0); i < candidates_leading_b.size(); i++) {
      for (unsigned int j(0); j < candidates_sub_b.size(); j++) {
	  if (candidates_leading_b.at(i) == candidates_sub_b.at(j)) continue; // no same jet
	  //if (m_runNumber<=311481 && Props::HLTBJetMatched_J80_MV2c20_70.get(candidates_leading_b.at(i)) == Props::HLTBJetMatched_J60_MV2c20_85.get(candidates_sub_b.at(j)))// not matched to same HLT jet
	    //continue;
	  //if (m_runNumber>311481 && Props::HLTBJetMatched_J80_MV2c10_70.get(candidates_leading_b.at(i)) == Props::HLTBJetMatched_J60_MV2c10_85.get(candidates_sub_b.at(j)))// not matched to same HLT jet
	  //  continue;
	  TLorentzVector HiggsVect = candidates_leading_b.at(i)->p4() + candidates_sub_b.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = candidates_leading_b.at(i);
	    _signalJets.at(1) = candidates_sub_b.at(j);
	  }
	}
    }

  if ( _signalJets.at(0) == nullptr ) return 0;

  std::vector<xAOD::Jet const*> candidates_forward;
  std::vector<xAOD::Jet const*> candidates_all;
  std::vector<xAOD::Jet const*> candidates_real;

  double maxMjj(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  bool finalforward = false;
  int njets = 0;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    inputJets.at(i)->auxdecor<float>("randfactor") = 1;
  }

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i) == _signalJets.at(0) || inputJets.at(i) == _signalJets.at(1)) continue;
    if (fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>160e3) finalforward = true;
    if (fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>30e3) njets++;
    //if (fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>160e3) finalforward = true;
    //if (fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>30e3) njets++;
  }
  if (njets<2) finalforward = false;

  int ntries = 0;
  while (finalforward && ntries<1000) {
    ntries++;
    for (unsigned int i(0); i < inputJets.size(); i++) {
      if (inputJets.at(i) == _signalJets.at(0) || inputJets.at(i) == _signalJets.at(1)) continue;
      float eta = inputJets.at(i)->eta();
      inputJets.at(i)->auxdecor<float>("randfactor") = m_rand->Gaus(eta,200e3/inputJets.at(i)->pt())/(eta!=0?eta:1);
    }
    for (unsigned int i(0); i < inputJets.size(); i++) {
      if (inputJets.at(i) == _signalJets.at(0) || inputJets.at(i) == _signalJets.at(1)) continue;
      if (fabs(inputJets.at(i)->eta())>2.5) continue;
      //if (fabs(inputJets.at(i)->eta())>2.5) continue;
      if (inputJets.at(i)->pt()<160e3 || fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))>3.1) continue;
      for (unsigned int j(0); j < inputJets.size(); j++) {
        if (inputJets.at(j) == _signalJets.at(0) || inputJets.at(j) == _signalJets.at(1)) continue;
        if (fabs(inputJets.at(j)->eta())>2.5) continue;
        //if (fabs(inputJets.at(j)->eta())>2.5) continue;
        if (inputJets.at(j)->pt()<30e3 || fabs(inputJets.at(j)->eta()*inputJets.at(j)->auxdata<float>("randfactor"))>4.5) continue;
        TLorentzVector tlv1,tlv2;
        tlv1.SetPtEtaPhiM(inputJets.at(i)->pt(),inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"),inputJets.at(i)->phi(),inputJets.at(i)->m());
        tlv2.SetPtEtaPhiM(inputJets.at(j)->pt(),inputJets.at(j)->eta()*inputJets.at(j)->auxdata<float>("randfactor"),inputJets.at(j)->phi(),inputJets.at(j)->m());
        if ((tlv1+tlv2).M()<800e3) continue;
        finalforward = false;
      }
    }
  }

  for (unsigned int i(0); i < inputJets.size(); i++) {
    //if(fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<3.1 && fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>160e3 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    if((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : (!Props::HLTGSCJetMatchedPt.exists(inputJets.at(i)) ? 151e3 : Props::HLTGSCJetMatchedPt.get(inputJets.at(i))) )>150e3) && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<3.1 && fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>160e3 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    //if((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))>150e3) && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<3.1 && fabs(inputJets.at(i)->eta())<3.1 && inputJets.at(i)->pt()>160e3 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    //if((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))>150e3) && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<3.1 && fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>160e3 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    //if (fabs(inputJets.at(i)->eta())<3.1 && inputJets.at(i)->pt()>160e3 && (Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))>150e3)) {
      candidates_forward.push_back(inputJets.at(i));
    }
  }

  // not signal candidates
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()<30e3) continue;
     
    //if (fabs(inputJets.at(i)->eta())<4.5 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    //if (fabs(inputJets.at(i)->eta())<2.5 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    //if (fabs(inputJets.at(i)->eta())<3.1 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    if (fabs(inputJets.at(i)->eta())<2.5 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
      candidates_all.push_back( inputJets.at(i) );
    }
    if (fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
      candidates_real.push_back( inputJets.at(i) );
    }
  }
  float maxmJJ = 0;
  float maxeta = 0;
  for (unsigned int i(0); i < candidates_real.size(); i++) {
    if (candidates_real.at(i)->pt()>60e3 && fabs(candidates_real.at(i)->eta())>maxeta) maxeta = fabs(candidates_real.at(i)->eta());
    for (unsigned int j(0); j < i; j++) {
      if (candidates_real.at(i)->pt()<160e3 && candidates_real.at(j)->pt()<160e3) continue;
      float newmJJ = (candidates_real.at(i)->p4()+candidates_real.at(j)->p4()).M();
      if (newmJJ>maxmJJ) maxmJJ = newmJJ;
    }
  }
  
  if (candidates_forward.size()<1 or candidates_all.size() < 1) return 0;
  if (maxmJJ>800e3) return 0;
  if (maxmJJ<0e3) return 0;
  if (maxeta>3.2) return 0;
  info->auxdecor<float>("mJJ_base") = maxmJJ;

  for (unsigned int i(0); i < candidates_forward.size(); i++) {
      for (unsigned int j(0); j < candidates_all.size(); j++) {
	  TLorentzVector jet1;
	  TLorentzVector jet2;
          //jet1.SetPtEtaPhiM(candidates_forward.at(i)->pt(),candidates_forward.at(i)->eta(),candidates_forward.at(i)->phi(),candidates_forward.at(i)->m());
          //jet2.SetPtEtaPhiM(candidates_all.at(j)->pt(),candidates_all.at(j)->eta(),candidates_all.at(j)->phi(),candidates_all.at(j)->m());
          jet1.SetPtEtaPhiM(candidates_forward.at(i)->pt(),candidates_forward.at(i)->eta()*candidates_forward.at(i)->auxdata<float>("randfactor"),candidates_forward.at(i)->phi(),candidates_forward.at(i)->m());
          jet2.SetPtEtaPhiM(candidates_all.at(j)->pt(),candidates_all.at(j)->eta()*candidates_all.at(j)->auxdata<float>("randfactor"),candidates_all.at(j)->phi(),candidates_all.at(j)->m());
          if (jet1.Pt()<jet2.Pt() && fabs(jet2.Eta())<3.1) continue;
	  if (candidates_forward.at(i) == candidates_all.at(j)) continue;
	  if((jet1+jet2).M() > maxMjj){
	    maxMjj = (jet1+jet2).M();
	    vbfjet1 = candidates_forward.at(i);
	    vbfjet2 = candidates_all.at(j);
	  }
	}
    }

  if (vbfjet1 == nullptr) return 0;
  if (vbfjet2 == nullptr) return 0;

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );
  
  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_fake(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info)
{
  m_rand->SetSeed(info->eventNumber()*3);

  double btagweight_lead =0;
  double btagweight_sub =0;
  if (WP == "85"){
    btagweight_lead = m_b_85_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "77"){
    btagweight_lead = m_b_77_weight;
    btagweight_sub = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_70_weight;
  }
  if (WP == "asym" or WP == "overlap" || WP == "DEFAULT"){
    btagweight_lead = m_b_77_weight;
    //btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "truth"){
    btagweight_lead = -1;
    btagweight_sub = -1;
  }

  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber>340453 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.get(info);
  else if (runNumber>=338675 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  else if (!Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  //if (runNumber<=303943 && !m_isMC) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  if (runNumber<=303943 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  else if (runNumber<=311481 && !Props::embZpt.exists(info)) passesHLT = ((Props::passHLT_2j35_bmv2c2060_split_2j35_L14J15.get(info)));
/*
  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber>340453 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.get(info);
  else if (runNumber>=338675 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  else if (!Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  //if (runNumber<=303943 && !m_isMC) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  if (runNumber<=303943 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  else if (runNumber<=311481 && !Props::embZpt.exists(info)) passesHLT = ((Props::passHLT_2j35_bmv2c2060_split_2j35_L14J15.get(info)));
*/
  if (btagweight_lead>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0;

//if (Props::passHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25.get(info)) passesHLT = true;
//if (Props::passHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25.get(info)) passesHLT = true;
//HLT_j50_gsc65_bmv2c1040_split_3j50_gsc65_boffperf_split
//HLT_2j45_gsc55_bmv2c1060_split_ht300_L1HT190-J15s5.ETA21
//HLT_2j45_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21



  if (Props::embZpt.exists(info)) {
    //if (Props::embZm.get(info)<81e3 || Props::embZm.get(info)>101e3) return 0;
    //if (Props::embZm.get(info)<66e3 || Props::embZm.get(info)>116e3) return 0;
    if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;
  }

  std::vector<xAOD::Jet const*> candidates_leading_b;
  std::vector<xAOD::Jet const*> candidates_sub_b;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if ((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTJetMatchedPt.get(inputJets.at(i)))>80e3) && inputJets.at(i)->pt()>85e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
    //if (inputJets.at(i)->pt()>85e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
      candidates_leading_b.push_back( inputJets.at(i) );
    }
    if ((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTJetMatchedPt.get(inputJets.at(i)))>60e3) && inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
    //if (inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
      candidates_sub_b.push_back( inputJets.at(i) );
    }
  }
  
  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;
  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);
  for (unsigned int i(0); i < candidates_leading_b.size(); i++) {
      for (unsigned int j(0); j < candidates_sub_b.size(); j++) {
	  if (candidates_leading_b.at(i) == candidates_sub_b.at(j)) continue; // no same jet
	  //if (m_runNumber<=311481 && Props::HLTBJetMatched_J80_MV2c20_70.get(candidates_leading_b.at(i)) == Props::HLTBJetMatched_J60_MV2c20_85.get(candidates_sub_b.at(j)))// not matched to same HLT jet
	    //continue;
	  //if (m_runNumber>311481 && Props::HLTBJetMatched_J80_MV2c10_70.get(candidates_leading_b.at(i)) == Props::HLTBJetMatched_J60_MV2c10_85.get(candidates_sub_b.at(j)))// not matched to same HLT jet
	  //  continue;
	  TLorentzVector HiggsVect = candidates_leading_b.at(i)->p4() + candidates_sub_b.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = candidates_leading_b.at(i);
	    _signalJets.at(1) = candidates_sub_b.at(j);
	  }
	}
    }

  if ( _signalJets.at(0) == nullptr ) return 0;
  TLorentzVector tlvbb = (_signalJets.at(0)->p4()+_signalJets.at(1)->p4());

  std::vector<xAOD::Jet const*> candidates_forward;
  std::vector<xAOD::Jet const*> candidates_all;
  std::vector<xAOD::Jet const*> candidates_real;

  double maxMjj(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  bool finalforward = false;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    inputJets.at(i)->auxdecor<float>("randfactor") = 1;
  }

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i) == _signalJets.at(0) || inputJets.at(i) == _signalJets.at(1)) continue;
    if (inputJets.at(i)->pt()>60e3) finalforward = true;
  }

  int ntries = 0;
  while (finalforward && ntries<1000) {
    TLorentzVector recoil;
    for (unsigned int i(0); i < inputJets.size(); i++) {
      if (inputJets.at(i) == _signalJets.at(0) || inputJets.at(i) == _signalJets.at(1)) continue;
      float eta = inputJets.at(i)->eta();
      inputJets.at(i)->auxdecor<float>("randfactor") = m_rand->Gaus(eta,200e3/inputJets.at(i)->pt())/(eta!=0?eta:1);
      //inputJets.at(i)->auxdecor<float>("randfactor") = m_rand->Gaus(eta,100e3/inputJets.at(i)->pt())/(eta!=0?eta:1);
      TLorentzVector tlv;
      tlv.SetPtEtaPhiM(inputJets.at(i)->pt(),inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"),inputJets.at(i)->phi(),inputJets.at(i)->m());
      recoil += tlv;
    }
    //float decay = (recoil.Eta()-tlvbb.Eta())*(m_rand->Exp(2));
    /*for (unsigned int i(0); i < inputJets.size(); i++) {
      if (inputJets.at(i) == _signalJets.at(0) || inputJets.at(i) == _signalJets.at(1)) continue;
      float origeta = inputJets.at(i)->eta();
      float eta = inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor");
      inputJets.at(i)->auxdecor<float>("randfactor") = (eta-decay)/origeta;
    }*/
    for (unsigned int i(0); i < inputJets.size(); i++) {
      if (inputJets.at(i) == _signalJets.at(0) || inputJets.at(i) == _signalJets.at(1)) continue;
      float eta = inputJets.at(i)->eta();
      if (inputJets.at(i)->pt()>60e3 && fabs(eta*inputJets.at(i)->auxdata<float>("randfactor"))>3.2 && fabs(eta*inputJets.at(i)->auxdata<float>("randfactor"))<4.5) finalforward = false;
    }


    ntries++;
  }

  // forward jets
  for (unsigned int i(0); i < inputJets.size(); i++) {
    //if(Props::HLTJetMatchedPt.get(inputJets.at(i))>45e3 && fabs(inputJets.at(i)->eta())>1.778 && fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>60e3 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    //if(Props::HLTJetMatchedPt.get(inputJets.at(i))>45e3 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))>3.2 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>60e3 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    //if(Props::HLTJetMatchedPt.get(inputJets.at(i))>45e3 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))>3.2 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>60e3 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    if(fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))>3.2 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && fabs(inputJets.at(i)->eta())<2.5 && inputJets.at(i)->pt()>60e3 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
      if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
      candidates_forward.push_back(inputJets.at(i));
    }
  }

  // not signal candidates
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()<30e3) continue;
     
    if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    //if (Props::HLTJetMatchedPt.get(inputJets.at(i))>35e3 && fabs(inputJets.at(i)->eta())<2.5 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    if (fabs(inputJets.at(i)->eta())<2.5 && fabs(inputJets.at(i)->eta()*inputJets.at(i)->auxdata<float>("randfactor"))<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
      candidates_all.push_back( inputJets.at(i) );
    }
    //if (Props::HLTJetMatchedPt.get(inputJets.at(i))>35e3 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
    if (fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i) != _signalJets.at(0) && inputJets.at(i) != _signalJets.at(1)){
      candidates_real.push_back( inputJets.at(i) );
    }
  }
  float maxmJJ = 0;
  float maxeta = 0;
  for (unsigned int i(0); i < candidates_real.size(); i++) {
    //if (candidates_real.at(i)->pt()>30e3 && fabs(candidates_real.at(i)->eta())>maxeta) maxeta = fabs(candidates_real.at(i)->eta());
    if (candidates_real.at(i)->pt()>60e3 && fabs(candidates_real.at(i)->eta())>maxeta) maxeta = fabs(candidates_real.at(i)->eta());
    for (unsigned int j(0); j < i; j++) {
      //if (candidates_real.at(i)->pt()<60e3 && candidates_real.at(j)->pt()<60e3) continue;
      float newmJJ = (candidates_real.at(i)->p4()+candidates_real.at(j)->p4()).M();
      if (newmJJ>maxmJJ) maxmJJ = newmJJ;
    }
  }
  
  if (candidates_forward.size()<1 or candidates_all.size() < 1) return 0;
  if (maxmJJ>800e3) return 0;
  if (maxeta>3.2) return 0;
  info->auxdecor<float>("mJJ_base") = maxmJJ;

  for (unsigned int i(0); i < candidates_forward.size(); i++) {
      for (unsigned int j(0); j < candidates_all.size(); j++) {
	  TLorentzVector jet1;
	  TLorentzVector jet2;
          //jet1.SetPtEtaPhiM(candidates_forward.at(i)->pt(),candidates_forward.at(i)->eta(),candidates_forward.at(i)->phi(),candidates_forward.at(i)->m());
          //jet2.SetPtEtaPhiM(candidates_all.at(j)->pt(),candidates_all.at(j)->eta(),candidates_all.at(j)->phi(),candidates_all.at(j)->m());
          jet1.SetPtEtaPhiM(candidates_forward.at(i)->pt(),candidates_forward.at(i)->eta()*candidates_forward.at(i)->auxdata<float>("randfactor"),candidates_forward.at(i)->phi(),candidates_forward.at(i)->m());
          jet2.SetPtEtaPhiM(candidates_all.at(j)->pt(),candidates_all.at(j)->eta()*candidates_all.at(j)->auxdata<float>("randfactor"),candidates_all.at(j)->phi(),candidates_all.at(j)->m());
          if (jet1.Pt()<jet2.Pt() && fabs(jet2.Eta())>3.2) continue;
	  if (candidates_forward.at(i) == candidates_all.at(j)) continue;
	  if((jet1+jet2).M() > maxMjj){
	    maxMjj = (jet1+jet2).M();
	    vbfjet1 = candidates_forward.at(i);
	    vbfjet2 = candidates_all.at(j);
	  }
	}
    }

  if (vbfjet1 == nullptr) return 0;
  if (vbfjet2 == nullptr) return 0;

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );
  
  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_1for2cen(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info, bool allowFF,bool dotrig)
{
  double btagweight_lead =0;
  double btagweight_sub =0;
  if (WP == "85"){
    btagweight_lead = m_b_85_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "77"){
    btagweight_lead = m_b_77_weight;
    btagweight_sub = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_70_weight;
  }
  if (WP == "asym" or WP == "overlap" || WP == "DEFAULT"){
    btagweight_lead = m_b_77_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "truth"){
    btagweight_lead = -1;
    btagweight_sub = -1;
  }
  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  passesHLT = Props::passHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490.get(info);
  if (runNumber<=311481) passesHLT = Props::passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490.get(info);
  if (dotrig && btagweight_lead>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0;

  if (Props::embZpt.exists(info)) if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;

  std::vector<xAOD::Jet const*> candidates_leading_b;
  std::vector<xAOD::Jet const*> candidates_sub_b;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if ((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) :(!Props::HLTGSCJetMatchedPt.exists(inputJets.at(i)) ? 81e3 : Props::HLTGSCJetMatchedPt.get(inputJets.at(i))))>80e3) && inputJets.at(i)->pt()>85e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
    //if ((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))>80e3) && inputJets.at(i)->pt()>85e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
    //if (inputJets.at(i)->pt()>85e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
      candidates_leading_b.push_back( inputJets.at(i) );
    }
    if ((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : (!Props::HLTGSCJetMatchedPt.exists(inputJets.at(i)) ? 61e3 : Props::HLTGSCJetMatchedPt.get(inputJets.at(i))))>60e3) && inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
    //if ((Props::embZpt.exists(info) || (runNumber<=311481 ? Props::HLTJetMatchedPt.get(inputJets.at(i)) : Props::HLTGSCJetMatchedPt.get(inputJets.at(i)))>60e3) && inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
    //if (inputJets.at(i)->pt()>65e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
      candidates_sub_b.push_back( inputJets.at(i) );
    }
  }
  
  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;
  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);
  for (unsigned int i(0); i < candidates_leading_b.size(); i++) {
    for (unsigned int j(0); j < candidates_sub_b.size(); j++) {
      if (candidates_leading_b.at(i) == candidates_sub_b.at(j)) continue; // no same jet
      TLorentzVector HiggsVect = candidates_leading_b.at(i)->p4() + candidates_sub_b.at(j)->p4();
      if ( HiggsVect.Pt() < MaxPt ) continue;
      MaxPt = HiggsVect.Pt();
      _signalJets.at(0) = candidates_leading_b.at(i);
      _signalJets.at(1) = candidates_sub_b.at(j);
    }
  }

  if ( _signalJets.at(0) == nullptr ) return 1;

  std::vector<xAOD::Jet const*> candidates_forward;
  std::vector<xAOD::Jet const*> candidates_all;

  double maxMjj(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  // forward jets
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if(fabs(inputJets.at(i)->eta())>3.2 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>60e3){
      if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
      candidates_forward.push_back(inputJets.at(i));
    }
  }

  // not signal candidates
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()<30e3) continue;
    if (fabs(inputJets.at(i)->eta())>(allowFF?4.5:3.2)) continue;
    if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if ( inputJets.at(i) != _signalJets.at(0) and inputJets.at(i) != _signalJets.at(1)){
      candidates_all.push_back( inputJets.at(i) );
    }
  }
  
  if (candidates_forward.size()<1 or candidates_all.size() < 1) return 3;

  for (unsigned int i(0); i < candidates_forward.size(); i++) {
    for (unsigned int j(0); j < candidates_all.size(); j++) {
      TLorentzVector jet1(candidates_forward.at(i)->p4());
      TLorentzVector jet2(candidates_all.at(j)->p4());
      if (candidates_forward.at(i) == candidates_all.at(j)) continue;
      if((jet1+jet2).M() < maxMjj) continue;
      maxMjj = (jet1+jet2).M();
      vbfjet1 = candidates_forward.at(i);
      vbfjet2 = candidates_all.at(j);
    }
  }

  if (vbfjet1 == nullptr) return 4;
  if (vbfjet2 == nullptr) return 4;
  vbfjet1->auxdecor<float>("randfactor") = 1;
  vbfjet2->auxdecor<float>("randfactor") = 1;

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );
  
  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_1for2cen_triggerJ1(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info, bool allowFF,bool dotrig)
{
  double btagweight_lead =0;
  double btagweight_sub =0;
  if (WP == "85"){
    btagweight_lead = m_b_85_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "77"){
    btagweight_lead = m_b_77_weight;
    btagweight_sub = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_70_weight;
  }
  if (WP == "asym" or WP == "overlap" || WP == "DEFAULT"){
    btagweight_lead = m_b_60_weight;
    btagweight_sub = m_b_60_weight;
  }
  if (WP == "truth"){
    btagweight_lead = -1;
    btagweight_sub = -1;
  }
  //bool passesHLT = false;
  //UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  //passesHLT = Props::passHLT_j175_gsc225_bmv2c1040_split.get(info);
  //if (runNumber>=296939 && runNumber<=311481) passesHLT = Props::passHLT_j175_bmv2c2040_split.get(info);
  //if (!passesHLT && !Props::embZpt.exists(info)) return 0;
  //if (!passesHLT && !Props::embZpt.exists(info) && !m_isMC) return 0;

  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber>340453 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.get(info);
  else if (runNumber>=338675 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  else if (!Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  //if (runNumber<=303943 && !m_isMC) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  if (runNumber<=303943 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  else if (runNumber<=311481 && !Props::embZpt.exists(info)) passesHLT = ((Props::passHLT_2j35_bmv2c2060_split_2j35_L14J15.get(info)));
  if (btagweight_lead>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0;
  if (Props::embZpt.exists(info)) if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;



  std::vector<xAOD::Jet const*> candidates_leading_b;
  std::vector<xAOD::Jet const*> candidates_sub_b;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()>100e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
      candidates_leading_b.push_back( inputJets.at(i) );
    }
    if (inputJets.at(i)->pt()>80e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
      candidates_sub_b.push_back( inputJets.at(i) );
    }
  }
  
  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;
  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);
  for (unsigned int i(0); i < candidates_leading_b.size(); i++) {
    for (unsigned int j(0); j < candidates_sub_b.size(); j++) {
      if (candidates_leading_b.at(i) == candidates_sub_b.at(j)) continue; // no same jet
      TLorentzVector HiggsVect = candidates_leading_b.at(i)->p4() + candidates_sub_b.at(j)->p4();
      if ( HiggsVect.Pt() < MaxPt ) continue;
      MaxPt = HiggsVect.Pt();
      _signalJets.at(0) = candidates_leading_b.at(i);
      _signalJets.at(1) = candidates_sub_b.at(j);
    }
  }

  if ( _signalJets.at(0) == nullptr ) return 0;

  std::vector<xAOD::Jet const*> candidates_forward;
  std::vector<xAOD::Jet const*> candidates_all;

  double maxMjj(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  // forward jets
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if(fabs(inputJets.at(i)->eta())>3. && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>40e3){
      if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
      candidates_forward.push_back(inputJets.at(i));
    }
  }

  // not signal candidates
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()<30e3) continue;
    if (fabs(inputJets.at(i)->eta())>(allowFF?4.5:3.2)) continue;
    if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if ( inputJets.at(i) != _signalJets.at(0) and inputJets.at(i) != _signalJets.at(1)){
      candidates_all.push_back( inputJets.at(i) );
    }
  }
  
  if (candidates_forward.size()!=1 || candidates_all.size() < 1) return 0;

  for (unsigned int i(0); i < candidates_forward.size(); i++) {
    for (unsigned int j(0); j < candidates_all.size(); j++) {
      TLorentzVector jet1(candidates_forward.at(i)->p4());
      TLorentzVector jet2(candidates_all.at(j)->p4());
      if (candidates_forward.at(i) == candidates_all.at(j)) continue;
      //if(jet1.Pt()+jet2.Pt() < maxMjj) continue;
      //maxMjj = jet1.Pt()+jet2.Pt();
      if((jet1+jet2).M() < maxMjj) continue;
      maxMjj = (jet1+jet2).M();
      vbfjet1 = candidates_forward.at(i);
      vbfjet2 = candidates_all.at(j);
    }
  }

  if (vbfjet1 == nullptr) return 0;
  if (vbfjet2 == nullptr) return 0;
  vbfjet1->auxdecor<float>("randfactor") = 1;
  vbfjet2->auxdecor<float>("randfactor") = 1;

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );
  
  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_1for2cen_triggerB2(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info, bool allowFF,bool dotrig)
{
  double btagweight_lead =0;
  double btagweight_sub =0;
  if (WP == "85"){
    btagweight_lead = m_b_85_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "77"){
    btagweight_lead = m_b_77_weight;
    btagweight_sub = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_70_weight;
  }
  if (WP == "asym" or WP == "overlap" || WP == "DEFAULT"){
    btagweight_lead = m_b_60_weight;
    btagweight_sub = m_b_60_weight;
  }
  if (WP == "truth"){
    btagweight_lead = -1;
    btagweight_sub = -1;
  }
  //bool passesHLT = false;
  //UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  //passesHLT = Props::passHLT_j175_gsc225_bmv2c1040_split.get(info);
  //if (runNumber>=296939 && runNumber<=311481) passesHLT = Props::passHLT_j175_bmv2c2040_split.get(info);
  //if (!passesHLT && !Props::embZpt.exists(info) && !m_isMC) return 0;
  //if (Props::embZpt.exists(info)) if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;


  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber>340453 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25.get(info);
  else if (runNumber>=338675 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  else if (!Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25.get(info);//need to actually use 60 for the most part, but then need to do the lumi correctly
  //if (runNumber<=303943 && !m_isMC) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  if (runNumber<=303943 && !Props::embZpt.exists(info)) passesHLT = Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15.get(info);
  else if (runNumber<=311481 && !Props::embZpt.exists(info)) passesHLT = ((Props::passHLT_2j35_bmv2c2060_split_2j35_L14J15.get(info)));
  if (btagweight_lead>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0;
  if (Props::embZpt.exists(info)) if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;

  std::vector<xAOD::Jet const*> candidates_leading_b;
  std::vector<xAOD::Jet const*> candidates_sub_b;

  int nbjets = 0;
  for (unsigned int i(0); i < inputJets.size(); i++) {
    const xAOD::Jet *jet = inputJets.at(i);
    if (fabs(jet->eta())<2.5 && jet->pt()>40e3 && Props::MV2c10.get( jet ) > m_b_85_weight) nbjets++;
    if (inputJets.at(i)->pt()>100e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
      candidates_leading_b.push_back( inputJets.at(i) );
    }
    if (inputJets.at(i)->pt()>40e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
      candidates_sub_b.push_back( inputJets.at(i) );
    }
  }
  
  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;
  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);
  for (unsigned int i(0); i < candidates_leading_b.size(); i++) {
    for (unsigned int j(0); j < candidates_sub_b.size(); j++) {
      if (candidates_leading_b.at(i) == candidates_sub_b.at(j)) continue; // no same jet
      float HiggsVect = candidates_leading_b.at(i)->pt() + candidates_sub_b.at(j)->pt();
      //TLorentzVector HiggsVect = candidates_leading_b.at(i)->p4() + candidates_sub_b.at(j)->p4();
      if ( HiggsVect < MaxPt ) continue;
      MaxPt = HiggsVect;
      _signalJets.at(0) = candidates_leading_b.at(i);
      _signalJets.at(1) = candidates_sub_b.at(j);
    }
  }

  if (nbjets!=2 || _signalJets.at(0) == nullptr ) return 0;

  std::vector<xAOD::Jet const*> candidates_forward;
  std::vector<xAOD::Jet const*> candidates_all;

  double maxMjj(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  // forward jets
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if(fabs(inputJets.at(i)->eta())>3.2 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>80e3){
      if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
      candidates_forward.push_back(inputJets.at(i));
    }
  }

  // not signal candidates
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()<30e3) continue;
    if (fabs(inputJets.at(i)->eta())>(allowFF?4.5:3.2)) continue;
    if (Props::Timing.exists(inputJets.at(i)) && fabs(inputJets.at(i)->eta())>2.5 && (!Props::PassFJvtLoose.get( inputJets.at(i) ) || fabs(Props::Timing.get( inputJets.at(i) ))>10.)) continue;
    if ( inputJets.at(i) != _signalJets.at(0) and inputJets.at(i) != _signalJets.at(1)){
      candidates_all.push_back( inputJets.at(i) );
    }
  }
  
  if (candidates_forward.size()<1 or candidates_all.size() < 1) return 0;

  for (unsigned int i(0); i < candidates_forward.size(); i++) {
    for (unsigned int j(0); j < candidates_all.size(); j++) {
      TLorentzVector jet1(candidates_forward.at(i)->p4());
      TLorentzVector jet2(candidates_all.at(j)->p4());
      if (candidates_forward.at(i) == candidates_all.at(j)) continue;
      //if(jet1.Pt()+jet2.Pt() < maxMjj) continue;
      //maxMjj = jet1.Pt()+jet2.Pt();
      if((jet1.Pt()+jet2.Pt()) < maxMjj) continue;
      maxMjj = (jet1.Pt()+jet2.Pt());
      vbfjet1 = candidates_forward.at(i);
      vbfjet2 = candidates_all.at(j);
    }
  }

  if (vbfjet1 == nullptr) return 0;
  if (vbfjet2 == nullptr) return 0;
  vbfjet1->auxdecor<float>("randfactor") = 1;
  vbfjet2->auxdecor<float>("randfactor") = 1;

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );
  
  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_1forAlt3cen(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info)
{
  double btagweight_lead =0;
  double btagweight_sub =0;
  if (WP == "85"){
    btagweight_lead = m_b_85_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "77" || WP == "DEFAULT"){
    btagweight_lead = m_b_77_weight;
    btagweight_sub = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_70_weight;
  }
  if (WP == "asym" or WP == "overlap"){
    btagweight_lead = m_b_77_weight;
    //btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "truth"){
    btagweight_lead = -1;
    btagweight_sub = -1;
  }
  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber>340453) passesHLT = Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split.get(info);
  if (runNumber<=340453) passesHLT = Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split.get(info);
  if (runNumber<=311481) passesHLT = true;
  if (btagweight_lead>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0;

  if (Props::embZpt.exists(info)) {
    //if (Props::embZm.get(info)<81e3 || Props::embZm.get(info)>101e3) return 0;
    //if (Props::embZm.get(info)<66e3 || Props::embZm.get(info)>116e3) return 0;
    if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;
  }

  std::vector<xAOD::Jet const*> candidates_leading_b;
  std::vector<xAOD::Jet const*> candidates_sub_b;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()>70e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
      candidates_leading_b.push_back( inputJets.at(i) );
    }
    if (inputJets.at(i)->pt()>50e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
      candidates_sub_b.push_back( inputJets.at(i) );
    }
  }
  
  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;
  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);
  for (unsigned int i(0); i < candidates_leading_b.size(); i++) {
      for (unsigned int j(0); j < candidates_sub_b.size(); j++) {
	  if (candidates_leading_b.at(i) == candidates_sub_b.at(j)) continue; // no same jet
	  //if (m_runNumber<=311481 && Props::HLTBJetMatched_J80_MV2c20_70.get(candidates_leading_b.at(i)) == Props::HLTBJetMatched_J60_MV2c20_85.get(candidates_sub_b.at(j)))// not matched to same HLT jet
	    //continue;
	  //if (m_runNumber>311481 && Props::HLTBJetMatched_J80_MV2c10_70.get(candidates_leading_b.at(i)) == Props::HLTBJetMatched_J60_MV2c10_85.get(candidates_sub_b.at(j)))// not matched to same HLT jet
	  //  continue;
	  TLorentzVector HiggsVect = candidates_leading_b.at(i)->p4() + candidates_sub_b.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = candidates_leading_b.at(i);
	    _signalJets.at(1) = candidates_sub_b.at(j);
	  }
	}
    }

  if ( _signalJets.at(0) == nullptr ) return 0;

  std::vector<xAOD::Jet const*> candidates_forward;
  std::vector<xAOD::Jet const*> candidates_all;

  double maxMjj(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  // forward jets
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if( fabs(inputJets.at(i)->eta())>3.2 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>60e3){
      candidates_forward.push_back(inputJets.at(i));
    }
  }

  // not signal candidates
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()<90e3) continue;
    if (fabs(inputJets.at(i)->eta())>2.4) continue;
    if ( WP == "overlap"){
      candidates_all.push_back( inputJets.at(i) );
    }
    else if ( inputJets.at(i) != _signalJets.at(0) and inputJets.at(i) != _signalJets.at(1)){
      candidates_all.push_back( inputJets.at(i) );
    }
  }
  
  if (candidates_forward.size()<1 or candidates_all.size() < 1) return 0;

  for (unsigned int i(0); i < candidates_forward.size(); i++) {
      for (unsigned int j(0); j < candidates_all.size(); j++) {
	  TLorentzVector jet1(candidates_forward.at(i)->p4());
	  TLorentzVector jet2(candidates_all.at(j)->p4());
	  if (candidates_forward.at(i) == candidates_all.at(j)) continue;
	  if((jet1+jet2).M() > maxMjj){
	    maxMjj = (jet1+jet2).M();
	    vbfjet1 = candidates_forward.at(i);
	    vbfjet2 = candidates_all.at(j);
	  }
	}
    }

  if (vbfjet1 == nullptr) return 0;
  if (vbfjet2 == nullptr) return 0;

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );
  
  if (_debugMode) this->printResults();
  return 10;
}

int jetAssignmentTool::assignBtag_1for3cen(std::vector<xAOD::Jet const*> &inputJets, std::string WP, const xAOD::EventInfo* info)
{
  double btagweight_lead =0;
  double btagweight_sub =0;
  if (WP == "85"){
    btagweight_lead = m_b_85_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "77" || WP == "DEFAULT"){
    btagweight_lead = m_b_77_weight;
    btagweight_sub = m_b_77_weight;
  }
  if (WP == "70"){
    btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_70_weight;
  }
  if (WP == "asym" or WP == "overlap"){
    btagweight_lead = m_b_77_weight;
    //btagweight_lead = m_b_70_weight;
    btagweight_sub = m_b_85_weight;
  }
  if (WP == "truth"){
    btagweight_lead = -1;
    btagweight_sub = -1;
  }
  bool passesHLT = false;
  UInt_t runNumber = (m_isMC?info->auxdata<UInt_t>("RandomRunNumber"):info->runNumber());
  if (runNumber>340453) passesHLT = Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split.get(info);
  if (runNumber<=340453) passesHLT = Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split.get(info);
  if (runNumber<=311481 && runNumber>=308084) passesHLT = Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split.get(info);
  if (btagweight_lead>-1 && !passesHLT && !Props::embZpt.exists(info)) return 0;
  if (runNumber<308084) return 0;

  if (Props::embZpt.exists(info)) {
    //if (Props::embZm.get(info)<81e3 || Props::embZm.get(info)>101e3) return 0;
    //if (Props::embZm.get(info)<66e3 || Props::embZm.get(info)>116e3) return 0;
    if (!Props::passHLT_2mu14.get(info) && !Props::passHLT_mu50.get(info)) return 0;
  }

  std::vector<xAOD::Jet const*> candidates_leading_b;
  std::vector<xAOD::Jet const*> candidates_sub_b;

  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()>90e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_lead<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_lead))  {
      candidates_leading_b.push_back( inputJets.at(i) );
    }
    if (inputJets.at(i)->pt()>50e3 && fabs(inputJets.at(i)->eta())<2.5 && (btagweight_sub<0 ? Props::HadronConeExclTruthLabelID.get(inputJets.at(i))==5 : Props::MV2c10.get(inputJets.at(i))>btagweight_sub))  {
      candidates_sub_b.push_back( inputJets.at(i) );
    }
  }
  
  // Choose signal jets to be the pair with highest pT passing tight btagging WP  
  double MaxPt = -1;
  _signalJets.push_back(nullptr);
  _signalJets.push_back(nullptr);
  for (unsigned int i(0); i < candidates_leading_b.size(); i++) {
      for (unsigned int j(0); j < candidates_sub_b.size(); j++) {
	  if (candidates_leading_b.at(i) == candidates_sub_b.at(j)) continue; // no same jet
	  //if (m_runNumber<=311481 && Props::HLTBJetMatched_J80_MV2c20_70.get(candidates_leading_b.at(i)) == Props::HLTBJetMatched_J60_MV2c20_85.get(candidates_sub_b.at(j)))// not matched to same HLT jet
	    //continue;
	  //if (m_runNumber>311481 && Props::HLTBJetMatched_J80_MV2c10_70.get(candidates_leading_b.at(i)) == Props::HLTBJetMatched_J60_MV2c10_85.get(candidates_sub_b.at(j)))// not matched to same HLT jet
	  //  continue;
	  TLorentzVector HiggsVect = candidates_leading_b.at(i)->p4() + candidates_sub_b.at(j)->p4();
	  if ( HiggsVect.Pt() > MaxPt ) {
	    MaxPt = HiggsVect.Pt();
	    _signalJets.at(0) = candidates_leading_b.at(i);
	    _signalJets.at(1) = candidates_sub_b.at(j);
	  }
	}
    }

  if ( _signalJets.at(0) == nullptr ) return 0;

  std::vector<xAOD::Jet const*> candidates_forward;
  std::vector<xAOD::Jet const*> candidates_all;

  double maxMjj(-1.);
  xAOD::Jet const* vbfjet1 = NULL;
  xAOD::Jet const* vbfjet2 = NULL;

  // forward jets
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if( fabs(inputJets.at(i)->eta())>3.2 && fabs(inputJets.at(i)->eta())<4.5 && inputJets.at(i)->pt()>60e3){
      candidates_forward.push_back(inputJets.at(i));
    }
  }

  // not signal candidates
  for (unsigned int i(0); i < inputJets.size(); i++) {
    if (inputJets.at(i)->pt()<70e3) continue;
    if (fabs(inputJets.at(i)->eta())>2.8) continue;
    if ( WP == "overlap"){
      candidates_all.push_back( inputJets.at(i) );
    }
    else if ( inputJets.at(i) != _signalJets.at(0) and inputJets.at(i) != _signalJets.at(1)){
      candidates_all.push_back( inputJets.at(i) );
    }
  }
  
  if (candidates_forward.size()<1 or candidates_all.size() < 1) return 0;

  for (unsigned int i(0); i < candidates_forward.size(); i++) {
      for (unsigned int j(0); j < candidates_all.size(); j++) {
	  TLorentzVector jet1(candidates_forward.at(i)->p4());
	  TLorentzVector jet2(candidates_all.at(j)->p4());
	  if (candidates_forward.at(i) == candidates_all.at(j)) continue;
	  if((jet1+jet2).M() > maxMjj){
	    maxMjj = (jet1+jet2).M();
	    vbfjet1 = candidates_forward.at(i);
	    vbfjet2 = candidates_all.at(j);
	  }
	}
    }

  if (vbfjet1 == nullptr) return 0;
  if (vbfjet2 == nullptr) return 0;

  _vbfJets.push_back( vbfjet1 );
  _vbfJets.push_back( vbfjet2 );
  
  if (_debugMode) this->printResults();
  return 10;
}

const std::string jetAssignmentTool::assignmentType() const { return _type; }
std::vector<xAOD::Jet const*> jetAssignmentTool::signalJets() const { return _signalJets; }
std::vector<xAOD::Jet const*> jetAssignmentTool::vbfJets() const { return _vbfJets; }



/* Utilities */

bool JetSort::sortJetsByEta(xAOD::Jet const* jet1,xAOD::Jet const* jet2)
{
  return (jet1->eta() > jet2->eta());
}

bool JetSort::sortJetsByBtag(xAOD::Jet const* jet1, xAOD::Jet const* jet2)
{
  return (Props::MV2c10.get(jet1) > Props::MV2c10.get(jet2));
}

bool JetSort::sortJetsByPt(xAOD::Jet const* jet1, xAOD::Jet const* jet2)
{
  return (jet1->pt() > jet2->pt());
}
