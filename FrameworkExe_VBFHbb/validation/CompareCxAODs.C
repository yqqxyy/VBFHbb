#include <cmath>
#include <fstream>
#include <set>
#include <string>

using namespace std;
const float SAFE = 0.01; // Variables whose ratio deviate from 1 by less than this amount are considered unchanged (green)
const float WARN = 0.10; // Variables whose ratio deviate from 1 by less than this but more than SAFE are considered slightly different (yellow). More than this is considered different (red).

double PercentDifference(double varOne, double varTwo) {
  if (varOne == 0) {
    return varTwo == 0 ? 0 : 1;
  }
  return abs((varOne - varTwo) / min(varOne, varTwo));
}

// Inputs: ROOT files to be compared and the trees in each one containing the tuples.
// Tuples produced using the the stage 1 framework test configuration should have the default tree name "CollectionTree"
// fileOne: reference
// fileTwo: test
void CompareCxAODs(const TString& fileOne, const TString& fileTwo, const TString& plotdir) {
  gROOT->Reset();
  ofstream logfile(plotdir+"/summary.log");
  logfile << "Comparing " << fileOne << " and " << fileTwo << endl;

  // Change this value to perform comparison on different variations
  string variation = "Nominal";

  TFile file1(fileOne, "");
  TFile file2(fileTwo, "");

  TTree* tree1 = (TTree*) file1.Get("CollectionTree");
  TTree* tree2 = (TTree*) file2.Get("CollectionTree");

  logfile << "Number of events for REFERENCE file: " << tree1->GetEntries() << endl;
  logfile << "Number of events for TEST file: " << tree2->GetEntries() << endl;


  // First, dump branches that are missing in one case
  logfile << endl << endl;
  logfile << "Branches present in TEST but not present in REFERENCE" << endl;
  for (int i = 0; i < tree2->GetNbranches(); i++) {
    TBranch* branch = (TBranch*) tree2->GetListOfBranches()->At(i);
    string branchname = branch->GetName();
    if (tree1->GetBranch(branchname.c_str()) == NULL) {
      logfile << branch->GetName() << endl;
      continue;
    }
  }
  logfile << endl << endl;
  logfile << "Branches present in REFERENCE but not present in TEST" << endl;
  for (int i = 0; i < tree1->GetNbranches(); i++) {
    TBranch* branch = (TBranch*) tree1->GetListOfBranches()->At(i);
    string branchname = branch->GetName();
    if (tree2->GetBranch(branchname.c_str()) == NULL) {
      logfile << branch->GetName() << endl;
      continue;
    }
  }
  logfile << endl << endl;
  logfile << "Swag swag swag" << endl;
  logfile << endl << endl;

  TCanvas * c = new TCanvas("c", "c");

  set<string> known_types;
  known_types.insert("Int_t");
  known_types.insert("Float_t");
  known_types.insert("Double_t");
  known_types.insert("Bool_t");
  known_types.insert("vector<int>");
  known_types.insert("vector<float>");
  known_types.insert("vector<double>");
  known_types.insert("vector<bool>");

  for (int i = 0; i < tree1->GetNbranches(); i++) {

    TBranch* branch = (TBranch*) tree1->GetListOfBranches()->At(i);
    TString branchname = branch->GetName();
    if (tree2->GetBranch(branchname) == NULL) {
      continue;
    }

    const char* dataType = ((TLeaf*) branch->GetListOfLeaves()->At(0))->GetTypeName();
    if(known_types.find(dataType) == known_types.end()) {
      logfile << "Variable " << branchname << " is of unknown type " << dataType << endl;
      continue;
    }

    tree1->Draw(branchname+">>hplot(40)");
    TH1F *hplot = (TH1F*)gDirectory->Get("hplot");
    TH1F* hplotref = (TH1F*)(hplot->Clone("hplotref"));
    hplot->Reset();
    // hopefully this histo has same binning / min / max values
    tree2->Draw(branchname+">>hplot");

    hplot = (TH1F*)gDirectory->Get("hplot");
    TH1F* hplotratio = (TH1F*)(hplot->Clone("hplotratio"));
    hplotratio->Divide(hplotref);

    bool hasSignifDev = false;
    for(int i=0; i<hplotref->GetNbinsX()+2; i++) {
      float ratio = hplotratio->GetBinContent(i);
      // ratio not 0 (happens when no stat in a bin)
      // + ratio significantly different from 1
      // => the plot should be shown
      if(fabs(ratio)>1.e-6 && fabs(ratio-1)>SAFE) {
        hasSignifDev = true;
        break;
      }
    }

    // no significant deviation: do not store for not wasting resources
    if(! hasSignifDev) {
      delete hplot;
      delete hplotratio;
      delete hplotref;
      continue;
    }


    c->Clear();
    c->Divide(1, 2, 0.01, 0.05);
    c->cd(1);
    hplotref->SetLineColor(kBlack);
    hplotref->SetName("Ref");
    hplotref->SetTitle("Ref");
    hplotref->Draw("hist");
    hplot->SetLineColor(kRed);
    hplot->SetMarkerColor(kRed);
    hplot->Draw("samee");
    hplot->SetName("Test");
    hplot->SetTitle("Test");
    gPad->BuildLegend(0.5, 0.5, 0.8, 0.8);
    c->cd(2);
    hplotratio->SetLineColor(kRed);
    hplotratio->SetMarkerColor(kRed);
    hplotratio->Draw("hist");

    c->SaveAs(plotdir+"/"+branchname.ReplaceAll(".","_")+".png");
  }


  logfile.close();
}
