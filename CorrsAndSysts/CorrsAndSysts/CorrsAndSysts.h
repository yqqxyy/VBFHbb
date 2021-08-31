#ifndef CorrsAndSysts_h
#define CorrsAndSysts_h

/*
 * CorrsAndSysts class
 *
 * supply functions to apply corrections
 * and systematics needed for the H->bb
 * analysis
 *
 *  G. Facini, N. Morange & D. Buescher
 *  Wed Dec 12 13:07:00 CET 2012
 */

//
// Take all corrections and systematics that affect shapes out of Heather's
// script
//
// You should apply:
//
// NLO EW Higgs corrections
//
// W, Z, and top backgrounds shape corrections.
//
// pT and Mbb shape systematics. Cut-based values are here, for first
// estimates/checks. Should be replaced with systematics better tailored for
// MVA.
//
//
#include "PMGTools/PMGSherpa22VJetsWeightTool.h"

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "TF1.h"
#include "TH1.h"
#include "TObject.h"
#include "TSpline.h"
#include "TString.h"

namespace CAS {

enum EventType {
  WHlvbb = 0,
  qqZHllbb,
  qqZHvvbb,
  ggZHllbb, //**added by Lei: 2014Feb01
  ggZHvvbb, //**added by Lei: 2014Feb01
  WHlvcc,
  qqZHllcc,
  qqZHvvcc,
  ggZHllcc,
  ggZHvvcc,
  Z, // inclusive Z - FLS
  W, // inclusive W - FLS
  Wb,
  Wbb, // sgargiul 26.10.2019 - for BDTr pTV shape
  Wbc, // sgargiul 26.10.2019 - for BDTr pTV shape
  Wbl, // sgargiul 26.10.2019 - for BDTr pTV shape
  Wc,
  Wcc,
  Wcl,
  Wl,
  Zb,
  Zbb,
  Zbc,
  Zbl,
  Zc,
  Zcc,
  Zcl,
  Zl,
  ttbar,
  ttbarbc,
  ttbaroth,
  ttbarbq,
  ttbarlq,
  ttbarqq,
  stop_Wtbq,
  stop_Wtlq,
  stop_Wt,
  stop_Wtbb,
  stop_Wtoth,
  stop_s,
  stop_t,
  WW,
  WZ,
  ZZ,
  ZZcc,
  ZZbkg,
  WWcl,
  WWbkg,
  WZhadlepcl,
  WZhadlepbkg,
  WZlephadcc,
  WZlephadbkg,
  multijet,
  NONAME
};
enum DetailEventType {
  WWincl = 0,
  WlnuZhad,
  WhadZll,
  WhadZnunu,
  ZnunuZhad,
  ZllZhad,
  WWHerwig,
  WZHerwig,
  ZZHerwig,
  NODETAILNAME
};
enum SysVar { Do = 0, Up = 1 };

enum Systematic // only systematics affecting the shape are relevant to this
                // tool
{ Nominal,
  SysStopWtDSDR,    // gdigrego 19.11.19
  SysStoptPTV,      // yama 12.07.2016
  SysStoptMBB,      // yama 12.07.2016
  SysStopWtPTV,     // abell 29.04.18
  SysStopWtMBB,     // abell 29.04.18
  SysStopWtbbACC,   // abell 29.04.18
  SysStopWtothACC,  // abell 29.04.18
  SysTTbarMJISR,    // mfoti 20.11.19
  SysTTbarMJPS,     // mfoti 21.11.19
  SysTTbarbcMEACC,  // sjiggins 17.01.20
  SysTTbarbcPSACC,  // sjiggins 17.01.20
  SysTTbarOthMEACC, // sjiggins 17.01.20
  SysTTbarOthPSACC, // sjiggins 10.01.20

  SysWPtV_BDTr,     // sgargiul 24.10.19 //pTV shape used to reweight Sherpa
  SysTTbarPtV_BDTr, // kalkhour 14.11.19
                    // nominal when using BDTr shape systs
  SysZPtV,          // dbuesche 07.07.16
  SysZMbb,          // dbuesche 07.07.16
  SysMJJMR,         // bmoser 04.12.19
  SysMJVHPH7,       // bmoser 03.01.20
  SysVVMbbME,       // francav 15.07.16
  SysVVPTVME,       // francav 15.07.16
  SysVVMbbPSUE,     // francav 15.07.16
  SysVVPTVPSUE,     // francav 15.07.16
  SysVHQCDscalePTV, // francav 19.07.16
  SysVHQCDscaleMbb, // francav 19.07.16
  SysVHPDFPTV,      // francav 19.07.16
  SysVHQCDscalePTV_ggZH, // francav 19.07.16
  SysVHQCDscaleMbb_ggZH, // francav 19.07.16
  SysVHPDFPTV_ggZH,      // francav 19.07.16
  SysVHUEPSPTV,          // francav 19.07.16
  SysVHUEPSMbb,          // francav 19.07.16
  SysVHNLOEWK,           // francav 19.07.16

  // Boosted VHbb
  SysZMbbBoosted,    // weitao 20.11.19
  SysWMbbBoosted,    // weitao 20.11.19
  SysVVMURMJBoosted, // weitao 03.12.19
  SysVVPP8MJBoosted, // weitao 03.12.19

  //==================================================================

  //---------------VHcc------------------
  SysZhf_shape_ShMGPy8_mCC, // amendesj @ 20/04/20 (for 0&2lep)
  SysZmf_shape_ShMGPy8_mCC, // amendesj @ 20/04/20 (for 0&2lep)
  SysZl_shape_ShMGPy8_mCC,  // amendesj @ 20/04/20 (for 0&2lep)
  SysWhf_shape_ShMGPy8_mCC, // gkoren @ 12/04/2020 (for 0&1lep)
  SysWmf_shape_ShMGPy8_mCC, // gkoren @ 12/04/2020 (for 0&1lep)
  SysWl_shape_ShMGPy8_mCC,  // amendesj @ 20/04/2020 (for 0&1lep)

  // SR systs
  SysZhf_shape_ShMGPy8_SR_mCC,
  SysZmf_shape_ShMGPy8_SR_mCC,
  SysZl_shape_ShMGPy8_SR_mCC,
  SysWhf_shape_ShMGPy8_SR_mCC,
  SysWmf_shape_ShMGPy8_SR_mCC,
  SysWl_shape_ShMGPy8_SR_mCC,

  SysTTbarbq_shape_mCC_Pow_aMC, // mironova @ 30/04/20
  SysTTbarbq_shape_mCC_Her_Pyt, // mironova @ 30/04/20
  SysTTbarbq_shape_mCC_rHi,     // mironova @ 30/04/20

  SysTTbarlq_shape_mCC_Pow_aMC, // mironova @ 30/04/20
  SysTTbarlq_shape_mCC_Her_Pyt, // mironova @ 30/04/20
  SysTTbarlq_shape_mCC_rHi,     // mironova @ 30/04/20

  SysStopWtbq_shape_mCC_Pow_aMC, // mironova @ 30/04/20
  SysStopWtbq_shape_mCC_Her_Pyt, // mironova @ 30/04/20

  SysStopWtlq_shape_mCC_Pow_aMC, // mironova @ 30/04/20
  SysStopWtlq_shape_mCC_Her_Pyt, // mironova @ 30/04/20

  SysZZcc_shape_PwPy8_mCC,  // mironova @ 05/05/20
  SysZZbkg_shape_PwPy8_mCC, // mironova @ 05/05/20
  SysWWcl_shape_PwPy8_mCC,  // mironova @ 05/05/20
  SysWWbkg_shape_PwPy8_mCC, // mironova @ 05/05/20

  SysWZlephadcc_shape_PwPy8_mCC,  // mironova @ 05/05/20
  SysWZlephadbkg_shape_PwPy8_mCC, // mironova @ 05/05/20
  SysWZhadlepcl_shape_PwPy8_mCC,  // mironova @ 05/05/20
  SysWZhadlepbkg_shape_PwPy8_mCC, // mironova @ 05/05/20

  SysqqZHcc_shape_PwHwg7_mCC, // mironova @ 13/06/2020
  SysggZHcc_shape_PwHwg7_mCC, // mironova @ 13/06/2020
  SysqqWHcc_shape_PwHwg7_mCC, // mironova @ 13/06/2020
  LAST };

} // end namespace CAS

class CorrsAndSysts {
public:
  ~CorrsAndSysts() = default;

private:
  CorrsAndSysts(){};

  void Initialize();

  // Stores the WorkDir_DIR compiled location
  std::string m_WorkDir_DIR = "";

  // string to enums or enums to string
  std::map<std::string, CAS::EventType> m_typeNames = {};
  std::map<std::string, CAS::DetailEventType> m_detailtypeNames = {};
  std::map<CAS::Systematic, std::string> m_systNames = {};
  std::map<CAS::SysVar, std::string> m_varNames = {};
  std::map<std::string, CAS::Systematic> m_systFromNames = {};
  std::map<std::string, CAS::SysVar> m_varFromNames = {};

  bool m_draw = true;
  bool m_debug = false;
  bool m_zero = false;
  bool m_one = false;
  bool m_two = false;

  // use TH* to store weights ; seems easier to maintain if ever we need e.g 2-d
  // corrections corrections
  std::shared_ptr<TH1> m_h_WHlvbbNLOEWKCorrection = nullptr;
  std::shared_ptr<TH1> m_h_ZHllbbNLOEWKCorrection = nullptr;
  std::shared_ptr<TH1> m_h_ZHvvbbNLOEWKCorrection = nullptr;

  std::shared_ptr<TF1> m_f_SysTTbarPtV_BDTr_0l_2j = nullptr; // kalkhour 14.11.19
  std::shared_ptr<TF1> m_f_SysTTbarPtV_BDTr_0l_3j = nullptr; // kalkhour 14.11.19
  std::shared_ptr<TF1> m_f_SysTTbarPtV_BDTr_1l_2j = nullptr; // kalkhour 14.11.19
  std::shared_ptr<TF1> m_f_SysTTbarPtV_BDTr_1l_3j = nullptr; // kalkhour 14.11.19

  std::shared_ptr<TF1> m_f_SysStoptPTV = nullptr;     // eschopf 10.05.17
  std::shared_ptr<TF1> m_f_SysStoptPTV2jet = nullptr; // yama 12.07.16
  std::shared_ptr<TF1> m_f_SysStoptPTV3jet = nullptr; // yama 12.07.16
  std::shared_ptr<TF1> m_f_SysStoptMBB = nullptr;     // yama 12.07.16

  std::shared_ptr<TH1> m_h_SysStopWtDSDR_0L_Mptv_HPSR = nullptr; // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_0L_Mptv_LPSR = nullptr; // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_0L_Mptv_CR0J = nullptr; // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_0L_Mptv_CR1J = nullptr; // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_1L_Mptv_HPSR = nullptr; // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_1L_Mptv_LPSR = nullptr; // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_1L_Mptv_CR = nullptr;   // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_0L_Hptv_HPSR = nullptr; // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_1L_Hptv_HPSR = nullptr; // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_0L_Hptv_LPSR = nullptr; // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_1L_Hptv_LPSR = nullptr; // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_0L_Hptv_CR0J = nullptr; // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_0L_Hptv_CR1J = nullptr; // gdigrego 19.11.19
  std::shared_ptr<TH1> m_h_SysStopWtDSDR_1L_Hptv_CR = nullptr;   // gdigrego 19.11.19

  std::shared_ptr<TF1> m_f_SysStopWtbbPTV = nullptr;  // abell 26.04.18
  std::shared_ptr<TF1> m_f_SysStopWtbbMBB = nullptr;  // abell 26.04.18
  std::shared_ptr<TF1> m_f_SysStopWtothPTV = nullptr; // abell 26.04.18
  std::shared_ptr<TF1> m_f_SysStopWtothMBB = nullptr; // abell 26.04.18

  std::shared_ptr<TF1> m_f_SysTTbarMJISR_M = nullptr; // mfoti 20.11.19
  std::shared_ptr<TF1> m_f_SysTTbarMJISR_H = nullptr; // mfoti 20.11.19
  std::shared_ptr<TH1> m_h_SysTTbarMJPS = nullptr;    // mfoti 21.11.19

  std::shared_ptr<TF1> m_f_SysWPtV_BDTr_2j_bb = nullptr; // sgargiul 24.10.19
  std::shared_ptr<TF1> m_f_SysWPtV_BDTr_2j_bc = nullptr; // sgargiul 24.10.19
  std::shared_ptr<TF1> m_f_SysWPtV_BDTr_2j_bl = nullptr; // sgargiul 24.10.19
  std::shared_ptr<TF1> m_f_SysWPtV_BDTr_2j_cc = nullptr; // sgargiul 24.10.19
  std::shared_ptr<TF1> m_f_SysWPtV_BDTr_3j_bb = nullptr; // sgargiul 24.10.19
  std::shared_ptr<TF1> m_f_SysWPtV_BDTr_3j_bc = nullptr; // sgargiul 24.10.19
  std::shared_ptr<TF1> m_f_SysWPtV_BDTr_3j_bl = nullptr; // sgargiul 24.10.19
  std::shared_ptr<TF1> m_f_SysWPtV_BDTr_3j_cc = nullptr; // sgargiul 24.10.19

  std::shared_ptr<TF1> m_f_SysZPtV = nullptr;
  std::shared_ptr<TF1> m_f_SysZMbb = nullptr;

  std::shared_ptr<TF1> m_f_SysVVMbbME_WZlvqq_2j = nullptr; // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVMbbME_ZZvvqq_2j = nullptr; // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVMbbME_ZZllqq_2j = nullptr; // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVMbbME_WZlvqq_3j = nullptr; // VD
  std::shared_ptr<TF1> m_f_SysVVMbbME_ZZvvqq_3j = nullptr; // VD
  std::shared_ptr<TF1> m_f_SysVVMbbME_ZZllqq_3j = nullptr; // VD

  std::shared_ptr<TH1> m_h_SysVVMbbPSUE_WZlvqq_2j = nullptr; // NM 17/06/15
  std::shared_ptr<TH1> m_h_SysVVMbbPSUE_ZZvvqq_2j = nullptr; // NM 17/06/15
  std::shared_ptr<TH1> m_h_SysVVMbbPSUE_ZZllqq_2j = nullptr; // NM 17/06/15
  std::shared_ptr<TH1> m_h_SysVVMbbPSUE_WZlvqq_3j = nullptr; // NM 17/06/15
  std::shared_ptr<TH1> m_h_SysVVMbbPSUE_ZZvvqq_3j = nullptr; // NM 17/06/15
  std::shared_ptr<TH1> m_h_SysVVMbbPSUE_ZZllqq_3j = nullptr; // NM 17/06/15

  std::shared_ptr<TF1> m_f_SysVVPTVME_WZlvqq_2j = nullptr;   // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVPTVME_ZZvvqq_2j = nullptr;   // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVPTVME_ZZllqq_2j = nullptr;   // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVPTVPSUE_WZlvqq_2j = nullptr; // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVPTVPSUE_ZZvvqq_2j = nullptr; // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVPTVPSUE_ZZllqq_2j = nullptr; // francav 15.07.16

  std::shared_ptr<TF1> m_f_SysVVPTVME_WZlvqq_3j = nullptr;   // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVPTVME_ZZvvqq_3j = nullptr;   // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVPTVME_ZZllqq_3j = nullptr;   // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVPTVPSUE_WZlvqq_3j = nullptr; // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVPTVPSUE_ZZvvqq_3j = nullptr; // francav 15.07.16
  std::shared_ptr<TF1> m_f_SysVVPTVPSUE_ZZllqq_3j = nullptr; // francav 15.07.16

  std::shared_ptr<TH1> m_h_SysVVMJJMR_01L_M = nullptr; // bmoser 04.12.19
  std::shared_ptr<TH1> m_h_SysVVMJJMR_01L_H = nullptr; // bmoser 04.12.19
  std::shared_ptr<TH1> m_h_SysVVMJJMR_2L_M = nullptr;  // bmoser 04.12.19
  std::shared_ptr<TH1> m_h_SysVVMJJMR_2L_H = nullptr;  // bmoser 04.12.19

  std::shared_ptr<TF1> m_f_SysVHQCDscalePTV_WlvH_2j = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHQCDscalePTV_ZvvH_2j = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHQCDscalePTV_ZllH_2j = nullptr; // francav 19.07.16

  std::shared_ptr<TF1> m_f_SysVHQCDscalePTV_WlvH_3j = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHQCDscalePTV_ZvvH_3j = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHQCDscalePTV_ZllH_3j = nullptr; // francav 19.07.16

  std::shared_ptr<TF1> m_f_SysVHQCDscaleMbb_WlvH = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHQCDscaleMbb_ZvvH = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHQCDscaleMbb_ZllH = nullptr; // francav 19.07.16

  std::shared_ptr<TF1> m_f_SysVHPDFPTV_WlvH_2j = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHPDFPTV_ZvvH_2j = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHPDFPTV_ZllH_2j = nullptr; // francav 19.07.16

  std::shared_ptr<TF1> m_f_SysVHPDFPTV_WlvH_3j = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHPDFPTV_ZvvH_3j = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHPDFPTV_ZllH_3j = nullptr; // francav 19.07.16

  std::shared_ptr<TF1> m_f_SysVHUEPSPTV_WlvH_2j = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHUEPSPTV_ZllH_2j = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHUEPSPTV_ZvvH_2j = nullptr; // francav 19.07.16

  std::shared_ptr<TF1> m_f_SysVHUEPSPTV_WlvH_3j = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHUEPSPTV_ZllH_3j = nullptr; // francav 19.07.16
  std::shared_ptr<TF1> m_f_SysVHUEPSPTV_ZvvH_3j = nullptr; // francav 19.07.16

  std::shared_ptr<TH1> m_h_SysVHUEPSMbb = nullptr; // NM 17/06/15

  std::shared_ptr<TH1> m_h_SysVHMJJMR_01L_M = nullptr; // bmoser 04.12.19
  std::shared_ptr<TH1> m_h_SysVHMJJMR_01L_H = nullptr; // bmoser 04.12.19
  std::shared_ptr<TH1> m_h_SysVHMJJMR_2L_M = nullptr;  // bmoser 04.12.19
  std::shared_ptr<TH1> m_h_SysVHMJJMR_2L_H = nullptr;  // bmoser 04.12.19

  std::shared_ptr<TH1> m_h_SysMJVHPH7_0L_LP_M = nullptr;  // bmoser 03.01.20
  std::shared_ptr<TH1> m_h_SysMJVHPH7_0L_LP_H = nullptr;  // bmoser 03.01.20
  std::shared_ptr<TH1> m_h_SysMJVHPH7_1L_LP_M = nullptr;  // bmoser 03.01.20
  std::shared_ptr<TH1> m_h_SysMJVHPH7_1L_LP_H = nullptr;  // bmoser 03.01.20
  std::shared_ptr<TH1> m_h_SysMJVHPH7_01L_HP_M = nullptr; // bmoser 03.01.20
  std::shared_ptr<TH1> m_h_SysMJVHPH7_01L_HP_H = nullptr; // bmoser 03.01.20
  std::shared_ptr<TH1> m_h_SysMJVHPH7_2L_M = nullptr;     // bmoser 03.01.20
  std::shared_ptr<TH1> m_h_SysMJVHPH7_2L_H = nullptr;     // bmoser 03.01.20

  // Boosted VHbb
  // Z+jets MUR
  std::shared_ptr<TF1> m_f_SysZMbbBoosted_Zvv_HP_250_400 = nullptr;
  std::shared_ptr<TF1> m_f_SysZMbbBoosted_Zvv_HP_400 = nullptr;
  std::shared_ptr<TF1> m_f_SysZMbbBoosted_Zvv_LP_250_400 = nullptr;
  std::shared_ptr<TF1> m_f_SysZMbbBoosted_Zvv_LP_400 = nullptr;
  std::shared_ptr<TF1> m_f_SysZMbbBoosted_Zll_250_400 = nullptr;
  std::shared_ptr<TF1> m_f_SysZMbbBoosted_Zll_400 = nullptr; // weitao 20.11.19
  // W+jets MUR
  std::shared_ptr<TF1> m_f_SysWMbbBoosted_0L_HP_250_400 = nullptr;
  std::shared_ptr<TF1> m_f_SysWMbbBoosted_0L_HP_400 = nullptr;
  std::shared_ptr<TF1> m_f_SysWMbbBoosted_0L_LP_250_400 = nullptr;
  std::shared_ptr<TF1> m_f_SysWMbbBoosted_0L_LP_400 = nullptr;
  std::shared_ptr<TF1> m_f_SysWMbbBoosted_1L_HP_250_400 = nullptr;
  std::shared_ptr<TF1> m_f_SysWMbbBoosted_1L_HP_400 = nullptr;
  std::shared_ptr<TF1> m_f_SysWMbbBoosted_1L_LP_250_400 = nullptr;
  std::shared_ptr<TF1> m_f_SysWMbbBoosted_1L_LP_400 = nullptr; // weitao 20.11.19
  // Diboson MUR
  std::shared_ptr<TF1> m_f_SysVVMURMJBoosted = nullptr;
  // Diboson PwPy8 vs Sherpa
  std::shared_ptr<TF1> m_f_SysVVPP8MJBoosted_HP = nullptr;
  std::shared_ptr<TF1> m_f_SysVVPP8MJBoosted_LP = nullptr;

  //========================VHcc=================================

  // For 0 & 2-lepton
  std::shared_ptr<TF1>
      m_f_SysZhf_75_150ptv_shape_ShMGPy8_mCC = nullptr; // mironova @ 25/02/21

  std::shared_ptr<TF1>
      m_f_SysZmf_75_150ptv_shape_ShMGPy8_mCC = nullptr; // mironova @ 25/02/21

  std::shared_ptr<TF1>
      m_f_SysZhf_150ptv_shape_ShMGPy8_mCC = nullptr; // mironova @ 25/02/21

  std::shared_ptr<TF1>
      m_f_SysZmf_150ptv_shape_ShMGPy8_mCC = nullptr; // mironova @ 25/02/21

  std::shared_ptr<TF1>
      m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_mCC = nullptr; // mironova @ 25/02/21
  std::shared_ptr<TF1>
      m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_mCC = nullptr; // mironova @ 25/02/21
  std::shared_ptr<TF1>
      m_f_SysZl_2jet_250ptv_shape_ShMGPy8_mCC = nullptr; // mironova @ 25/02/21
  std::shared_ptr<TF1>
      m_f_SysZl_3jet_250ptv_shape_ShMGPy8_mCC = nullptr; // mironova @ 25/02/21
  std::shared_ptr<TF1>
      m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_mCC = nullptr; // mironova @ 25/02/21
  std::shared_ptr<TF1>
      m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_mCC = nullptr; // mironova @ 25/02/21

  // For 0 & 1 lepton
  std::shared_ptr<TF1> m_f_SysWhf_shape_ShMGPy8_mCC = nullptr; // gkoren @ 12/04/20

  std::shared_ptr<TF1> m_f_SysWmf_shape_ShMGPy8_mCC = nullptr; // gkoren @ 12/04/20

  std::shared_ptr<TF1>
      m_f_SysWl_150_250ptv_shape_ShMGPy8_mCC = nullptr; // mironova @ 25/02/21
  std::shared_ptr<TF1>
      m_f_SysWl_250ptv_shape_ShMGPy8_mCC = nullptr; // mironova @ 25/02/21

  // SR SYSTS - mironova
  // For 0 & 2-lepton
  std::shared_ptr<TF1>
      m_f_SysZhf_75_150ptv_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21

  std::shared_ptr<TF1>
      m_f_SysZmf_75_150ptv_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21

  std::shared_ptr<TF1>
      m_f_SysZhf_150ptv_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21

  std::shared_ptr<TF1>
      m_f_SysZmf_150ptv_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21

  std::shared_ptr<TF1>
      m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21
  std::shared_ptr<TF1>
      m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21
  std::shared_ptr<TF1>
      m_f_SysZl_2jet_250ptv_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21
  std::shared_ptr<TF1>
      m_f_SysZl_3jet_250ptv_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21
  std::shared_ptr<TF1>
      m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21
  std::shared_ptr<TF1>
      m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21

  // For 0 & 1 lepton
  std::shared_ptr<TF1> m_f_SysWhf_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21

  std::shared_ptr<TF1> m_f_SysWmf_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21

  std::shared_ptr<TF1>
      m_f_SysWl_150_250ptv_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21
  std::shared_ptr<TF1>
      m_f_SysWl_250ptv_shape_ShMGPy8_SR_mCC = nullptr; // mironova @ 25/02/21

  // mironova @ 11/04/20
  std::shared_ptr<TF1> m_f_SysTTbarlq_shape_mCC_Her_Pyt = nullptr;
  std::shared_ptr<TF1> m_f_SysTTbarlq_shape_mCC_Pow_aMC = nullptr;

  std::shared_ptr<TF1> m_f_SysTTbarlq_150_250ptv_shape_mCC_rHi = nullptr;
  std::shared_ptr<TF1> m_f_SysTTbarlq_250ptv_shape_mCC_rHi = nullptr;

  // mironova @ 11/04/20
  std::shared_ptr<TF1> m_f_SysTTbarbq_150_250ptv_shape_mCC_Her_Pyt = nullptr;
  std::shared_ptr<TF1> m_f_SysTTbarbq_250ptv_shape_mCC_Her_Pyt = nullptr;

  std::shared_ptr<TF1> m_f_SysTTbarbq_150_250ptv_shape_mCC_Pow_aMC = nullptr;
  std::shared_ptr<TF1> m_f_SysTTbarbq_250ptv_shape_mCC_Pow_aMC = nullptr;

  std::shared_ptr<TF1> m_f_SysTTbarbq_150_250ptv_shape_mCC_rHi = nullptr;
  std::shared_ptr<TF1> m_f_SysTTbarbq_250ptv_shape_mCC_rHi = nullptr;

  // mironova @ 11/04/20
  std::shared_ptr<TF1> m_f_SysStopWtlq_150_250ptv_shape_mCC_Her_Pyt = nullptr;
  std::shared_ptr<TF1> m_f_SysStopWtlq_250ptv_shape_mCC_Her_Pyt = nullptr;

  std::shared_ptr<TF1> m_f_SysStopWtlq_150_250ptv_shape_mCC_Pow_aMC = nullptr;
  std::shared_ptr<TF1> m_f_SysStopWtlq_250ptv_shape_mCC_Pow_aMC = nullptr;

  // mironova @ 11/04/20
  std::shared_ptr<TF1> m_f_SysStopWtbq_150_250ptv_shape_mCC_Her_Pyt = nullptr;
  std::shared_ptr<TF1> m_f_SysStopWtbq_250ptv_shape_mCC_Her_Pyt = nullptr;

  std::shared_ptr<TF1> m_f_SysStopWtbq_150_250ptv_shape_mCC_Pow_aMC = nullptr;
  std::shared_ptr<TF1> m_f_SysStopWtbq_250ptv_shape_mCC_Pow_aMC = nullptr;

  // mironova @ 05/05/20

  std::shared_ptr<TF1> m_f_SysZZcc_75_150ptv_shape_PwPy8_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysZZcc_150_250ptv_shape_PwPy8_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysZZcc_250ptv_shape_PwPy8_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysZZbkg_75_150ptv_shape_PwPy8_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysZZbkg_150_250ptv_shape_PwPy8_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysZZbkg_250ptv_shape_PwPy8_mCC = nullptr;

  std::shared_ptr<TF1> m_f_SysWWcl_150_250ptv_shape_PwPy8_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysWWcl_250ptv_shape_PwPy8_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysWWbkg_150_250ptv_shape_PwPy8_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysWWbkg_250ptv_shape_PwPy8_mCC = nullptr;

  std::shared_ptr<TF1> m_f_SysWZlephadcc_150_250ptv_shape_PwPy8_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysWZlephadcc_250ptv_shape_PwPy8_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysWZlephadbkg_150_250ptv_shape_PwPy8_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysWZlephadbkg_250ptv_shape_PwPy8_mCC = nullptr;

  std::shared_ptr<TF1> m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_0lep = nullptr;
  std::shared_ptr<TF1> m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_0lep = nullptr;
  std::shared_ptr<TF1> m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_0lep = nullptr;
  std::shared_ptr<TF1> m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_0lep = nullptr;

  std::shared_ptr<TF1> m_f_SysWZhadlepcl_75_150ptv_shape_PwPy8_mCC_2lep = nullptr;
  std::shared_ptr<TF1> m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_2lep = nullptr;
  std::shared_ptr<TF1> m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_2lep = nullptr;
  std::shared_ptr<TF1> m_f_SysWZhadlepbkg_75_150ptv_shape_PwPy8_mCC_2lep = nullptr;
  std::shared_ptr<TF1> m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_2lep = nullptr;
  std::shared_ptr<TF1> m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_2lep = nullptr;

  // mironova @ 13/06/20

  std::shared_ptr<TF1> m_f_SysqqZHcc_150_250ptv_shape_PwHwg7_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysqqZHcc_250ptv_shape_PwHwg7_mCC = nullptr;

  std::shared_ptr<TF1> m_f_SysggZHcc_150_250ptv_shape_PwHwg7_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysggZHcc_250ptv_shape_PwHwg7_mCC = nullptr;

  std::shared_ptr<TF1> m_f_SysqqWHcc_150_250ptv_shape_PwHwg7_mCC = nullptr;
  std::shared_ptr<TF1> m_f_SysqqWHcc_250ptv_shape_PwHwg7_mCC = nullptr;

  //============================================

  std::map<TString, std::shared_ptr<TObject>> m_allHists = {};

public:
  CorrsAndSysts(TString name, bool draw = true); // e.g OneLepton_8TeV

  inline void SetDebug(int i) { m_debug = i; }

  CAS::DetailEventType GetDetailEventType(TString name);
  CAS::EventType GetEventType(TString name);

  void WriteHistsToFile(TString fname);
  // all values (VpT, Mbb) in MeV !

  // Higgs pT reweighting (NLO EW corrections)
  float Get_HiggsNLOEWKCorrection(CAS::EventType type, float VpT);
  inline float Get_HiggsNLOEWKCorrection(TString evtType, float VpT) {
    return Get_HiggsNLOEWKCorrection(m_typeNames[evtType.Data()], VpT);
  }

  float Get_SystematicWeight(CAS::EventType type, float VpT, float Mbb,
                             float truthPt, int njet, int ntag,
                             CAS::Systematic sys = CAS::Nominal,
                             CAS::SysVar var = CAS::Up, bool print = false);

  // forge the normalized syste name from the enums
  inline std::string GetSystName(CAS::Systematic sys, CAS::SysVar var) {
    return m_systNames[sys] + m_varNames[var];
  }

  // inverse function
  void GetSystFromName(TString name, CAS::Systematic &sys, CAS::SysVar &var);

  // faster method, which does not allow to determine the direction
  void GetSystFromName(const TString &name, CAS::Systematic &sys);

}; // close CorrsAndSysts class

namespace Utils {

// utility: HistogramsFilling and Building
std::shared_ptr<TH1F>
BuildTH1(std::vector<Double_t> contents, TString hname, float min, float max,
         std::map<TString, std::shared_ptr<TObject>> &hists);
void FillTH1(std::vector<Float_t> contents, std::shared_ptr<TH1> h,
             std::map<TString, std::shared_ptr<TObject>> &hists);
void FillTH1(int len, Float_t *contents, std::shared_ptr<TH1> h,
             std::map<TString, std::shared_ptr<TObject>> &hists);
inline float GetScale(float value, std::shared_ptr<TH1> h);

template <typename T>
void SaveHist(std::shared_ptr<T> h,
              std::map<TString, std::shared_ptr<TObject>> &hists) {
  if (hists.find(h->GetName()) != hists.end()) {
    std::cout << "CorrsAndSysts::ERROR - non-unique name of histogram/function "
                 "is being used - please correct"
              << std::endl;
    exit(-1);
  }
  std::shared_ptr<TObject> objPtr = std::shared_ptr<TObject>(h);
  hists[h->GetName()] = objPtr;
}

void ArraySubstractOne(float *array, unsigned int length);

// map<K,V> => map<V,K>
template <typename T, typename U>
std::map<U, T> reverseMap(const std::map<T, U> &m_in);

// Implementation
//
template <typename T, typename U>
std::map<U, T> reverseMap(const std::map<T, U> &m_in) {
  typedef typename std::map<T, U>::const_iterator map_it;
  map_it it = m_in.begin();
  std::map<U, T> m_out;
  while (it != m_in.end()) {
    m_out[it->second] = it->first;
    it++;
  }
  return m_out;
}

// Template function for deleting pointers inside a map whereby the map has the
// format
// FORMAT ::     std::map< typename A, typename B > m_Map
//        Where:  A = any object used as a key
//                B = Pointer to object needed for deletion.
template <typename Iter> void DeleteMapPointers(Iter begin, Iter end) {
  // Loop through and delete pointed objects
  for (; begin != end; begin++) {
    delete begin->second;
  }
}

} // namespace Utils

#endif // CorrsAndSysts_HPP_
