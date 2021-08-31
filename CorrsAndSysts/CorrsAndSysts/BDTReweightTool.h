#ifndef BDTReweightTool_h
#define BDTReweightTool_h

#include "TF1.h"
#include "TMVA/Config.h"
#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"
#include "TString.h"
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <map>
#include <set>
#include <string>
#include <vector>

//---------------------------
// The BDTr Sys are handled by a dedicated struct
// named "BDTrVariation".
// this struct contains the following informations
// name;      //name of the variation eg. ttbar_ME, ttbar_PS
// histName;  //name of the shape histogram as saved in the shape file
// xmlFile;   //name of the xml file for TMVAReader
// mvaName;   //**common** name of the SubMethods within the XML to be
// considered for this Variation
//            //eg. BDT::MVACat_0flav_2j. !!!Do not specify the FOLD!!!
// mvaFolds;  //vector containing the two BDT names first 1o2 then 2o2
// eg. BDT::MVACat_0flav_2j_1o2 and _2o2
// it automatically search for a match with mvaName!
// useTF1 ;   //in case the weights are stored as functions
// channel;   //analysis lepton channel 0,1,2
// nJ;        //number of jets for this variation
// flavLabel; //flavour label convention
// as specified in HistSvc.
// eg.  bb = 0, bc = 1, bl = 2, cc = 3.

// Store the shapes as well:
// shapeHist;   //histogram storing the shape of the reweighting
// in case the shape is stored as a function:
// shapeFunc;   //function storing the shape of the reweighting

// To add anew BDTr Variation you just need to:
// 1) Add a new entry to the BDTSyst list

// 2) Add a new entry to:

// ---> void intializeBDTrVariations(TString xmlPath, int channel);

// using the constructor:

// ---> BDTrVariation(TString var_name, TString hist_name, TString xml, TString
// shape_file, FlavLabel flav_label, int nJet, int channel, bool use_tf1 =
// false, TString mva_name = "");
//
// a reverse_map is then automatically created and you can retrieve the
// variation in AnalysisReader_VHQQ with:

// --->  BDTSyst getBDTrVariation(TString variation, int nJ, int FlavourLabel);

//--------------------------------------------

// The weights and the BDTScores are computed in:
//

enum BDTrAnaType {
  Boosted,
  Resolved,
  VHcc,
};

enum BDTSyst {
  // ttbar ME
  nJ2_ttbar_ME_bb,
  nJ2_ttbar_ME_bc,
  nJ2_ttbar_ME_oth,
  nJ3_ttbar_ME_bb,
  nJ3_ttbar_ME_bc,
  nJ3_ttbar_ME_oth,

  // ttbar PS
  nJ2_ttbar_PS_bb,
  nJ2_ttbar_PS_bc,
  nJ2_ttbar_PS_oth,
  nJ3_ttbar_PS_bb,
  nJ3_ttbar_PS_bc,
  nJ3_ttbar_PS_oth,

  // W+jet
  // Sh vs MG
  nJ2_W_SHtoMG5_HF_bb,
  nJ2_W_SHtoMG5_HF_bc,
  nJ2_W_SHtoMG5_HF_bl,
  nJ2_W_SHtoMG5_HF_cc,

  nJ3_W_SHtoMG5_HF_bb,
  nJ3_W_SHtoMG5_HF_bc,
  nJ3_W_SHtoMG5_HF_bl,
  nJ3_W_SHtoMG5_HF_cc,
};

// Define Flavour Fractions
// Follow the same convention as HistSvc
enum FlavLabel { bb, bc, bl, cc };

struct BDTrVariation {
  TString name = "";     // name of the variation
  TString histName = ""; // name of the shape histogram
  TString xmlFile = "";  // name of the xml file
  TString mvaName = ""; // name of the SubMethods within the XML to be considered for
                   // this Variation eg. BDT::MVACat_0flav_2j_1o2,
                   // BDT::MVACat_0flav_2j_2o2
  bool useTF1;     // in case the weights are stored as functions
  int channel = -1;     // analysis lepton channel 0,1,2
  int nJ = -1;          // number of jets to be applied to
  FlavLabel
      flavLabel; // flavour label convention
                 // as specified in HistSvc.
                 // eg. used for training Wjet: bb = 0, bc = 1, bl = 2, cc = 3.

  // Store the shapes as well:
  TH1 *shapeHist = nullptr; // histogram storing the shape of the reweighting
  // in case the shape is stored as a function:
  TF1 *shapeFunc = nullptr; // function storing the shape of the reweighting

  std::vector<TString>
      mvaFolds = {}; // vector containing the two BDT names first 1o2 then 2o2

  // The XML reader associated to this variation
  TMVA::Reader *TMVAReader = nullptr;

  // default constructor
  BDTrVariation();

  // destructor
  ~BDTrVariation(){};

  // constructor
  BDTrVariation(TString var_name, TString hist_name, TString xml,
                TString shape_file, FlavLabel flav_label, int nJet, int channel,
                bool use_tf1 = false, TString mva_name = "");

  // add some helpers
  TF1 *GetShapeFunction(TString var_name, TString shape_file);
  TH1 *GetShapeHistogram(TString var_name, TString shape_file);

  // These follow the Moriond2020 convention.
  // you can bypass it by directly specifying the mva_foldName
  TString DetermineMVAFoldName(FlavLabel flavLabel, int nJet);

  // you can bypass this by directly specifying the full file
  TString DetermineXMLName(const TString &variation, int nJet, int channel,
                           FlavLabel flav_label, TString &xmlPath);
};

class BDTReweightTool {
public:
  // default constructor
  BDTReweightTool();

  // destructor
  ~BDTReweightTool();

  // full constructor
  BDTReweightTool(int channel, TTree *tree, bool debug,
                  TString AppName = "App_BDTr");

  void Initialise(int channel, TTree *tree);

  // Main "dumper" for the BDTr Variations.
  // Please Add the BDTVariations constructors here
  void intializeBDTrVariations(TString xmlPath, int channel);

  // Automatically add the variables to the TMVAReader method
  // as *float pointers from the *TTree specified at initialization level
  // Note: the vairables MUSt be initialized as float in the TTree!!
  void AddBDTrVariablesAndBook(TTree *tree);

  // Automatically get the variables directly from the XML
  void getExpressions(TString xmlFile, std::vector<TString> &variables,
                      std::vector<TString> &spectators,
                      std::vector<TString> &BDTs);

  // Function to determine the BDT Score and the reweight
  void DetermineWeightAndScore(BDTSyst bdtr_var, long long int EventWeight,
                               float &weight, float &BDTScore);

  void setSample(const TString sample) {
    m_sample = sample;
    return;
  }

  // Function to be used in the AnalysisCode to retrieve the BDTSyst type to be
  // applied for a given variation. This speeds up the code as we are now
  // dealing with numbers and not strings.
  BDTSyst getBDTrVariation(TString variation, int nJ, int FlavourLabel);

private:
  TString m_sample = ""; // MCsample eg ttbar, Wjet
  TString
      m_AppName = ""; // Name of the BDtReweightingTool, in case multiple are needed
  int m_debug = 0;   // activate debug

  //  BDTVariables variables;
  std::map<TString, float *> m_variables = {};

  std::map<BDTSyst, BDTrVariation *> m_BDTr_variations = {};
  std::map<TString, std::map<int, std::map<int, BDTSyst>>> m_BDTr_var_reverse = {};

  // now Utils:
  std::string m_WorkDir_DIR = gSystem->Getenv("WorkDir_DIR");

  // fill the m_BDTr_var_reverse map such as the variation can be retrieved as:
  // m_BDTr_var_reverse[variation][nJ][FlavourLabel]
  // loop over the m_BDTr_variations list.
  void CreateReverseBDTrVariationMap();

  // Function for special hacks in the variables
  void RenameVariables(TString &varName, int& channel);
};

#endif // BDTReweightTool_HPP_
