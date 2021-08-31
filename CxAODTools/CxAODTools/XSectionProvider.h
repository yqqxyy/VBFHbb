#ifndef CxAODTools_XSectionProvider_H
#define CxAODTools_XSectionProvider_H

#include <string>
#include <map>
#include "CxAODTools/ConfigStore.h"

class XSectionProvider {

private:

  struct XSection {
    std::string name;
    float xSection;
    float kFactor;
    float filterEff;
    std::string samplename;
  };
  
  struct SampleNameAndETag {
    std::string etag;
    std::string sampleName;
  };

  std::map<int, XSection> m_xSections;
  std::map<int, SampleNameAndETag> m_sampleNamesAndTags;
  
  ConfigStore *m_config;
  bool m_usePMGxsecDB;
  std::string m_timestamp;
  std::string m_analysis;

  void readSampleInfo();
  void ReadXSectionPMG();
  void ReadXSectionLocal(std::string fileName);

public:
 
  XSectionProvider(std::string fileName="");
  XSectionProvider(ConfigStore* config, std::string fileName="");
  ~XSectionProvider() = default;

  bool hasMCchannel (int mc_channel_number);
  float getXSection(int mc_channel_number);
  std::string getSampleName(int mc_channel_number);
  std::string getSampleDetailName(int mc_channel_number);
};


#endif
