#include<TCanvas.h>
#include<TLatex.h>
#include<TH1F.h>
#include<TLegend.h>
#include<TFile.h>
#include<stdio.h>
#include<stdlib.h>
#include<TStyle.h>
#include<TROOT.h>
#include<TPad.h>
#include <TStyle.h>
#include "TGaxis.h"
#include "TEfficiency.h"

#include <iostream>
#include <iomanip>

using namespace std;


void plot_Vjets(){

  // -------------------------------------------------------------------------
  // This simple macros compute the VpT spectrum obtained when we run CxAODReader step.
  // It is expecting to have the sample root files in the directory defined in "path" string variabe below.
  // These names could be re-defined (if changed) in the vector of string below.
  // Execute with : root -b -q plot_Vjets.C
  // -------------------------------------------------------------------------

  // params
  int my_rebin = 1;
  TString path = "../"; // put what you want here

  gStyle->SetOptStat(1111111);
  gStyle->SetOptTitle(0); 
  gStyle->SetOptFit(0);
  //gStyle->SetLineWidth(1.5);
  gStyle->SetHistLineWidth(3);
  
  ///////////////
  // Get histos
  
  std::vector<TString> sample;
  sample.push_back( TString("ZeeL") );
  sample.push_back( TString("ZeeC") );
  sample.push_back( TString("ZeeB") );
  sample.push_back( TString("ZmumuL") );
  sample.push_back( TString("ZmumuC") );
  sample.push_back( TString("ZmumuB") );
  sample.push_back( TString("ZtautauL") );
  sample.push_back( TString("ZtautauC") );
  sample.push_back( TString("ZtautauB") );
  sample.push_back( TString("ZnunuL") );
  sample.push_back( TString("ZnunuC") );
  sample.push_back( TString("ZnunuB") );
  sample.push_back( TString("WenuL") );
  sample.push_back( TString("WenuC") );
  sample.push_back( TString("WenuB") );
  sample.push_back( TString("WmunuL") );
  sample.push_back( TString("WmunuC") );
  sample.push_back( TString("WmunuB") );
  sample.push_back( TString("WtaunuL") );
  sample.push_back( TString("WtaunuC") );
  sample.push_back( TString("WtaunuB") );


  for (int i_sample = 0; i_sample < sample.size(); i_sample++) {
    TFile *my_file = new TFile(path+"/hist-"+sample[i_sample]+".root");
    TH1F *h_Vjets = (TH1F*) my_file->Get("VptTruth"); h_Vjets->Rebin(my_rebin);
    
    ///////////////
    // plot
    TCanvas* c_Vjets = new TCanvas("c_Vjets","c_Vjets",1060,720);
    c_Vjets->cd(); c_Vjets->SetLogy(); c_Vjets->cd()->SetGrid();
    h_Vjets->SetLineWidth(2);
    h_Vjets->SetLineColor(kRed);
    h_Vjets->GetXaxis()->SetTitle("p_{T}^{V} truth");
    h_Vjets->Draw();

    TLatex *tex = new TLatex(0.5,0.7,sample[i_sample]);
    tex->SetNDC();
    tex->SetTextFont(72);
    tex->SetLineWidth(2);
    tex->Draw();

    ///////////////
    // save
    c_Vjets->SaveAs("./"+sample[i_sample]+".png");

    c_Vjets->Close();
  }

}
