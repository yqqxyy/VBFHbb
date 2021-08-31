#include <iostream>

#include "EventLoop/StatusCode.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "CxAODMaker_VBFHbb/EventInfoHandler_VBFHbb.h"
#include "CxAODTools/ConfigStore.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "xAODTrigger/JetRoIAuxContainer.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthEventContainer.h"

#include "CxAODTools_VBFHbb/CommonProperties_VBFHbb.h"
#include <TFile.h>

bool sortf(TLorentzVector i,TLorentzVector j) {return (i.Pt()>j.Pt());}

EventInfoHandler_VBFHbb::EventInfoHandler_VBFHbb(ConfigStore & config, xAOD::TEvent * event) :
  EventInfoHandler(config, event)
{

}

EL::StatusCode EventInfoHandler_VBFHbb::initialize()
{

  EL::StatusCode sc = EventInfoHandler::initialize();
  if ( sc != EL::StatusCode::SUCCESS ) {
    Error("EventInfoHandler_VBFHbb::initialize()", "Failed to call parent class' initialize().");
    return sc;
  }
  
  m_higgsMCtool_VBF = new TruthWeightTools::HiggsWeightTool("HiggsWeightTool_VBF");
  m_higgsMCtool_ggF = new TruthWeightTools::HiggsWeightTool("HiggsWeightTool_ggF");

  if (m_higgsMCtool_ggF->setProperty("ForceNNLOPS", true).isFailure())
    {
      Error("EventInfoHandler_VBFHbb::initialize()","Failed to set forcennlops");
      return EL::StatusCode::FAILURE;
    }

  if (m_higgsMCtool_VBF->setProperty("ForceVBF", true).isFailure())
    {
      Error("EventInfoHandler_VBFHbb::initialize()","Failed to set forcevbf");
      return EL::StatusCode::FAILURE;
    }

  if (false && m_isMC && m_higgsMCtool_ggF->initialize().isFailure() ) {
    Error("EventInfoHandler_VBFHbb::initialize()","Failed to initialize the Truth Weight Tool for ggF.");
    return EL::StatusCode::FAILURE;
  }

  if (false && m_isMC && m_higgsMCtool_VBF->initialize().isFailure() ) {
    Error("EventInfoHandler_VBFHbb::initialize()","Failed to initialize the Truth Weight Tool for VBF.");
    return EL::StatusCode::FAILURE;
  }
  
  // b-jet Trigger Emulation Tool
  m_triggerEmulation = new Trig::TrigBtagEmulationTool("Trig::TrigBtagEmulationTool/TrigBtagEmulationTool");
  if ( m_triggerEmulation->setProperty("UseTriggerNavigation",false).isFailure() )
    {
      Error("EventInfoHandler_VBFHbb::initialize()","Failed to set TrigBtagEmulationTool's Navigation to False.");
      return EL::StatusCode::FAILURE;
    }
  if ( m_triggerEmulation->setProperty("TrigDecisionToolName","TrigDecisionTool").isFailure() )
    {
      Error("EventInfoHandler_VBFHbb::initialize()","Failed to set TrigBtagEmulationTool's decision tooll name");
      return EL::StatusCode::FAILURE;
    }

  if ( m_triggerEmulation->initialize().isFailure() ) {
    Error("EventInfoHandler_VBFHbb::initialize()","Failed to initialize the Trigger Emulation Tool.");
    return EL::StatusCode::FAILURE;
  }

  // ==== Specifying the triggers to be emulated
  // L1 triggers
  std::vector<std::string> toBeEmulated_L1_triggers;

  toBeEmulated_L1_triggers.push_back("L1_4J15");
  toBeEmulated_L1_triggers.push_back("L1_2J15.31ETA49");
  toBeEmulated_L1_triggers.push_back("L1_J40.0ETA25");
  toBeEmulated_L1_triggers.push_back("L1_J20.31ETA49");
  toBeEmulated_L1_triggers.push_back("L1_J15.31ETA49");
  toBeEmulated_L1_triggers.push_back("L1_2J15");
  toBeEmulated_L1_triggers.push_back("L1_2J25");


  for (unsigned int index(0); index < toBeEmulated_L1_triggers.size(); index++)
    {
      std::vector<std::string> emulation_L1;
      emulation_L1.push_back( toBeEmulated_L1_triggers.at(index).c_str() );
      emulation_L1.push_back( Form("EMUL_%s",toBeEmulated_L1_triggers.at(index).c_str()) );
      m_triggerEmulation->addEmulatedChain( emulation_L1 );
    }

  // HLT triggers
  std::vector<std::string> toBeEmulated_HLT_triggers;

  toBeEmulated_HLT_triggers.push_back("HLT_4j35");
  toBeEmulated_HLT_triggers.push_back("HLT_4j45");
  toBeEmulated_HLT_triggers.push_back("HLT_2j35_bmv2c2060_split");
  toBeEmulated_HLT_triggers.push_back("HLT_2j45_bmv2c2070_split");
  toBeEmulated_HLT_triggers.push_back("HLT_j80_bmv2c2070_split" );
  toBeEmulated_HLT_triggers.push_back("HLT_2j60_bmv2c2085_split");
  toBeEmulated_HLT_triggers.push_back("HLT_j80_bmv2c2085_split");
  toBeEmulated_HLT_triggers.push_back("HLT_j45_320eta490");
  toBeEmulated_HLT_triggers.push_back("HLT_2j60_320eta490");
  // supporting HLT triggers
  toBeEmulated_HLT_triggers.push_back("HLT_j80_0eta240");
  toBeEmulated_HLT_triggers.push_back("HLT_2j60");


  for (unsigned int index(0); index < toBeEmulated_HLT_triggers.size(); index++)
    {
      std::vector<std::string> emulation_HLT;
      emulation_HLT.push_back( toBeEmulated_HLT_triggers.at(index).c_str() );
      emulation_HLT.push_back( Form("EMUL_%s",toBeEmulated_HLT_triggers.at(index).c_str()) );
      m_triggerEmulation->addEmulatedChain( emulation_HLT );
    }


  // Full triggers
  std::vector<std::string> emulation_4central_period1;
  emulation_4central_period1.push_back("HLT_2j35_bmv2c2060_split_2j35_L14J15");
  emulation_4central_period1.push_back("EMUL_L1_4J15");
  emulation_4central_period1.push_back("EMUL_HLT_4j35");
  emulation_4central_period1.push_back("EMUL_HLT_2j35_bmv2c2060_split");
  m_triggerEmulation->addEmulatedChain(emulation_4central_period1);

  std::vector<std::string> emulation_4central_period2;
  emulation_4central_period2.push_back("HLT_2j45_bmv2c2070_split_2j45_L14J15");
  emulation_4central_period2.push_back("EMUL_L1_4J15");
  emulation_4central_period2.push_back("EMUL_HLT_4j45");
  emulation_4central_period2.push_back("EMUL_HLT_2j45_bmv2c2070_split");
  m_triggerEmulation->addEmulatedChain(emulation_4central_period2);

  std::vector<std::string> emulation_2central;
  emulation_2central.push_back("HLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490_L1_J40.0ETA25_2J25_J20.31ETA49");
  emulation_2central.push_back("EMUL_L1_J40.0ETA25");
  emulation_2central.push_back("EMUL_L1_J20.31ETA49");
  emulation_2central.push_back("EMUL_L1_J15.31ETA49");
  emulation_2central.push_back("EMUL_L1_2J25");
  emulation_2central.push_back("EMUL_HLT_j80_bmv2c2070_split");
  emulation_2central.push_back("EMUL_HLT_2j60_bmv2c2085_split");
  emulation_2central.push_back("EMUL_HLT_j45_320eta490");
  m_triggerEmulation->addEmulatedChain(emulation_2central);

  std::vector<std::string> emulation_1central;
  emulation_1central.push_back("HLT_j80_bmv2c2085_split_2j60_320eta490_L1_J40.0ETA25_2J15.31ETA49");
  emulation_1central.push_back("EMUL_L1_J40.0ETA25");
  emulation_1central.push_back("EMUL_L1_2J15.31ETA49");
  emulation_1central.push_back("EMUL_HLT_j80_bmv2c2085_split");
  emulation_1central.push_back("EMUL_HLT_2j60_320eta490");
  m_triggerEmulation->addEmulatedChain(emulation_1central);


  // ====
  // Add Emulated Triggers
  /*m_trigDecorators [ "EmulTool_L1_4J15" ]          = &Props::passEmulToolL1_4J15;
  m_trigDecorators [ "EmulTool_L1_2J15.31ETA49" ]  = &Props::passEmulToolL1_2J15_31ETA49;
  m_trigDecorators [ "EmulTool_L1_J40.0ETA25" ]    = &Props::passEmulToolL1_J40_0ETA25;
  m_trigDecorators [ "EmulTool_L1_J20.31ETA49" ]   = &Props::passEmulToolL1_J20_31ETA49;
  m_trigDecorators [ "EmulTool_L1_2J15" ]          = &Props::passEmulToolL1_2J15;
  m_trigDecorators [ "EmulTool_L1_2J25" ]          = &Props::passEmulToolL1_2J25;

  m_trigDecorators [ "EmulTool_HLT_4j35" ]         = &Props::passEmulToolHLT_4j35;
  m_trigDecorators [ "EmulTool_HLT_4j45" ]         = &Props::passEmulToolHLT_4j45;
  m_trigDecorators [ "EmulTool_HLT_2j35_bmv2c2060_split" ] = &Props::passEmulToolHLT_2j35_bmv2c2060_split;
  m_trigDecorators [ "EmulTool_HLT_2j45_bmv2c2070_split" ] = &Props::passEmulToolHLT_2j45_bmv2c2070_split;
  m_trigDecorators [ "EmulTool_HLT_j80_bmv2c2070_split" ]  = &Props::passEmulToolHLT_j80_bmv2c2070_split;
  m_trigDecorators [ "EmulTool_HLT_2j60_bmv2c2085_split" ] = &Props::passEmulToolHLT_2j60_bmv2c2085_split;
  m_trigDecorators [ "EmulTool_HLT_j80_bmv2c2085_split" ]  = &Props::passEmulToolHLT_j80_bmv2c2085_split;
  m_trigDecorators [ "EmulTool_HLT_j45_320eta490" ]        = &Props::passEmulToolHLT_j45_320eta490;
  m_trigDecorators [ "EmulTool_HLT_2j60_320eta490" ]       = &Props::passEmulToolHLT_2j60_320eta490;
  // Supporting HLT triggers
  m_trigDecorators [ "EmulTool_HLT_j80_0eta240" ]       = &Props::passEmulToolHLT_j80_0eta240;
  m_trigDecorators [ "EmulTool_HLT_2j60" ]              = &Props::passEmulToolHLT_2j60;


  m_trigDecorators [ "EmulTool_HLT_2j35_bmv2c2060_split_2j35_L14J15" ] = &Props::passEmulToolHLT_2j35_bmv2c2060_split_2j35_L14J15;
  m_trigDecorators [ "EmulTool_HLT_2j45_bmv2c2070_split_2j45_L14J15" ] = &Props::passEmulToolHLT_2j45_bmv2c2070_split_2j45_L14J15;
  m_trigDecorators [ "EmulTool_HLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490_L1_J40.0ETA25_2J25_J20.31ETA49" ] = &Props::passEmulToolHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490_L1_J40_0ETA25_2J25_J20_31ETA49;
  m_trigDecorators [ "EmulTool_HLT_j80_bmv2c2085_split_2j60_320eta490_L1_J40.0ETA25_2J15.31ETA49" ] = &Props::passEmulToolHLT_j80_bmv2c2085_split_2j60_320eta490_L1_J40_0ETA25_2J15_31ETA49;
*/



//  uniql1jets = new xAOD::JetRoIContainer();
//  uniql1jetsaux = new xAOD::JetRoIAuxContainer();
//  uniql1jets->setStore(uniql1jetsaux);

  //initialize vbf triggers
  //trigger tool
  //----------------
  //get list of triggers from config (not a txt file!)
  std::vector<std::string> triggerList0;
  triggerList0.clear();
  m_config.getif< std::vector<std::string> >("triggersVBF", triggerList0);
  m_config.getif<bool>("doTrigEmul", m_doTrigEmul);
  // work on triggers in the list
  if(!triggerList0.size()) Warning("EventInfoHandler_VBFHbb::checkVBFTriggers()","No VBF triggers defined in config file!");
  else{
    for (const std::string & trigger : triggerList0) {
      std::cout<<"VBF trigger used : "<<trigger<<std::endl;
      // pass flags and isActive flags along with emulated trigger parts
      if      ( trigger == "L1_J40.0ETA25_2J25_J20.31ETA49"                            ) {
        m_trigDecorators [ "L1_J40.0ETA25_2J25_J20.31ETA49" ]  = &Props::passL1_J40_0ETA25_2J25_J20_31ETA49;
        m_trigIsActive   [ "L1_J40.0ETA25_2J25_J20.31ETA49" ]  = &Props::isActiveL1_J40_0ETA25_2J25_J20_31ETA49;
        m_trigDecorators [ "L1_J40.0ETA25_2J25"             ]  = &Props::passL1_J40_0ETA25_2J25;
        m_trigDecorators [ "L1_J20.31ETA49"                 ]  = &Props::passL1_J20_31ETA49;
        m_trigIsActive   [ "L1_J20.31ETA49"                 ]  = &Props::isActiveL1_J20_31ETA49;
        m_trigDecorators [ "L1_J15.31ETA49"                 ]  = &Props::passL1_J15_31ETA49;
        m_trigIsActive   [ "L1_J15.31ETA49"                 ]  = &Props::isActiveL1_J15_31ETA49;
      }
      else if ( trigger == "HLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490" ) {
        m_trigDecorators [ "HLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490" ]  = &Props::passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490;
        m_trigIsActive   [ "HLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490" ]  = &Props::isActiveHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490;
        m_trigDecorators [ "HLT_j80_bmv2c2070_split_j60_bmv2c2085_split"               ]  = &Props::passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split;
        m_trigDecorators [ "HLT_j45_320eta490"                                         ]  = &Props::passHLT_j45_320eta490;
      } 
      else if ( trigger == "HLT_j80_0eta240_j60_j45_320eta490"                         ) { 
        m_trigDecorators [ "HLT_j80_0eta240_j60_j45_320eta490" ]  = &Props::passHLT_j80_0eta240_j60_j45_320eta490;
        m_trigIsActive   [ "HLT_j80_0eta240_j60_j45_320eta490" ]  = &Props::isActiveHLT_j80_0eta240_j60_j45_320eta490;
        m_trigDecorators [ "HLT_j80_0eta240_j60"               ]  = &Props::passHLT_j80_0eta240_j60;
        m_trigDecorators [ "HLT_j45_320eta490"                 ]  = &Props::passHLT_j45_320eta490;
      } 
      else if ( trigger == "L1_J40.0ETA25_2J15.31ETA49"                                ) { 
        m_trigDecorators [ "L1_J40.0ETA25_2J15.31ETA49" ]  = &Props::passL1_J40_0ETA25_2J15_31ETA49;
        m_trigIsActive   [ "L1_J40.0ETA25_2J15.31ETA49" ]  = &Props::isActiveL1_J40_0ETA25_2J15_31ETA49;
        m_trigDecorators [ "L1_J40.0ETA25"              ]  = &Props::passL1_J40_0ETA25;
        m_trigDecorators [ "L1_2J15.31ETA49"            ]  = &Props::passL1_2J15_31ETA49;
      } 
      else if ( trigger == "HLT_j80_bmv2c2085_split_2j60_320eta490"                    ) {
        m_trigDecorators [ "HLT_j80_bmv2c2085_split_2j60_320eta490" ]  = &Props::passHLT_j80_bmv2c2085_split_2j60_320eta490;
        m_trigIsActive   [ "HLT_j80_bmv2c2085_split_2j60_320eta490" ]  = &Props::isActiveHLT_j80_bmv2c2085_split_2j60_320eta490;
        m_trigDecorators [ "HLT_j80_bmv2c2085_split"                ]  = &Props::passHLT_j80_bmv2c2085_split;
        m_trigDecorators [ "HLT_2j60_320eta490"                     ]  = &Props::passHLT_2j60_320eta490;
      } 
      else if ( trigger == "HLT_j80_0eta240_2j60_320eta490"                            ) {
        m_trigDecorators [ "HLT_j80_0eta240_2j60_320eta490" ]  = &Props::passHLT_j80_0eta240_2j60_320eta490;
        m_trigIsActive   [ "HLT_j80_0eta240_2j60_320eta490" ]  = &Props::isActiveHLT_j80_0eta240_2j60_320eta490;
        m_trigDecorators [ "HLT_j80_0eta240"                ]  = &Props::passHLT_j80_0eta240;
        m_trigDecorators [ "HLT_2j60_320eta490"             ]  = &Props::passHLT_2j60_320eta490;
      } 
      else if ( trigger == "L1_4J20"                              ) { m_trigDecorators [ "L1_4J20"                              ] = &Props::passL1_4J20                              ; m_trigIsActive [ "L1_4J20"                              ] = &Props::isActiveL1_4J20                              ; }  
      else if ( trigger == "L1_J20.31ETA49"                              ) { m_trigDecorators [ "L1_J20.31ETA49"                              ] = &Props::passL1_J20_31ETA49                              ; m_trigIsActive [ "L1_J20.31ETA49"                              ] = &Props::isActiveL1_J20_31ETA49                              ; }  
      else if ( trigger == "L1_J15.31ETA49"                              ) { m_trigDecorators [ "L1_J15.31ETA49"                              ] = &Props::passL1_J15_31ETA49                              ; m_trigIsActive [ "L1_J15.31ETA49"                              ] = &Props::isActiveL1_J15_31ETA49                              ; }  
      else if ( trigger == "HLT_2j55_bmv2c2077_split_2j55"        ) { m_trigDecorators [ "HLT_2j55_bmv2c2077_split_2j55"        ] = &Props::passHLT_2j55_bmv2c2077_split_2j55        ; m_trigIsActive [ "HLT_2j55_bmv2c2077_split_2j55"        ] = &Props::isActiveHLT_2j55_bmv2c2077_split_2j55        ; }  
      else if ( trigger == "HLT_2j45_bmv2c2070_split_2j45"        ) { m_trigDecorators [ "HLT_2j45_bmv2c2070_split_2j45"        ] = &Props::passHLT_2j45_bmv2c2070_split_2j45        ; m_trigIsActive [ "HLT_2j45_bmv2c2070_split_2j45"        ] = &Props::isActiveHLT_2j45_bmv2c2070_split_2j45        ; }  
      else if ( trigger == "L1_4J15"                              ) { m_trigDecorators [ "L1_4J15"                              ] = &Props::passL1_4J15                              ; m_trigIsActive [ "L1_4J15"                              ] = &Props::isActiveL1_4J15                              ; }  
      else if ( trigger == "HLT_2j35_bmv2c2070_split_2j35_L14J15" ) { m_trigDecorators [ "HLT_2j35_bmv2c2070_split_2j35_L14J15" ] = &Props::passHLT_2j35_bmv2c2070_split_2j35_L14J15 ; m_trigIsActive [ "HLT_2j35_bmv2c2070_split_2j35_L14J15" ] = &Props::isActiveHLT_2j35_bmv2c2070_split_2j35_L14J15 ; }  
      else if ( trigger == "HLT_2j35_bmv2c2060_split_2j35_L14J15" ) { m_trigDecorators [ "HLT_2j35_bmv2c2060_split_2j35_L14J15" ] = &Props::passHLT_2j35_bmv2c2060_split_2j35_L14J15 ; m_trigIsActive [ "HLT_2j35_bmv2c2060_split_2j35_L14J15" ] = &Props::isActiveHLT_2j35_bmv2c2060_split_2j35_L14J15 ; }  
      else if ( trigger == "HLT_2j35_bmv2c2050_split_2j35_L14J15" ) { m_trigDecorators [ "HLT_2j35_bmv2c2050_split_2j35_L14J15" ] = &Props::passHLT_2j35_bmv2c2050_split_2j35_L14J15 ; m_trigIsActive [ "HLT_2j35_bmv2c2050_split_2j35_L14J15" ] = &Props::isActiveHLT_2j35_bmv2c2050_split_2j35_L14J15 ; }  
      else if ( trigger == "HLT_j75_bmv2c2060_split_3j75_L14J15"  ) { m_trigDecorators [ "HLT_j75_bmv2c2060_split_3j75_L14J15"  ] = &Props::passHLT_j75_bmv2c2060_split_3j75_L14J15  ; m_trigIsActive [ "HLT_j75_bmv2c2060_split_3j75_L14J15"  ] = &Props::isActiveHLT_j75_bmv2c2060_split_3j75_L14J15  ; }  
      else if ( trigger == "HLT_j75_bmv2c2070_split_3j75_L14J15"  ) { m_trigDecorators [ "HLT_j75_bmv2c2070_split_3j75_L14J15"  ] = &Props::passHLT_j75_bmv2c2070_split_3j75_L14J15  ; m_trigIsActive [ "HLT_j75_bmv2c2070_split_3j75_L14J15"  ] = &Props::isActiveHLT_j75_bmv2c2070_split_3j75_L14J15  ; }  
      else if ( trigger == "HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700"  ) { m_trigDecorators [ "HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700"  ] = &Props::passHLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700  ; m_trigIsActive [ "HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700"  ] = &Props::isActiveHLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700  ; }  
      else if ( trigger == "HLT_j175_gsc225_bmv2c1040_split"  ) { m_trigDecorators [ "HLT_j175_gsc225_bmv2c1040_split"  ] = &Props::passHLT_j175_gsc225_bmv2c1040_split  ; m_trigIsActive [ "HLT_j175_gsc225_bmv2c1040_split"  ] = &Props::isActiveHLT_j175_gsc225_bmv2c1040_split  ; }  
      else if ( trigger == "HLT_j175_bmv2c2040_split"  ) { m_trigDecorators [ "HLT_j175_bmv2c2040_split"  ] = &Props::passHLT_j175_bmv2c2040_split  ; m_trigIsActive [ "HLT_j175_bmv2c2040_split"  ] = &Props::isActiveHLT_j175_bmv2c2040_split  ; }  
      else if ( trigger == "HLT_j225_bmv2c2060_split"  ) { m_trigDecorators [ "HLT_j225_bmv2c2060_split"  ] = &Props::passHLT_j225_bmv2c2060_split  ; m_trigIsActive [ "HLT_j225_bmv2c2060_split"  ] = &Props::isActiveHLT_j225_bmv2c2060_split  ; }  
      else if ( trigger == "HLT_j225_gsc275_bmv2c1060_split"  ) { m_trigDecorators [ "HLT_j225_gsc275_bmv2c1060_split"  ] = &Props::passHLT_j225_gsc275_bmv2c1060_split  ; m_trigIsActive [ "HLT_j225_gsc275_bmv2c1060_split"  ] = &Props::isActiveHLT_j225_gsc275_bmv2c1060_split  ; }  
      else if ( trigger == "HLT_j225_gsc275_bmv2c1070_split"  ) { m_trigDecorators [ "HLT_j225_gsc275_bmv2c1070_split"  ] = &Props::passHLT_j225_gsc275_bmv2c1070_split  ; m_trigIsActive [ "HLT_j225_gsc275_bmv2c1070_split"  ] = &Props::isActiveHLT_j225_gsc275_bmv2c1070_split  ; }  
      else if ( trigger == "HLT_j225_gsc300_bmv2c1070_split"  ) { m_trigDecorators [ "HLT_j225_gsc300_bmv2c1070_split"  ] = &Props::passHLT_j225_gsc300_bmv2c1070_split  ; m_trigIsActive [ "HLT_j225_gsc300_bmv2c1070_split"  ] = &Props::isActiveHLT_j225_gsc300_bmv2c1070_split  ; }  
      else if ( trigger == "HLT_j225_gsc300_bmv2c1077_split"  ) { m_trigDecorators [ "HLT_j225_gsc300_bmv2c1077_split"  ] = &Props::passHLT_j225_gsc300_bmv2c1077_split  ; m_trigIsActive [ "HLT_j225_gsc300_bmv2c1077_split"  ] = &Props::isActiveHLT_j225_gsc300_bmv2c1077_split  ; }  
      else if ( trigger == "HLT_j420"  ) { m_trigDecorators [ "HLT_j420"  ] = &Props::passHLT_j420  ; m_trigIsActive [ "HLT_j420"  ] = &Props::isActiveHLT_j420  ; }  

else if ( trigger == "HLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15.0ETA25") { m_trigDecorators [ "HLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15.0ETA25"] = &Props::passHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25; m_trigIsActive [ "HLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15.0ETA25"] = &Props::isActiveHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25; }  
else if ( trigger == "HLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15.0ETA25") { m_trigDecorators [ "HLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15.0ETA25"] = &Props::passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25; m_trigIsActive [ "HLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15.0ETA25"] = &Props::isActiveHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25; }  
else if ( trigger == "HLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15.0ETA25") { m_trigDecorators [ "HLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15.0ETA25"] = &Props::passHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25; m_trigIsActive [ "HLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15.0ETA25"] = &Props::isActiveHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25; }  
else if ( trigger == "HLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15.0ETA25") { m_trigDecorators [ "HLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15.0ETA25"] = &Props::passHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25; m_trigIsActive [ "HLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15.0ETA25"] = &Props::isActiveHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25; }  
else if ( trigger == "HLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25") { m_trigDecorators [ "HLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25"] = &Props::passHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25; m_trigIsActive [ "HLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25"] = &Props::isActiveHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25; }  
else if ( trigger == "HLT_2j35_bmv2c1060_split_2j35_L14J15.0ETA25") { m_trigDecorators [ "HLT_2j35_bmv2c1060_split_2j35_L14J15.0ETA25"] = &Props::passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25; m_trigIsActive [ "HLT_2j35_bmv2c1060_split_2j35_L14J15.0ETA25"] = &Props::isActiveHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25; }  
else if ( trigger == "HLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15.0ETA25") { m_trigDecorators [ "HLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15.0ETA25"] = &Props::passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25; m_trigIsActive [ "HLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15.0ETA25"] = &Props::isActiveHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25; }  
else if ( trigger == "HLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30") { m_trigDecorators [ "HLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30"] = &Props::passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30; m_trigIsActive [ "HLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30"] = &Props::isActiveHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30; }  
else if ( trigger == "HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30") { m_trigDecorators [ "HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30"] = &Props::passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30; m_trigIsActive [ "HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30"] = &Props::isActiveHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30; }  
else if ( trigger == "HLT_j100_2j55_bmv2c2060_split") { m_trigDecorators [ "HLT_j100_2j55_bmv2c2060_split"] = &Props::passHLT_j100_2j55_bmv2c2060_split; m_trigIsActive [ "HLT_j100_2j55_bmv2c2060_split"] = &Props::isActiveHLT_j100_2j55_bmv2c2060_split; }  
else if ( trigger == "HLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15.0ETA25") { m_trigDecorators [ "HLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15.0ETA25"] = &Props::passHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25; m_trigIsActive [ "HLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15.0ETA25"] = &Props::isActiveHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25; }  
else if ( trigger == "HLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15.0ETA25") { m_trigDecorators [ "HLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15.0ETA25"] = &Props::passHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25; m_trigIsActive [ "HLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15.0ETA25"] = &Props::isActiveHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25; }  
else if ( trigger == "HLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15.0ETA25") { m_trigDecorators [ "HLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15.0ETA25"] = &Props::passHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25; m_trigIsActive [ "HLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15.0ETA25"] = &Props::isActiveHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25; }  
else if ( trigger == "HLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25") { m_trigDecorators [ "HLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25"] = &Props::passHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25; m_trigIsActive [ "HLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25"] = &Props::isActiveHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25; }  
else if ( trigger == "HLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490") { m_trigDecorators [ "HLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490"] = &Props::passHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490; m_trigIsActive [ "HLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490"] = &Props::isActiveHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490; }  
else if ( trigger == "HLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49") { m_trigDecorators [ "HLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49"] = &Props::passHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; m_trigIsActive [ "HLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49"] = &Props::isActiveHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; }  
else if ( trigger == "HLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49") { m_trigDecorators [ "HLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49"] = &Props::passHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; m_trigIsActive [ "HLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49"] = &Props::isActiveHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; }  
else if ( trigger == "HLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49") { m_trigDecorators [ "HLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49"] = &Props::passHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; m_trigIsActive [ "HLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49"] = &Props::isActiveHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49; }  
else if ( trigger == "HLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split") { m_trigDecorators [ "HLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split"] = &Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split; m_trigIsActive [ "HLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split"] = &Props::isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split; }  
else if ( trigger == "HLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split") { m_trigDecorators [ "HLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split"] = &Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split; m_trigIsActive [ "HLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split"] = &Props::isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split; }  
else if ( trigger == "HLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split") { m_trigDecorators [ "HLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split"] = &Props::passHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split; m_trigIsActive [ "HLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split"] = &Props::isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split; }  
else if ( trigger == "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j25_gsc45_bmv2c1070_split") { m_trigDecorators [ "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j25_gsc45_bmv2c1070_split"] = &Props::passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split; m_trigIsActive [ "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j25_gsc45_bmv2c1070_split"] = &Props::isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split; }  
else if ( trigger == "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF") { m_trigDecorators [ "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF"] = &Props::passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF; m_trigIsActive [ "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF"] = &Props::isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF; }  
else if ( trigger == "HLT_noalg_l1topo_L1MJJ-400-CF") { m_trigDecorators [ "HLT_noalg_l1topo_L1MJJ-400-CF"] = &Props::passHLT_noalg_l1topo_L1MJJ_400_CF; m_trigIsActive [ "HLT_noalg_l1topo_L1MJJ-400-CF"] = &Props::isActiveHLT_noalg_l1topo_L1MJJ_400_CF; }  
else if ( trigger == "L1_HT150-J20s5.ETA31_MJJ-400-CF") { m_trigDecorators [ "L1_HT150-J20s5.ETA31_MJJ-400-CF"] = &Props::passL1_HT150_J20s5_ETA31_MJJ_400_CF; m_trigIsActive [ "L1_HT150-J20s5.ETA31_MJJ-400-CF"] = &Props::isActiveL1_HT150_J20s5_ETA31_MJJ_400_CF; }  
else if ( trigger == "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j35_gsc45_bmv2c1070_split") { m_trigDecorators [ "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j35_gsc45_bmv2c1070_split"] = &Props::passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split; m_trigIsActive [ "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j35_gsc45_bmv2c1070_split"] = &Props::isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split; }  
else if ( trigger == "HLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21") { m_trigDecorators [ "HLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21"] = &Props::passHLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190_J15s5_ETA21; m_trigIsActive [ "HLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21"] = &Props::isActiveHLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190_J15s5_ETA21; }  
else if ( trigger == "HLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-500-NFF") { m_trigDecorators [ "HLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-500-NFF"] = &Props::passHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF; m_trigIsActive [ "HLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-500-NFF"] = &Props::isActiveHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF; }  
else if ( trigger == "HLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_bmv2c1070_split") { m_trigDecorators [ "HLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_bmv2c1070_split"] = &Props::passHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split; m_trigIsActive [ "HLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_bmv2c1070_split"] = &Props::isActiveHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split; }  

      // triggers for turn on study with gamma                                                                                                                                                                                                                                                                                                                                                                        
      else if ( trigger == "HLT_g140_loose" ) { m_trigDecorators [ "HLT_g140_loose" ] = &Props::passHLT_g140_loose ; }
      //else if ( trigger == "HLT_g25_medium" ) { m_trigDecorators [ "HLT_g25_medium" ] = &Props::passHLT_g25_medium ; }
      else if ( trigger == "HLT_2j45_bmv2c2077_split_2j45_L14J15" ) { m_trigDecorators [ "HLT_2j45_bmv2c2077_split_2j45_L14J15" ] = &Props::passHLT_2j45_bmv2c2077_split_2j45_L14J15 ; m_trigIsActive [ "HLT_2j45_bmv2c2077_split_2j45_L14J15" ] = &Props::isActiveHLT_2j45_bmv2c2077_split_2j45_L14J15 ; }

      else if ( trigger == "HLT_2j45_bmv2c2070_split_2j45_L14J15" ) { m_trigDecorators [ "HLT_2j45_bmv2c2070_split_2j45_L14J15" ] = &Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15 ; m_trigIsActive [ "HLT_2j45_bmv2c2070_split_2j45_L14J15" ] = &Props::isActiveHLT_2j45_bmv2c2070_split_2j45_L14J15 ;}
      else if ( trigger == "HLT_2j45_bmv2c2070_split_2j45_L14J15" ) { m_trigDecorators [ "HLT_2j45_bmv2c2070_split_2j45_L14J15" ] = &Props::passHLT_2j45_bmv2c2070_split_2j45_L14J15 ; m_trigIsActive [ "HLT_2j45_bmv2c2070_split_2j45_L14J15" ] = &Props::isActiveHLT_2j45_bmv2c2070_split_2j45_L14J15 ;}
      else if ( trigger == "HLT_4j45"                             ) { m_trigDecorators [ "HLT_4j45"                             ] = &Props::passHLT_4j45                             ; m_trigIsActive [ "HLT_4j45"                             ] = &Props::isActiveHLT_4j45                             ; }
      else {
        Error("EventInfoHandler_VBFHbb::initialize()","No pass decorator for trigger %s defined! Need to create one.", trigger.c_str());
        return EL::StatusCode::FAILURE;
      }
    }
  }

  //add prescales to a map
  if(!m_triggerList.size()) Warning("EventInfoHandler_VBFHbb::initialize()","No trigger defined when form back the trigger list!");
  else{
    // setup vector of prescale pointers
    for (const std::string & trigger : m_triggerList) {
      // trigger flags
      if ( trigger == "HLT_e15_lhtight_ivarloose_3j20_L1EM13VH_3J20") m_trigPrescales["HLT_e15_lhtight_ivarloose_3j20_L1EM13VH_3J20"] = &Props::prescaleHLT_e15_lhtight_ivarloose_3j20_L1EM13VH_3J20;

      else if ( trigger == "L1_EM13VH" ) m_trigPrescales["L1_EM13VH"] = &Props::prescaleL1_EM13VH;
      else if ( trigger == "L1_EM15VH" ) m_trigPrescales["L1_EM15VH"] = &Props::prescaleL1_EM15VH;
      else if ( trigger == "L1_EM18VH" ) m_trigPrescales["L1_EM18VH"] = &Props::prescaleL1_EM18VH;
      else if ( trigger == "L1_EM20VH" ) m_trigPrescales["L1_EM20VH"] = &Props::prescaleL1_EM20VH;
      else if ( trigger == "L1_EM22VHI" ) m_trigPrescales["L1_EM22VHI"] = &Props::prescaleL1_EM22VHI;
      else if ( trigger == "L1_2EM15VH" ) m_trigPrescales["L1_2EM15VH"] = &Props::prescaleL1_2EM15VH;
      else if ( trigger == "HLT_e17_lhloose_L1EM15" ) m_trigPrescales["HLT_e17_lhloose_L1EM15"] = &Props::prescaleHLT_e17_lhloose_L1EM15;
      else if ( trigger == "HLT_e24_lhvloose_L1EM18VH" ) m_trigPrescales["HLT_e24_lhvloose_L1EM18VH"] = &Props::prescaleHLT_e24_lhvloose_L1EM18VH;
      else if ( trigger == "HLT_e24_lhvloose_L1EM20VH" ) m_trigPrescales["HLT_e24_lhvloose_L1EM20VH"] = &Props::prescaleHLT_e24_lhvloose_L1EM20VH;
      else if ( trigger == "HLT_e24_medium_iloose_L1EM18VH" ) m_trigPrescales["HLT_e24_medium_iloose_L1EM18VH"] = &Props::prescaleHLT_e24_medium_iloose_L1EM18VH;
      else if ( trigger == "HLT_e24_medium_iloose_L1EM20VH" ) m_trigPrescales["HLT_e24_medium_iloose_L1EM20VH"] = &Props::prescaleHLT_e24_medium_iloose_L1EM20VH;
      else if ( trigger == "HLT_e24_lhmedium_iloose_L1EM18VH" ) m_trigPrescales["HLT_e24_lhmedium_iloose_L1EM18VH"] = &Props::prescaleHLT_e24_lhmedium_iloose_L1EM18VH;
      else if ( trigger == "HLT_e24_lhmedium_iloose_L1EM20VH" ) m_trigPrescales["HLT_e24_lhmedium_iloose_L1EM20VH"] = &Props::prescaleHLT_e24_lhmedium_iloose_L1EM20VH;

      else if ( trigger == "HLT_e24_lhmedium_L1EM18VH" ) m_trigPrescales["HLT_e24_lhmedium_L1EM18VH"] = &Props::prescaleHLT_e24_lhmedium_L1EM18VH;
      else if ( trigger == "HLT_e24_lhmedium_L1EM20VH" ) m_trigPrescales["HLT_e24_lhmedium_L1EM20VH"] = &Props::prescaleHLT_e24_lhmedium_L1EM20VH;
      else if ( trigger == "HLT_e24_lhtight_iloose" ) m_trigPrescales["HLT_e24_lhtight_iloose"] = &Props::prescaleHLT_e24_lhtight_iloose;
      else if ( trigger == "HLT_e24_tight_iloose" ) m_trigPrescales["HLT_e24_tight_iloose"] = &Props::prescaleHLT_e24_tight_iloose;
      else if ( trigger == "HLT_e26_tight_iloose" ) m_trigPrescales["HLT_e26_tight_iloose"] = &Props::prescaleHLT_e26_tight_iloose;
      else if ( trigger == "HLT_e26_tight1_iloose" ) m_trigPrescales["HLT_e26_tight1_iloose"] = &Props::prescaleHLT_e26_tight1_iloose;
      else if ( trigger == "HLT_e26_lhtight_iloose" ) m_trigPrescales["HLT_e26_lhtight_iloose"] = &Props::prescaleHLT_e26_lhtight_iloose;
      else if ( trigger == "HLT_e26_lhmedium_nod0_ivarloose" ) m_trigPrescales["HLT_e26_lhmedium_nod0_ivarloose"] = &Props::prescaleHLT_e26_lhmedium_nod0_ivarloose;
      else if ( trigger == "HLT_e60_medium" ) m_trigPrescales["HLT_e60_medium"] = &Props::prescaleHLT_e60_medium;
      else if ( trigger == "HLT_e60_medium1" ) m_trigPrescales["HLT_e60_medium1"] = &Props::prescaleHLT_e60_medium1;
      else if ( trigger == "HLT_e60_lhmedium" ) m_trigPrescales["HLT_e60_lhmedium"] = &Props::prescaleHLT_e60_lhmedium;
      else if ( trigger == "HLT_e60_lhmedium_nod0" ) m_trigPrescales["HLT_e60_lhmedium_nod0"] = &Props::prescaleHLT_e60_lhmedium_nod0;
      else if ( trigger == "HLT_e120_lhloose" ) m_trigPrescales["HLT_e120_lhloose"] = &Props::prescaleHLT_e120_lhloose;
      else if ( trigger == "HLT_e120_lhloose_nod0" ) m_trigPrescales["HLT_e120_lhloose_nod0"] = &Props::prescaleHLT_e120_lhloose_nod0;
      else if ( trigger == "HLT_e140_lhloose" ) m_trigPrescales["HLT_e140_lhloose"] = &Props::prescaleHLT_e140_lhloose;
      else if ( trigger == "HLT_e140_lhloose_nod0" ) m_trigPrescales["HLT_e140_lhloose_nod0"] = &Props::prescaleHLT_e140_lhloose_nod0;
      else if ( trigger == "HLT_2e17_loose" ) m_trigPrescales["HLT_2e17_loose"] = &Props::prescaleHLT_2e17_loose;
      else if ( trigger == "HLT_2e17_loose1" ) m_trigPrescales["HLT_2e17_loose1"] = &Props::prescaleHLT_2e17_loose1;
      else if ( trigger == "HLT_2e17_lhloose" ) m_trigPrescales["HLT_2e17_lhloose"] = &Props::prescaleHLT_2e17_lhloose;
      else if ( trigger == "HLT_e26_lhtight_ivarloose" ) m_trigPrescales["HLT_e26_lhtight_ivarloose"] = &Props::prescaleHLT_e26_lhtight_ivarloose;
      else if ( trigger == "HLT_e26_lhtight_nod0_ivarloose" ) m_trigPrescales["HLT_e26_lhtight_nod0_ivarloose"] = &Props::prescaleHLT_e26_lhtight_nod0_ivarloose;
      else if ( trigger == "HLT_e26_lhtight_smooth_ivarloose" ) m_trigPrescales["HLT_e26_lhtight_smooth_ivarloose"] = &Props::prescaleHLT_e26_lhtight_smooth_ivarloose;
      else if ( trigger == "HLT_e24_lhtight_ivarloose" ) m_trigPrescales["HLT_e24_lhtight_ivarloose"] = &Props::prescaleHLT_e24_lhtight_ivarloose;
      else if ( trigger == "HLT_e24_lhtight_nod0_ivarloose" ) m_trigPrescales["HLT_e24_lhtight_nod0_ivarloose"] = &Props::prescaleHLT_e24_lhtight_nod0_ivarloose;
      else if ( trigger == "HLT_e24_lhmedium_ivarloose" ) m_trigPrescales["HLT_e24_lhmedium_ivarloose"] = &Props::prescaleHLT_e24_lhmedium_ivarloose;
      else if ( trigger == "HLT_e24_lhmedium_nod0_ivarloose" ) m_trigPrescales["HLT_e24_lhmedium_nod0_ivarloose"] = &Props::prescaleHLT_e24_lhmedium_nod0_ivarloose;
      else if ( trigger == "HLT_e24_lhmedium_nod0_L1EM20VH" ) m_trigPrescales["HLT_e24_lhmedium_nod0_L1EM20VH"] = &Props::prescaleHLT_e24_lhmedium_nod0_L1EM20VH;
      else if ( trigger == "HLT_2e17_lhloose_nod0" ) m_trigPrescales["HLT_2e17_lhloose_nod0"] = &Props::prescaleHLT_2e17_lhloose_nod0;
      else if ( trigger == "HLT_2e15_lhloose_L12EM13VH" ) m_trigPrescales["HLT_2e15_lhloose_L12EM13VH"] = &Props::prescaleHLT_2e15_lhloose_L12EM13VH;
      else if ( trigger == "HLT_2e15_lhloose_nod0_L12EM13VH" ) m_trigPrescales["HLT_2e15_lhloose_nod0_L12EM13VH"] = &Props::prescaleHLT_2e15_lhloose_nod0_L12EM13VH;
      else if ( trigger == "HLT_2e12_lhloose_L12EM10VH" ) m_trigPrescales["HLT_2e12_lhloose_L12EM10VH"] = &Props::prescaleHLT_2e12_lhloose_L12EM10VH;
      else if ( trigger == "HLT_2e12_lhloose_nod0_L12EM10VH" ) m_trigPrescales["HLT_2e12_lhloose_nod0_L12EM10VH"] = &Props::prescaleHLT_2e12_lhloose_nod0_L12EM10VH;
      else if ( trigger == "HLT_g10_loose" ) m_trigPrescales["HLT_g10_loose"] = &Props::prescaleHLT_g10_loose;
      else if ( trigger == "HLT_g15_loose_L1EM7" ) m_trigPrescales["HLT_g15_loose_L1EM7"] = &Props::prescaleHLT_g15_loose_L1EM7;
      else if ( trigger == "HLT_g20_loose_L1EM12" ) m_trigPrescales["HLT_g20_loose_L1EM12"] = &Props::prescaleHLT_g20_loose_L1EM12;
      else if ( trigger == "HLT_g25_loose_L1EM15" ) m_trigPrescales["HLT_g25_loose_L1EM15"] = &Props::prescaleHLT_g25_loose_L1EM15;
      else if ( trigger == "HLT_g35_loose_L1EM15" ) m_trigPrescales["HLT_g35_loose_L1EM15"] = &Props::prescaleHLT_g35_loose_L1EM15;
      else if ( trigger == "HLT_g40_loose_L1EM15" ) m_trigPrescales["HLT_g40_loose_L1EM15"] = &Props::prescaleHLT_g40_loose_L1EM15;
      else if ( trigger == "HLT_g45_loose_L1EM15" ) m_trigPrescales["HLT_g45_loose_L1EM15"] = &Props::prescaleHLT_g45_loose_L1EM15;
      else if ( trigger == "HLT_g50_loose_L1EM15" ) m_trigPrescales["HLT_g50_loose_L1EM15"] = &Props::prescaleHLT_g50_loose_L1EM15;
      else if ( trigger == "HLT_g60_loose" ) m_trigPrescales["HLT_g60_loose"] = &Props::prescaleHLT_g60_loose;
      else if ( trigger == "HLT_g70_loose" ) m_trigPrescales["HLT_g70_loose"] = &Props::prescaleHLT_g70_loose;
      else if ( trigger == "HLT_g80_loose" ) m_trigPrescales["HLT_g80_loose"] = &Props::prescaleHLT_g80_loose;
      else if ( trigger == "HLT_g100_loose" ) m_trigPrescales["HLT_g100_loose"] = &Props::prescaleHLT_g100_loose;
      else if ( trigger == "HLT_g120_loose" ) m_trigPrescales["HLT_g120_loose"] = &Props::prescaleHLT_g120_loose;
      else if ( trigger == "L1_MJJ-400" ) m_trigPrescales["L1_MJJ-400"] = &Props::prescaleL1_MJJ_400;
      else if ( trigger == "L1_MJJ-700" ) m_trigPrescales["L1_MJJ-700"] = &Props::prescaleL1_MJJ_700;
      else if ( trigger == "L1_MJJ-800" ) m_trigPrescales["L1_MJJ-800"] = &Props::prescaleL1_MJJ_800;
      else if ( trigger == "L1_MJJ-900" ) m_trigPrescales["L1_MJJ-900"] = &Props::prescaleL1_MJJ_900;
      else if ( trigger == "L1_J30_2J20_4J20.0ETA49_MJJ-400" ) m_trigPrescales["L1_J30_2J20_4J20.0ETA49_MJJ-400"] = &Props::prescaleL1_J30_2J20_4J20_0ETA49_MJJ_400;
      else if ( trigger == "L1_J30_2J20_4J20.0ETA49_MJJ-700" ) m_trigPrescales["L1_J30_2J20_4J20.0ETA49_MJJ-700"] = &Props::prescaleL1_J30_2J20_4J20_0ETA49_MJJ_700;
      else if ( trigger == "L1_J30_2J20_4J20.0ETA49_MJJ-800" ) m_trigPrescales["L1_J30_2J20_4J20.0ETA49_MJJ-800"] = &Props::prescaleL1_J30_2J20_4J20_0ETA49_MJJ_800;
      else if ( trigger == "L1_J30_2J20_4J20.0ETA49_MJJ-900" ) m_trigPrescales["L1_J30_2J20_4J20.0ETA49_MJJ-900"] = &Props::prescaleL1_J30_2J20_4J20_0ETA49_MJJ_900;
      else if ( trigger == "L1_3J20_4J20.0ETA49_MJJ-400" ) m_trigPrescales["L1_3J20_4J20.0ETA49_MJJ-400"] = &Props::prescaleL1_3J20_4J20_0ETA49_MJJ_400;
      else if ( trigger == "L1_3J20_4J20.0ETA49_MJJ-700" ) m_trigPrescales["L1_3J20_4J20.0ETA49_MJJ-700"] = &Props::prescaleL1_3J20_4J20_0ETA49_MJJ_700;
      else if ( trigger == "L1_3J20_4J20.0ETA49_MJJ-800" ) m_trigPrescales["L1_3J20_4J20.0ETA49_MJJ-800"] = &Props::prescaleL1_3J20_4J20_0ETA49_MJJ_800;
      else if ( trigger == "L1_3J20_4J20.0ETA49_MJJ-900" ) m_trigPrescales["L1_3J20_4J20.0ETA49_MJJ-900"] = &Props::prescaleL1_3J20_4J20_0ETA49_MJJ_900;
      else if ( trigger == "L1_4J20.0ETA49" ) m_trigPrescales["L1_4J20.0ETA49"] = &Props::prescaleL1_4J20_0ETA49;
      else if ( trigger == "HLT_g15_loose_2j40_0eta490_3j25_0eta490" ) m_trigPrescales["HLT_g15_loose_2j40_0eta490_3j25_0eta490"] = &Props::prescaleHLT_g15_loose_2j40_0eta490_3j25_0eta490;
      else if ( trigger == "HLT_g20_loose_2j40_0eta490_3j25_0eta490" ) m_trigPrescales["HLT_g20_loose_2j40_0eta490_3j25_0eta490"] = &Props::prescaleHLT_g20_loose_2j40_0eta490_3j25_0eta490;
      else if ( trigger == "HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700" ) m_trigPrescales["HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700"] = &Props::prescaleHLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ_700;
      else if ( trigger == "HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-900" ) m_trigPrescales["HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-900"] = &Props::prescaleHLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ_900;
      else if ( trigger == "HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490" ) m_trigPrescales["HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490"] = &Props::prescaleHLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490;
      else if ( trigger == "HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490_invm700" ) m_trigPrescales["HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490_invm700"] = &Props::prescaleHLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490_invm700;
      else if ( trigger == "HLT_g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700" ) m_trigPrescales["HLT_g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700"] = &Props::prescaleHLT_g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700;
      else if ( trigger == "HLT_g25_loose_2j40_0eta490_3j25_0eta490_invm700" ) m_trigPrescales["HLT_g25_loose_2j40_0eta490_3j25_0eta490_invm700"] = &Props::prescaleHLT_g25_loose_2j40_0eta490_3j25_0eta490_invm700;
      else if ( trigger == "HLT_g25_medium_L1EM22VHI_4j35_0eta490_invm700" ) m_trigPrescales["HLT_g25_medium_L1EM22VHI_4j35_0eta490_invm700"] = &Props::prescaleHLT_g25_medium_L1EM22VHI_4j35_0eta490_invm700;
      else if ( trigger == "HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j35_0eta490_invm700" ) m_trigPrescales["HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j35_0eta490_invm700"] = &Props::prescaleHLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j35_0eta490_invm700;
      else if ( trigger == "HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_2j35_0eta490" ) m_trigPrescales["HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_2j35_0eta490"] = &Props::prescaleHLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_2j35_0eta490;
      else if ( trigger == "HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_split_3j35_0eta490_invm700" ) m_trigPrescales["HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_split_3j35_0eta490_invm700"] = &Props::prescaleHLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_split_3j35_0eta490_invm700;
      else if ( trigger == "HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_split_2j35_0eta490" ) m_trigPrescales["HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_split_2j35_0eta490"] = &Props::prescaleHLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_split_2j35_0eta490;
      else if ( trigger == "HLT_g25_medium_L1EM22VHI_j35_0eta490_boffperf_split_3j35_0eta490_invm700" ) m_trigPrescales["HLT_g25_medium_L1EM22VHI_j35_0eta490_boffperf_split_3j35_0eta490_invm700"] = &Props::prescaleHLT_g25_medium_L1EM22VHI_j35_0eta490_boffperf_split_3j35_0eta490_invm700;
      else if ( trigger == "HLT_g25_medium_L1EM22VHI_2j35_0eta490_boffperf_split_2j35_0eta490" ) m_trigPrescales["HLT_g25_medium_L1EM22VHI_2j35_0eta490_boffperf_split_2j35_0eta490"] = &Props::prescaleHLT_g25_medium_L1EM22VHI_2j35_0eta490_boffperf_split_2j35_0eta490;
      else if ( trigger == "HLT_g25_loose_L1EM20VH_4j35_0eta490" ) m_trigPrescales["HLT_g25_loose_L1EM20VH_4j35_0eta490"] = &Props::prescaleHLT_g25_loose_L1EM20VH_4j35_0eta490;
      else if ( trigger == "HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-400" ) m_trigPrescales["HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-400"] = &Props::prescaleHLT_2j55_bperf_L1J30_2J20_4J20_0ETA49_MJJ_400;
      else if ( trigger == "HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-700" ) m_trigPrescales["HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-700"] = &Props::prescaleHLT_2j55_bperf_L1J30_2J20_4J20_0ETA49_MJJ_700;
      else if ( trigger == "HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-800" ) m_trigPrescales["HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-800"] = &Props::prescaleHLT_2j55_bperf_L1J30_2J20_4J20_0ETA49_MJJ_800;
      else if ( trigger == "HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-900" ) m_trigPrescales["HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-900"] = &Props::prescaleHLT_2j55_bperf_L1J30_2J20_4J20_0ETA49_MJJ_900;
      else if ( trigger == "HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-400" ) m_trigPrescales["HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-400"] = &Props::prescaleHLT_2j55_bperf_L13J20_4J20_0ETA49_MJJ_400;
      else if ( trigger == "HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-700" ) m_trigPrescales["HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-700"] = &Props::prescaleHLT_2j55_bperf_L13J20_4J20_0ETA49_MJJ_700;
      else if ( trigger == "HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-800" ) m_trigPrescales["HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-800"] = &Props::prescaleHLT_2j55_bperf_L13J20_4J20_0ETA49_MJJ_800;
      else if ( trigger == "HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-900" ) m_trigPrescales["HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-900"] = &Props::prescaleHLT_2j55_bperf_L13J20_4J20_0ETA49_MJJ_900;
      else if ( trigger == "HLT_2j55_bperf_L14J20.0ETA49" ) m_trigPrescales["HLT_2j55_bperf_L14J20.0ETA49"] = &Props::prescaleHLT_2j55_bperf_L14J20_0ETA49;
      else if ( trigger == "HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-400" ) m_trigPrescales["HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-400"] = &Props::prescaleHLT_2j55_bloose_L1J30_2J20_4J20_0ETA49_MJJ_400;
      else if ( trigger == "HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-700" ) m_trigPrescales["HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-700"] = &Props::prescaleHLT_2j55_bloose_L1J30_2J20_4J20_0ETA49_MJJ_700;
      else if ( trigger == "HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-800" ) m_trigPrescales["HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-800"] = &Props::prescaleHLT_2j55_bloose_L1J30_2J20_4J20_0ETA49_MJJ_800;
      else if ( trigger == "HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-900" ) m_trigPrescales["HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-900"] = &Props::prescaleHLT_2j55_bloose_L1J30_2J20_4J20_0ETA49_MJJ_900;
      else if ( trigger == "HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-400" ) m_trigPrescales["HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-400"] = &Props::prescaleHLT_2j55_bloose_L13J20_4J20_0ETA49_MJJ_400;
      else if ( trigger == "HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-700" ) m_trigPrescales["HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-700"] = &Props::prescaleHLT_2j55_bloose_L13J20_4J20_0ETA49_MJJ_700;
      else if ( trigger == "HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-800" ) m_trigPrescales["HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-800"] = &Props::prescaleHLT_2j55_bloose_L13J20_4J20_0ETA49_MJJ_800;
      else if ( trigger == "HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-900" ) m_trigPrescales["HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-900"] = &Props::prescaleHLT_2j55_bloose_L13J20_4J20_0ETA49_MJJ_900;
      else if ( trigger == "HLT_2j55_bloose_L14J20.0ETA49" ) m_trigPrescales["HLT_2j55_bloose_L14J20.0ETA49"] = &Props::prescaleHLT_2j55_bloose_L14J20_0ETA49;
      else if ( trigger == "HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-400" ) m_trigPrescales["HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-400"] = &Props::prescaleHLT_2j55_bmedium_L13J20_4J20_0ETA49_MJJ_400;
      else if ( trigger == "HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-700" ) m_trigPrescales["HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-700"] = &Props::prescaleHLT_2j55_bmedium_L13J20_4J20_0ETA49_MJJ_700;
      else if ( trigger == "HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-800" ) m_trigPrescales["HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-800"] = &Props::prescaleHLT_2j55_bmedium_L13J20_4J20_0ETA49_MJJ_800;
      else if ( trigger == "HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-900" ) m_trigPrescales["HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-900"] = &Props::prescaleHLT_2j55_bmedium_L13J20_4J20_0ETA49_MJJ_900;
      else if ( trigger == "HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-400" ) m_trigPrescales["HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-400"] = &Props::prescaleHLT_2j55_bmedium_L1J30_2J20_4J20_0ETA49_MJJ_400;
      else if ( trigger == "HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-700" ) m_trigPrescales["HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-700"] = &Props::prescaleHLT_2j55_bmedium_L1J30_2J20_4J20_0ETA49_MJJ_700;
      else if ( trigger == "HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-800" ) m_trigPrescales["HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-800"] = &Props::prescaleHLT_2j55_bmedium_L1J30_2J20_4J20_0ETA49_MJJ_800;
      else if ( trigger == "HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-900" ) m_trigPrescales["HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-900"] = &Props::prescaleHLT_2j55_bmedium_L1J30_2J20_4J20_0ETA49_MJJ_900;
      else if ( trigger == "HLT_2j55_bmedium_L14J20.0ETA49" ) m_trigPrescales["HLT_2j55_bmedium_L14J20.0ETA49"] = &Props::prescaleHLT_2j55_bmedium_L14J20_0ETA49;
      else if ( trigger == "L1_EM20VHI" ) m_trigPrescales["L1_EM20VHI"] = &Props::prescaleL1_EM20VHI;
      else if ( trigger == "HLT_j300" ) m_trigPrescales["HLT_j300" ] = &Props::prescaleHLT_j300;
      else if ( trigger == "HLT_j320" ) m_trigPrescales["HLT_j320" ] = &Props::prescaleHLT_j320;
      else if ( trigger == "HLT_j360" ) m_trigPrescales["HLT_j360" ] = &Props::prescaleHLT_j360;
      else if ( trigger == "HLT_j380" ) m_trigPrescales["HLT_j380" ] = &Props::prescaleHLT_j380;
      else if ( trigger == "HLT_j400" ) m_trigPrescales["HLT_j400" ] = &Props::prescaleHLT_j400;
      else if ( trigger == "HLT_j420" ) m_trigPrescales["HLT_j420" ] = &Props::prescaleHLT_j420;
      else if ( trigger == "HLT_j440" ) m_trigPrescales["HLT_j440" ] = &Props::prescaleHLT_j440;
      else if ( trigger == "HLT_j460" ) m_trigPrescales["HLT_j460" ] = &Props::prescaleHLT_j460;
      else if ( trigger == "HLT_j45_bperf" ) m_trigPrescales["HLT_j45_bperf" ] = &Props::prescaleHLT_j45_bperf;
      // trigger objects
      //jet and photon only for HIGG5D1/3
      else {
        Warning("EventInfoHandler_VBFHbb::initialize()","No prescale for trigger %s defined! Need to create one.", trigger.c_str());
        //return EL::StatusCode::FAILURE;
      }
    }    

    //vbf triggers
    if(!triggerList0.size()) Warning("EventInfoHandler_VBFHbb::initialize()","No VBF triggers defined in config file!");
    else{
      for (const std::string & trigger : triggerList0) {
        // trigger flags
        if      ( trigger == "L1_J40.0ETA25_2J25_J20.31ETA49" ) m_trigPrescales["L1_J40.0ETA25_2J25_J20.31ETA49"] = &Props::prescaleL1_J40_0ETA25_2J25_J20_31ETA49;
        else if      ( trigger == "L1_J20.31ETA49" ) m_trigPrescales["L1_J20.31ETA49"] = &Props::prescaleL1_J20_31ETA49;
        else if      ( trigger == "L1_J15.31ETA49" ) m_trigPrescales["L1_J15.31ETA49"] = &Props::prescaleL1_J15_31ETA49;
        else if ( trigger == "HLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490" )  
          m_trigPrescales["HLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490"] =     &Props::prescaleHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490;
        else if ( trigger == "HLT_j80_0eta240_j60_j45_320eta490" )     m_trigPrescales["HLT_j80_0eta240_j60_j45_320eta490"] =     &Props::prescaleHLT_j80_0eta240_j60_j45_320eta490;
        else if ( trigger == "L1_J40.0ETA25_2J15.31ETA49" ) m_trigPrescales["L1_J40.0ETA25_2J15.31ETA49"] = &Props::prescaleL1_J40_0ETA25_2J15_31ETA49;
        else if ( trigger == "HLT_j80_bmv2c2085_split_2j60_320eta490" ) 
          m_trigPrescales["HLT_j80_bmv2c2085_split_2j60_320eta490"] =     &Props::prescaleHLT_j80_bmv2c2085_split_2j60_320eta490;
        else if ( trigger == "HLT_j80_0eta240_2j60_320eta490" )     m_trigPrescales["HLT_j80_0eta240_2j60_320eta490"] =     &Props::prescaleHLT_j80_0eta240_2j60_320eta490;
        else if ( trigger == "L1_4J20" )     m_trigPrescales["L1_4J20"] =     &Props::prescaleL1_4J20;
        else if ( trigger == "HLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15.0ETA25" )     m_trigPrescales["HLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15.0ETA25"] =     &Props::prescaleHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25;
        else if ( trigger == "HLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15.0ETA25" )     m_trigPrescales["HLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15.0ETA25"] =     &Props::prescaleHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25;
        else if ( trigger == "HLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15.0ETA25" )     m_trigPrescales["HLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15.0ETA25"] =     &Props::prescaleHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25;
        else if ( trigger == "HLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15.0ETA25" )     m_trigPrescales["HLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15.0ETA25"] =     &Props::prescaleHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25;
        else if ( trigger == "HLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25" )     m_trigPrescales["HLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25"] =     &Props::prescaleHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25;
        else if ( trigger == "HLT_2j35_bmv2c1060_split_2j35_L14J15.0ETA25" )     m_trigPrescales["HLT_2j35_bmv2c1060_split_2j35_L14J15.0ETA25"] =     &Props::prescaleHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25;
        else if ( trigger == "HLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15.0ETA25" )     m_trigPrescales["HLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15.0ETA25"] =     &Props::prescaleHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25;
        else if ( trigger == "HLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30" )     m_trigPrescales["HLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30"] =     &Props::prescaleHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30;
        else if ( trigger == "HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30" )     m_trigPrescales["HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30"] =     &Props::prescaleHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30;
        else if ( trigger == "HLT_j100_2j55_bmv2c2060_split" )     m_trigPrescales["HLT_j100_2j55_bmv2c2060_split"] =     &Props::prescaleHLT_j100_2j55_bmv2c2060_split;
        else if ( trigger == "HLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15.0ETA25" )     m_trigPrescales["HLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15.0ETA25"] =     &Props::prescaleHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25;
        else if ( trigger == "HLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15.0ETA25" )     m_trigPrescales["HLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15.0ETA25"] =     &Props::prescaleHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25;
        else if ( trigger == "HLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15.0ETA25" )     m_trigPrescales["HLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15.0ETA25"] =     &Props::prescaleHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25;
        else if ( trigger == "HLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25" )     m_trigPrescales["HLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25"] =     &Props::prescaleHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25;
        else if ( trigger == "HLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490" )     m_trigPrescales["HLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490"] =     &Props::prescaleHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490;
        else if ( trigger == "HLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49" )     m_trigPrescales["HLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49"] =     &Props::prescaleHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49;
        else if ( trigger == "HLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49" )     m_trigPrescales["HLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49"] =     &Props::prescaleHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49;
        else if ( trigger == "HLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49" )     m_trigPrescales["HLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49"] =     &Props::prescaleHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49;
        else if ( trigger == "HLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split" )     m_trigPrescales["HLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split"] =     &Props::prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split;
        else if ( trigger == "HLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split" )     m_trigPrescales["HLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split"] =     &Props::prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split;
        else if ( trigger == "HLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split" )     m_trigPrescales["HLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split"] =     &Props::prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split;
        else if ( trigger == "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j25_gsc45_bmv2c1070_split" )     m_trigPrescales["HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j25_gsc45_bmv2c1070_split"] =     &Props::prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split;
        else if ( trigger == "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF" )     m_trigPrescales["HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF"] =     &Props::prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF;
        else if ( trigger == "HLT_noalg_l1topo_L1MJJ-400-CF" )     m_trigPrescales["HLT_noalg_l1topo_L1MJJ-400-CF"] =     &Props::prescaleHLT_noalg_l1topo_L1MJJ_400_CF;
        else if ( trigger == "L1_HT150-J20s5.ETA31_MJJ-400-CF" )     m_trigPrescales["L1_HT150-J20s5.ETA31_MJJ-400-CF"] =     &Props::prescaleL1_HT150_J20s5_ETA31_MJJ_400_CF;
        else if ( trigger == "HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j35_gsc45_bmv2c1070_split" )     m_trigPrescales["HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j35_gsc45_bmv2c1070_split"] =     &Props::prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split;
        else if ( trigger == "HLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21" )     m_trigPrescales["HLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21"] =     &Props::prescaleHLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190_J15s5_ETA21;
        else if ( trigger == "HLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-500-NFF" )     m_trigPrescales["HLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-500-NFF"] =     &Props::prescaleHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF;
        else if ( trigger == "HLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_bmv2c1070_split" )     m_trigPrescales["HLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_bmv2c1070_split"] =     &Props::prescaleHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split;
        else if ( trigger == "HLT_2j55_bmv2c2077_split_2j55" )     m_trigPrescales["HLT_2j55_bmv2c2077_split_2j55"] =     &Props::prescaleHLT_2j55_bmv2c2077_split_2j55;
        else if ( trigger == "HLT_2j45_bmv2c2070_split_2j45" )     m_trigPrescales["HLT_2j45_bmv2c2070_split_2j45"] =     &Props::prescaleHLT_2j45_bmv2c2070_split_2j45;
        else if ( trigger == "L1_4J15" )     m_trigPrescales["L1_4J15"] =     &Props::prescaleL1_4J15;
        else if ( trigger == "HLT_2j35_bmv2c2070_split_2j35_L14J15" )     m_trigPrescales["HLT_2j35_bmv2c2070_split_2j35_L14J15"] =     &Props::prescaleHLT_2j35_bmv2c2070_split_2j35_L14J15;
        else if ( trigger == "HLT_2j35_bmv2c2060_split_2j35_L14J15" )     m_trigPrescales["HLT_2j35_bmv2c2060_split_2j35_L14J15"] =     &Props::prescaleHLT_2j35_bmv2c2060_split_2j35_L14J15;
        else if ( trigger == "HLT_2j35_bmv2c2050_split_2j35_L14J15" )     m_trigPrescales["HLT_2j35_bmv2c2050_split_2j35_L14J15"] =     &Props::prescaleHLT_2j35_bmv2c2050_split_2j35_L14J15;
        else if ( trigger == "HLT_j75_bmv2c2060_split_3j75_L14J15" )     m_trigPrescales["HLT_j75_bmv2c2060_split_3j75_L14J15"] =     &Props::prescaleHLT_j75_bmv2c2060_split_3j75_L14J15;
        else if ( trigger == "HLT_j75_bmv2c2070_split_3j75_L14J15" )     m_trigPrescales["HLT_j75_bmv2c2070_split_3j75_L14J15"] =     &Props::prescaleHLT_j75_bmv2c2070_split_3j75_L14J15;
        else if ( trigger == "HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700" )     m_trigPrescales["HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700"] =     &Props::prescaleHLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700;
        else if ( trigger == "HLT_j175_gsc225_bmv2c1040_split" )     m_trigPrescales["HLT_j175_gsc225_bmv2c1040_split"] =     &Props::prescaleHLT_j175_gsc225_bmv2c1040_split;
        else if ( trigger == "HLT_j175_bmv2c2040_split" )     m_trigPrescales["HLT_j175_bmv2c2040_split"] =     &Props::prescaleHLT_j175_bmv2c2040_split;
        else if ( trigger == "HLT_j225_bmv2c2060_split" )     m_trigPrescales["HLT_j225_bmv2c2060_split"] =     &Props::prescaleHLT_j225_bmv2c2060_split;
        else if ( trigger == "HLT_j225_gsc275_bmv2c1060_split" )     m_trigPrescales["HLT_j225_gsc275_bmv2c1060_split"] =     &Props::prescaleHLT_j225_gsc275_bmv2c1060_split;
        else if ( trigger == "HLT_j225_gsc275_bmv2c1070_split" )     m_trigPrescales["HLT_j225_gsc275_bmv2c1070_split"] =     &Props::prescaleHLT_j225_gsc275_bmv2c1070_split;
        else if ( trigger == "HLT_j225_gsc300_bmv2c1070_split" )     m_trigPrescales["HLT_j225_gsc300_bmv2c1070_split"] =     &Props::prescaleHLT_j225_gsc300_bmv2c1070_split;
        else if ( trigger == "HLT_j225_gsc300_bmv2c1077_split" )     m_trigPrescales["HLT_j225_gsc300_bmv2c1077_split"] =     &Props::prescaleHLT_j225_gsc300_bmv2c1077_split;
        else if ( trigger == "HLT_j420" )     m_trigPrescales["HLT_j420"] =     &Props::prescaleHLT_j420;
        // triggers for turn on study with gamma
        else if ( trigger == "HLT_g140_loose" )     m_trigPrescales["HLT_g140_loose"] =     &Props::prescaleHLT_g140_loose;
        //else if ( trigger == "HLT_g25_medium" )     m_trigPrescales["HLT_g25_medium"] =     &Props::prescaleHLT_g25_medium;
        else if ( trigger == "HLT_2j45_bmv2c2077_split_2j45_L14J15" )     m_trigPrescales["HLT_2j45_bmv2c2077_split_2j45_L14J15"] =     &Props::prescaleHLT_2j45_bmv2c2077_split_2j45_L14J15;
        else if ( trigger == "HLT_2j45_bmv2c2070_split_2j45_L14J15" )     m_trigPrescales["HLT_2j45_bmv2c2070_split_2j45_L14J15"] =     &Props::prescaleHLT_2j45_bmv2c2070_split_2j45_L14J15;
        else if ( trigger == "HLT_2j45_bmv2c2070_split_2j45_L14J15" )     m_trigPrescales["HLT_2j45_bmv2c2070_split_2j45_L14J15"] =     &Props::prescaleHLT_2j45_bmv2c2070_split_2j45_L14J15;
        else if ( trigger == "HLT_4j45" )     m_trigPrescales["HLT_4j45"] =     &Props::prescaleHLT_4j45;
        else {
          Error("EventInfoHandler_VBFHbb::initialize()","No prescale for trigger %s defined! Need to create one.", trigger.c_str());
          return EL::StatusCode::FAILURE;
        }
      }    
    }
  }
  TFile f("$WorkDir_DIR/data/CxAODMaker_VBFHbb/hist_muon.root");
  m_muonEff = (TH2D*)(gDirectory->Get("LooseMuons"));
  m_muonTrigEff = (TH2D*)(gDirectory->Get("TriggerRZs"));
  m_muonEff->SetDirectory(0);
  m_muonTrigEff->SetDirectory(0);
  TFile f_cvmfs("$ATLAS_DB_AREA/GroupData/MuonEfficiencyCorrections/190203_Winter_r21/Reco_Loose_Z.root");
  m_cvmfs_muonEff2016 = (TH2F*)(gDirectory->Get(m_isMC?"MC_Eff_2016":"Eff_2016"));
  m_cvmfs_muonEff2017 = (TH2F*)(gDirectory->Get(m_isMC?"MC_Eff_2017":"Eff_2017"));
  m_cvmfs_muonEff2018 = (TH2F*)(gDirectory->Get(m_isMC?"MC_Eff_2018":"Eff_2018"));
  m_cvmfs_muonEff2016->SetDirectory(0);
  m_cvmfs_muonEff2017->SetDirectory(0);
  m_cvmfs_muonEff2018->SetDirectory(0);
  TFile f_trig_cvmfs2016("$ATLAS_DB_AREA/GroupData/MuonEfficiencyCorrections/190203_Winter_r21/muontrigger_sf_2016_mc16a_v05.root");
  m_cvmfs_mu14Eff2016A_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodA"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu14Eff2016B_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodB"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu14Eff2016C_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodC"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu14Eff2016D1D3_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodD1D3"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu14Eff2016D4D8_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodD4D8"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu14Eff2016E_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodE"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu14Eff2016F_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodF"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu14Eff2016G_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodG"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu14Eff2016I_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodI"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu14Eff2016K_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodK"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu14Eff2016L_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodL"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu14Eff2016A_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodA"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu14Eff2016B_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodB"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu14Eff2016C_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodC"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu14Eff2016D1D3_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodD1D3"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu14Eff2016D4D8_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodD4D8"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu14Eff2016E_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodE"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu14Eff2016F_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodF"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu14Eff2016G_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodG"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu14Eff2016I_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodI"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu14Eff2016K_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodK"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu14Eff2016L_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodL"))->Get("HLT_mu14"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu14Eff2016A_barrel->SetDirectory(0);
  m_cvmfs_mu14Eff2016B_barrel->SetDirectory(0);
  m_cvmfs_mu14Eff2016C_barrel->SetDirectory(0);
  m_cvmfs_mu14Eff2016D1D3_barrel->SetDirectory(0);
  m_cvmfs_mu14Eff2016D4D8_barrel->SetDirectory(0);
  m_cvmfs_mu14Eff2016E_barrel->SetDirectory(0);
  m_cvmfs_mu14Eff2016F_barrel->SetDirectory(0);
  m_cvmfs_mu14Eff2016G_barrel->SetDirectory(0);
  m_cvmfs_mu14Eff2016I_barrel->SetDirectory(0);
  m_cvmfs_mu14Eff2016K_barrel->SetDirectory(0);
  m_cvmfs_mu14Eff2016L_barrel->SetDirectory(0);
  m_cvmfs_mu14Eff2016A_endcap->SetDirectory(0);
  m_cvmfs_mu14Eff2016B_endcap->SetDirectory(0);
  m_cvmfs_mu14Eff2016C_endcap->SetDirectory(0);
  m_cvmfs_mu14Eff2016D1D3_endcap->SetDirectory(0);
  m_cvmfs_mu14Eff2016D4D8_endcap->SetDirectory(0);
  m_cvmfs_mu14Eff2016E_endcap->SetDirectory(0);
  m_cvmfs_mu14Eff2016F_endcap->SetDirectory(0);
  m_cvmfs_mu14Eff2016G_endcap->SetDirectory(0);
  m_cvmfs_mu14Eff2016I_endcap->SetDirectory(0);
  m_cvmfs_mu14Eff2016K_endcap->SetDirectory(0);
  m_cvmfs_mu14Eff2016L_endcap->SetDirectory(0);

  m_cvmfs_mu50Eff2016A_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodA"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu50Eff2016B_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodB"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu50Eff2016C_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodC"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu50Eff2016D1D3_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodD1D3"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu50Eff2016D4D8_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodD4D8"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu50Eff2016E_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodE"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu50Eff2016F_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodF"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu50Eff2016G_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodG"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu50Eff2016I_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodI"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu50Eff2016K_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodK"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu50Eff2016L_barrel = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodL"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_barrel_mc_nominal":"eff_etaphi_fine_barrel_data_nominal"));
  m_cvmfs_mu50Eff2016A_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodA"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu50Eff2016B_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodB"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu50Eff2016C_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodC"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu50Eff2016D1D3_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodD1D3"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu50Eff2016D4D8_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodD4D8"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu50Eff2016E_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodE"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu50Eff2016F_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodF"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu50Eff2016G_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodG"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu50Eff2016I_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodI"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu50Eff2016K_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodK"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu50Eff2016L_endcap = (TH2F*)(((TDirectoryFile*)((TDirectoryFile*)((TDirectoryFile*)gDirectory->Get("Loose"))->Get("PeriodL"))->Get("HLT_mu50"))->Get(m_isMC?"eff_etaphi_fine_endcap_mc_nominal":"eff_etaphi_fine_endcap_data_nominal"));
  m_cvmfs_mu50Eff2016A_barrel->SetDirectory(0);
  m_cvmfs_mu50Eff2016B_barrel->SetDirectory(0);
  m_cvmfs_mu50Eff2016C_barrel->SetDirectory(0);
  m_cvmfs_mu50Eff2016D1D3_barrel->SetDirectory(0);
  m_cvmfs_mu50Eff2016D4D8_barrel->SetDirectory(0);
  m_cvmfs_mu50Eff2016E_barrel->SetDirectory(0);
  m_cvmfs_mu50Eff2016F_barrel->SetDirectory(0);
  m_cvmfs_mu50Eff2016G_barrel->SetDirectory(0);
  m_cvmfs_mu50Eff2016I_barrel->SetDirectory(0);
  m_cvmfs_mu50Eff2016K_barrel->SetDirectory(0);
  m_cvmfs_mu50Eff2016L_barrel->SetDirectory(0);
  m_cvmfs_mu50Eff2016A_endcap->SetDirectory(0);
  m_cvmfs_mu50Eff2016B_endcap->SetDirectory(0);
  m_cvmfs_mu50Eff2016C_endcap->SetDirectory(0);
  m_cvmfs_mu50Eff2016D1D3_endcap->SetDirectory(0);
  m_cvmfs_mu50Eff2016D4D8_endcap->SetDirectory(0);
  m_cvmfs_mu50Eff2016E_endcap->SetDirectory(0);
  m_cvmfs_mu50Eff2016F_endcap->SetDirectory(0);
  m_cvmfs_mu50Eff2016G_endcap->SetDirectory(0);
  m_cvmfs_mu50Eff2016I_endcap->SetDirectory(0);
  m_cvmfs_mu50Eff2016K_endcap->SetDirectory(0);
  m_cvmfs_mu50Eff2016L_endcap->SetDirectory(0);

  TFile f_trig_cvmfs2017("$ATLAS_DB_AREA/GroupData/MuonEfficiencyCorrections/190203_Winter_r21/muontrigger_sf_2017_mc16d_v03.root");
  TFile f_trig_cvmfs2018("$ATLAS_DB_AREA/GroupData/MuonEfficiencyCorrections/190203_Winter_r21/muontrigger_sf_2018_mc16e_v02.root");

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode EventInfoHandler_VBFHbb::executeEvent()
{
  EL::StatusCode sc = EventInfoHandler::executeEvent();
  if ( sc != EL::StatusCode::SUCCESS ) {
    Error("EventInfoHandler_VBFHbb::executeEvent()", "Failed to call parent class' executeEvent().");
    return sc;
  }

  for (auto& ps : m_trigPrescales) {
    float triggerPrescale = static_cast<float>(m_trigDecTool->getPrescale(ps.first));

    ps.second->set(m_eventInfoIn, triggerPrescale);
    if(m_debug) Info("EventInfoHandler_VBFHbb::executeEvent()","Prescale for trigger %s: %f",ps.first.c_str(),triggerPrescale); 
  }

  // calculate npv
  // primary vertex
  const xAOD::Vertex * primVtx = 0;
  int nVtx2Trks = 0;
  // get the vertex container
  const xAOD::VertexContainer * primVtxCont = 0;
  if ( ! m_event->retrieve(primVtxCont, "PrimaryVertices").isSuccess() ) {
    Error("EventInfoHandler::executeEvent()", "Failed to retrieve primary vertices collection. Exiting.");
    return EL::StatusCode::FAILURE;
  } 

  // find the primary vertex and count the number of vertices with at least 2 tracks
  int indexPV = 0;
  for (const xAOD::Vertex * vtx : *primVtxCont) {
    //ask for type - no need to ask for at least two associated tracks anymore
    if ( vtx->vertexType() == xAOD::VxType::PriVtx ) {
      primVtx = vtx;
      m_indexPV = indexPV;
    }
    if ( vtx->nTrackParticles() >= 2 ) ++nVtx2Trks;
    indexPV++;
  }
  Props::npv.set(m_eventInfoIn, indexPV);
  
  //##############################################################
  //Find out if trigger actually ran during that event
  //##############################################################

  for (auto &trig : m_trigIsActive){
    const unsigned int bits =  m_trigDecTool->isPassedBits(trig.first); // defined here http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Trigger/TrigAnalysis/TrigDecisionTool/TrigDecisionTool/DecisionAccess.h
    //typedef std::bitset<sizeof(const unsigned int)> UIntBits;
    typedef std::bitset<32> UIntBits;
    //std::cout << trig.first << " UIntBits " << UIntBits(bits) << std::endl;
    bool isSuppressedByPrescale   = UIntBits(bits).test(2);  // EF_prescaled             is at position 2,  see http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Trigger/TrigAnalysis/TrigDecisionTool/TrigDecisionTool/Conditions.h
    bool L1isPassedAfterPrescale  = UIntBits(bits).test(16); // L1isPassedAfterPrescale  is at position 16
    bool L1isPassedBeforePrescale = UIntBits(bits).test(17); // L1isPassedBeforePrescale is at position 17
    if (trig.first.find("HLT") == 0){ //if it is an HLT trigger
      trig.second->set(m_eventInfoIn, (int)( L1isPassedAfterPrescale && !isSuppressedByPrescale ));
    }
    else {
      trig.second->set(m_eventInfoIn, (int) L1isPassedBeforePrescale );
    }
  }

  //############################################################################################
  //trigger emulation for our triggers that are not included in MC, or we need a turn on in data
  //############################################################################################

  std::unordered_map<std::string /* trigger name */, bool /* emulation decision */> triggersToEmulate;
  std::unordered_map<std::string /* trigger name */, bool /* emulation decision */>::iterator it_triggersToEmulate;

  Props::hpt.set(m_eventInfoIn,-1);
  Props::heta.set(m_eventInfoIn,-1);
  Props::hphi.set(m_eventInfoIn,-1);
  Props::hm.set(m_eventInfoIn,-1);
  if (m_isMC) {
    const xAOD::TruthEventContainer * eventCont = 0;
    if (!m_event->retrieve(eventCont,"TruthEvents").isSuccess()) {
      Error("execute()", "Failed to retrieve TruthEvents");
      return EL::StatusCode::FAILURE;
    }
    for (size_t i = 0; i < eventCont->at(0)->nTruthParticles(); i++) {
      const xAOD::TruthParticle *part = static_cast<const xAOD::TruthParticle*>(eventCont->at(0)->truthParticle(i));
      if (!part) continue;
      if (part->pdgId()!=25) continue;
      if (part->status()!=62) continue;
      Props::hpt.set(m_eventInfoIn,part->pt());
      Props::heta.set(m_eventInfoIn,part->eta());
      Props::hphi.set(m_eventInfoIn,part->phi());
      Props::hm.set(m_eventInfoIn,part->m());
    }
  }

  if (m_isEmbedding) {
    const xAOD::MuonContainer * embmuCont = 0;
    if (!m_event->retrieve(embmuCont,"MuonsEmb").isSuccess()) {
      Error("execute()", "Failed to retrieve MuonsEmb");
      return EL::StatusCode::FAILURE;
    }
    UInt_t runNumber = (m_isMC?m_eventInfoIn->auxdata<UInt_t>("RandomRunNumber"):m_eventInfoIn->runNumber());
    float muonEff = 1;
    float muonTrigEff = 1;
    float muonEff_cvmfs = 1;
    float muonTrigEff_cvmfs = 1;
    float muonTrigEff_cvmfs_14 = 1;
    float muonTrigEff_cvmfs_50 = 1;
    TLorentzVector tlvz;
    for (const auto& muon : *embmuCont) {
      if (m_muonEff->GetBinContent(m_muonEff->FindBin(muon->pt(),fabs(muon->eta())))>0) muonEff *= m_muonEff->GetBinContent(m_muonEff->FindBin(muon->pt(),fabs(muon->eta())));
      TH2F *m_cvmfs_muonEff = NULL;
      if (runNumber<=311481) m_cvmfs_muonEff = m_cvmfs_muonEff2016;
      else if (runNumber<=340453) m_cvmfs_muonEff = m_cvmfs_muonEff2017;
      else m_cvmfs_muonEff = m_cvmfs_muonEff2018;
      if (m_cvmfs_muonEff->GetBinContent(m_cvmfs_muonEff->FindBin(muon->eta(),muon->phi()))>0) muonEff_cvmfs *= m_cvmfs_muonEff->GetBinContent(m_cvmfs_muonEff->FindBin(muon->eta(),muon->phi()));
      tlvz += muon->p4();

    }
    const xAOD::Muon *muon1 = embmuCont->at(0);
    const xAOD::Muon *muon2 = embmuCont->at(1);
    TH2F *m_cvmfs_mu14Eff1 = NULL;
    TH2F *m_cvmfs_mu50Eff1 = NULL;
    if (runNumber<=300287 && fabs(muon1->eta())<1.05) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016A_barrel;
    else if (runNumber<=300287) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016A_endcap;
    else if (runNumber<=300908 && fabs(muon1->eta())<1.05) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016B_barrel;
    else if (runNumber<=300908) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016B_endcap;
    else if (runNumber<=302393 && fabs(muon1->eta())<1.05) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016C_barrel;
    else if (runNumber<=302393) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016C_endcap;
    else if (runNumber<=302872 && fabs(muon1->eta())<1.05) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016D1D3_barrel;
    else if (runNumber<=302872) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016D1D3_endcap;
    else if (runNumber<=303560 && fabs(muon1->eta())<1.05) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016D4D8_barrel;
    else if (runNumber<=303560) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016D4D8_endcap;
    else if (runNumber<=303892 && fabs(muon1->eta())<1.05) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016E_barrel;
    else if (runNumber<=303892) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016E_endcap;
    else if (runNumber<=304494 && fabs(muon1->eta())<1.05) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016F_barrel;
    else if (runNumber<=304494) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016F_endcap;
    else if (runNumber<=306714 && fabs(muon1->eta())<1.05) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016G_barrel;
    else if (runNumber<=306714) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016G_endcap;
    else if (runNumber<=308084 && fabs(muon1->eta())<1.05) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016I_barrel;
    else if (runNumber<=308084) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016I_endcap;
    else if (runNumber<=309759 && fabs(muon1->eta())<1.05) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016K_barrel;
    else if (runNumber<=309759) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016K_endcap;
    else if (runNumber<=311481 && fabs(muon1->eta())<1.05) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016L_barrel;
    else m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016L_endcap;
    //else if (runNumber<=311481) m_cvmfs_mu14Eff1 = m_cvmfs_mu14Eff2016L_endcap;
    if (runNumber<=300287 && fabs(muon1->eta())<1.05) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016A_barrel;
    else if (runNumber<=300287) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016A_endcap;
    else if (runNumber<=300908 && fabs(muon1->eta())<1.05) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016B_barrel;
    else if (runNumber<=300908) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016B_endcap;
    else if (runNumber<=302393 && fabs(muon1->eta())<1.05) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016C_barrel;
    else if (runNumber<=302393) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016C_endcap;
    else if (runNumber<=302872 && fabs(muon1->eta())<1.05) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016D1D3_barrel;
    else if (runNumber<=302872) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016D1D3_endcap;
    else if (runNumber<=303560 && fabs(muon1->eta())<1.05) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016D4D8_barrel;
    else if (runNumber<=303560) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016D4D8_endcap;
    else if (runNumber<=303892 && fabs(muon1->eta())<1.05) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016E_barrel;
    else if (runNumber<=303892) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016E_endcap;
    else if (runNumber<=304494 && fabs(muon1->eta())<1.05) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016F_barrel;
    else if (runNumber<=304494) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016F_endcap;
    else if (runNumber<=306714 && fabs(muon1->eta())<1.05) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016G_barrel;
    else if (runNumber<=306714) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016G_endcap;
    else if (runNumber<=308084 && fabs(muon1->eta())<1.05) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016I_barrel;
    else if (runNumber<=308084) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016I_endcap;
    else if (runNumber<=309759 && fabs(muon1->eta())<1.05) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016K_barrel;
    else if (runNumber<=309759) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016K_endcap;
    else if (runNumber<=311481 && fabs(muon1->eta())<1.05) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016L_barrel;
    //else if (runNumber<=311481) m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016L_endcap;
    else m_cvmfs_mu50Eff1 = m_cvmfs_mu50Eff2016L_endcap;

    TH2F *m_cvmfs_mu14Eff2 = NULL;
    TH2F *m_cvmfs_mu50Eff2 = NULL;
    if (runNumber<=300287 && fabs(muon2->eta())<1.05) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016A_barrel;
    else if (runNumber<=300287) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016A_endcap;
    else if (runNumber<=300908 && fabs(muon2->eta())<1.05) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016B_barrel;
    else if (runNumber<=300908) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016B_endcap;
    else if (runNumber<=302393 && fabs(muon2->eta())<1.05) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016C_barrel;
    else if (runNumber<=302393) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016C_endcap;
    else if (runNumber<=302872 && fabs(muon2->eta())<1.05) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016D1D3_barrel;
    else if (runNumber<=302872) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016D1D3_endcap;
    else if (runNumber<=303560 && fabs(muon2->eta())<1.05) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016D4D8_barrel;
    else if (runNumber<=303560) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016D4D8_endcap;
    else if (runNumber<=303892 && fabs(muon2->eta())<1.05) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016E_barrel;
    else if (runNumber<=303892) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016E_endcap;
    else if (runNumber<=304494 && fabs(muon2->eta())<1.05) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016F_barrel;
    else if (runNumber<=304494) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016F_endcap;
    else if (runNumber<=306714 && fabs(muon2->eta())<1.05) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016G_barrel;
    else if (runNumber<=306714) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016G_endcap;
    else if (runNumber<=308084 && fabs(muon2->eta())<1.05) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016I_barrel;
    else if (runNumber<=308084) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016I_endcap;
    else if (runNumber<=309759 && fabs(muon2->eta())<1.05) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016K_barrel;
    else if (runNumber<=309759) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016K_endcap;
    else if (runNumber<=311481 && fabs(muon2->eta())<1.05) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016L_barrel;
    //else if (runNumber<=311481) m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016L_endcap;
    else m_cvmfs_mu14Eff2 = m_cvmfs_mu14Eff2016L_endcap;
    if (runNumber<=300287 && fabs(muon2->eta())<1.05) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016A_barrel;
    else if (runNumber<=300287) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016A_endcap;
    else if (runNumber<=300908 && fabs(muon2->eta())<1.05) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016B_barrel;
    else if (runNumber<=300908) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016B_endcap;
    else if (runNumber<=302393 && fabs(muon2->eta())<1.05) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016C_barrel;
    else if (runNumber<=302393) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016C_endcap;
    else if (runNumber<=302872 && fabs(muon2->eta())<1.05) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016D1D3_barrel;
    else if (runNumber<=302872) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016D1D3_endcap;
    else if (runNumber<=303560 && fabs(muon2->eta())<1.05) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016D4D8_barrel;
    else if (runNumber<=303560) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016D4D8_endcap;
    else if (runNumber<=303892 && fabs(muon2->eta())<1.05) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016E_barrel;
    else if (runNumber<=303892) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016E_endcap;
    else if (runNumber<=304494 && fabs(muon2->eta())<1.05) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016F_barrel;
    else if (runNumber<=304494) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016F_endcap;
    else if (runNumber<=306714 && fabs(muon2->eta())<1.05) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016G_barrel;
    else if (runNumber<=306714) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016G_endcap;
    else if (runNumber<=308084 && fabs(muon2->eta())<1.05) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016I_barrel;
    else if (runNumber<=308084) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016I_endcap;
    else if (runNumber<=309759 && fabs(muon2->eta())<1.05) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016K_barrel;
    else if (runNumber<=309759) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016K_endcap;
    else if (runNumber<=311481 && fabs(muon2->eta())<1.05) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016L_barrel;
    //else if (runNumber<=311481) m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016L_endcap;
    else m_cvmfs_mu50Eff2 = m_cvmfs_mu50Eff2016L_endcap;

    float muon1eff14 = m_cvmfs_mu14Eff1->GetBinContent(m_cvmfs_mu14Eff1->FindBin(muon1->eta(),muon1->phi()));
    float muon1eff50 = m_cvmfs_mu50Eff1->GetBinContent(m_cvmfs_mu50Eff1->FindBin(muon1->eta(),muon1->phi()));
    float muon2eff14 = m_cvmfs_mu14Eff2->GetBinContent(m_cvmfs_mu14Eff2->FindBin(muon2->eta(),muon2->phi()));
    float muon2eff50 = m_cvmfs_mu50Eff2->GetBinContent(m_cvmfs_mu50Eff2->FindBin(muon2->eta(),muon2->phi()));
    if (muon1eff14<=0) muon1eff14 = 1;
    if (muon1eff50<=0) muon1eff50 = 1;
    if (muon2eff14<=0) muon2eff14 = 1;
    if (muon2eff50<=0) muon2eff50 = 1;

    if (muon1->pt()>51e3 && muon2->pt()>51e3) {
      muonTrigEff_cvmfs = muon1eff50+muon2eff50-muon1eff50*muon2eff50;
      muonTrigEff_cvmfs_14 = muon1eff14*muon2eff14;
      muonTrigEff_cvmfs_50 = muon1eff50+muon2eff50-muon1eff50*muon2eff50;
    }
    if (muon1->pt()>51e3 && muon2->pt()<51e3) {
      muonTrigEff_cvmfs = muon1eff50;
      muonTrigEff_cvmfs_14 = muon1eff14*muon2eff14;
      muonTrigEff_cvmfs_50 = muon1eff50;
    }
    if (muon1->pt()<51e3 && muon2->pt()>51e3) {
      muonTrigEff_cvmfs = muon2eff50;
      muonTrigEff_cvmfs_14 = muon1eff14*muon2eff14;
      muonTrigEff_cvmfs_50 = muon2eff50;
    }
    if (muon1->pt()<51e3 && muon2->pt()<51e3) {
      muonTrigEff_cvmfs =  muon1eff14*muon2eff14;
      muonTrigEff_cvmfs_14 = muon1eff14*muon2eff14;
      muonTrigEff_cvmfs_50 = 0;
    }

    if (m_muonTrigEff->GetBinContent(m_muonTrigEff->FindBin(tlvz.Pt(),fabs(tlvz.Eta())))>0) muonTrigEff = m_muonTrigEff->GetBinContent(m_muonTrigEff->FindBin(tlvz.Pt(),fabs(tlvz.Eta())));
    Props::muonEff.set(m_eventInfoIn, muonEff);
    Props::muonEff_cvmfs.set(m_eventInfoIn, muonEff_cvmfs);
    Props::muonTrigEff.set(m_eventInfoIn, muonTrigEff);
    Props::muonTrigEff_cvmfs_14.set(m_eventInfoIn, muonTrigEff_cvmfs_14);
    Props::muonTrigEff_cvmfs_50.set(m_eventInfoIn, muonTrigEff_cvmfs_50);
    Props::muonTrigEff_cvmfs.set(m_eventInfoIn, muonTrigEff_cvmfs);
    Props::embMu1pt.set(m_eventInfoIn,muon1->pt());
    Props::embMu1eta.set(m_eventInfoIn,muon1->eta());
    Props::embMu1phi.set(m_eventInfoIn,muon1->phi());
    Props::embMu1ptvarcone30.set(m_eventInfoIn,muon1->auxdata<float>("ptvarcone30"));
    //Props::embMu1ptvarcone30.set(m_eventInfoIn,muon1->auxdata<float>("ptvarcone30_TightTTVA_pt1000"));
    Props::embMu1topoetcone20.set(m_eventInfoIn,muon1->auxdata<float>("topoetcone20"));
    Props::embMu2pt.set(m_eventInfoIn,muon2->pt());
    Props::embMu2eta.set(m_eventInfoIn,muon2->eta());
    Props::embMu2phi.set(m_eventInfoIn,muon2->phi());
    Props::embMu2ptvarcone30.set(m_eventInfoIn,muon2->auxdata<float>("ptvarcone30"));
    //Props::embMu2ptvarcone30.set(m_eventInfoIn,muon2->auxdata<float>("ptvarcone30_TightTTVA_pt1000"));
    Props::embMu2topoetcone20.set(m_eventInfoIn,muon2->auxdata<float>("topoetcone20"));
    Props::embZpt.set(m_eventInfoIn,(muon1->p4()+muon2->p4()).Pt());
    Props::embZeta.set(m_eventInfoIn,(muon1->p4()+muon2->p4()).Eta());
    Props::embZphi.set(m_eventInfoIn,(muon1->p4()+muon2->p4()).Phi());
    Props::embZm.set(m_eventInfoIn,(muon1->p4()+muon2->p4()).M());


    return EL::StatusCode::SUCCESS;
  }

  //#####################
  //L1 trigger emulation
  const xAOD::JetRoIContainer* l1jets = 0;
  if ( ! m_event->retrieve(l1jets, "LVL1JetRoIs").isSuccess() ) {
    Error("EventInfoHandler_VBFHbb::executeEvent()", "TriggerEmulation: Failed to retrieve L1 Jet collection. Exiting.");
    return EL::StatusCode::FAILURE;
  } 

  //L1_J40.0ETA25_2J25_J20.31ETA49
  int nJ40_0ETA25 = 0;
  int nJ25 = 0;
  int nJ20_31ETA49 = 0;
  //L1_J40.0ETA25_2J15.31ETA49
  int nJ15_31ETA49 = 0;
  //L1_4J15
  int nJ15 = 0;

  float l1ht = 0;
  float l1mjj = 0;
  std::vector<float> l1s;
  std::vector<TLorentzVector> l1s20;
  std::vector<TLorentzVector> l1s30;
  xAOD::JetRoIContainer::const_iterator l1sg, l1sgEnd = l1jets->end();
  for(l1sg = l1jets->begin(); l1sg!=l1sgEnd; l1sg++) {
    int nsame = 0;
    xAOD::JetRoIContainer::const_iterator l1, l1End = l1jets->end();
    TLorentzVector tlv1;
    tlv1.SetPtEtaPhiM((*l1sg)->et8x8(),(*l1sg)->eta(),(*l1sg)->phi(),0);
    for(l1 = l1jets->begin(); l1!=l1End; l1++) {
      if ((*l1sg)->et8x8() == (*l1)->et8x8() &&
          (*l1sg)->eta() == (*l1)->eta() &&
          (*l1sg)->phi() == (*l1)->phi()) {
	nsame ++;
      }
    }
    if (fabs((*l1sg)->eta())<3.1 && (*l1sg)->et8x8() > 20e3) l1s.push_back((*l1sg)->et8x8());
    if ((*l1sg)->et8x8() > 20e3) l1s20.push_back(tlv1);
    if ((*l1sg)->et8x8() > 30e3) l1s30.push_back(tlv1);

    if (nsame<=1){
      if ((*l1sg)->et8x8() > 40e3 && fabs((*l1sg)->eta()) < 2.5) nJ40_0ETA25++;
      if ((*l1sg)->et8x8() > 25e3 && fabs((*l1sg)->eta()) < 3.1) nJ25++;
      if ((*l1sg)->et8x8() > 20e3 && 3.1 < fabs((*l1sg)->eta()) && fabs((*l1sg)->eta()) < 4.9) nJ20_31ETA49++;
      //L1_J40.0ETA25_2J15.31ETA49
      if ((*l1sg)->et8x8() > 15e3 && 3.1 < fabs((*l1sg)->eta()) && fabs((*l1sg)->eta()) < 4.9) nJ15_31ETA49++;
      //L1_4J15
      if ((*l1sg)->et8x8() > 15e3 && fabs((*l1sg)->eta()) < 3.1) nJ15++;
    }
  }
  std::sort(l1s.begin(), l1s.end(), std::greater<float>());
  std::sort(l1s20.begin(), l1s20.end(), sortf);
  std::sort(l1s30.begin(), l1s30.end(), sortf);
  for (size_t i = 0; i < l1s20.size(); i++) {
    if (i>5) continue;
    if (fabs(l1s20[i].Eta())<3.1) continue;
    for (size_t j = 0; j < l1s30.size(); j++) {
      if (j>5) continue;
      if (fabs(l1s30[j].Eta())>3.1) continue;
      if ((l1s20[i]+l1s30[j]).M()>l1mjj) l1mjj = (l1s20[i]+l1s30[j]).M();
    
    }
  }

  for (size_t i = 0; i <= 4; i++) if (l1s.size()>i) l1ht += l1s[i];

  //uniql1jetsaux->clear();
  //uniql1jets->clear();


  bool passEmuL1_J40_0ETA25_2J25             = nJ40_0ETA25 >= 1 && nJ25 >= 2;                     
  bool passEmuL1_J20_31ETA49                 = nJ20_31ETA49 >= 1;                                 
  bool passEmuL1_J15_31ETA49                 = nJ15_31ETA49 >= 1;                                 
  bool passEmuL1_J40_0ETA25_2J25_J20_31ETA49 = passEmuL1_J40_0ETA25_2J25 && passEmuL1_J20_31ETA49;

  bool passEmuL1_J40_0ETA25                  = nJ40_0ETA25 >= 1;                                  
  bool passEmuL1_2J15_31ETA49                = nJ15_31ETA49 >= 2;                                 
  bool passEmuL1_J40_0ETA25_2J15_31ETA49     = passEmuL1_J40_0ETA25 && passEmuL1_2J15_31ETA49;    

  bool passEmuL1_4J15                        = nJ15 >= 4;                                         

  triggersToEmulate.insert ( std::make_pair ( "L1_J40.0ETA25_2J25"             , passEmuL1_J40_0ETA25_2J25             )); 
  triggersToEmulate.insert ( std::make_pair ( "L1_J20.31ETA49"                 , passEmuL1_J20_31ETA49                 )); 
  triggersToEmulate.insert ( std::make_pair ( "L1_J15.31ETA49"                 , passEmuL1_J15_31ETA49                 )); 
  triggersToEmulate.insert ( std::make_pair ( "L1_J40.0ETA25_2J25_J20.31ETA49" , passEmuL1_J40_0ETA25_2J25_J20_31ETA49 )); 

  triggersToEmulate.insert ( std::make_pair ( "L1_J40.0ETA25"                  , passEmuL1_J40_0ETA25                  )); 
  triggersToEmulate.insert ( std::make_pair ( "L1_2J15.31ETA49"                , passEmuL1_2J15_31ETA49                )); 
  triggersToEmulate.insert ( std::make_pair ( "L1_J40.0ETA25_2J15.31ETA49"     , passEmuL1_J40_0ETA25_2J15_31ETA49     )); 

  triggersToEmulate.insert ( std::make_pair ( "L1_4J15"                        , passEmuL1_4J15                        )); 




  //#####################
  //HLT trigger emulation
  const xAOD::JetContainer* hltjets = 0;
  if ( ! m_event->retrieve(hltjets, "HLT_xAOD__JetContainer_a4tcemsubjesISFS").isSuccess() ) {
    Error("EventInfoHandler_VBFHbb::executeEvent()", "TriggerEmulation: Failed to retrieve HLT Jet collection. Exiting.");
    return EL::StatusCode::FAILURE;
  } 

  //HLT_j80_0eta240_j60_j45_320eta490
  int nj80_0eta240 = 0;
  int nj60 = 0;
  int nj45_320eta490 = 0;
  //HLT_j80_0eta240_2j60_320eta490
  int nj60_320eta490 = 0;
  //HLT_4j45
  int nj45 = 0;

  float hltht = 0;
  float hltmjj = 0;
  std::vector<float> hts;
  for (auto jet : *hltjets){
    for (auto jet2 : *hltjets){
      if ((jet->p4()+jet2->p4()).M()>hltmjj) hltmjj = (jet->p4()+jet2->p4()).M();
    }
    if (jet->p4().Et() > 30e3 && fabs(jet->eta())<3.2) hts.push_back(jet->pt());
    if (jet->p4().Et() > 30e3 && fabs(jet->eta())<3.2) hltht += jet->p4().Et();
    //HLT_j80_0eta240_j60_j45_320eta490
    if (jet->p4().Et() > 80e3 && fabs(jet->eta()) < 2.4) nj80_0eta240++;
    if (jet->p4().Et() > 60e3 && fabs(jet->eta()) < 3.2) nj60++;
    if (jet->p4().Et() > 45e3 && 3.2 < fabs(jet->eta()) && fabs(jet->eta()) < 4.9) nj45_320eta490++;
    //HLT_j80_0eta240_2j60_320eta490
    if (jet->p4().Et() > 60e3 && 3.2 < fabs(jet->eta()) && fabs(jet->eta()) < 4.9) nj60_320eta490++;
    //HLT_4j45
    if (jet->p4().Et() > 45e3 && fabs(jet->eta()) < 3.2) nj45++;
  }
  std::sort(hts.begin(), hts.end(), std::greater<float>());
  Props::hltht.set(m_eventInfoIn, hltht);
  Props::hltmjj.set(m_eventInfoIn, hltmjj);
  Props::l1ht.set(m_eventInfoIn, l1ht);
  Props::l1mjj.set(m_eventInfoIn, l1mjj);

  bool passEmuHLT_j80_0eta240_j60               = nj80_0eta240 >= 1 && nj60 >= 2;                         
  bool passEmuHLT_j45_320eta490                 = nj45_320eta490 >= 1;                                    
  bool passEmuHLT_j80_0eta240_j60_j45_320eta490 = passEmuHLT_j80_0eta240_j60 && passEmuHLT_j45_320eta490; 

  bool passEmuHLT_j80_0eta240                   = nj80_0eta240 >= 1;                                      
  bool passEmuHLT_2j60_320eta490                = nj60_320eta490 >= 2;                                    
  bool passEmuHLT_j80_0eta240_2j60_320eta490    = passEmuHLT_j80_0eta240 && passEmuHLT_2j60_320eta490;    

  bool passEmuHLT_4j45 = nj45 >= 4;

  triggersToEmulate.insert ( std::make_pair ( "HLT_j80_0eta240_j60"               , passEmuHLT_j80_0eta240_j60               ));
  triggersToEmulate.insert ( std::make_pair ( "HLT_j45_320eta490"                 , passEmuHLT_j45_320eta490                 ));
  triggersToEmulate.insert ( std::make_pair ( "HLT_j80_0eta240_j60_j45_320eta490" , passEmuHLT_j80_0eta240_j60_j45_320eta490 ));

  triggersToEmulate.insert ( std::make_pair ( "HLT_j80_0eta240"                   , passEmuHLT_j80_0eta240                   ));
  triggersToEmulate.insert ( std::make_pair ( "HLT_2j60_320eta490"                , passEmuHLT_2j60_320eta490                ));
  triggersToEmulate.insert ( std::make_pair ( "HLT_j80_0eta240_2j60_320eta490"    , passEmuHLT_j80_0eta240_2j60_320eta490    ));

  triggersToEmulate.insert ( std::make_pair ( "HLT_4j45"                          , passEmuHLT_4j45                          ));


  //#####################
  //assign actual values to the m_trigDecorators
  for (it_triggersToEmulate = triggersToEmulate.begin(); it_triggersToEmulate != triggersToEmulate.end(); it_triggersToEmulate++){
    std::string trigName = it_triggersToEmulate->first;                            
    bool passTDT         = m_trigDecTool->isPassed( it_triggersToEmulate->first ); 
    bool passEmu         = it_triggersToEmulate->second;                           
    if ( !passTDT && !passEmu ) m_trigDecorators [ trigName ] ->set ( m_eventInfoIn, 0 ) ;
    if ( passTDT              ) m_trigDecorators [ trigName ] ->set ( m_eventInfoIn, 1 ) ;
    if ( passEmu              ) m_trigDecorators [ trigName ] ->set ( m_eventInfoIn, 2 ) ;
    if ( passTDT && passEmu   ) m_trigDecorators [ trigName ] ->set ( m_eventInfoIn, 3 ) ;
    if(m_debug) Info("EventInfoHandler_VBFHbb::executeEvent()","Emulation decision for trigger %s: %i",trigName.c_str(),passEmu); 
  }


  // Trigger Emulation Tool
  if (m_doTrigEmul && m_triggerEmulation->execute().isFailure() )
    {
      Error("EventInfoHandler_VBFHbb::executeEvent()", "TrigBtagEmulationTool: Failed to execute trigger emulation..");
      return EL::StatusCode::FAILURE;
    }

  triggersToEmulate.clear();

  // Trigger Emulation Tool

  std::vector<std::string> toBeEmulated_L1_triggers;
  std::vector<std::string> toBeEmulated_HLT_triggers;
  std::vector<std::string> toBeEmulated_full_triggers;
  if (m_doTrigEmul) {
    toBeEmulated_L1_triggers.push_back("L1_4J15");
    toBeEmulated_L1_triggers.push_back("L1_2J15.31ETA49");
    toBeEmulated_L1_triggers.push_back("L1_J40.0ETA25");
    toBeEmulated_L1_triggers.push_back("L1_J20.31ETA49");
    toBeEmulated_L1_triggers.push_back("L1_J15.31ETA49");
    toBeEmulated_L1_triggers.push_back("L1_2J15");
    toBeEmulated_L1_triggers.push_back("L1_2J25");

    toBeEmulated_HLT_triggers.push_back("HLT_4j35");
    toBeEmulated_HLT_triggers.push_back("HLT_4j45");
    toBeEmulated_HLT_triggers.push_back("HLT_2j35_bmv2c2060_split");
    toBeEmulated_HLT_triggers.push_back("HLT_2j45_bmv2c2070_split");
    toBeEmulated_HLT_triggers.push_back("HLT_j80_bmv2c2070_split" );
    toBeEmulated_HLT_triggers.push_back("HLT_2j60_bmv2c2085_split");
    toBeEmulated_HLT_triggers.push_back("HLT_j80_bmv2c2085_split");
    toBeEmulated_HLT_triggers.push_back("HLT_j45_320eta490");
    toBeEmulated_HLT_triggers.push_back("HLT_2j60_320eta490");
    // Supporting HLT triggers
    toBeEmulated_HLT_triggers.push_back("HLT_j80_0eta240");
    toBeEmulated_HLT_triggers.push_back("HLT_2j60");

    toBeEmulated_full_triggers.push_back("HLT_2j35_bmv2c2060_split_2j35_L14J15");
    toBeEmulated_full_triggers.push_back("HLT_2j45_bmv2c2070_split_2j45_L14J15");
    toBeEmulated_full_triggers.push_back("HLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490_L1_J40.0ETA25_2J25_J20.31ETA49");
    toBeEmulated_full_triggers.push_back("HLT_j80_bmv2c2085_split_2j60_320eta490_L1_J40.0ETA25_2J15.31ETA49");
  }


  for (unsigned int index(0); index < toBeEmulated_L1_triggers.size(); index++)
    {
      std::string name = toBeEmulated_L1_triggers.at(index);
      bool triggerDecision = m_triggerEmulation->isPassed(name.c_str());
      triggersToEmulate.insert( std::make_pair ( name.c_str(),triggerDecision ) );
    }

  for (unsigned int index(0); index < toBeEmulated_HLT_triggers.size(); index++)
    {
      std::string name = toBeEmulated_HLT_triggers.at(index);
      bool triggerDecision = m_triggerEmulation->isPassed(name.c_str());
      triggersToEmulate.insert( std::make_pair ( name.c_str(),triggerDecision ) );
    }

  for (unsigned int index(0); index < toBeEmulated_full_triggers.size(); index++)
    {
      std::string name = toBeEmulated_full_triggers.at(index);
      bool triggerDecision = m_triggerEmulation->isPassed(name.c_str());
      triggersToEmulate.insert( std::make_pair ( name.c_str(),triggerDecision ) );
    }



  // Assign actual values to the m_trigDecorators
  for (it_triggersToEmulate = triggersToEmulate.begin(); it_triggersToEmulate != triggersToEmulate.end(); it_triggersToEmulate++){
    std::string trigName = it_triggersToEmulate->first;
    bool passTDT         = m_trigDecTool->isPassed( it_triggersToEmulate->first );
    bool passEmu         = it_triggersToEmulate->second; 
    if ( !passTDT && !passEmu ) m_trigDecorators [ Form("EmulTool_%s",trigName.c_str()) ] ->set ( m_eventInfoIn, 0 ) ;
    if ( passTDT              ) m_trigDecorators [ Form("EmulTool_%s",trigName.c_str()) ] ->set ( m_eventInfoIn, 1 ) ;
    if ( passEmu              ) m_trigDecorators [ Form("EmulTool_%s",trigName.c_str()) ] ->set ( m_eventInfoIn, 2 ) ;
    if ( passTDT && passEmu   ) m_trigDecorators [ Form("EmulTool_%s",trigName.c_str()) ] ->set ( m_eventInfoIn, 3 ) ;
    if(m_debug) Info("EventInfoHandler_VBFHbb::executeEvent()","Emulation decision for trigger %s: %i",trigName.c_str(),passEmu);
  }


  // Theory uncertainty of ggF or VBF
  if ( m_isMC and ( m_eventInfoIn->mcChannelNumber() == 445338 or m_eventInfoIn->mcChannelNumber() == 445342 ) ) {

    TLorentzVector h;
    int HTXS_Njets30=1;
    int HTXS_Stage1=0;
    int HTXS_index=0;
    double HTXS_pTH=0;
    // All input files used for test have the HTXS content                                                                                         
    if (m_eventInfoIn->isAvailable<int>("HTXS_Njets_pTjet30")) {
      HTXS_Njets30 = m_eventInfoIn->auxdata<int>("HTXS_Njets_pTjet30");
      HTXS_Stage1 = m_eventInfoIn->auxdata<int>("HTXS_Stage1_Category_pTjet30");
      HTXS_index  = m_eventInfoIn->auxdata<int>("HTXS_Stage1_FineIndex_pTjet30");
      HTXS_pTH  = m_eventInfoIn->auxdata<float>("HTXS_Higgs_pt");
      double HTXS_etaH = m_eventInfoIn->auxdata<float>("HTXS_Higgs_eta");
      double HTXS_phiH = m_eventInfoIn->auxdata<float>("HTXS_Higgs_phi");
      double HTXS_mH   = m_eventInfoIn->auxdata<float>("HTXS_Higgs_m");
      
      h.SetPtEtaPhiM(HTXS_pTH,HTXS_etaH,HTXS_phiH,HTXS_mH);
      h*=1e-3; // convert to GeV                                                                                                                   
    } else HTXS_pTH=h.Pt()*1000;
    
    TruthWeightTools::HiggsWeights hw;

    if (m_eventInfoIn->mcChannelNumber() == 445338){
      hw= m_higgsMCtool_VBF->getHiggsWeights(HTXS_Njets30,HTXS_pTH,HTXS_Stage1);
    }
    if (m_eventInfoIn->mcChannelNumber() == 445342){
      hw= m_higgsMCtool_ggF->getHiggsWeights(HTXS_Njets30,HTXS_pTH,HTXS_Stage1);
    }

    std::vector<float> pdf4lhc_unc;
    std::vector<float> pdfnnpdf30_unc;
    std::vector<float> qcd_uncert_nnlops;
    std::vector<float> qcd_uncert_nnlops_2np;
    std::vector<float> qcd_uncert_WG1;
    std::vector<float> alternative_pdf;

    for(int isys=0; isys<hw.pdf4lhc_unc.size(); ++isys){
      pdf4lhc_unc.push_back( hw.pdf4lhc_unc[isys]/hw.nominal);
    }
    for(int isys=0; isys<hw.nnpdf30_unc.size(); ++isys){
      pdfnnpdf30_unc.push_back( hw.nnpdf30_unc[isys]/hw.nominal);
    }
    
    for(int isys=0; isys<hw.qcd_nnlops.size(); ++isys){
      qcd_uncert_nnlops.push_back( hw.qcd_nnlops[isys]/hw.nominal);
    }

    qcd_uncert_WG1.push_back( hw.qcd_wg1_mu/hw.nominal);
    qcd_uncert_WG1.push_back( hw.qcd_wg1_res/hw.nominal);
    qcd_uncert_WG1.push_back( hw.qcd_wg1_mig01/hw.nominal);
    qcd_uncert_WG1.push_back( hw.qcd_wg1_mig12/hw.nominal);
    qcd_uncert_WG1.push_back( hw.qcd_wg1_pTH/hw.nominal);
    qcd_uncert_WG1.push_back( hw.qcd_wg1_qm_b/hw.nominal);
    qcd_uncert_WG1.push_back( hw.qcd_wg1_qm_t/hw.nominal);

    alternative_pdf.push_back(hw.nnpdf30_nnlo/hw.nominal);
    alternative_pdf.push_back(hw.pdf4lhc_nnlo/hw.nominal);
    alternative_pdf.push_back(hw.nnpdf30_nlo/hw.nominal);
    alternative_pdf.push_back(hw.pdf4lhc_nlo/hw.nominal);
    alternative_pdf.push_back(hw.mmht2014nlo/hw.nominal);
    alternative_pdf.push_back(hw.mmht2014nlo/hw.nominal);
    alternative_pdf.push_back(hw.ct14nlo/hw.nominal);
    alternative_pdf.push_back(hw.ct10nlo/hw.nominal);


    qcd_uncert_nnlops_2np.push_back(hw.qcd_nnlops_nnlo/hw.nominal);
    qcd_uncert_nnlops_2np.push_back(hw.qcd_nnlops_pow/hw.nominal);
    
    Props::alpha_s_up.set(m_eventInfoIn, hw.alphaS_up /hw.nominal );
    Props::alpha_s_dn.set(m_eventInfoIn, hw.alphaS_dn /hw.nominal );
    Props::weight_pdf4lhc.set(m_eventInfoIn, pdf4lhc_unc);
    Props::weight_pdfnnpdf30.set(m_eventInfoIn, pdfnnpdf30_unc);
    Props::weight_qcd_nnlops.set(m_eventInfoIn, qcd_uncert_nnlops);
    Props::weight_qcd_nnlops_2np.set(m_eventInfoIn, qcd_uncert_nnlops_2np);
    Props::weight_qcd_WG1.set(m_eventInfoIn, qcd_uncert_WG1);
    Props::weight_alternative_pdf.set(m_eventInfoIn, alternative_pdf);

  }
  else{
    std::vector<float> dummy;
    Props::alpha_s_up.set(m_eventInfoIn, 1);
    Props::alpha_s_dn.set(m_eventInfoIn, 1);
    Props::weight_pdf4lhc.set(m_eventInfoIn, dummy);
    Props::weight_pdfnnpdf30.set(m_eventInfoIn, dummy);
    Props::weight_qcd_nnlops.set(m_eventInfoIn, dummy);
    Props::weight_qcd_nnlops_2np.set(m_eventInfoIn, dummy);
    Props::weight_qcd_WG1.set(m_eventInfoIn, dummy);
    Props::weight_alternative_pdf.set(m_eventInfoIn,dummy); 
  }

  return EL::StatusCode::SUCCESS;
}


void EventInfoHandler_VBFHbb::writeOutputVariables(xAOD::EventInfo* eventInfoIn, xAOD::EventInfo* eventInfoOut) {
  EventInfoHandler::writeOutputVariables(eventInfoIn, eventInfoOut);
 
  for (auto& ps : m_trigPrescales) {
    ps.second->copy(m_eventInfoIn, eventInfoOut);
  }

  for (auto& trig : m_trigIsActive) {
    trig.second->copy(m_eventInfoIn, eventInfoOut);
  }
  Props::npv.copyIfExists(eventInfoIn, eventInfoOut);
  Props::hltht.copyIfExists(eventInfoIn, eventInfoOut);
  Props::hltmjj.copyIfExists(eventInfoIn, eventInfoOut);
  Props::l1ht.copyIfExists(eventInfoIn, eventInfoOut);
  Props::l1mjj.copyIfExists(eventInfoIn, eventInfoOut);

  Props::alpha_s_up.copyIfExists(m_eventInfoIn, eventInfoOut);
  Props::alpha_s_dn.copyIfExists(m_eventInfoIn, eventInfoOut);
  Props::weight_pdf4lhc.copyIfExists(eventInfoIn, eventInfoOut);
  Props::weight_pdfnnpdf30.copyIfExists(eventInfoIn, eventInfoOut);
  Props::weight_qcd_nnlops.copyIfExists(m_eventInfoIn, eventInfoOut);
  Props::weight_qcd_nnlops_2np.copyIfExists(m_eventInfoIn, eventInfoOut);
  Props::weight_qcd_WG1.copyIfExists(m_eventInfoIn, eventInfoOut);
  Props::weight_alternative_pdf.copyIfExists(m_eventInfoIn, eventInfoOut);

  Props::muonEff.copyIfExists(eventInfoIn, eventInfoOut);
  Props::muonEff_cvmfs.copyIfExists(eventInfoIn, eventInfoOut);
  Props::muonTrigEff.copyIfExists(eventInfoIn, eventInfoOut);
  Props::muonTrigEff_cvmfs_14.copyIfExists(eventInfoIn, eventInfoOut);
  Props::muonTrigEff_cvmfs_50.copyIfExists(eventInfoIn, eventInfoOut);
  Props::muonTrigEff_cvmfs.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embMu1pt.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embMu1eta.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embMu1phi.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embMu1ptvarcone30.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embMu1topoetcone20.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embMu2pt.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embMu2eta.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embMu2phi.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embMu2ptvarcone30.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embMu2topoetcone20.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embZpt.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embZeta.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embZphi.copyIfExists(eventInfoIn, eventInfoOut);
  Props::embZm.copyIfExists(eventInfoIn, eventInfoOut);
  Props::hpt.copyIfExists(eventInfoIn, eventInfoOut);
  Props::heta.copyIfExists(eventInfoIn, eventInfoOut);
  Props::hphi.copyIfExists(eventInfoIn, eventInfoOut);
  Props::hm.copyIfExists(eventInfoIn, eventInfoOut);

}
