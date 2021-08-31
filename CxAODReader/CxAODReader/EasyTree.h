#ifndef CxAODReader_EasyTree_H
#define CxAODReader_EasyTree_H

#include <xAODBase/IParticle.h>

#include "CxAODReader/MVATree.h"
#include "CxAODReader/StaticMVATree.h"

namespace EL {
class IWorker;
}

namespace CxAODReader_EasyTree_internal {
template <class Tree>
using STree = StaticMVATree<Tree, bool, int, unsigned, long, unsigned long,
                            unsigned long long, float, double, std::string,
                            xAOD::IParticle::FourMom_t, std::vector<int>,
                            std::vector<float>, std::vector<double>,
                            std::vector<xAOD::IParticle::FourMom_t>>;
}

// #######################################
// HOW TO USE EASYTREE TO CREATE BRANCHES
// #######################################
// There is two methods:
// (1) Use directly m_etree->SetBranchAndValue to create branch on the fly when
// processing event (2) Create branches after intialization of the EasyTree and
// call SetBranchAndValue when processing events
//
// DO NOT MIX THE TWO METHOD (checks are performed to avoid that mix)
//
// ==============================
// Method (1): Direct creation of branch and setting value at the same time (on
// the fly when processing event)
// ==============================
// when processing events
// m_etree->SetBranchAndValue<std::vector<double>> ( "fatjet_pT" ,
// vector_fatjet_pT, std::vector<double> ()) ;
// -> BRANCH IS CREATED DIRECTLY calling this method for the first time

// =========================================
// Method (2): Creation of branch in initialize function + setting value when
// processing event
// =========================================
// in the AnalysisReader::initialize() function (or equivalent function in
// AnalysisReader_VHQQ for example)

// bool allow_direct_branch_creation= false ; // -> will disable method (1)
// m_etree = new EasyTree(writeEasyTree, m_analysisType, wk(), m_variations,
// false, allow_direct_branch_creation);
// m_etree->CreateBranch<std::vector<double> ( "fatjet_pT" , std::vector<double>
// ()) ; (create all branch you need)
//
// when processing event simply call
// m_etree->SetBranchAndValue<std::vector<double>>( "fatjet_pT" ,
// vector_fatjet_pT , std::vector<double> ()) ;
// ###########
// NOTA BENE:  for Method (2) the default value will be the one used when
// calling CreateBranch, the default value given for SetBranchAndValue is indeed
// useless but switching between methods (1) and (2) is easy, it is only a one
// line change
// -> change boolean value allow_direct_branch_creation when calling EasyTree
// constructor
// -> allow_direct_branch_creation = true  -> method (1) : enables  branch
// creation on the fly
// -> allow_direct_branch_creation = false -> method (2) : disables branch
// creation on the fly
// ###########

// ==================================
// Why using method (2) is better?
// ==================================
// Imagine for example you want to save fatjet pT in different branches, and you
// select events with at most 2 fatjets so you would like to have 2 branches
// fatjet1_pt and fatjet2_pt
//

// with method (1) you would do
// if (vector_fatjet_pT.size() > 1)
//   m_etree->SetBranchAndValue<double>> ( "fatjet1_pT" , vector_fatjet_pT[0],
//   -99.) ;
// if (vector_fatjet_pT.size() > 2)
//   m_etree->SetBranchAndValue<double>> ( "fatjet2_pT" , vector_fatjet_pT[1],
//   -99.) ;
//
//
// It will only create the corresponding fatjet2_pT branch if at least an event
// with 2 fatjets pass selections So for some really tight selections you may
// have files with no such kind of events and the corresponding output Tree
// corresponding to that job would have missing branches. with method (2) you
// are sure that the branches will be created and will be filled with default
// values -99. if SetBranchValue is not called

class EasyTree : public MVATree,
                 public CxAODReader_EasyTree_internal::STree<EasyTree> {
  friend CxAODReader_EasyTree_internal::STree<EasyTree>;

protected:
  std::string m_analysisType;
  // if true will allow on the fly branch creation, otherwise it will not allow
  // it
  bool m_allow_direct_branch_creation;

  template <class T> void AddBranch(TString name, T *address) {
    for (std::pair<std::string, TTree *> tree : m_treeMap) {
      tree.second->Branch(name, address);
    }
  }

  virtual void SetBranches() override {}

  virtual void TransformVars() override {}

public:
  bool get_direct_branch_creation_allowed() {
    return m_allow_direct_branch_creation;
  }
  EasyTree(bool persistent, std::string analysisType, EL::IWorker *wk,
           std::vector<std::string> variations, bool nominalOnly,
           const bool &allow_direct_branch_creation = true);
  ~EasyTree() {}

  virtual void Reset() override { StaticReset(); }

  void Fill();

  // template function cannot be defined in .cxx source file,
  // they have to be defined in the .h header file

  template <class T>
  void
  SetBranchAndValue(const std::string &name, const T &val, const T &default_val,
                    const std::string &analysis_type = "StaticMVATree::ALL") {
    if (m_allow_direct_branch_creation) {
      // Create branch on the fly when processing event
      // in some cases one wants to prevent creation on the fly and rather
      // initialise branches before processing events call StaticMVA method
      CxAODReader_EasyTree_internal::STree<EasyTree>::SetBranchAndValue<T>(
          name, val, default_val, analysis_type);
    } else {
      // Do not allow to create branch on the fly -> need to initialize branches
      // before processing events check branch has been initialized
      if (!this->exists<T>(name)) {
        std::ostringstream error_message;
        error_message << std::endl;
        error_message << "Branch was not initialized: branchname = " << name
                      << std::endl;
        error_message
            << "EasyTree was created with m_allow_direct_branch_creation = "
            << m_allow_direct_branch_creation << std::endl;
        error_message << "On the fly branch creation when processing events is "
                         "hence disabled"
                      << std::endl;
        error_message << "Either set m_allow_direct_branch_creation = "
                      << !m_allow_direct_branch_creation << std::endl;
        error_message
            << "Or call EasyTree::CreateBranch in AnalysisReader::initialize"
            << std::endl;
        error_message << "It is better to use EasyTree::CreateBranch to "
                         "initialize branches before processing events"
                      << std::endl;
        error_message << "See EasyTree.h header for more detail" << std::endl;

        Error("EasyTree::SetBranchAndValue", error_message.str().c_str());
      }

      // if exists, just update value. default_val and analysis_type have no
      // reason to change see StaticMVA::SetBranchAndValue function
      std::get<0>(this->access<T>(name)) = val;
    }
  }

  template <class T>
  void CreateBranch(const std::string &name, const T &default_val,
                    const std::string &analysis_type = "StaticMVATree::ALL") {
    if (m_allow_direct_branch_creation) {
      std::ostringstream error_message;
      error_message << "For this EasyTree m_allow_direct_branch_creation = "
                    << m_allow_direct_branch_creation << std::endl;
      error_message << "Either set m_allow_direct_branch_creation = "
                    << !m_allow_direct_branch_creation << std::endl;
      error_message
          << "Or do not use CreateBranch, since with the current setup, "
             "branches will be created on the fly when processing events"
          << std::endl;
      error_message << "It is better to use EasyTree::CreateBranch to "
                       "initialize branches before processing events"
                    << std::endl;
      error_message << "See EasyTree.h header for more detail" << std::endl;
      error_message << "branchname = " << name << std::endl;

      Error("EasyTree::CreateBranch", error_message.str().c_str());
    }
    // initialize branch with default value in the AnalysisReader::initialize()
    // function call StaticMVA method with val = default_val
    CxAODReader_EasyTree_internal::STree<EasyTree>::SetBranchAndValue<T>(
        name, default_val, default_val, analysis_type);

    // add branch to the trees
    // if trees are going to be written to output files
    if (m_persistent) {
      // will create the branch
      StaticSetBranches(m_analysisType, this);
    }
  }
};

#endif // ifndef CxAODReader_EasyTree_H
