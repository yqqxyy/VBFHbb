#ifndef CxAODReader_AnalysisReader_H
#define CxAODReader_AnalysisReader_H

#include <algorithm>
#include <utility>
#include <vector>

#include <EventLoop/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

#ifndef __MAKECINT__
#include "CxAODReader/HistSvc.h"
#include "CxAODTools/BTaggingTool.h"
#include "CxAODTools/CommonProperties.h"
#include "CxAODTools/EventSelection.h"
#include "CxAODTools/EvtWeightVariations.h"
#include "CxAODTools/FakeFactor.h"
#include "CxAODTools/OverlapRegisterAccessor.h"
#include "CxAODTools/PUReweightingTool.h"
#include "CxAODTools/ReturnCheck.h"
#include "CxAODTools/XSectionProvider.h"
#include "CxAODTools/sumOfWeightsProvider.h"
#endif // not __MAKECINT__

#include "CxAODTools/ConfigStore.h"
#include <TH1D.h>
#include <TH1F.h>

#include "CxAODReader/ObjectReader.h"
#include "CxAODTools/Enums.h"
#include "xAODRootAccess/TStore.h"

class TriggerTool;

//==============================================================
// IMPORTANT COMMENT
// EventLoop explanation + meaning of "//!" and when to use it
// if //! is used after declaration it is a transient variable
// if not used the variable is non transient
// see
// https://gitlab.cern.ch/CxAODFramework/CxAODOperations_VHbb/-/wikis/CxAOD-Reader#eventloop-important-technical-details
//==============================================================

class AnalysisReader : public EL::Algorithm {
protected:
  xAOD::TStore m_TStore; // An instance of TStore is needed if any CP::tool is
                         // usd in the class!

  xAOD::TEvent *m_event = nullptr; //!

  virtual EL::StatusCode initializeVariablesUsingConfigStore();
  virtual EL::StatusCode initializeBTagConfigs();
  virtual EL::StatusCode initializeBTagConfigs_CheckOldStyle();
  virtual EL::StatusCode initializeCxAODTag();
  virtual EL::StatusCode initializeEvent();
  virtual EL::StatusCode initializeReader();
  virtual EL::StatusCode initializeSelection();
  virtual EL::StatusCode initializeTools();
  virtual EL::StatusCode finalizeTools();
  virtual EL::StatusCode initializeIsMC();
  virtual EL::StatusCode initializeChannel();
  virtual EL::StatusCode initializeSumOfWeights();
  virtual EL::StatusCode initializeVariations();

  virtual EL::StatusCode initializeTreeBranches();
  virtual EL::StatusCode initializeValidationSelection();
  virtual EL::StatusCode fill_validation();

  virtual EL::StatusCode applyVariationToTools(TString sysName);
  virtual EL::StatusCode setEventWeight();
  virtual EL::StatusCode doOverlapRemoval(std::string varName);

  bool DirectoryOrFileExists(const std::string &path);
  std::string GetSubmitDirName(const std::string &absolutepath_submitDir);
  void CreateDirectory(const std::string &path);
  void AddOutputToJob(EL::Job &job, const std::string &label,
                      const std::string &eospath, const bool &useEOSXRD);

  EL::StatusCode clearEmptyContainersWithNonZeroSize();

  // function to add the btagging systs to m_SystWeight
  virtual EL::StatusCode
  fill_BTagSystematics(const std::map<std::string, float> map_btag_EffSFs,
                       std::string btc_name = "Default",
                       std::string description = "");

  //***** DEPRECATED ******//
  // Please move to the new structure!
  // follow instructions inside this function to update it.
  float computeBTagSFWeight(const std::vector<const xAOD::Jet *> &signalJets,
                            const std::string &authorName);

  // this function pointer has to be set to some
  // fill function in initializeSelection()
  std::function<EL::StatusCode()> m_fillFunction;           //!
  std::function<EL::StatusCode()> m_validationFillFunction; //!

  // example fill function
  EL::StatusCode fill_example();

  EL::StatusCode checkEventDuplicates();
  EL::StatusCode checkSherpaTruthPt(bool &pass, float &sherpaTruthPt);
  EL::StatusCode checkPowhegTruthMtt(bool &pass, float &powhegTruthMtt);
  EL::StatusCode checkSherpaVZqqZbb(bool &pass, std::string m_varName);
  EL::StatusCode applyMCEventWeight();
  EL::StatusCode applyExtension0LepTTbarWeight();
  EL::StatusCode applyExtension1LepTTbarWeight();
  EL::StatusCode applyExtension0LepZnunuWeight();
  EL::StatusCode applyExtension01LepStopWtWeight();
  EL::StatusCode applyLumiWeight();
  EL::StatusCode applyPUWeight();
  EL::StatusCode applyVZqqZbbWeight(int DSID);
  bool vetoDilepTopEvents();
  bool removeTopOverlapEvents();
  EL::StatusCode applyVZqqZccWeight(int DSID);

  EL::StatusCode getTLVFromJet(const xAOD::Jet *jet, TLorentzVector &tlv,
                               const std::string &name4Vec = "OneMu");
  EL::StatusCode getCorrectedTLV(const xAOD::Jet *jet, TLorentzVector &tlv,
                                 bool isFatJet,
                                 const std::string &thisCorsr = "OneMu");
  EL::StatusCode applyRtrkUncertFix(const xAOD::Jet *jet);

  void getNeutrinoPz(double MET, double MET_phi, const TLorentzVector &lepVec,
                     double &nu_pz1, double &nu_pz2);
  double getNeutrinoPz(double MET, double MET_phi, const TLorentzVector &lepVec,
                       bool min = true);
  TLorentzVector getNeutrinoTLV(const TLorentzVector &metVec,
                                const TLorentzVector &lepVec, bool min);

  void getGAHeavyFlavHadronLabels_PtSort(
      const xAOD::Jet *jet, std::vector<std::pair<int, float>> &label) const;
  void getGAHeavyFlavHadronLabels_PtSort(const xAOD::Jet *jet, int &jetflav1,
                                         int &jetflav2) const;
  int getGAHeavyFlavHadronLabels_leadPt(const xAOD::Jet *jet) const;

  bool
  checkMCEventWeight(); // Method to discard events with large MCEventWeights
                        // seen in ggZllHbb (MC16d), ggZvvHbb (MC16e), ggZllHcc
                        // (MC16e) and ggZvvHcc (MC16e) [to be removed when xAOD
                        // prodcution fixed]

public:
  static bool sortPtGAHeavyFlavHadrons(std::pair<int, float> had1,
                                       std::pair<int, float> had2);

  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)

public:
  // this is a standard constructor
  AnalysisReader();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob(EL::Job &job) override;
  virtual EL::StatusCode fileExecute() override;
  virtual EL::StatusCode histInitialize() override;
  virtual EL::StatusCode changeInput(bool firstFile) override;
  virtual EL::StatusCode initialize() override;
  virtual EL::StatusCode execute() override;
  virtual EL::StatusCode postExecute() override;
  virtual EL::StatusCode finalize() override;
  virtual EL::StatusCode histFinalize() override;
  virtual EL::StatusCode executePreEvtSel() { return EL::StatusCode::SUCCESS; };

  void setConfig(ConfigStore *config) { m_config = config; }

  void setSumOfWeightsFile(const std::string sumOfWeightsFile) {
    m_sumOfWeightsFile = sumOfWeightsFile;
  }

protected:
  // list of all object reader
  std::vector<ObjectReaderBase *> m_objectReader = {}; //!

  // object reader to handle all variations of one object container
  ObjectReader<xAOD::EventInfo> *m_eventInfoReader = nullptr;               //!
  ObjectReader<xAOD::MissingETContainer> *m_METReader = nullptr;            //!
  ObjectReader<xAOD::MissingETContainer> *m_METMJTightReader = nullptr;     //!
  ObjectReader<xAOD::MissingETContainer> *m_METMJMuTightReader = nullptr;   //!
  ObjectReader<xAOD::MissingETContainer> *m_MPTReader = nullptr;            //!
  ObjectReader<xAOD::MissingETContainer> *m_truthMETReader = nullptr;       //!
  ObjectReader<xAOD::ElectronContainer> *m_electronReader = nullptr;        //!
  ObjectReader<xAOD::ElectronContainer> *m_forwardElectronReader = nullptr; //!
  ObjectReader<xAOD::PhotonContainer> *m_photonReader = nullptr;            //!
  ObjectReader<xAOD::MuonContainer> *m_muonReader = nullptr;                //!
  ObjectReader<xAOD::TauJetContainer> *m_tauReader = nullptr;               //!
  ObjectReader<xAOD::DiTauJetContainer> *m_ditauReader = nullptr;           //!
  ObjectReader<xAOD::JetContainer> *m_jetReader = nullptr;                  //!
  ObjectReader<xAOD::JetContainer> *m_fatJetReader = nullptr;               //!
  ObjectReader<xAOD::JetContainer> *m_fatJetAltReader = nullptr;            //!
  ObjectReader<xAOD::JetContainer> *m_trackJetReader = nullptr;             //!
  ObjectReader<xAOD::JetContainer> *m_subJetReader = nullptr;               //!
  ObjectReader<xAOD::TruthEventContainer> *m_truthEventReader = nullptr;    //!
  ObjectReader<xAOD::TruthParticleContainer> *m_truthParticleReader =
      nullptr;                                                             //!
  ObjectReader<xAOD::TruthParticleContainer> *m_truthTauReader = nullptr;  //!
  ObjectReader<xAOD::TruthParticleContainer> *m_truthMuonReader = nullptr; //!
  ObjectReader<xAOD::TruthParticleContainer> *m_truthElectronReader =
      nullptr; //!
  ObjectReader<xAOD::TruthParticleContainer> *m_truthNeutrinoReader =
      nullptr;                                                       //!
  ObjectReader<xAOD::JetContainer> *m_truthWZJetReader = nullptr;    //!
  ObjectReader<xAOD::JetContainer> *m_truthWZFatJetReader = nullptr; //!
  ObjectReader<xAOD::EventInfo> *m_mmcReader = nullptr;              //!

  // pointers to the current variation of objects
  // can be directly accessed from fill functions,
  // but it is safer to use the result of a selection class
  // (for consistent pre-selection, pT-sorting, ...)
#ifndef __MAKECINT__
  const xAOD::EventInfo *m_eventInfo = nullptr;                //!
  const xAOD::EventInfo *m_mmc = nullptr;                      //!
  const xAOD::ElectronContainer *m_electrons = nullptr;        //!
  const xAOD::ElectronContainer *m_forwardElectrons = nullptr; //!
  const xAOD::PhotonContainer *m_photons = nullptr;            //!
  const xAOD::MuonContainer *m_muons = nullptr;                //!
  const xAOD::TauJetContainer *m_taus = nullptr;               //!
  const xAOD::DiTauJetContainer *m_ditaus = nullptr;           //!
  const xAOD::JetContainer *m_jets = nullptr;                  //!
  const xAOD::JetContainer *m_fatJets = nullptr;               //!
  const xAOD::JetContainer *m_fatJetsAlt = nullptr;            //!
  const xAOD::JetContainer *m_trackJets = nullptr;             //!
  const xAOD::JetContainer *m_subJets = nullptr;               //!
  const xAOD::MissingETContainer *m_metCont = nullptr;         //!
  const xAOD::MissingET *m_met = nullptr;                      //!
  const xAOD::MissingET *m_met_soft = nullptr;                 //!
  const xAOD::MissingETContainer *m_mptCont = nullptr;         //!
  // const xAOD::MissingETContainer *m_truthCont;        //!
  const xAOD::MissingET *m_mpt = nullptr;                         //!
  const xAOD::MissingET *m_truthMET = nullptr;                    //!
  const xAOD::TruthEventContainer *m_truthEvent = nullptr;        //!
  const xAOD::TruthParticleContainer *m_truthParts = nullptr;     //!
  const xAOD::TruthParticleContainer *m_truthMuons = nullptr;     //!
  const xAOD::TruthParticleContainer *m_truthTaus = nullptr;      //!
  const xAOD::TruthParticleContainer *m_truthElectrons = nullptr; //!
  const xAOD::TruthParticleContainer *m_truthNeutrinos = nullptr; //!
  const xAOD::JetContainer *m_truthWZJets = nullptr;              //!
  const xAOD::JetContainer *m_truthWZFatJets = nullptr;           //!
#endif // not __MAKECINT__

#ifndef __MAKECINT__
  template <class containerType>
  ObjectReader<containerType> *registerReader(std::string name);
#endif // not __MAKECINT__

  std::string m_currentVar =
      ""; //! need this to pass the variation name to the derived classes
  bool m_currentVarIsNominal = false; //!

  BTaggingTool *m_bTagTool = nullptr;            //!
  EvtWeightVariations *m_EvtWeightVar = nullptr; //!

  long m_eventCounter = 0;               //!
  long m_eventCountPassed = 0;           //!
  unsigned int m_countSkippedEvents = 0; //!
  bool m_isMC = false;                   //!
  std::string m_period = "a";            //!
  int m_mcChannel = -999;                //!
  int m_mcChannelFromInputFile = -999;   //!
  // QG Tagging
  typedef std::map<float, std::pair<TLorentzVector, int>> TJ_jetsinevent_t; //!
  typedef std::map<int, TJ_jetsinevent_t> TJ_eventinrun_t;                  //!
  typedef std::map<int, TJ_eventinrun_t> TJ_dsid_t;                         //!
  TJ_dsid_t m_truthjetsmap = {};                                            //!
  bool m_qgtruthmatch = false;                                              //!
  int m_qg_truth_map_status = 0;                                            //!
  int computeQGTruthMap(std::string inputFile);

  double m_weight = 1.;                  //!
  double m_MCEventWeight = 1.;           //!
  double m_SherpaWeightThreshold = 100.; //!
  int m_maxEvents = -1;       //! do we have a max number of events to run on?
  float m_luminosity = -990.; //! for rescaling the MC to a particular value of
                              //! the luminosity (default is 1 pb-1)
  double m_dataOneOverSF;     //! for plotting mu properly
  bool m_isSherpaVJets = false;            //! is Sherpa file
  bool m_isSherpaPt0WJets = false;         //! is present sample SherpaWJets Pt0
  bool m_isSherpaPt0ZJets = false;         //! is present sample SherpaZJets Pt0
  bool m_isSherpa = false;                 //!
  bool m_lowerShMCWeightThreshold = false; //!
  std::vector<std::string> m_triggerSystList = {}; //!
  int m_randomRunNumber = -1;                      //!
  float m_pileupReweight = -999.;                  //!
  float m_averageMu = -999.;                       //!
  float m_actualMu = -999.;                        //!
  float m_averageMuScaled = -999.;                 //!
  float m_actualMuScaled = -999.;                  //!

  int m_metMJCalc = -999.;        //!
  float m_jetPtCut = -999.;       //!
  float m_jetRapidityCut = -999.; //!
  float m_lepPtCut = -999.;       //!
  // m_config has to be a non transient variable (see important comment above)
  ConfigStore *m_config = nullptr;

  // list of weight variations for HistSvc
  std::vector<HistSvc::WeightSyst> m_weightSysts = {};   //!
  std::vector<HistSvc::WeightSyst> m_weightSystsCS = {}; //!

  bool m_debug = false;                            //!
  bool m_validation = false;                       //!
  std::vector<std::string> m_variations = {};      //!
  bool m_applyEventPreSelection = false;           //!
  bool m_allowORwithTruthSelection = false;        //!
  bool m_applySherpaTruthPtCut = false;            //!
  bool m_applyPowhegTruthMttCut = false;           //!
  float m_usePowhegInclFraction = -1.;             //!
  bool m_computePileupReweight = false;            //!
  bool m_recomputePileupReweight = false;          //!
  bool m_doVZqqZccOverlap = false;                 //!
  bool m_useTTBarMETFiltered0LepSamples = false;   //!
  bool m_useTTBarMETFiltered1LepSamples = false;  //! 
  bool m_useZnunuExtension0LepSamples = false;     //!
  bool m_useStopWtMETFiltered01LepSamples = false; //!
  bool m_applyLumiWeight = true;                   //!
  bool m_applyPUWeight = true;                     //!
  bool m_nominalOnly = false;                      //!
  bool m_doTruthTagging = false;                   //!
  std::string m_CxAODTag = "CxAODTag31";           //!
  std::string m_analysisType = "";                 //!
  Enums::analysisLepChannel m_analysisLepChannel =
      Enums::analysisLepChannel::undefined;    //!
  std::string m_applyVZccWeight = "Weight";    //!
  std::string m_tagStrategy = "AllSignalJets"; //!

  std::unordered_map<std::string, BTagConfig> m_BTagConfigs = {};          //!
  bool m_checkEventDuplicates = false;                                     //!
  bool m_failEventDuplicates = false;                                      //!
  std::map<long int, std::map<long int, int>> m_eventCountDuplicates = {}; //!
  bool m_putAllSysInOneDir = false;                                        //!
  bool m_recomputeMuTrigSF = false;                                        //!
  bool m_doQCD = false;                                                    //!
  bool m_doFakeFactor = false;                                             //!
  // add VZbb filtered samples
  std::string m_applyVZbbWeight = ""; //!

  // yield file can be set from executable
  // m_sumOfWeightsFile has to be a non transient variable (see important
  // comment above)
  std::string m_sumOfWeightsFile = "";

#ifndef __MAKECINT__
  HistSvc *m_histSvc = nullptr;                           //!
  HistSvc *m_truthHistSvc = nullptr;                      //!
  HistNameSvc *m_histNameSvc = nullptr;                   //!
  HistNameSvc *m_truthHistNameSvc = nullptr;              //!
  EventSelection *m_eventSelection = nullptr;             //!
  EventSelection *m_eventPostSelection = nullptr;         //!
  XSectionProvider *m_xSectionProvider = nullptr;         //!
  sumOfWeightsProvider *m_sumOfWeightsProvider = nullptr; //!
  TriggerTool *m_triggerTool = nullptr;                   //!
  PUReweightingTool *m_puReweightingTool = nullptr;       //!
  OverlapRegisterAccessor *m_overlapRegAcc = nullptr;     //!
  FakeFactor *m_FakeFactor_el = nullptr;                  //!
  FakeFactor *m_FakeFactor_mu = nullptr;                  //!
#endif                                                    // not __MAKECINT_

  // Add switch to discard events with large MC weights, samples concerned are
  // ggZllHbb (MC16d), ggZvvHbb (MC16e), ggZllHcc (MC16e) and ggZvvHcc (MC16e)
  // Switch should be removed once the bug is fixed from xAODs
  bool m_doLargeMCEventWeightsRemoval = false; //!

  int m_evtWeightVarMode = -1; //!

  bool m_doVZqqZbbOverlap = true;         //!
  bool m_doRemoveTopDilepOverlap = false; //!

  // this is needed to distribute the algorithm to the workers
  ClassDefOverride(AnalysisReader, 1);
};

#ifndef __MAKECINT__
#include "CxAODReader/AnalysisReader.icc"
#endif // not __MAKECINT__

#endif // ifndef CxAODReader_AnalysisReader_H
