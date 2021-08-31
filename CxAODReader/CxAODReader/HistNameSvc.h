#ifndef HistNameSvc_h
#define HistNameSvc_h

#include <map>
#include <string>

class HistNameSvc {
public:
  enum class Analysis { undefined = 0, VHbb, VHccLegacy };
  enum class AnalysisType { undefined = 0, VBF, CUT, MVA };
  enum class AnalysisStrategy { undefined = 0, Resolved, Merged, SimpleMerge };

  enum class TagCategory { inclusive = -1, LL, MM, TT };

protected:
  // buffer for the full output name
  std::string m_name;

  std::string m_sample;
  Analysis m_analysis;
  AnalysisType m_analysisType;
  AnalysisStrategy m_analysisStrategy;
  bool m_useEventFlav;
  bool m_useEventFlavTau;
  bool m_useVHccSplitTTbar;
  bool m_useVHccSplitDiboson;
  bool m_useVHccSplitWjets;
  int m_DSID;
  int m_jet0flav;
  int m_jet1flav;
  int m_nTag;
  TagCategory m_tagCategory;
  int m_nJet;
  int m_nVRJet;
  int m_nFatJet;
  int m_nProng;
  double m_pTV; // MeV
  std::string m_description;
  std::string m_variation;
  bool m_isNominal;
  float m_mVH; // MeV
  int m_nBTagTrackJetUnmatched;
  int m_nTrackJetInFatJet;
  bool m_doOneSysDir;
  bool m_doResolvedPtvSplit400GeV;
  bool m_doMoriond;
  std::string m_lepFlav;

  void appendEventFlavour(std::string &buff);
  void appendEventnVRjets(std::string &buff);
  void appendLeptonFlavour(std::string &buff);
  void appendFlavorLabel(int &buff);
  void appendVHccSplitTTbar(std::string &buff);
  void appendVHccSplitDiboson(std::string &buff, int DSID);

  bool m_doMergedPtvSplit600GeV;

public:
  HistNameSvc();
  ~HistNameSvc(){};

  std::map<int, std::string> m_samples;

  virtual void reset(bool resetSample = false);

  virtual std::string getFullHistName(const std::string &variable);

  std::string getEventFlavour();
  int getFlavorLabel();
  std::string getFullSample();

  bool get_isNominal() { return m_isNominal; }

  virtual void set_sample(const std::string &sample);
  std::string get_sample() { return m_sample; }
  void set_analysis(Analysis type) { m_analysis = type; }
  void set_analysisType(AnalysisType type) { m_analysisType = type; }
  Analysis get_analysis() { return m_analysis; }
  AnalysisType get_analysisType() { return m_analysisType; }
  void set_doMoriond(bool doMoriond) { m_doMoriond = doMoriond; };
  bool get_doMoriond() { return m_doMoriond; }
  void set_analysisStrategy(AnalysisStrategy strategy) {
    m_analysisStrategy = strategy;
  }
  AnalysisStrategy get_analysisStrategy() { return m_analysisStrategy; }
  void set_eventFlavour(int jet0flav, int jet1flav) {
    m_jet0flav = jet0flav;
    m_jet1flav = jet1flav;
  }
  void get_eventFlavour(int &jet0flav, int &jet1flav) {
    jet0flav = m_jet0flav;
    jet1flav = m_jet1flav;
  }
  void set_nTag(int nTag) { m_nTag = nTag; }
  int get_nTag() { return m_nTag; }
  void set_tagCategory(TagCategory category) { m_tagCategory = category; }
  void set_nJet(int nJet) { m_nJet = nJet; }
  void set_nVRJet(int nVRJet) { m_nVRJet = nVRJet; }
  void set_nProng(int nProng) { m_nProng = nProng; };
  int get_nProng() { return m_nProng; }
  int get_nJet() { return m_nJet; }
  void set_nFatJet(int nFatJet) { m_nFatJet = nFatJet; }
  void set_pTV(double pTV) { m_pTV = pTV; }
  float get_pTV() { return m_pTV; }
  void set_description(const std::string &description) {
    m_description = description;
  }
  void set_variation(const std::string &variation);
  void set_isNominal();
  std::string get_description() { return m_description; }
  void set_useEventFlav(bool useEventFlav) { m_useEventFlav = useEventFlav; }
  int get_useEventFlav() { return m_useEventFlav; }
  void set_useEventFlavTau(bool useEventFlavTau) {
    m_useEventFlavTau = useEventFlavTau;
  }
  int get_useEventFlavTau() { return m_useEventFlavTau; }

  bool get_doResolvedPtvSplit400GeV() { return m_doResolvedPtvSplit400GeV; }
  void set_doResolvedPtvSplit400GeV(bool doResolvedPtvSplit400GeV) {
    m_doResolvedPtvSplit400GeV = doResolvedPtvSplit400GeV;
  }
  bool get_useVHccSplitWjets() { return m_useVHccSplitWjets; }
  void set_useVHccSplitWjets(bool useVHccSplitWjets) {
    m_useVHccSplitWjets = useVHccSplitWjets;
  }
  bool get_useVHccSplitTTbar() { return m_useVHccSplitTTbar; }
  void set_useVHccSplitTTbar(bool useVHccSplitTTbar) {
    m_useVHccSplitTTbar = useVHccSplitTTbar;
  }
  bool get_useVHccSplitDiboson() { return m_useVHccSplitDiboson; }
  void set_useVHccSplitDiboson(bool useVHccSplitDiboson, int mcChannel) {
    m_useVHccSplitDiboson = useVHccSplitDiboson;
    m_DSID = mcChannel;
  }
  std::string get_lepFlav() { return m_lepFlav; }
  void set_lepFlav(const std::string &lepFlav) { m_lepFlav = lepFlav; }

  void set_nBTagTrackJetUnmatched(int n) { m_nBTagTrackJetUnmatched = n; }
  int get_nBTagTrackJetUnmatched() { return m_nBTagTrackJetUnmatched; }
  void set_nTrackJetInFatJet(int n) { m_nTrackJetInFatJet = n; }
  void set_mVH(float mVH) { m_mVH = mVH; }
  void set_doOneSysDir(bool doOneSysDir);

  void set_doMergedPtvSplit600GeV(bool doMergedPtvSplit600GeV) {
    m_doMergedPtvSplit600GeV = doMergedPtvSplit600GeV;
  }
};

#endif
