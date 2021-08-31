#ifndef CxAODReader_AnalysisReader_VBFHbb_H
#define CxAODReader_AnalysisReader_VBFHbb_H

#include "CxAODReader/AnalysisReader.h"
#include "CxAODReader/EasyTree.h"
#include "CxAODReader_VBFHbb/MVATree_VBFHbb.h"

#include "CxAODTools_VBFHbb/VBFHbbInclEvtSelection.h"
#include "CxAODReader/Histo.h"
#include "CxAODReader/HistFastSvc.h"

#include "KinematicFit/KinematicFit.h"

#include "CxAODTools_VBFHbb/VBFHbbJetAssignment.h"
#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"


class TriggerTool;

PROPERTY( Props, int, nBTagsAll) //number of b-tagged track jets in leading fat jet

// for 0lepton boosted analysis
namespace AnalysisCutflow {
   enum {
     preselection = 0,
     trigger,
     leptons,
     tauVeto,
     MET,
     MPT,
     Ngoodfatjet,
     mindPhi,
     dPhiMETfatjet,
     dPhiMETMPT,
     mbbwindow,
     trackjetsmatch,
     MET500,
     cat_0tag_1pfat0pjet,
     cat_1tag_1pfat0pjet,
     cat_2tag_1pfat0pjet,
      cat_3ptag_1pfat0pjet
   };
}

namespace AnalysisRegions {
   enum {
      mbbSR = 0,
      mbbCRlow,
      mbbCRhigh,
      mbbInclusive
   };
}

struct PhysicsMetadata {
  enum class Analysis { undefined, cutbased, MVA } analysis = Analysis::undefined;
  enum class Channel { undefined, TwoLep, OneLep, ZeroLep } channel = Channel::undefined;
  enum class Flavor { undefined, MuMu, ElEl, MuEl, ElMu, Mu, El } flavor = Flavor::undefined;
  enum class Regime { undefined, SMVH, resolved, merged } regime = Regime::undefined;
  enum class Region { undefined, SR, topCR, mbbCR } region = Region::undefined;
  enum class MbbSideBandResolved { undefined, Outer, Low, High } mbbSideBandResolved = MbbSideBandResolved::undefined; 
  enum class MbbSideBandMerged { undefined, Outer, Low, High } mbbSideBandMerged = MbbSideBandMerged::undefined; 
  int        nJets = 0;
  int        nSigJet = 0;
  int        nForwardJet = 0;
  int        nTags = 0;
  int        b1Flav = 0;
  int        b2Flav = 0;
  int        nAddBTrkJets = 0;
};

class AnalysisReader_VBFHbb : public AnalysisReader {
protected:

  virtual EL::StatusCode initializeSelection () override;
  virtual EL::StatusCode initializeCorrsAndSysts ();
  virtual EL::StatusCode initializeTools () override;
  virtual EL::StatusCode initializeSumOfWeights() override;

  virtual EL::StatusCode setObjectsForOR (const xAOD::ElectronContainer *electrons,
                                          const xAOD::PhotonContainer   *photons,
                                          const xAOD::MuonContainer     *muons,
                                          const xAOD::TauJetContainer   *taus,
                                          const xAOD::JetContainer      *jets,
                                          const xAOD::JetContainer      *fatJets); // override;

  EL::StatusCode fill_bTaggerHists (const xAOD::Jet *jet);
  EL::StatusCode fill_nJetHistos (const std::vector<const xAOD::Jet*> &jets,
                                  const string                  &jetType);

  EL::StatusCode fill_VBF ();

  EL::StatusCode fill_VBFCutFlow (std::string label);
  float reducedSF(TH2D *hist,float eta,float pt);
  int getSTXS();

  void     compute_btagging ();
  float    computeBTagSFWeightPerJet (const std::vector<const xAOD::Jet*> &signalJets, const std::string &authorName , TString WP);
  //  double      compute_truthTagging (const std::vector<const xAOD::Jet*> &signalJets, std::string tagStrategy = "AllSignalJets");
  void        setevent_nJets   (const std::vector<const xAOD::Jet*> &signalJets,
                                const std::vector<const xAOD::Jet*> &forwardJets);
  
  bool jet_selection_vbf (
    std::vector<const xAOD::Jet*>  inputJets,
    std::vector<const xAOD::Jet*> &signalJets,
    std::vector<const xAOD::Jet*> &vbfJets);

  EL::StatusCode save_meta_info (std::vector<const xAOD::Jet*>  const& Jets,
				 std::vector<const xAOD::Jet*>  const& signalJets);
  EL::StatusCode save_trigger_info ();
  EL::StatusCode save_jet_info (
				std::vector<const xAOD::Jet*>  const& inputJets,
				std::vector<const xAOD::Jet*>  const& signalJets,
				std::vector<const xAOD::Jet*>  const& vbfJets,
				std::vector<const xAOD::Jet*>  const& trackJets);

  // TAKEN FROM VBF+GAMMA
  // needed to exclude trackJets in region between b-jets when computing HT soft  //
                                                                                     
  std::pair<std::pair<float, float>, float>getCenterSlopeOfEllipse(const xAOD::Jet* \
								   jet1, const xAOD::Jet* jet2);
  bool trackJetIsInEllipse(std::pair<std::pair<float, float>, float> El,const xAOD::\
			   Jet* jet, float dRBB, float r);


  // a function to check whether an event passes a specific set of cuts
  bool passSpecificCuts(const unsigned long int flag, const std::vector<unsigned int> &cuts);
  
  void checkMbbWindow(const TLorentzVector &HVec, const TLorentzVector &Hvec_merged, bool &passMHResolved, bool &passMHMerged);

  EL::StatusCode fill_jetHistos (std::vector<const xAOD::Jet*> jets, std::vector<const xAOD::Jet*> signalJets, std::vector<const xAOD::Jet*> forwardJets);
  
  BTaggingTool *m_bTagTool_Loose;  // !
  BTaggingTool *m_bTagTool_Medium;  // !

  std::string m_analysisType;           // !
  std::string m_analysisStrategy;       // !
  std::string m_selection;           // !
  bool m_doTruthTagging;                // !
  bool m_doMbbWindow         = true;    // !
  bool m_doFillCR            = false;   // !
  bool m_doOnlyInputs        = false;   // !
  bool m_doReduceFillHistos  = false;   // !
  MVATree_VBFHbb   *m_tree     = nullptr; // !
  MVATree_VBFHbb *m_tree_vbf = nullptr; // !
  EasyTree       *m_etree    = nullptr; // !
  PhysicsMetadata m_physicsMeta;  // !
  bool m_doBlinding          = false;   //!
  bool m_doMergeJetBins      = false;   //!
  std::string m_jetCorrType;  //!
  std::string m_fatJetCorrType;  //!
  std::string m_tagStrategy; //!
  std::vector<std::string> m_csCorrections; //!
  std::vector<std::string> m_csVariations; //!
  long   m_eventCountPassed_VBFHbb; // !
  std::string m_fJVTCut;
  float m_luminosity_a;//!
  float m_luminosity_d;//!
  float m_luminosity_e;//!

    double m_b_85_weight = 0.11;
    double m_b_77_weight = 0.64;
    double m_b_70_weight = 0.83;
    double m_b_60_weight = 0.94;

  // Jet Assignment Tool
  jetAssignmentTool *m_jetAssigner; //!

  KF::KinematicFit *m_KF; //!

  // for boosted analysis
  bool m_doBlindAnalysis; //!
  bool m_doFillHistograms; //!
  bool m_useFastHistSvc; //!
  bool m_doMergeCR; //!
  bool m_doCutflow; //!
  HistoVec m_fatJetHistos; //!
  bool m_DoBJetTriggerOfflineScale;//!
  bool m_DoBJetTriggerWeight;//!

  //double ReadOfflineBJetTriggerWeight(TGraph* TrighHist, double pT);
  std::vector<float> ReadOfflineBJetTriggerWeight(TGraph* TrigSF, double var, bool ispT=true);

  TGraph *m_TrigEffHist_offline70_online60_data;//!
  TGraph *m_TrigEffHist_offline70_online60_mc;//!
  TGraph *m_TrigEffHist_offline70_online70_data;//!
  TGraph *m_TrigEffHist_offline70_online70_mc;//!
  TGraph *m_TrigEffHist_offline85_online85_data;//!
  TGraph *m_TrigEffHist_offline85_online85_mc;//!

  TGraph *m_SF_Off85_match_hlt85_jetPt;//!
  TGraph *m_SF_Off85_match_hlt70_jetPt;//!
  TGraph *m_SF_Off85_match_hlt60_jetPt;//!

  TGraph *m_SF_Off77_match_hlt85_jetPt;//!
  TGraph *m_SF_Off77_match_hlt70_jetPt;//!
  TGraph *m_SF_Off77_match_hlt60_jetPt;//!

  TGraph *m_SF_Off70_match_hlt85_jetPt;//!
  TGraph *m_SF_Off70_match_hlt70_jetPt;//!
  TGraph *m_SF_Off70_match_hlt60_jetPt;//!

  TGraph *m_SF_Off60_match_hlt85_jetPt;//!
  TGraph *m_SF_Off60_match_hlt70_jetPt;//!
  TGraph *m_SF_Off60_match_hlt60_jetPt;//!


  TGraph *m_Eff_Off85_match_hlt85_jetPt;//!
  TGraph *m_Eff_Off85_match_hlt70_jetPt;//!
  TGraph *m_Eff_Off85_match_hlt60_jetPt;//!
  TGraph *m_Eff_Off85_match_hlt40_jetPt;//!

  TGraph *m_Eff_Off77_match_hlt85_jetPt;//!
  TGraph *m_Eff_Off77_match_hlt70_jetPt;//!
  TGraph *m_Eff_Off77_match_hlt60_jetPt;//!
  TGraph *m_Eff_Off77_match_hlt40_jetPt;//!

  TGraph *m_Eff_Off70_match_hlt85_jetPt;//!
  TGraph *m_Eff_Off70_match_hlt70_jetPt;//!
  TGraph *m_Eff_Off70_match_hlt60_jetPt;//!
  TGraph *m_Eff_Off70_match_hlt40_jetPt;//!

  TGraph *m_Eff_Off60_match_hlt85_jetPt;//!
  TGraph *m_Eff_Off60_match_hlt70_jetPt;//!
  TGraph *m_Eff_Off60_match_hlt60_jetPt;//!
  TGraph *m_Eff_Off60_match_hlt40_jetPt;//!

  TGraph *m_SF_Off85_match_hlt85_jetPt16;//!
  TGraph *m_SF_Off85_match_hlt70_jetPt16;//!
  TGraph *m_SF_Off85_match_hlt60_jetPt16;//!

  TGraph *m_SF_Off77_match_hlt85_jetPt16;//!
  TGraph *m_SF_Off77_match_hlt70_jetPt16;//!
  TGraph *m_SF_Off77_match_hlt60_jetPt16;//!

  TGraph *m_SF_Off70_match_hlt85_jetPt16;//!
  TGraph *m_SF_Off70_match_hlt70_jetPt16;//!
  TGraph *m_SF_Off70_match_hlt60_jetPt16;//!

  TGraph *m_SF_Off60_match_hlt85_jetPt16;//!
  TGraph *m_SF_Off60_match_hlt70_jetPt16;//!
  TGraph *m_SF_Off60_match_hlt60_jetPt16;//!


  TGraph *m_Eff_Off85_match_hlt85_jetPt16;//!
  TGraph *m_Eff_Off85_match_hlt70_jetPt16;//!
  TGraph *m_Eff_Off85_match_hlt60_jetPt16;//!
  TGraph *m_Eff_Off85_match_hlt40_jetPt16;//!

  TGraph *m_Eff_Off77_match_hlt85_jetPt16;//!
  TGraph *m_Eff_Off77_match_hlt70_jetPt16;//!
  TGraph *m_Eff_Off77_match_hlt60_jetPt16;//!
  TGraph *m_Eff_Off77_match_hlt40_jetPt16;//!

  TGraph *m_Eff_Off70_match_hlt85_jetPt16;//!
  TGraph *m_Eff_Off70_match_hlt70_jetPt16;//!
  TGraph *m_Eff_Off70_match_hlt60_jetPt16;//!
  TGraph *m_Eff_Off70_match_hlt40_jetPt16;//!

  TGraph *m_Eff_Off60_match_hlt85_jetPt16;//!
  TGraph *m_Eff_Off60_match_hlt70_jetPt16;//!
  TGraph *m_Eff_Off60_match_hlt60_jetPt16;//!
  TGraph *m_Eff_Off60_match_hlt40_jetPt16;//!

  TGraph *m_Eff_Event_leadingJet_jetEta;//!

  TFile  *m_inFile;//!
  TFile  *m_inFile16;//!
  std::string   m_JetAssignmentStrategy;//!
  lwt::LightweightNeuralNetwork *conehead;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_l0;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_l1;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_l5;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_l10;//!
  lwt::LightweightGraph *lwtgraph_adv_e100_l0;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_adv_e50_b10_mb10_n512;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_adv_e100_b10_mb10_n512;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_adv_e200_b10_mb10_n512;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_adv_e50_b100_mb10_n512;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_adv_e100_b100_mb10_n512;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_adv_e200_b100_mb10_n512;//!
  lwt::LightweightGraph *lwtgraph_4cen_loose_l0;//!
  lwt::LightweightGraph *lwtgraph_4cen_loose_l10;//!
  lwt::LightweightGraph *lwtgraph_mcmj_l0;//!
  lwt::LightweightGraph *lwtgraph_mcmj_l10;//!
  lwt::LightweightGraph *lwtgraph_mcmjw_l0;//!
  lwt::LightweightGraph *lwtgraph_mcmjw_l10;//!
  lwt::LightweightGraph *lwtgraph_mcmjw_e200_l0;//!
  lwt::LightweightGraph *lwtgraph_mcmjw_e200_l1;//!
  lwt::LightweightGraph *lwtgraph_mcmjw_e200_l5;//!
  lwt::LightweightGraph *lwtgraph_mcmjw_e200_l10;//!
  lwt::LightweightGraph *lwtgraph_nunez26;//!
  lwt::LightweightGraph *lwtgraph_RE29;//!
  lwt::LightweightGraph *lwtgraph_RE34;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_20190905_b100_l0;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_20190905_b100_l10;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_20190905_b10_l0;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_20190905_b10_l10;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_nojets0;//!
  lwt::LightweightGraph *lwtgraph_1for2cen_nojets10;//!
  lwt::LightweightGraph *lwtgraph_data1_e50_l0;//!
  lwt::LightweightGraph *lwtgraph_data1_e50_l10;//!
  lwt::LightweightGraph *lwtgraph_data1_e100_l0;//!
  lwt::LightweightGraph *lwtgraph_data1_e100_l10;//!
  lwt::LightweightGraph *lwtgraph_data1_e100_l40;//!
  lwt::LightweightGraph *lwtgraph_data1_e100_l60;//!
  lwt::LightweightGraph *lwtgraph_data1_e100_l100;//!
  lwt::LightweightGraph *lwtgraph_data2_e50_l0;//!
  lwt::LightweightGraph *lwtgraph_data2_e50_l10;//!
  lwt::LightweightGraph *lwtgraph_data2_e50_l100;//!
  lwt::LightweightGraph *lwtgraph_data2_e100_l0;//!
  lwt::LightweightGraph *lwtgraph_data2_e100_l10;//!
  lwt::LightweightGraph *lwtgraph_data2_e100_l40;//!
  lwt::LightweightGraph *lwtgraph_data2_e100_l60;//!
  lwt::LightweightGraph *lwtgraph_data2_e100_l100;//!
  lwt::LightweightGraph *lwtgraph_data3_e50_l0;//!
  lwt::LightweightGraph *lwtgraph_data3_e50_l10;//!
  lwt::LightweightGraph *lwtgraph_data3_e100_l0;//!
  lwt::LightweightGraph *lwtgraph_data3_e100_l10;//!
  lwt::LightweightGraph *lwtgraph_data3_e100_l40;//!
  lwt::LightweightGraph *lwtgraph_data3_e100_l60;//!
  lwt::LightweightGraph *lwtgraph_data3_e100_l100;//!
  lwt::LightweightGraph *lwtgraph_data3_e100_l200;//!
  lwt::LightweightGraph *lwtgraph_data5_e100_l0;//!
  lwt::LightweightGraph *lwtgraph_data5_e100_l10;//!
  lwt::LightweightGraph *lwtgraph_data6_e100_l5;//!
  lwt::LightweightGraph *lwtgraph_data6_e100_l10;//!
  lwt::LightweightGraph *lwtgraph_data7_e100_l0;//!
  lwt::LightweightGraph *lwtgraph_data7_e100_l5;//!
  lwt::LightweightGraph *lwtgraph_data7_e100_l6;//!
  lwt::LightweightGraph *lwtgraph_data7_e100_l7;//!
  lwt::LightweightGraph *lwtgraph_data7_e100_l10;//!
  lwt::LightweightGraph *lwtgraph_data7_e100_l40;//!
  lwt::LightweightGraph *lwtgraph_data8_e100_l0;//!
  lwt::LightweightGraph *lwtgraph_data8_e100_l3;//!
  lwt::LightweightGraph *lwtgraph_data8_e100_l5;//!
  lwt::LightweightGraph *lwtgraph_data8_e100_l7;//!
  lwt::LightweightGraph *lwtgraph_data8_e100_l10;//!
  lwt::LightweightGraph *lwtgraph_data9_e100_l0;//!
  lwt::LightweightGraph *lwtgraph_data9_e100_l3;//!
  lwt::LightweightGraph *lwtgraph_data9_e100_l5;//!
  lwt::LightweightGraph *lwtgraph_data9_e100_l7;//!
  lwt::LightweightGraph *lwtgraph_data9_e100_l10;//!
  lwt::LightweightGraph *lwtgraph_data9_e100_l11;//!
  lwt::LightweightGraph *lwtgraph_data9_e100_l12;//!
  lwt::LightweightGraph *lwtgraph_data9_e100_l13;//!
  lwt::LightweightGraph *lwtgraph_data9_e100_l15;//!
  lwt::LightweightGraph *lwtgraph_data9_e100_l20;//!
  lwt::LightweightGraph *lwtgraph_data9_e100_l40;//!
  lwt::LightweightGraph *lwtgraph_data9_e100_l100;//!
  lwt::LightweightGraph *lwtgraph_dataM_e100_l0;//!
  lwt::LightweightGraph *lwtgraph_dataM_e100_l10;//!
  lwt::LightweightGraph *lwtgraph_2025odd_e1000_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_2025odd_e1000_l10;//!
  lwt::LightweightGraph *lwtgraph_2024odd_e1000_l0;//!
  lwt::LightweightGraph *lwtgraph_3c_2024odd_e1000_l0;//!
  lwt::LightweightGraph *lwtgraph_4100w2k1_e1000_l10;//!
  lwt::LightweightGraph *lwtgraph_4100w2k0_e1000_l10;//!
  lwt::LightweightGraph *lwtgraph_4100w4k0_l10;//!
  lwt::LightweightGraph *lwtgraph_4100w4k1_l10;//!
  lwt::LightweightGraph *lwtgraph_4100w4k2_l10;//!
  lwt::LightweightGraph *lwtgraph_4100w4k3_l10;//!
  lwt::LightweightGraph *lwtgraph_4100w4k0_l0;//!
  lwt::LightweightGraph *lwtgraph_4100w4k1_l0;//!
  lwt::LightweightGraph *lwtgraph_4100w4k2_l0;//!
  lwt::LightweightGraph *lwtgraph_4100w4k3_l0;//!
  lwt::LightweightGraph *lwtgraph_4200w4k0_l10;//!
  lwt::LightweightGraph *lwtgraph_4200w4k1_l10;//!
  lwt::LightweightGraph *lwtgraph_4200w4k2_l10;//!
  lwt::LightweightGraph *lwtgraph_4200w4k3_l10;//!
  lwt::LightweightGraph *lwtgraph_4200w2k1_e1000_l10;//!
  lwt::LightweightGraph *lwtgraph_4200w2k0_e1000_l10;//!
  lwt::LightweightGraph *lwtgraph_3000even_l0;//!
  lwt::LightweightGraph *lwtgraph_3000odd_l0;//!
  lwt::LightweightGraph *lwtgraph_3000even_l10;//!
  lwt::LightweightGraph *lwtgraph_3000odd_l10;//!
  lwt::LightweightGraph *lwtgraph_3000even_l20;//!
  lwt::LightweightGraph *lwtgraph_3000odd_l20;//!
  lwt::LightweightGraph *lwtgraph_3000even_l40;//!
  lwt::LightweightGraph *lwtgraph_3000odd_l40;//!
  lwt::LightweightGraph *lwtgraph_3000even_l60;//!
  lwt::LightweightGraph *lwtgraph_3000odd_l60;//!
  lwt::LightweightGraph *lwtgraph_3000even_l80;//!
  lwt::LightweightGraph *lwtgraph_3000odd_l80;//!
  lwt::LightweightGraph *lwtgraph_3000reven_l0;//!
  lwt::LightweightGraph *lwtgraph_3000rodd_l0;//!
  lwt::LightweightGraph *lwtgraph_3000reven_l10;//!
  lwt::LightweightGraph *lwtgraph_3000rodd_l10;//!
  lwt::LightweightGraph *lwtgraph_3000reven_l20;//!
  lwt::LightweightGraph *lwtgraph_3000rodd_l20;//!
  lwt::LightweightGraph *lwtgraph_3c_3000reven_l0;//!
  lwt::LightweightGraph *lwtgraph_3c_3000rodd_l0;//!
  lwt::LightweightGraph *lwtgraph_3c_3000reven_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_3000rodd_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_3000reven_l20;//!
  lwt::LightweightGraph *lwtgraph_3c_3000rodd_l20;//!
  lwt::LightweightGraph *lwtgraph_3c_3000even_l0;//!
  lwt::LightweightGraph *lwtgraph_3c_3000odd_l0;//!
  lwt::LightweightGraph *lwtgraph_3c_3000even_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_3000odd_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_3000even_l20;//!
  lwt::LightweightGraph *lwtgraph_3c_3000odd_l20;//!
  lwt::LightweightGraph *lwtgraph_3c_3000even_l40;//!
  lwt::LightweightGraph *lwtgraph_3c_3000odd_l40;//!
  lwt::LightweightGraph *lwtgraph_3c_3000even_l100;//!
  lwt::LightweightGraph *lwtgraph_3c_3000odd_l100;//!
  lwt::LightweightGraph *lwtgraph_3c_4100w2k1_e1000_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_4100w2k0_e1000_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_4100w4k0_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_4100w4k1_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_4100w4k2_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_4100w4k3_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_4100w4k0_l0;//!
  lwt::LightweightGraph *lwtgraph_3c_4100w4k1_l0;//!
  lwt::LightweightGraph *lwtgraph_3c_4100w4k2_l0;//!
  lwt::LightweightGraph *lwtgraph_3c_4100w4k3_l0;//!
  lwt::LightweightGraph *lwtgraph_3c_4200w4k0_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_4200w4k1_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_4200w4k2_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_4200w4k3_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_4200w2k1_e1000_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_4200w2k0_e1000_l10;//!
  lwt::LightweightGraph *lwtgraph_2029odd_e1000_l40;//!
  lwt::LightweightGraph *lwtgraph_3c_2029odd_e1000_l40;//!
  lwt::LightweightGraph *lwtgraph_dataM_pt200_e100_l0;//!
  lwt::LightweightGraph *lwtgraph_dataM_pt200_e100_l10;//!
  lwt::LightweightGraph *lwtgraph_3c_dataM800_e100_l0;//!
  lwt::LightweightGraph *lwtgraph_3c_dataM800_e100_l10;//!
  lwt::LightweightGraph *lwtgraph_GG12;//!
  lwt::LightweightGraph *lwtgraph_GG2;//!
  TMVA::Reader *reader2cen;//!
  TMVA::Reader *reader2cenWeight;//!
  TMVA::Reader *reader_1for2cen_adv;//!
  TMVA::Reader *reader_3cenCon_adv;//!
  TMVA::Reader *reader_1for3cen_adv;//!
  TMVA::Reader *reader_1for2cen_adv2;//!
  TMVA::Reader *reader_1for2cen_adv3;//!
  TMVA::Reader *reader_mcmj;//!
  TMVA::Reader *reader_1for2cen_final;//!
  TMVA::Reader *reader_1for2cen_full;//!
  TMVA::Reader *reader_1for2cen_trueasym;//!
  TMVA::Reader *reader_1for2cen_angle;//!
  TMVA::Reader *reader_1for2cen_nojets;//!
  TMVA::Reader *reader_1for2cen_nojetsrap;//!
  TMVA::Reader *reader_1for2cen_pflow_5jet;//!
  TMVA::Reader *reader_1for2cen_pflow_data;//!
  TMVA::Reader *reader_walrus_pflow_data;//!
  TMVA::Reader *reader_1for2cen_pflow;//!
  TMVA::Reader *reader_3cenCon_final;//!
  TMVA::Reader *reader_walrus_final;//!
  TMVA::Reader *reader_4cen_final;//!
  TMVA::Reader *reader2cenOldExt;//!
  TMVA::Reader *reader3cen;//!
  TMVA::Reader *readerHighpt;//!
  TMVA::Reader *readertopoInc;//!
  TMVA::Reader *reader2cenOld;//!
  TMVA::Reader *reader4cen;//!
  TMVA::Reader *reader_4cen_adv;//!
  TH2D *m_forwardSF2018;//!
  TH2D *m_forwardSF2017;//!
  TH2D *m_forwardSF2016;//!
  TH2D *m_centralSF2018;//!
  TH2D *m_centralSF2017;//!
  TH2D *m_centralSF2016;//!
  TH2D *m_forwardEff2018;//!
  TH2D *m_forwardEff2017;//!
  TH2D *m_forwardEff2016;//!
  TH2D *m_centralEff2018;//!
  TH2D *m_centralEff2017;//!
  TH2D *m_centralEff2016;//!

  TH1D *m_B2_1for2cenEff2018;//!
  TH1D *m_B2_1for2cenEff2017;//!
  TH1D *m_B2_1for2cenEff2016;//!
  TH1D *m_B2_3cenEff2018;//!
  TH1D *m_B2_3cenEff2017;//!
  TH1D *m_B2_3cenEff2016;//!

  TH2F *m_tmp2d;//!
  TH1D *m_herwigMbb;//!
  TH1D *m_Shower_mBB;//!
  TH1D *m_Shower_nJets30pt;//!
  TH1F *h_rescale_4100w4fold0_l10;//!
  TH1F *h_rescale_4100w4fold1_l10;//!
  TH1F *h_rescale_4100w4fold2_l10;//!
  TH1F *h_rescale_4100w4fold3_l10;//!
  TH1F *h_rescale_4100w4fold0_l0;//!
  TH1F *h_rescale_4100w4fold1_l0;//!
  TH1F *h_rescale_4100w4fold2_l0;//!
  TH1F *h_rescale_4100w4fold3_l0;//!
  TH1F *h_rescale_4100w2fold0_l10;//!
  TH1F *h_rescale_4100w2fold1_l10;//!
  TH1F *h_rescale_3c_4100w4fold0_l10;//!
  TH1F *h_rescale_3c_4100w4fold1_l10;//!
  TH1F *h_rescale_3c_4100w4fold2_l10;//!
  TH1F *h_rescale_3c_4100w4fold3_l10;//!
  TH1F *h_rescale_3c_4100w4fold0_l0;//!
  TH1F *h_rescale_3c_4100w4fold1_l0;//!
  TH1F *h_rescale_3c_4100w4fold2_l0;//!
  TH1F *h_rescale_3c_4100w4fold3_l0;//!
  TH1F *h_rescale_3c_4100w2fold0_l10;//!
  TH1F *h_rescale_3c_4100w2fold1_l10;//!

  float m_bdt_mJJ,m_bdt_pTJJ,m_bdt_pT_balance,m_bdt_mindRJ1_Ex,m_bdt_mindRJ2_Ex,m_bdt_eta_J_star,m_bdt_cosTheta_boost,m_bdt_deltaMJJ,m_bdt_deltaMJJ_pTBB_nomBB,m_bdt_max_J1J2,m_bdt_NTrk500PVJ1,m_bdt_NTrk500PVJ2,m_bdt_mBB,m_bdt_centralMaxJ_Ex_dRB1,m_bdt_MV2c10B2,m_bdt_nLooseMv2c10,m_bdt_min_mindRJ1_Ex,m_bdt_min_mindRJ2_Ex,m_bdt_centralMaxJ_Ex,m_bdt_hframe_cosTheta_h_b1,m_bdt_asymJJ,m_bdt_nJets,m_bdt_dEtaJJ,m_bdt_angleAsymBB,m_bdt_inclusiveMaxJ_Ex,m_bdt_inclusiveMaxJ_zeppenfeld,m_bdt_mismatchJJ,m_bdt_pTBB,m_bdt_deltaMJJMax,m_bdt_deltaMJJ2,m_bdt_pTJ1,m_bdt_dPhiBBJJ,m_bdt_dEtaBBJJ_dEtaJJ,m_bdt_nJets20pt,m_bdt_mBB_no_corr,m_bdt_dEtaBBJJ,m_bdt_trueangleAsymBB,m_bdt_mBB_angle;

  static bool sort_pt(const xAOD::Jet* jetA, const xAOD::Jet* jetB) {
    return jetA->pt() > jetB->pt();
  }

public:

  AnalysisReader_VBFHbb ();

  virtual ~AnalysisReader_VBFHbb ();

  // this is needed to distribute the algorithm to the workers
  ClassDefOverride(AnalysisReader_VBFHbb, 1);
};

#endif // ifndef CxAODReader_AnalysisReader_VBFHbb_H
