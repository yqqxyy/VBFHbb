#include <TSystem.h>
#include <TROOT.h>
void runCxAODPlots();
using namespace std;

// macro for plotting histograms made by CxAODReader
// Run using e.g.
// root -b -q FrameworkExe/macros/runCxAODPlots.cxx
//
// Prerequisites:
//  The PlottingTool
//  svn co svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/HiggsPhys/Run2/Hbb/InputsProcessingTools/PlottingTool/trunk PlottingTool
//
//

void runCxAODPlots()
{

  // use PlotMaker to plot
  TString libdir="PlottingTool/";
  TString includePath = gSystem->GetIncludePath();
  bool do_hadd = true;
  // bool do_hadd = false;
  TString OutputPath = "./";
  TString OutputFileName = "inputsFile.root";
  TString PathInputFile = "submitDir/";
  // TString PathInputFile = "result_3/";
  TString pattern = "hist-*";
  //cout << "includePath: " << includePath << endl;
  gSystem->SetIncludePath(includePath+" -I"+libdir+"transform");
  //cout << "includePath update: " << gSystem->GetIncludePath() << endl;

  gROOT->ProcessLine(".L "+libdir+"transform/HistoTransform.C+");

  gROOT->ProcessLine(".L "+libdir+"Utils.cxx+");
  gROOT->ProcessLine(".L "+libdir+"SystTool.cxx+");
  gROOT->ProcessLine(".L "+libdir+"Config.cxx+");
  gROOT->ProcessLine(".L "+libdir+"LatexMaker.cxx+");
  gROOT->ProcessLine(".L "+libdir+"PlotMaker.cxx+");

  if(do_hadd){
    gSystem->Exec("hadd "+ OutputPath+OutputFileName +" "+PathInputFile+"/"+pattern);
  }

  // Remove .root to the file name to be in the Config.h convention
  OutputFileName.ReplaceAll(".root","");
  // TString input(".x FrameworkExe/macros/makePlots2Lepton.cxx+(\""+OutputPath+"\",\""+ OutputFileName+"\")");
  TString input(".x FrameworkExe/macros/makePlots1Lepton.cxx+(\""+OutputPath+"\",\""+ OutputFileName+"\")");
  //TString input(".x FrameworkExe/macros/makePlots0Lepton.cxx+(\""+OutputPath+"\",\""+ OutputFileName+"\")");
  //TString input(".x FrameworkExe/macros/makePlotsHVTlvqq.cxx+(\""+OutputPath+"\",\""+ OutputFileName+"\")");
  gROOT->ProcessLine(input);

}
