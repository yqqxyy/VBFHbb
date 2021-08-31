#include "CorrsAndSysts/BDTReweightTool.h"
#include "TError.h"
#include "TFile.h"
#include "TKey.h"
#include "TLeaf.h"
#include "TPRegexp.h"
#include "TRegexp.h"
#include "TTree.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <typeinfo>

// Important:
//---------------------------------------------------
// Add a new BDTr variation: Go to this function -->
// --> void BDTReweightTool::intializeBDTrVariations(TString xmlPath, int
// channel)

// Legend of the BDtr Variation constructor:
// BDTrVariation* myname = new BDTrVariation("variation name", "histogram name",
// "xml name", "shape file", flavLabel, nJet, useTF1, "mvaName")
//---------------------------------------------------

//======================================================================
//                BDTrVariation (struct) Functions
//======================================================================

// Constructor: When NOT specified, the MVA Name and the XMLFiles
// are automatically computed following the Moriond2020 convention
BDTrVariation::BDTrVariation(TString var_name, TString hist_name, TString xml,
                             TString shape_file, FlavLabel flav_label, int nJet,
                             int channel, bool use_tf1, TString mva_name) {

  // variation
  this->name = var_name;

  // name of the shape histogram
  this->histName = hist_name;

  // Now the xml name
  if (xml.Contains("weights.xml")) { // you can speficy the full Path
    Info("BDTrVariation()", "Found xml file with full name specified %s ",
         xml.Data());
    this->xmlFile = xml;
  } else { // or just the Path in case of Moriond2020 XMLs
    TString xml_moriond =
        DetermineXMLName(var_name, nJet, channel, flav_label,
                         xml); // Follow Moriond 2020 convention
    this->xmlFile = xml_moriond;
  }

  // use TH1 or TF1 shapes
  this->useTF1 = use_tf1;

  // nJets and flavour labels. See xml for the convention
  this->nJ = nJet;
  this->flavLabel = flav_label;
  this->channel = channel;

  // Now determine the MVAName to evaluate
  // DetermineMVAFoldName(0, 2, mvaName); --> mvaName = BDT::MVACat_0flav_2j
  // If not specified, Follow the Moriond2020 convention
  if (mva_name != "") {
    this->mvaName = mva_name;
    Info("BDTrVariation()", "Found MVAName: %s ", mva_name.Data());
  } else
    mva_name =
        DetermineMVAFoldName(flav_label, nJet); // follow Moriond2020 convention

  this->mvaName = mva_name;

  // retrieve the shape histograms
  if (use_tf1)
    this->shapeFunc = GetShapeFunction(hist_name, shape_file);
  else
    this->shapeHist = GetShapeHistogram(hist_name, shape_file);

  // intialize the related TMVAReader
  this->TMVAReader = new TMVA::Reader("V:Color:Silent");
}

TH1 *BDTrVariation::GetShapeHistogram(TString name, TString shape_file) {
  TFile *file = TFile::Open(shape_file.Data());
  file->cd();
  TH1 *histo = static_cast<TH1 *>((file->Get(name))->Clone());
  if (histo == nullptr)
    Error("BDTrVariation::GetHistogram()",
          "Histogram %s not found. "
          "Please check the input File",
          name.Data());
  //  histo->SetDirectory(0);
  file->Close();
  return histo;
}

TF1 *BDTrVariation::GetShapeFunction(TString name, TString shape_file) {
  TFile *file = TFile::Open(shape_file.Data());
  file->cd();
  TF1 *func = static_cast<TF1 *>((file->Get(name))->Clone());
  if (func == nullptr)
    Error("BDTrVariation::GetFunction()",
          "Function %s not found. "
          "Please check the input File",
          name.Data());
  file->Close();
  return func;
}

// Follow Moriond 2020 convention
TString BDTrVariation::DetermineXMLName(const TString &variation, int nJet,
                                        int channel, FlavLabel flav_label,
                                        TString &xmlPath) {

  TString XMLname = xmlPath + "/TMVAClassification_BDTCategories_";
  XMLname += std::to_string(channel) + "lep_" + std::to_string(nJet) + "jet_";
  XMLname += variation;

  if (variation.Contains("ttbar")) {
    if (flav_label == FlavLabel::bb)
      XMLname += "_bb";
    else if (flav_label == FlavLabel::bc)
      XMLname += "_bc";
    else if (flav_label == FlavLabel::bl)
      XMLname += "_oth";
  } else if (variation.Contains("W"))
    XMLname += "_HF";

  XMLname += ".weights.xml";

  Info("BDTrVariation::DetermineXMLName()", "Reading XML '%s'.",
       XMLname.Data());

  return XMLname;
}

// Follow Moriond 2020 convention
TString BDTrVariation::DetermineMVAFoldName(FlavLabel flavLabel, int nJet) {
  TString mvaName = "BDT::MVACat_";
  mvaName += std::to_string(flavLabel) + "flav_" + std::to_string(nJet) + "j";

  Info("BDTrVariation::DetermineMVAFoldName()", "MVAToEval is '%s'.",
       mvaName.Data());

  return mvaName;
}

//======================================================================
//                        BDTrSyst Functions
//======================================================================

// Constructors:
BDTReweightTool::~BDTReweightTool() {}

BDTReweightTool::BDTReweightTool(int channel, TTree *tree, bool debug,
                                 TString AppName) {

  Info("BDTReweightTool::BDTReweightTool()",
       "Creating BDT reweighting object with name: %s", AppName.Data());
  m_debug = debug;
  m_AppName = AppName;
  m_variables.clear();
  m_BDTr_variations.clear();

  Initialise(channel, tree);
}

void BDTReweightTool::Initialise(int channel, TTree *tree) {

  // now initialize the XML files
  std::string xmlPath = m_WorkDir_DIR + "/data/CorrsAndSysts/";
  Info("BDTrSyst::Initialise()", "setting the path of BDTr XML files to: %s",
       xmlPath.c_str());

  // define the BDTr ID for this sample + set the variations
  intializeBDTrVariations(xmlPath, channel);

  // Now add the list of variables to BDtr depending on the m_BDTrType and the
  // channel
  AddBDTrVariablesAndBook(tree);

  CreateReverseBDTrVariationMap();

  // print a summary
  Info("BDTReweightTool::Initialize()",
       "Initializing BDTReweightTool: %d lepton channel.", channel);

  Info("BDTReweightTool::Initialize()",
       "All initialisation settings for"
       " BDTReweightTool have been completed.");

  return;
}

// weightfile "dump".
// ---------------------
// Important 2: xmlPath --> The XML file can:
// - take the **full XML name** for quick testing (xmlPath +
// <myname>.weight.xml) or..
// - NOW: the Moriond 2020 xml name convention is automatically retrieved. Just
// give the xmlPath.
void BDTReweightTool::intializeBDTrVariations(TString xmlPath, int channel) {

  // Common shape file
  //-----------------
  // Now Also different files for each process are accepted.
  // Please specify the Full Path in the constructor.

  std::string shapeFile = m_WorkDir_DIR + "/data/CorrsAndSysts/Shapes_" +
                          std::to_string(channel) + "Lepton.root";

  Info("BDTrSyst::Initialise()",
       "setting the path of BDTr common shape file to: %s", shapeFile.c_str());

  TString pTVtag = "_75ptv_";
  if (channel == 0)
    pTVtag = "_150ptv_";

  TString xmlFile = "";
  TString mvaName = ""; // Tag of the BDTs to Evaluate WITHOUT kFold Tag

  // Initialize BDTr Variations

  // Important: Legend of the BDtr Variation constructor.
  //---------------------------------------------------
  // BDTrVariation* myname = new BDTrVariation("variation name", "histogram
  // name", "xml name", "shape file", flavLabel, nJet, useTF1, "mvaName") See
  // Also header file for more details.
  //---------------------------------------------------

  // Initialize BDTr Variations for TTBAR
  // Tip: when mvaName is not specified, it is automatically defined following
  // the convention in: DetermineMVAFoldName(0, 2, mvaName); --> mvaName =
  // BDT::MVACat_0flav_2j Convention: 0 = bb, 1 = bc, 2 = oth (here = bl)

  //    =================== ttbar ME - 2J ========================
  m_BDTr_variations[nJ2_ttbar_ME_bb] =
      new BDTrVariation("ttbar_ME", "ttbar_ME_2tag2jet" + pTVtag + "bb",
                        xmlPath, shapeFile, FlavLabel::bb, 2, channel, true);

  m_BDTr_variations[nJ2_ttbar_ME_bc] =
      new BDTrVariation("ttbar_ME", "ttbar_ME_2tag2jet" + pTVtag + "bc",
                        xmlPath, shapeFile, FlavLabel::bc, 2, channel, true);

  m_BDTr_variations[nJ2_ttbar_ME_oth] =
      new BDTrVariation("ttbar_ME", "ttbar_ME_2tag2jet" + pTVtag + "oth",
                        xmlPath, shapeFile, FlavLabel::bl, 2, channel, true);

  //    =================== ttbar ME - 3J ========================
  m_BDTr_variations[nJ3_ttbar_ME_bb] =
      new BDTrVariation("ttbar_ME", "ttbar_ME_2tag3jet" + pTVtag + "bb",
                        xmlPath, shapeFile, FlavLabel::bb, 3, channel, true);

  m_BDTr_variations[nJ3_ttbar_ME_bc] =
      new BDTrVariation("ttbar_ME", "ttbar_ME_2tag3jet" + pTVtag + "bc",
                        xmlPath, shapeFile, FlavLabel::bc, 3, channel, true);

  m_BDTr_variations[nJ3_ttbar_ME_oth] =
      new BDTrVariation("ttbar_ME", "ttbar_ME_2tag3jet" + pTVtag + "oth",
                        xmlPath, shapeFile, FlavLabel::bl, 3, channel, true);

  //    =================== ttbar PS - 2J ========================
  m_BDTr_variations[nJ2_ttbar_PS_bb] =
      new BDTrVariation("ttbar_PS", "ttbar_PS_2tag2jet" + pTVtag + "bb",
                        xmlPath, shapeFile, FlavLabel::bb, 2, channel, true);

  m_BDTr_variations[nJ2_ttbar_PS_bc] =
      new BDTrVariation("ttbar_PS", "ttbar_PS_2tag2jet" + pTVtag + "bc",
                        xmlPath, shapeFile, FlavLabel::bc, 2, channel, true);

  m_BDTr_variations[nJ2_ttbar_PS_oth] =
      new BDTrVariation("ttbar_PS", "ttbar_PS_2tag2jet" + pTVtag + "oth",
                        xmlPath, shapeFile, FlavLabel::bl, 2, channel, true);

  //    =================== ttbar PS - 3J ========================
  m_BDTr_variations[nJ3_ttbar_PS_bb] =
      new BDTrVariation("ttbar_PS", "ttbar_PS_2tag3jet" + pTVtag + "bb",
                        xmlPath, shapeFile, FlavLabel::bb, 3, channel, true);

  m_BDTr_variations[nJ3_ttbar_PS_bc] =
      new BDTrVariation("ttbar_PS", "ttbar_PS_2tag3jet" + pTVtag + "bc",
                        xmlPath, shapeFile, FlavLabel::bc, 3, channel, true);

  m_BDTr_variations[nJ3_ttbar_PS_oth] =
      new BDTrVariation("ttbar_PS", "ttbar_PS_2tag3jet" + pTVtag + "oth",
                        xmlPath, shapeFile, FlavLabel::bl, 3, channel, true);

  //-------------------------------------------------------------//

  // Initialize BDTr Variations for WJET
  // DetermineMVAFoldName(0, 2, mvaName); --> mvaName = BDT::MVACat_0flav_2j
  // Convention: 0 = bb, 1 = bc, 2 = bl, 3 = cc

  //    =================== W_SHtoMG5_HF - 2J ========================

  // DetermineXMLName("W_SHtoMG5_HF", 2, xmlPath, xmlFile);
  m_BDTr_variations[nJ2_W_SHtoMG5_HF_bb] =
      new BDTrVariation("W_SHtoMG5", "W_SHtoMG5_2tag2jet" + pTVtag + "bb",
                        xmlPath, shapeFile, FlavLabel::bb, 2, channel);

  m_BDTr_variations[nJ2_W_SHtoMG5_HF_bc] =
      new BDTrVariation("W_SHtoMG5", "W_SHtoMG5_2tag2jet" + pTVtag + "bc",
                        xmlPath, shapeFile, FlavLabel::bc, 2, channel);

  m_BDTr_variations[nJ2_W_SHtoMG5_HF_bl] =
      new BDTrVariation("W_SHtoMG5", "W_SHtoMG5_2tag2jet" + pTVtag + "bl",
                        xmlPath, shapeFile, FlavLabel::bl, 2, channel);

  m_BDTr_variations[nJ2_W_SHtoMG5_HF_cc] =
      new BDTrVariation("W_SHtoMG5", "W_SHtoMG5_2tag2jet" + pTVtag + "cc",
                        xmlPath, shapeFile, FlavLabel::cc, 2, channel);

  //    =================== W_SHtoMG5_HF - 3J ========================
  m_BDTr_variations[nJ3_W_SHtoMG5_HF_bb] =
      new BDTrVariation("W_SHtoMG5", "W_SHtoMG5_2tag3jet" + pTVtag + "bb",
                        xmlPath, shapeFile, FlavLabel::bb, 3, channel);

  m_BDTr_variations[nJ3_W_SHtoMG5_HF_bc] =
      new BDTrVariation("W_SHtoMG5", "W_SHtoMG5_2tag3jet" + pTVtag + "bc",
                        xmlPath, shapeFile, FlavLabel::bc, 3, channel);

  m_BDTr_variations[nJ3_W_SHtoMG5_HF_bl] =
      new BDTrVariation("W_SHtoMG5", "W_SHtoMG5_2tag3jet" + pTVtag + "bl",
                        xmlPath, shapeFile, FlavLabel::bl, 3, channel);

  m_BDTr_variations[nJ3_W_SHtoMG5_HF_cc] =
      new BDTrVariation("W_SHtoMG5", "W_SHtoMG5_2tag3jet" + pTVtag + "cc",
                        xmlPath, shapeFile, FlavLabel::cc, 3, channel);
}

// Place here the special Hacks in case some naming convention between the TTree
// and the xml file is different.
void BDTReweightTool::RenameVariables(TString &varName, int& channel) {

  if (varName == "mTop")
    varName = "Mtop";

  else if (varName == "EventNumber")
    varName = "EventNumber_bdtr";

  else if (varName == "nJ")
    varName = "nJ_bdtr";

  else if(channel == 0 && varName =="HT")
    varName = "HT_bdtr";

  else if(channel == 0 && varName =="dEtaBB")
    varName = "dEtaVBB";
  
  return;
}
void BDTReweightTool::AddBDTrVariablesAndBook(TTree *tree) {

  for (auto pair : m_BDTr_variations) {
    auto bdtr = pair.second;

    std::vector<TString> expMVAinput;
    std::vector<TString> expMVAspectator;
    std::vector<TString> expMVABDTs;

    getExpressions(bdtr->xmlFile, expMVAinput, expMVAspectator, expMVABDTs);

    Info("BDTReweightTool::AddBDTrVariablesAndBook()",
         "booking MVA reader for file: %s", bdtr->xmlFile.Data());

    TObjArray *listOfLeaves = tree->GetListOfLeaves();

    for (unsigned int invar_i = 0; invar_i < expMVAinput.size(); invar_i++) {
      TString expName = expMVAinput[invar_i]; // hack check it
      TString varName = expName;

      // Hack for Moriond2020
      RenameVariables(varName, bdtr->channel);

      if (listOfLeaves->Contains(varName)) {
        if (m_variables.find(varName) == m_variables.end())
          m_variables[varName] =
              (float *)(tree->GetLeaf(varName)->GetValuePointer());
      } else {
        Error("BDTReweightTool::AddBDTrVariablesAndBook()",
              "No leaf with name '%s' in tree", varName.Data());
        exit(EXIT_FAILURE);
      }

      if (m_debug)
        Info("BDTReweightTool::AddBDTVariablesAndBook()",
             "setting variable: %s", varName.Data());

      bdtr->TMVAReader->AddVariable(expName, m_variables[varName]);
    }

    for (unsigned int spec_i = 0; spec_i < expMVAspectator.size(); spec_i++) {
      TString expSpec = expMVAspectator[spec_i];
      TString specName = expSpec;

      RenameVariables(specName, bdtr->channel);
      if (listOfLeaves->Contains(specName)) {
        if (m_variables.find(specName) == m_variables.end())
          m_variables[specName] =
              (float *)(tree->GetLeaf(specName)->GetValuePointer());
      } else {
        Error("BDTReweightTool::AddBDTrVariablesAndBook()",
              "No leaf with name '%s' in tree", specName.Data());
        exit(EXIT_FAILURE);
      }

      if (m_debug)
        Info("BDTReweightTool::AddBDTrVariablesAndBook()",
             "setting spectator: %s", specName.Data());

      bdtr->TMVAReader->AddSpectator(expSpec, m_variables[specName]);
    }

    // store only the methods needed for this Variation.
    // the methids are stored in bdtr->mvaFolds
    for (unsigned int i = 0; i < expMVABDTs.size(); i++) {

      if (expMVABDTs[i].Contains(bdtr->mvaName)) {

        Info("BDTReweightTool::AddBDTrVariablesAndBook()",
             "Booking MVA: %s for BDTSyst: %s ", expMVABDTs[i].Data(),
             bdtr->histName.Data());

        bdtr->mvaFolds.push_back(expMVABDTs[i]);
        bdtr->TMVAReader->BookMVA(expMVABDTs[i], bdtr->xmlFile);
      }
    }

    if (bdtr->mvaFolds.size() == 0)
      Error("BDTReweightTool::AddBDTrVariablesAndBook()",
            "No mvaFolds found for %s ", bdtr->mvaName.Data());
  }
  return;
}

void BDTReweightTool::getExpressions(TString xmlFile,
                                     std::vector<TString> &variables,
                                     std::vector<TString> &spectators,
                                     std::vector<TString> &BDTs) {

  TXMLEngine *xmlEngine = new TXMLEngine();
  XMLDocPointer_t doc = xmlEngine->ParseFile(xmlFile);
  if (!doc) {
    Error("ReweightTool::getExpressions()", "Could not parse XML file '%s'!",
          xmlFile.Data());
    exit(EXIT_FAILURE);
  }

  XMLNodePointer_t node = xmlEngine->DocGetRootElement(doc);
  if (!node) {
    Error("ReweightTool::getExpressions()",
          "Could get root node from XML file '%s'!", xmlFile.Data());
    exit(EXIT_FAILURE);
  }

  // Variables
  //------------
  // Go to level of Variables:
  node = xmlEngine->GetChild(node);
  // Loop over nodes:
  while (node) {
    TString nodeName = xmlEngine->GetNodeName(node);
    // Read Expression from Variable:
    if (nodeName == "Variable") {
      const char *expr = xmlEngine->GetAttr(node, "Expression");
      variables.push_back(TString(expr));
    }
    if (nodeName == "Variables") {
      // Go to level of Variable:
      node = xmlEngine->GetChild(node);
    } else {
      // Next node:
      node = xmlEngine->GetNext(node);
    }
  }

  // Spectators
  //-----------
  // Have to to this loop twice to get spectators, but could be written better!
  node = xmlEngine->DocGetRootElement(doc);
  node = xmlEngine->GetChild(node);
  while (node) {
    TString nodeName = xmlEngine->GetNodeName(node);
    // Read Expression from Spectator:
    if (nodeName == "Spectator") {
      const char *expr = xmlEngine->GetAttr(node, "Expression");
      if (expr) {
        spectators.push_back(TString(expr));
      }
    }
    if (nodeName == "Spectators") {
      // Go to level of Spectator:
      node = xmlEngine->GetChild(node);
    } else {
      // Next node:
      node = xmlEngine->GetNext(node);
    }
  }

  // BDTs
  //------
  // Now loop to get the available BDTs
  node = xmlEngine->DocGetRootElement(doc);
  node = xmlEngine->GetChild(node);
  while (node) {
    TString nodeName = xmlEngine->GetNodeName(node);
    // Read Expression from Spectator:
    if (nodeName == "SubMethod") {
      const char *expr = xmlEngine->GetAttr(node, "Method");
      BDTs.push_back(TString(expr));
    }
    if (nodeName == "Weights") {
      // Go to level of Spectator:
      node = xmlEngine->GetChild(node);
    } else {
      // Next node:
      node = xmlEngine->GetNext(node);
    }
  }
  xmlEngine->CleanNode(xmlEngine->DocGetRootElement(doc));
  delete xmlEngine;
  return;
}

void BDTReweightTool::DetermineWeightAndScore(BDTSyst bdtr_var,
                                              long long int EventNumber,
                                              float &reweight,
                                              float &BDTScore) {

  BDTScore = -9999.;
  reweight = 1;

  // retrieve correct BDTr variation:
  if (!m_BDTr_variations.count(bdtr_var)) {
    Info("BDTReweightTool::DetermineWeightAndScore()",
         " No ratio files or BDT score has been found for this event.");
    return;
  }

  auto bdtr = m_BDTr_variations.at(bdtr_var);

  if (m_debug) {
    Info("BDTReweightTool::DetermineBDTScore()",
         "Looking at sample '%s' with variation %s .", m_sample.Data(),
         bdtr->name.Data());

    std::vector<TString> expressions, spectators, BDTs;
    getExpressions(bdtr->xmlFile, expressions, spectators, BDTs);

    for (unsigned int i = 0; i < expressions.size(); ++i) {
      // Hack for Moriond2020
      RenameVariables(expressions.at(i), bdtr->channel);

      Info("BDTReweightTool::DetermineBDTScore()",
           " Found value %f for variable '%s'.",
           *m_variables[expressions.at(i)], expressions.at(i).Data());
    }
    for (unsigned int i = 0; i < spectators.size(); ++i) {
      RenameVariables(spectators.at(i), bdtr->channel);

      Info("BDTReweightTool::DetermineBDTScore()",
           " Found value %f for spectator '%s'.",
           *m_variables[spectators.at(i)], spectators.at(i).Data());
    }

    for (unsigned int i = 0; i < BDTs.size(); ++i) {
      Info("BDTReweightTool::DetermineBDTScore()", " Found BDT '%s'.",
           BDTs.at(i).Data());
    }
  } // m_debug

  if (bdtr->mvaFolds.size() == 0) {
    Error("DetermineWeightAndScore()", "No MVAFolds, please check settings!");
    exit(EXIT_FAILURE);
  }

  TString MVAToEval = bdtr->mvaFolds.at(0);

  if (bdtr->mvaFolds.size() > 1) {
    MVAToEval =
        (EventNumber % 2 == 0) ? bdtr->mvaFolds.at(0) : bdtr->mvaFolds.at(1);
  }

  if (bdtr->mvaFolds.size() > 2) {
    Error("BDTReweightTool::DetermineBDTScore()",
          "More than two MVAFolds stored. Please Check the MVAToEval Rule! "
          "Exiting..");
    return exit(EXIT_FAILURE);
  }

  BDTScore = bdtr->TMVAReader->EvaluateMVA(MVAToEval);

  if (m_debug)
    Info("BDTReweightTool::DetermineBDTScore()", "BDT value: %f", BDTScore);

  if (bdtr->useTF1) {
    reweight = bdtr->shapeFunc->Eval(BDTScore);
  } else {
    reweight =
        bdtr->shapeHist->GetBinContent(bdtr->shapeHist->FindBin(BDTScore));
  }

  if (m_debug)
    Info("BDTReweightTool::DetermineBDTScore()", "reweight value: %f",
         reweight);

  return;
}

// Utils:
//---------------------------
void BDTReweightTool::CreateReverseBDTrVariationMap() {

  for (auto pair : m_BDTr_variations) {
    auto bdtr_key = pair.first;
    auto bdtr = pair.second;

    TString variation = bdtr->name;

    // If you need special hacks for testing new xmlFiles:
    // TRegexp re("_[bc|bl|cc|oth|HF]$");
    // variation(re) = "";
    // std::cout <<"variation after regexp  " <<variation <<std::endl;

    int nJ = bdtr->nJ;
    FlavLabel FlavourLabel = bdtr->flavLabel;

    if (m_debug)
      Info("CreateReverseBDTrVariationMap()",
           "BDTr reverse map: %s nJ %d flavLabel %d key %d", variation.Data(),
           nJ, (int)FlavourLabel, bdtr_key);

    m_BDTr_var_reverse[variation][nJ][FlavourLabel] = bdtr_key;
  }
  return;
}

BDTSyst BDTReweightTool::getBDTrVariation(TString variation, int nJ,
                                          int FlavourLabel) {

  if (!m_BDTr_var_reverse.count(variation)) {
    Error("BDTReweightTool::getBDTrVariation()",
          "No variation: %s"
          "found in the reverse map. Please check the name in BDTReweightTool.",
          variation.Data());
    exit(EXIT_FAILURE);
  }

  BDTSyst bdtr_var = m_BDTr_var_reverse.at(variation).at(nJ).at(FlavourLabel);

  if (m_debug)
    Info("BDTReweightTool::getBDTrVariation()", "Returning BDTSyst: %d ",
         bdtr_var);

  return bdtr_var;
}
