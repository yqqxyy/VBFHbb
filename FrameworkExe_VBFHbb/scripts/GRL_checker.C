#include <fstream>

void GRL_checker(const char *dirname="../../../data13TeVCxAOD1206/2lep/data", const char* folder_ext="_CxAOD.root"){
  TSystemDirectory dir(dirname, dirname);
  TList *folders = dir.GetListOfFiles();
  string line;
  if(folders){
    ifstream file("../../CxAODMaker/data/data15_13TeV.periodAllYear_DetStatus-v63-pro18-01_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml");
    TObjArray *ty;
    if(file.is_open()){
      while (getline(file,line)){
	TString currentLine = line;
	if(currentLine.Contains("RunList")){
	  currentLine.Remove(0,25);
	  currentLine.Remove(currentLine.Length()-11);
	  ty = currentLine.Tokenize(",");
	}
      }
      file.close();
    }
    for (Int_t i = 0; i < ty->GetEntries(); i++){
      TSystemFile *folderIter;
      TIter next(folders);
      bool foundFile = false;
      while ((folderIter=(TSystemFile*)next())){
	TString folderName = folderIter->GetName();
	if(folderName.Contains(folder_ext) && folderIter->IsDirectory()){
	  if(folderName.Contains(((TObjString *)(ty->At(i)))->String())){
	    foundFile = true;
	  }
	}
      }
      if (!foundFile){
	cout << "Run " << ((TObjString *)(ty->At(i)))->String() << " missing from " << dirname << endl;
      }
    }
  }
}
