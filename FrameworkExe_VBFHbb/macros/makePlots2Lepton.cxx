#include "PlottingTool/Config.h"
#include "PlottingTool/PlotMaker.h"

#include <iostream>
#include <utility> 

void makePlots2Lepton(std::string InputPath,std::string InputFile) {

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
  std::string lumi_str("3.32");
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
  config.setAnalysis( Config::TwoLepton,  // Type = ZeroLepton, OneLepton or TwoLepton: Appears on the plots
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


  //config.addSignalSample("qqZvvH125",  "VH 125", kRed);
  //config.addSignalSample("WlvH125",    "VH 125", kRed);
  //config.addSignalSample("qqZllH125",  "VH 125", kRed);

  // config.addSignalSample("ggA300ZHllbb",  "m_{A}=300 GeV ", kRed+1);
  config.addSignalSample("ggA500ZHllbb",  "m_{A}=500 GeV ", kRed+1);
  // config.addSignalSample("ggA800ZHllbb",  "m_{A}=800 GeV ", kRed+1);
  // config.addSignalSample("ggA1000ZHllbb",  "m_{A}=1000 GeV ", kRed+1);
  // config.addSignalSample("ggA1500ZHllbb",  "m_{A}=1500 GeV ", kRed+1);
  // config.addSignalSample("ggA2000ZHllbb",  "m_{A}=2000 GeV ", kRed+1);

  //
  config.addBackgroundSample("ttbar",  "ttbar", kOrange);
  config.addBackgroundSample("stop_s",  "single top", kOrange - 1);
  config.addBackgroundSample("stop_t",  "single top", kOrange - 1);
  config.addBackgroundSample("stop_Wt", "single top", kOrange - 1);

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
  config.addBackgroundSample("ZZ", "diboson", kGray);
  config.addBackgroundSample("WZ", "diboson", kGray);
  config.addBackgroundSample("WW", "diboson", kGray);
  config.addBackgroundSample("qqZllH125",  "VH 125", kRed);

  //  std::string scaleFactorsTag = "SF_1L_nJ_0107";
  // config.readScaleFactors("scaleFactors/"+scaleFactorsTag+".txt");
  // config.setOutputPath(config.getOutputPath()+"/"+scaleFactorsTag);

  // To optimize plot readability , samples that contribute less than a fraction X are dropped from the legend
  //  config.setLegendThreshold(0.05);

  // To draw the histograms in no-stack mode
  //  config.setNoStack();
  config.setNoSignalStack();

  // set total background name
  // this is not mandatory but would save CPU and memory
  // Note: if any background scale factors are different than one, 
  //       this cannot be used and will be ignored
  // config.setTotalBackgroundName("bkg");
 
   //  ////////////
  //  // Variables

  config.addVariable(Config::BDTInput, "ZPV", "z vertex; z [cm]; Events",-200,200,1); 
  config.addVariable(Config::BDTInput, "NVtx", "N_{vtx}", 0, 60,4);
  config.addVariable(Config::BDTInput, "mu", "mu", 0, 60,4);

  config.addVariable(Config::BDTInput, "pTL1", "P_{T}(l_{1}) [GeV]",0,200,1);
  config.addVariable(Config::BDTInput, "pTL2", "P_{T}(l_{2}) [GeV]",0,200,1);
  config.addVariable(Config::BDTInput, "EtaL1", "#eta(l_{1})", -5, 5, 2);
  config.addVariable(Config::BDTInput, "EtaL2", "#eta(l_{2})", -5, 5, 2);
  config.addVariable(Config::BDTInput, "dPhiLL", "#Delta#phi(ll)", 0, 3.15, 4);
  config.addVariable(Config::BDTInput, "dEtaLL", "#Delta#eta(ll)", 0, 5.0, 4);
  config.addVariable(Config::BDTInput, "pTV", "P_{T}^{Z} [GeV]",0,500,2);
  config.addVariable(Config::BDTInput, "pTZee", "P_{T}^{Z}(ee) [GeV]",0,500,2);
  config.addVariable(Config::BDTInput, "pTZmumu", "P_{T}^{Z}(#mu#mu) [GeV]",0,500,2);
  config.addVariable(Config::BDTInput, "Mee", "m_{ee} [GeV]", 60, 120, 1); 
  config.addVariable(Config::BDTInput, "Mmumu", "m_{#mu#mu} [GeV]", 60, 120, 1); 
  config.addVariable(Config::BDTInput, "mLL", "m_{ll} [GeV]", 60, 120, 1); 
  config.addVariable(Config::BDTInput, "MET", "E_{T}^{miss} [GeV]",0,500,2); 
  config.addVariable(Config::BDTInput, "HT", "H_{T} [GeV]",0,500,1); 
  config.addVariable(Config::BDTInput, "METHT", "E_{T}^{miss}/#sqrt{ H_{T}} [#sqrt{GeV}]",0,500,1); 

  config.addVariable(Config::BDTInput, "dRBB", "#Delta R(b,b)", 0, 5, 2);
  config.addVariable(Config::BDTInput, "dPhiBB", "#Delta#phi(bb)", 0, 3.15, 4);
  config.addVariable(Config::BDTInput, "dEtaBB", "#Delta#eta(bb)", 0, 5.0, 2);
  config.addVariable(Config::BDTInput, "mBB", "m(bb) [GeV]", 0., 500., 5);
  config.addVariable(Config::BDTInput, "pTBB", "p_{T}^{bb} [GeV]", 0., 500., 5);
  config.addVariable(Config::BDTInput, "pTB1", "p_{T}(b_{1}) [GeV]", 0., 500, 5);
  config.addVariable(Config::BDTInput, "pTB2", "p_{T}(b_{2}) [GeV]", 0., 500, 5);
  config.addVariable(Config::BDTInput, "EtaB1", "#eta(b_{1})", -5, 5, 2);
  config.addVariable(Config::BDTInput, "EtaB2", "#eta(b_{2})", -5, 5, 2);
  config.addVariable(Config::BDTInput, "MV2c20B1", "MV2c20 (b_{1})", -1.1, 1.1, 1);
  config.addVariable(Config::BDTInput, "MV2c20B2", "MV2c20 (b_{2})", -1.1, 1.1, 1);
  config.addVariable(Config::BDTInput, "pTJ3", "p_{T}(j_{3}) [GeV]", 0., 500, 5);
  config.addVariable(Config::BDTInput, "EtaJ3", "#eta(j_{3})", -5, 5, 2);
  config.addVariable(Config::BDTInput, "mBBJ", "m(bbj) [GeV]", 0, 750, 5);
  config.addVariable(Config::BDTInput, "pTBBJ", "p_{T}^{bbj} [GeV]", 0., 500., 5);
  config.addVariable(Config::BDTInput, "dPhiVH", "#Delta#phi(V,H)", 2.5, 3.15, 1);
  config.addVariable(Config::BDTInput, "dEtaVH", "#Delta#eta(V,H)", 0, 5, 1);
  config.addVariable(Config::BDTInput, "dRVH", "#Delta R(V,H)", 0, 5, 2);
  config.addVariable(Config::BDTInput, "pTVH", "p_{T}^{VH} [GeV]", 0., 500., 5);
  config.addVariable(Config::BDTInput, "mVH", "m(VH) [GeV]", 0., 700., 5);

  config.addVariable(Config::BDTInput, "PtJets", "p_{T} (jets)", 0.0, 100.0, 2);
  config.addVariable(Config::BDTInput, "EtaJets", "#eta (jets)", -5, 5, 2);

  config.addVariable(Config::BDTInput, "NJets", " N_{jets}", -0.5, 10.5, 1);
  config.addVariable(Config::BDTInput, "NSigJets", " N_{jets} (central)", -0.5, 10.5, 1);
  config.addVariable(Config::BDTInput, "NFwdJets", " N_{jets} (fwd)", -0.5, 10.5, 1);
  config.addVariable(Config::BDTInput, "NbJets", " N_{b-tag}", -0.5, 10.5, 1);

  //  // config.addVariable(Config::BDTInput, "MV2c00_B", "MV2c00_B", -1.1, 1.1, 1);
  //  // config.addVariable(Config::BDTInput, "MV2c00_C", "MV2c00_C", -1.1, 1.1, 1);
  //  // config.addVariable(Config::BDTInput, "MV2c00_L", "MV2c00_L", -1.1, 1.1, 1);

  //  // config.addVariable(Config::BDTInput, "NFwdJets", "NFwdJets", -0.5, 10.5, 1);
  // // config.addVariable(Config::BDTInput, "NSigJets", "NSigJets", -0.5, 10.5, 1);

  // // config.addVariable(Config::BDTInput, "PtFwdJets", "PtFwdJets", 0.0, 100.0, 1);
  // // config.addVariable(Config::BDTInput, "PtSigJets", "PtSigJets", 0.0, 100.0, 1);

  // // config.addVariable(Config::BDTInput, "EtaFwdJets", "PtFwdJets", -5, 5, 1);
  // // config.addVariable(Config::BDTInput, "EtaSigJets", "PtSigJets", -5, 5, 1);

  //  ////////////
  //  // Regions

  // config.addRegion("2tag3jet_vpt120",                      // name: as it appears in histogram name
  //               "2 tags, 3 jets, p_{T}^{V} >= 120 GeV", // title: as it appears on the plots
  //               "2 tags");                              // slide title: used to decide which plots to appear on the same slide
  config.clearRegions();

  vector<string> regions; 
  regions.push_back("SR");

  vector<pair<string,string> >jetBins;
  jetBins.push_back(make_pair("2jet","2 jet"));
  jetBins.push_back(make_pair("3jet","3 jet"));
  jetBins.push_back(make_pair("4jet","4 jet"));
  jetBins.push_back(make_pair("5pjet","5+ jet"));

  vector<pair<string,string> > tagBins;
  tagBins.push_back(make_pair("0tag","0 tag"));
  tagBins.push_back(make_pair("1tag","1 tag"));
  tagBins.push_back(make_pair("2tag","2 tag"));
  tagBins.push_back(make_pair("3ptag","3+ tag"));

  vector<string> pTVBins;
  pTVBins.push_back("_0_150ptv_");
  pTVBins.push_back("_150_200ptv_");
  pTVBins.push_back("_200_500ptv_");
  pTVBins.push_back("_500ptv_");

  //merge in ptV
  string pTVBinName = "_0ptv_";
  string pTVBinTitle = ", pTV > 0 GeV";
  

  for (unsigned int reg = 0; reg < regions.size(); reg++){
    string region = regions.at(reg);
    //std::cout <<"region "<< region << std::endl;

    for (unsigned int ptv = 0; ptv < pTVBins.size(); ptv++){
      string pTVBin = pTVBins.at(ptv);
      //std::cout <<"pTVBin "<< pTVBin << std::endl; 

      //loop on tags
      for (unsigned int tag = 0; tag < tagBins.size(); tag++){
	string tagBin = tagBins.at(tag).first;
	string tagBinName = tagBins.at(tag).second;
	//std::cout <<"tagBin "<< tagBin << std::endl;

	//loop on jets
	for (unsigned int jet = 0; jet < jetBins.size(); jet++){
	  string jetBin = jetBins.at(jet).first;
	  string jetBinName = jetBins.at(jet).second;
	  //std::cout <<"jetBin "<< jetBin << std::endl;

	  string name = tagBin+jetBin+pTVBin+region;
	  
	  config.addRegion(name, tagBin+jetBin+pTVBinName+region, jetBinName+", "+tagBinName+pTVBinTitle,"");

	  //merge jets: 2+ jet
	  config.addRegion(name, tagBin+"2pjet"+pTVBinName+region, "2+ jet, "+tagBinName+pTVBinTitle,"");
	  //merge tags: 0+ tag
	  config.addRegion(name, "0ptag"+jetBin+pTVBinName+region, jetBinName+", 0+ tag"+pTVBinTitle,"");
	  //merge jets and tags_ 2+jet, 0+ tag
	  config.addRegion(name, "0ptag2pjet"+pTVBinName+region, "2+ jet, 0+ tag"+pTVBinTitle,"");
	}
      }
    }
  }

  /////////////////////
  /// Blinding the data
  
  // We can also set explicit blinding on specific variable and regions
  // config.addWindowBlinding("mBB",               // variable name to blind: should match addVariable() below 
  //  			   "2tag2jet_vpt0_120", // region in which to blind: should match names in the input file
  //  			   "",                  // BDT transformation as declared above: only relevant for BDT outputs
  //  			   100000,              // Lower limit
  //  			   149999);             // Upper limit

  //loop on ptv bins
  for (unsigned int ptv = 0; ptv < pTVBins.size(); ptv++){
    string pTVBin = pTVBins.at(ptv);
 
    //loop on jets
    for (unsigned int jet = 0; jet < jetBins.size(); jet++){
      string jetBin = jetBins.at(jet).first;

      config.addWindowBlinding("mBB", "2tag"+jetBin+pTVBin+"_SR", "", 95, 140);
      config.addWindowBlinding("mVH", "2tag"+jetBin+pTVBin+"_SR", "", 450, 550); 
    }
  }


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
  if(!config.isValid()) {
    std::cout << "Error in configuration: ===> Aborting..." << std::endl;
    return;
  }

  // we have all what we need to know
  // ==> now make the plots
  PlotMaker plotter;
  plotter.makePlots(config);
}
