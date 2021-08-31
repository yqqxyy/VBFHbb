#ifndef CxAODTools_Enums_H
#define CxAODTools_Enums_H

#include<string> 
#include<iostream> 

class Enums
{
  public: 
    enum class analysisLepChannel 
    {
      // Important: do not change values of those enums member
      // undefined = -1, so that ZeroLep = 0 and so on
      // hence integer values assigned correspond to the number of lepton(s)
      // code rely on the correct correspondance between enum and value
      undefined = -1,
      ZeroLep = 0,
      OneLep = 1,
      TwoLep = 2
    };

    static Enums::analysisLepChannel getAnalysisLepChannel(const std::string & analysisType)
    {
      Enums::analysisLepChannel analysisLepChannel = Enums::analysisLepChannel::undefined ; 

      if      (analysisType == "0lep") { analysisLepChannel = Enums::analysisLepChannel::ZeroLep ;}
      else if (analysisType == "1lep") { analysisLepChannel = Enums::analysisLepChannel::OneLep  ;}
      else if (analysisType == "2lep") { analysisLepChannel = Enums::analysisLepChannel::TwoLep  ;}

      return analysisLepChannel ; 
    }
}; 


#endif