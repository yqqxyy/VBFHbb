#ifndef CxAODReader_MVATree_VBFHbb_H
#define CxAODReader_MVATree_VBFHbb_H

#include "CxAODReader/MVATree.h"
#include "vector"

class MVATree_VBFHbb : public MVATree {

 protected:
  
  std::string m_analysisType;
  bool m_onlyFitInputs;

  virtual void AddBranch(TString name, Float_t* address);
  virtual void AddBranch(TString name, Int_t* address);
  virtual void AddBranch(TString name, ULong64_t * address);
  virtual void AddBranch(TString name, std::string* address);

  // Additional methods to store std::vectors
  virtual void AddBranch(TString name,std::vector<int> *address);
  virtual void AddBranch(TString name,std::vector<float> *address);

  virtual void SetBranches() override;
  virtual void TransformVars() override;

 public:

  MVATree_VBFHbb(bool persistent, bool readMVA, std::string analysisType, EL::IWorker* wk, std::vector<std::string> variations, bool nominalOnly,bool onlyFitInputs,bool isMC);

  ~MVATree_VBFHbb() {}

  virtual void Reset() override;
  virtual void ReadMVA();

  std::string sample;

  bool m_isMC;
  int RunNumber;
  int LumiBlock;
  unsigned long long EventNumber;
  int npv;
  int ActualMu;
  int AverageMu;
  int ActualMuScaled;
  int AverageMuScaled;
  int AverageMuRaw;
  int ActualMuRaw;
  float embMu1pt;
  float embMu1eta;
  float embMu1phi;
  float embMu1ptvarcone30;
  float embMu1topoetcone20;
  float embMu2pt;
  float embMu2eta;
  float embMu2phi;
  float embMu2ptvarcone30;
  float embMu2topoetcone20;
  float embZpt;
  float embZeta;
  float embZphi;
  float embZm;
  float hpt;
  float heta;
  float hy;
  float mJJ_truth;
  float mJJ_truth_new;
  float pTHJJ_truth_new;
  float mJJ_truth_abs;
  float dEtaJJ_truth;
  float dEtaJJ_truth_abs;
  float nJets_truth;
  float HTXS_Higgs_pt;
  float HTXS_Higgs_eta;
  float HTXS_Higgs_phi;
  float HTXS_Higgs_m;
  int HTXS_Stage0_Category;
  int HTXS_Njets_pTjet25;
  int HTXS_Njets_pTjet30;
  int HTXS_Stage1_1_Category_pTjet25;
  int HTXS_Stage1_1_Category_pTjet30;
  int HTXS_Stage1_1_FineIndex_pTjet25;
  int HTXS_Stage1_1_FineIndex_pTjet30;
  int HTXS_Stage1_1_Fine_Category_pTjet25;
  int HTXS_Stage1_1_Fine_Category_pTjet30;
  int HTXS_Stage1_1_Fine_FineIndex_pTjet25;
  int HTXS_Stage1_1_Fine_FineIndex_pTjet30;
  int HTXS_Stage1_Category_pTjet25;
  int HTXS_Stage1_Category_pTjet30;
  int HTXS_Stage1_FineIndex_pTjet25;
  int HTXS_Stage1_FineIndex_pTjet30;
  int STXS_bin;
  float hphi;
  float hm;
  float embMuonWeight_cvmfs;
  float embMuonTrigWeight_cvmfs;
  float embMuonTrigWeight_cvmfs_14;
  float embMuonTrigWeight_cvmfs_50;
  float embMet;
  float embMuonWeight;
  float embMuonTrigWeight;
  float MCWeight;
  float PUWeight;
  float batman;
  float BJetTriggerWeight;
  float ewkWeight;
  float ewkWeightOff;
  float EventWeight;
  float EventWeightforwardSF__1up;
  float EventWeightforwardSF__1down;
  float EventWeightcentralSF__1up;
  float EventWeightcentralSF__1down;
  float EventWeightB2SF__1up;
  float EventWeightB2SF__1down;
  float EventWeightBTrig_Eta_EFF__1up;
  float EventWeightBTrig_Eta_EFF__1down;
  float EventWeightBTrig_SF__1up;
  float EventWeightBTrig_SF__1down;
  float EventWeightShower_mBB__1up;
  float EventWeightShower_mBB__1down;
  float EventWeightShower_nJets30pt__1up;
  float EventWeightShower_nJets30pt__1down;
  float EventWeightFT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_extrapolation_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_extrapolation_AntiKt4EMPFlowJets__1down;
  float EventWeightFT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1up;
  float EventWeightFT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1down;
  float EventWeightalpha_s__1up;
  float EventWeightalpha_s__1down;
  float EventWeightewkCorrection__1up;
  float EventWeightewkCorrection__1down;
  float EventWeightewkCorrectionNorm__1up;
  float EventWeightewkCorrectionNorm__1down;
  float EventWeightBR_bb__1up;
  float EventWeightBR_bb__1down;
  float EventWeightmuR_0p5_muF_0p5;
  float EventWeightmuRmuF__1down;
  float EventWeightmuR_0p5_muF_1p0;
  float EventWeightmuR_0p5_muF_2p0;
  float EventWeightmuR_1p0_muF_0p5;
  float EventWeightmuR_1p0_muF_1p0;
  float EventWeightmuR_1p0_muF_2p0;
  float EventWeightmuR_2p0_muF_0p5;
  float EventWeightmuR_2p0_muF_1p0;
  float EventWeightmuR_2p0_muF_2p0;
  float EventWeightmuRmuF__1up;
  float EventWeightPDF_VBF__1up;
  float EventWeightPDF_VBF__1down;
  float EventWeightQCDscale_VBF__1up;
  float EventWeightQCDscale_VBF__1down;
  float EventWeightPDF_ggF__1up;
  float EventWeightPDF_ggF__1down;
  float EventWeightQCDscale_ggF_mu__1up;
  float EventWeightQCDscale_ggF_mu__1down;
  float EventWeightQCDscale_ggF_res__1up;
  float EventWeightQCDscale_ggF_res__1down;
  float EventWeightQCDscale_ggF_mig01__1up;
  float EventWeightQCDscale_ggF_mig01__1down;
  float EventWeightQCDscale_ggF_mig12__1up;
  float EventWeightQCDscale_ggF_mig12__1down;
  float EventWeightQCDscale_ggF_pTH60__1up;
  float EventWeightQCDscale_ggF_pTH60__1down;
  float EventWeightQCDscale_ggF_pTH120__1up;
  float EventWeightQCDscale_ggF_pTH120__1down;
  float EventWeightQCDscale_ggF_qm_t__1up;
  float EventWeightQCDscale_ggF_qm_t__1down;
  float EventWeightQCDscale_ggF_vbf2j__1up;
  float EventWeightQCDscale_ggF_vbf2j__1down;
  float EventWeightQCDscale_ggF_vbf3j__1up;
  float EventWeightQCDscale_ggF_vbf3j__1down;
  float EventWeightQCDscale_ggF__1up;
  float EventWeightQCDscale_ggF__1down;
  float EventWeightPDF4LHC_NLO_30_EV1__1up;
  float EventWeightPDF4LHC_NLO_30_EV1__1down;
  float EventWeightPDF4LHC_NLO_30_EV2__1up;
  float EventWeightPDF4LHC_NLO_30_EV2__1down;
  float EventWeightPDF4LHC_NLO_30_EV3__1up;
  float EventWeightPDF4LHC_NLO_30_EV3__1down;
  float EventWeightPDF4LHC_NLO_30_EV4__1up;
  float EventWeightPDF4LHC_NLO_30_EV4__1down;
  float EventWeightPDF4LHC_NLO_30_EV5__1up;
  float EventWeightPDF4LHC_NLO_30_EV5__1down;
  float EventWeightPDF4LHC_NLO_30_EV6__1up;
  float EventWeightPDF4LHC_NLO_30_EV6__1down;
  float EventWeightPDF4LHC_NLO_30_EV7__1up;
  float EventWeightPDF4LHC_NLO_30_EV7__1down;
  float EventWeightPDF4LHC_NLO_30_EV8__1up;
  float EventWeightPDF4LHC_NLO_30_EV8__1down;
  float EventWeightPDF4LHC_NLO_30_EV9__1up;
  float EventWeightPDF4LHC_NLO_30_EV9__1down;
  float EventWeightPDF4LHC_NLO_30_EV10__1up;
  float EventWeightPDF4LHC_NLO_30_EV10__1down;
  float EventWeightPDF4LHC_NLO_30_EV11__1up;
  float EventWeightPDF4LHC_NLO_30_EV11__1down;
  float EventWeightPDF4LHC_NLO_30_EV12__1up;
  float EventWeightPDF4LHC_NLO_30_EV12__1down;
  float EventWeightPDF4LHC_NLO_30_EV13__1up;
  float EventWeightPDF4LHC_NLO_30_EV13__1down;
  float EventWeightPDF4LHC_NLO_30_EV14__1up;
  float EventWeightPDF4LHC_NLO_30_EV14__1down;
  float EventWeightPDF4LHC_NLO_30_EV15__1up;
  float EventWeightPDF4LHC_NLO_30_EV15__1down;
  float EventWeightPDF4LHC_NLO_30_EV16__1up;
  float EventWeightPDF4LHC_NLO_30_EV16__1down;
  float EventWeightPDF4LHC_NLO_30_EV17__1up;
  float EventWeightPDF4LHC_NLO_30_EV17__1down;
  float EventWeightPDF4LHC_NLO_30_EV18__1up;
  float EventWeightPDF4LHC_NLO_30_EV18__1down;
  float EventWeightPDF4LHC_NLO_30_EV19__1up;
  float EventWeightPDF4LHC_NLO_30_EV19__1down;
  float EventWeightPDF4LHC_NLO_30_EV20__1up;
  float EventWeightPDF4LHC_NLO_30_EV20__1down;
  float EventWeightPDF4LHC_NLO_30_EV21__1up;
  float EventWeightPDF4LHC_NLO_30_EV21__1down;
  float EventWeightPDF4LHC_NLO_30_EV22__1up;
  float EventWeightPDF4LHC_NLO_30_EV22__1down;
  float EventWeightPDF4LHC_NLO_30_EV23__1up;
  float EventWeightPDF4LHC_NLO_30_EV23__1down;
  float EventWeightPDF4LHC_NLO_30_EV24__1up;
  float EventWeightPDF4LHC_NLO_30_EV24__1down;
  float EventWeightPDF4LHC_NLO_30_EV25__1up;
  float EventWeightPDF4LHC_NLO_30_EV25__1down;
  float EventWeightPDF4LHC_NLO_30_EV26__1up;
  float EventWeightPDF4LHC_NLO_30_EV26__1down;
  float EventWeightPDF4LHC_NLO_30_EV27__1up;
  float EventWeightPDF4LHC_NLO_30_EV27__1down;
  float EventWeightPDF4LHC_NLO_30_EV28__1up;
  float EventWeightPDF4LHC_NLO_30_EV28__1down;
  float EventWeightPDF4LHC_NLO_30_EV29__1up;
  float EventWeightPDF4LHC_NLO_30_EV29__1down;
  float EventWeightPDF4LHC_NLO_30_EV30__1up;
  float EventWeightPDF4LHC_NLO_30_EV30__1down;
  float BJetSF;
  int MCChannelNumber;

  int nJets;
  int nJets20pt;
  int nJets20pt_central;
  int nJets20pt_centraljvt;
  int nJets20pt_rap;
  int nJets20pt_fsrmin08;
  int nJets20pt_fsrmin10;
  int nJets20pt_fsrmin12;
  int nJets20pt_fsr10;
  int nJets20pt_fsr12;
  int nJets20pt_fsr14;
  int nJets20pt_fsr16;
  int nJets20pt_fsr18;
  int nJets20pt_fsr20;
  int nJets30pt;
  int nJets30pt_central;
  int nJets30pt_rap;
  int nElectrons;
  int nMuons;
  int nPhotons;
  int met;
  int metsoft;
  int sumsoft;
  int metjet;

  int nTightEmbMv2c10;
  int nMediumEmbMv2c10;
  int nLooseEmbMv2c10;

  int nTightMv2c10;
  int nMediumMv2c10;
  int nLooseMv2c10;

  std::vector<float> pT;
  std::vector<float> phi;
  std::vector<float> eta;
  std::vector<float> mv2c10;
  std::vector<float> HLTJetMatchedPt;
  std::vector<float> L1JetMatchedPt;
  std::vector<float> L1JetMatchedEta;
  std::vector<float> L1JetMatchedPhi;
  std::vector<float> TruthJetMatchedPt;
  std::vector<float> TruthJetMatchedEta;
  std::vector<float> mv2c20;
  std::vector<int>   truthLabel;
  std::vector<int>   truthLabelID;
  std::vector<int>   coneTruthLabelID;
  std::vector<int>   hadronConeExclTruthLabelID;
  std::vector<int>   partonTruthLabelID;
  std::vector<float> weightSysts;
  std::vector<float> weightspdf4lhc;
  std::vector<float> weightspdfnnpdf30;
  std::vector<float> weightsqcdnnlops;
  std::vector<float> weightsqcdnnlops2np;
  std::vector<float> weightsqcdwg1;
  std::vector<float> weightsalternativepdf;

  float alpha_s_up;
  float alpha_s_dn;
  float BR_bb_up;
  float BR_bb_dn;
  float PDF_VBF_up;
  float PDF_VBF_dn;
  float PDF_ggF_up;
  float PDF_ggF_dn;

  int whoIsJ1;
  int whoIsJ2;
  int whoIsB1;
  int whoIsB2;

  float blind;
  float mBB;
  float mBB_nom;
  float mBB_trm;
  float mBB_no_corr;
  float mBB_onlymu;
  float mBB_truth;
  float eventWeightHerwigMbb;
  float mBB_fsrmin08;
  float mBB_fsrmin10;
  float mBB_fsrmin12;
  float mBB_fsr10;
  float mBB_fsr12;
  float mBB_fsr14;
  float mBB_fsr16;
  float mBB_fsr18;
  float mBB_fsr20;
  float dRBB;
  float dRBB_no_corr;
  float dRBB_onlymu;
  float dPhiBB;
  float dEtaBB;
  float pTBB;
  float pTBB_no_corr;
  float pTBB_onlymu;
  float pTBB_fsrmin08;
  float pTBB_fsrmin10;
  float pTBB_fsrmin12;
  float pTBB_fsr10;
  float pTBB_fsr12;
  float pTBB_fsr14;
  float pTBB_fsr16;
  float pTBB_fsr18;
  float pTBB_fsr20;
  float EtaBB;
  float PhiBB;
  float EtaBB_no_corr;
  float EtaBB_onlymu;
  float PhiBB_no_corr;
  float PhiBB_onlymu;

  float mJJ;
  float mJJ_base;
  float etaJ1_base;
  float etaJ2_base;
  float mJJJ;
  float deltaMJJ;
  float deltaMJJ2;
  float deltaMJJJ;
  float deltaMJJ_pTBB_nomBB;
  float deltaMJJ_pTBB;
  float dRJJ;
  float dRMin;
  float pTMin;
  float TE_Q;
  float TE_X1;
  float TE_X2;
  float TE_XF1;
  float TE_XF2;
  float dPhiJJ;
  float dPhiBBJJ;
  float dEtaJJ;
  float dEtaBBJJ;
  float EtaJJ;
  float PhiJJ;
  float pTJJ;
  float asymJJ;
  float angleAsymBB;
  float mBB_angle;
  float pTJJJ;

  float pTJ1;
  float mJ1;
  float pTJ1J;
  float pTJ2;
  float mJ2;
  float pTJ2J;
  float forwardSFJ1;
  float forwardSFJ2;
  float forwardSF;
  float centralSFJ1;
  float centralSFJ2;
  float centralSF;
  float B2SF;
  float pTB1;
  float mB1;
  float pTB1J;
  float pTB2;
  float mB2;
  float pTB2J;
  float pTB1_no_corr;
  float pTB2_no_corr;
  float pTB1_onlymu;
  float pTB2_onlymu;
  float HLTB1;
  float HLTB2;
  float HLTJ1;
  float HLTJ2;
  float L1B1;
  float L1B2;
  float L1J1;
  float L1J2;

  float J1FJVTLoose;
  float J2FJVTLoose;
  float J1FJVTTight;
  float J2FJVTTight;
  float J1Timing;
  float J2Timing;
  float J1JVT;
  float J2JVT;
  float J1FJVT;
  float J2FJVT;

  float etaJ1;
  float etaJ2;
  float etaB1;
  float etaB2;
  float etaB1_no_corr;
  float etaB2_no_corr;
  float etaB1_onlymu;
  float etaB2_onlymu;

  float dRB1J1;
  float dRB1J2;
  float dRB2J1;
  float dRB2J2;

  float TruthLabelB1;
  float TruthLabelB2;
  float TruthLabelPartonB1;
  float TruthLabelPartonB2;
  float HadronConeExclTruthLabelB1;
  float HadronConeExclTruthLabelB2;

  float MV2c10B1;
  float MV2c10B2;
  float MV2c10J1;
  float MV2c10J2;

  float NTrk500PVJ1;
  float NTrk500PVJ2;

  float QGTagger_NTracksJ1;
  float QGTagger_NTracksJ2;
  float QGTagger_TracksC1J1;
  float QGTagger_TracksC1J2;
  float QGTagger_TracksWidthJ1;
  float QGTagger_TracksWidthJ2;
  float QGTaggerWeightJ1;
  float QGTaggerWeightJ2;

  float pT_balance;
  float pT_balanceJ;
  float pT_balance_pTBB;
  float pT_balance_fsrmin08;
  float pT_balance_fsrmin10;
  float pT_balance_fsrmin12;
  float pT_balance_fsr10;
  float pT_balance_fsr12;
  float pT_balance_fsr14;
  float pT_balance_fsr16;
  float pT_balance_fsr18;
  float pT_balance_fsr20;
  float hframe_cosTheta_h_b1;
  float bdt2cen;
  float bdt2cenWeight;
  float bdt_mcmj;
  float bdt_1for2cen_final;
  float bdt_1for2cen_full;
  float bdt_1for2cen_trueasym;
  float bdt_1for2cen_angle;
  float bdt_1for2cen_nojets;
  float bdt_1for2cen_nojetsrap;
  float bdt_1for2cen_pflow_5jet;
  float bdt_1for2cen_pflow_data;
  float bdt_walrus_pflow_data;
  float bdt_1for2cen_pflow;
  float bdt_walrus_final;
  float bdt2cenOldExt;
  float bdt3cen;
  float bdtHighpt;
  float bdttopoInc;
  float bdt2cenOld;
  float nn_adv_e100_l0;
  float nn_nunez26;
  float nn_RE29;
  float nn_RE34;
  float nn_dataM_e100_l0;
  float nn_dataM_e100_l10;
  float nn_2025odd_e1000_l10;
  float nn_3c_2025odd_e1000_l10;
  float nn_2024odd_e1000_l0;
  float nn_3c_2024odd_e1000_l0;
  float nn_2029odd_e1000_l40;
  float nn_3c_2029odd_e1000_l40;
  float nn_dataM_pt200_e100_l0;
  float nn_dataM_pt200_e100_l10;
  float nn_3c_dataM800_e100_l0;
  float nn_3c_dataM800_e100_l10;
  float nn_4100w2k0_e1000_l10;
  float nn_4100w2k1_e1000_l10;
  float nn_4100w2kfold_e1000_l10;
  float nn_4100w2kmerge_e1000_l10;
  float nn_4100w2f0_e1000_l10;
  float nn_4100w2f1_e1000_l10;
  float nn_4100w2fmerge_e1000_l10;
  float nn_4100w4k0_l10;
  float nn_4100w4k1_l10;
  float nn_4100w4k2_l10;
  float nn_4100w4k3_l10;
  float nn_4100w4k0_l0;
  float nn_4100w4k1_l0;
  float nn_4100w4k2_l0;
  float nn_4100w4k3_l0;
  float nn_4100w4kfold_l10;
  float nn_4100w4kmerge_l10;
  float nn_4100w4f0_l10;
  float nn_4100w4f1_l10;
  float nn_4100w4f2_l10;
  float nn_4100w4f3_l10;
  float nn_4100w4fmerge_l10;
  float nn_4100w4f0_l0;
  float nn_4100w4f1_l0;
  float nn_4100w4f2_l0;
  float nn_4100w4f3_l0;
  float nn_4100w4fmerge_l0;
  float nn_4200w4k0_l10;
  float nn_4200w4k1_l10;
  float nn_4200w4k2_l10;
  float nn_4200w4k3_l10;
  float nn_4200w4kfold_l10;
  float nn_4200w4kmerge_l10;
  float nn_4200w2k0_e1000_l10;
  float nn_4200w2k1_e1000_l10;
  float nn_4200w2kfold_e1000_l10;
  float nn_4200w2kmerge_e1000_l10;
  float nn_3000odd_l0;
  float nn_3000even_l0;
  float nn_3000shuffle_l0;
  float nn_3000odd_l10;
  float nn_3000even_l10;
  float nn_3000shuffle_l10;
  float nn_3000odd_l20;
  float nn_3000even_l20;
  float nn_3000shuffle_l20;
  float nn_3000odd_l40;
  float nn_3000even_l40;
  float nn_3000shuffle_l40;
  float nn_3000odd_l60;
  float nn_3000even_l60;
  float nn_3000shuffle_l60;
  float nn_3000odd_l80;
  float nn_3000even_l80;
  float nn_3000shuffle_l80;
  float nn_3000rodd_l0;
  float nn_3000reven_l0;
  float nn_3000rshuffle_l0;
  float nn_3000rodd_l10;
  float nn_3000reven_l10;
  float nn_3000rshuffle_l10;
  float nn_3000rodd_l20;
  float nn_3000reven_l20;
  float nn_3000rshuffle_l20;
  float nn_3c_3000rodd_l0;
  float nn_3c_3000reven_l0;
  float nn_3c_3000rshuffle_l0;
  float nn_3c_3000rodd_l10;
  float nn_3c_3000reven_l10;
  float nn_3c_3000rshuffle_l10;
  float nn_3c_3000rodd_l20;
  float nn_3c_3000reven_l20;
  float nn_3c_3000rshuffle_l20;
  float nn_3c_3000odd_l0;
  float nn_3c_3000even_l0;
  float nn_3c_3000shuffle_l0;
  float nn_3c_3000odd_l10;
  float nn_3c_3000even_l10;
  float nn_3c_3000shuffle_l10;
  float nn_3c_3000odd_l20;
  float nn_3c_3000even_l20;
  float nn_3c_3000shuffle_l20;
  float nn_3c_3000odd_l40;
  float nn_3c_3000even_l40;
  float nn_3c_3000shuffle_l40;
  float nn_3c_3000odd_l100;
  float nn_3c_3000even_l100;
  float nn_3c_3000shuffle_l100;
  float nn_3c_4100w2k0_e1000_l10;
  float nn_3c_4100w2k1_e1000_l10;
  float nn_3c_4100w2kfold_e1000_l10;
  float nn_3c_4100w2kmerge_e1000_l10;
  float nn_3c_4100w2f0_e1000_l10;
  float nn_3c_4100w2f1_e1000_l10;
  float nn_3c_4100w2fmerge_e1000_l10;
  float nn_3c_4100w4k0_l10;
  float nn_3c_4100w4k1_l10;
  float nn_3c_4100w4k2_l10;
  float nn_3c_4100w4k3_l10;
  float nn_3c_4100w4k0_l0;
  float nn_3c_4100w4k1_l0;
  float nn_3c_4100w4k2_l0;
  float nn_3c_4100w4k3_l0;
  float nn_3c_4100w4kfold_l10;
  float nn_3c_4100w4kmerge_l10;
  float nn_3c_4100w4f0_l10;
  float nn_3c_4100w4f1_l10;
  float nn_3c_4100w4f2_l10;
  float nn_3c_4100w4f3_l10;
  float nn_3c_4100w4fmerge_l10;
  float nn_3c_4100w4f0_l0;
  float nn_3c_4100w4f1_l0;
  float nn_3c_4100w4f2_l0;
  float nn_3c_4100w4f3_l0;
  float nn_3c_4100w4fmerge_l0;
  float nn_3c_4200w4k0_l10;
  float nn_3c_4200w4k1_l10;
  float nn_3c_4200w4k2_l10;
  float nn_3c_4200w4k3_l10;
  float nn_3c_4200w4kfold_l10;
  float nn_3c_4200w4kmerge_l10;
  float nn_3c_4200w2k0_e1000_l10;
  float nn_3c_4200w2k1_e1000_l10;
  float nn_3c_4200w2kfold_e1000_l10;
  float nn_3c_4200w2kmerge_e1000_l10;
  float nn_GG12;
  float nn_GG2;

  float mindRJ1_Ex;
  float mindRJ1_Ex_base;
  float mindRJ1_Ex_closest;
  float mindRJ1_Ex_dRB1;
  float mindRJ1_ExPt;
  float mindRJ1_ExPt_closest;
  float mindRJ1_ExPt_dRB1;
  float mindRJ2_Ex;
  float mindRJ2_Ex_base;
  float mindRJ2_Ex_closest;
  float mindRJ2_Ex_dRB1;
  float mindRJ2_ExPt;
  float mindRJ2_ExPt_closest;
  float mindRJ2_ExPt_dRB1;
  float mindRB1_Ex;
  float mindRB2_Ex;

  // =======                                                                   

  float max_J1J2;
  float eta_J_star;
  float HT_soft;
  float centralMaxJ_Ex;
  float nonforwardMaxJ_Ex;
  float inclusiveMaxJ_Ex;
  float centralMaxJ_Ex_dRB1;
  float nonforwardMaxJ_Ex_dRB1;
  float inclusiveMaxJ_Ex_dRB1;
  float centralMaxJ_zeppenfeld;
  float nonforwardMaxJ_zeppenfeld;
  float inclusiveMaxJ_zeppenfeld;
  float centralMaxJ_eta;
  float nonforwardMaxJ_eta;
  float inclusiveMaxJ_eta;
  float centralMaxJ_NTrk500PV;
  float nonforwardMaxJ_NTrk500PV;
  float inclusiveMaxJ_NTrk500PV;
  float centralMaxJ_dEtaJ1;
  float nonforwardMaxJ_dEtaJ1;
  float inclusiveMaxJ_dEtaJ1;
  float HT_MVA;
  float cosTheta_MVA;
  float cosTheta_CMS;
  float cosTheta_boost;

  // =======                                                                                                                                                             
  /* int passCURRENT; */
  /* int passATLAS; */
  /* int passCMS; */
  /* int passHYBRID; */

  /* int whoIsB1_CURRENT; */
  /* int whoIsB2_CURRENT; */
  /* int whoIsB1_ATLAS; */
  /* int whoIsB2_ATLAS; */
  /* int whoIsB1_CMS; */
  /* int whoIsB2_CMS; */
  /* int whoIsB1_HYBRID; */
  /* int whoIsB2_HYBRID; */

  // =======

  std::vector<float> track_jet_pt;
  std::vector<float> track_jet_eta;
  std::vector<int> track_jet_n;

  //  float BDT;

  // trigger decisions
  int passHLT_2mu14;
  int passHLT_mu50;
  // 2 central + 1 forward trigger
  int passL1_J40_0ETA25_2J25_J20_31ETA49;
  int passL1_J40_0ETA25_2J25;
  int passL1_J20_31ETA49;
  int passL1_J15_31ETA49;
  int passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490; 
  int passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25; 
  int passHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25; 
  int passHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25; 
  int passHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25; 
  int passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25; 
  int passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25; 
  int passHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25; 
  int passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30; 
  int passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30; 
  int passHLT_j100_2j55_bmv2c2060_split; 
  int passHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25; 
  int passHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25; 
  int passHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25; 
  int passHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25; 
  int passHLT_j175_gsc225_bmv2c1040_split; 
  int passHLT_j175_bmv2c2040_split; 
  int passHLT_j225_gsc275_bmv2c1060_split; 
  int passHLT_j225_gsc300_bmv2c1070_split; 
  int passHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490; 
  int passHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split; 
  int passHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split; 
  int passHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split; 
  int passHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; 
  int passHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; 
  int passHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; 
  int passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split; 
  int passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split; 
  int passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF; 
  int passHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split; 
  int passHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF; 
  int passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split; 
  int passHLT_j45_320eta490; 
  int passHLT_j80_0eta240_j60_j45_320eta490; 
  int passHLT_j80_0eta240_j60; 
  // 1 central + 2 forward trigger
  int passL1_J40_0ETA25_2J15_31ETA49;
  int passL1_J40_0ETA25;
  int passL1_2J15_31ETA49;
  int passHLT_j80_bmv2c2085_split_2j60_320eta490;
  int passHLT_j80_bmv2c2085_split;
  int passHLT_2j60_320eta490;
  int passHLT_j80_0eta240_2j60_320eta490;
  int passHLT_j80_0eta240;
  // old triggers
  int passL1_4J20;
  int passHLT_2j55_bmv2c2077_split_2j55;
  int passHLT_2j45_bmv2c2070_split_2j45;
  int passL1_4J15;
  int passHLT_2j35_bmv2c2070_split_2j35_L14J15;
  int passHLT_2j35_bmv2c2060_split_2j35_L14J15;
  int passHLT_2j35_bmv2c2050_split_2j35_L14J15;
  int passHLT_j75_bmv2c2060_split_3j75_L14J15; 
  int passHLT_j75_bmv2c2070_split_3j75_L14J15;
  // additional triggers for photon study
  int passHLT_2j45_bmv2c2077_split_2j45_L14J15; 
  int passHLT_2j45_bmv2c2070_split_2j45_L14J15; 
  int passHLT_4j45; 

  // trigger activity
  int isActiveL1_J40_0ETA25_2J25_J20_31ETA49;
  int isActiveHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490; 
  int isActiveHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25; 
  int isActiveHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25; 
  int isActiveHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25; 
  int isActiveHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25; 
  int isActiveHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25; 
  int isActiveHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25; 
  int isActiveHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25; 
  int isActiveHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30; 
  int isActiveHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30; 
  int isActiveHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25; 
  int isActiveHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25; 
  int isActiveHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25; 
  int isActiveHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25; 
  int isActiveHLT_j175_gsc225_bmv2c1040_split; 
  int isActiveHLT_j175_bmv2c2040_split; 
  int isActiveHLT_j225_gsc275_bmv2c1060_split; 
  int isActiveHLT_j225_gsc300_bmv2c1070_split; 
  int isActiveHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490; 
  int isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split; 
  int isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split; 
  int isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split; 
  int isActiveHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; 
  int isActiveHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; 
  int isActiveHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; 
  int isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split; 
  int isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split; 
  int isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF; 
  int isActiveHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split; 
  int isActiveHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF; 
  int isActiveHLT_j80_0eta240_j60_j45_320eta490; 
  int isActiveL1_J40_0ETA25_2J15_31ETA49;
  int isActiveHLT_j80_bmv2c2085_split_2j60_320eta490;
  int isActiveHLT_j80_0eta240_2j60_320eta490;
  int isActiveL1_4J20;
  int isActiveHLT_2j55_bmv2c2077_split_2j55;
  int isActiveHLT_2j45_bmv2c2070_split_2j45;
  int isActiveL1_4J15;
  int isActiveHLT_2j35_bmv2c2070_split_2j35_L14J15;
  int isActiveHLT_2j35_bmv2c2060_split_2j35_L14J15;
  int isActiveHLT_2j35_bmv2c2050_split_2j35_L14J15;
  int isActiveHLT_j75_bmv2c2060_split_3j75_L14J15; 
  int isActiveHLT_j75_bmv2c2070_split_3j75_L14J15;
  int isActiveHLT_2j45_bmv2c2077_split_2j45_L14J15; 
  int isActiveHLT_2j45_bmv2c2070_split_2j45_L14J15; 
  int isActiveHLT_4j45; 

  // trigger prescales
  // gammas
  // 2 central + 1 forward trigger
  float prescaleL1_J40_0ETA25_2J25_J20_31ETA49;
  float prescaleHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490; 
  float prescaleHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25; 
  float prescaleHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25; 
  float prescaleHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25; 
  float prescaleHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25; 
  float prescaleHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25; 
  float prescaleHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25; 
  float prescaleHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25; 
  float prescaleHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30; 
  float prescaleHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30; 
  float prescaleHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25; 
  float prescaleHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25; 
  float prescaleHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25; 
  float prescaleHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25; 
  float prescaleHLT_j175_gsc225_bmv2c1040_split; 
  float prescaleHLT_j175_bmv2c2040_split; 
  float prescaleHLT_j225_gsc275_bmv2c1060_split; 
  float prescaleHLT_j225_gsc300_bmv2c1070_split; 
  float prescaleHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490; 
  float prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split; 
  float prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split; 
  float prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split; 
  float prescaleHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; 
  float prescaleHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; 
  float prescaleHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; 
  float prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split; 
  float prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split; 
  float prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF; 
  float prescaleHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split; 
  float prescaleHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF; 
  float prescaleHLT_j80_0eta240_j60_j45_320eta490; 
  // 1 central + 2 forward trigger
  float prescaleL1_J40_0ETA25_2J15_31ETA49;
  float prescaleHLT_j80_bmv2c2085_split_2j60_320eta490;
  float prescaleHLT_j80_0eta240_2j60_320eta490;
  // old triggers
  float prescaleL1_4J20;
  float prescaleHLT_2j55_bmv2c2077_split_2j55;
  float prescaleHLT_2j45_bmv2c2070_split_2j45;
  float prescaleL1_4J15;
  float prescaleHLT_2j35_bmv2c2070_split_2j35_L14J15;
  float prescaleHLT_2j35_bmv2c2060_split_2j35_L14J15;
  float prescaleHLT_2j35_bmv2c2050_split_2j35_L14J15;
  float prescaleHLT_j75_bmv2c2060_split_3j75_L14J15; 
  float prescaleHLT_j75_bmv2c2070_split_3j75_L14J15;
  // additional triggers for photon study
  float prescaleHLT_2j45_bmv2c2077_split_2j45_L14J15; 
  float prescaleHLT_2j45_bmv2c2070_split_2j45_L14J15; 
  float prescaleHLT_4j45; 

};

#endif
