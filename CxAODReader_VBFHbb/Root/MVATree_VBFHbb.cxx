#include "CxAODReader_VBFHbb/MVATree_VBFHbb.h"
#include "EventLoop/Worker.h"

MVATree_VBFHbb::MVATree_VBFHbb(bool persistent, bool readMVA, std::string analysisType, EL::IWorker* wk, std::vector<std::string> variations, bool nominalOnly,bool onlyFitInputs,bool isMC) : 
  MVATree(persistent, readMVA, wk, variations, nominalOnly),
  m_analysisType(analysisType),m_onlyFitInputs(onlyFitInputs),m_isMC(isMC)
{
  SetBranches();
}


void MVATree_VBFHbb::AddBranch(TString name, Int_t* address)
{
  for (std::pair<std::string, TTree*> tree : m_treeMap) {
    tree.second -> Branch(name, address);
  }
  m_reader.AddVariable(name, address);
}

void MVATree_VBFHbb::AddBranch(TString name, Float_t* address)
{
  for (std::pair<std::string, TTree*> tree : m_treeMap) {
    tree.second -> Branch(name, address);
  }
  m_reader.AddVariable(name, address);
}

void MVATree_VBFHbb::AddBranch(TString name, ULong64_t* address)
{
  for (std::pair<std::string, TTree*> tree : m_treeMap) {
    tree.second -> Branch(name, address);
  }
  m_reader.AddVariable(name, address);
}

void MVATree_VBFHbb::AddBranch(TString name, std::string* address)
{
  for (std::pair<std::string, TTree*> tree : m_treeMap) {
    tree.second -> Branch(name, address);
  }
}


// Additional methods for storing std::vectors
void MVATree_VBFHbb::AddBranch(TString name, std::vector<int> *address)
{
  for (std::pair<std::string, TTree*> tree : m_treeMap) {
    tree.second -> Branch(name, address);
  }
}
void MVATree_VBFHbb::AddBranch(TString name, std::vector<float> *address)
{
  for (std::pair<std::string, TTree*> tree : m_treeMap) {
    tree.second -> Branch(name, address);
  }
}


void MVATree_VBFHbb::SetBranches()
{
  // prepare MVA reader
  m_reader.SetSplitVar(&EventNumber);
  m_reader.AddReader("reader", 2);

  //  AddBranch("sample", &sample);
  AddBranch("mBB"        ,     &mBB);
  AddBranch("mBB_nom"        ,     &mBB_nom);
  AddBranch("mBB_trm"        ,     &mBB_trm);
  AddBranch("pTBB"       ,     &pTBB);
  //AddBranch("dRBB"       ,     &dRBB);
  //AddBranch("dRBB_no_corr",    &dRBB_no_corr);
  //AddBranch("dRBB_onlymu",    &dRBB_onlymu);
  AddBranch("embZm",     &embZm);
  AddBranch("bdt2cenOldExt" ,    &bdt2cenOldExt );
  //AddBranch("bdt2cen" ,    &bdt2cen );
  AddBranch("eventWeight",     &EventWeight);
  AddBranch("dRMin"       ,     &dRMin);
  AddBranch("pTMin"       ,     &pTMin);
  AddBranch("blind"        ,     &blind);
  AddBranch("bdt_mcmj" ,    &bdt_mcmj );
  AddBranch("bdt_1for2cen_final" ,    &bdt_1for2cen_final );
  AddBranch("bdt_1for2cen_full" ,    &bdt_1for2cen_full );
  AddBranch("bdt_1for2cen_trueasym" ,    &bdt_1for2cen_trueasym );
  AddBranch("bdt_1for2cen_angle" ,    &bdt_1for2cen_angle );
  AddBranch("bdt_1for2cen_nojets" ,    &bdt_1for2cen_nojets );
  AddBranch("bdt_1for2cen_nojetsrap" ,    &bdt_1for2cen_nojetsrap );
  AddBranch("bdt_1for2cen_pflow_5jet" ,    &bdt_1for2cen_pflow_5jet );
  AddBranch("bdt_1for2cen_pflow_data" ,    &bdt_1for2cen_pflow_data );
  AddBranch("bdt_walrus_pflow_data" ,    &bdt_walrus_pflow_data );
  AddBranch("bdt_1for2cen_pflow" ,    &bdt_1for2cen_pflow );
  AddBranch("bdt_walrus_final" ,    &bdt_walrus_final );

  //AddBranch("nn_dataM_e100_l0" ,    &nn_dataM_e100_l0 );
  //AddBranch("nn_dataM_e100_l10" ,    &nn_dataM_e100_l10 );
  AddBranch("nn_2025odd_e1000_l10" ,    &nn_2025odd_e1000_l10 );
  AddBranch("nn_3c_2025odd_e1000_l10" ,    &nn_3c_2025odd_e1000_l10 );
  AddBranch("nn_2024odd_e1000_l0" ,    &nn_2024odd_e1000_l0 );
  AddBranch("nn_3c_2024odd_e1000_l0" ,    &nn_3c_2024odd_e1000_l0 );
  AddBranch("nn_4100w2k0_e1000_l10" ,    &nn_4100w2k0_e1000_l10 );
  AddBranch("nn_4100w2k1_e1000_l10" ,    &nn_4100w2k1_e1000_l10 );
  AddBranch("nn_4100w2kfold_e1000_l10" ,    &nn_4100w2kfold_e1000_l10 );
  AddBranch("nn_4100w2kmerge_e1000_l10" ,    &nn_4100w2kmerge_e1000_l10 );
  AddBranch("nn_4100w2f0_e1000_l10" ,    &nn_4100w2f0_e1000_l10 );
  AddBranch("nn_4100w2f1_e1000_l10" ,    &nn_4100w2f1_e1000_l10 );
  AddBranch("nn_4100w2fmerge_e1000_l10" ,    &nn_4100w2fmerge_e1000_l10 );
  AddBranch("nn_4100w4k0_l10" ,    &nn_4100w4k0_l10 );
  AddBranch("nn_4100w4k1_l10" ,    &nn_4100w4k1_l10 );
  AddBranch("nn_4100w4k2_l10" ,    &nn_4100w4k2_l10 );
  AddBranch("nn_4100w4k3_l10" ,    &nn_4100w4k3_l10 );
  AddBranch("nn_4100w4k0_l0" ,    &nn_4100w4k0_l0 );
  AddBranch("nn_4100w4k1_l0" ,    &nn_4100w4k1_l0 );
  AddBranch("nn_4100w4k2_l0" ,    &nn_4100w4k2_l0 );
  AddBranch("nn_4100w4k3_l0" ,    &nn_4100w4k3_l0 );
  AddBranch("nn_4100w4kfold_l10" ,    &nn_4100w4kfold_l10 );
  AddBranch("nn_4100w4kmerge_l10" ,    &nn_4100w4kmerge_l10 );
  AddBranch("nn_4100w4f0_l10" ,    &nn_4100w4f0_l10 );
  AddBranch("nn_4100w4f1_l10" ,    &nn_4100w4f1_l10 );
  AddBranch("nn_4100w4f2_l10" ,    &nn_4100w4f2_l10 );
  AddBranch("nn_4100w4f3_l10" ,    &nn_4100w4f3_l10 );
  AddBranch("nn_4100w4fmerge_l10" ,    &nn_4100w4fmerge_l10 );
  AddBranch("nn_4100w4f0_l0" ,    &nn_4100w4f0_l0 );
  AddBranch("nn_4100w4f1_l0" ,    &nn_4100w4f1_l0 );
  AddBranch("nn_4100w4f2_l0" ,    &nn_4100w4f2_l0 );
  AddBranch("nn_4100w4f3_l0" ,    &nn_4100w4f3_l0 );
  AddBranch("nn_4100w4fmerge_l0" ,    &nn_4100w4fmerge_l0 );
  AddBranch("nn_4200w4k0_l10" ,    &nn_4200w4k0_l10 );
  AddBranch("nn_4200w4k1_l10" ,    &nn_4200w4k1_l10 );
  AddBranch("nn_4200w4k2_l10" ,    &nn_4200w4k2_l10 );
  AddBranch("nn_4200w4k3_l10" ,    &nn_4200w4k3_l10 );
  AddBranch("nn_4200w4kfold_l10" ,    &nn_4200w4kfold_l10 );
  AddBranch("nn_4200w4kmerge_l10" ,    &nn_4200w4kmerge_l10 );
  AddBranch("nn_4200w2k0_e1000_l10" ,    &nn_4200w2k0_e1000_l10 );
  AddBranch("nn_4200w2k1_e1000_l10" ,    &nn_4200w2k1_e1000_l10 );
  AddBranch("nn_4200w2kfold_e1000_l10" ,    &nn_4200w2kfold_e1000_l10 );
  AddBranch("nn_4200w2kmerge_e1000_l10" ,    &nn_4200w2kmerge_e1000_l10 );
  AddBranch("nn_3000odd_l0" ,    &nn_3000odd_l0 );
  AddBranch("nn_3000even_l0" ,    &nn_3000even_l0 );
  AddBranch("nn_3000shuffle_l0" ,    &nn_3000shuffle_l0 );
  AddBranch("nn_3000odd_l10" ,    &nn_3000odd_l10 );
  AddBranch("nn_3000even_l10" ,    &nn_3000even_l10 );
  AddBranch("nn_3000shuffle_l10" ,    &nn_3000shuffle_l10 );
  AddBranch("nn_3000odd_l20" ,    &nn_3000odd_l20 );
  AddBranch("nn_3000even_l20" ,    &nn_3000even_l20 );
  AddBranch("nn_3000shuffle_l20" ,    &nn_3000shuffle_l20 );
  AddBranch("nn_3000odd_l40" ,    &nn_3000odd_l40 );
  AddBranch("nn_3000even_l40" ,    &nn_3000even_l40 );
  AddBranch("nn_3000shuffle_l40" ,    &nn_3000shuffle_l40 );
  AddBranch("nn_3000odd_l60" ,    &nn_3000odd_l60 );
  AddBranch("nn_3000even_l60" ,    &nn_3000even_l60 );
  AddBranch("nn_3000shuffle_l60" ,    &nn_3000shuffle_l60 );
  AddBranch("nn_3000odd_l80" ,    &nn_3000odd_l80 );
  AddBranch("nn_3000even_l80" ,    &nn_3000even_l80 );
  AddBranch("nn_3000shuffle_l80" ,    &nn_3000shuffle_l80 );
  AddBranch("nn_3000rodd_l0" ,    &nn_3000rodd_l0 );
  AddBranch("nn_3000reven_l0" ,    &nn_3000reven_l0 );
  AddBranch("nn_3000rshuffle_l0" ,    &nn_3000rshuffle_l0 );
  AddBranch("nn_3000rodd_l10" ,    &nn_3000rodd_l10 );
  AddBranch("nn_3000reven_l10" ,    &nn_3000reven_l10 );
  AddBranch("nn_3000rshuffle_l10" ,    &nn_3000rshuffle_l10 );
  AddBranch("nn_3000rodd_l20" ,    &nn_3000rodd_l20 );
  AddBranch("nn_3000reven_l20" ,    &nn_3000reven_l20 );
  AddBranch("nn_3000rshuffle_l20" ,    &nn_3000rshuffle_l20 );
  AddBranch("nn_3c_3000rodd_l0" ,    &nn_3c_3000rodd_l0 );
  AddBranch("nn_3c_3000reven_l0" ,    &nn_3c_3000reven_l0 );
  AddBranch("nn_3c_3000rshuffle_l0" ,    &nn_3c_3000rshuffle_l0 );
  AddBranch("nn_3c_3000rodd_l10" ,    &nn_3c_3000rodd_l10 );
  AddBranch("nn_3c_3000reven_l10" ,    &nn_3c_3000reven_l10 );
  AddBranch("nn_3c_3000rshuffle_l10" ,    &nn_3c_3000rshuffle_l10 );
  AddBranch("nn_3c_3000rodd_l20" ,    &nn_3c_3000rodd_l20 );
  AddBranch("nn_3c_3000reven_l20" ,    &nn_3c_3000reven_l20 );
  AddBranch("nn_3c_3000rshuffle_l20" ,    &nn_3c_3000rshuffle_l20 );
  AddBranch("nn_3c_3000odd_l0" ,    &nn_3c_3000odd_l0 );
  AddBranch("nn_3c_3000even_l0" ,    &nn_3c_3000even_l0 );
  AddBranch("nn_3c_3000shuffle_l0" ,    &nn_3c_3000shuffle_l0 );
  AddBranch("nn_3c_3000odd_l10" ,    &nn_3c_3000odd_l10 );
  AddBranch("nn_3c_3000even_l10" ,    &nn_3c_3000even_l10 );
  AddBranch("nn_3c_3000shuffle_l10" ,    &nn_3c_3000shuffle_l10 );
  AddBranch("nn_3c_3000odd_l20" ,    &nn_3c_3000odd_l20 );
  AddBranch("nn_3c_3000even_l20" ,    &nn_3c_3000even_l20 );
  AddBranch("nn_3c_3000shuffle_l20" ,    &nn_3c_3000shuffle_l20 );
  AddBranch("nn_3c_3000odd_l40" ,    &nn_3c_3000odd_l40 );
  AddBranch("nn_3c_3000even_l40" ,    &nn_3c_3000even_l40 );
  AddBranch("nn_3c_3000shuffle_l40" ,    &nn_3c_3000shuffle_l40 );
  AddBranch("nn_3c_3000odd_l100" ,    &nn_3c_3000odd_l100 );
  AddBranch("nn_3c_3000even_l100" ,    &nn_3c_3000even_l100 );
  AddBranch("nn_3c_3000shuffle_l100" ,    &nn_3c_3000shuffle_l100 );
  AddBranch("nn_3c_4100w2k0_e1000_l10" ,    &nn_3c_4100w2k0_e1000_l10 );
  AddBranch("nn_3c_4100w2k1_e1000_l10" ,    &nn_3c_4100w2k1_e1000_l10 );
  AddBranch("nn_3c_4100w2kfold_e1000_l10" ,    &nn_3c_4100w2kfold_e1000_l10 );
  AddBranch("nn_3c_4100w2kmerge_e1000_l10" ,    &nn_3c_4100w2kmerge_e1000_l10 );
  AddBranch("nn_3c_4100w2f0_e1000_l10" ,    &nn_3c_4100w2f0_e1000_l10 );
  AddBranch("nn_3c_4100w2f1_e1000_l10" ,    &nn_3c_4100w2f1_e1000_l10 );
  AddBranch("nn_3c_4100w2fmerge_e1000_l10" ,    &nn_3c_4100w2fmerge_e1000_l10 );
  AddBranch("nn_3c_4100w4k0_l10" ,    &nn_3c_4100w4k0_l10 );
  AddBranch("nn_3c_4100w4k1_l10" ,    &nn_3c_4100w4k1_l10 );
  AddBranch("nn_3c_4100w4k2_l10" ,    &nn_3c_4100w4k2_l10 );
  AddBranch("nn_3c_4100w4k3_l10" ,    &nn_3c_4100w4k3_l10 );
  AddBranch("nn_3c_4100w4k0_l0" ,    &nn_3c_4100w4k0_l0 );
  AddBranch("nn_3c_4100w4k1_l0" ,    &nn_3c_4100w4k1_l0 );
  AddBranch("nn_3c_4100w4k2_l0" ,    &nn_3c_4100w4k2_l0 );
  AddBranch("nn_3c_4100w4k3_l0" ,    &nn_3c_4100w4k3_l0 );
  AddBranch("nn_3c_4100w4kfold_l10" ,    &nn_3c_4100w4kfold_l10 );
  AddBranch("nn_3c_4100w4kmerge_l10" ,    &nn_3c_4100w4kmerge_l10 );
  AddBranch("nn_3c_4100w4f0_l10" ,    &nn_3c_4100w4f0_l10 );
  AddBranch("nn_3c_4100w4f1_l10" ,    &nn_3c_4100w4f1_l10 );
  AddBranch("nn_3c_4100w4f2_l10" ,    &nn_3c_4100w4f2_l10 );
  AddBranch("nn_3c_4100w4f3_l10" ,    &nn_3c_4100w4f3_l10 );
  AddBranch("nn_3c_4100w4fmerge_l10" ,    &nn_3c_4100w4fmerge_l10 );
  AddBranch("nn_3c_4100w4f0_l0" ,    &nn_3c_4100w4f0_l0 );
  AddBranch("nn_3c_4100w4f1_l0" ,    &nn_3c_4100w4f1_l0 );
  AddBranch("nn_3c_4100w4f2_l0" ,    &nn_3c_4100w4f2_l0 );
  AddBranch("nn_3c_4100w4f3_l0" ,    &nn_3c_4100w4f3_l0 );
  AddBranch("nn_3c_4100w4fmerge_l0" ,    &nn_3c_4100w4fmerge_l0 );
  AddBranch("nn_3c_4200w4k0_l10" ,    &nn_3c_4200w4k0_l10 );
  AddBranch("nn_3c_4200w4k1_l10" ,    &nn_3c_4200w4k1_l10 );
  AddBranch("nn_3c_4200w4k2_l10" ,    &nn_3c_4200w4k2_l10 );
  AddBranch("nn_3c_4200w4k3_l10" ,    &nn_3c_4200w4k3_l10 );
  AddBranch("nn_3c_4200w4kfold_l10" ,    &nn_3c_4200w4kfold_l10 );
  AddBranch("nn_3c_4200w4kmerge_l10" ,    &nn_3c_4200w4kmerge_l10 );
  AddBranch("nn_3c_4200w2k0_e1000_l10" ,    &nn_3c_4200w2k0_e1000_l10 );
  AddBranch("nn_3c_4200w2k1_e1000_l10" ,    &nn_3c_4200w2k1_e1000_l10 );
  AddBranch("nn_3c_4200w2kfold_e1000_l10" ,    &nn_3c_4200w2kfold_e1000_l10 );
  AddBranch("nn_3c_4200w2kmerge_e1000_l10" ,    &nn_3c_4200w2kmerge_e1000_l10 );


  //AddBranch("nn_3c_dataM800_e100_l0" ,    &nn_3c_dataM800_e100_l0 );
  //AddBranch("nn_3c_dataM800_e100_l10" ,    &nn_3c_dataM800_e100_l10 );
  AddBranch("nJets20pt"  ,     &nJets20pt);
  AddBranch("eventWeightforwardSF__1up",     &EventWeightforwardSF__1up);
  AddBranch("eventWeightforwardSF__1down",     &EventWeightforwardSF__1down);
  AddBranch("eventWeightcentralSF__1up",     &EventWeightcentralSF__1up);
  AddBranch("eventWeightcentralSF__1down",     &EventWeightcentralSF__1down);
  AddBranch("eventWeightB2SF__1up",     &EventWeightB2SF__1up);
  AddBranch("eventWeightB2SF__1down",     &EventWeightB2SF__1down);
  AddBranch("eventWeightBTrig_Eta_EFF__1up",     &EventWeightBTrig_Eta_EFF__1up);
  AddBranch("eventWeightBTrig_Eta_EFF__1down",     &EventWeightBTrig_Eta_EFF__1down);
  AddBranch("eventWeightBTrig_SF__1up",     &EventWeightBTrig_SF__1up);
  AddBranch("eventWeightBTrig_SF__1down",     &EventWeightBTrig_SF__1down);
  AddBranch("eventWeightShower_mBB__1up",     &EventWeightShower_mBB__1up);
  AddBranch("eventWeightShower_mBB__1down",     &EventWeightShower_mBB__1down);
  AddBranch("eventWeightShower_nJets30pt__1up",     &EventWeightShower_nJets30pt__1up);
  AddBranch("eventWeightShower_nJets30pt__1down",     &EventWeightShower_nJets30pt__1down);

  if (!m_onlyFitInputs) {
  if (m_isMC) {
    AddBranch("TruthLabelB1",&TruthLabelB1);
    AddBranch("TruthLabelB2",&TruthLabelB2);
    AddBranch("TruthLabelPartonB1",&TruthLabelPartonB1);
    AddBranch("TruthLabelPartonB2",&TruthLabelPartonB2);
    AddBranch("HadronConeExclTruthLabelB1", &HadronConeExclTruthLabelB1);
    AddBranch("HadronConeExclTruthLabelB2", &HadronConeExclTruthLabelB2);


  AddBranch("eventWeightFT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_extrapolation_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_extrapolation_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_extrapolation_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_extrapolation_AntiKt4EMPFlowJets__1up);
  AddBranch("eventWeightFT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1down", &EventWeightFT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1down);
  AddBranch("eventWeightFT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1up", &EventWeightFT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1up);

  AddBranch("eventWeightalpha_s__1up",     &EventWeightalpha_s__1up);
  AddBranch("eventWeightalpha_s__1down",     &EventWeightalpha_s__1down);
  AddBranch("eventWeightewkCorrection__1up",     &EventWeightewkCorrection__1up);
  AddBranch("eventWeightewkCorrection__1down",     &EventWeightewkCorrection__1down);
  AddBranch("eventWeightewkCorrectionNorm__1up",     &EventWeightewkCorrectionNorm__1up);
  AddBranch("eventWeightewkCorrectionNorm__1down",     &EventWeightewkCorrectionNorm__1down);
  AddBranch("eventWeightBR_bb__1up",     &EventWeightBR_bb__1up);
  AddBranch("eventWeightBR_bb__1down",     &EventWeightBR_bb__1down);
  AddBranch("eventWeightmuR_0p5_muF_0p5",     &EventWeightmuR_0p5_muF_0p5);
  AddBranch("eventWeightmuRmuF__1down",     &EventWeightmuRmuF__1down);
  AddBranch("eventWeightmuR_0p5_muF_1p0",     &EventWeightmuR_0p5_muF_1p0);
  AddBranch("eventWeightmuR_0p5_muF_2p0",     &EventWeightmuR_0p5_muF_2p0);
  AddBranch("eventWeightmuR_1p0_muF_0p5",     &EventWeightmuR_1p0_muF_0p5);
  AddBranch("eventWeightmuR_1p0_muF_1p0",     &EventWeightmuR_1p0_muF_1p0);
  AddBranch("eventWeightmuR_1p0_muF_2p0",     &EventWeightmuR_1p0_muF_2p0);
  AddBranch("eventWeightmuR_2p0_muF_0p5",     &EventWeightmuR_2p0_muF_0p5);
  AddBranch("eventWeightmuR_2p0_muF_1p0",     &EventWeightmuR_2p0_muF_1p0);
  AddBranch("eventWeightmuR_2p0_muF_2p0",     &EventWeightmuR_2p0_muF_2p0);
  AddBranch("eventWeightmuRmuF__1up",     &EventWeightmuRmuF__1up);
  AddBranch("eventWeightPDF_VBF__1up",     &EventWeightPDF_VBF__1up);
  AddBranch("eventWeightPDF_VBF__1down",     &EventWeightPDF_VBF__1down);
  AddBranch("eventWeightQCDscale_VBF__1up",     &EventWeightQCDscale_VBF__1up);
  AddBranch("eventWeightQCDscale_VBF__1down",     &EventWeightQCDscale_VBF__1down);
  AddBranch("eventWeightPDF_ggF__1up",     &EventWeightPDF_ggF__1up);
  AddBranch("eventWeightPDF_ggF__1down",     &EventWeightPDF_ggF__1down);
  AddBranch("eventWeightQCDscale_ggF_mu__1up",     &EventWeightQCDscale_ggF_mu__1up);
  AddBranch("eventWeightQCDscale_ggF_mu__1down",     &EventWeightQCDscale_ggF_mu__1down);
  AddBranch("eventWeightQCDscale_ggF_res__1up",     &EventWeightQCDscale_ggF_res__1up);
  AddBranch("eventWeightQCDscale_ggF_res__1down",     &EventWeightQCDscale_ggF_res__1down);
  AddBranch("eventWeightQCDscale_ggF_mig01__1up",     &EventWeightQCDscale_ggF_mig01__1up);
  AddBranch("eventWeightQCDscale_ggF_mig01__1down",     &EventWeightQCDscale_ggF_mig01__1down);
  AddBranch("eventWeightQCDscale_ggF_mig12__1up",     &EventWeightQCDscale_ggF_mig12__1up);
  AddBranch("eventWeightQCDscale_ggF_mig12__1down",     &EventWeightQCDscale_ggF_mig12__1down);
  AddBranch("eventWeightQCDscale_ggF_pTH60__1up",     &EventWeightQCDscale_ggF_pTH60__1up);
  AddBranch("eventWeightQCDscale_ggF_pTH60__1down",     &EventWeightQCDscale_ggF_pTH60__1down);
  AddBranch("eventWeightQCDscale_ggF_pTH120__1up",     &EventWeightQCDscale_ggF_pTH120__1up);
  AddBranch("eventWeightQCDscale_ggF_pTH120__1down",     &EventWeightQCDscale_ggF_pTH120__1down);
  AddBranch("eventWeightQCDscale_ggF_qm_t__1up",     &EventWeightQCDscale_ggF_qm_t__1up);
  AddBranch("eventWeightQCDscale_ggF_qm_t__1down",     &EventWeightQCDscale_ggF_qm_t__1down);
  AddBranch("eventWeightQCDscale_ggF_vbf2j__1up",     &EventWeightQCDscale_ggF_vbf2j__1up);
  AddBranch("eventWeightQCDscale_ggF_vbf2j__1down",     &EventWeightQCDscale_ggF_vbf2j__1down);
  AddBranch("eventWeightQCDscale_ggF_vbf3j__1up",     &EventWeightQCDscale_ggF_vbf3j__1up);
  AddBranch("eventWeightQCDscale_ggF_vbf3j__1down",     &EventWeightQCDscale_ggF_vbf3j__1down);
  AddBranch("eventWeightQCDscale_ggF__1up",     &EventWeightQCDscale_ggF__1up);
  AddBranch("eventWeightQCDscale_ggF__1down",     &EventWeightQCDscale_ggF__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV1__1up",     &EventWeightPDF4LHC_NLO_30_EV1__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV1__1down",     &EventWeightPDF4LHC_NLO_30_EV1__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV2__1up",     &EventWeightPDF4LHC_NLO_30_EV2__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV2__1down",     &EventWeightPDF4LHC_NLO_30_EV2__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV3__1up",     &EventWeightPDF4LHC_NLO_30_EV3__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV3__1down",     &EventWeightPDF4LHC_NLO_30_EV3__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV4__1up",     &EventWeightPDF4LHC_NLO_30_EV4__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV4__1down",     &EventWeightPDF4LHC_NLO_30_EV4__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV5__1up",     &EventWeightPDF4LHC_NLO_30_EV5__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV5__1down",     &EventWeightPDF4LHC_NLO_30_EV5__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV6__1up",     &EventWeightPDF4LHC_NLO_30_EV6__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV6__1down",     &EventWeightPDF4LHC_NLO_30_EV6__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV7__1up",     &EventWeightPDF4LHC_NLO_30_EV7__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV7__1down",     &EventWeightPDF4LHC_NLO_30_EV7__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV8__1up",     &EventWeightPDF4LHC_NLO_30_EV8__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV8__1down",     &EventWeightPDF4LHC_NLO_30_EV8__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV9__1up",     &EventWeightPDF4LHC_NLO_30_EV9__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV9__1down",     &EventWeightPDF4LHC_NLO_30_EV9__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV10__1up",     &EventWeightPDF4LHC_NLO_30_EV10__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV10__1down",     &EventWeightPDF4LHC_NLO_30_EV10__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV11__1up",     &EventWeightPDF4LHC_NLO_30_EV11__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV11__1down",     &EventWeightPDF4LHC_NLO_30_EV11__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV12__1up",     &EventWeightPDF4LHC_NLO_30_EV12__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV12__1down",     &EventWeightPDF4LHC_NLO_30_EV12__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV13__1up",     &EventWeightPDF4LHC_NLO_30_EV13__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV13__1down",     &EventWeightPDF4LHC_NLO_30_EV13__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV14__1up",     &EventWeightPDF4LHC_NLO_30_EV14__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV14__1down",     &EventWeightPDF4LHC_NLO_30_EV14__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV15__1up",     &EventWeightPDF4LHC_NLO_30_EV15__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV15__1down",     &EventWeightPDF4LHC_NLO_30_EV15__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV16__1up",     &EventWeightPDF4LHC_NLO_30_EV16__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV16__1down",     &EventWeightPDF4LHC_NLO_30_EV16__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV17__1up",     &EventWeightPDF4LHC_NLO_30_EV17__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV17__1down",     &EventWeightPDF4LHC_NLO_30_EV17__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV18__1up",     &EventWeightPDF4LHC_NLO_30_EV18__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV18__1down",     &EventWeightPDF4LHC_NLO_30_EV18__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV19__1up",     &EventWeightPDF4LHC_NLO_30_EV19__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV19__1down",     &EventWeightPDF4LHC_NLO_30_EV19__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV20__1up",     &EventWeightPDF4LHC_NLO_30_EV20__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV20__1down",     &EventWeightPDF4LHC_NLO_30_EV20__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV21__1up",     &EventWeightPDF4LHC_NLO_30_EV21__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV21__1down",     &EventWeightPDF4LHC_NLO_30_EV21__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV22__1up",     &EventWeightPDF4LHC_NLO_30_EV22__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV22__1down",     &EventWeightPDF4LHC_NLO_30_EV22__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV23__1up",     &EventWeightPDF4LHC_NLO_30_EV23__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV23__1down",     &EventWeightPDF4LHC_NLO_30_EV23__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV24__1up",     &EventWeightPDF4LHC_NLO_30_EV24__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV24__1down",     &EventWeightPDF4LHC_NLO_30_EV24__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV25__1up",     &EventWeightPDF4LHC_NLO_30_EV25__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV25__1down",     &EventWeightPDF4LHC_NLO_30_EV25__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV26__1up",     &EventWeightPDF4LHC_NLO_30_EV26__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV26__1down",     &EventWeightPDF4LHC_NLO_30_EV26__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV27__1up",     &EventWeightPDF4LHC_NLO_30_EV27__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV27__1down",     &EventWeightPDF4LHC_NLO_30_EV27__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV28__1up",     &EventWeightPDF4LHC_NLO_30_EV28__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV28__1down",     &EventWeightPDF4LHC_NLO_30_EV28__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV29__1up",     &EventWeightPDF4LHC_NLO_30_EV29__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV29__1down",     &EventWeightPDF4LHC_NLO_30_EV29__1down);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV30__1up",     &EventWeightPDF4LHC_NLO_30_EV30__1up);
  AddBranch("eventWeightPDF4LHC_NLO_30_EV30__1down",     &EventWeightPDF4LHC_NLO_30_EV30__1down);

    //AddBranch("TE_Q"       ,     &TE_Q);
    //AddBranch("TE_X1"       ,     &TE_X1);
    //AddBranch("TE_X2"       ,     &TE_X2);
    //AddBranch("TE_XF1"       ,     &TE_XF1);
    //AddBranch("TE_XF2"       ,     &TE_XF2);
    AddBranch("hpt",     &hpt);
    AddBranch("heta",     &heta);
    AddBranch("hy",     &hy);
    AddBranch("mJJ_truth",     &mJJ_truth);
    AddBranch("mJJ_truth_new",     &mJJ_truth_new);
    AddBranch("pTHJJ_truth_new",     &pTHJJ_truth_new);
    AddBranch("mJJ_truth_abs",     &mJJ_truth_abs);
    AddBranch("dEtaJJ_truth",     &dEtaJJ_truth);
    AddBranch("dEtaJJ_truth_abs",     &dEtaJJ_truth_abs);
    AddBranch("nJets_truth",     &nJets_truth);
    AddBranch("HTXS_Higgs_pt",     &HTXS_Higgs_pt);
    AddBranch("HTXS_Higgs_eta",     &HTXS_Higgs_eta);
    AddBranch("HTXS_Higgs_phi",     &HTXS_Higgs_phi);
    AddBranch("HTXS_Higgs_m",     &HTXS_Higgs_m);
    AddBranch("HTXS_Njets_pTjet25",     &HTXS_Njets_pTjet25);
    AddBranch("HTXS_Njets_pTjet30",     &HTXS_Njets_pTjet30);
    AddBranch("HTXS_Stage1_1_Category_pTjet25",     &HTXS_Stage1_1_Category_pTjet25);
    AddBranch("HTXS_Stage1_1_Category_pTjet30",     &HTXS_Stage1_1_Category_pTjet30);
    AddBranch("HTXS_Stage1_1_FineIndex_pTjet25",     &HTXS_Stage1_1_FineIndex_pTjet25);
    AddBranch("HTXS_Stage1_1_FineIndex_pTjet30",     &HTXS_Stage1_1_FineIndex_pTjet30);
    AddBranch("HTXS_Stage1_1_Fine_Category_pTjet25",     &HTXS_Stage1_1_Fine_Category_pTjet25);
    AddBranch("HTXS_Stage1_1_Fine_Category_pTjet30",     &HTXS_Stage1_1_Fine_Category_pTjet30);
    AddBranch("HTXS_Stage1_1_Fine_FineIndex_pTjet25",     &HTXS_Stage1_1_Fine_FineIndex_pTjet25);
    AddBranch("HTXS_Stage1_1_Fine_FineIndex_pTjet30",     &HTXS_Stage1_1_Fine_FineIndex_pTjet30);
    AddBranch("HTXS_Stage1_Category_pTjet25",     &HTXS_Stage1_Category_pTjet25);
    AddBranch("HTXS_Stage1_Category_pTjet30",     &HTXS_Stage1_Category_pTjet30);
    AddBranch("HTXS_Stage1_FineIndex_pTjet25",     &HTXS_Stage1_FineIndex_pTjet25);
    AddBranch("HTXS_Stage1_FineIndex_pTjet30",     &HTXS_Stage1_FineIndex_pTjet30);
    AddBranch("STXS_bin",     &STXS_bin);
    AddBranch("hphi",     &hphi);
    AddBranch("hm",     &hm);
    //AddBranch("weightSysts",     &weightSysts);
    //AddBranch("weightspdf4lhc",    &weightspdf4lhc);
    //AddBranch("weightspdfnnpdf30", &weightspdfnnpdf30);
    //AddBranch("weightsqcdnnlops",  &weightsqcdnnlops);
    //AddBranch("weightsqcdnnlops2np",  &weightsqcdnnlops2np);
    //AddBranch("weightsqcdwg1",     &weightsqcdwg1);
    //AddBranch("weightsalternativepdf",  &weightsalternativepdf);
    AddBranch("mcWeight"   ,     &MCWeight);
    AddBranch("puWeight"   ,     &PUWeight);
    AddBranch("batman"   ,     &batman);
    AddBranch("BJetSF", &BJetSF);
    AddBranch("alpha_s_up", &alpha_s_up);
    AddBranch("alpha_s_dn", &alpha_s_dn);
    AddBranch("mcChannelNumber", &MCChannelNumber);
    AddBranch("ewkWeight", &ewkWeight);
    AddBranch("ewkWeightOff", &ewkWeightOff);
  }
  AddBranch("BJetTriggerWeight", &BJetTriggerWeight);

  //AddBranch("nJets20pt_central"  ,     &nJets20pt_central);
  //AddBranch("nJets20pt_centraljvt"  ,     &nJets20pt_centraljvt);
  //AddBranch("nJets20pt_rap"  ,     &nJets20pt_rap);
  //AddBranch("nn_nunez26" ,    &nn_nunez26 );
  //AddBranch("nn_RE29" ,    &nn_RE29 );
  //AddBranch("nn_RE34" ,    &nn_RE34 );
  //AddBranch("nJets20pt_fsrmin08"  ,     &nJets20pt_fsrmin08);
  //AddBranch("nJets20pt_fsrmin10"  ,     &nJets20pt_fsrmin10);
  //AddBranch("nJets20pt_fsrmin12"  ,     &nJets20pt_fsrmin12);
/*
  AddBranch("nJets20pt_fsr10"  ,     &nJets20pt_fsr10);
  AddBranch("nJets20pt_fsr12"  ,     &nJets20pt_fsr12);
  AddBranch("nJets20pt_fsr14"  ,     &nJets20pt_fsr14);
  AddBranch("nJets20pt_fsr16"  ,     &nJets20pt_fsr16);
  AddBranch("nJets20pt_fsr18"  ,     &nJets20pt_fsr18);
  AddBranch("nJets20pt_fsr20"  ,     &nJets20pt_fsr20);
*/

  //AddBranch("nn_adv_e100_l0" ,    &nn_adv_e100_l0 );

    AddBranch("runNumber"  ,     &RunNumber);
    AddBranch("lumiBlock"  ,     &LumiBlock);
    AddBranch("eventNumber",     &EventNumber);
    AddBranch("npv",     &npv);
    AddBranch("ActualMu",     &ActualMu);
    AddBranch("AverageMu",     &AverageMu);
    AddBranch("ActualMuScaled",     &ActualMuScaled);
    AddBranch("AverageMuScaled",     &AverageMuScaled);
    AddBranch("AverageMuRaw",     &AverageMuRaw);
    AddBranch("ActualMuRaw",     &ActualMuRaw);
    AddBranch("embMu1pt",     &embMu1pt);
    AddBranch("embMu1eta",     &embMu1eta);
    AddBranch("embMu1phi",     &embMu1phi);
    AddBranch("embMu1ptvarcone30",     &embMu1ptvarcone30);
    //AddBranch("embMu1topoetcone20",     &embMu1topoetcone20);
    AddBranch("embMu2pt",     &embMu2pt);
    //AddBranch("embMu2eta",     &embMu2eta);
    //AddBranch("embMu2phi",     &embMu2phi);
    AddBranch("embMu2ptvarcone30",     &embMu2ptvarcone30);
    //AddBranch("embMu2topoetcone20",     &embMu2topoetcone20);
    AddBranch("embZpt",     &embZpt);
    //AddBranch("embZeta",     &embZeta);
    //AddBranch("embZphi",     &embZphi);
    AddBranch("embMuonWeight_cvmfs",     &embMuonWeight_cvmfs);
    AddBranch("embMuonTrigWeight_cvmfs",     &embMuonTrigWeight_cvmfs);
    AddBranch("embMuonTrigWeight_cvmfs_14",     &embMuonTrigWeight_cvmfs_14);
    AddBranch("embMuonTrigWeight_cvmfs_50",     &embMuonTrigWeight_cvmfs_50);
    AddBranch("embMet",     &embMet);
    AddBranch("embMuonWeight",     &embMuonWeight);
    AddBranch("embMuonTrigWeight",     &embMuonTrigWeight);
    //AddBranch("nJets"      ,     &nJets);
    //AddBranch("nJets30pt"  ,     &nJets30pt);
    //AddBranch("nJets30pt_central"  ,     &nJets30pt_central);
    //AddBranch("nJets30pt_rap"  ,     &nJets30pt_rap);
    AddBranch("nElectrons"  ,     &nElectrons);
    AddBranch("nMuons"  ,     &nMuons);
    AddBranch("nPhotons"  ,     &nPhotons);
    //AddBranch("met"  ,     &met);
    //AddBranch("metsoft"  ,     &metsoft);
    //AddBranch("metjet"  ,     &metjet);
    //AddBranch("sumsoft"  ,     &sumsoft);

   // AddBranch("nTightEmbMv2c10",    &nTightEmbMv2c10);
    //AddBranch("nMediumEmbMv2c10",   &nMediumEmbMv2c10);
    //AddBranch("nLooseEmbMv2c10",    &nLooseEmbMv2c10);

    AddBranch("nTightMv2c10",    &nTightMv2c10);
    AddBranch("nMediumMv2c10",   &nMediumMv2c10);
    AddBranch("nLooseMv2c10",    &nLooseMv2c10);
/*
    AddBranch("pT"         ,     &pT);
    AddBranch("phi"        ,     &phi);
    AddBranch("eta"        ,     &eta);
    AddBranch("mv2c10"     ,     &mv2c10);
    AddBranch("HLTJetMatchedPt"     ,     &HLTJetMatchedPt);
    AddBranch("L1JetMatchedPt"     ,     &L1JetMatchedPt);
    AddBranch("L1JetMatchedEta"     ,     &L1JetMatchedEta);
    AddBranch("L1JetMatchedPhi"     ,     &L1JetMatchedPhi);
    AddBranch("TruthJetMatchedPt"     ,     &TruthJetMatchedPt);
    AddBranch("TruthJetMatchedEta"     ,     &TruthJetMatchedEta);
    AddBranch("mv2c20"     ,     &mv2c20);

    AddBranch("truthLabel" ,     &truthLabelID);
    AddBranch("coneTruthLabelID",&coneTruthLabelID);
    AddBranch("hadronConeExclTruthLabelID",&hadronConeExclTruthLabelID);
    AddBranch("partonTruthLabelID" ,     &partonTruthLabelID);



    AddBranch("whoIsJ1"    ,     &whoIsJ1);
    AddBranch("whoIsJ2"    ,     &whoIsJ2);
    AddBranch("whoIsB1"    ,     &whoIsB1);
    AddBranch("whoIsB2"    ,     &whoIsB2);
*/

    AddBranch("mBB_no_corr",     &mBB_no_corr);
    AddBranch("mBB_onlymu",     &mBB_onlymu);
    AddBranch("mBB_truth",     &mBB_truth);
    AddBranch("eventWeightHerwigMbb",     &eventWeightHerwigMbb);
/*
    AddBranch("mBB_fsrmin08",     &mBB_fsrmin08);
    AddBranch("mBB_fsrmin10",     &mBB_fsrmin10);
    AddBranch("mBB_fsrmin12",     &mBB_fsrmin12);
*/
/*
    AddBranch("mBB_fsr10",     &mBB_fsr10);
    AddBranch("mBB_fsr12",     &mBB_fsr12);
    AddBranch("mBB_fsr14",     &mBB_fsr14);
    AddBranch("mBB_fsr16",     &mBB_fsr16);
    AddBranch("mBB_fsr18",     &mBB_fsr18);
    AddBranch("mBB_fsr20",     &mBB_fsr20);
*/
    //AddBranch("dPhiBB"     ,     &dPhiBB);
    //AddBranch("dEtaBB"     ,     &dEtaBB);
    AddBranch("pTBB_no_corr",    &pTBB_no_corr);
    AddBranch("pTBB_onlymu",    &pTBB_onlymu);
/*
    AddBranch("pTBB_fsrmin08",     &pTBB_fsrmin08);
    AddBranch("pTBB_fsrmin10",     &pTBB_fsrmin10);
    AddBranch("pTBB_fsrmin12",     &pTBB_fsrmin12);
*/
/*
    AddBranch("pTBB_fsr10",     &pTBB_fsr10);
    AddBranch("pTBB_fsr12",     &pTBB_fsr12);
    AddBranch("pTBB_fsr14",     &pTBB_fsr14);
    AddBranch("pTBB_fsr16",     &pTBB_fsr16);
    AddBranch("pTBB_fsr18",     &pTBB_fsr18);
    AddBranch("pTBB_fsr20",     &pTBB_fsr20);
*/
    //AddBranch("EtaBB"       ,    &EtaBB);
    //AddBranch("PhiBB"       ,    &PhiBB);
    //AddBranch("EtaBB_no_corr",   &EtaBB_no_corr);
    //AddBranch("EtaBB_onlymu",   &EtaBB_onlymu);
    //AddBranch("PhiBB_no_corr",   &PhiBB_no_corr);
    //AddBranch("PhiBB_onlymu",   &PhiBB_onlymu);

    AddBranch("mJJ"        ,     &mJJ);
    AddBranch("mJJ_base"        ,     &mJJ_base);
    AddBranch("etaJ1_base"        ,     &etaJ1_base);
    AddBranch("etaJ2_base"        ,     &etaJ2_base);
    //AddBranch("mJJJ"        ,     &mJJJ);
    AddBranch("deltaMJJ"      ,  &deltaMJJ);
    //AddBranch("deltaMJJ2"      ,  &deltaMJJ2);
    //AddBranch("deltaMJJJ"      ,  &deltaMJJJ);
    //AddBranch("deltaMJJ_pTBB_nomBB"      ,  &deltaMJJ_pTBB_nomBB);
    //AddBranch("deltaMJJ_pTBB"      ,  &deltaMJJ_pTBB);
    //AddBranch("dRJJ"       ,     &dRJJ);
    AddBranch("dPhiJJ"     ,     &dPhiJJ);
    AddBranch("dPhiBBJJ"     ,     &dPhiBBJJ);
    AddBranch("dEtaJJ"     ,     &dEtaJJ);
    AddBranch("dEtaBBJJ"     ,     &dEtaBBJJ);
    //AddBranch("EtaJJ"     ,     &EtaJJ);
    //AddBranch("PhiJJ"     ,     &PhiJJ);
    AddBranch("pTJJ"       ,     &pTJJ);
    //AddBranch("pTJJJ"       ,     &pTJJJ);

  // ====                                                                                                                                                               

  //   AddBranch("passCURRENT",     &passCURRENT);
  //   AddBranch("passATLAS",     &passATLAS);
  //   AddBranch("passCMS",     &passCMS);
  //   AddBranch("passHYBRID",     &passHYBRID);

  //   AddBranch("whoIsB1_CURRENT" , &whoIsB1_CURRENT);
  //   AddBranch("whoIsB2_CURRENT" , &whoIsB2_CURRENT);
  //   AddBranch("whoIsB1_ATLAS", &whoIsB1_ATLAS);
  //   AddBranch("whoIsB2_ATLAS", &whoIsB2_ATLAS);
  //   AddBranch("whoIsB1_CMS", &whoIsB1_CMS );
  //   AddBranch("whoIsB2_CMS", &whoIsB2_CMS );
  //   AddBranch("whoIsB1_HYBRID", &whoIsB1_HYBRID );
  //   AddBranch("whoIsB2_HYBRID", &whoIsB2_HYBRID );

  // =====   

    AddBranch("pTJ1",&pTJ1);
    AddBranch("mJ1",&mJ1);
    //AddBranch("pTJ1J",&pTJ1J);
    AddBranch("pTJ2",&pTJ2);
    AddBranch("mJ2",&mJ2);
    //AddBranch("pTJ2J",&pTJ2J);
    AddBranch("forwardSFJ1",&forwardSFJ1);
    AddBranch("forwardSFJ2",&forwardSFJ2);
    AddBranch("forwardSF",&forwardSF);
    AddBranch("centralSFJ1",&centralSFJ1);
    AddBranch("centralSFJ2",&centralSFJ2);
    AddBranch("centralSF",&centralSF);
    AddBranch("B2SF",&B2SF);
    AddBranch("pTB1",&pTB1);
    AddBranch("mB1",&mB1);
    //AddBranch("pTB1J",&pTB1J);
    AddBranch("pTB2",&pTB2);
    AddBranch("mB2",&mB2);
    //AddBranch("pTB2J",&pTB2J);
    AddBranch("pTB1_no_corr",&pTB1_no_corr);
    AddBranch("pTB2_no_corr",&pTB2_no_corr);
    AddBranch("pTB1_onlymu",&pTB1_onlymu);
    AddBranch("pTB2_onlymu",&pTB2_onlymu);
    AddBranch("HLTB1",&HLTB1);
    AddBranch("HLTB2",&HLTB2);
    AddBranch("HLTJ1",&HLTJ1);
    AddBranch("HLTJ2",&HLTJ2);
    AddBranch("L1B1",&L1B1);
    AddBranch("L1B2",&L1B2);
    AddBranch("L1J1",&L1J1);
    AddBranch("L1J2",&L1J2);

    AddBranch("etaJ1",&etaJ1);
    AddBranch("etaJ2",&etaJ2);
    AddBranch("etaB1",&etaB1);
    AddBranch("etaB2",&etaB2);
    //AddBranch("etaB1_no_corr",&etaB1_no_corr);
    //AddBranch("etaB2_no_corr",&etaB2_no_corr);
    //AddBranch("etaB1_onlymu",&etaB1_onlymu);
    //AddBranch("etaB2_onlymu",&etaB2_onlymu);

    /*AddBranch("J1FJVTLoose",&J1FJVTLoose);
    AddBranch("J2FJVTLoose",&J2FJVTLoose);
    AddBranch("J1FJVTTight",&J1FJVTTight);
    AddBranch("J2FJVTTight",&J2FJVTTight);
    AddBranch("J1Timing",&J1Timing);
    AddBranch("J2Timing",&J2Timing);
    AddBranch("J1JVT",&J1JVT);
    AddBranch("J2JVT",&J2JVT);
    AddBranch("J1FJVT",&J1FJVT);
    AddBranch("J2FJVT",&J2FJVT);*/

    //AddBranch("dRB1J1",&dRB1J1);
    //AddBranch("dRB1J2",&dRB1J2);
    //AddBranch("dRB2J1",&dRB2J1);
    //AddBranch("dRB2J2",&dRB2J2);

    AddBranch("mindRJ1_Ex",&mindRJ1_Ex);
    AddBranch("mindRJ1_Ex_base",&mindRJ1_Ex_base);
    //AddBranch("mindRJ1_Ex_closest",&mindRJ1_Ex_closest);
    //AddBranch("mindRJ1_Ex_dRB1",&mindRJ1_Ex_dRB1);
    //AddBranch("mindRJ1_ExPt",&mindRJ1_ExPt);
    //AddBranch("mindRJ1_ExPt_closest",&mindRJ1_ExPt_closest);
    //AddBranch("mindRJ1_ExPt_dRB1",&mindRJ1_ExPt_dRB1);
    AddBranch("mindRJ2_Ex",&mindRJ2_Ex);
    AddBranch("mindRJ2_Ex_base",&mindRJ2_Ex_base);
    //AddBranch("mindRJ2_Ex_closest",&mindRJ2_Ex_closest);
    //AddBranch("mindRJ2_Ex_dRB1",&mindRJ2_Ex_dRB1);
    //AddBranch("mindRJ2_ExPt",&mindRJ2_ExPt);
    //AddBranch("mindRJ2_ExPt_closest",&mindRJ2_ExPt_closest);
    //AddBranch("mindRJ2_ExPt_dRB1",&mindRJ2_ExPt_dRB1);
    //AddBranch("mindRB1_Ex",&mindRB1_Ex);
    //AddBranch("mindRB2_Ex",&mindRB2_Ex);
/*
*/

    AddBranch("MV2c10B1",&MV2c10B1);
    AddBranch("MV2c10B2",&MV2c10B2);
    AddBranch("MV2c10J1",&MV2c10J1);
    AddBranch("MV2c10J2",&MV2c10J2);

    AddBranch("NTrk500PVJ1",&NTrk500PVJ1);
    AddBranch("NTrk500PVJ2",&NTrk500PVJ2);

    //AddBranch("QGTagger_NTracksJ1",&QGTagger_NTracksJ1);
    //AddBranch("QGTagger_NTracksJ2",&QGTagger_NTracksJ2);
    //AddBranch("QGTagger_TracksWidthJ1",&QGTagger_TracksWidthJ1);
    //AddBranch("QGTagger_TracksWidthJ2",&QGTagger_TracksWidthJ2);
    //AddBranch("QGTagger_TracksC1J1",&QGTagger_TracksC1J1);
    //AddBranch("QGTagger_TracksC1J2",&QGTagger_TracksC1J2);
    AddBranch("QGTaggerWeightJ1", &QGTaggerWeightJ1);
    AddBranch("QGTaggerWeightJ2", &QGTaggerWeightJ2);


    AddBranch("pT_balance" ,    &pT_balance );
    //AddBranch("pT_balanceJ" ,    &pT_balanceJ );
    //AddBranch("pT_balance_pTBB" ,    &pT_balance_pTBB );
    /*
    AddBranch("pT_balance_fsrmin08",     &pT_balance_fsrmin08);
    AddBranch("pT_balance_fsrmin10",     &pT_balance_fsrmin10);
    AddBranch("pT_balance_fsrmin12",     &pT_balance_fsrmin12);
*/
/*
    AddBranch("pT_balance_fsr10",     &pT_balance_fsr10);
    AddBranch("pT_balance_fsr12",     &pT_balance_fsr12);
    AddBranch("pT_balance_fsr14",     &pT_balance_fsr14);
    AddBranch("pT_balance_fsr16",     &pT_balance_fsr16);
    AddBranch("pT_balance_fsr18",     &pT_balance_fsr18);
    AddBranch("pT_balance_fsr20",     &pT_balance_fsr20);
    AddBranch("hframe_cosTheta_h_b1" ,    &hframe_cosTheta_h_b1 );
    AddBranch("bdt2cenWeight" ,    &bdt2cenWeight );
    AddBranch("bdt3cen" ,    &bdt3cen );
    AddBranch("bdtHighpt" ,    &bdtHighpt );
    AddBranch("bdttopoInc" ,    &bdttopoInc );
    AddBranch("bdt2cenOld" ,    &bdt2cenOld );
*/

  // ======                                                                    

    AddBranch("max_J1J2" ,     &max_J1J2);
    AddBranch("eta_J_star" ,     &eta_J_star);
    //AddBranch("cosTheta_MVA",    &cosTheta_MVA);
    //AddBranch("cosTheta",    &cosTheta_CMS);
    AddBranch("cosTheta_boost",    &cosTheta_boost);
    //AddBranch("HT_soft"    ,     &HT_soft);
    AddBranch("centralMaxJ_Ex"    ,     &centralMaxJ_Ex);
    //AddBranch("angleAsymBB"    ,     &angleAsymBB);
    AddBranch("mBB_angle"    ,     &mBB_angle);
    AddBranch("asymJJ"    ,     &asymJJ);
    //AddBranch("nonforwardMaxJ_Ex"    ,     &nonforwardMaxJ_Ex);
    //AddBranch("inclusiveMaxJ_Ex"    ,     &inclusiveMaxJ_Ex);
    //AddBranch("centralMaxJ_Ex_dRB1"    ,     &centralMaxJ_Ex_dRB1);
    //AddBranch("nonforwardMaxJ_Ex_dRB1"    ,     &nonforwardMaxJ_Ex_dRB1);
    //AddBranch("inclusiveMaxJ_Ex_dRB1"    ,     &inclusiveMaxJ_Ex_dRB1);
    //AddBranch("centralMaxJ_zeppenfeld"    ,     &centralMaxJ_zeppenfeld);
    //AddBranch("nonforwardMaxJ_zeppenfeld"    ,     &nonforwardMaxJ_zeppenfeld);
    //AddBranch("inclusiveMaxJ_zeppenfeld"    ,     &inclusiveMaxJ_zeppenfeld);
    //AddBranch("centralMaxJ_eta"    ,     &centralMaxJ_eta);
    //AddBranch("nonforwardMaxJ_eta"    ,     &nonforwardMaxJ_eta);
    //AddBranch("inclusiveMaxJ_eta"    ,     &inclusiveMaxJ_eta);
    //AddBranch("centralMaxJ_NTrk500PV"    ,     &centralMaxJ_NTrk500PV);
    //AddBranch("nonforwardMaxJ_NTrk500PV"    ,     &nonforwardMaxJ_NTrk500PV);
    //AddBranch("inclusiveMaxJ_NTrk500PV"    ,     &inclusiveMaxJ_NTrk500PV);
    //AddBranch("centralMaxJ_dEtaJ1"    ,     &centralMaxJ_dEtaJ1);
    //AddBranch("nonforwardMaxJ_dEtaJ1"    ,     &nonforwardMaxJ_dEtaJ1);
    //AddBranch("inclusiveMaxJ_dEtaJ1"    ,     &inclusiveMaxJ_dEtaJ1);
    //AddBranch("HT_MVA"    ,     &HT_MVA);
  //    AddBranch("BDT"        ,     &BDT);
    AddBranch("passHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490", &passHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490);
    AddBranch("passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490", &passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490);
    AddBranch("passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30", &passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30);
    AddBranch("passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30", &passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30);
    AddBranch("passHLT_j100_2j55_bmv2c2060_split", &passHLT_j100_2j55_bmv2c2060_split);
    AddBranch("isActiveHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30", &isActiveHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30);
    AddBranch("isActiveHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30", &isActiveHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30);
    //AddBranch("isActiveHLT_j100_2j55_bmv2c2060_split", &isActiveHLT_j100_2j55_bmv2c2060_split);
    AddBranch("prescaleHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30", &prescaleHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30);
    AddBranch("prescaleHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30", &prescaleHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30);
    //AddBranch("prescaleHLT_j100_2j55_bmv2c2060_split", &prescaleHLT_j100_2j55_bmv2c2060_split);

  // store trigger decisions

/*
   AddBranch("passHLT_2mu14", &passHLT_2mu14);
    AddBranch("passHLT_mu50", &passHLT_mu50);
    AddBranch("passL1_J40_0ETA25_2J25_J20_31ETA49", &passL1_J40_0ETA25_2J25_J20_31ETA49);
    AddBranch("passL1_J40_0ETA25_2J25", &passL1_J40_0ETA25_2J25);
    AddBranch("passL1_J20_31ETA49", &passL1_J20_31ETA49);
    AddBranch("passL1_J15_31ETA49", &passL1_J15_31ETA49);
    AddBranch("passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25", &passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25);
    AddBranch("passHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25", &passHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25);
    AddBranch("passHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25", &passHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25);
    AddBranch("passHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25", &passHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25);
    AddBranch("passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25", &passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25);
    AddBranch("passHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25", &passHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25);
    AddBranch("passHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25", &passHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25);
    AddBranch("passHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25", &passHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25);
    AddBranch("passHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25", &passHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25);
    AddBranch("passHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25", &passHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25);
    AddBranch("passHLT_j175_gsc225_bmv2c1040_split", &passHLT_j175_gsc225_bmv2c1040_split);
    AddBranch("passHLT_j175_bmv2c2040_split", &passHLT_j175_bmv2c2040_split);
    AddBranch("passHLT_j225_gsc275_bmv2c1060_split", &passHLT_j225_gsc275_bmv2c1060_split);
    AddBranch("passHLT_j225_gsc300_bmv2c1070_split", &passHLT_j225_gsc300_bmv2c1070_split);
    AddBranch("passHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split", &passHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split);
    AddBranch("passHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split", &passHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split);
    AddBranch("passHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split", &passHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split);
    AddBranch("passHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49", &passHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49);
    AddBranch("passHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49", &passHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49);
    AddBranch("passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split", &passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split);
    AddBranch("passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split", &passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split);
    AddBranch("passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF", &passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF);
    AddBranch("passHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split", &passHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split);
    AddBranch("passHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF", &passHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF);
    AddBranch("passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split", &passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split);
    AddBranch("passHLT_j45_320eta490", &passHLT_j45_320eta490);
    AddBranch("passHLT_j80_0eta240_j60_j45_320eta490", &passHLT_j80_0eta240_j60_j45_320eta490);
    AddBranch("passHLT_j80_0eta240_j60", &passHLT_j80_0eta240_j60);

    AddBranch("passL1_J40_0ETA25_2J15_31ETA49", &passL1_J40_0ETA25_2J15_31ETA49);
    AddBranch("passL1_J40_0ETA25", &passL1_J40_0ETA25);
    AddBranch("passL1_2J15_31ETA49", &passL1_2J15_31ETA49);
    AddBranch("passHLT_j80_bmv2c2085_split_2j60_320eta490", &passHLT_j80_bmv2c2085_split_2j60_320eta490);
    AddBranch("passHLT_j80_bmv2c2085_split", &passHLT_j80_bmv2c2085_split);
    AddBranch("passHLT_2j60_320eta490", &passHLT_2j60_320eta490);
    AddBranch("passHLT_j80_0eta240_2j60_320eta490", &passHLT_j80_0eta240_2j60_320eta490);
    AddBranch("passHLT_j80_0eta240", &passHLT_j80_0eta240);

    AddBranch("passL1_4J20", &passL1_4J20);
    AddBranch("passHLT_2j55_bmv2c2077_split_2j55", &passHLT_2j55_bmv2c2077_split_2j55);
    AddBranch("passHLT_2j45_bmv2c2070_split_2j45", &passHLT_2j45_bmv2c2070_split_2j45);

    AddBranch("passL1_4J15", &passL1_4J15);
    AddBranch("passHLT_2j35_bmv2c2070_split_2j35_L14J15", &passHLT_2j35_bmv2c2070_split_2j35_L14J15);
    AddBranch("passHLT_2j35_bmv2c2060_split_2j35_L14J15", &passHLT_2j35_bmv2c2060_split_2j35_L14J15);
    AddBranch("passHLT_2j35_bmv2c2050_split_2j35_L14J15", &passHLT_2j35_bmv2c2050_split_2j35_L14J15);
    AddBranch("passHLT_j75_bmv2c2060_split_3j75_L14J15", &passHLT_j75_bmv2c2060_split_3j75_L14J15);
    AddBranch("passHLT_j75_bmv2c2070_split_3j75_L14J15", &passHLT_j75_bmv2c2070_split_3j75_L14J15);

    AddBranch("passHLT_2j45_bmv2c2077_split_2j45_L14J15", &passHLT_2j45_bmv2c2077_split_2j45_L14J15);
    AddBranch("passHLT_2j45_bmv2c2070_split_2j45_L14J15", &passHLT_2j45_bmv2c2070_split_2j45_L14J15);
    AddBranch("passHLT_4j45", &passHLT_4j45);


  // store if trigger was active
    AddBranch("isActiveL1_J40_0ETA25_2J25_J20_31ETA49", &isActiveL1_J40_0ETA25_2J25_J20_31ETA49);
    AddBranch("isActiveHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490", &isActiveHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490);
    AddBranch("isActiveHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25", &isActiveHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25);
    AddBranch("isActiveHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25", &isActiveHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25);
    AddBranch("isActiveHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25", &isActiveHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25);
    AddBranch("isActiveHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25", &isActiveHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25);
    AddBranch("isActiveHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25", &isActiveHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25);
    AddBranch("isActiveHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25", &isActiveHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25);
    AddBranch("isActiveHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25", &isActiveHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25);
    AddBranch("isActiveHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25", &isActiveHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25);
    AddBranch("isActiveHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25", &isActiveHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25);
    AddBranch("isActiveHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25", &isActiveHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25);
    AddBranch("isActiveHLT_j175_gsc225_bmv2c1040_split", &isActiveHLT_j175_gsc225_bmv2c1040_split);
    AddBranch("isActiveHLT_j175_bmv2c2040_split", &isActiveHLT_j175_bmv2c2040_split);
    AddBranch("isActiveHLT_j225_gsc275_bmv2c1060_split", &isActiveHLT_j225_gsc275_bmv2c1060_split);
    AddBranch("isActiveHLT_j225_gsc300_bmv2c1070_split", &isActiveHLT_j225_gsc300_bmv2c1070_split);
    AddBranch("isActiveHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490", &isActiveHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490);
    AddBranch("isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split", &isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split);
    AddBranch("isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split", &isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split);
    AddBranch("isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split", &isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split);
    AddBranch("isActiveHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49", &isActiveHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49);
    AddBranch("isActiveHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49", &isActiveHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49);
    AddBranch("isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split", &isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split);
    AddBranch("isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split", &isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split);
    AddBranch("isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF", &isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF);
    AddBranch("isActiveHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split", &isActiveHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split);
    AddBranch("isActiveHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF", &isActiveHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF);
    AddBranch("isActiveHLT_j80_0eta240_j60_j45_320eta490", &isActiveHLT_j80_0eta240_j60_j45_320eta490);
    AddBranch("isActiveL1_J40_0ETA25_2J15_31ETA49", &isActiveL1_J40_0ETA25_2J15_31ETA49);
    AddBranch("isActiveHLT_j80_bmv2c2085_split_2j60_320eta490", &isActiveHLT_j80_bmv2c2085_split_2j60_320eta490);
    AddBranch("isActiveHLT_j80_0eta240_2j60_320eta490", &isActiveHLT_j80_0eta240_2j60_320eta490);
    AddBranch("isActiveL1_4J20", &isActiveL1_4J20);
    AddBranch("isActiveHLT_2j55_bmv2c2077_split_2j55", &isActiveHLT_2j55_bmv2c2077_split_2j55);
    AddBranch("isActiveHLT_2j45_bmv2c2070_split_2j45", &isActiveHLT_2j45_bmv2c2070_split_2j45);
    AddBranch("isActiveL1_4J15", &isActiveL1_4J15);
    AddBranch("isActiveHLT_2j35_bmv2c2070_split_2j35_L14J15", &isActiveHLT_2j35_bmv2c2070_split_2j35_L14J15);
    AddBranch("isActiveHLT_2j35_bmv2c2060_split_2j35_L14J15", &isActiveHLT_2j35_bmv2c2060_split_2j35_L14J15);
    AddBranch("isActiveHLT_2j35_bmv2c2050_split_2j35_L14J15", &isActiveHLT_2j35_bmv2c2050_split_2j35_L14J15);
    AddBranch("isActiveHLT_j75_bmv2c2060_split_3j75_L14J15", &isActiveHLT_j75_bmv2c2060_split_3j75_L14J15);
    AddBranch("isActiveHLT_j75_bmv2c2070_split_3j75_L14J15", &isActiveHLT_j75_bmv2c2070_split_3j75_L14J15);
    AddBranch("isActiveHLT_2j45_bmv2c2077_split_2j45_L14J15", &isActiveHLT_2j45_bmv2c2077_split_2j45_L14J15);
    AddBranch("isActiveHLT_2j45_bmv2c2070_split_2j45_L14J15", &isActiveHLT_2j45_bmv2c2070_split_2j45_L14J15);
    AddBranch("isActiveHLT_4j45", &isActiveHLT_4j45);

  // store trigger prescales
    AddBranch("prescaleL1_J40_0ETA25_2J25_J20_31ETA49", &prescaleL1_J40_0ETA25_2J25_J20_31ETA49);
    AddBranch("prescaleHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490", &prescaleHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490);
    AddBranch("prescaleHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25", &prescaleHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25);
    AddBranch("prescaleHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25", &prescaleHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25);
    AddBranch("prescaleHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25", &prescaleHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25);
    AddBranch("prescaleHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25", &prescaleHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25);
    AddBranch("prescaleHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25", &prescaleHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25);
    AddBranch("prescaleHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25", &prescaleHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25);
    AddBranch("prescaleHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25", &prescaleHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25);
    AddBranch("prescaleHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25", &prescaleHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25);
    AddBranch("prescaleHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25", &prescaleHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25);
    AddBranch("prescaleHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25", &prescaleHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25);
    AddBranch("prescaleHLT_j175_gsc225_bmv2c1040_split", &prescaleHLT_j175_gsc225_bmv2c1040_split);
    AddBranch("prescaleHLT_j175_bmv2c2040_split", &prescaleHLT_j175_bmv2c2040_split);
    AddBranch("prescaleHLT_j225_gsc275_bmv2c1060_split", &prescaleHLT_j225_gsc275_bmv2c1060_split);
    AddBranch("prescaleHLT_j225_gsc300_bmv2c1070_split", &prescaleHLT_j225_gsc300_bmv2c1070_split);
    AddBranch("prescaleHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490", &prescaleHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490);
    AddBranch("prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split", &prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split);
    AddBranch("prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split", &prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split);
    AddBranch("prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split", &prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split);
    AddBranch("prescaleHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49", &prescaleHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49);
    AddBranch("prescaleHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49", &prescaleHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49);
    AddBranch("prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split", &prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split);
    AddBranch("prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split", &prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split);
    AddBranch("prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF", &prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF);
    AddBranch("prescaleHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split", &prescaleHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split);
    AddBranch("prescaleHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF", &prescaleHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF);
    AddBranch("prescaleHLT_j80_0eta240_j60_j45_320eta490", &prescaleHLT_j80_0eta240_j60_j45_320eta490);
    AddBranch("prescaleL1_J40_0ETA25_2J15_31ETA49", &prescaleL1_J40_0ETA25_2J15_31ETA49);
    AddBranch("prescaleHLT_j80_bmv2c2085_split_2j60_320eta490", &prescaleHLT_j80_bmv2c2085_split_2j60_320eta490);
    AddBranch("prescaleHLT_j80_0eta240_2j60_320eta490", &prescaleHLT_j80_0eta240_2j60_320eta490);
    AddBranch("prescaleL1_4J20", &prescaleL1_4J20);
    AddBranch("prescaleHLT_2j55_bmv2c2077_split_2j55", &prescaleHLT_2j55_bmv2c2077_split_2j55);
    AddBranch("prescaleHLT_2j45_bmv2c2070_split_2j45", &prescaleHLT_2j45_bmv2c2070_split_2j45);
    AddBranch("prescaleL1_4J15", &prescaleL1_4J15);
    AddBranch("prescaleHLT_2j35_bmv2c2070_split_2j35_L14J15", &prescaleHLT_2j35_bmv2c2070_split_2j35_L14J15);
    AddBranch("prescaleHLT_2j35_bmv2c2060_split_2j35_L14J15", &prescaleHLT_2j35_bmv2c2060_split_2j35_L14J15);
    AddBranch("prescaleHLT_2j35_bmv2c2050_split_2j35_L14J15", &prescaleHLT_2j35_bmv2c2050_split_2j35_L14J15);
    AddBranch("prescaleHLT_j75_bmv2c2060_split_3j75_L14J15", &prescaleHLT_j75_bmv2c2060_split_3j75_L14J15);
    AddBranch("prescaleHLT_j75_bmv2c2070_split_3j75_L14J15", &prescaleHLT_j75_bmv2c2070_split_3j75_L14J15);
    AddBranch("prescaleHLT_2j45_bmv2c2077_split_2j45_L14J15", &prescaleHLT_2j45_bmv2c2077_split_2j45_L14J15);
    AddBranch("prescaleHLT_2j45_bmv2c2070_split_2j45_L14J15", &prescaleHLT_2j45_bmv2c2070_split_2j45_L14J15);
    AddBranch("prescaleHLT_4j45", &prescaleHLT_4j45);
*/

  }

  // book MVA reader
  TString xmlFile = getenv("ROOTCOREBIN");                                                                                                                                                                  
  if (m_analysisType == "vbf" && m_readMVA) {
	 xmlFile += "/data/CxAODReader_VBFHbb/vbfgamma_BDT_0of2_weights.xml";
	 m_reader.BookReader("reader", xmlFile);
  }
	
}

void MVATree_VBFHbb::TransformVars() {
}


void MVATree_VBFHbb::Reset()
{
  //  sample = "Unknown";

  RunNumber       = 0;
  LumiBlock       = 0;
  EventNumber     = 0;
  npv             = 0;
  ActualMu             = 0;
  AverageMu             = 0;
  ActualMuScaled             = 0;
  AverageMuScaled             = 0;
  AverageMuRaw             = 0;
  ActualMuRaw             = 0;
  embMu1pt             = 0;
  embMu1eta             = 0;
  embMu1phi             = 0;
  embMu1ptvarcone30             = 0;
  embMu1topoetcone20             = 0;
  embMu2pt             = 0;
  embMu2eta             = 0;
  embMu2phi             = 0;
  embMu2ptvarcone30             = 0;
  embMu2topoetcone20             = 0;
  embZpt             = 0;
  embZeta             = 0;
  embZphi             = 0;
  embZm             = 0;
  hpt             = 0;
  heta             = 0;
  hy             = 0;
  mJJ_truth             = 0;
  mJJ_truth_abs             = 0;
  dEtaJJ_truth             = 0;
  dEtaJJ_truth_abs             = 0;
  nJets_truth             = 0;
  hphi             = 0;
  hm             = 0;
  embMuonWeight_cvmfs             = 0;
  embMuonTrigWeight_cvmfs             = 0;
  embMuonTrigWeight_cvmfs_14             = 0;
  embMuonTrigWeight_cvmfs_50             = 0;
  embMet             = 0;
  embMuonWeight             = 0;
  embMuonTrigWeight             = 0;
  MCWeight        = 0;
  PUWeight        = 0;
  batman        = 0;
  EventWeight     = 0;
  EventWeightforwardSF__1up     = 0;
  EventWeightforwardSF__1down     = 0;
  EventWeightcentralSF__1up     = 0;
  EventWeightcentralSF__1down     = 0;
  EventWeightBTrig_Eta_EFF__1up     = 0;
  EventWeightBTrig_Eta_EFF__1down     = 0;
  EventWeightBTrig_SF__1up     = 0;
  EventWeightBTrig_SF__1down     = 0;
  EventWeightShower_mBB__1up     = 0;
  EventWeightShower_mBB__1down     = 0;
  EventWeightShower_nJets30pt__1up     = 0;
  EventWeightShower_nJets30pt__1down     = 0;
  EventWeightFT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_Eigen_B_0_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_Eigen_B_1_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_Eigen_B_2_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_Eigen_C_0_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_Eigen_C_1_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_Eigen_C_2_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_Eigen_C_3_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_Eigen_Light_0_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_Eigen_Light_1_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_Eigen_Light_2_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_Eigen_Light_3_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_Eigen_Light_4_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_extrapolation_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_extrapolation_AntiKt4EMPFlowJets__1down = 0;
  EventWeightFT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1up = 0;
  EventWeightFT_EFF_extrapolation_from_charm_AntiKt4EMPFlowJets__1down = 0;
  BJetTriggerWeight =0;
  BJetSF =0;
  MCChannelNumber = 0;
  nJets           = 0;
  nJets20pt       = 0;
  nJets20pt_central       = 0;
  nJets20pt_centraljvt       = 0;
  nJets20pt_rap       = 0;
  nJets20pt_fsrmin08       = 0;
  nJets20pt_fsrmin10       = 0;
  nJets20pt_fsrmin12       = 0;
/*
  nJets20pt_fsr10       = 0;
  nJets20pt_fsr12       = 0;
  nJets20pt_fsr14       = 0;
  nJets20pt_fsr16       = 0;
  nJets20pt_fsr18       = 0;
  nJets20pt_fsr20       = 0;
*/
  nJets30pt       = 0;
  nJets30pt_central       = 0;
  nJets30pt_rap       = 0;
  nElectrons       = 0;
  nMuons       = 0;
  nPhotons       = 0;
  met       = 0;
  metsoft       = 0;
  metjet       = 0;
  sumsoft       = 0;

  nTightEmbMv2c10    = 0;
  nMediumEmbMv2c10   = 0;
  nLooseEmbMv2c10    = 0;

  nTightMv2c10    = 0;
  nMediumMv2c10   = 0;
  nLooseMv2c10    = 0;

  pT.clear();
  phi.clear();
  eta.clear();
  mv2c10.clear();
  HLTJetMatchedPt.clear();
  L1JetMatchedPt.clear();
  L1JetMatchedEta.clear();
  L1JetMatchedPhi.clear();
  TruthJetMatchedPt.clear();
  TruthJetMatchedEta.clear();
  mv2c20.clear();
  truthLabelID.clear();
  coneTruthLabelID.clear();
  hadronConeExclTruthLabelID.clear();
  partonTruthLabelID.clear();
  weightSysts.clear();

  weightspdf4lhc.clear();
  weightspdfnnpdf30.clear();
  weightsqcdnnlops.clear();
  weightsqcdnnlops2np.clear();
  weightsqcdwg1.clear();
  weightsalternativepdf.clear();


  alpha_s_up      = 0;
  alpha_s_dn      = 0;
  whoIsJ1         = 0;
  whoIsJ2         = 0;
  whoIsB1         = 0;
  whoIsB2         = 0;

  blind              = 0;
  mBB              = 0;
  mBB_no_corr      = 0;
  mBB_onlymu      = 0;
  mBB_truth      = 0;
  mBB_fsrmin08      = 0;
  mBB_fsrmin10      = 0;
  mBB_fsrmin12      = 0;
/*
  mBB_fsr10      = 0;
  mBB_fsr12      = 0;
  mBB_fsr14      = 0;
  mBB_fsr16      = 0;
  mBB_fsr18      = 0;
  mBB_fsr20      = 0;
*/
  dRBB             = 0;
  dRBB_no_corr     = 0;
  dRBB_onlymu     = 0;
  dPhiBB           = 0;
  dEtaBB           = 0;
  pTBB             = 0;
  pTBB_no_corr      = 0;
  pTBB_onlymu      = 0;
  pTBB_fsrmin08      = 0;
  pTBB_fsrmin10      = 0;
  pTBB_fsrmin12      = 0;
/*
  pTBB_fsr10      = 0;
  pTBB_fsr12      = 0;
  pTBB_fsr14      = 0;
  pTBB_fsr16      = 0;
  pTBB_fsr18      = 0;
  pTBB_fsr20      = 0;
*/
  EtaBB             = 0;
  PhiBB             = 0;
  EtaBB_no_corr      = 0;
  EtaBB_onlymu      = 0;
  PhiBB_no_corr      = 0;
  PhiBB_onlymu      = 0;

  mJJ              = 0;
  mJJ_base              = 0;
  etaJ1_base              = 0;
  etaJ2_base              = 0;
  mJJJ              = 0;
  deltaMJJ              = 0;
  deltaMJJ2              = 0;
  deltaMJJJ              = 0;
  deltaMJJ_pTBB_nomBB              = 0;
  deltaMJJ_pTBB              = 0;
  dRJJ             = 0;
  dRMin             = 0;
  pTMin             = 0;
  TE_Q             = 0;
  TE_X1             = 0;
  TE_X2             = 0;
  TE_XF1             = 0;
  TE_XF2             = 0;
  dPhiJJ           = 0;
  dPhiBBJJ           = 0;
  dEtaJJ           = 0;
  dEtaBBJJ           = 0;
  EtaJJ           = 0;
  PhiJJ           = 0;
  pTJJ             = 0;
  pTJJJ             = 0;

  // ======                                                                                                                                                              
  // passCURRENT = 0;
  // passATLAS = 0;
  // passCMS = 0;
  // passHYBRID = 0;

  // whoIsB1_CURRENT = 0;
  // whoIsB2_CURRENT = 0;
  // whoIsB1_ATLAS = 0;
  // whoIsB2_ATLAS = 0;
  // whoIsB1_CMS = 0;
  // whoIsB2_CMS = 0;
  // whoIsB1_HYBRID = 0;
  // whoIsB2_HYBRID = 0;

  // ======  

  pTJ1 = 0;
  pTJ1J = 0;
  pTJ2 = 0;
  pTJ2J = 0;
  pTB1 = 0;
  mB1 = 0;
  pTB1J = 0;
  pTB2 = 0;
  mB2 = 0;
  pTB2J = 0;
  pTB1_no_corr = 0;
  pTB2_no_corr = 0;
  pTB1_onlymu = 0;
  pTB2_onlymu = 0;

  J1FJVTLoose = 0;
  J2FJVTLoose = 0;
  J1FJVTTight = 0;
  J2FJVTTight = 0;
  J1Timing = 0;
  J2Timing = 0;
  J1JVT = 0;
  J2JVT = 0;
  J1FJVT = 0;
  J2FJVT = 0;

  etaJ1 = 0;
  etaJ2 = 0;
  etaB1 = 0;
  etaB2 = 0;
  etaB1_no_corr = 0;
  etaB2_no_corr = 0;
  etaB1_onlymu = 0;
  etaB2_onlymu = 0;

  dRB1J1 = 0;
  dRB1J2 = 0;
  dRB2J1 = 0;
  dRB2J2 = 0;

  mindRJ1_Ex = 0;
  mindRJ1_Ex_base = 0;
  mindRJ1_Ex_closest = 0;
  mindRJ1_Ex_dRB1 = 0;
  mindRJ1_ExPt = 0;
  mindRJ1_ExPt_closest = 0;
  mindRJ1_ExPt_dRB1 = 0;
  mindRJ2_Ex = 0;
  mindRJ2_Ex_base = 0;
  mindRJ2_Ex_closest = 0;
  mindRJ2_Ex_dRB1 = 0;
  mindRJ2_ExPt = 0;
  mindRJ2_ExPt_closest = 0;
  mindRJ2_ExPt_dRB1 = 0;
  mindRB1_Ex = 0;
  mindRB2_Ex = 0;

  TruthLabelB1 = 0;
  TruthLabelB2 = 0;
  TruthLabelPartonB1 = 0;
  TruthLabelPartonB2 = 0;
  HadronConeExclTruthLabelB1 =0;
  HadronConeExclTruthLabelB2 =0;

  MV2c10B1 = 0;
  MV2c10B2 = 0;
  MV2c10J1 = 0;
  MV2c10J2 = 0;

  NTrk500PVJ1 =0;
  NTrk500PVJ2 =0;

  pT_balance = 0;
  pT_balanceJ = 0;
  pT_balance_pTBB = 0;
  pT_balance_fsrmin08      = 0;
  pT_balance_fsrmin10      = 0;
  pT_balance_fsrmin12      = 0;
/*
  pT_balance_fsr10      = 0;
  pT_balance_fsr12      = 0;
  pT_balance_fsr14      = 0;
  pT_balance_fsr16      = 0;
  pT_balance_fsr18      = 0;
  pT_balance_fsr20      = 0;
*/
  hframe_cosTheta_h_b1 = 0;
  bdt2cen = 0;
  bdt2cenWeight = 0;
  bdt_mcmj = 0;
  bdt_1for2cen_final = 0;
  bdt_1for2cen_full = 0;
  bdt_1for2cen_trueasym = 0;
  bdt_1for2cen_angle = 0;
  bdt_1for2cen_nojets = 0;
  bdt_1for2cen_nojetsrap = 0;
  bdt_1for2cen_pflow_5jet = 0;
  bdt_1for2cen_pflow_data = 0;
  bdt_walrus_pflow_data = 0;
  bdt_1for2cen_pflow = 0;
  bdt_walrus_final = 0;
  bdt2cenOldExt = 0;
  bdt3cen = 0;
  bdtHighpt = 0;
  bdttopoInc = 0;
  bdt2cenOld = 0;
  nn_adv_e100_l0 = 0;

  // ======                                                                    

  max_J1J2       = 0;
  eta_J_star       = 0;
  HT_soft          = 0;
  centralMaxJ_Ex          = 0;
  angleAsymBB          = 0;
  mBB_angle          = 0;
  asymJJ          = 0;
  nonforwardMaxJ_Ex          = 0;
  inclusiveMaxJ_Ex          = 0;
  centralMaxJ_Ex_dRB1          = 0;
  nonforwardMaxJ_Ex_dRB1          = 0;
  inclusiveMaxJ_Ex_dRB1          = 0;
  centralMaxJ_zeppenfeld          = 0;
  nonforwardMaxJ_zeppenfeld          = 0;
  inclusiveMaxJ_zeppenfeld          = 0;
  centralMaxJ_eta          = 0;
  nonforwardMaxJ_eta          = 0;
  inclusiveMaxJ_eta          = 0;
  centralMaxJ_NTrk500PV          = 0;
  nonforwardMaxJ_NTrk500PV          = 0;
  inclusiveMaxJ_NTrk500PV          = 0;
  centralMaxJ_dEtaJ1          = 0;
  nonforwardMaxJ_dEtaJ1          = 0;
  inclusiveMaxJ_dEtaJ1          = 0;
  HT_MVA           = 0;
  cosTheta_MVA     = 0;
  cosTheta_CMS     = 0;
  cosTheta_boost     = 0;
  //  BDT              = -2;

  passHLT_2mu14 = -1;
  passHLT_mu50 = -1;
  passL1_J40_0ETA25_2J25_J20_31ETA49 = -1;
  passL1_J40_0ETA25_2J25 = -1;
  passL1_J20_31ETA49 = -1;
  passL1_J15_31ETA49 = -1;
  passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490 = -1;
  passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 = -1;
  passHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 = -1;
  passHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 = -1;
  passHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 = -1;
  passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25 = -1;
  passHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25 = -1;
  passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30 = -1;
  passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30 = -1;
  passHLT_j100_2j55_bmv2c2060_split = -1;
  passHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 = -1;
  passHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 = -1;
  passHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 = -1;
  passHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 = -1;
  passHLT_j175_gsc225_bmv2c1040_split = -1;
  passHLT_j175_bmv2c2040_split = -1;
  passHLT_j225_gsc275_bmv2c1060_split = -1;
  passHLT_j225_gsc300_bmv2c1070_split = -1;
  passHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490 = -1;
  passHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split = -1;
  passHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split = -1;
  passHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split = -1;
  passHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = -1;
  passHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = -1;
  passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split = -1;
  passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split = -1;
  passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF = -1;
  passHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split = -1;
  passHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF = -1;
  passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split = -1;
  passHLT_j45_320eta490 = -1;
  passHLT_j80_0eta240_j60_j45_320eta490 = -1;
  passHLT_j80_0eta240_j60 = -1;
  passL1_J40_0ETA25_2J15_31ETA49= -1;
  passL1_J40_0ETA25= -1;
  passL1_2J15_31ETA49= -1;
  passHLT_j80_bmv2c2085_split_2j60_320eta490= -1;
  passHLT_j80_bmv2c2085_split= -1;
  passHLT_2j60_320eta490= -1;
  passHLT_j80_0eta240_2j60_320eta490= -1;
  passHLT_j80_0eta240= -1;
  passL1_4J20= -1;
  passHLT_2j55_bmv2c2077_split_2j55= -1;
  passHLT_2j45_bmv2c2070_split_2j45= -1;
  passL1_4J15= -1;
  passHLT_2j35_bmv2c2070_split_2j35_L14J15= -1;
  passHLT_2j35_bmv2c2060_split_2j35_L14J15= -1;
  passHLT_2j35_bmv2c2050_split_2j35_L14J15= -1;
  passHLT_j75_bmv2c2060_split_3j75_L14J15 = -1;
  passHLT_j75_bmv2c2070_split_3j75_L14J15= -1;
  passHLT_2j45_bmv2c2077_split_2j45_L14J15 = -1;
  passHLT_2j45_bmv2c2070_split_2j45_L14J15 = -1;
  passHLT_4j45 = -1;

  isActiveL1_J40_0ETA25_2J25_J20_31ETA49 = -1;
  isActiveHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490 = -1;
  isActiveHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 = -1;
  isActiveHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 = -1;
  isActiveHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 = -1;
  isActiveHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 = -1;
  isActiveHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25 = -1;
  isActiveHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25 = -1;
  isActiveHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30 = -1;
  isActiveHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30 = -1;
  isActiveHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 = -1;
  isActiveHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 = -1;
  isActiveHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 = -1;
  isActiveHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 = -1;
  isActiveHLT_j175_gsc225_bmv2c1040_split = -1;
  isActiveHLT_j175_bmv2c2040_split = -1;
  isActiveHLT_j225_gsc275_bmv2c1060_split = -1;
  isActiveHLT_j225_gsc300_bmv2c1070_split = -1;
  isActiveHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490 = -1;
  isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split = -1;
  isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split = -1;
  isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split = -1;
  isActiveHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = -1;
  isActiveHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = -1;
  isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split = -1;
  isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split = -1;
  isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF = -1;
  isActiveHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split = -1;
  isActiveHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF = -1;
  isActiveHLT_j80_0eta240_j60_j45_320eta490 = -1;
  isActiveL1_J40_0ETA25_2J15_31ETA49= -1;
  isActiveHLT_j80_bmv2c2085_split_2j60_320eta490= -1;
  isActiveHLT_j80_0eta240_2j60_320eta490= -1;
  isActiveL1_4J20= -1;
  isActiveHLT_2j55_bmv2c2077_split_2j55= -1;
  isActiveHLT_2j45_bmv2c2070_split_2j45= -1;
  isActiveL1_4J15= -1;
  isActiveHLT_2j35_bmv2c2070_split_2j35_L14J15= -1;
  isActiveHLT_2j35_bmv2c2060_split_2j35_L14J15= -1;
  isActiveHLT_2j35_bmv2c2050_split_2j35_L14J15= -1;
  isActiveHLT_j75_bmv2c2060_split_3j75_L14J15 = -1;
  isActiveHLT_j75_bmv2c2070_split_3j75_L14J15= -1;
  isActiveHLT_2j45_bmv2c2077_split_2j45_L14J15 = -1;
  isActiveHLT_2j45_bmv2c2070_split_2j45_L14J15 = -1;
  isActiveHLT_4j45 = -1;

  prescaleL1_J40_0ETA25_2J25_J20_31ETA49 = -2.0;
  prescaleHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490 = -2.0;
  prescaleHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 = -2.0;
  prescaleHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 = -2.0;
  prescaleHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 = -2.0;
  prescaleHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 = -2.0;
  prescaleHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25 = -2.0;
  prescaleHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25 = -2.0;
  prescaleHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30 = -2.0;
  prescaleHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30 = -2.0;
  prescaleHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 = -2.0;
  prescaleHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 = -2.0;
  prescaleHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 = -2.0;
  prescaleHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 = -2.0;
  prescaleHLT_j175_gsc225_bmv2c1040_split = -2.0;
  prescaleHLT_j175_bmv2c2040_split = -2.0;
  prescaleHLT_j225_gsc275_bmv2c1060_split = -2.0;
  prescaleHLT_j225_gsc300_bmv2c1070_split = -2.0;
  prescaleHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490 = -2.0;
  prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split = -2.0;
  prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split = -2.0;
  prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split = -2.0;
  prescaleHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = -2.0;
  prescaleHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 = -2.0;
  prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split = -2.0;
  prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split = -2.0;
  prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF = -2.0;
  prescaleHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split = -2.0;
  prescaleHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF = -2.0;
  prescaleHLT_j80_0eta240_j60_j45_320eta490 = -2.0;
  prescaleL1_J40_0ETA25_2J15_31ETA49 = -2.0;
  prescaleHLT_j80_bmv2c2085_split_2j60_320eta490 = -2.0;
  prescaleHLT_j80_0eta240_2j60_320eta490 = -2.0;
  prescaleL1_4J20 = -2.0;
  prescaleHLT_2j55_bmv2c2077_split_2j55 = -2.0;
  prescaleHLT_2j45_bmv2c2070_split_2j45 = -2.0;
  prescaleL1_4J15 = -2.0;
  prescaleHLT_2j35_bmv2c2070_split_2j35_L14J15 = -2.0;
  prescaleHLT_2j35_bmv2c2060_split_2j35_L14J15 = -2.0;
  prescaleHLT_2j35_bmv2c2050_split_2j35_L14J15 = -2.0;
  prescaleHLT_j75_bmv2c2060_split_3j75_L14J15 = -2.0;
  prescaleHLT_j75_bmv2c2070_split_3j75_L14J15 = -2.0;
  prescaleHLT_2j45_bmv2c2077_split_2j45_L14J15 = -2.0;
  prescaleHLT_2j45_bmv2c2070_split_2j45_L14J15 = -2.0;
  prescaleHLT_4j45 = -2.0;

}

void MVATree_VBFHbb::ReadMVA() {
  
  if (!m_readMVA) return;

  //  BDT = m_reader.EvaluateMVA("reader");
}


