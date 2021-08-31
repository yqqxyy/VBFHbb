#include "PATInterfaces/ISystematicsTool.h" 
#include "EventLoop/StatusCode.h"
#include "MuonSelectorTools/MuonSelectionTool.h"

#include "CxAODMaker/JetSemileptonic.h"
#include "CxAODTools/CommonProperties.h"
#include "CxAODTools/ReturnCheck.h"

#include "PathResolver/PathResolver.h" 



JetSemileptonic::JetSemileptonic():
  m_muonSelectionTool(nullptr),
  m_debug(false),
  m_msgLevel(MSG::WARNING)
{
}

JetSemileptonic::~JetSemileptonic() {
  delete m_muonSelectionTool;
}

//this initializes the tmva tools
EL::StatusCode JetSemileptonic::initialize(bool debug) {

  m_debug=debug;
  if (m_debug) {
    m_msgLevel = MSG::DEBUG;
  } else {
    m_msgLevel = MSG::INFO;
  }
  
  // initialize the muon selector tool
  //---------------------------------------------------
  m_muonSelectionTool = new CP::MuonSelectionTool("MuonSelectionToolForJetSemileptonic");
  m_muonSelectionTool->msg().setLevel( m_msgLevel );
  TOOL_CHECK("MuonHandler::initializeTools()",m_muonSelectionTool->setProperty("MaxEta", 2.5));
  // Setting the tool accept flag to Loose, these muons will be used for sys calibration
  // Possible values: Tight, Medium, Loose, VeryLoose
  TOOL_CHECK("MuonHandler::initializeTools()",m_muonSelectionTool->setProperty("MuQuality", (int) xAOD::Muon::Quality::Medium));
  TOOL_CHECK("MuonHandler::initializeTools()",m_muonSelectionTool->initialize());
  return EL::StatusCode::SUCCESS;
}

bool JetSemileptonic::passForJetElectron(const xAOD::Electron* electron)
{
  bool passSel = true;
  if (!Props::isLooseLH.get(electron)) passSel = false;
  if(electron->caloCluster()){ 
    if (!(fabs(electron->caloCluster()->etaBE(2)) < 2.47)) passSel = false; //do we want to exclude the crack region?
  }
  else{
    if (!(fabs(electron->eta()) < 2.47)) passSel = false; //do we want to exclude the crack region?
    Warning("JetSemileptonic::passForJetElectron","Did not find caloCluster, use eta() instead of caloCluster()->etaBE(2) to check eta range!");
  }
  if (!(electron->pt() > 4000.)) passSel = false;
  if (!(electron->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON))) passSel = false;
  return passSel;
}

bool JetSemileptonic::passForJetMuon(const xAOD::Muon* muon)
{
  bool passSel = true;
  bool accepted = static_cast<bool>(m_muonSelectionTool->accept(*muon));
  if (!(accepted)) passSel = false;
  if (!(muon->pt() > 5000.)) passSel = false;
  return passSel;
}

TLorentzVector JetSemileptonic::getELossTLV(const xAOD::Muon* muon)
{
  float eLoss=0.0;
  muon->parameter(eLoss,xAOD::Muon::EnergyLoss);
  TLorentzVector m=muon->p4();
  double theta=m.Theta();
  double phi=m.Phi();
  double eLossX=eLoss*sin(theta)*cos(phi);
  double eLossY=eLoss*sin(theta)*sin(phi);
  double eLossZ=eLoss*cos(theta);
  return TLorentzVector(eLossX,eLossY,eLossZ,eLoss);
}


EL::StatusCode JetSemileptonic::countElectronInJet(xAOD::Jet* jet, const TString& /*variation*/, const xAOD::ElectronContainer* electrons) {
  unsigned counter04=0;
  unsigned counter=0;
  unsigned index_smallestDR=99;
  double smallestDR=99.9;
  TLorentzVector one;
  // loop over all the electrons
  for (unsigned i=0; i!=electrons->size(); i++) {
    const xAOD::Electron* electron = (*electrons)[i];
    bool good=passForJetElectron(electron);
    if(!good) continue;
    // electron must be within 0.4 of the jet axis                                                                                                                             
    double DR=jet->p4().DeltaR(electron->p4());
    //apply the a DRCut changing with electron pT, just as in the Overlap Removal procedure                                                                                    
    double DRCut=std::min(0.4,0.04+10000.0/electron->pt());
    //if DR>DRCut (but smaller than 0.4), then the electron has deposited energy in the jet               
    //so remove it from the jet (though some calorimeter part would be more correct)
    if(!(DR<DRCut)) continue;
    counter++;
    if(DR<smallestDR)
      {
	index_smallestDR=i;
	smallestDR=DR;
      }
  }//end loop over electrons 

  if (counter>0) {
    const xAOD::Electron* electron = (*electrons)[index_smallestDR];
    one=electron->p4();
  }

  Props::nrElectronInJet04.set(jet,counter04);
  Props::nrElectronInJet.set(jet,counter);
  //Props::PtRatioOneElectronInJet.set(jet,one.Pt()/jet->pt());
  //Props::ERatioOneElectronInJet.set(jet,one.E()/jet->e());

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetSemileptonic::correctMuonInJet(xAOD::Jet* jet, const xAOD::MuonContainer* muons) {

  TLorentzVector j=jet->p4();
  unsigned counter=0;
  unsigned index_smallestDR=99;
  double smallestDR=99.9;
  TLorentzVector one;
  TLorentzVector one_eLoss;
  TLorentzVector ptreco;
  
  // loop over all the muons                                                                                                                                                                                                                                   
  for (unsigned i=0; i!=muons->size(); i++) {
    const xAOD::Muon* muon = (*muons)[i];
    bool good=passForJetMuon(muon);
    if(!good) continue;
    // muon must be within some DR of the jet axis
    double DR=jet->p4().DeltaR(muon->p4());
    //apply the a DRCut changing with muon pT, just as in the Overlap Removal procedure
    double DRCut=std::min(0.4,0.04+10000.0/muon->pt());
    if(!(DR<DRCut)) continue;
    counter++;
    if(DR<smallestDR)
      {
	index_smallestDR=i;
	smallestDR=DR;
      }
  } // done loop over muons
  if (counter>0) {
    const xAOD::Muon* muon = (*muons)[index_smallestDR];
    one=muon->p4();
    one_eLoss+=getELossTLV(muon);
  }
  Props::nrMuonInJet.set(jet,counter);
  //create the TLorentzVector for the different lepton-in-jet
  std::map<std::string, TLorentzVector> map_name_tlv;
  map_name_tlv["OneMu"]=j-one_eLoss+one;
  // insert the 4-vector of all these lepton-in-jet corrections into the jet
  for(auto const & item : map_name_tlv) {
    EL_CHECK("setJetTLV",setJetTLV(jet,item.first,item.second-j));//subtracting j, which is Nominal, already in MeV
  }

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JetSemileptonic::setJetTLV(xAOD::Jet * jet, const std::string& name, const TLorentzVector& tlv) {
  JetFourMom_t jetVec;
  jetVec.SetPt(tlv.Pt());
  jetVec.SetEta(tlv.Eta());
  jetVec.SetPhi(tlv.Phi());
  jetVec.SetM(tlv.M());
  if (name=="") {
    //change the jet main value
    jet->setJetP4(jetVec); 
  } else {
    //set a new signal state
    jet->setJetP4(name,jetVec);
  }

  //alternatively we can write this as a new state of the signal jet, as explained in the xAOD::Jet, especially for CaloJet
  //http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODJet/xAODJet/versions/Jet_v1.h
  //Signal States : Jets can hold several other 4-momentum representing various calibration states (primaliry used for calo jets). See also P4SignalState
  //void setJetP4(const std::string & sname, const JetFourMom_t & p4);
  //jet->setJetP4("EMJESGSMu",jetVec);
  //to validate this signal state
  //JetFourMom_t jet2=jet->jetP4("EMJESGSMu");
  
  return EL::StatusCode::SUCCESS;
}

