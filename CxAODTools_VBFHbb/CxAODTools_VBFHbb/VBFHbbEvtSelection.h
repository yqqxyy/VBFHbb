#ifndef CxAODTools__VBFHbbEvtSelection_H
#define CxAODTools__VBFHbbEvtSelection_H

#include "CxAODTools/EventSelection.h"

namespace xAOD {
#ifndef XAODJET_JET_H
  class Jet;
#endif
#ifndef XAODEGAMMA_ELECTRON_H
  class Electron;
#endif
#ifndef XAODMUON_MUON_H
  class Muon;
#endif

}

template <typename T>
class VBFHbbEvtSelection : public EventSelection
{
  protected:
    T m_result;

  public:
    VBFHbbEvtSelection() noexcept;

    virtual ~VBFHbbEvtSelection() noexcept {}

    virtual T& result() { return m_result; } // probably should be const ?

    virtual bool passPreSelection(SelectionContainers & containers, bool isKinVar) override;

    virtual bool passSelection(SelectionContainers & containers, bool isKinVar) override;

    virtual void setIsMC(bool isMC=false) {m_isMC = isMC;};

  protected:
    //common to Incl and 1ph analysis
    virtual bool passJetPreSelection     (const xAOD::JetContainer* jets,const xAOD::JetContainer* fatjets);
    virtual bool passTriggerPreSelection (const xAOD::EventInfo* evtinfo);

    virtual bool passKinematics         ();



    //specific to each analysis
    virtual bool passJetSelection     (const xAOD::JetContainer* jets, const xAOD::EventInfo* evtinfo) = 0;
    virtual bool passTriggerSelection (const xAOD::EventInfo* evtinfo) = 0;
    virtual void clearResult          () = 0;

    double m_b_85_weight = 0.07;
    double m_b_77_weight = 0.63;
    double m_b_70_weight = 0.83;
    double m_b_60_weight = 0.94;
    //double m_b_85_weight = 0.11;
    //double m_b_77_weight = 0.64;
    //double m_b_70_weight = 0.83;
    //double m_b_60_weight = 0.94;

    bool m_isMC;


};

#include "CxAODTools_VBFHbb/VBFHbbEvtSelection.icc"

#endif
