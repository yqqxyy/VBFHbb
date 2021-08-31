#ifndef CxAODTools_BTaggingTool_H
#define CxAODTools_BTaggingTool_H

// TODO: obsolete
/**
 * Usage:
 * ======
 *
 * - declare tool in header:
 *   BTaggingTool m_btagtool;
 *
 * - in event loop, pass xAOD::Jets to the tool and get efficiency / scale
 * factor through e.g. float btag_eff = btagtool.getEfficiency( *jet ); float
 * btag_sf  = btagtool.getScaleFactor( *jet );
 *
 *   ==> returns -1 in both cases if the result shouldn't be used in further
 * analysis
 *
 * - you can also pass a jet container and a JetDecorator to get the scale
 * factors for all jets in it m_btagtool.getScaleFactor( jets, deco ); // "jets"
 * is an xAOD:::JetContainer, "deco" is a JetDecorator
 *
 * - Get list of systematic variations
 *   CP::SystematicsSet syst = m_btagtool.affectingSystematics();
 *
 *
 *
 * - final comment: This header should not be included in places visible to
 * CINT. In some cases, it may therefore be better to declare a pointer to the
 * tool and simply make a forward declaration in order to hide this header from
 * CINT.
 */

// Infra structure includes
// - have to include the header (instead of forward declaration of xAOD::Jet)
//   since xAOD::Jet is a typedef, i.e. not a class, hence cannot forward
//   declare...
#include "xAODBTagging/BTagging.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

// External tool (having only a forward declaration here makes linking fail on
// the grid, apparently)
#include "PATInterfaces/SystematicSet.h"
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"
#include "xAODBTaggingEfficiency/BTaggingTruthTaggingTool.h"

#include "CxAODTools/ConfigStore.h"
#include "CxAODTools/ReturnCheck.h"
#include "CxAODTools/XSectionProvider.h"
#include "EventLoop/StatusCode.h"

struct BTagConfig {
  enum class TaggerType { UNKNOWN, MV2c10, DL1, DL1r };
  
  std::string  tagger;         // Btagging Tagger: DL1, MV2c10, DL1r..
  std::string  jetAuthor;      // Jet Collection: Pflow, TrackJets..
  std::string  btagcuteff;     // Threshold in string
  std::string  scheme;         // FixedCut or Continuous
  std::string  wp;             // Operating Point in string
  std::string  rScheme;        // Reduction scheme
  std::string  wp_veto;          // Operating point for the veto used in VHcc only
  std::string  taggerName_veto;  // taggername for the veto used in VHcc only
  std::string  ExcludeEV = "";   // list of EV to be excluded

  //  unsigned MCIndex = 9999;
  bool useQuantile = false;
  bool useContinuous = false;    // use continuous btagging
  bool useReducedSetOfBTagEV = true; // use just a reduced se of Btagging EV: Store Just UP Variations
  int maxTruthTag = 2;           // maximum number of truth tagged jets

  TaggerType taggerEnum = TaggerType::MV2c10;
  TaggerType Veto_taggerEnum = TaggerType::MV2c10;

  //Truth Tagging Settings
  int  requiredTT = 2;
  bool exclusive = true; 
  bool doHybridTagging = false;
  bool doTruthTagging = false;
  std::string tagAlgorithm = "TagLabel"; //Also FlavLabel
  std::string strategy = "";

  std::vector<float> binEdges;

  //Now some constructors

  /// Default copy constructor
  BTagConfig(const BTagConfig&) = default;

  /// Default move constructor
  BTagConfig( BTagConfig&&) = default;

  // destructor
  ~BTagConfig() = default;

  // constructor
  BTagConfig (const std::map<std::string, std::string> &config);

  //some setters 
  BTagConfig& setExcludedBTagEV(const std::string &list);
  BTagConfig& setTaggerVeto(TString &vetotaggerstring);
  BTagConfig& setTagger (const std::string &tagName);
  BTagConfig& setTaggerType (); 
  BTagConfig& setJetAuthor (const std::string &author);
  BTagConfig& setTaggingScheme (const std::string &sc);
  BTagConfig& setTaggerEfficiency (const int &eff);
  BTagConfig& setBinEdges(const std::string& fileNameCDI);

  //some getters 
  std::string get_Tagger() const noexcept { return tagger; }
  std::string get_JetAuthor() const noexcept { return jetAuthor; }
  std::string get_BtagCutEff() const noexcept { return btagcuteff; }
  std::string get_TaggingScheme() const noexcept { return scheme; }
  std::string get_WorkingPoint() const noexcept { return wp; }

  void print() const;
};


/** This is the main body of the BTaggingTool:
The tool caches in memory a map of @BTaggingConfigs struct

private:
   std::unordered_map<std::string, BTagConfig> m_btc_map;

with customizable keys: std::string key = "Name of my BTagConfig" (Default = "Default"),
so one can call them by name in the analysis and easily switch among them.

** USING A SINGLE CONFIG AS DEFAULT **
All analyses that do NOT need multiple configs, can simply set ONE BTagConfig
using setDefaultBTagConfig.
It will be named as "Default" and used everywhere. All functions are already defaulted to the "Default" config.
So no need to explicit the @btc_name in these cases.
This ensures compatibility with the old analyses.

** MULTIPLE ANALYSES CONFIGS **
The analyses that DO need multiple BTagConfigs can switch between them by providing 
the correct string btc_name when calling the functions in the analysis code.

eg. getEfficiency(jet, "Resolved")

will compute the efficiency for jet "jet" using the settings
(author, wp, tagger, quantile, rcScheme etc..) of the BTagConfig named "Resolved".

At the same time, in the analysis one can switch right after to another BTagConfig:

eg. getEfficiency(jet, "Merged")  

to retrieve the efficiency of jet "jet" but using a completely different BTag Configuration named "Merged".
**/

class BTaggingTool {
  using TaggerType = BTagConfig::TaggerType;

 public:

  // constructor
  BTaggingTool() = default;

  // destructor
  ~BTaggingTool() = default;

  // initialize external tool
  StatusCode initialize(std::unordered_map<std::string, BTagConfig>& btc_map, const std::string &fileNameCDI, bool debug = false);

  // functions to load the BTagConfigs:
  StatusCode Set_BtagConfigMap(std::unordered_map<std::string, BTagConfig>& map);

  //add a single element to the BtagConfigMap
  StatusCode AddElement_BtagConfigMap(std::string name, BTagConfig& btc);

  //load the btag config to the map and set it as default
  StatusCode Load_DefaultBtagConfig(BTagConfig& default_btc);

  //Set an already loaded BtagConfig as the Default one
  StatusCode Set_BtagConfigAsDefault(std::string btc_name);

  ///////////////////////////////////////////////////////

  //some getters to retrieve the information from a specific cached BTagConfig
  std::string getTagger(std::string btc_name = "Default") const noexcept {  return m_btc_map.at(btc_name).tagger; }
  std::string getJetAuthor(std::string btc_name = "Default") const noexcept { return m_btc_map.at(btc_name).jetAuthor; }
  std::string getBtagCutEff(std::string btc_name = "Default") const noexcept { return m_btc_map.at(btc_name).btagcuteff; }
  std::string getTaggingScheme(std::string btc_name = "Default") const noexcept { return m_btc_map.at(btc_name).scheme; }
  std::string getWorkingPoint(std::string btc_name = "Default") const noexcept { return m_btc_map.at(btc_name).wp; }
  std::string getTaggingStrategy(std::string btc_name = "Default") const noexcept { return m_btc_map.at(btc_name).strategy; }

  ///////////////////////////////////////////////////////
  //Direct tagging functions:
  //these functions are here to be a non-const interface for an external user of the tool (since it uses selectTools())
  //eg. the private getJetTaggerWeight must be const, since its used inside other const functions.
  
  bool isTagged(const xAOD::Jet &jet, std::string btc_name = "Default");
  
  void isTagged_UsingTruthFlavLabel(const xAOD::Jet *Jet);

  float getTaggerWeight (const xAOD::Jet &jet, std::string btc_name = "Default");
  
  ///////////////////////////////////////////////////////
  //get Efficiencies/Inefficiencies and Scale Factors:

  float getEfficiency(const xAOD::Jet &jet, std::string btc_name = "Default");
  float getInefficiency(const xAOD::Jet &jet, std::string btc_name = "Default");
  float getMCEfficiency(const xAOD::Jet &jet, std::string btc_name = "Default");
  void getEfficiency(const xAOD::Jet &jet, float &eff, std::string btc_name = "Default") {
    eff = getEfficiency(jet, btc_name);
  }

  void getInefficiency(const xAOD::Jet &jet, float &eff, std::string btc_name = "Default") {
    eff = getInefficiency(jet, btc_name);
  }
  void getMCEfficiency(const xAOD::Jet &jet, float &eff, std::string btc_name = "Default") {
    eff = getMCEfficiency(jet, btc_name);
  }

  float getScaleFactor(const xAOD::Jet &jet, std::string btc_name = "Default");
  float getInefficiencyScaleFactor(const xAOD::Jet &jet, std::string btc_name = "Default");
  void getScaleFactor(const xAOD::Jet &jet, float &sf, std::string btc_name = "Default") {
    sf = getScaleFactor(jet, btc_name);
  }
  void getInefficiencyScaleFactor(const xAOD::Jet &jet, float &sf, std::string btc_name = "Default") {
    sf = getInefficiencyScaleFactor(jet, btc_name);
  }

  float getFullEfficiency(const xAOD::Jet &jet, bool tagged, std::string btc_name = "Default");
  float getFullEfficiency(const xAOD::Jet &jet, std::string  btc_name = "Default") {
    return getFullEfficiency(jet, isTagged(jet, btc_name), btc_name);
  }
  float getFullScaleFactor(const xAOD::Jet &jet, bool tagged, std::string btc_name = "Default");
  float getFullScaleFactor(const xAOD::Jet &jet, std::string btc_name = "Default") {
    return getFullScaleFactor(jet, isTagged(jet, btc_name), btc_name);
  }

  // get scale factors for a vector of jets
  // - attaches the scale factor as a decoration to each jet
  void getScaleFactor(const xAOD::JetContainer &jets, std::string btc_name = "Default");

  int getQuantile (const xAOD::Jet &jet, std::string btc_name = "Default");

  ///////////////////////////////////////////////////////
  //setMCIndex functions:
  BTaggingTool &setMCIndex(const char *fileName,
                           XSectionProvider &xSectionProvider);
  BTaggingTool &setMCIndex(const int &mcChannel,
                           XSectionProvider &xSectionProvider);
  
  ///////////////////////////////////////////////////////
  //tag weight functions:

  StatusCode computeEventWeight (xAOD::JetContainer &xaodjets, std::map<std::string, float>& btageffweights, bool doSyst = false, std::string btc_name = "Default");

  StatusCode compute_TruthTag_EventWeight (std::map<std::string, float> btageffweights, bool doSyst = false, std::string btc_name = "Default");

  // Jet pt sorting
  static bool sort_pt(const xAOD::Jet* jetA, const xAOD::Jet* jetB) {
    return jetA->pt() > jetB->pt();
  }

  //*****DEPRECATED*****
  //---> To be removed soon
  std::map<std::string, float> computeEventWeight_old(const std::vector<const xAOD::Jet*> &xaodjets, bool doSyst = false, std::string btc_name = "Default");

  ///////////////////////////////////////////////////////
  //truth tag functions (handled internally):
  void truth_tag_jets(unsigned long long eventNumber,const xAOD::JetContainer &signalJets, bool doSyst = false, std::string btc_name = "Default");

  void truth_tag_jets(unsigned long long eventNumber,const std::vector<const xAOD::Jet*> &signalJets, bool doSyst = false, std::string btc_name = "Default");

  ///////////////////////////////////////////////////////
  //vhbb specific functions:

  StatusCode compute_btag_info(unsigned long long eventNumber, std::vector<const xAOD::Jet *> &signalJets, bool isMC, bool doSyst, std::string btc_name = "Default");

  StatusCode get_tagjet_selection_vhbb(std::vector<const xAOD::Jet *> &signalJets, std::vector<const xAOD::Jet *> &selectedJets,
				       int &tagcatExcl, std::string btc_name = "Default", const std::vector<const xAOD::Jet *> &forwardJets = std::vector<const xAOD::Jet *>());

  StatusCode set_BtagProperties_fatJets(const xAOD::Jet *fatJet, std::vector<const xAOD::Jet *> trackJetsInFatJet,
					std::vector<const xAOD::Jet *> trackJetsNotInFatJet, bool isMC);    

  StatusCode computeEventWeight_vhbb(const std::vector<const xAOD::Jet *> &signalJets,
				      std::map<std::string, float>& btagEffSFs, float& nominalSF, bool doSyst, std::string btc_name = "Default");

  ///////////////////////////////////////////////////////
  //vhcc specific functions:

   void tagjet_selection_vhcc(std::vector<const xAOD::Jet *> &signalJets,
 			     std::vector<const xAOD::Jet *> &selectedJets,
			      int &tagcatExcl, int &nSubleadingTags, std::string btc_name = "Default");
 
   void tagjet_selection_vhcc(std::vector<const xAOD::Jet *> &signalJets, std::vector<const xAOD::Jet *> &fwdJets,
 			     std::vector<const xAOD::Jet *> &selectedJets,
			      int &tagcatExcl, int &nSubleadingTags, std::string btc_name = "Default");

  //this function computes the event weights (direct tag and truth tag) for the VHcc style of doing truth tagging
  //(filling each event to multiple histograms, 0, 1 and 2 tag, and whatever direct tag histograms )
  //it fill the map eventweights with event weights for 0,1,2 truth tags, and the -1 entry is for the direct tag histgoram (whatever that is)
   std::map< std::string, std::map<int, float> > compute_TruthTag_EventWeight_VHcc(std::vector<const xAOD::Jet*> &xaodjets, std::string btc_name = "Default",
     bool doSyst=false, std::string description="SR", const float& correction_dR_jet1 = 1, const float& correction_dR_jet2 = 1,bool decorrelateTaus=false);
  
   //function to retrieve affectingSystematics of a specific btc:
   CP::SystematicSet affectingSystematics (std::string btc_name = "Default") const;

 private:
  
   std::unordered_map<std::string, BTagConfig> m_btc_map;

   enum class CDI_FileType {
     UNKNOWN,
       OFFICIAL_JUL_2019,
       VHcc_Custom_CDI,
       OFFICIAL_OCT_2019,
       OFFICIAL_MAR_2020, 
       OFFICIAL_DEC_2020,
       OFFICIAL_APR_2021,
       };
   
  CDI_FileType m_reference_file_type = CDI_FileType::UNKNOWN;
 
  ///////////////////////////////////////////////////////
  //Initialize Tools:
  StatusCode initializeTruthTagTool(std::shared_ptr<BTaggingTruthTaggingTool> &tt_ptr, const BTagConfig& btc,  bool is_veto = false);
  StatusCode initializeEffTool(std::shared_ptr<BTaggingEfficiencyTool> &eff_ptr, const BTagConfig& btc,  bool is_veto = false);
  StatusCode initializeSelectTool(std::shared_ptr<BTaggingSelectionTool> &select_ptr, const BTagConfig& btc,  bool is_veto = false);

  void selectTools (const BTagConfig& btc, bool is_for_veto = false);

  // external tools
  // TAGGER, JET_AUTHOR, WORKING_POINT
  std::unordered_map<std::string,
    std::unordered_map<std::string,std::unordered_map<std::string,
    std::shared_ptr<BTaggingEfficiencyTool>>>> m_efficiencyTools;

  std::unordered_map<std::string,
    std::unordered_map<std::string, std::unordered_map<std::string,
    std::shared_ptr<BTaggingSelectionTool>>>>  m_selectionTools;

  std::unordered_map< std::string,
    std::unordered_map<std::string,
    std::unordered_map<std::string,
    std::shared_ptr<BTaggingTruthTaggingTool>>>> m_TruthTaggingTools;

  std::weak_ptr<BTaggingEfficiencyTool> m_currentEfficiencyTool;
  std::weak_ptr<BTaggingSelectionTool> m_currentSelectionTool;
  
  std::weak_ptr<BTaggingTruthTaggingTool> m_currentTruthTaggingTool;
  
  std::map<std::string, std::string> m_flavorDSID;
  bool m_debug = false, m_printOutOfRangeWarnings;
  std::string m_fileNameCDI;

  //MC Index definitions:
  unsigned m_MCIndex = 9999;
  const unsigned m_defaultMCIndex = 99;
  std::map<unsigned, unsigned> m_actualMC2MCIndex;
  std::set<unsigned> m_validMCIndicies;

///////////////////////////////////////////////////////
  //MCIndex private Helpers:
  unsigned indexMCEfficiencyFromFileName(const std::string &name);
  unsigned indexMCEfficiencyFromChannel(const int &mcChannel,
                                        XSectionProvider &xSectionProvider);
  unsigned get_MC_index_from_string(const std::string &name,
                                    const unsigned &defaultMCIndex);

  void SetMCIndex(std::shared_ptr<BTaggingEfficiencyTool> efftool);

  ///////////////////////////////////////////////////////

  //Event Weight private:
  std::map<std::string, float> computeEventWeight (const std::vector<const xAOD::Jet*> &xaodjets, bool doSyst = false, std::string btc_name = "Default");
  std::map<std::string, float> compute_TruthTag_EventWeight (bool doSyst = false, std::string btc_name = "Default");

  ///////////////////////////////////////////////////////
  //Utils private:
  void PrintConfiguration(const BTagConfig& btc) {
      Info("PrintConfiguration()",
           "BTaggingTool configuration: Algorithm: %s, "
           "Author: %s, Efficiency: %s, Scheme: %s",
           btc.tagger.c_str(), btc.jetAuthor.c_str(), btc.btagcuteff.c_str(),
           btc.scheme.c_str());
  }

  void getBinFromEfficiency(std::string& btagcuteff, int &effbin) const;

  std::string getFlavorLabel (const xAOD::Jet &jet) const;

  int getFlavor (const xAOD::Jet &jet) const;
  ///////////////////////////////////////////////////////
  //Direct Tagging underlying functions:
  bool isJetTagged (const xAOD::Jet &jet, std::shared_ptr<BTaggingSelectionTool> &select_ptr, 
		    TaggerType taggerEnum, bool useContinuous = false, std::string trsh = "70") const;

  bool isJetTaggedVeto (const xAOD::Jet &jet, std::shared_ptr<BTaggingSelectionTool> &select_ptr,
			TaggerType taggerEnum, TaggerType taggerEnum_veto) const;


  float getJetTaggerWeight (const xAOD::Jet &jet, std::shared_ptr<BTaggingSelectionTool> &select_ptr, TaggerType taggerEnum) const;
  
///////////////////////////////////////////////////////
//VHcc specific private functions:

  void GetVhccEffs(std::vector<float> &jet_effs, std::vector<float> &jet_ineffs,
		   std::vector<float> &jet_directtag_SFs,std::shared_ptr<BTaggingEfficiencyTool> efftool_nominal, 
		   std::shared_ptr<BTaggingEfficiencyTool> efftool_veto,std::vector<const xAOD::Jet*> &xaodjets, std::string btc_name, std::string description);

///////////////////////////////////////////////////////
//Truth Tagging helpers & structures:

// when truth tagging, if we want to require a particular set of jets to be
// tagged, we can split into two groups and enforce different requirments on
// each group. the final truth tagging event weight is the product of the two
// weights

 struct TruthTagJets {
   std::vector<float> pt;
   std::vector<float> eta;
   std::vector<int> flav;
   std::vector<float> tagw;
   std::vector<int> signaljet_index;
   int n_requiredTags;
   Analysis::TruthTagResults tt_results;

   void clear() {
     pt.clear();
     eta.clear();
     flav.clear();
     tagw.clear();
     signaljet_index.clear();
     n_requiredTags = 0;
     tt_results.clear();
   }

   int size() { return pt.size(); }

   bool hasJetIndex(int indx) {
     for (unsigned int i = 0; i < signaljet_index.size(); i++) {
       if (indx == signaljet_index[i]) {
	 return true;
       }
     }
     return false;
   }
   bool istagged(int ijet, bool exclusive) {
     for (unsigned int i = 0; i < signaljet_index.size(); i++) {
       if (ijet == signaljet_index[i]) {
	 std::vector<bool> chosen_permuation =
	   tt_results.getEventPermutation(n_requiredTags, exclusive);
	 return chosen_permuation[i];
       }
     }
     return false;
   }
   float tagweight(int ijet, bool exclusive) {
     for (unsigned int i = 0; i < signaljet_index.size(); i++) {
       if (ijet == signaljet_index[i]) {
	 std::vector<float> randomTaggerScores =
	   tt_results.getRandomTaggerScores(n_requiredTags, exclusive);
	 return randomTaggerScores[i];
       }
     }
     return -99;
   }
   float quantile(int ijet, bool exclusive) {
     for (unsigned int i = 0; i < signaljet_index.size(); i++) {
       if (ijet == signaljet_index[i]) {
	 std::vector<int> quantiles =
	   tt_results.getEventQuantiles(n_requiredTags, exclusive);
	 return quantiles[i];
       }
     }
     return -99;
   }
   float getEventWeight(bool exclusive, std::string syst_name = "Nominal") {
     if (pt.size() == 0) {
       return 1.0;
     }
     return tt_results.getEventWeight(n_requiredTags, exclusive, syst_name);
   }
 };

 std::map<std::string, TruthTagJets> m_truthtag_jets;
 std::map<std::string, TruthTagJets> m_truthtag_jets_groupB;
 // a vector to store b-jets that are used in hybrid truth tagging.
 std::map<std::string, std::vector<const xAOD::Jet *> > m_hybrid_jets;

 void SelectJetsForTruthTagging(TruthTagJets& tt_jets, TruthTagJets& tt_jets_groupB,
				const std::vector<const xAOD::Jet*> &signalJets, const BTagConfig& btc) const;

};
#endif  // ifndef CxAODTools_BTaggingTool_H
