// Dear emacs, this is -*-c++-*-
#ifndef CxAODTools_VBFHbb_CommonProperties_H
#define CxAODTools_VBFHbb_CommonProperties_H

#ifndef __MAKECINT__

#include "CxAODTools/CommonProperties.h"


// Preprocessor macro to define property classes
//
// Usage example:
// (in ~ CxAODTools_MyAnalysis/CxAODTools_MyAnalysis/Properties_MyAnalysis.h)
//
// #include "CxAODTools/CommonProperties.h"
// PROPERTY( Props , int   , myInt   );
// PROPERTY( Props , float , myFloat );
// PROPERTY( MyProps , int   , myInt   );
// ...

// Note, it is _never_ needed to specify your own namespace "MyProps" if the
// property already exists in another namespace, e.g. in "Props". In general,
// it's better always use namespace "Props" in your own analysis, since you
// then get an extra compile time check on whether you declared a property
// with the same name as another previously declared property but with a
// different type (if using a different namespace, this would compile, but
// fail at runtime).

// IMPORTANT :
// Don't attempt use bool properties !!!
// A bool properties gets saved as char when written to disk.
// Consequently, you cannot use the same code to access the property, when
// reading the file back in. For this reason, the bool template is disabled.


PROPERTY( Props , int , npv)
PROPERTY( Props , float , hltht)
PROPERTY( Props , float , hltmjj)
PROPERTY( Props , float , l1ht)
PROPERTY( Props , float , l1mjj)

//########################
// trigger passes
//########################
//2017 trigger
PROPERTY( Props , int , passHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , int , passHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , int , passHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , int , passHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , int , passHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , int , passHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25 )
PROPERTY( Props , int , passHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , int , passHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30 )
PROPERTY( Props , int , passHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30 )
PROPERTY( Props , int , passHLT_j100_2j55_bmv2c2060_split )
PROPERTY( Props , int , passHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , int , passHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , int , passHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , int , passHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , int , passHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490 )
PROPERTY( Props , int , passHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 )
PROPERTY( Props , int , passHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 )
PROPERTY( Props , int , passHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 )
PROPERTY( Props , int , passHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split )
PROPERTY( Props , int , passHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split )
PROPERTY( Props , int , passHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split )
PROPERTY( Props , int , passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split )
PROPERTY( Props , int , passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF )
PROPERTY( Props , int , passHLT_noalg_l1topo_L1MJJ_400_CF )
PROPERTY( Props , int , passL1_HT150_J20s5_ETA31_MJJ_400_CF )
PROPERTY( Props , int , passHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split )
PROPERTY( Props , int , passHLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190_J15s5_ETA21 )
PROPERTY( Props , int , passHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF )
PROPERTY( Props , int , passHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split )


// 2 central + 1 forward trigger
PROPERTY( Props , int , passL1_J40_0ETA25_2J25_J20_31ETA49 )
PROPERTY( Props , int , passL1_J20_31ETA49 )
PROPERTY( Props , int , passL1_J15_31ETA49 )
PROPERTY( Props , int , passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490 )
PROPERTY( Props , int , passHLT_j80_0eta240_j60_j45_320eta490 )
// 1 central + 2 forward trigger
PROPERTY( Props , int , passL1_J40_0ETA25_2J15_31ETA49 )
PROPERTY( Props , int , passHLT_j80_bmv2c2085_split_2j60_320eta490 )
PROPERTY( Props , int , passHLT_j80_0eta240_2j60_320eta490 )
// old triggers
PROPERTY( Props , int , passL1_4J20 )
PROPERTY( Props , int , passHLT_2j55_bmv2c2077_split_2j55 )
PROPERTY( Props , int , passHLT_2j45_bmv2c2070_split_2j45 )
PROPERTY( Props , int , passL1_4J15 )
PROPERTY( Props , int , passHLT_2j35_bmv2c2070_split_2j35_L14J15 )
PROPERTY( Props , int , passHLT_2j35_bmv2c2060_split_2j35_L14J15 )
PROPERTY( Props , int , passHLT_2j35_bmv2c2050_split_2j35_L14J15 )
PROPERTY( Props , int , passHLT_j75_bmv2c2060_split_3j75_L14J15 )
PROPERTY( Props , int , passHLT_j75_bmv2c2070_split_3j75_L14J15 )
PROPERTY( Props , int , passHLT_j175_gsc225_bmv2c1040_split )
PROPERTY( Props , int , passHLT_j175_bmv2c2040_split )
PROPERTY( Props , int , passHLT_j225_bmv2c2060_split )
PROPERTY( Props , int , passHLT_j225_gsc275_bmv2c1060_split )
PROPERTY( Props , int , passHLT_j225_gsc275_bmv2c1070_split )
PROPERTY( Props , int , passHLT_j225_gsc300_bmv2c1070_split )
PROPERTY( Props , int , passHLT_j225_gsc300_bmv2c1077_split )
// additional triggers for photon study
PROPERTY( Props , int , passHLT_2j45_bmv2c2077_split_2j45_L14J15 )
PROPERTY( Props , int , passHLT_2j45_bmv2c2070_split_2j45_L14J15 )
PROPERTY( Props , int , passHLT_2j45_bmv2c2070_split_2j45_L14J20 )
PROPERTY( Props , int , passHLT_4j45 )

//################################################################
// trigger emulation parts to make proper turn on curves with data
//################################################################
// 2 central + 1 forward trigger
PROPERTY( Props , int , passL1_J40_0ETA25_2J25 )
PROPERTY( Props , int , passHLT_j80_bmv2c2070_split_j60_bmv2c2085_split )
PROPERTY( Props , int , passHLT_j45_320eta490 )
PROPERTY( Props , int , passHLT_j80_0eta240_j60 )
// 1 central + 2 forward trigger
PROPERTY( Props , int , passL1_J40_0ETA25 )
PROPERTY( Props , int , passL1_2J15_31ETA49 )
PROPERTY( Props , int , passHLT_j80_bmv2c2085_split )
PROPERTY( Props , int , passHLT_2j60_320eta490 )
PROPERTY( Props , int , passHLT_j80_0eta240 )

/// trigger matching objects<
PROPERTY( Props , char , HLTBJetMatched_2J45_MV2c20_70)
PROPERTY( Props , char , HLTBJetMatched_2J45_MV2c10_70)
PROPERTY( Props , char , HLTBJetMatched_2J35_MV2c20_60)
PROPERTY( Props , char , HLTBJetMatched_2J45_MV2c10_60)
PROPERTY( Props , char , HLTBJetMatched_2J45_MV2c10_50)
PROPERTY( Props , char , HLTBJetMatched_2J55_MV2c10_70)
PROPERTY( Props , char , HLTBJetMatched_2J55_MV2c20_60)
PROPERTY( Props , char , HLTBJetMatched_2J55_MV2c10_60)
PROPERTY( Props , float , HLTJetMatchedPt)
PROPERTY( Props , float , HLTGSCJetMatchedPt)
PROPERTY( Props , float , HLTRAWJetMatchedPt)
PROPERTY( Props , float , L1JetMatchedPt)
PROPERTY( Props , float , L1JetMatchedEta)
PROPERTY( Props , float , L1JetMatchedPhi)
PROPERTY( Props , float , TruthJetMatchedPt)
PROPERTY( Props , float , TruthJetMatchedEta)
PROPERTY( Props , float , TruthWZ_pt)
PROPERTY( Props , float , TruthWZ_eta)
PROPERTY( Props , float , TruthWZ_phi)
PROPERTY( Props , float , TruthWZ_m)
PROPERTY( Props , char , HLTBJetMatched_J80_MV2c20_70)
PROPERTY( Props , char , HLTBJetMatched_J60_MV2c20_85)
PROPERTY( Props , char , HLTBJetMatched_J80_MV2c10_70)
PROPERTY( Props , char , HLTBJetMatched_J60_MV2c10_85)
PROPERTY( Props , char , isJvtHS)
PROPERTY( Props , char , isJvtPU)
PROPERTY( Props , float , JetConstitScaleMomentum_pt )
PROPERTY( Props , float , muonEff)
PROPERTY( Props , float , muonEff_cvmfs)
PROPERTY( Props , float , muonTrigEff)
PROPERTY( Props , float , muonTrigEff_cvmfs_14)
PROPERTY( Props , float , muonTrigEff_cvmfs_50)
PROPERTY( Props , float , muonTrigEff_cvmfs)
PROPERTY( Props , float , embMu1pt)
PROPERTY( Props , float , embMu1eta)
PROPERTY( Props , float , embMu1phi)
PROPERTY( Props , float , embMu1ptvarcone30)
PROPERTY( Props , float , embMu1topoetcone20)
PROPERTY( Props , float , embMu2pt)
PROPERTY( Props , float , embMu2eta)
PROPERTY( Props , float , embMu2phi)
PROPERTY( Props , float , embMu2ptvarcone30)
PROPERTY( Props , float , embMu2topoetcone20)
PROPERTY( Props , float , embZpt)
PROPERTY( Props , float , embZeta)
PROPERTY( Props , float , embZphi)
PROPERTY( Props , float , embZm)
PROPERTY( Props , float , hpt)
PROPERTY( Props , float , heta)
PROPERTY( Props , float , hphi)
PROPERTY( Props , float , hm)
//=======
//########################
// trigger emulation tool
//########################

PROPERTY( Props , int , passEmulToolL1_4J15 )
PROPERTY( Props , int , passEmulToolL1_2J15_31ETA49 )
PROPERTY( Props , int , passEmulToolL1_J40_0ETA25 )
PROPERTY( Props , int , passEmulToolL1_J20_31ETA49 )
PROPERTY( Props , int , passEmulToolL1_J15_31ETA49 )
PROPERTY( Props , int , passEmulToolL1_2J15 )
PROPERTY( Props , int , passEmulToolL1_2J25 )

PROPERTY( Props , int , passEmulToolHLT_4j35 )
PROPERTY( Props , int , passEmulToolHLT_4j45 )
PROPERTY( Props , int , passEmulToolHLT_2j35_bmv2c2060_split )
PROPERTY( Props , int , passEmulToolHLT_2j45_bmv2c2070_split )
PROPERTY( Props , int , passEmulToolHLT_j80_bmv2c2070_split  )
PROPERTY( Props , int , passEmulToolHLT_2j60_bmv2c2085_split )
PROPERTY( Props , int , passEmulToolHLT_j80_bmv2c2085_split  )
PROPERTY( Props , int , passEmulToolHLT_j45_320eta490 )
PROPERTY( Props , int , passEmulToolHLT_2j60_320eta490 )
// Supporting HLT triggers
PROPERTY( Props , int , passEmulToolHLT_j80_0eta240)
PROPERTY( Props , int , passEmulToolHLT_2j60)

PROPERTY( Props , int , passEmulToolHLT_2j35_bmv2c2060_split_2j35_L14J15 )
PROPERTY( Props , int , passEmulToolHLT_2j45_bmv2c2070_split_2j45_L14J15 )
PROPERTY( Props , int , passEmulToolHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490_L1_J40_0ETA25_2J25_J20_31ETA49 )
PROPERTY( Props , int , passEmulToolHLT_j80_bmv2c2085_split_2j60_320eta490_L1_J40_0ETA25_2J15_31ETA49 )
//>>>>>>> 74df6ffc79544f3eaba934dbfee7cf7ba57cd8ca


//#############################
// were triggers active or not?
//#############################
//2017
PROPERTY( Props , char , isActiveHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , char , isActiveHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , char , isActiveHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , char , isActiveHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , char , isActiveHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , char , isActiveHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25 )
PROPERTY( Props , char , isActiveHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , char , isActiveHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30 )
PROPERTY( Props , char , isActiveHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30 )
PROPERTY( Props , char , isActiveHLT_j100_2j55_bmv2c2060_split )
PROPERTY( Props , char , isActiveHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , char , isActiveHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , char , isActiveHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , char , isActiveHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , char , isActiveHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490 )
PROPERTY( Props , char , isActiveHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 )
PROPERTY( Props , char , isActiveHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 )
PROPERTY( Props , char , isActiveHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 )
PROPERTY( Props , char , isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split )
PROPERTY( Props , char , isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split )
PROPERTY( Props , char , isActiveHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split )
PROPERTY( Props , char , isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split )
PROPERTY( Props , char , isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF )
PROPERTY( Props , char , isActiveHLT_noalg_l1topo_L1MJJ_400_CF )
PROPERTY( Props , char , isActiveL1_HT150_J20s5_ETA31_MJJ_400_CF )
PROPERTY( Props , char , isActiveHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split )
PROPERTY( Props , char , isActiveHLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190_J15s5_ETA21 )
PROPERTY( Props , char , isActiveHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF )
PROPERTY( Props , char , isActiveHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split )
// 2 central + 1 forward trigger
PROPERTY( Props , char , isActiveL1_J40_0ETA25_2J25_J20_31ETA49 )
PROPERTY( Props , char , isActiveL1_J20_31ETA49 )
PROPERTY( Props , char , isActiveL1_J15_31ETA49 )
PROPERTY( Props , char , isActiveHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490 )
PROPERTY( Props , char , isActiveHLT_j80_0eta240_j60_j45_320eta490 )
// 1 central + 2 forward trigger
PROPERTY( Props , char , isActiveL1_J40_0ETA25_2J15_31ETA49 )
PROPERTY( Props , char , isActiveHLT_j80_bmv2c2085_split_2j60_320eta490 )
PROPERTY( Props , char , isActiveHLT_j80_0eta240_2j60_320eta490 )
// old triggers
PROPERTY( Props , char , isActiveL1_4J20 )
PROPERTY( Props , char , isActiveHLT_2j55_bmv2c2077_split_2j55 )
PROPERTY( Props , char , isActiveHLT_2j45_bmv2c2070_split_2j45 )
PROPERTY( Props , char , isActiveL1_4J15 )
PROPERTY( Props , char , isActiveHLT_2j35_bmv2c2070_split_2j35_L14J15 )
PROPERTY( Props , char , isActiveHLT_2j35_bmv2c2060_split_2j35_L14J15 )
PROPERTY( Props , char , isActiveHLT_2j35_bmv2c2050_split_2j35_L14J15 )
PROPERTY( Props , char , isActiveHLT_j75_bmv2c2060_split_3j75_L14J15 )
PROPERTY( Props , char , isActiveHLT_j75_bmv2c2070_split_3j75_L14J15 )
PROPERTY( Props , char , isActiveHLT_j175_gsc225_bmv2c1040_split )
PROPERTY( Props , char , isActiveHLT_j175_bmv2c2040_split )
PROPERTY( Props , char , isActiveHLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700 )
PROPERTY( Props , char , isActiveHLT_j225_bmv2c2060_split )
PROPERTY( Props , char , isActiveHLT_j225_gsc275_bmv2c1060_split )
PROPERTY( Props , char , isActiveHLT_j225_gsc275_bmv2c1070_split )
PROPERTY( Props , char , isActiveHLT_j225_gsc300_bmv2c1070_split )
PROPERTY( Props , char , isActiveHLT_j225_gsc300_bmv2c1077_split )
PROPERTY( Props , char , isActiveHLT_j420 )
// additional triggers for photon study
PROPERTY( Props , char , isActiveHLT_g140_loose )
PROPERTY( Props , char , isActiveHLT_g25_medium )
PROPERTY( Props , char , isActiveHLT_2j45_bmv2c2077_split_2j45_L14J15 )
PROPERTY( Props , char , isActiveHLT_2j45_bmv2c2070_split_2j45_L14J15 )
PROPERTY( Props , char , isActiveHLT_2j45_bmv2c2070_split_2j45_L14J20 )
PROPERTY( Props , char , isActiveHLT_4j45 )

//##################
// trigger prescales
//##################
//2017
PROPERTY( Props , float , prescaleHLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , float , prescaleHLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , float , prescaleHLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , float , prescaleHLT_2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , float , prescaleHLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , float , prescaleHLT_2j35_bmv2c1060_split_2j35_L14J15_0ETA25 )
PROPERTY( Props , float , prescaleHLT_2j35_bmv2c1060_split_2j35_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , float , prescaleHLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30 )
PROPERTY( Props , float , prescaleHLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30 )
PROPERTY( Props , float , prescaleHLT_j100_2j55_bmv2c2060_split )
PROPERTY( Props , float , prescaleHLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , float , prescaleHLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , float , prescaleHLT_2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , float , prescaleHLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15_0ETA25 )
PROPERTY( Props , float , prescaleHLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490 )
PROPERTY( Props , float , prescaleHLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 )
PROPERTY( Props , float , prescaleHLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 )
PROPERTY( Props , float , prescaleHLT_j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25_0ETA23_2J15_31ETA49 )
PROPERTY( Props , float , prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split )
PROPERTY( Props , float , prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split )
PROPERTY( Props , float , prescaleHLT_j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c2070_split )
PROPERTY( Props , float , prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j25_gsc45_bmv2c1070_split )
PROPERTY( Props , float , prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF )
PROPERTY( Props , float , prescaleHLT_noalg_l1topo_L1MJJ_400_CF )
PROPERTY( Props , float , prescaleL1_HT150_J20s5_ETA31_MJJ_400_CF )
PROPERTY( Props , float , prescaleHLT_ht300_2j40_0eta490_invm700_L1HT150_J20s5_ETA31_MJJ_400_CF_AND_2j35_gsc45_bmv2c1070_split )
PROPERTY( Props , float , prescaleHLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190_J15s5_ETA21 )
PROPERTY( Props , float , prescaleHLT_j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ_500_NFF )
PROPERTY( Props , float , prescaleHLT_j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ_500_NFF_AND_2j35_bmv2c1070_split )
PROPERTY( Props , float , prescaleL1_EM13VH )
PROPERTY( Props , float , prescaleL1_EM15VH )
PROPERTY( Props , float , prescaleL1_EM18VH )
PROPERTY( Props , float , prescaleL1_EM20VH )
PROPERTY( Props , float , prescaleL1_EM22VHI )
PROPERTY( Props , float , prescaleL1_2EM15VH )
PROPERTY( Props , float , prescaleHLT_e15_lhtight_ivarloose_3j20_L1EM13VH_3J20 )
PROPERTY( Props , float , prescaleHLT_e17_lhloose_L1EM15 )
PROPERTY( Props , float , prescaleHLT_e24_lhvloose_L1EM18VH )
PROPERTY( Props , float , prescaleHLT_e24_lhvloose_L1EM20VH )
PROPERTY( Props , float , prescaleHLT_e24_medium_iloose_L1EM18VH )
PROPERTY( Props , float , prescaleHLT_e24_medium_iloose_L1EM20VH )
PROPERTY( Props , float , prescaleHLT_e24_lhmedium_iloose_L1EM18VH )
PROPERTY( Props , float , prescaleHLT_e24_lhmedium_iloose_L1EM20VH )
PROPERTY( Props , float , prescaleHLT_e24_lhmedium_L1EM18VH )
PROPERTY( Props , float , prescaleHLT_e24_lhmedium_L1EM20VH )
PROPERTY( Props , float , prescaleHLT_e24_tight_iloose )
PROPERTY( Props , float , prescaleHLT_e24_lhtight_iloose )
PROPERTY( Props , float , prescaleHLT_e26_tight_iloose )
PROPERTY( Props , float , prescaleHLT_e26_tight1_iloose )
PROPERTY( Props , float , prescaleHLT_e26_lhtight_iloose )
PROPERTY( Props , float , prescaleHLT_e26_lhtight_ivarloose )
PROPERTY( Props , float , prescaleHLT_e26_lhtight_smooth_ivarloose )
PROPERTY( Props , float , prescaleHLT_e26_lhmedium_nod0_ivarloose )
PROPERTY( Props , float , prescaleHLT_e60_medium )
PROPERTY( Props , float , prescaleHLT_e60_medium1 )
PROPERTY( Props , float , prescaleHLT_e60_lhmedium )
PROPERTY( Props , float , prescaleHLT_e60_lhmedium_nod0 )
PROPERTY( Props , float , prescaleHLT_e120_lhloose )
PROPERTY( Props , float , prescaleHLT_e120_lhloose_nod0 )
PROPERTY( Props , float , prescaleHLT_e140_lhloose )
PROPERTY( Props , float , prescaleHLT_e140_lhloose_nod0 )
PROPERTY( Props , float , prescaleHLT_2e17_loose )
PROPERTY( Props , float , prescaleHLT_2e17_loose1 )
PROPERTY( Props , float , prescaleHLT_2e17_lhloose )
PROPERTY( Props , float , prescaleHLT_e26_lhtight_nod0_ivarloose )
PROPERTY( Props , float , prescaleHLT_e24_lhtight_ivarloose )
PROPERTY( Props , float , prescaleHLT_e24_lhtight_nod0_ivarloose )
PROPERTY( Props , float , prescaleHLT_e24_lhmedium_ivarloose )
PROPERTY( Props , float , prescaleHLT_e24_lhmedium_nod0_ivarloose )
PROPERTY( Props , float , prescaleHLT_e24_lhmedium_nod0_L1EM20VH )
PROPERTY( Props , float , prescaleHLT_2e17_lhloose_nod0 )
PROPERTY( Props , float , prescaleHLT_2e15_lhloose_L12EM13VH )
PROPERTY( Props , float , prescaleHLT_2e15_lhloose_nod0_L12EM13VH )
PROPERTY( Props , float , prescaleHLT_2e12_lhloose_L12EM10VH )
PROPERTY( Props , float , prescaleHLT_2e12_lhloose_nod0_L12EM10VH )
PROPERTY( Props , float , prescaleHLT_g10_loose )
PROPERTY( Props , float , prescaleHLT_g15_loose_L1EM7 )
PROPERTY( Props , float , prescaleHLT_g20_loose_L1EM12 )
PROPERTY( Props , float , prescaleHLT_g25_loose_L1EM15 )
PROPERTY( Props , float , prescaleHLT_g35_loose_L1EM15 )
PROPERTY( Props , float , prescaleHLT_g40_loose_L1EM15 )
PROPERTY( Props , float , prescaleHLT_g45_loose_L1EM15 )
PROPERTY( Props , float , prescaleHLT_g50_loose_L1EM15 )
PROPERTY( Props , float , prescaleHLT_g60_loose )
PROPERTY( Props , float , prescaleHLT_g70_loose )
PROPERTY( Props , float , prescaleHLT_g80_loose )
PROPERTY( Props , float , prescaleHLT_g90_loose )
PROPERTY( Props , float , prescaleHLT_g100_loose )
PROPERTY( Props , float , prescaleHLT_g120_loose )
PROPERTY( Props , float , prescaleL1_MJJ_400 )
PROPERTY( Props , float , prescaleL1_MJJ_700 )
PROPERTY( Props , float , prescaleL1_MJJ_800 )
PROPERTY( Props , float , prescaleL1_MJJ_900 )
PROPERTY( Props , float , prescaleL1_J30_2J20_4J20_0ETA49_MJJ_400 )
PROPERTY( Props , float , prescaleL1_J30_2J20_4J20_0ETA49_MJJ_700 )
PROPERTY( Props , float , prescaleL1_J30_2J20_4J20_0ETA49_MJJ_800 )
PROPERTY( Props , float , prescaleL1_J30_2J20_4J20_0ETA49_MJJ_900 )
PROPERTY( Props , float , prescaleL1_3J20_4J20_0ETA49_MJJ_400 )
PROPERTY( Props , float , prescaleL1_3J20_4J20_0ETA49_MJJ_700 )
PROPERTY( Props , float , prescaleL1_3J20_4J20_0ETA49_MJJ_800 )
PROPERTY( Props , float , prescaleL1_3J20_4J20_0ETA49_MJJ_900 )
PROPERTY( Props , float , prescaleL1_4J20_0ETA49 )
PROPERTY( Props , float , prescaleHLT_g15_loose_2j40_0eta490_3j25_0eta490 )
PROPERTY( Props , float , prescaleHLT_g20_loose_2j40_0eta490_3j25_0eta490 )
PROPERTY( Props , float , prescaleHLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ_700 )
PROPERTY( Props , float , prescaleHLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ_900 )
PROPERTY( Props , float , prescaleHLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490 )
PROPERTY( Props , float , prescaleHLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490_invm700 )
PROPERTY( Props , float , prescaleHLT_g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700 )
PROPERTY( Props , float , prescaleHLT_g25_loose_2j40_0eta490_3j25_0eta490_invm700 )
PROPERTY( Props , float , prescaleHLT_g25_medium_L1EM22VHI_4j35_0eta490_invm700)
PROPERTY( Props , float , prescaleHLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j35_0eta490_invm700)
PROPERTY( Props , float , prescaleHLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_2j35_0eta490)
PROPERTY( Props , float , prescaleHLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_split_3j35_0eta490_invm700)
PROPERTY( Props , float , prescaleHLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_split_2j35_0eta490)
PROPERTY( Props , float , prescaleHLT_g25_medium_L1EM22VHI_j35_0eta490_boffperf_split_3j35_0eta490_invm700)
PROPERTY( Props , float , prescaleHLT_g25_medium_L1EM22VHI_2j35_0eta490_boffperf_split_2j35_0eta490)
PROPERTY( Props , float , prescaleHLT_g25_loose_L1EM20VH_4j35_0eta490)
PROPERTY( Props , float , prescaleHLT_2j55_bperf_L1J30_2J20_4J20_0ETA49_MJJ_400 )
PROPERTY( Props , float , prescaleHLT_2j55_bperf_L1J30_2J20_4J20_0ETA49_MJJ_700 )
PROPERTY( Props , float , prescaleHLT_2j55_bperf_L1J30_2J20_4J20_0ETA49_MJJ_800 )
PROPERTY( Props , float , prescaleHLT_2j55_bperf_L1J30_2J20_4J20_0ETA49_MJJ_900 )
PROPERTY( Props , float , prescaleHLT_2j55_bperf_L13J20_4J20_0ETA49_MJJ_400 )
PROPERTY( Props , float , prescaleHLT_2j55_bperf_L13J20_4J20_0ETA49_MJJ_700 )
PROPERTY( Props , float , prescaleHLT_2j55_bperf_L13J20_4J20_0ETA49_MJJ_800 )
PROPERTY( Props , float , prescaleHLT_2j55_bperf_L13J20_4J20_0ETA49_MJJ_900 )
PROPERTY( Props , float , prescaleHLT_2j55_bperf_L14J20_0ETA49 )
PROPERTY( Props , float , prescaleHLT_2j55_bloose_L1J30_2J20_4J20_0ETA49_MJJ_400 )
PROPERTY( Props , float , prescaleHLT_2j55_bloose_L1J30_2J20_4J20_0ETA49_MJJ_700 )
PROPERTY( Props , float , prescaleHLT_2j55_bloose_L1J30_2J20_4J20_0ETA49_MJJ_800 )
PROPERTY( Props , float , prescaleHLT_2j55_bloose_L1J30_2J20_4J20_0ETA49_MJJ_900 )
PROPERTY( Props , float , prescaleHLT_2j55_bloose_L13J20_4J20_0ETA49_MJJ_400 )
PROPERTY( Props , float , prescaleHLT_2j55_bloose_L13J20_4J20_0ETA49_MJJ_700 )
PROPERTY( Props , float , prescaleHLT_2j55_bloose_L13J20_4J20_0ETA49_MJJ_800 )
PROPERTY( Props , float , prescaleHLT_2j55_bloose_L13J20_4J20_0ETA49_MJJ_900 )
PROPERTY( Props , float , prescaleHLT_2j55_bloose_L14J20_0ETA49 )
PROPERTY( Props , float , prescaleHLT_2j55_bmedium_L13J20_4J20_0ETA49_MJJ_400 )
PROPERTY( Props , float , prescaleHLT_2j55_bmedium_L13J20_4J20_0ETA49_MJJ_700 )
PROPERTY( Props , float , prescaleHLT_2j55_bmedium_L13J20_4J20_0ETA49_MJJ_800 )
PROPERTY( Props , float , prescaleHLT_2j55_bmedium_L13J20_4J20_0ETA49_MJJ_900 )
PROPERTY( Props , float , prescaleHLT_2j55_bmedium_L1J30_2J20_4J20_0ETA49_MJJ_400 )
PROPERTY( Props , float , prescaleHLT_2j55_bmedium_L1J30_2J20_4J20_0ETA49_MJJ_700 )
PROPERTY( Props , float , prescaleHLT_2j55_bmedium_L1J30_2J20_4J20_0ETA49_MJJ_800 )
PROPERTY( Props , float , prescaleHLT_2j55_bmedium_L1J30_2J20_4J20_0ETA49_MJJ_900 )
PROPERTY( Props , float , prescaleHLT_2j55_bmedium_L14J20_0ETA49 )
PROPERTY( Props , float , prescaleL1_EM20VHI )
// jet trigger prescales
PROPERTY( Props , float , prescaleHLT_j300 )
PROPERTY( Props , float , prescaleHLT_j320 )
PROPERTY( Props , float , prescaleHLT_j360 )
PROPERTY( Props , float , prescaleHLT_j380 )
PROPERTY( Props , float , prescaleHLT_j400 )
PROPERTY( Props , float , prescaleHLT_j420 )
PROPERTY( Props , float , prescaleHLT_j440 )
PROPERTY( Props , float , prescaleHLT_j460 )
PROPERTY( Props , float , prescaleHLT_j45_bperf )
// 2 central + 1 forward trigger prescales
PROPERTY( Props , float , prescaleL1_J40_0ETA25_2J25_J20_31ETA49 )
PROPERTY( Props , float , prescaleL1_J20_31ETA49 )
PROPERTY( Props , float , prescaleL1_J15_31ETA49 )
PROPERTY( Props , float , prescaleHLT_j80_bmv2c2070_split_j60_bmv2c2085_split_j45_320eta490 )
PROPERTY( Props , float , prescaleHLT_j80_0eta240_j60_j45_320eta490 )
// 1 central + 2 forward trigger prescales
PROPERTY( Props , float , prescaleL1_J40_0ETA25_2J15_31ETA49 )
PROPERTY( Props , float , prescaleHLT_j80_bmv2c2085_split_2j60_320eta490 )
PROPERTY( Props , float , prescaleHLT_j80_0eta240_2j60_320eta490 )
// old trigger prescales
PROPERTY( Props , float , prescaleL1_4J20 )
PROPERTY( Props , float , prescaleHLT_2j55_bmv2c2077_split_2j55 )
PROPERTY( Props , float , prescaleHLT_2j45_bmv2c2070_split_2j45 )
PROPERTY( Props , float , prescaleL1_4J15 )
PROPERTY( Props , float , prescaleHLT_2j35_bmv2c2070_split_2j35_L14J15 )
PROPERTY( Props , float , prescaleHLT_2j35_bmv2c2060_split_2j35_L14J15 )
PROPERTY( Props , float , prescaleHLT_2j35_bmv2c2050_split_2j35_L14J15 )
PROPERTY( Props , float , prescaleHLT_j75_bmv2c2060_split_3j75_L14J15 )
PROPERTY( Props , float , prescaleHLT_j75_bmv2c2070_split_3j75_L14J15 )
PROPERTY( Props , float , prescaleHLT_j175_gsc225_bmv2c1040_split )
PROPERTY( Props , float , prescaleHLT_j175_bmv2c2040_split )
PROPERTY( Props , float , prescaleHLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700 )
PROPERTY( Props , float , prescaleHLT_j225_bmv2c2060_split )
PROPERTY( Props , float , prescaleHLT_j225_gsc275_bmv2c1060_split )
PROPERTY( Props , float , prescaleHLT_j225_gsc275_bmv2c1070_split )
PROPERTY( Props , float , prescaleHLT_j225_gsc300_bmv2c1070_split )
PROPERTY( Props , float , prescaleHLT_j225_gsc300_bmv2c1077_split )
// additional triggers for photon study prescales
PROPERTY( Props , float , prescaleHLT_g140_loose )
PROPERTY( Props , float , prescaleHLT_g25_medium )
PROPERTY( Props , float , prescaleHLT_2j45_bmv2c2077_split_2j45_L14J15 )
PROPERTY( Props , float , prescaleHLT_2j45_bmv2c2070_split_2j45_L14J15 )
PROPERTY( Props , float , prescaleHLT_2j45_bmv2c2070_split_2j45_L14J20 )
PROPERTY( Props , float , prescaleHLT_4j45 )

PROPERTY( Props , std::vector<float> , weight_pdf4lhc)
PROPERTY( Props , std::vector<float> , weight_pdfnnpdf30)
PROPERTY( Props , std::vector<float> , weight_qcd_nnlops)
PROPERTY( Props , std::vector<float> , weight_qcd_nnlops_2np)
PROPERTY( Props , std::vector<float> , weight_qcd_WG1)
PROPERTY( Props , std::vector<float> , weight_alternative_pdf)
PROPERTY( Props , float, alpha_s_up)
PROPERTY( Props , float, alpha_s_dn)
PROPERTY( Props , int , xbbDoubleResult )


#endif // __MAKECINT__
#endif //CxAODMaker_VBFHbb_CommonProperties_H
