#include <cmath>

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
void CompareTuples(TString fileOne, TString fileTwo, TString treeOne = "CollectionTree", TString treeTwo = "CollectionTree") {
  gROOT->Reset();

  // Change this value to perform comparison on different variations
  string variation = "Nominal";
  
  TFile file1(fileOne, "");
  TFile file2(fileTwo, "");

  TTree* tree1 = (TTree*) file1.Get(treeOne);
  TTree* tree2 = (TTree*) file2.Get(treeTwo);
  
  if (tree1->GetEntries() != tree2->GetEntries()) {
    cout << "Number of events: \033[1;31mDifferent\033[0m\n";
    cout << "Aborting validation\n";
    return;
  }
  cout << "Number of events: \033[1;32mSame\033[0m\n";

  // Getting values from a TBranch in ROOT requires having an object of the appropriate type to copy the values into
  // Simple value types that we want to compare should be listed here for that purpose. Add more as necessary.
  int intVar[2] = {0,0};
  float floatVar[2] = {0.,0.};
  double doubleVar[2] = {0., 0.};
  bool boolVar[2] = {false, false};
  vector<int>* vintVar[2] = {NULL, NULL};
  vector<float>* vfloatVar[2] = {NULL, NULL};
  vector<double>* vdoubleVar[2] = {NULL, NULL};
  vector<bool>* vboolVar[2] = {NULL, NULL};

  enum DataType {INT, FLOAT, DOUBLE, BOOL, INTVECTOR, FLOATVECTOR, DOUBLEVECTOR, BOOLVECTOR, UNKNOWN};
  enum Difference {SAME, WARNING, DIFF};
  DataType data = UNKNOWN;

  int counts[3] = {0,0,0}; // counts of SAME, WARNING, and DIFF results

  for (int i = 0; i < tree1->GetNbranches(); i++) {
    // Reset variables
    intVar[0] = 0;
    intVar[1] = 0;
    floatVar[0] = 0;
    floatVar[1] = 0;
    boolVar[0] = false;
    boolVar[1] = false;
    vintVar[0] = NULL;
    vintVar[1] = NULL;
    vfloatVar[0] = NULL;
    vfloatVar[1] = NULL;
    vboolVar[0] = NULL;
    vboolVar[1] = NULL;
    data = UNKNOWN;

    TBranch* branch = (TBranch*) tree1->GetListOfBranches()->At(i);
    string branchname = branch->GetName();
    if (branchname.find(variation) == string::npos) {
      continue;
    }
    if (tree2->GetBranch(branchname.c_str()) == NULL) {
      cout << branch->GetName() << ": \033[1;31mFailed\033[0m (branch not present in file 2)\n";
      counts[2] += 1;
      continue;
    }
    const char* dataType = ((TLeaf*) branch->GetListOfLeaves()->At(0))->GetTypeName();
    if (strcmp(dataType, "Int_t") == 0) {
      tree1->SetBranchAddress(branch->GetName(), &intVar[0]);
      tree2->SetBranchAddress(branch->GetName(), &intVar[1]);
      data = INT;
    } else if (strcmp(dataType, "Float_t") == 0) {
      tree1->SetBranchAddress(branch->GetName(), &floatVar[0]);
      tree2->SetBranchAddress(branch->GetName(), &floatVar[1]);
      data = FLOAT;
    } else if (strcmp(dataType, "Double_t") == 0) {
      tree1->SetBranchAddress(branch->GetName(), &doubleVar[0]);
      tree2->SetBranchAddress(branch->GetName(), &doubleVar[1]);
      data = DOUBLE;
    } else if (strcmp(dataType, "Bool_t") == 0) {
      tree1->SetBranchAddress(branch->GetName(), &boolVar[0]);
      tree2->SetBranchAddress(branch->GetName(), &boolVar[1]);
      data = BOOL;
    } else if (strcmp(dataType, "vector<int>") == 0) {
      tree1->SetBranchAddress(branch->GetName(), &vintVar[0]);
      tree2->SetBranchAddress(branch->GetName(), &vintVar[1]);
      data = INTVECTOR;
    } else if (strcmp(dataType, "vector<float>") == 0) {
      tree1->SetBranchAddress(branch->GetName(), &vfloatVar[0]);
      tree2->SetBranchAddress(branch->GetName(), &vfloatVar[1]);
      data = FLOATVECTOR;
    } else if (strcmp(dataType, "vector<double>") == 0) {
      tree1->SetBranchAddress(branch->GetName(), &vdoubleVar[0]);
      tree2->SetBranchAddress(branch->GetName(), &vdoubleVar[1]);
      data = DOUBLEVECTOR;
    } else if (strcmp(dataType, "vector<bool>") == 0) {
      tree1->SetBranchAddress(branch->GetName(), &vboolVar[0]);
      tree2->SetBranchAddress(branch->GetName(), &vboolVar[1]);
      data = BOOLVECTOR;
    }

    if (data == UNKNOWN) {
      cout << branch->GetName() << " is of unknown or non-basic data type " << dataType << ": Ignoring\n";
      continue;
    }

    bool pass = true;
    Difference result = SAME;
    for (int j = 0; j < tree1->GetEntries(); j++) {
      tree1->GetEntry(j);
      tree2->GetEntry(j);
      double diff = 0;
      switch(data) {
      case INT:
	diff = PercentDifference((double) intVar[0], (double) intVar[1]);
	break;
      case FLOAT:
	diff = PercentDifference(floatVar[0], floatVar[1]);
	break;
      case DOUBLE:
	diff = PercentDifference(doubleVar[0], doubleVar[1]);
	break;
      case BOOL:
	diff = boolVar[0] == boolVar[1] ? 0 : 1;
	break;
      case INTVECTOR:
	if (vintVar[0]->size() != vintVar[1]->size()) {
	  diff = 1;
	} else {
	  for (int k = 0; k < vintVar[0]->size(); k++) {
	    diff = max(diff, PercentDifference((double) vintVar[0]->at(k), (double) vintVar[1]->at(k)));
	  }
	}
	break;
      case FLOATVECTOR:
	if (vfloatVar[0]->size() != vfloatVar[1]->size()) {
	  diff = 1;
	} else {
	  for (int k = 0; k < vfloatVar[0]->size(); k++) {
	    diff = max(diff, PercentDifference(vfloatVar[0]->at(k), vfloatVar[1]->at(k)));
	  }
	}
	break;
      case DOUBLEVECTOR:
	if (vdoubleVar[0]->size() != vdoubleVar[1]->size()) {
	  diff = 1;
	} else {
	  for (int k = 0; k < vdoubleVar[0]->size(); k++) {
	    diff = max(diff, PercentDifference(vdoubleVar[0]->at(k), vdoubleVar[1]->at(k)));
	  }
	}
	break;
      case BOOLVECTOR:
	if (vboolVar[0]->size() != vboolVar[1]->size()) {
	  diff = 1;
	} else {
	  for (int k = 0; k < vboolVar[0]->size(); k++) {
	    if (vboolVar[0]->at(k) != vboolVar[1]->at(k)) {
	      diff = 1;
	      break;
	    }
	  }
	}
	break;
      case UNKNOWN:
	break;
      }
      if (diff < WARN && diff >= SAFE) {
	result = WARNING;
      } else if (diff >= WARN) {
	result = DIFF;
	break;
      }
    }
    if (result == DIFF) {
      cout << branch->GetName() << ": \033[1;31mDifferent\033[0m\n";
      counts[2] += 1;
    } else if (result == WARNING) {
      cout << branch->GetName() << ": \033[1;33mSlightly different\033[0m\n";
      counts[1] += 1;
    } else {
      cout << branch->GetName() << ": \033[1;32mSame\033[0m\n";
      counts[0] += 1;
    }
  }

  for (int i = 0; i < tree2->GetNbranches(); i++) {
    TBranch* branch = (TBranch*) tree2->GetListOfBranches()->At(i);
    string branchname = branch->GetName();
    if (branchname.find(variation) == string::npos) {
      continue;
    }
    if (tree1->GetBranch(branchname.c_str()) == NULL) {
      counts[2] += 1;
      cout << branch->GetName() << ": \033[1;31mFailed\033[0m (branch not present in file 1)\n";
      continue;
    }
  }

  cout << "-------------------------\n";
  cout << "SUMMARY\n";
  cout << "\033[1;32mSame\033[0m: " << counts[0] << "\n";
  cout << "\033[1;33mSlightly different\033[0m: " << counts[1] << "\n";
  cout << "\033[1;31mDifferent\033[0m: " << counts[2] << "\n";
}
