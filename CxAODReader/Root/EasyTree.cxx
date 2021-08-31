#include "CxAODReader/EasyTree.h"
#include "EventLoop/Worker.h"

EasyTree::EasyTree(bool persistent, std::string analysisType, EL::IWorker *wk,
                   std::vector<std::string> variations, bool nominalOnly,
                   const bool &allow_direct_branch_creation)
    : MVATree(persistent, false, wk, variations, nominalOnly),
      m_analysisType(analysisType),
      m_allow_direct_branch_creation(allow_direct_branch_creation) {
  SetBranches();
}

// void EasyTree::SetBranches() {
// }

// void EasyTree::TransformVars() {
// }

void EasyTree::Fill() {
  StaticSetBranches(m_analysisType, this);
  MVATree::Fill();
}

// void EasyTree::Reset()
// {
//   StaticReset();
// }
