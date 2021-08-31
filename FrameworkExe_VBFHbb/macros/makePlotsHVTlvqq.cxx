#include "PlottingTool/Config.h"
#include "PlottingTool/PlotMaker.h"

#include <iostream>


void makePlotsHVTlvqq(std::string InputPath,std::string InputFile) {

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

  config.addBackgroundSample("qqZvvH125",  "VH 125", kGreen+3);
  config.addBackgroundSample("WlvH125",    "VH 125", kGreen+3);
  config.addBackgroundSample("qqZllH125",  "VH 125", kGreen+3);

  //config.addSignalSample("AZh300llbb",   "m_{A}=300 GeV",  kRed+1);
  //config.addSignalSample("AZh800llbb",   "m_{A}=800 GeV",  kRed+1);
  //config.addSignalSample("AZh1000llbb",  "m_{A}=1000 GeV", kRed+1);

  //config.addSignalSample("HVTlvbb500",   "m_{W'}=500 GeV",  kRed+1);
  config.addSignalSample("HVTlvbb1000",  "m_{W'}=1000 GeV", kRed+1);
  //config.addSignalSample("HVTlvbb1500",  "m_{W'}=1500 GeV", kRed+1);
  //config.addSignalSample("HVTlvbb2000",  "m_{W'}=2000 GeV", kRed+1);
  //config.addSignalSample("HVTlvbb2500",  "m_{W'}=2500 GeV", kRed+1);

  config.addBackgroundSample("ttbar",  "ttbar", kOrange);
  config.addBackgroundSample("stops",  "single top", kOrange - 1);
  config.addBackgroundSample("stopt",  "single top", kOrange - 1);
  config.addBackgroundSample("stopWt", "single top", kOrange - 1);

  config.addBackgroundSample("W",   "W+jets", kGreen - 6);
  config.addBackgroundSample("Wl",  "W+l",  kGreen - 9);
  config.addBackgroundSample("Wcl", "W+cl", kGreen - 6);
  config.addBackgroundSample("Wcc", "W+cc", kGreen + 1);
  config.addBackgroundSample("Wbl", "W+bl", kGreen + 2);
  config.addBackgroundSample("Wbc", "W+bc", kGreen + 3);
  config.addBackgroundSample("Wbb", "W+bb", kGreen + 4);

  config.addBackgroundSample("Z",   "Z+jets", kAzure - 8);
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
  config.setLegendThreshold(1e-6);

  // To draw the histograms in no-stack mode
  //  config.setNoStack();

  // set total background name
  // this is not mandatory but would save CPU and memory
  // Note: if any background scale factors are different than one, 
  //       this cannot be used and will be ignored
  // config.setTotalBackgroundName("bkg");



  ////////////
  // Variables

  config.addVariable(Config::BDTInput, "pTV", "P_{T}^{V} [GeV]",0, 1000, 1);
  config.addVariable(Config::BDTInput, "pTL", "p_{T}^{lep} [GeV]",0, 1000, 1);
  config.addVariable(Config::BDTInput, "mTW", "m_{T}^{W} [GeV]",0, 300, 2);
  config.addVariable(Config::BDTInput, "mW1", "m_{T}^{W} (p_{z}^{#nu}=smaller) [GeV]", 0, 1000, 2);
  config.addVariable(Config::BDTInput, "mW2", "m_{T}^{W} (p_{z}^{#nu}=larger) [GeV]",  0, 1000, 2);
  config.addVariable(Config::BDTInput, "pZNu1", "p_{z}^{#nu} (smaller, real solution) [GeV]", 0, 2000, 2);
  config.addVariable(Config::BDTInput, "pZNu2", "p_{z}^{#nu} (larger, real solution) [GeV]",  0, 2000, 2);
  config.addVariable(Config::BDTInput, "pZNu1i", "p_{z}^{#nu} (smaller, imaginary solution) [GeV]", 0, 2000, 2);
  config.addVariable(Config::BDTInput, "pZNu2i", "p_{z}^{#nu} (larger, imaginary solution) [GeV]",  0, 2000, 2);
  config.addVariable(Config::BDTInput, "MET", "E_{T}^{miss} [GeV]",0, 1000, 1);

  config.addVariable(Config::BDTInput, "mBB", "m(jet) [GeV]", 0., 500., 1);
  config.addVariable(Config::BDTInput, "mBBS", "m(jet) [GeV]", 0., 500., 1);
  config.addVariable(Config::BDTInput, "pTB1", "p_{T}(jet) [GeV]", 0., 1000., 1);
  config.addVariable(Config::BDTInput, "dPhiVBB", "#Delta#phi(V,jet)", 0, 3.15, 1);

  config.addVariable(Config::BDTInput, "mVH", "m(VH) (p_{z}^{#nu}=no m_{H}=no) [GeV]", 0, 2000, 2);
  config.addVariable(Config::BDTInput, "mVH1", "m(VH) (p_{z}^{#nu}=smaller m_{H}=no) [GeV]", 0, 2000, 2);
  config.addVariable(Config::BDTInput, "mVH2", "m(VH) (p_{z}^{#nu}=larger m_{H}=no) [GeV]", 0, 2000, 2);
  config.addVariable(Config::BDTInput, "mVHS", "m(VH) (p_{z}^{#nu}=no m_{H}=yes) [GeV]", 0, 2000, 2);
  config.addVariable(Config::BDTInput, "mVHS1", "m(VH) (p_{z}^{#nu}=smaller m_{H}=yes) [GeV]", 0, 2000, 2);
  config.addVariable(Config::BDTInput, "mVHS2", "m(VH) (p_{z}^{#nu}=larger m_{H}=yes) [GeV]", 0, 2000, 2);
  config.addVariable(Config::BDTInput, "Tau21", "Tau21(jet)", 0, 1.2, 2);
  config.addVariable(Config::BDTInput, "Tau32", "Tau32(jet)", 0, 1.2, 2);
  config.addVariable(Config::BDTInput, "pTVHnorm", "p_{T}^{VH} / (p_{T}^{VH} + p_{T}^{VH}) [GeV]", 0, 1, 2);

  // config.addRegion("2tag3jet_vpt120",                      // name: as it appears in histogram name
  //               "2 tags, 3 jets, p_{T}^{V} >= 120 GeV", // title: as it appears on the plots
  //               "2 tags");                              // slide title: used to decide which plots to appear on the same slide
  config.clearRegions();
  config.addRegion("1tag1fat0pjet_0_120ptv_SR","1 tag, 1 fat jet, p_{T}^{V} < 120 GeV, SR","");
  config.addRegion("2tag1fat0pjet_0_120ptv_SR","2 tag, 1 fat jet, p_{T}^{V} < 120 GeV, SR","");
  config.addRegion("1tag1fat0pjet_120ptv_SR","1 tag, 1 fat jet, p_{T}^{V} >= 120 GeV, SR","");
  config.addRegion("2tag1fat0pjet_120ptv_SR","2 tag, 1 fat jet, p_{T}^{V} >= 120 GeV, SR","");
  config.addRegion("1tag1fat0pjet_0_120ptv_mBBcr","1 tag, 1 fat jet, p_{T}^{V} < 120 GeV, mBBcr","");
  config.addRegion("2tag1fat0pjet_0_120ptv_mBBcr","2 tag, 1 fat jet, p_{T}^{V} < 120 GeV, mBBcr","");
  config.addRegion("1tag1fat0pjet_120ptv_mBBcr","1 tag, 1 fat jet, p_{T}^{V} >= 120 GeV, mBBcr","");
  config.addRegion("2tag1fat0pjet_120ptv_mBBcr","2 tag, 1 fat jet, p_{T}^{V} >= 120 GeV, mBBcr","");

  config.clearSystematics();
  
  //config.addSystematic("SysMuonScale", false);
   
  config.addSensitivityPlot( "mVH", "mVH", "2tag1fat0pjet_120ptv_SR", "");

  ///////////////////////////
  /// Now ready to make plots

  // check for errors in configuration
  if(!config.isValid()) {
    std::cout << "Error in configuration: ===> Aborting..." << std::endl;
    return;
  }

  // we have all what we need to know
  // ==> now make the plots
  PlotMaker plotter;
  plotter.makePlots(config);
}
