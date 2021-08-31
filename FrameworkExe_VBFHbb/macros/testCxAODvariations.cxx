
#include <iostream>

#include "TSystem.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TError.h"

/*
 * Script for testing and plotting systematic variations in a CxAOD.
 * Plotting is skipped for variations without effect. 
 *
 * Run with:
 * root -b FrameworkExe/macros/testCxAODvariations.cxx+'("submitDir/data-CxAOD/CxAOD.root")'
 */

using namespace std;

struct NominalBranch {
  TString name;
  TString container;
  TString variable;
  vector<TString> copies;
};

void printValue(double value) {
  if (value == 0) {
    cout << "     ZERO     ";
  } else {
    if (value > 0) cout << " ";
    cout << "  " << TString::Format("%.5e",value).Data();
  }
}

void testCxAODvariations(TString fileName = "submitDir/data-CxAOD/CxAOD.root") {

  // ============== config ==============

  bool drawCorrelation = true;

  // ============== read tree ==============

  TFile* file  = TFile::Open(fileName);
  if (!file) {
    Error("testCxAODvariations()", "Could not find file '%s'!", fileName.Data());
    return;
  }
  if (file->IsZombie()) {
    Error("testCxAODvariations()", "Could not open file '%s'!", fileName.Data());
    return;
  }

  TTree* tree = (TTree*) file->Get("CollectionTree");
  TObjArray* branches = tree->GetListOfBranches();

  vector<NominalBranch*> nominalBranches;
  TString pattern = "___NominalAuxDyn.";

  // ============== read nominal branches ==============

  for (int i = 0; i < branches->GetSize(); i++) {
    TBranch* branch = (TBranch*)branches->At(i);
    TString name = branch->GetName();
    if (name.Contains(pattern)) {
      int pos = name.Index(pattern);
      NominalBranch* nominalBranch = new NominalBranch();
      nominalBranch->name = name;
      nominalBranch->container = name(0, pos);
      nominalBranch->variable = name(pos+pattern.Length(), name.Length()-1);
      nominalBranches.push_back(nominalBranch);
      cout << "Added: " << nominalBranch->container.Data() << "  " << nominalBranch->variable.Data() << endl;
    }
  }

  // ============== find copies ==============

  for (int i = 0; i < branches->GetSize(); i++) {
    TBranch* branch = (TBranch*)branches->At(i);
    TString name = branch->GetName();
    if (name.Contains("___NominalAuxDyn.")) {
      continue;
    }
    for (unsigned int j = 0; j < nominalBranches.size(); j++) {
      NominalBranch* nominalBranch = nominalBranches.at(j);
      //cout << "Test: " << nominalBranch->container.Data() << "  " << nominalBranch->variable.Data() << "  " << name.Data() << endl;
      if (name.BeginsWith(nominalBranch->container + "___") &&
	  name.EndsWith("." + nominalBranch->variable))
	nominalBranch->copies.push_back(name);
    }
  }

  //============== investigate ==============

  // prepare drawing
  TCanvas* canvas = 0;
  if (drawCorrelation) {
    canvas = new TCanvas();
    gSystem -> Exec("mkdir -vp plotVariations");
  }

  // loop over nominal branches
  for (unsigned int j = 0; j < nominalBranches.size(); j++) {
    NominalBranch* nominalBranch = nominalBranches.at(j);
    TString name = nominalBranch->name;

    // get stats
    tree->Draw(name+">>hnom","","goff");
    TH1* hnom = (TH1*) gDirectory->Get("hnom");
    double entries = hnom->GetEntries();
    double meanNom = hnom->GetMean();
    double RMSNom = hnom->GetRMS();
    delete hnom;

    // print stats
    cout << endl;
    cout << name.Data();
    cout << "   entries = ";
    printValue(entries);
    cout << "   mean = ";
    printValue(meanNom);
    cout << "   RMS = ";
    printValue(RMSNom);
    cout << endl;

    // loop over variations
    for (unsigned int k = 0; k < nominalBranch->copies.size(); k++) {

      // print table header
      if (k == 0) {
	cout << "   ";
	cout << "mean(var-nom) ";
	cout << "RMS(var-nom)  ";
	cout << endl;
      }

      // get stats
      TString copy = nominalBranch->copies.at(k);

      tree->Draw(copy+"-"+name+">>hdiff","","goff");
      TH1* hdiff = (TH1*) gDirectory->Get("hdiff");
      double mean = hdiff->GetMean();
      double RMS = hdiff->GetRMS();

      // print stats
      printValue(mean);
      printValue(RMS);
      cout << "   " << copy.Data();
      cout << endl;

      // clean up
      delete hdiff;

      if (drawCorrelation && (mean != 0 || RMS != 0)) {

	// draw 2d histogram of (var-nom) vs. nom
        tree->Draw(copy+"-"+name+":"+name+">>hcorr","","colz");
        TH2* hcorr = (TH2*) gDirectory->Get("hcorr");
	hcorr -> SetTitle(copy);
	hcorr -> GetXaxis() -> SetTitle(nominalBranch->variable + " (nominal)");
	hcorr -> GetYaxis() -> SetTitle("variation - nominal");
	canvas -> Print("plotVariations/corr_"+copy+".png");

	// clean up
	delete hcorr;
      }

    }
  }
  cout << "done!" << endl;
  gSystem->Exit(0);
}


