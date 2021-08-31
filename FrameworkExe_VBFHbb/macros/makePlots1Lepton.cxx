#include "PlottingTool/Config.h"
#include "PlottingTool/PlotMaker.h"

#include <iostream>


void makePlots1Lepton(std::string InputPath,std::string InputFile) {

  Config config;

  // should we overwrite existing plots or only do missing ones?
  config.setOverwrite(true);

  /////////
  /// Input

  // path to the input file: can be local or on eos
  config.setInputPath(InputPath);

  // Since your input file name might not follow the conventions above,
  // it can be overwritten using the line below
  // Note: specify the name WITHOUT ".root" at the end
  config.setInputFile(InputFile);

  // If input histograms are stored in a subdirectory of the input file, specify below
  // config.setInputTDirectory("OneLepton");

  // read lumi from input file
  std::string lumi_str("5");
  TFile* inputfile=TFile::Open(config.getInputFileFullName("").c_str(),"READ");
  TH1* hlumi= (inputfile) ? (TH1*)inputfile->Get("lumi") : 0;
  if(hlumi) {
    float lumi_fb=hlumi->GetBinContent(1)/1000.0;
    lumi_str=Form("%.2f",lumi_fb);
    inputfile->Close();
  }
  cout << "Using luminosity " << lumi_str << endl;

  ////////////////
  /// General info
  config.setAnalysis( Config::OneLepton,  // Type = ZeroLepton, OneLepton or TwoLepton: Appears on the plots
		      "Internal",         // Status: Appear on the plots next to ATLAS label
		      "LHCP2013",         // Name = "LHCP2013": only useful for the above naming convention
		      "13",               // Center of mass energy: Appears on the plots
		      lumi_str,           // Integrated luminosity: Appears on the plots
		      "125",              // Mass of the generated higgs boson: only useful for the above naming convention
		      "Iowa",             // Name of your institute: only usefule for the above naming convention
		      "v2.1");            // Input file version: only useful for the above naming convention


  //////////
  /// Output

  // path to output plots
  config.setOutputPath("./plots");

  // BDT transformations:
  config.addBDTTransformation(""); // no transformation

  /// Samples

  // declare the samples
  config.addDataSample("data",       // name: as it appears in the histogram names
  		       "Data",       // title: as it should appear in the legend
  		       1);           // color: this is marker(line+fill) color for data(MC)

  config.addSignalSample("qqZvvH125",  "VH 125", kRed);
  config.addSignalSample("WlvH125",    "VH 125", kRed);
  config.addSignalSample("qqZllH125",  "VH 125", kRed);

  config.addSignalSample("qqZvvH125",  "VH 125", kRed);
  config.addSignalSample("WlvH125",    "VH 125", kRed);
  config.addSignalSample("qqZllH125",  "VH 125", kRed);

  config.addSignalSample("AZh300llbb",  "m_{A}=300 GeV ", kRed+1);
  config.addSignalSample("AZh800llbb",  "m_{A}=800 GeV ", kRed+1);
  config.addSignalSample("AZh1000llbb",  "m_{A}=1000 GeV ", kRed+1);


  //
  config.addBackgroundSample("ttbar",  "ttbar", kOrange);
  config.addBackgroundSample("stops",  "single top", kOrange - 1);
  config.addBackgroundSample("stopt",  "single top", kOrange - 1);
  config.addBackgroundSample("stopWt", "single top", kOrange - 1);

  //
  config.addBackgroundSample("Wl",  "W+l",  kGreen - 9);
  config.addBackgroundSample("Wcl", "W+cl", kGreen - 6);
  config.addBackgroundSample("Wcc", "W+cc", kGreen + 1);
  config.addBackgroundSample("Wbl", "W+bl", kGreen + 2);
  config.addBackgroundSample("Wbc", "W+bc", kGreen + 3);
  config.addBackgroundSample("Wbb", "W+bb", kGreen + 4);
  config.addBackgroundSample("Zl",  "Z+l",  kAzure - 9);
  config.addBackgroundSample("Zcl", "Z+cl", kAzure - 8);
  config.addBackgroundSample("Zcc", "Z+cc", kAzure - 4);
  config.addBackgroundSample("Zbl", "Z+bl", kAzure + 1);
  config.addBackgroundSample("Zbc", "Z+bc", kAzure + 2);
  config.addBackgroundSample("Zbb", "Z+bb", kAzure + 3);

  //  std::string scaleFactorsTag = "SF_1L_nJ_0107";
  // config.readScaleFactors("scaleFactors/"+scaleFactorsTag+".txt");
  // config.setOutputPath(config.getOutputPath()+"/"+scaleFactorsTag);

  // To optimize plot readability , samples that contribute less than a fraction X are dropped from the legend
  //  config.setLegendThreshold(0.05);

  // To draw the histograms in no-stack mode
  //  config.setNoStack();

  // set total background name
  // this is not mandatory but would save CPU and memory
  // Note: if any background scale factors are different than one,
  //       this cannot be used and will be ignored
  // config.setTotalBackgroundName("bkg");

  config.clearRegions();

  // config.addRegion("2tag3jet_vpt120",                      // name: as it appears in histogram name
  //               "2 tags, 3 jets, p_{T}^{V} >= 120 GeV", // title: as it appears on the plots
  //               "2 tags");                              // slide title: used to decide which plots to appear on the same slide

  // Signal Regions
  config.addRegion("0ptag0pjet_0ptv_SR",
                   "0+ tag, 0+ jets, SR", "");
  config.addRegion("0tag0pjet_200ptv_SR",
                   "0 tag, 0+ jets, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("0tag0pjet_160_200ptv_SR",
                   "0 tag, 0+ jets, 160, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("0tag0pjet_120_160ptv_SR",
                   "0 tag, 0+ jets, 120, p_{T}^{V} #geq 160 GeV, SR", "");
  config.addRegion("0tag4pjet_200ptv_SR",
                   "0 tag, 4+ jets, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("0tag4pjet_120_160ptv_SR",
                   "0 tag, 4+ jets, 120, p_{T}^{V} #geq 160 GeV, SR", "");
  config.addRegion("1tag0pjet_200ptv_SR",
                   "1 tag, 0+ jets, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("1tag0pjet_120_160ptv_SR",
                   "1 tag, 0+ jets, 120, p_{T}^{V} #geq 160 GeV, SR", "");
  config.addRegion("1tag0pjet_160_200ptv_SR",
                   "1 tag, 0+ jets, 160, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("1tag3jet_120_160ptv_SR",
                   "1 tag, 3 jets, 120, p_{T}^{V} #geq 160 GeV, SR", "");
  config.addRegion("1tag3jet_160_200ptv_SR",
                   "1 tag, 3 jets, 160, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("1tag4pjet_200ptv_SR",
                   "1 tag, 4+ jets, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("1tag4pjet_160_200ptv_SR",
                   "1 tag, 4+ jets, 160, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("2tag0pjet_200ptv_SR",
                   "2 tag, 0+ jets, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("2tag0pjet_160_200ptv_SR",
                   "2 tag, 0+ jets, 160, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("2tag0pjet_120_160ptv_SR",
                   "2 tag, 0+ jets, 120, p_{T}^{V} #geq 160 GeV, SR", "");
  config.addRegion("2tag2jet_200ptv_SR",
                   "2 tag, 2 jets, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("2tag2jet_120_160ptv_SR",
                   "2 tag, 2 jets, 120, p_{T}^{V} #geq 160 GeV, SR", "");
  config.addRegion("2tag2jet_120_200ptv_SR",
                   "2 tag, 2 jets, 120, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("2tag2jet_160_200ptv_SR",
                   "2 tag, 2 jets, 160, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("2tag3jet_200ptv_SR",
                   "2 tag, 3 jets, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("2tag3jet_160_200ptv_SR",
                   "2 tag, 3 jets, 160, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("2tag4pjet_200ptv_SR",
                   "2 tag, 4+ jets, p_{T}^{V} #geq 200 GeV, SR", "");
  config.addRegion("2tag4pjet_160_200ptv_SR",
                   "2 tag, 4+ jets, 160, p_{T}^{V} #geq 200 GeV, SR", "");

  // mBB Control Regions
  config.addRegion("0tag0pjet_200ptv_mBBcr",
                   "0+ tag, 0+ jets, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("0tag0pjet_120_160ptv_mBBcr",
                   "0+ tag, 0+ jets, 120, p_{T}^{V} #geq 160 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("0tag0pjet_160_200ptv_mBBcr",
                   "0+ tag, 0+ jets, 160, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("0tag4pjet_200ptv_mBBcr",
                   "0 tag, 4+ jets, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("0tag4pjet_120_160ptv_mBBcr",
                   "0 tag, 4+ jets, 120, p_{T}^{V} #geq 160 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("1tag0pjet_200ptv_mBBcr",
                   "1 tag, 0+ jets, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("1tag0pjet_120_160ptv_mBBcr",
                   "1 tag, 0+ jets, 120, p_{T}^{V} #geq 160 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("1tag0pjet_160_200ptv_mBBcr",
                   "1 tag, 0+ jets, 160, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("1tag3jet_120_160ptv_mBBcr",
                   "1 tag, 3 jets, 120, p_{T}^{V} #geq 160 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("1tag3jet_160_200ptv_mBBcr",
                   "1 tag, 3 jets, 160, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("1tag4pjet_200ptv_mBBcr",
                   "1 tag, 4+ jets, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("1tag4pjet_160_200ptv_mBBcr",
                   "1 tag, 4+ jets, 160, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("2tag0pjet_200ptv_mBBcr",
                   "2 tag, 0+ jets, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("2tag0pjet_120_160ptv_mBBcr",
                   "2 tag, 0+ jets, 120, p_{T}^{V} #geq 160 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("2tag0pjet_160_200ptv_mBBcr",
                   "2 tag, 0+ jets, 160, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("2tag2jet_200ptv_mBBcr",
                   "2 tag, 2 jets, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("2tag2jet_120_160ptv_mBBcr",
                   "2 tag, 2 jets, 120, p_{T}^{V} #geq 160 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("2tag2jet_120_200ptv_mBBcr",
                   "2 tag, 2 jets, 120, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("2tag2jet_160_200ptv_mBBcr",
                   "2 tag, 2 jets, 160, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("2tag3jet_200ptv_mBBcr",
                   "2 tag, 3 jets, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("2tag3jet_160_200ptv_mBBcr",
                   "2 tag, 3 jets, 160, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("2tag4pjet_200ptv_mBBcr",
                   "2 tag, 4+ jets, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");
  config.addRegion("2tag4pjet_160_200ptv_mBBcr",
                   "2 tag, 4+ jets, 160, p_{T}^{V} #geq 200 GeV, m_{b#bar{b}} CR", "");

  config.clearSystematics();

  // or with a reduced set of systematics
  // Note: first clear the systematics as shown above
  // config.addSystematic("SysMETScaleSoftTerms", // name
  //  		       false);         // is one sided?
  // config.addSystematic("SysMETResoSoftTerms", false);
  // config.addSystematic("SysJetEResol", true);
  // config.addSystematic("SysBJetReso", true);
  // config.addSystematic("SysMJShape", false);
  // config.addSystematic("SysMETScaleSoftTerms", false);
  // config.addSystematic("SysWDPhi", false);
  // config.addSystematic("SysWPtV", false);
  // config.addSystematic("SysWbbMbbGS", false);
  // config.addSystematic("SysWMbb", false);
  // config.addSystematic("SysTopPt", false);
  // config.addSystematic("SysTtbarPtWCont", false);
  // config.addSystematic("SysTtbarMBBCont", false);
  // config.addSystematic("SysTtbarPtB1Cont", false);
  // config.addSystematic("SysTtbarMetCont", false);
  // config.addSystematic("SysWtChanAcerMC", false);
  // config.addSystematic("SysWtChanPythiaHerwig", false);
  // config.addSystematic("SysTChanPtB2", false);
  // config.addSystematic("SysVVJetScalePtST1", false);
  // config.addSystematic("SysVVJetScalePtST2", false);
  // config.addSystematic("SysVVMbb", false);
  config.addSystematic("SysMuonScale", false);

  ////////////////////////
  /// Detailed systematics

  // One can ask for detailed systematic plots for specific variables in specific region
  // This will make one plot per nuisance parameter
  // config.addDetailedSystematicsPlots("mva",               // name of the variable: as declared above
  // 				     "trafo6",            // name of the transformation: relevant for BDT outputs
  // 				     "2tag2jet_vpt0_120", // name of the region: as declared above
  // 				     "Systematics: ");    // slide title: used to decide which plots to appear on the same slide
  // config.addDetailedSystematicsPlots("mva", "trafo6", "2tag2jet_vpt120", "Systematics: ");
  // config.addDetailedSystematicsPlots("mBB", "", "2tag2jet_vpt0_120", "Systematics: ");
  // config.addDetailedSystematicsPlots("mBB", "", "2tag2jet_vpt120", "Systematics: ");
  // config.addDetailedSystematicsPlots("pTB1", "", "2tag2jet_vpt0_120", "Systematics: ");
  // config.addDetailedSystematicsPlots("pTB1", "", "2tag2jet_vpt120", "Systematics: ");
  // config.addDetailedSystematicsPlots("pTB2", "", "2tag2jet_vpt0_120", "Systematics: ");
  // config.addDetailedSystematicsPlots("pTB2", "", "2tag2jet_vpt120", "Systematics: ");
  // config.addDetailedSystematicsPlots("MET", "", "2tag2jet_vpt0_120", "Systematics: ");
  // config.addDetailedSystematicsPlots("MET", "", "2tag2jet_vpt120", "Systematics: ");

  // config.addDetailedSystematicsPlots("mvaWlvH125", "trafo6", "2lltag2jet_vpt120", "Systematics (high pTV): ");
  // config.addDetailedSystematicsPlots("mvaWlvH125", "trafo6", "2mmtag2jet_vpt120", "Systematics (high pTV): ");
  // config.addDetailedSystematicsPlots("mvaWlvH125", "trafo6", "2tttag2jet_vpt120", "Systematics (high pTV): ");

  // config.addDetailedSystematicsPlots("mvaWlvH125", "trafo6", "2lltag2jet_vpt0_120", "Systematics (low pTV): ");
  // config.addDetailedSystematicsPlots("mvaWlvH125", "trafo6", "2mmtag2jet_vpt0_120", "Systematics (low pTV): ");
  // config.addDetailedSystematicsPlots("mvaWlvH125", "trafo6", "2tttag2jet_vpt0_120", "Systematics (low pTV): ");

  // config.addDetailedSystematicsPlots("mBB", "", "2lltag2jet_vpt120", "Systematics (high pTV): ");
  // config.addDetailedSystematicsPlots("mBB", "", "2mmtag2jet_vpt120", "Systematics (high pTV): ");
  // config.addDetailedSystematicsPlots("mBB", "", "2tttag2jet_vpt120", "Systematics (high pTV): ");

  // config.addDetailedSystematicsPlots("mBB", "", "2lltag2jet_vpt0_120", "Systematics (low pTV): ");
  // config.addDetailedSystematicsPlots("mBB", "", "2mmtag2jet_vpt0_120", "Systematics (low pTV): ");
  // config.addDetailedSystematicsPlots("mBB", "", "2tttag2jet_vpt0_120", "Systematics (low pTV): ");

  // config.addDetailedSystematicsPlots("mva", "trafoF", "1tag2jet_vpt0_120",   "Systematics 2J (low pTV): ");
  // config.addDetailedSystematicsPlots("mva", "trafoF", "2lltag2jet_vpt0_120", "Systematics 2J (low pTV): ");
  // config.addDetailedSystematicsPlots("mva", "trafoF", "2mmtag2jet_vpt0_120", "Systematics 2J (low pTV): ");
  // config.addDetailedSystematicsPlots("mva", "trafoF", "2tttag2jet_vpt0_120", "Systematics 2J (low pTV): ");

  // config.addDetailedSystematicsPlots("mva", "trafoF", "1tag2jet_vpt120",   "Systematics 2J (high pTV): ");
  // config.addDetailedSystematicsPlots("mva", "trafoF", "2lltag2jet_vpt120", "Systematics 2J (high pTV): ");
  // config.addDetailedSystematicsPlots("mva", "trafoF", "2mmtag2jet_vpt120", "Systematics 2J (high pTV): ");
  // config.addDetailedSystematicsPlots("mva", "trafoF", "2tttag2jet_vpt120", "Systematics 2J (high pTV): ");

  // config.addDetailedSystematicsPlots("mva", "trafoF", "1tag3jet_vpt0_120",   "Systematics 3J (low pTV): ");
  // config.addDetailedSystematicsPlots("mva", "trafoF", "2lltag3jet_vpt0_120", "Systematics 3J (low pTV): ");
  // config.addDetailedSystematicsPlots("mva", "trafoF", "2mmtag3jet_vpt0_120", "Systematics 3J (low pTV): ");
  // config.addDetailedSystematicsPlots("mva", "trafoF", "2tttag3jet_vpt0_120", "Systematics 3J (low pTV): ");

  // config.addDetailedSystematicsPlots("mva", "trafoF", "1tag3jet_vpt120",   "Systematics 3J (high pTV): ");
  // config.addDetailedSystematicsPlots("mva", "trafoF", "2lltag3jet_vpt120", "Systematics 3J (high pTV): ");
  // config.addDetailedSystematicsPlots("mva", "trafoF", "2mmtag3jet_vpt120", "Systematics 3J (high pTV): ");
  // config.addDetailedSystematicsPlots("mva", "trafoF", "2tttag3jet_vpt120", "Systematics 3J (high pTV): ");

  // for(int iCR=0; iCR<config.getNRegions(); iCR++) {
  //   Config::Region region = config.getRegion(iCR);
  //   for(int iVar=0; iVar<config.getNVariables(); iVar++) {
  //     Config::Variable var = config.getVariable(iVar);
  //     std::string title = "Systematics: "+var.name+" ";
  //     if(region.name.find("0tag") != std::string::npos ) {
  // 	title += "0 tag";
  //     }else if(region.name.find("1tag") != std::string::npos) {
  // 	title += "1 tag";
  //     }else{
  // 	title += "2 tag";
  // 	if(region.name.find("2jet") != std::string::npos) {
  // 	  title += " , 2 jets";
  // 	}else{
  // 	  title += " , 3 jets";
  // 	}
  // 	if(region.name.find("vpt120") != std::string::npos) {
  // 	  title += " (high pTV)";
  // 	}else {
  // 	  title += " (low pTV)";
  // 	}
  //     }
  //     config.addDetailedSystematicsPlots(var.name,
  // 					 "", //var.type == Config::BDTOutput ? "trafo6" : "",
  // 					 region.name,
  // 					 title);
  //   }
  // }


  // additional plots
  Config config_bTagClosure(config);
  Config config_bTagEff2D(config);
  Config config_bTagFatJet2D(config);

  // 1D
  config.setMake1DPlots(true);
  config.setMake2DPlots(false);
  config_bTagClosure.setMake1DPlots(true);
  config_bTagClosure.setMake2DPlots(false);

  // 2D
  config_bTagEff2D.setMake1DPlots(false);
  config_bTagEff2D.setMake2DPlots(true);
  config_bTagFatJet2D.setMake1DPlots(false);
  config_bTagFatJet2D.setMake2DPlots(true);



  ////////////
  // Variables

  // 2D b-tagging efficiency plots (non-fat jets)
  config_bTagEff2D.addVariable(Config::BDTInput, "pT", "p_{T} [GeV]", 0, 400, 1);
  config_bTagEff2D.addVariable(Config::BDTInput, "eff_C", "eff", 0, 1.1, 1);
  config_bTagEff2D.addVariable(Config::BDTInput, "eff_L", "eff", 0, 1.1, 1);
  config_bTagEff2D.addVariable(Config::BDTInput, "eff_B", "eff", 0, 1.1, 1);
  config_bTagEff2D.addVariable(Config::BDTInput, "eta", "#eta", -3, 3, 1);

  // 2D fat jet plots
  config_bTagFatJet2D.addVariable(Config::BDTInput, "nBTag", "number of b-tagged sub-jets (Track Jets)", 0, 5, 1);
  config_bTagFatJet2D.addVariable(Config::BDTInput, "nTrueBJets_FatJet", "number of true b-, sub-jets (Track Jets)", 0, 6, 1);
  config_bTagFatJet2D.addVariable(Config::BDTInput, "nTrueBJets", "number of true b-, sub-jets (Track Jets)", 0, 6, 1);
  config_bTagFatJet2D.addVariable(Config::BDTInput, "nTrackJets_FatJet", "number of sub-jets (Track Jets)", 0, 10, 1);
  config_bTagFatJet2D.addVariable(Config::BDTInput, "pT", "fat jet p_{T} [GeV]", 0, 1000, 1);
  config_bTagFatJet2D.addVariable(Config::BDTInput, "eta", "fat jet #eta", -5, 5, 1);
  config_bTagFatJet2D.addVariable(Config::BDTInput, "pT_FatJet", "fat jet p_{T} [GeV]", 0, 1000, 1);
  config_bTagFatJet2D.addVariable(Config::BDTInput, "eta_FatJet", "fat jet #eta", -5, 5, 1);

  // // 1D closure plots
  config_bTagClosure.addVariable(Config::BDTInput, "cut_mBB", "m(b#bar{b}) [GeV] (cut)", 0., 500., 1);
  config_bTagClosure.addVariable(Config::BDTInput, "eff_mBB", "m(b#bar{b}) [GeV] (eff)", 0., 500., 1);
  config_bTagClosure.addVariable(Config::BDTInput, "truthTag_mBB", "m(b#bar{b}) [GeV] (truth tag)", 0., 500., 1);
  
  // // 1D mBB plots
  config.addVariable(Config::BDTInput, "mBB", "m(b#bar{b}) [GeV]", 0., 500., 1);
  config.addVariable(Config::BDTInput, "dRBB", "#Delta R(b,#bar{b})", 0, 5, 1);
  config.addVariable(Config::BDTInput, "dPhiBB", "#Delta #phi(b,#bar{b})", 0, 3.15, 1);
  config.addVariable(Config::BDTInput, "dEtaBB", "#Delta#eta(b,#bar{b})", 0, 4.0, 1);
  config.addVariable(Config::BDTInput, "pTBB", "(p_{T})_{b#bar{b}} [GeV]", 0, 500, 1);


  config.addVariable(Config::BDTInput, "ZPV", "z vertex; z [cm]; Events",-200,200,1);
  
  config.addVariable(Config::BDTInput, "ptj", "P_{T}^{jets} [GeV]",0,100,1);
  config.addVariable(Config::BDTInput, "etaj", "#eta^{jets}",-3,3,1);
  
  config.addVariable(Config::BDTInput, "pTV", "P_{T}^{W} [GeV]",0,500,1);
  
  config.addVariable(Config::BDTInput, "pTL", "p_{T}^{lep} [GeV]",0,500,1);
  config.addVariable(Config::BDTInput, "mTW", "m_{T}^{W} [GeV]",0,500,1);
  config.addVariable(Config::BDTInput, "MET", "E_{T}^{miss} [GeV]",0,500,1);
  
  // config.addVariable(Config::BDTInput, "dRBB", "#Delta R(b,b)", 0, 5, 0);
  // config.addVariable(Config::BDTInput, "dPhiBB", "#Delta#phi(bb)", 0, 3.15, 1);
  // config.addVariable(Config::BDTInput, "dEtaBB", "#Delta#eta(bb)", 0, 5.0, 1);
  // config.addVariable(Config::BDTInput, "mBB", "m(bb) [GeV]", 0., 500., 5);
  config.addVariable(Config::BDTInput, "pTB1", "p_{T}(b_{1}) [GeV]", 0., 50.0, 5);
  config.addVariable(Config::BDTInput, "pTB2", "p_{T}(b_{2}) [GeV]", 0., 50.0, 5);
  config.addVariable(Config::BDTInput, "pTJ3", "p_{T}(j_{3}) [GeV]", 0., 50.0, 5);
  config.addVariable(Config::BDTInput, "mBBJ", "m(b#bar{b}j) [GeV]", 0, 750, 5);
  config.addVariable(Config::BDTInput, "dPhiVBB", "#Delta#phi(V,b,#bar{b})", 2.5, 3.15, 1);
  
  config.addVariable(Config::BDTInput, "MV2c00_B", "MV2c00_B", -1.1, 1.1, 1);
  config.addVariable(Config::BDTInput, "MV2c00_C", "MV2c00_C", -1.1, 1.1, 1);
  config.addVariable(Config::BDTInput, "MV2c00_L", "MV2c00_L", -1.1, 1.1, 1);
  
  config.addVariable(Config::BDTInput, "NFwdJets", "number of forward jets", -0.5, 10.5, 1);
  config.addVariable(Config::BDTInput, "NSigJets", "number of signal jets", -0.5, 10.5, 1);
  
  config.addVariable(Config::BDTInput, "PtFwdJets", "forward jet p_{T}", 0.0, 100.0, 1);
  config.addVariable(Config::BDTInput, "PtSigJets", "signal jet p_{T}", 0.0, 100.0, 1);
  
  config.addVariable(Config::BDTInput, "EtaFwdJets", "forward jet #eta", -4, 4, 1);
  config.addVariable(Config::BDTInput, "EtaSigJets", "signal jet #eta", -4, 4, 1);




   /////////////////////////
   /// Computing sensitivity

   // One can output expected sensitivity numbers based on some specific plots and regions
   // The sensitivity metric used is one of two:
   //  - Normal approximation:
   //    S/sqrt(B + dB*dB)
   //  - Log likelihood ratio:
   //    sqrt(2*((S+B)*lob(1+S/B)-S))
   // It is computed for each bin and then added in quadrature
   // Several histograms from different regions can be combined (one variable per region)
   // NOTE: the concerned variables and regions and transformations
   //       need to be added for ploting above.

   // define the regions to combine
   //   config.addSensitivityPlot( "mva",             // name to uniquely identify a sensitivity calculation
   // 			     "mva",             // variable name used for this region
   // 			     "2tag2jet_vpt120", // name of the region
   // 			     "trafo6");         // name of the transformation in case of BDT
   //   config.addSensitivityPlot( "mva", "mva", "2tag3jet_vpt120", "trafo6");
   //   config.addSensitivityPlot( "mva", "mva", "2tag2jet_vpt0_120", "trafo6");
   //   config.addSensitivityPlot( "mva", "mva", "2tag3jet_vpt0_120", "trafo6");

   //   // you can combine the same plots in different ways
   //   config.addSensitivityPlot( "mva 2t2j", "mva", "2tag2jet_vpt120", "trafo6");
   //   config.addSensitivityPlot( "mva 2t2j", "mva", "2tag2jet_vpt0_120", "trafo6");
   //   config.addSensitivityPlot( "mva 2t3j", "mva", "2tag3jet_vpt120", "trafo6");
   //   config.addSensitivityPlot( "mva 2t3j", "mva", "2tag3jet_vpt0_120", "trafo6");

   //   config.addSensitivityPlot( "mva high ptv", "mva", "2tag2jet_vpt120", "trafo6");
   //   config.addSensitivityPlot( "mva high ptv", "mva", "2tag3jet_vpt120", "trafo6");
   //   config.addSensitivityPlot( "mva low ptv", "mva", "2tag2jet_vpt0_120", "trafo6");
   //   config.addSensitivityPlot( "mva low ptv", "mva", "2tag3jet_vpt0_120", "trafo6");

   //   config.addSensitivityPlot( "cuts", "mjj", "2lltag3jet_vpt200", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2lltag2jet_vpt200", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2mmtag3jet_vpt200", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2mmtag2jet_vpt200", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2tttag3jet_vpt200", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2tttag2jet_vpt200", "");

   //   config.addSensitivityPlot( "cuts", "mjj", "2lltag3jet_vpt160_200", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2lltag2jet_vpt160_200", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2mmtag3jet_vpt160_200", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2mmtag2jet_vpt160_200", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2tttag3jet_vpt160_200", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2tttag2jet_vpt160_200", "");

   //   config.addSensitivityPlot( "cuts", "mjj", "2lltag3jet_vpt120_160", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2lltag2jet_vpt120_160", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2mmtag3jet_vpt120_160", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2mmtag2jet_vpt120_160", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2tttag3jet_vpt120_160", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2tttag2jet_vpt120_160", "");

   //   config.addSensitivityPlot( "cuts", "mjj", "2lltag3jet_vpt90_120", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2lltag2jet_vpt90_120", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2mmtag3jet_vpt90_120", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2mmtag2jet_vpt90_120", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2tttag3jet_vpt90_120", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2tttag2jet_vpt90_120", "");

   //   config.addSensitivityPlot( "cuts", "mjj", "2lltag3jet_vpt0_90", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2lltag2jet_vpt0_90", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2mmtag3jet_vpt0_90", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2mmtag2jet_vpt0_90", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2tttag3jet_vpt0_90", "");
   //   config.addSensitivityPlot( "cuts", "mjj", "2tttag2jet_vpt0_90", "");



  ///////////////////////////
  /// Now ready to make plots

  // check for errors in configuration
  if(!config.isValid() || !config_bTagClosure.isValid() || !config_bTagEff2D.isValid() || !config_bTagFatJet2D.isValid()) {
    std::cout << "Error in configuration: ===> Aborting..." << std::endl;
    return;
  }

  // we have all what we need to know
  // ==> now make the plots
  PlotMaker plotter;
  plotter.makePlots(config);
  PlotMaker plotter_bTagClosure;
  plotter_bTagClosure.makePlots(config_bTagClosure);
  PlotMaker plotter_bTagEff2D;
  plotter_bTagEff2D.makePlots(config_bTagEff2D);
  PlotMaker plotter_bTagFatJet2D;
  plotter_bTagFatJet2D.makePlots(config_bTagFatJet2D);
}
