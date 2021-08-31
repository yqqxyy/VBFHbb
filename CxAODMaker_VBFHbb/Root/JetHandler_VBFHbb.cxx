#include <iostream>

// jet specific includes
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "CxAODMaker_VBFHbb/JetHandler_VBFHbb.h"

#include "CxAODTools_VBFHbb/CommonProperties_VBFHbb.h"

#include "TrigBtagEmulationTool/TriggerFeature.h"
#include <TFile.h>

JetHandler_VBFHbb::JetHandler_VBFHbb(const std::string& name, ConfigStore & config, xAOD::TEvent * event,
                       EventInfoHandler & eventInfoHandler) :
  JetHandler(name, config, event, eventInfoHandler),
  m_btagCut(1.)

{
  using std::placeholders::_1;
  m_selectFcns.clear();
  m_selectFcns.push_back(std::bind( &JetHandler_VBFHbb::passVetoJet, this, _1));
  m_selectFcns.push_back(std::bind( &JetHandler_VBFHbb::passSignalJet, this, _1));

  m_config.getif<float>("btagCut", m_btagCut);

}


bool JetHandler_VBFHbb::passVetoJet(xAOD::Jet* jet)
{
  bool passCentral = checkCentralJet(jet,true);
  
  bool passForward = true;
  if(passForward) m_cutflow->count("Forward jet input",200);
  if (!(jet->pt() > 30000.)) passForward = false;
  if (passForward) m_cutflow->count("Forward jet pt",201);
  if (!(fabs(jet->eta()) >= 2.5)) passForward = false;
  if (!(fabs(jet->eta()) < 4.5)) passForward = false;
  if (passForward) m_cutflow->count("Forward jet |eta|",202);

  if (passForward) m_cutflow->count("Forward jet selected",203);
  
  bool passSel = true;
  if (passSel) m_cutflow->count("VetoJet input", 300);

  if (!(Props::goodJet.get(jet))) passSel = false;
  if (passSel) m_cutflow->count("GoodJet");
  if(!(Props::PassJvtMedium.get(jet))) passSel = false;
  
  if (passSel) m_cutflow->count("JVT");
  if (!(passCentral || passForward)) passSel = false;
  if (passSel) m_cutflow->count("CentralOrForwardJet");
  
  Props::isVetoJet.set(jet, passSel);
  Props::isVBFJet.set(jet, passSel && jet->pt()>45000.);
  Props::passPreSel.set(jet, passSel);
  if (passSel) m_cutflow->count("VetoJet selected");

  //parton truth ID
  if(m_isMC){
    Props::PartonTruthLabelID.set(jet, jet->getAttribute<int>("PartonTruthLabelID"));
    Props::isJvtHS.set(jet, jet->getAttribute<char>("isJvtHS"));
    Props::isJvtPU.set(jet, jet->getAttribute<char>("isJvtPU"));
  }

  return passSel;
}

bool JetHandler_VBFHbb::passSignalJet(xAOD::Jet* jet)
{
  
  bool passSel = true;
  if (!(Props::isVetoJet.get(jet))) passSel = false;
  if (passSel) m_cutflow->count("SignalJet input", 400);
  if (!(checkCentralJet(jet,false))) passSel = false;
  if (passSel) m_cutflow->count("IsCentral");
  
  Props::isSignalJet.set(jet, passSel);
  Props::isVBFSignalJet.set(jet, passSel && jet->pt()>30000.);

  if (passSel) m_cutflow->count("SignalJet selected");
  return passSel;
}

bool JetHandler_VBFHbb::checkCentralJet(xAOD::Jet* jet,bool isInCutFlow)
{
  bool passSel = true;
  if (passSel && isInCutFlow ) m_cutflow->count("Central jet input",100);
  if (!(jet->pt() > 20000.)) passSel = false;  
  if (jet->pt() > 20000. && isInCutFlow ) m_cutflow->count("Central jet pt");
  if (!(fabs(jet->eta()) < 2.5)) passSel = false;
  if (fabs(jet->eta()) < 2.5 && isInCutFlow ) m_cutflow->count("Central jet |eta|");

  if (passSel&& isInCutFlow ) m_cutflow->count("Central jet selected");
  
  return passSel;
}

EL::StatusCode JetHandler_VBFHbb::writeCustomVariables(xAOD::Jet* inJet, xAOD::Jet* outJet, bool /*isKinVar*/, bool isWeightVar, const TString& /*sysName*/)   
{
  // trigger match

  BJetTriggerMatch(inJet, "HLT_2j45_bmv2c2070_split_2j45_L14J15");
  BJetTriggerMatch(inJet, "HLT_2J45_bmv2c2070_split_2J45_L14J15");
  BJetTriggerMatch(inJet, "HLT_2j45_bmv2c2070_split_2j45");
  BJetTriggerMatch(inJet, "HLT_2J45_bmv2c2070_split_2J45");
  BJetTriggerMatch(inJet, "HLT_2j35_bmv2c2060_split_2j35_L14J15");

  BJetTriggerMatch(inJet, "HLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15.0ETA25");
  BJetTriggerMatch(inJet, "HLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15.0ETA25");
  BJetTriggerMatch(inJet, "HLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490");
  BJetTriggerMatch(inJet, "HLT_j55_gsc80_bmv2c1060_split_j45_gsc60_bmv2c1085_split_j45_320eta490");
  BJetTriggerMatch(inJet, "HLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490");
  BJetTriggerMatch(inJet, "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j25_gsc45_bmv2c1070_split");
  BJetTriggerMatch(inJet, "HLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490");

  //BJetTriggerMatch(inJet, "HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700");
  //BJetTriggerMatch(inJet, "HLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25");

  if (!m_isEmbedding) JetTriggerMatch(inJet);
  
  if(!isWeightVar){
    Props::JetConstitScaleMomentum_pt.copy(inJet, outJet);
    Props::isVetoJet.copy(inJet, outJet);
    Props::isSignalJet.copy(inJet, outJet);
    Props::isVBFJet.copy(inJet, outJet);
    Props::isVBFSignalJet.copy(inJet, outJet);
    Props::isBTag.copyIfExists(inJet, outJet);
    Props::PartonTruthLabelID.copyIfExists(inJet, outJet);
    Props::HLTBJetMatched_2J45_MV2c20_70.copyIfExists(inJet, outJet);
    Props::HLTBJetMatched_2J35_MV2c20_60.copyIfExists(inJet, outJet);
    Props::HLTBJetMatched_2J45_MV2c10_60.copyIfExists(inJet, outJet);
    Props::HLTBJetMatched_2J45_MV2c10_50.copyIfExists(inJet, outJet);
    Props::HLTBJetMatched_2J55_MV2c10_70.copyIfExists(inJet, outJet);
    Props::HLTBJetMatched_2J55_MV2c10_60.copyIfExists(inJet, outJet);
    Props::HLTJetMatchedPt.copyIfExists(inJet, outJet);
    Props::L1JetMatchedPt.copyIfExists(inJet, outJet);
    Props::L1JetMatchedEta.copyIfExists(inJet, outJet);
    Props::L1JetMatchedPhi.copyIfExists(inJet, outJet);
    Props::HLTBJetMatched_J80_MV2c20_70.copyIfExists(inJet, outJet);
    Props::HLTBJetMatched_J60_MV2c20_85.copyIfExists(inJet, outJet);
    Props::HLTBJetMatched_J80_MV2c10_70.copyIfExists(inJet, outJet);
    Props::HLTBJetMatched_J60_MV2c10_85.copyIfExists(inJet, outJet);
    Props::HLTBJetMatched_2J45_MV2c10_70.copyIfExists(inJet, outJet);
    Props::QGTagger.copyIfExists(inJet, outJet);
    Props::QGTaggerWeight.copyIfExists(inJet, outJet);
    Props::PassFJvtLoose.copyIfExists(inJet, outJet);
    Props::PassFJvtTight.copyIfExists(inJet, outJet);
    Props::Jvt.copyIfExists(inJet, outJet);
    Props::isJvtHS.copyIfExists(inJet, outJet);
    Props::isJvtPU.copyIfExists(inJet, outJet);
    
  }
  
  return EL::StatusCode::SUCCESS;
 }

void JetHandler_VBFHbb::JetTriggerMatch(xAOD::Jet * recoJet){
  const xAOD::JetContainer* jet_trig_HLT = 0;
  if (!m_event->retrieve(jet_trig_HLT,"HLT_xAOD__JetContainer_a4tcemsubjesISFS").isSuccess()) {
     Error("execute()", "Failed to retrieve HLT_xAOD__JetContainer_a4tcemsubjesISFS");
     return;
  }
  
  double mindR_any = 1000;
  float IndexAnyPt = 0;
  float IndexAnyEta = 0;
  float IndexAnyPhi = 0;
  xAOD::JetContainer::const_iterator jet_trigItr_HLT;
  for(jet_trigItr_HLT = jet_trig_HLT->begin();  jet_trigItr_HLT!= jet_trig_HLT->end(); ++jet_trigItr_HLT){
    const xAOD::Jet *hlt_jet = *jet_trigItr_HLT;
    float dR = hlt_jet->p4().DeltaR(recoJet->p4());
    if (dR<mindR_any and dR<0.4){
      mindR_any = dR;
      IndexAnyPt =  hlt_jet->pt();
      IndexAnyEta =  hlt_jet->eta();
      IndexAnyPhi =  hlt_jet->phi();
    }
  }
  Props::HLTJetMatchedPt.set(recoJet, IndexAnyPt);

  if (m_isEmbedding) return;

  const xAOD::JetRoIContainer* l1jets = 0;
  if ( ! m_event->retrieve(l1jets, "LVL1JetRoIs").isSuccess() ) {
    Error("EventInfoHandler_VBFHbb::executeEvent()", "TriggerEmulation: Failed to retrieve L1 Jet collection. Exiting.");
    return;
  } 

  xAOD::JetRoIContainer::const_iterator l1sg, l1sgEnd = l1jets->end();
  TLorentzVector closestL1;
  for(l1sg = l1jets->begin(); l1sg!=l1sgEnd; l1sg++) {
    int nsame = 0;
    xAOD::JetRoIContainer::const_iterator l1, l1End = l1jets->end();
    TLorentzVector tlv1;
    tlv1.SetPtEtaPhiM((*l1sg)->et8x8(),(*l1sg)->eta(),(*l1sg)->phi(),0);
    if (fabs(tlv1.Eta())<3.1) continue;
    if (tlv1.Eta()*recoJet->eta()<0) continue;
    if (closestL1.Pt()>0 && fabs(closestL1.DeltaPhi(recoJet->p4()))<fabs(tlv1.DeltaPhi(recoJet->p4()))) continue;
    closestL1 = tlv1;
  }
  Props::L1JetMatchedPt.set(recoJet, closestL1.Pt());
  Props::L1JetMatchedEta.set(recoJet, closestL1.Eta());
  Props::L1JetMatchedPhi.set(recoJet, closestL1.Phi());
}

void JetHandler_VBFHbb::BJetTriggerMatch(xAOD::Jet * recoJet, std::string trig_type){

  double mindR_pT35_tight = 1000;
  double mindR_pT45_medium = 1000;
  double mindR_pT80_medium = 1000;
  double mindR_pT60_loose = 1000;
  double mindR_pT45_mv2c1060 = 1000;
  double mindR_pT45_mv2c1050 = 1000;
  double mindR_pT55_mv2c1070 = 1000;
  double mindR_pT45_mv2c1070 = 1000;
  double mindR_pT55_mv2c1060 = 1000;
  double mindR_pT80_mv2c1070 = 1000;
  double mindR_pT60_mv2c1085 = 1000;

  int IndexpT35_tight = -1;
  int IndexpT45_medium = -1;
  int IndexpT45_mv2c1060 = -1;
  int IndexpT45_mv2c1050 = -1;
  int IndexpT55_mv2c1070 = -1;
  int IndexpT45_mv2c1070 = -1;
  int IndexpT55_mv2c1060 = -1;
  int IndexpT80_medium = -1;
  int IndexpT60_loose = -1;
  int IndexpT80_mv2c1070 = -1;
  int IndexpT60_mv2c1085 = -1;

  int iHLTJet = -1;
  
  //  Make accessors/decorators
  static SG::AuxElement::Decorator< const xAOD::BTagging* > hltBTagDecor( "HLTBTag" );

  Trig::FeatureContainer fc;
  fc = m_trigDecTool->features(trig_type);

  Trig::FeatureContainer::combination_const_iterator comb   (fc.getCombinations().begin());
  Trig::FeatureContainer::combination_const_iterator combEnd(fc.getCombinations().end());

  for( ; comb!=combEnd ; ++comb) {
    std::vector< Trig::Feature<xAOD::JetContainer> >            jetCollections  = comb->containerFeature<xAOD::JetContainer>(trig_type.find("gsc")!=std::string::npos?"GSCJet":"SplitJet");
    std::vector< Trig::Feature<xAOD::JetContainer> >            EFjetCollections  = comb->containerFeature<xAOD::JetContainer>("EFJet");
    std::vector< Trig::Feature<xAOD::BTaggingContainer> >       bjetCollections = comb->containerFeature<xAOD::BTaggingContainer>("HLTBjetFex");
    
    if(jetCollections.size() != bjetCollections.size()){
      std::cout << "ERROR Problem in container size: " << " jets: "<< jetCollections.size() << " bjets: "<< bjetCollections.size() << std::endl;
      continue;
    }

    for ( unsigned ifeat=0 ; ifeat<jetCollections.size() ; ifeat++ ) {
      const xAOD::Jet* hlt_jet = getTrigObject<xAOD::Jet, xAOD::JetContainer>(jetCollections.at(ifeat));
      if(!hlt_jet) continue;
      
      const xAOD::BTagging* hlt_btag = getTrigObject<xAOD::BTagging, xAOD::BTaggingContainer>(bjetCollections.at(ifeat));
       
      if(!hlt_btag) continue;
      
      xAOD::Jet* newHLTBJet = new xAOD::Jet();
      newHLTBJet->makePrivateStore( hlt_jet );
      
      // Add Link to BTagging Info
      newHLTBJet->auxdecor< const xAOD::BTagging* >("HLTBTag") = hlt_btag;
      const xAOD::BTagging * tagInfo = newHLTBJet->auxdata< const xAOD::BTagging* >("HLTBTag");

      double hlt_bscore_mv2c20;
      double hlt_bscore_mv2c10;
      if(tagInfo) {
	tagInfo->MVx_discriminant("MV2c20",hlt_bscore_mv2c20);
	tagInfo->MVx_discriminant("MV2c10",hlt_bscore_mv2c10);
      }

      double dR = newHLTBJet->p4().DeltaR(recoJet->p4());
      iHLTJet ++;

      /// 2J35 60%
      if ( newHLTBJet->pt() > 35000 and hlt_bscore_mv2c20 > Trig::TriggerFeatureBtag::MV2C2060) {
	if (dR<mindR_pT35_tight and dR<0.4){
	  mindR_pT35_tight = dR;
	  IndexpT35_tight =  iHLTJet;
	}
      }

      /// 2J45 70%
      if ( newHLTBJet->pt() > 45000 and hlt_bscore_mv2c20 > Trig::TriggerFeatureBtag::MV2C2070) {
	if (dR<mindR_pT45_medium and dR<0.4){
	  mindR_pT45_medium = dR;
	  IndexpT45_medium = iHLTJet;
	}
      }

      if ( newHLTBJet->pt() > 45000 and hlt_bscore_mv2c10 > Trig::TriggerFeatureBtag::MV2C1070) {
	if (dR<mindR_pT45_mv2c1070 and dR<0.4){
	  mindR_pT45_mv2c1070 = dR;
	  IndexpT45_mv2c1070 = iHLTJet;
	}
      }

      if ( newHLTBJet->pt() > 45000 and hlt_bscore_mv2c10 > Trig::TriggerFeatureBtag::MV2C1060) {
	if (dR<mindR_pT45_mv2c1060 and dR<0.4){
	  mindR_pT45_mv2c1060 = dR;
	  IndexpT45_mv2c1060 = iHLTJet;
	}
      }

      if ( newHLTBJet->pt() > 45000 and hlt_bscore_mv2c10 > Trig::TriggerFeatureBtag::MV2C1050) {
	if (dR<mindR_pT45_mv2c1050 and dR<0.4){
	  mindR_pT45_mv2c1050 = dR;
	  IndexpT45_mv2c1050 = iHLTJet;
	}
      }

      if ( newHLTBJet->pt() > 55000 and hlt_bscore_mv2c10 > Trig::TriggerFeatureBtag::MV2C1070) {
	if (dR<mindR_pT55_mv2c1070 and dR<0.4){
	  mindR_pT55_mv2c1070 = dR;
	  IndexpT55_mv2c1070 = iHLTJet;
	}
      }

      if ( newHLTBJet->pt() > 55000 and hlt_bscore_mv2c10 > Trig::TriggerFeatureBtag::MV2C1060) {
	if (dR<mindR_pT55_mv2c1060 and dR<0.4){
	  mindR_pT55_mv2c1060 = dR;
	  IndexpT55_mv2c1060 = iHLTJet;
	}
      }

      /// J80 70%
      if ( newHLTBJet->pt() > 80000 and hlt_bscore_mv2c20 > Trig::TriggerFeatureBtag::MV2C2070) {
	if (dR<mindR_pT80_medium and dR<0.4){
	  mindR_pT80_medium = dR;
	  IndexpT80_medium= iHLTJet;
	}
      }

      /// J60 85%
      if ( newHLTBJet->pt() > 60000 and hlt_bscore_mv2c20 > Trig::TriggerFeatureBtag::MV2C2085) {
	if (dR<mindR_pT60_loose and dR<0.4){
	  mindR_pT60_loose = dR;
	  IndexpT60_loose= iHLTJet;
	}
      }

      /// J80 70%
      if ( newHLTBJet->pt() > 80000 and hlt_bscore_mv2c10 > Trig::TriggerFeatureBtag::MV2C1070) {
	if (dR<mindR_pT80_mv2c1070 and dR<0.4){
	  mindR_pT80_mv2c1070 = dR;
	  IndexpT80_mv2c1070= iHLTJet;
	}
      }

      /// J60 85%
      if ( newHLTBJet->pt() > 60000 and hlt_bscore_mv2c10 > Trig::TriggerFeatureBtag::MV2C1085) {
	if (dR<mindR_pT60_mv2c1085 and dR<0.4){
	  mindR_pT60_mv2c1085 = dR;
	  IndexpT60_mv2c1085 = iHLTJet;
	}
      }
      delete newHLTBJet;
    }//feature
  }// Combinations

  if (trig_type == "HLT_2j45_bmv2c2070_split_2j45_L14J15"){
    Props::HLTBJetMatched_2J45_MV2c20_70.set(recoJet, IndexpT45_medium);
  }
  if (trig_type == "HLT_2j35_bmv2c2060_split_2j35_L14J15"){
    Props::HLTBJetMatched_2J35_MV2c20_60.set(recoJet, IndexpT35_tight);
  }
  if (trig_type == "HLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15.0ETA25"){
    Props::HLTBJetMatched_2J45_MV2c10_60.set(recoJet, IndexpT45_mv2c1060);
  }
  if (trig_type == "HLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15.0ETA25"){
    Props::HLTBJetMatched_2J45_MV2c10_50.set(recoJet, IndexpT45_mv2c1050);
  }
  if (trig_type == "HLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15.0ETA25"){
    Props::HLTBJetMatched_2J55_MV2c10_70.set(recoJet, IndexpT55_mv2c1070);
  }
  if (trig_type == "HLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15.0ETA25"){
    Props::HLTBJetMatched_2J55_MV2c10_60.set(recoJet, IndexpT55_mv2c1060);
  }
  if (trig_type == "HLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490"){
    Props::HLTBJetMatched_J80_MV2c20_70.set(recoJet, IndexpT80_medium);
    Props::HLTBJetMatched_J60_MV2c20_85.set(recoJet, IndexpT60_loose);
  }
  if (trig_type == "HLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490"){
    Props::HLTBJetMatched_J80_MV2c10_70.set(recoJet, IndexpT80_mv2c1070);
    Props::HLTBJetMatched_J60_MV2c10_85.set(recoJet, IndexpT60_mv2c1085);
  }
  if (trig_type == "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j25_gsc45_bmv2c1070_split"){
    Props::HLTBJetMatched_2J45_MV2c10_70.set(recoJet, IndexpT45_mv2c1070);
  }
}

EL::StatusCode JetHandler_VBFHbb::decorateOriginParticles(xAOD::JetContainer * jets) {
  const xAOD::MuonContainer * embmuCont = 0;
  if (m_isEmbedding) {
    if (!m_event->retrieve(embmuCont,"MuonsEmb").isSuccess()) {
      Error("execute()", "Failed to retrieve MuonsEmb");
      return EL::StatusCode::FAILURE;
    }

    for (const auto& muon : *embmuCont) {
      float closest = 0.4;
      xAOD::Jet *closeJet = NULL;
      for (xAOD::Jet * jet : *jets) {
        if (jet->p4().DeltaR(muon->p4())>closest) continue;
        closest = jet->p4().DeltaR(muon->p4());
        closeJet = jet;
      }
      float energyloss = (muon->auxdata<float>("EnergyLoss")/cosh(muon->eta()));
      if (!closeJet) continue;
      float scalefactor = (closeJet->pt()-energyloss)/closeJet->pt();
      if (scalefactor<0) scalefactor = 0;
      xAOD::JetFourMom_t newfour;
      newfour.SetPxPyPzE(closeJet->jetP4("JetConstitScaleMomentum").Px()*scalefactor,closeJet->jetP4("JetConstitScaleMomentum").Py()*scalefactor,closeJet->jetP4("JetConstitScaleMomentum").Pz()*scalefactor,closeJet->jetP4("JetConstitScaleMomentum").E()*scalefactor);
      //closeJet->setJetP4("JetConstitScaleMomentum",newfour);
    }
  }

  return JetHandler::decorateOriginParticles(jets);
}
