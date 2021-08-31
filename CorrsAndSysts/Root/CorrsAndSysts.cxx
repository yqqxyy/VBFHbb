#include "CorrsAndSysts/CorrsAndSysts.h"
#include "TFile.h"
#include <algorithm>
#include <cmath>

#include "TSystem.h" //Inclusion for WorkDir_DIR env var
#include <TVector2.h>

/*
 * See header file for description
 */

CorrsAndSysts::CorrsAndSysts(TString name, bool draw)
{
  m_draw = draw;

  // set analysis basesd on contentes of name
  if (name.Contains("Zero") || name.Contains("zero") || name.Contains("ZERO")) {
    m_zero = true;
  }
  if (name.Contains("One") || name.Contains("one") || name.Contains("ONE")) {
    m_one = true;
  }
  if (name.Contains("Two") || name.Contains("two") || name.Contains("TWO")) {
    m_two = true;
  }

  // check lepton selection chosen correctly
  if ((m_zero && m_one) || (m_zero && m_two) || (m_one && m_two)) {
    std::cout << "CorrsAndSysts initialized for multiple analyses" << std::endl;
    exit(-1);
  }
  if (!m_zero && !m_one && !m_two) {
    std::cout << "CorrsAndSysts initialized for none of the analyses"
              << std::endl;
    exit(-1);
  }

  // write some stuff so users can be confident they did this correctly
  std::cout << "Initalize CorrsAndSysts for ";
  if (m_zero) {
    std::cout << "Zero";
  }
  if (m_one) {
    std::cout << "One";
  }
  if (m_two) {
    std::cout << "Two";
  }
  std::cout << " Lepton analysis" << std::endl;

  // initalize the corrections and systematics
  Initialize();

} // CorrsAndSysts

void CorrsAndSysts::Initialize() {

  // Extract the ROTCOREBIN compiled location
  m_WorkDir_DIR = gSystem->Getenv("WorkDir_DIR");
  // This cannot be set in a setter as things are setup only once here

  m_varNames[CAS::Up] = "Up";
  m_varNames[CAS::Do] = "Do";

  //============== Declare the use cases: =============================//

  // initialize the mappings to event types
  m_typeNames["WHlvbb"] = CAS::WHlvbb;
  m_typeNames["WlvH125"] = CAS::WHlvbb;
  m_typeNames["qqWlvH125"] = CAS::WHlvbb;
  m_typeNames["qqWminuseveHNLO125"] = CAS::WHlvbb;
  m_typeNames["qqWminusmuvmuHNLO125"] = CAS::WHlvbb;
  m_typeNames["qqWpluseveHNLO125"] = CAS::WHlvbb;
  m_typeNames["qqWplusmuvmuHNLO125"] = CAS::WHlvbb;
  m_typeNames["WlvH"] = CAS::WHlvbb;

  m_typeNames["qqZHllbb"] = CAS::qqZHllbb;
  m_typeNames["Zll125"] = CAS::qqZHllbb;

  m_typeNames["qqZllH125"] = CAS::qqZHllbb;
  m_typeNames["qqZllH"] = CAS::qqZHllbb;

  m_typeNames["qqZHvvbb"] = CAS::qqZHvvbb;
  m_typeNames["qqZvvH"] = CAS::qqZHvvbb;
  m_typeNames["qqZvvH125"] = CAS::qqZHvvbb;
  m_typeNames["ZvvH125"] = CAS::qqZHvvbb;

  m_typeNames["WHlvcc"] = CAS::WHlvcc;
  m_typeNames["qqZHllcc"] = CAS::qqZHllcc;
  m_typeNames["qqZHvvcc"] = CAS::qqZHvvcc;
  m_typeNames["ggZHllcc"] = CAS::ggZHllcc;
  m_typeNames["ggZHvvcc"] = CAS::ggZHvvcc;
  m_typeNames["ggZllH125cc"] = CAS::ggZHllcc;
  m_typeNames["ggZvvH125cc"] = CAS::ggZHvvcc;
  m_typeNames["qqWlvH125cc"] = CAS::WHlvcc;
  m_typeNames["qqZvvH125cc"] = CAS::qqZHvvcc;
  m_typeNames["qqZllH125cc"] = CAS::qqZHllcc;

  m_typeNames["ggZveveH125"] = CAS::ggZHvvbb;
  m_typeNames["ggZvmvmH125"] = CAS::ggZHvvbb;
  m_typeNames["ggZvtvtH125"] = CAS::ggZHvvbb;
  m_typeNames["ggZvvH125"] = CAS::ggZHvvbb;

  m_typeNames["ggZeeH125"] = CAS::ggZHllbb;
  m_typeNames["ggZmmH125"] = CAS::ggZHllbb;
  m_typeNames["ggZttH125"] = CAS::ggZHllbb;
  m_typeNames["ggZllH125"] = CAS::ggZHllbb;

  //** added by Lei: 2014jan31
  m_typeNames["ggZllH"] = CAS::ggZHllbb;
  m_typeNames["ggZvvH"] = CAS::ggZHvvbb;
  m_typeNames["ggZHllbb"] = CAS::ggZHllbb;
  m_typeNames["ggZHvvbb"] = CAS::ggZHvvbb;

  m_typeNames["Z"] = CAS::Z; // FLS
  m_typeNames["W"] = CAS::W; // FLS
  m_typeNames["Wb"] = CAS::Wb;
  m_typeNames["Wbb"] = CAS::Wbb;
  m_typeNames["Wbc"] = CAS::Wbc;
  m_typeNames["Wbl"] = CAS::Wbl;
  m_typeNames["Wc"] = CAS::Wc;
  m_typeNames["Wcl"] = CAS::Wcl;
  m_typeNames["Wcc"] = CAS::Wcc;
  m_typeNames["Wl"] = CAS::Wl;
  m_typeNames["Wll"] = CAS::Wl;

  m_typeNames["Zb"] = CAS::Zb;
  m_typeNames["Zbb"] = CAS::Zbb;
  m_typeNames["Zbc"] = CAS::Zbc;
  m_typeNames["Zbl"] = CAS::Zbl;
  m_typeNames["Zc"] = CAS::Zc;
  m_typeNames["Zcc"] = CAS::Zcc;
  m_typeNames["Zcl"] = CAS::Zcl;
  m_typeNames["Zl"] = CAS::Zl;
  m_typeNames["Zll"] = CAS::Zl;

  m_typeNames["top"] = CAS::ttbar;
  m_typeNames["ttbar"] = CAS::ttbar;
  m_typeNames["ttbarbc"] = CAS::ttbarbc;
  m_typeNames["ttbaroth"] = CAS::ttbaroth;
  m_typeNames["ttbarbq"] = CAS::ttbarbq;
  m_typeNames["ttbarlq"] = CAS::ttbarlq;
  m_typeNames["ttbarqq"] = CAS::ttbarqq;
  m_typeNames["TTbar"] = CAS::ttbar;

  m_typeNames["stop_Wt"] = CAS::stop_Wt;
  m_typeNames["stop_Wtbb"] = CAS::stop_Wtbb;
  m_typeNames["stop_Wtoth"] = CAS::stop_Wtoth;
  m_typeNames["stopWt"] = CAS::stop_Wt;
  m_typeNames["stop_Wtbq"] = CAS::stop_Wtbq;
  m_typeNames["stop_Wtlq"] = CAS::stop_Wtlq;
  m_typeNames["stopWtbb"] = CAS::stop_Wtbb;
  m_typeNames["stopWtbq"] = CAS::stop_Wtbq;
  m_typeNames["stopWtlq"] = CAS::stop_Wtlq;
  m_typeNames["stopWtoth"] = CAS::stop_Wtoth;

  m_typeNames["stops"] = CAS::stop_s;
  m_typeNames["stop_s"] = CAS::stop_s;

  m_typeNames["stopt"] = CAS::stop_t;
  m_typeNames["stop_t"] = CAS::stop_t;

  m_typeNames["WW"] = CAS::WW;
  m_typeNames["WZ"] = CAS::WZ;
  m_typeNames["ZZ"] = CAS::ZZ;
  m_typeNames["ZZcc"] = CAS::ZZcc;
  m_typeNames["ZZbkg"] = CAS::ZZbkg;
  m_typeNames["WWcl"] = CAS::WWcl;
  m_typeNames["WWbkg"] = CAS::WWbkg;
  m_typeNames["WZlephadcc"] = CAS::WZlephadcc;
  m_typeNames["WZlephadbkg"] = CAS::WZlephadbkg;
  m_typeNames["WZhadlepcl"] = CAS::WZhadlepcl;
  m_typeNames["WZhadlepbkg"] = CAS::WZhadlepbkg;

  m_typeNames["multijet"] = CAS::multijet;
  m_typeNames["dijetJZW"] = CAS::multijet;
  m_typeNames["dijetZJ"] = CAS::multijet;

  //========================================================================//

  m_detailtypeNames["Sherpa_CT10_WpqqWmlv"] = CAS::WWincl;
  m_detailtypeNames["Sherpa_CT10_WplvWmqq"] = CAS::WWincl;
  m_detailtypeNames["Sherpa_CT10_WlvZqq"] = CAS::WlnuZhad;
  m_detailtypeNames["Sherpa_CT10_WqqZll"] = CAS::WhadZll;
  m_detailtypeNames["Sherpa_CT10_WqqZvv"] = CAS::WhadZnunu;
  m_detailtypeNames["Sherpa_CT10_ZqqZvv"] = CAS::ZnunuZhad;
  m_detailtypeNames["Sherpa_CT10_ZqqZll"] = CAS::ZllZhad;

  m_detailtypeNames["NODETAILNAME"] = CAS::NODETAILNAME;

  //=============================================================================

  m_systNames[CAS::Nominal] = "";
  m_systNames[CAS::SysTTbarPtV_BDTr] =
      "SysTTbarPtV_BDTr"; // kalkhour 14.11.19 //pTV-reweighting systematic.
                          // Derived on reco; used to reweight nominal ttbar
                          // PowhegPythia8 before training in the BDTr approach
  m_systNames[CAS::SysTTbarbcMEACC] = "SysTTbarbcMEACC";
  m_systNames[CAS::SysTTbarbcPSACC] = "SysTTbarbcPSACC";
  m_systNames[CAS::SysTTbarOthMEACC] = "SysTTbarOthMEACC";
  m_systNames[CAS::SysTTbarOthPSACC] = "SysTTbarOthPSACC";
  m_systNames[CAS::SysStoptPTV] = "SysStoptPTV";
  m_systNames[CAS::SysStoptMBB] = "SysStoptMBB";
  m_systNames[CAS::SysStopWtPTV] = "SysStopWtPTV";
  m_systNames[CAS::SysStopWtMBB] = "SysStopWtMBB";
  m_systNames[CAS::SysStopWtbbACC] = "SysStopWtbbACC";
  m_systNames[CAS::SysStopWtothACC] = "SysStopWtothACC";

  m_systNames[CAS::SysTTbarMJISR] = "SysTTbarMJISR"; // mfoti 20.11.19
  m_systNames[CAS::SysTTbarMJPS] = "SysTTbarMJPS";   // mfoti 21.11.19
  m_systNames[CAS::SysStopWtDSDR] = "SysStopWtDSDR"; // gdigrego 19.11.19

  m_systNames[CAS::SysWPtV_BDTr] =
      "SysWPtV_BDTr"; // sgargiul 24.10.19 //pTV-reweighting systematic.
                      // Derived on reco; used to reweight nominal W+jets
                      // Sherpa before training in the BDTr approach

  m_systNames[CAS::SysZPtV] = "SysZPtV";
  m_systNames[CAS::SysZMbb] = "SysZMbb";

  m_systNames[CAS::SysVVMbbME] = "SysVVMbbME";     // francav 15.07.16
  m_systNames[CAS::SysVVPTVME] = "SysVVPTVME";     // francav 15.07.16
  m_systNames[CAS::SysVVMbbPSUE] = "SysVVMbbPSUE"; // francav 15.07.16
  m_systNames[CAS::SysVVPTVPSUE] = "SysVVPTVPSUE"; // francav 15.07.16

  m_systNames[CAS::SysVHQCDscalePTV] = "SysVHQCDscalePTV"; // francav 19.07.16
  m_systNames[CAS::SysVHQCDscaleMbb] = "SysVHQCDscaleMbb"; // francav 19.07.16
  m_systNames[CAS::SysVHPDFPTV] = "SysVHPDFPTV";           // francav 19.07.16
  m_systNames[CAS::SysVHQCDscalePTV_ggZH] =
      "SysVHQCDscalePTV_ggZH"; // francav 19.07.16
  m_systNames[CAS::SysVHQCDscaleMbb_ggZH] =
      "SysVHQCDscaleMbb_ggZH";                             // francav 19.07.16
  m_systNames[CAS::SysVHPDFPTV_ggZH] = "SysVHPDFPTV_ggZH"; // francav 19.07.16
  m_systNames[CAS::SysVHUEPSPTV] = "SysVHUEPSPTV";         // francav 19.07.16
  m_systNames[CAS::SysVHUEPSMbb] = "SysVHUEPSMbb";         // francav 19.07.16
  m_systNames[CAS::SysVHNLOEWK] = "SysVHNLOEWK";           // francav 19.07.16

  m_systNames[CAS::SysMJJMR] = "SysMJJMR";     // bmoser 04.12.19
  m_systNames[CAS::SysMJVHPH7] = "SysMJVHPH7"; // bmoser 03.01.20

  // Boosted VHbb
  m_systNames[CAS::SysZMbbBoosted] = "SysZMbbBoosted";
  m_systNames[CAS::SysWMbbBoosted] = "SysWMbbBoosted";
  m_systNames[CAS::SysVVMURMJBoosted] = "SysVVMURMJBoosted";
  m_systNames[CAS::SysVVPP8MJBoosted] = "SysVVPP8MJBoosted";

  //---------------VHcc------------------

  // Zhf 0 & 2-lepton - amendesj @ 20/04/2020
  m_systNames[CAS::SysZhf_shape_ShMGPy8_mCC] = "SysZhf_shape_ShMGPy8_mCC";

  // Zmf 0 & 2-lepton - amendesj @ 20/04/2020
  m_systNames[CAS::SysZmf_shape_ShMGPy8_mCC] = "SysZmf_shape_ShMGPy8_mCC";

  // Zl 0 & 2-lepton - amendesj @ 20/04/2020
  m_systNames[CAS::SysZl_shape_ShMGPy8_mCC] = "SysZl_shape_ShMGPy8_mCC";

  // Whf 0 & 1-lepton - gkoren @ 20/04/2020
  m_systNames[CAS::SysWhf_shape_ShMGPy8_mCC] = "SysWhf_shape_ShMGPy8_mCC";

  // Wmf 0 & 1-lepton - gkoren @ 20/04/2020
  m_systNames[CAS::SysWmf_shape_ShMGPy8_mCC] = "SysWmf_shape_ShMGPy8_mCC";

  // Wl 0 & 1-lepton - gkoren @ 20/04/2020
  m_systNames[CAS::SysWl_shape_ShMGPy8_mCC] = "SysWl_shape_ShMGPy8_mCC";

  // SR shapes - mironova
  // Zhf 0 & 2-lepton - mironova @ 25/02/21
  m_systNames[CAS::SysZhf_shape_ShMGPy8_SR_mCC] = "SysZhf_shape_ShMGPy8_SR_mCC";

  // Zmf 0 & 2-lepton - mironova @ 25/02/21
  m_systNames[CAS::SysZmf_shape_ShMGPy8_SR_mCC] = "SysZmf_shape_ShMGPy8_SR_mCC";

  // Zl 0 & 2-lepton - mironova @ 25/02/21
  m_systNames[CAS::SysZl_shape_ShMGPy8_SR_mCC] = "SysZl_shape_ShMGPy8_SR_mCC";

  // Whf 0 & 1-lepton - mironova @ 25/02/21
  m_systNames[CAS::SysWhf_shape_ShMGPy8_SR_mCC] = "SysWhf_shape_ShMGPy8_SR_mCC";

  // Wmf 0 & 1-lepton - mironova @ 25/02/21
  m_systNames[CAS::SysWmf_shape_ShMGPy8_SR_mCC] = "SysWmf_shape_ShMGPy8_SR_mCC";

  // Wl 0 & 1-lepton - mironova @ 25/02/21
  m_systNames[CAS::SysWl_shape_ShMGPy8_SR_mCC] = "SysWl_shape_ShMGPy8_SR_mCC";

  // ttbarbq
  m_systNames[CAS::SysTTbarbq_shape_mCC_Pow_aMC] =
      "SysTTbarbq_shape_mCC_Pow_aMC"; // mironova @ 30/04/20
  m_systNames[CAS::SysTTbarbq_shape_mCC_Her_Pyt] =
      "SysTTbarbq_shape_mCC_Her_Pyt"; // mironova @ 30/04/20
  m_systNames[CAS::SysTTbarbq_shape_mCC_rHi] =
      "SysTTbarbq_shape_mCC_rHi"; // mironova @ 30/04/202

  // ttbarlq
  m_systNames[CAS::SysTTbarlq_shape_mCC_Pow_aMC] =
      "SysTTbarlq_shape_mCC_Pow_aMC"; // mironova @ 30/04/20
  m_systNames[CAS::SysTTbarlq_shape_mCC_Her_Pyt] =
      "SysTTbarlq_shape_mCC_Her_Pyt"; // mironova @ 30/04/20
  m_systNames[CAS::SysTTbarlq_shape_mCC_rHi] =
      "SysTTbarlq_shape_mCC_rHi"; // mironova @ 30/04/202

  // stopWtbq
  m_systNames[CAS::SysStopWtbq_shape_mCC_Pow_aMC] =
      "SysStopWtbq_shape_mCC_Pow_aMC"; // mironova @ 30/04/20
  m_systNames[CAS::SysStopWtbq_shape_mCC_Her_Pyt] =
      "SysStopWtbq_shape_mCC_Her_Pyt"; // mironova @ 30/04/20

  // stopWtlq
  m_systNames[CAS::SysStopWtlq_shape_mCC_Pow_aMC] =
      "SysStopWtlq_shape_mCC_Pow_aMC"; // mironova @ 30/04/20
  m_systNames[CAS::SysStopWtlq_shape_mCC_Her_Pyt] =
      "SysStopWtlq_shape_mCC_Her_Pyt"; // mironova @ 30/04/20

  // Diboson - mironova @ 05/05/20
  m_systNames[CAS::SysZZcc_shape_PwPy8_mCC] = "SysZZcc_shape_PwPy8_mCC";
  m_systNames[CAS::SysZZbkg_shape_PwPy8_mCC] = "SysZZbkg_shape_PwPy8_mCC";
  m_systNames[CAS::SysWWcl_shape_PwPy8_mCC] = "SysWWcl_shape_PwPy8_mCC";
  m_systNames[CAS::SysWWbkg_shape_PwPy8_mCC] = "SysWWbkg_shape_PwPy8_mCC";

  m_systNames[CAS::SysWZlephadcc_shape_PwPy8_mCC] =
      "SysWZlephadcc_shape_PwPy8_mCC";
  m_systNames[CAS::SysWZlephadbkg_shape_PwPy8_mCC] =
      "SysWZlephadbkg_shape_PwPy8_mCC";
  m_systNames[CAS::SysWZhadlepcl_shape_PwPy8_mCC] =
      "SysWZhadlepcl_shape_PwPy8_mCC";
  m_systNames[CAS::SysWZhadlepbkg_shape_PwPy8_mCC] =
      "SysWZhadlepbkg_shape_PwPy8_mCC";

  // VHcc - mironova @ 13/06/2020

  m_systNames[CAS::SysqqZHcc_shape_PwHwg7_mCC] = "SysqqZHcc_shape_PwHwg7_mCC";
  m_systNames[CAS::SysggZHcc_shape_PwHwg7_mCC] = "SysggZHcc_shape_PwHwg7_mCC";
  m_systNames[CAS::SysqqWHcc_shape_PwHwg7_mCC] = "SysqqWHcc_shape_PwHwg7_mCC";

  //================== Compute the reverse Maps ===========================//

  m_systFromNames =
      Utils::reverseMap<CAS::Systematic, std::string>(m_systNames);
  m_varFromNames = Utils::reverseMap<CAS::SysVar, std::string>(m_varNames);

  //================== End of Systs declaration ===========================//

  /*********************************************
   *
   *    CORRECTIONS
   *    These should be applied to the nominal
   *
   *    Below the histograms which contain the
   *    values of the correction are created
   *
   *********************************************/

  // signal NLO EW corrections
  m_h_WHlvbbNLOEWKCorrection =
      std::make_shared<TH1F>("WHlvbbpTCorr", "WHlvbbpTCorr", 95, 25.e3, 500.e3);
  m_h_WHlvbbNLOEWKCorrection->SetDirectory(0);
  m_h_ZHllbbNLOEWKCorrection =
      std::make_shared<TH1F>("ZHllbbpTCorr", "ZHllbbpTCorr", 95, 25.e3, 500.e3);
  m_h_ZHllbbNLOEWKCorrection->SetDirectory(0);
  m_h_ZHvvbbNLOEWKCorrection =
      std::make_shared<TH1F>("ZHvvbbpTCorr", "ZHvvbbpTCorr", 95, 25.e3, 500.e3);
  m_h_ZHvvbbNLOEWKCorrection->SetDirectory(0);

  Float_t a_whlvbbcorr[95] = {
      0.0305203,   0.0275837,   0.0239527,   0.0215937,  0.0170362,
      0.015103,    0.011289,    0.00953566,  0.00753487, 0.00497013,
      0.00211544,  -0.00118702, -0.00458593, -0.0044137, -0.00812502,
      -0.00802924, -0.0111074,  -0.0111592,  -0.0156807, -0.0176686,
      -0.0203234,  -0.0207377,  -0.0198427,  -0.0272455, -0.0255043,
      -0.0314772,  -0.0332882,  -0.0279931,  -0.0353625, -0.0363241,
      -0.0380089,  -0.038475,   -0.0418936,  -0.0490911, -0.0491424,
      -0.047753,   -0.055085,   -0.0538532,  -0.0635846, -0.0489332,
      -0.0591391,  -0.0654692,  -0.0667212,  -0.0739219, -0.0660949,
      -0.068587,   -0.0652828,  -0.0752986,  -0.0768462, -0.0739386,
      -0.088597,   -0.0880649,  -0.0800617,  -0.0899523, -0.081103,
      -0.0913021,  -0.0898038,  -0.102608,   -0.0973754, -0.101796,
      -0.0866853,  -0.112784,   -0.0972057,  -0.12284,   -0.143297,
      -0.126077,   -0.0955431,  -0.131121,   -0.120541,  -0.116152,
      -0.125354,   -0.12976,    -0.135101,   -0.12671,   -0.1366,
      -0.132531,   -0.136987,   -0.159221,   -0.149076,  -0.162372,
      -0.134163,   -0.152301,   -0.154554,   -0.16334,   -0.174772,
      -0.140081,   -0.139006,   -0.186223,   -0.139392,  -0.164362,
      -0.150565,   -0.172819,   -0.163485,   -0.182222,  -0.161225};
  Utils::FillTH1(95, a_whlvbbcorr, m_h_WHlvbbNLOEWKCorrection, m_allHists);
  Float_t a_zhllbbcorr[95] = {
      0.0180518,  0.0175002,  0.0132077,  0.0116164,  0.0122393,  0.00626285,
      0.00409211, 0.00514318, 0.0013087,  0.0006585,  -0.0028916, -0.0024331,
      -0.0050633, -0.0070058, -0.0024234, -0.0077263, -0.0099203, -0.0079423,
      -0.0024083, -0.0079295, -0.0140968, -0.0084094, -0.0091692, 0.0452771,
      -0.0587156, -0.0069676, -0.0047452, -0.0114475, -0.0114,    -0.0165374,
      -0.0013505, -0.0192127, -0.0103618, -0.0198998, -0.0228517, -0.0141691,
      -0.0247783, -0.0313323, -0.0254595, -0.0212254, -0.0351162, -0.033811,
      -0.0392759, -0.0285445, -0.0411198, -0.0374336, -0.0600432, -0.0387952,
      -0.0455118, -0.0681619, -0.0451166, -0.0476096, -0.0491737, -0.0710518,
      -0.0556638, -0.088824,  -0.0299995, -0.0827677, -0.0545532, -0.0744806,
      -0.0811843, -0.08833,   -0.064955,  -0.090745,  -0.0534193, -0.113034,
      -0.0849838, -0.114451,  -0.0691412, -0.099921,  -0.140833,  -0.0594616,
      -0.111809,  -0.11322,   -0.092648,  -0.121806,  -0.108687,  -0.095612,
      -0.161716,  -0.0803666, -0.140912,  -0.090183,  -0.160523,  -0.0537544,
      -0.21176,   -0.141927,  -0.105494,  -0.111952,  -0.167416,  -0.143798,
      -0.138735,  -0.143633,  -0.149896,  -0.111595,  -0.163634};
  Utils::FillTH1(95, a_zhllbbcorr, m_h_ZHllbbNLOEWKCorrection, m_allHists);
  Float_t a_zhvvbbcorr[95] = {
      0.0065368,    0.0050353,   0.0034537,    0.0039563,    0.0015541,
      0.0013437,    0.0013105,   0.000558902,  -0.000311199, 0.000118099,
      -0.0012695,   -0.0017933,  -0.0018836,   -0.0012507,   -0.0012401,
      -0.000592001, -0.0010327,  -0.000670198, -4.30009e-05, 0.0011333,
      0.0019119,    0.0040166,   0.0046145,    0.0038051,    0.0038172,
      0.0044688,    0.0048526,   0.0036004,    0.0019185,    0.0044529,
      0.0011151,    6.75992e-05, -0.0017795,   -0.0021627,   -0.0040829,
      -0.0029387,   -0.0061306,  -0.0082927,   -0.0075839,   -0.0130828,
      -0.0135549,   -0.0142077,  -0.0157659,   -0.0162739,   -0.0178021,
      -0.0197896,   -0.0221515,  -0.0235168,   -0.0289907,   -0.032802,
      -0.0301374,   -0.0312488,  -0.0334821,   -0.0356457,   -0.0365235,
      -0.040367,    -0.0513222,  -0.036924,    -0.0485935,   -0.0511039,
      -0.0501326,   -0.0534965,  -0.0633023,   -0.0537103,   -0.0610578,
      -0.062637,    -0.0618681,  -0.070481,    -0.0718773,   -0.068029,
      -0.0700012,   -0.0731712,  -0.0752671,   -0.0903928,   -0.079786,
      -0.0878243,   -0.0863028,  -0.0962839,   -0.0878311,   -0.0997877,
      -0.0959608,   -0.0928944,  -0.0908455,   -0.0945197,   -0.0934438,
      -0.0951179,   -0.109012,   -0.106231,    -0.100531,    -0.10736,
      -0.129367,    -0.115728,   -0.116603,    -0.11411,     -0.130529};
  Utils::FillTH1(95, a_zhvvbbcorr, m_h_ZHvvbbNLOEWKCorrection, m_allHists);

  /*********************************************
   *
   *    SYSTEMATICS
   *    These should be applied to the nominal
   *
   *    Below the histograms which contain the
   *    values of the correction are created
   *
   *********************************************/

  // VH ptV and mBB shape systematics
  // https://indico.cern.ch/event/540849/contributions/2249261/attachments/1311585/1962817/VHTheorySyst.pdf
  // Added by Paolo

  // updated for summer 2017 paper
  // based on studies by Chloe Gray
  // added by Elisabeth

  //=============VHbb Signal: pTV QCDscale  ==============================//

  m_f_SysVHQCDscalePTV_WlvH_2j = std::make_shared<TF1>(
      "WlvHbb_muR2p0_muF0p5_pTV2j_fit", "[0]+[1]*x/1e3 -1", 25, 500);
  m_f_SysVHQCDscalePTV_WlvH_2j->SetParameter(0, 0.95227);
  m_f_SysVHQCDscalePTV_WlvH_2j->SetParameter(1, 0.00021);
  Utils::SaveHist(m_f_SysVHQCDscalePTV_WlvH_2j, m_allHists);

  m_f_SysVHQCDscalePTV_ZvvH_2j = std::make_shared<TF1>(
      "ZvvHbb_muR2p0_muF0p5_pTV2j_fit", "[0]+[1]*x/1e3 -1", 25, 500);
  m_f_SysVHQCDscalePTV_ZvvH_2j->SetParameter(0, 0.95227);
  m_f_SysVHQCDscalePTV_ZvvH_2j->SetParameter(1, 0.00021);
  Utils::SaveHist(m_f_SysVHQCDscalePTV_ZvvH_2j, m_allHists);

  m_f_SysVHQCDscalePTV_ZllH_2j = std::make_shared<TF1>(
      "ZllHbb_muR2p0_muF0p5_pTV2j_fit", "[0]+[1]*x/1e3 -1", 25, 500);
  m_f_SysVHQCDscalePTV_ZllH_2j->SetParameter(0, 0.96631);
  m_f_SysVHQCDscalePTV_ZllH_2j->SetParameter(1, 0.00025);
  Utils::SaveHist(m_f_SysVHQCDscalePTV_ZllH_2j, m_allHists);

  //===========================  3-jet  ===================================//

  m_f_SysVHQCDscalePTV_WlvH_3j = std::make_shared<TF1>(
      "WlvHbb_muR2p0_muF0p5_pTV3j_fit", "[0]+[1]*x/1e3 -1", 25, 500);
  m_f_SysVHQCDscalePTV_WlvH_3j->SetParameter(0, 0.95227);
  m_f_SysVHQCDscalePTV_WlvH_3j->SetParameter(1, 0.00021);
  Utils::SaveHist(m_f_SysVHQCDscalePTV_WlvH_3j, m_allHists);

  m_f_SysVHQCDscalePTV_ZvvH_3j = std::make_shared<TF1>(
      "ZvvHbb_muR2p0_muF0p5_pTV3j_fit", "[0]+[1]*x/1e3 -1", 25, 500);
  m_f_SysVHQCDscalePTV_ZvvH_3j->SetParameter(0, 0.95227);
  m_f_SysVHQCDscalePTV_ZvvH_3j->SetParameter(1, 0.00021);
  Utils::SaveHist(m_f_SysVHQCDscalePTV_ZvvH_3j, m_allHists);

  m_f_SysVHQCDscalePTV_ZllH_3j = std::make_shared<TF1>(
      "ZllHbb_muR2p0_muF0p5_pTV3j_fit", "[0]+[1]*x/1e3 -1", 25, 500);
  m_f_SysVHQCDscalePTV_ZllH_3j->SetParameter(0, 0.96631);
  m_f_SysVHQCDscalePTV_ZllH_3j->SetParameter(1, 0.00025);
  Utils::SaveHist(m_f_SysVHQCDscalePTV_ZllH_3j, m_allHists);

  //=============VHbb Signal: mBB QCDscale ==========================//

  m_f_SysVHQCDscaleMbb_WlvH =
      std::make_shared<TF1>("WlvHbb_muR0p5_muF0p5_mBB_fit",
                            "[0]+[1]*(x/1e3)+[2]*(x/1e3)*(x/1e3) -1", 25, 500);
  m_f_SysVHQCDscaleMbb_WlvH->SetParameter(0, 0.94139);
  m_f_SysVHQCDscaleMbb_WlvH->SetParameter(1, 0.00148);
  m_f_SysVHQCDscaleMbb_WlvH->SetParameter(2, -8.40307603e-06);
  Utils::SaveHist(m_f_SysVHQCDscaleMbb_WlvH, m_allHists);

  m_f_SysVHQCDscaleMbb_ZvvH =
      std::make_shared<TF1>("ZvvHbb_muR0p5_muF0p5_mBB_fit",
                            "[0]+[1]*(x/1e3)+[2]*(x/1e3)*(x/1e3) -1", 25, 500);
  m_f_SysVHQCDscaleMbb_ZvvH->SetParameter(0, 0.94139);
  m_f_SysVHQCDscaleMbb_ZvvH->SetParameter(1, 0.00148);
  m_f_SysVHQCDscaleMbb_ZvvH->SetParameter(2, -8.40307603e-06);
  Utils::SaveHist(m_f_SysVHQCDscaleMbb_ZvvH, m_allHists);

  m_f_SysVHQCDscaleMbb_ZllH =
      std::make_shared<TF1>("ZllHbb_muR0p5_muF0p5_mBB_fit",
                            "[0]+[1]*(x/1e3)+[2]*(x/1e3)*(x/1e3) -1", 25, 500);
  m_f_SysVHQCDscaleMbb_ZllH->SetParameter(0, 0.94139);
  m_f_SysVHQCDscaleMbb_ZllH->SetParameter(1, 0.00148);
  m_f_SysVHQCDscaleMbb_ZllH->SetParameter(2, -8.40307603e-06);
  Utils::SaveHist(m_f_SysVHQCDscaleMbb_ZllH, m_allHists);

  //=============VHbb Signal: Theory PDF pTV
  //===================================//

  m_f_SysVHPDFPTV_WlvH_2j = std::make_shared<TF1>("WlvHbb_Envelop_pTV2j_fit",
                                                  "[0]*x/1e3+[1] -1", 25, 500);
  m_f_SysVHPDFPTV_WlvH_2j->SetParameter(0, 4.217549e-05);
  m_f_SysVHPDFPTV_WlvH_2j->SetParameter(1, 0.99059);
  Utils::SaveHist(m_f_SysVHPDFPTV_WlvH_2j, m_allHists);

  m_f_SysVHPDFPTV_ZvvH_2j = std::make_shared<TF1>("ZvvHbb_Envelop_pTV2j_fit",
                                                  "[0]*x/1e3+[1] -1", 25, 500);
  m_f_SysVHPDFPTV_ZvvH_2j->SetParameter(0, 4.217549e-05);
  m_f_SysVHPDFPTV_ZvvH_2j->SetParameter(1, 0.99059);
  Utils::SaveHist(m_f_SysVHPDFPTV_ZvvH_2j, m_allHists);

  m_f_SysVHPDFPTV_ZllH_2j = std::make_shared<TF1>("ZllHbb_Envelop_pTV2j_fit",
                                                  "[0]*x/1e3+[1] -1", 25, 500);
  m_f_SysVHPDFPTV_ZllH_2j->SetParameter(0, 4.217549e-05);
  m_f_SysVHPDFPTV_ZllH_2j->SetParameter(1, 0.99059);
  Utils::SaveHist(m_f_SysVHPDFPTV_ZllH_2j, m_allHists);

  //===========================  3-jet  ===================================//

  m_f_SysVHPDFPTV_WlvH_3j = std::make_shared<TF1>("WlvHbb_Envelop_pTV3j_fit",
                                                  "[0]*x/1e3+[1] -1", 25, 500);
  m_f_SysVHPDFPTV_WlvH_3j->SetParameter(0, 4.217549e-05);
  m_f_SysVHPDFPTV_WlvH_3j->SetParameter(1, 0.99059);
  Utils::SaveHist(m_f_SysVHPDFPTV_WlvH_3j, m_allHists);

  m_f_SysVHPDFPTV_ZvvH_3j = std::make_shared<TF1>("ZvvHbb_Envelop_pTV3j_fit",
                                                  "[0]*x/1e3+[1] -1", 25, 500);
  m_f_SysVHPDFPTV_ZvvH_3j->SetParameter(0, 4.217549e-05);
  m_f_SysVHPDFPTV_ZvvH_3j->SetParameter(1, 0.99059);
  Utils::SaveHist(m_f_SysVHPDFPTV_ZvvH_3j, m_allHists);

  m_f_SysVHPDFPTV_ZllH_3j = std::make_shared<TF1>("ZllHbb_Envelop_pTV3j_fit",
                                                  "[0]*x/1e3+[1] -1", 25, 500);
  m_f_SysVHPDFPTV_ZllH_3j->SetParameter(0, 4.217549e-05);
  m_f_SysVHPDFPTV_ZllH_3j->SetParameter(1, 0.99059);
  Utils::SaveHist(m_f_SysVHPDFPTV_ZllH_3j, m_allHists);

  //=============VHbb Signal: Theory UE/PS ===================================//

  m_f_SysVHUEPSPTV_WlvH_2j =
      std::make_shared<TF1>("WlvHbb_2jet_var2Do", "[0]*x/1e3+[1] -1", 25, 500);
  m_f_SysVHUEPSPTV_WlvH_2j->SetParameter(0, -0.000327);
  m_f_SysVHUEPSPTV_WlvH_2j->SetParameter(1, 1.07126);
  Utils::SaveHist(m_f_SysVHUEPSPTV_WlvH_2j, m_allHists);

  m_f_SysVHUEPSPTV_ZvvH_2j =
      std::make_shared<TF1>("ZvvHbb_2jet_var2Do", "[0]*x/1e3+[1] -1", 25, 500);
  m_f_SysVHUEPSPTV_ZvvH_2j->SetParameter(0, -0.000327);
  m_f_SysVHUEPSPTV_ZvvH_2j->SetParameter(1, 1.07126);
  Utils::SaveHist(m_f_SysVHUEPSPTV_ZvvH_2j, m_allHists);

  m_f_SysVHUEPSPTV_ZllH_2j =
      std::make_shared<TF1>("ZllHbb_2pjet_var2Do", "[0]*x/1e3+[1] -1", 25, 500);
  m_f_SysVHUEPSPTV_ZllH_2j->SetParameter(0, -0.000235);
  m_f_SysVHUEPSPTV_ZllH_2j->SetParameter(1, 1.03342);
  Utils::SaveHist(m_f_SysVHUEPSPTV_ZllH_2j, m_allHists);

  //===========================  3-jet  ===================================//

  m_f_SysVHUEPSPTV_WlvH_3j =
      std::make_shared<TF1>("WlvHbb_3jet_var2Up", "[0]*x/1e3+[1] -1", 25, 500);
  m_f_SysVHUEPSPTV_WlvH_3j->SetParameter(0, -0.000327);
  m_f_SysVHUEPSPTV_WlvH_3j->SetParameter(1, 1.07126);
  Utils::SaveHist(m_f_SysVHUEPSPTV_WlvH_3j, m_allHists);

  m_f_SysVHUEPSPTV_ZvvH_3j =
      std::make_shared<TF1>("ZvvHbb_3jet_var2Do", "[0]*x/1e3+[1] -1", 25, 500);
  m_f_SysVHUEPSPTV_ZvvH_3j->SetParameter(0, -0.000327);
  m_f_SysVHUEPSPTV_ZvvH_3j->SetParameter(1, 1.07126);
  Utils::SaveHist(m_f_SysVHUEPSPTV_ZvvH_3j, m_allHists);

  m_f_SysVHUEPSPTV_ZllH_3j = std::make_shared<TF1>("ZllHbb_2pjet_var2Do_3j",
                                                   "[0]*x/1e3+[1] -1", 25, 500);
  m_f_SysVHUEPSPTV_ZllH_3j->SetParameter(0, -0.000235);
  m_f_SysVHUEPSPTV_ZllH_3j->SetParameter(1, 1.03342);
  Utils::SaveHist(m_f_SysVHUEPSPTV_ZllH_3j, m_allHists);

  // ---------------------------VHcc------------------------------------//

  //======================= Z+jet: VHcc  ==============================//
  // Zhf = {Zcc,Zbb}
  // mironova @ 25/02/21
  m_f_SysZhf_150ptv_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysZhf_150ptv_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3) -1", 0, 500e3);
  m_f_SysZhf_150ptv_shape_ShMGPy8_mCC->SetParameter(0, 0.656152);
  m_f_SysZhf_150ptv_shape_ShMGPy8_mCC->SetParameter(1, 0.000898941);
  m_f_SysZhf_150ptv_shape_ShMGPy8_mCC->SetParameter(2, 1.13568e-05);
  m_f_SysZhf_150ptv_shape_ShMGPy8_mCC->SetParameter(3, -3.22445e-08);
  Utils::SaveHist(m_f_SysZhf_150ptv_shape_ShMGPy8_mCC, m_allHists);

  // Zmf = {Zcl,Zbl,Zbc}
  // mironova @ 25/02/21
  m_f_SysZmf_150ptv_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysZmf_150ptv_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3)-1", 0, 500e3);
  m_f_SysZmf_150ptv_shape_ShMGPy8_mCC->SetParameter(0, 0.812273);
  m_f_SysZmf_150ptv_shape_ShMGPy8_mCC->SetParameter(1, -0.00127381);
  m_f_SysZmf_150ptv_shape_ShMGPy8_mCC->SetParameter(2, 1.87841e-05);
  m_f_SysZmf_150ptv_shape_ShMGPy8_mCC->SetParameter(3, -3.48646e-08);
  Utils::SaveHist(m_f_SysZmf_150ptv_shape_ShMGPy8_mCC, m_allHists);

  // Zhf = {Zcc,Zbb}
  // mironova @ 25/02/21
  m_f_SysZhf_75_150ptv_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysZhf_75_150ptv_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3) -1", 0, 500e3);
  m_f_SysZhf_75_150ptv_shape_ShMGPy8_mCC->SetParameter(0, -0.494341);
  m_f_SysZhf_75_150ptv_shape_ShMGPy8_mCC->SetParameter(1, 0.0222273);
  m_f_SysZhf_75_150ptv_shape_ShMGPy8_mCC->SetParameter(2, -0.000100011);
  m_f_SysZhf_75_150ptv_shape_ShMGPy8_mCC->SetParameter(3, 1.34666e-07);
  Utils::SaveHist(m_f_SysZhf_75_150ptv_shape_ShMGPy8_mCC, m_allHists);

  // Zmf = {Zcl,Zbl,Zbc}
  // mironova @ 25/02/21
  m_f_SysZmf_75_150ptv_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysZmf_75_150ptv_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3)-1", 0, 500e3);
  m_f_SysZmf_75_150ptv_shape_ShMGPy8_mCC->SetParameter(0, 0.164996);
  m_f_SysZmf_75_150ptv_shape_ShMGPy8_mCC->SetParameter(1, 0.0118616);
  m_f_SysZmf_75_150ptv_shape_ShMGPy8_mCC->SetParameter(2, -5.10628e-05);
  m_f_SysZmf_75_150ptv_shape_ShMGPy8_mCC->SetParameter(3, 7.03564e-08);
  Utils::SaveHist(m_f_SysZmf_75_150ptv_shape_ShMGPy8_mCC, m_allHists);

  //================== Zl: VHcc  ==============================//
  // mironova @ 25/02/21
  m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysZl_2jet_150_250ptv_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1", 0, 500e3);
  m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_mCC->SetParameter(0, 0.184163);
  m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_mCC->SetParameter(1, 0.00659346);
  m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_mCC->SetParameter(2, -1.09136e-05);
  m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_mCC->SetParameter(3, 1.17347e-09);
  Utils::SaveHist(m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_mCC, m_allHists);

  m_f_SysZl_2jet_250ptv_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysZl_2jet_250ptv_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1", 0, 500e3);
  m_f_SysZl_2jet_250ptv_shape_ShMGPy8_mCC->SetParameter(0, 0.184163);
  m_f_SysZl_2jet_250ptv_shape_ShMGPy8_mCC->SetParameter(1, 0.00659346);
  m_f_SysZl_2jet_250ptv_shape_ShMGPy8_mCC->SetParameter(2, -1.09136e-05);
  m_f_SysZl_2jet_250ptv_shape_ShMGPy8_mCC->SetParameter(3, 1.17347e-09);
  Utils::SaveHist(m_f_SysZl_2jet_250ptv_shape_ShMGPy8_mCC, m_allHists);

  m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysZl_3jet_150_250ptv_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1", 0, 500e3);
  m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_mCC->SetParameter(0, 0.765005);
  m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_mCC->SetParameter(1, 0.000405682);
  m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_mCC->SetParameter(2, 7.44627e-06);
  m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_mCC->SetParameter(3, -1.54703e-08);
  Utils::SaveHist(m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_mCC, m_allHists);

  m_f_SysZl_3jet_250ptv_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysZl_3jet_250ptv_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3)-1", 0, 500e3);
  m_f_SysZl_3jet_250ptv_shape_ShMGPy8_mCC->SetParameter(0, 0.765005);
  m_f_SysZl_3jet_250ptv_shape_ShMGPy8_mCC->SetParameter(1, 0.000405682);
  m_f_SysZl_3jet_250ptv_shape_ShMGPy8_mCC->SetParameter(2, 7.44627e-06);
  m_f_SysZl_3jet_250ptv_shape_ShMGPy8_mCC->SetParameter(3, -1.54703e-08);
  Utils::SaveHist(m_f_SysZl_3jet_250ptv_shape_ShMGPy8_mCC, m_allHists);

  // mironova @ 25/02/21

  m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysZl_2jet_75_150ptv_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3)-1", 0, 500e3);
  m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_mCC->SetParameter(0, -0.739115);
  m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_mCC->SetParameter(1, 0.023006);
  m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_mCC->SetParameter(2, -8.86504e-05);
  m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_mCC->SetParameter(3, 1.12809e-07);
  Utils::SaveHist(m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_mCC, m_allHists);

  m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysZl_3jet_75_150ptv_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1", 0, 500e3);
  m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_mCC->SetParameter(0, 0.488778);
  m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_mCC->SetParameter(1, 0.0043022);
  m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_mCC->SetParameter(2, -5.886e-06);
  m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_mCC->SetParameter(3, -2.83831e-09);
  Utils::SaveHist(m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_mCC, m_allHists);

  //================== W+jet: VHcc  ==============================//
  // Whf = {Wcc,Wbb}
  // mironova @ 25/02/21
  m_f_SysWhf_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysWhf_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1 ", 0, 500e3);
  m_f_SysWhf_shape_ShMGPy8_mCC->SetParameter(0, 0.676749);
  m_f_SysWhf_shape_ShMGPy8_mCC->SetParameter(1, -0.00249327);
  m_f_SysWhf_shape_ShMGPy8_mCC->SetParameter(2, 3.59218e-05);
  m_f_SysWhf_shape_ShMGPy8_mCC->SetParameter(3, -7.12158e-08);
  Utils::SaveHist(m_f_SysWhf_shape_ShMGPy8_mCC, m_allHists);

  // Wmf = {Wcl,Wbl,Wbc}
  // mironova @ 25/02/21
  m_f_SysWmf_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysWmf_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3)-1", 0, 500e3);
  m_f_SysWmf_shape_ShMGPy8_mCC->SetParameter(0, 0.624112);
  m_f_SysWmf_shape_ShMGPy8_mCC->SetParameter(1, 0.00167091);
  m_f_SysWmf_shape_ShMGPy8_mCC->SetParameter(2, 5.09961e-06);
  m_f_SysWmf_shape_ShMGPy8_mCC->SetParameter(3, -1.57962e-08);
  Utils::SaveHist(m_f_SysWmf_shape_ShMGPy8_mCC, m_allHists);

  //================== Wl: VHcc  ==============================//
  // mironova @ 25/02/21
  m_f_SysWl_150_250ptv_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysWl_150_250ptv_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1", 0, 500e3);
  m_f_SysWl_150_250ptv_shape_ShMGPy8_mCC->SetParameter(0, 0.696171);
  m_f_SysWl_150_250ptv_shape_ShMGPy8_mCC->SetParameter(1, 6.16976e-05);
  m_f_SysWl_150_250ptv_shape_ShMGPy8_mCC->SetParameter(2, 1.37539e-05);
  m_f_SysWl_150_250ptv_shape_ShMGPy8_mCC->SetParameter(3, -2.85129e-08);
  Utils::SaveHist(m_f_SysWl_150_250ptv_shape_ShMGPy8_mCC, m_allHists);

  m_f_SysWl_250ptv_shape_ShMGPy8_mCC = std::make_shared<TF1>(
      "SysWl_250ptv_shape_ShMGPy8_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1", 0, 500e3);
  m_f_SysWl_250ptv_shape_ShMGPy8_mCC->SetParameter(0, 0.696171);
  m_f_SysWl_250ptv_shape_ShMGPy8_mCC->SetParameter(1, 6.16976e-05);
  m_f_SysWl_250ptv_shape_ShMGPy8_mCC->SetParameter(2, 1.37539e-05);
  m_f_SysWl_250ptv_shape_ShMGPy8_mCC->SetParameter(3, -2.85129e-08);
  Utils::SaveHist(m_f_SysWl_250ptv_shape_ShMGPy8_mCC, m_allHists);

  //============== Signal Region Syst VHcc  =======================//
  // SR systs - mironova

  //================== Z+jet: VHcc  ==============================//
  // Zhf = {Zcc,Zbb}
  // mironova @ 25/02/21
  m_f_SysZhf_150ptv_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysZhf_150ptv_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3) -1", 0, 500e3);
  m_f_SysZhf_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(0, 0.758766);
  m_f_SysZhf_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(1, 0.00336094);
  m_f_SysZhf_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(2, -5.28218e-06);
  m_f_SysZhf_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(3, -2.3378e-08);
  Utils::SaveHist(m_f_SysZhf_150ptv_shape_ShMGPy8_SR_mCC, m_allHists);

  // Zmf = {Zcl,Zbl,Zbc}
  // mironova @ 25/02/21
  m_f_SysZmf_150ptv_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysZmf_150ptv_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3)-1", 0, 500e3);
  m_f_SysZmf_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(0, 1.07979);
  m_f_SysZmf_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(1, -0.00171623);
  m_f_SysZmf_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(2, 1.18543e-05);
  m_f_SysZmf_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(3, -3.14039e-08);
  Utils::SaveHist(m_f_SysZmf_150ptv_shape_ShMGPy8_SR_mCC, m_allHists);

  // Zhf = {Zcc,Zbb}
  // mironova @ 25/02/21
  m_f_SysZhf_75_150ptv_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysZhf_75_150ptv_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3) -1", 0, 500e3);
  m_f_SysZhf_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(0, 2.10049);
  m_f_SysZhf_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(1, -0.017468);
  m_f_SysZhf_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(2, 7.06432e-05);
  m_f_SysZhf_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(3, -8.58742e-08);
  Utils::SaveHist(m_f_SysZhf_75_150ptv_shape_ShMGPy8_SR_mCC, m_allHists);

  // Zmf = {Zcl,Zbl,Zbc}
  // mironova @ 25/02/21
  m_f_SysZmf_75_150ptv_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysZmf_75_150ptv_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3)-1", 0, 500e3);
  m_f_SysZmf_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(0, 1.53344);
  m_f_SysZmf_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(1, -0.00907342);
  m_f_SysZmf_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(2, 4.07538e-05);
  m_f_SysZmf_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(3, -5.91714e-08);
  Utils::SaveHist(m_f_SysZmf_75_150ptv_shape_ShMGPy8_SR_mCC, m_allHists);

  //================== Zl: VHcc  ==============================//
  // mironova @ 25/02/21
  m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysZl_2jet_150_250ptv_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1", 0, 500e3);
  m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(0, 1.39525);
  m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(1, -0.00674389);
  m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(2, 3.06659e-05);
  m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(3, -5.59839e-08);
  Utils::SaveHist(m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_SR_mCC, m_allHists);

  m_f_SysZl_2jet_250ptv_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysZl_2jet_250ptv_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1", 0, 500e3);
  m_f_SysZl_2jet_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(0, 1.39525);
  m_f_SysZl_2jet_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(1, -0.00674389);
  m_f_SysZl_2jet_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(2, 3.06659e-05);
  m_f_SysZl_2jet_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(3, -5.59839e-08);
  Utils::SaveHist(m_f_SysZl_2jet_250ptv_shape_ShMGPy8_SR_mCC, m_allHists);

  m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysZl_3jet_150_250ptv_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1", 0, 500e3);
  m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(0, 0.867098);
  m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(1, 0.00205432);
  m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(2, -6.47704e-06);
  m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(3, 3.12119e-09);
  Utils::SaveHist(m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_SR_mCC, m_allHists);

  m_f_SysZl_3jet_250ptv_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysZl_3jet_250ptv_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3)-1", 0, 500e3);
  m_f_SysZl_3jet_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(0, 0.867098);
  m_f_SysZl_3jet_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(1, 0.00205432);
  m_f_SysZl_3jet_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(2, -6.47704e-06);
  m_f_SysZl_3jet_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(3, 3.12119e-09);
  Utils::SaveHist(m_f_SysZl_3jet_250ptv_shape_ShMGPy8_SR_mCC, m_allHists);

  // mironova @ 25/02/21

  m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysZl_2jet_75_150ptv_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3)-1", 0, 500e3);
  m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(0, 2.74487);
  m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(1, -0.0318983);
  m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(2, 0.000159834);
  m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(3, -2.57316e-07);
  Utils::SaveHist(m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_SR_mCC, m_allHists);

  m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysZl_3jet_75_150ptv_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1", 0, 500e3);
  m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(0, 1.34447);
  m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(1, -0.00564208);
  m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(2, 2.51553e-05);
  m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_SR_mCC->SetParameter(3, -3.90174e-08);
  Utils::SaveHist(m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_SR_mCC, m_allHists);

  //================== W+jet: VHcc  ==============================//

  // Whf = {Wcc,Wbb}
  // mironova @ 25/02/21
  m_f_SysWhf_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysWhf_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1 ", 0, 500e3);
  m_f_SysWhf_shape_ShMGPy8_SR_mCC->SetParameter(0, 1.08683);
  m_f_SysWhf_shape_ShMGPy8_SR_mCC->SetParameter(1, -0.000109889);
  m_f_SysWhf_shape_ShMGPy8_SR_mCC->SetParameter(2, -1.02893e-05);
  m_f_SysWhf_shape_ShMGPy8_SR_mCC->SetParameter(3, 2.23266e-08);
  Utils::SaveHist(m_f_SysWhf_shape_ShMGPy8_SR_mCC, m_allHists);

  // Wmf = {Wcl,Wbl,Wbc}
  // mironova @ 25/02/21
  m_f_SysWmf_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysWmf_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3)-1", 0, 500e3);
  m_f_SysWmf_shape_ShMGPy8_SR_mCC->SetParameter(0, 1.21185);
  m_f_SysWmf_shape_ShMGPy8_SR_mCC->SetParameter(1, -0.00424604);
  m_f_SysWmf_shape_ShMGPy8_SR_mCC->SetParameter(2, 2.38437e-05);
  m_f_SysWmf_shape_ShMGPy8_SR_mCC->SetParameter(3, -4.14344e-08);
  Utils::SaveHist(m_f_SysWmf_shape_ShMGPy8_SR_mCC, m_allHists);

  //================== Wl: VHcc  ==============================//
  // mironova @ 25/02/21
  m_f_SysWl_150_250ptv_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysWl_150_250ptv_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1", 0, 500e3);
  m_f_SysWl_150_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(0, 1.3448);
  m_f_SysWl_150_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(1, -0.00667622);
  m_f_SysWl_150_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(2, 3.57e-05);
  m_f_SysWl_150_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(3, -5.83245e-08);
  Utils::SaveHist(m_f_SysWl_150_250ptv_shape_ShMGPy8_SR_mCC, m_allHists);

  m_f_SysWl_250ptv_shape_ShMGPy8_SR_mCC = std::make_shared<TF1>(
      "SysWl_250ptv_shape_ShMGPy8_SR_mCC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1", 0, 500e3);
  m_f_SysWl_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(0, 1.3448);
  m_f_SysWl_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(1, -0.00667622);
  m_f_SysWl_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(2, 3.57e-05);
  m_f_SysWl_250ptv_shape_ShMGPy8_SR_mCC->SetParameter(3, -5.83245e-08);
  Utils::SaveHist(m_f_SysWl_250ptv_shape_ShMGPy8_SR_mCC, m_allHists);

  //================== TTbarbq: VHcc  ==============================//
  // ttbarbq mironova @ 30/04/2020
  m_f_SysTTbarbq_150_250ptv_shape_mCC_Her_Pyt = std::make_shared<TF1>(
      "SysTTbarbq_150_250ptv_shape_mCC_Her_Pyt",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3) -1 ", 0, 500e3);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(0, 1.20383);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(1, -0.00364248);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(2, 1.92981e-05);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(3, -3.1345e-08);
  Utils::SaveHist(m_f_SysTTbarbq_150_250ptv_shape_mCC_Her_Pyt, m_allHists);

  m_f_SysTTbarbq_250ptv_shape_mCC_Her_Pyt = std::make_shared<TF1>(
      "SysTTbarbq_250ptv_shape_mCC_Her_Pyt",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3) +[4]*((x/1e3)^4)-1 ",
      0, 500e3);
  m_f_SysTTbarbq_250ptv_shape_mCC_Her_Pyt->SetParameter(0, 1.2275);
  m_f_SysTTbarbq_250ptv_shape_mCC_Her_Pyt->SetParameter(1, -0.00259007);
  m_f_SysTTbarbq_250ptv_shape_mCC_Her_Pyt->SetParameter(2, 2.36509e-07);
  m_f_SysTTbarbq_250ptv_shape_mCC_Her_Pyt->SetParameter(3, 5.8073e-08);
  m_f_SysTTbarbq_250ptv_shape_mCC_Her_Pyt->SetParameter(4, -1.24202e-10);
  Utils::SaveHist(m_f_SysTTbarbq_250ptv_shape_mCC_Her_Pyt, m_allHists);

  m_f_SysTTbarbq_150_250ptv_shape_mCC_Pow_aMC = std::make_shared<TF1>(
      "SysTTbarbq_150_250ptv_shape_mCC_Pow_aMC",
      "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3) +[4]*((x/1e3)^4)-1 ",
      0, 500e3);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(0, 1.0618);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(1, 0.000750336);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(2, -2.18054e-05);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(3, 1.10981e-07);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(4, -1.6222e-10);
  Utils::SaveHist(m_f_SysTTbarbq_150_250ptv_shape_mCC_Pow_aMC, m_allHists);

  m_f_SysTTbarbq_250ptv_shape_mCC_Pow_aMC =
      std::make_shared<TF1>("SysTTbarbq_250ptv_shape_mCC_Pow_aMC",
                            "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + "
                            "[3]*((x/1e3)^3) + [4]*((x/1e3)^4) -1",
                            0, 500e3);
  m_f_SysTTbarbq_250ptv_shape_mCC_Pow_aMC->SetParameter(0, 1.19169);
  m_f_SysTTbarbq_250ptv_shape_mCC_Pow_aMC->SetParameter(1, -0.00214541);
  m_f_SysTTbarbq_250ptv_shape_mCC_Pow_aMC->SetParameter(2, 4.14841e-06);
  m_f_SysTTbarbq_250ptv_shape_mCC_Pow_aMC->SetParameter(3, 4.73884e-09);
  m_f_SysTTbarbq_250ptv_shape_mCC_Pow_aMC->SetParameter(4, -2.87665e-12);
  Utils::SaveHist(m_f_SysTTbarbq_250ptv_shape_mCC_Pow_aMC, m_allHists);

  m_f_SysTTbarbq_150_250ptv_shape_mCC_rHi =
      std::make_shared<TF1>("SysTTbarbq_150_250ptv_shape_mCC_rHi",
                            "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + "
                            "[3]*((x/1e3)^3) + [4]*((x/1e3)^4) -1",
                            0, 500e3);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_rHi->SetParameter(0, 1.06316);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_rHi->SetParameter(1, -0.00245376);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_rHi->SetParameter(2, 2.72849e-05);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_rHi->SetParameter(3, -1.13083e-07);
  m_f_SysTTbarbq_150_250ptv_shape_mCC_rHi->SetParameter(4, 1.57725e-10);
  Utils::SaveHist(m_f_SysTTbarbq_150_250ptv_shape_mCC_rHi, m_allHists);

  m_f_SysTTbarbq_250ptv_shape_mCC_rHi =
      std::make_shared<TF1>("SysTTbarbq_250ptv_shape_mCC_rHi",
                            "[0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + "
                            "[3]*((x/1e3)^3) + [4]*((x/1e3)^4) -1",
                            0, 500e3);
  m_f_SysTTbarbq_250ptv_shape_mCC_rHi->SetParameter(0, 1.06316);
  m_f_SysTTbarbq_250ptv_shape_mCC_rHi->SetParameter(1, -0.00245376);
  m_f_SysTTbarbq_250ptv_shape_mCC_rHi->SetParameter(2, 2.72849e-05);
  m_f_SysTTbarbq_250ptv_shape_mCC_rHi->SetParameter(3, -1.13083e-07);
  m_f_SysTTbarbq_250ptv_shape_mCC_rHi->SetParameter(4, 1.57725e-10);
  Utils::SaveHist(m_f_SysTTbarbq_250ptv_shape_mCC_rHi, m_allHists);

  //================== TTbarlq: VHcc  ==============================//
  // ttbarlq mironova @ 30/04/2020
  m_f_SysTTbarlq_shape_mCC_Her_Pyt = std::make_shared<TF1>(
      "SysTTbarlq_shape_mCC_Her_Pyt",
      "x<200e3 ? [0]*sin(x / (1e3*[1]) +[2]) * exp(- x / (1e3 * [3])) + [4] -1 "
      ": [0]*sin(200 / ([1]) +[2]) * exp(- 200 / ([3])) + [4] -1",
      0, 500e3);
  m_f_SysTTbarlq_shape_mCC_Her_Pyt->SetParameter(0, 1.58113182);
  m_f_SysTTbarlq_shape_mCC_Her_Pyt->SetParameter(1, 9.81154332);
  m_f_SysTTbarlq_shape_mCC_Her_Pyt->SetParameter(2, 46.19064014);
  m_f_SysTTbarlq_shape_mCC_Her_Pyt->SetParameter(3, 28.43223642);
  m_f_SysTTbarlq_shape_mCC_Her_Pyt->SetParameter(4, 1.02567763);
  Utils::SaveHist(m_f_SysTTbarlq_shape_mCC_Her_Pyt, m_allHists);

  m_f_SysTTbarlq_shape_mCC_Pow_aMC = std::make_shared<TF1>(
      "SysTTbarlq_shape_mCC_Pow_aMC",
      "x<200e3 ? [0]*sin(x / (1e3*[1]) +[2]) * exp(- x / (1e3 * [3])) + [4] -1 "
      ": [0]*sin(200 / ([1]) +[2]) * exp(- 200 / ([3])) + [4] -1",
      0, 500e3);
  m_f_SysTTbarlq_shape_mCC_Pow_aMC->SetParameter(0, 0.75385433);
  m_f_SysTTbarlq_shape_mCC_Pow_aMC->SetParameter(1, 11.78610678);
  m_f_SysTTbarlq_shape_mCC_Pow_aMC->SetParameter(2, 48.20040394);
  m_f_SysTTbarlq_shape_mCC_Pow_aMC->SetParameter(3, 38.10158937);
  m_f_SysTTbarlq_shape_mCC_Pow_aMC->SetParameter(4, 1.04835311);
  Utils::SaveHist(m_f_SysTTbarlq_shape_mCC_Pow_aMC, m_allHists);

  m_f_SysTTbarlq_150_250ptv_shape_mCC_rHi =
      std::make_shared<TF1>("SysTTbarlq_150_250ptv_shape_mCC_rHi",
                            "x<200e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) -1 : "
                            "[0] + [1]*200 +[2]*( (200)^2) -1",
                            0, 500e3);
  m_f_SysTTbarlq_150_250ptv_shape_mCC_rHi->SetParameter(0, 0.834402);
  m_f_SysTTbarlq_150_250ptv_shape_mCC_rHi->SetParameter(1, 0.00268185);
  m_f_SysTTbarlq_150_250ptv_shape_mCC_rHi->SetParameter(2, -8.30575e-06);
  Utils::SaveHist(m_f_SysTTbarlq_150_250ptv_shape_mCC_rHi, m_allHists);

  m_f_SysTTbarlq_250ptv_shape_mCC_rHi =
      std::make_shared<TF1>("SysTTbarlq_250ptv_shape_mCC_rHi",
                            "x<200e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) -1 : "
                            "[0] + [1]*200 +[2]*( (200)^2) -1",
                            0, 500e3);
  m_f_SysTTbarlq_250ptv_shape_mCC_rHi->SetParameter(0, 1.46735);
  m_f_SysTTbarlq_250ptv_shape_mCC_rHi->SetParameter(1, -0.00869924);
  m_f_SysTTbarlq_250ptv_shape_mCC_rHi->SetParameter(2, 3.72424e-05);
  Utils::SaveHist(m_f_SysTTbarlq_250ptv_shape_mCC_rHi, m_allHists);

  //================== stopWtbq: VHcc  ==============================//
  // stopWtbq mironova @ 30/04/2020
  m_f_SysStopWtbq_150_250ptv_shape_mCC_Her_Pyt = std::make_shared<TF1>(
      "SysStopWtbq_150_250ptv_shape_mCC_Her_Pyt",
      "x<200e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3) + [4]*( "
      "(x/1e3)^4) -1 : [0] + [1]*200 +[2]*( (200)^2) + [3]*((200)^3) + [4]*( "
      "(200)^4) -1",
      0, 500e3);
  m_f_SysStopWtbq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(0, 1.46706);
  m_f_SysStopWtbq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(1, -0.00470325);
  m_f_SysStopWtbq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(2, -4.47665e-05);
  m_f_SysStopWtbq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(3, 5.99649e-07);
  m_f_SysStopWtbq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(4, -1.59434e-09);
  Utils::SaveHist(m_f_SysStopWtbq_150_250ptv_shape_mCC_Her_Pyt, m_allHists);

  m_f_SysStopWtbq_250ptv_shape_mCC_Her_Pyt = std::make_shared<TF1>(
      "SysStopWtbq_250ptv_shape_mCC_Her_Pyt",
      "x<200e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3) + [4]*( "
      "(x/1e3)^4) -1 : [0] + [1]*200 +[2]*( (200)^2) + [3]*((200)^3) + [4]*( "
      "(200)^4) -1",
      0, 500e3);
  m_f_SysStopWtbq_250ptv_shape_mCC_Her_Pyt->SetParameter(0, 3.43683);
  m_f_SysStopWtbq_250ptv_shape_mCC_Her_Pyt->SetParameter(1, -0.0626555);
  m_f_SysStopWtbq_250ptv_shape_mCC_Her_Pyt->SetParameter(2, 0.000582317);
  m_f_SysStopWtbq_250ptv_shape_mCC_Her_Pyt->SetParameter(3, -2.36627e-06);
  m_f_SysStopWtbq_250ptv_shape_mCC_Her_Pyt->SetParameter(4, 3.60129e-09);
  Utils::SaveHist(m_f_SysStopWtbq_250ptv_shape_mCC_Her_Pyt, m_allHists);

  m_f_SysStopWtbq_150_250ptv_shape_mCC_Pow_aMC = std::make_shared<TF1>(
      "SysStopWtbq_150_250ptv_shape_mCC_Pow_aMC",
      "x<200e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3) + [4]*( "
      "(x/1e3)^4) -1 : [0] + [1]*200 +[2]*( (200)^2) + [3]*((200)^3) + [4]*( "
      "(200)^4) -1",
      0, 500e3);
  m_f_SysStopWtbq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(0, 3.37779);
  m_f_SysStopWtbq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(1, -0.0804718);
  m_f_SysStopWtbq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(2, 0.00103741);
  m_f_SysStopWtbq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(3, -5.96179e-06);
  m_f_SysStopWtbq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(4, 1.26122e-08);
  Utils::SaveHist(m_f_SysStopWtbq_150_250ptv_shape_mCC_Pow_aMC, m_allHists);

  m_f_SysStopWtbq_250ptv_shape_mCC_Pow_aMC = std::make_shared<TF1>(
      "SysStopWtbq_250ptv_shape_mCC_Pow_aMC",
      "x<200e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*((x/1e3)^3) + [4]*( "
      "(x/1e3)^4) -1 : [0] + [1]*200 +[2]*( (200)^2) + [3]*((200)^3) + [4]*( "
      "(200)^4) -1",
      0, 500e3);
  m_f_SysStopWtbq_250ptv_shape_mCC_Pow_aMC->SetParameter(0, 5.28316);
  m_f_SysStopWtbq_250ptv_shape_mCC_Pow_aMC->SetParameter(1, -0.147261);
  m_f_SysStopWtbq_250ptv_shape_mCC_Pow_aMC->SetParameter(2, 0.00186059);
  m_f_SysStopWtbq_250ptv_shape_mCC_Pow_aMC->SetParameter(3, -1.0109e-05);
  m_f_SysStopWtbq_250ptv_shape_mCC_Pow_aMC->SetParameter(4, 1.97283e-08);
  Utils::SaveHist(m_f_SysStopWtbq_250ptv_shape_mCC_Pow_aMC, m_allHists);

  //================== stopWtlq: VHcc  ==============================//
  // stopWtlq mironova @ 30/04/2020
  m_f_SysStopWtlq_150_250ptv_shape_mCC_Her_Pyt =
      std::make_shared<TF1>("SysStopWtlq_150_250ptv_shape_mCC_Her_Pyt",
                            "x<140e3 ? [0]*sin(x / (1e3*[1]) +[2]) * exp(- x / "
                            "(1e3 * [3])) + [4] + (x/(500*1e3)-1/10)-1 "
                            ": [0]*sin(140 / ([1]) +[2]) * exp(- 140 / ([3])) "
                            "+ [4] + (140/(500)-1/10)-1",
                            0, 500e3);
  m_f_SysStopWtlq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(0, 2.5);
  m_f_SysStopWtlq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(1, 9.81154332);
  m_f_SysStopWtlq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(2, 46.0);
  m_f_SysStopWtlq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(3, 28.43223642);
  m_f_SysStopWtlq_150_250ptv_shape_mCC_Her_Pyt->SetParameter(4, 0.85);
  Utils::SaveHist(m_f_SysStopWtlq_150_250ptv_shape_mCC_Her_Pyt, m_allHists);

  m_f_SysStopWtlq_250ptv_shape_mCC_Her_Pyt =
      std::make_shared<TF1>("SysStopWtlq_250ptv_shape_mCC_Her_Pyt",
                            "x<140e3 ? [0]*sin(x / (1e3*[1]) +[2]) * exp(- x / "
                            "(1e3 * [3])) + [4] + (x/(500*1e3)-1/10)-1 "
                            ": [0]*sin(140 / ([1]) +[2]) * exp(- 140 / ([3])) "
                            "+ [4] + (140/(500)-1/10)-1",
                            0, 500e3);
  m_f_SysStopWtlq_250ptv_shape_mCC_Her_Pyt->SetParameter(0, 2.5);
  m_f_SysStopWtlq_250ptv_shape_mCC_Her_Pyt->SetParameter(1, 9.81154332);
  m_f_SysStopWtlq_250ptv_shape_mCC_Her_Pyt->SetParameter(2, 45.8);
  m_f_SysStopWtlq_250ptv_shape_mCC_Her_Pyt->SetParameter(3, 28.43223642);
  m_f_SysStopWtlq_250ptv_shape_mCC_Her_Pyt->SetParameter(4, 0.85);
  Utils::SaveHist(m_f_SysStopWtlq_250ptv_shape_mCC_Her_Pyt, m_allHists);

  m_f_SysStopWtlq_150_250ptv_shape_mCC_Pow_aMC = std::make_shared<TF1>(
      "SysStopWtlq_150_250ptv_shape_mCC_Pow_aMC",
      "x<200e3 ? [0]*sin(x / (1e3*[1]) +[2]) * exp(- x / (1e3 * [3])) + [4] -1 "
      ": [0]*sin(200 / ([1]) +[2]) * exp(- 200 / ([3])) + [4] -1",
      0, 500e3);
  m_f_SysStopWtlq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(0, 1.5);
  m_f_SysStopWtlq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(1, 9.81154332);
  m_f_SysStopWtlq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(2, 46.0);
  m_f_SysStopWtlq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(3, 28.43223642);
  m_f_SysStopWtlq_150_250ptv_shape_mCC_Pow_aMC->SetParameter(4, 1.0);
  Utils::SaveHist(m_f_SysStopWtlq_150_250ptv_shape_mCC_Pow_aMC, m_allHists);

  m_f_SysStopWtlq_250ptv_shape_mCC_Pow_aMC = std::make_shared<TF1>(
      "SysStopWtlq_250ptv_shape_mCC_Pow_aMC",
      "x<200e3 ? [0]*sin(x / (1e3*[1]) +[2]) * exp(- x / (1e3 * [3])) + [4] -1 "
      ": [0]*sin(200 / ([1]) +[2]) * exp(- 200 / ([3])) + [4] -1",
      0, 500e3);
  m_f_SysStopWtlq_250ptv_shape_mCC_Pow_aMC->SetParameter(0, 1.1);
  m_f_SysStopWtlq_250ptv_shape_mCC_Pow_aMC->SetParameter(1, 9);
  m_f_SysStopWtlq_250ptv_shape_mCC_Pow_aMC->SetParameter(2, 45.1);
  m_f_SysStopWtlq_250ptv_shape_mCC_Pow_aMC->SetParameter(3, 35);
  m_f_SysStopWtlq_250ptv_shape_mCC_Pow_aMC->SetParameter(4, 1.02);
  Utils::SaveHist(m_f_SysStopWtlq_250ptv_shape_mCC_Pow_aMC, m_allHists);

  //================== Diboson: VHcc  ==============================//
  // Diboson mironova @ 05/05/20

  //================== Diboson ZZcc: VHcc  ==============================//
  m_f_SysZZcc_75_150ptv_shape_PwPy8_mCC = std::make_shared<TF1>(
      "SysZZcc_75_150ptv_shape_PwPy8_mCC",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysZZcc_75_150ptv_shape_PwPy8_mCC->SetParameter(0, 2.71028);
  m_f_SysZZcc_75_150ptv_shape_PwPy8_mCC->SetParameter(1, -0.0760661);
  m_f_SysZZcc_75_150ptv_shape_PwPy8_mCC->SetParameter(2, 0.00105697);
  m_f_SysZZcc_75_150ptv_shape_PwPy8_mCC->SetParameter(3, -5.63782e-06);
  m_f_SysZZcc_75_150ptv_shape_PwPy8_mCC->SetParameter(4, 1.05096e-08);
  Utils::SaveHist(m_f_SysZZcc_75_150ptv_shape_PwPy8_mCC, m_allHists);

  m_f_SysZZcc_150_250ptv_shape_PwPy8_mCC = std::make_shared<TF1>(
      "SysZZcc_150_250ptv_shape_PwPy8_mCC",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysZZcc_150_250ptv_shape_PwPy8_mCC->SetParameter(0, 5.48188);
  m_f_SysZZcc_150_250ptv_shape_PwPy8_mCC->SetParameter(1, -0.159685);
  m_f_SysZZcc_150_250ptv_shape_PwPy8_mCC->SetParameter(2, 0.00187173);
  m_f_SysZZcc_150_250ptv_shape_PwPy8_mCC->SetParameter(3, -8.455e-06);
  m_f_SysZZcc_150_250ptv_shape_PwPy8_mCC->SetParameter(4, 1.30484e-08);
  Utils::SaveHist(m_f_SysZZcc_150_250ptv_shape_PwPy8_mCC, m_allHists);

  m_f_SysZZcc_250ptv_shape_PwPy8_mCC =
      std::make_shared<TF1>("SysZZcc_250ptv_shape_PwPy8_mCC",
                            "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) -1 : "
                            "[0] + [1]*140 +[2]*( (140)^2) -1",
                            0, 500e3);
  m_f_SysZZcc_250ptv_shape_PwPy8_mCC->SetParameter(0, -0.359044);
  m_f_SysZZcc_250ptv_shape_PwPy8_mCC->SetParameter(1, 0.0208766);
  m_f_SysZZcc_250ptv_shape_PwPy8_mCC->SetParameter(2, -6.76179e-05);
  Utils::SaveHist(m_f_SysZZcc_250ptv_shape_PwPy8_mCC, m_allHists);

  //================== Diboson ZZbkg: VHcc  ==============================//

  m_f_SysZZbkg_75_150ptv_shape_PwPy8_mCC = std::make_shared<TF1>(
      "SysZZbkg_75_150ptv_shape_PwPy8_mCC",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysZZbkg_75_150ptv_shape_PwPy8_mCC->SetParameter(0, 2.77575);
  m_f_SysZZbkg_75_150ptv_shape_PwPy8_mCC->SetParameter(1, -0.0722631);
  m_f_SysZZbkg_75_150ptv_shape_PwPy8_mCC->SetParameter(2, 0.00102233);
  m_f_SysZZbkg_75_150ptv_shape_PwPy8_mCC->SetParameter(3, -5.9655e-06);
  m_f_SysZZbkg_75_150ptv_shape_PwPy8_mCC->SetParameter(4, 1.21983e-08);
  Utils::SaveHist(m_f_SysZZbkg_75_150ptv_shape_PwPy8_mCC, m_allHists);

  m_f_SysZZbkg_150_250ptv_shape_PwPy8_mCC = std::make_shared<TF1>(
      "SysZZbkg_150_250ptv_shape_PwPy8_mCC",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysZZbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(0, 2.34922);
  m_f_SysZZbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(1, -0.0536707);
  m_f_SysZZbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(2, 0.000703513);
  m_f_SysZZbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(3, -3.64212e-06);
  m_f_SysZZbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(4, 6.50063e-09);
  Utils::SaveHist(m_f_SysZZbkg_150_250ptv_shape_PwPy8_mCC, m_allHists);

  m_f_SysZZbkg_250ptv_shape_PwPy8_mCC = std::make_shared<TF1>(
      "SysZZbkg_250ptv_shape_PwPy8_mCC",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysZZbkg_250ptv_shape_PwPy8_mCC->SetParameter(0, 3.99039);
  m_f_SysZZbkg_250ptv_shape_PwPy8_mCC->SetParameter(1, -0.118896);
  m_f_SysZZbkg_250ptv_shape_PwPy8_mCC->SetParameter(2, 0.00158209);
  m_f_SysZZbkg_250ptv_shape_PwPy8_mCC->SetParameter(3, -8.47079e-06);
  m_f_SysZZbkg_250ptv_shape_PwPy8_mCC->SetParameter(4, 1.57836e-08);
  Utils::SaveHist(m_f_SysZZbkg_250ptv_shape_PwPy8_mCC, m_allHists);

  //================== Diboson WZhadlepcl: VHcc ==============================//

  m_f_SysWZhadlepcl_75_150ptv_shape_PwPy8_mCC_2lep = std::make_shared<TF1>(
      "SysWZhadlepcl_75_150ptv_shape_PwPy8_mCC_2lep",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysWZhadlepcl_75_150ptv_shape_PwPy8_mCC_2lep->SetParameter(0, 3.36142);
  m_f_SysWZhadlepcl_75_150ptv_shape_PwPy8_mCC_2lep->SetParameter(1, -0.0975614);
  m_f_SysWZhadlepcl_75_150ptv_shape_PwPy8_mCC_2lep->SetParameter(2, 0.00137727);
  m_f_SysWZhadlepcl_75_150ptv_shape_PwPy8_mCC_2lep->SetParameter(3,
                                                                 -7.89438e-06);
  m_f_SysWZhadlepcl_75_150ptv_shape_PwPy8_mCC_2lep->SetParameter(4,
                                                                 1.57658e-08);
  Utils::SaveHist(m_f_SysWZhadlepcl_75_150ptv_shape_PwPy8_mCC_2lep, m_allHists);

  m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_2lep = std::make_shared<TF1>(
      "SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_2lep",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1 : [0] "
      "+ [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) -1",
      0, 500e3);
  m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_2lep->SetParameter(0, 1.42511);
  m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_2lep->SetParameter(1,
                                                                  -0.0176611);
  m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_2lep->SetParameter(2,
                                                                  0.000213152);
  m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_2lep->SetParameter(3,
                                                                  -7.51633e-07);
  Utils::SaveHist(m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_2lep,
                  m_allHists);

  m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_2lep =
      std::make_shared<TF1>("SysWZhadlepcl_250ptv_shape_PwPy8_mCC_2lep",
                            "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) -1 : "
                            "[0] + [1]*140 +[2]*( (140)^2) -1",
                            0, 500e3);
  m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_2lep->SetParameter(0, 0.755276);
  m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_2lep->SetParameter(1, 0.00419016);
  m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_2lep->SetParameter(2, -1.58572e-05);
  Utils::SaveHist(m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_2lep, m_allHists);

  m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_0lep = std::make_shared<TF1>(
      "SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_0lep",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1 : [0] "
      "+ [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) -1",
      0, 500e3);
  m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_0lep->SetParameter(0, 1.37401);
  m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_0lep->SetParameter(1,
                                                                  -0.0163674);
  m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_0lep->SetParameter(2,
                                                                  0.000188403);
  m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_0lep->SetParameter(3,
                                                                  -5.77367e-07);
  Utils::SaveHist(m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_0lep,
                  m_allHists);

  m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_0lep =
      std::make_shared<TF1>("SysWZhadlepcl_250ptv_shape_PwPy8_mCC_0lep",
                            "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) -1 : "
                            "[0] + [1]*140 +[2]*( (140)^2) -1",
                            0, 500e3);
  m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_0lep->SetParameter(0, 0.46116);
  m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_0lep->SetParameter(1, 0.00753589);
  m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_0lep->SetParameter(2, -1.7924e-05);
  Utils::SaveHist(m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_0lep, m_allHists);

  //================== Diboson WZhadlepbkg: VHcc
  //==============================//

  m_f_SysWZhadlepbkg_75_150ptv_shape_PwPy8_mCC_2lep = std::make_shared<TF1>(
      "SysWZhadlepbkg_75_150ptv_shape_PwPy8_mCC_2lep",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysWZhadlepbkg_75_150ptv_shape_PwPy8_mCC_2lep->SetParameter(0, 1.31532);
  m_f_SysWZhadlepbkg_75_150ptv_shape_PwPy8_mCC_2lep->SetParameter(1,
                                                                  -0.0098682);
  m_f_SysWZhadlepbkg_75_150ptv_shape_PwPy8_mCC_2lep->SetParameter(2,
                                                                  0.000110418);
  m_f_SysWZhadlepbkg_75_150ptv_shape_PwPy8_mCC_2lep->SetParameter(3,
                                                                  -5.07157e-07);
  m_f_SysWZhadlepbkg_75_150ptv_shape_PwPy8_mCC_2lep->SetParameter(4,
                                                                  7.28881e-10);
  Utils::SaveHist(m_f_SysWZhadlepbkg_75_150ptv_shape_PwPy8_mCC_2lep,
                  m_allHists);

  m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_2lep = std::make_shared<TF1>(
      "SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_2lep",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1 : [0] "
      "+ [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) -1",
      0, 500e3);
  m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_2lep->SetParameter(0, 1.67224);
  m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_2lep->SetParameter(1,
                                                                   -0.0192198);
  m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_2lep->SetParameter(2,
                                                                   0.000186463);
  m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_2lep->SetParameter(
      3, -6.23989e-07);
  Utils::SaveHist(m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_2lep,
                  m_allHists);

  m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_2lep =
      std::make_shared<TF1>("SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_2lep",
                            "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) -1 : "
                            "[0] + [1]*140 +[2]*( (140)^2) -1",
                            0, 500e3);
  m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_2lep->SetParameter(0, 1.19233);
  m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_2lep->SetParameter(1, -0.00146689);
  m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_2lep->SetParameter(2, -2.2763e-06);
  Utils::SaveHist(m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_2lep, m_allHists);

  m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_0lep = std::make_shared<TF1>(
      "SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_0lep",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) -1 : [0] "
      "+ [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) -1",
      0, 500e3);
  m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_0lep->SetParameter(0, 1.46922);
  m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_0lep->SetParameter(1,
                                                                   -0.0139003);
  m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_0lep->SetParameter(2,
                                                                   0.000121841);
  m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_0lep->SetParameter(
      3, -3.09247e-07);
  Utils::SaveHist(m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_0lep,
                  m_allHists);

  m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_0lep =
      std::make_shared<TF1>("SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_0lep",
                            "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) -1 : "
                            "[0] + [1]*140 +[2]*( (140)^2) -1",
                            0, 500e3);
  m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_0lep->SetParameter(0, 0.541198);
  m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_0lep->SetParameter(1, 0.00695166);
  m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_0lep->SetParameter(2, -2.20895e-05);
  Utils::SaveHist(m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_0lep, m_allHists);

  //================== Diboson WlepZhad: VHcc  ==============================//

  m_f_SysWZlephadcc_150_250ptv_shape_PwPy8_mCC = std::make_shared<TF1>(
      "SysWZlephadcc_150_250ptv_shape_PwPy8_mCC",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysWZlephadcc_150_250ptv_shape_PwPy8_mCC->SetParameter(0, 3.57595);
  m_f_SysWZlephadcc_150_250ptv_shape_PwPy8_mCC->SetParameter(1, -0.0928873);
  m_f_SysWZlephadcc_150_250ptv_shape_PwPy8_mCC->SetParameter(2, 0.0010755);
  m_f_SysWZlephadcc_150_250ptv_shape_PwPy8_mCC->SetParameter(3, -4.67149e-06);
  m_f_SysWZlephadcc_150_250ptv_shape_PwPy8_mCC->SetParameter(4, 6.78097e-09);
  Utils::SaveHist(m_f_SysWZlephadcc_150_250ptv_shape_PwPy8_mCC, m_allHists);

  m_f_SysWZlephadcc_250ptv_shape_PwPy8_mCC = std::make_shared<TF1>(
      "SysWZlephadcc_250ptv_shape_PwPy8_mCC",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysWZlephadcc_250ptv_shape_PwPy8_mCC->SetParameter(0, 7.42473);
  m_f_SysWZlephadcc_250ptv_shape_PwPy8_mCC->SetParameter(1, -0.256308);
  m_f_SysWZlephadcc_250ptv_shape_PwPy8_mCC->SetParameter(2, 0.00347454);
  m_f_SysWZlephadcc_250ptv_shape_PwPy8_mCC->SetParameter(3, -1.91936e-05);
  m_f_SysWZlephadcc_250ptv_shape_PwPy8_mCC->SetParameter(4, 3.71582e-08);
  Utils::SaveHist(m_f_SysWZlephadcc_250ptv_shape_PwPy8_mCC, m_allHists);

  m_f_SysWZlephadbkg_150_250ptv_shape_PwPy8_mCC = std::make_shared<TF1>(
      "SysWZlephadbkg_150_250ptv_shape_PwPy8_mCC",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysWZlephadbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(0, 2.18062);
  m_f_SysWZlephadbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(1, -0.046621);
  m_f_SysWZlephadbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(2, 0.000605533);
  m_f_SysWZlephadbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(3, -3.14358e-06);
  m_f_SysWZlephadbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(4, 5.78286e-09);
  Utils::SaveHist(m_f_SysWZlephadbkg_150_250ptv_shape_PwPy8_mCC, m_allHists);

  m_f_SysWZlephadbkg_250ptv_shape_PwPy8_mCC = std::make_shared<TF1>(
      "SysWZlephadbkg_250ptv_shape_PwPy8_mCC",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysWZlephadbkg_250ptv_shape_PwPy8_mCC->SetParameter(0, 2.78059);
  m_f_SysWZlephadbkg_250ptv_shape_PwPy8_mCC->SetParameter(1, -0.0692982);
  m_f_SysWZlephadbkg_250ptv_shape_PwPy8_mCC->SetParameter(2, 0.000875077);
  m_f_SysWZlephadbkg_250ptv_shape_PwPy8_mCC->SetParameter(3, -4.36801e-06);
  m_f_SysWZlephadbkg_250ptv_shape_PwPy8_mCC->SetParameter(4, 7.5991e-09);
  Utils::SaveHist(m_f_SysWZlephadbkg_250ptv_shape_PwPy8_mCC, m_allHists);

  //================== Diboson (WW): VHcc  ==============================//

  m_f_SysWWcl_150_250ptv_shape_PwPy8_mCC = std::make_shared<TF1>(
      "SysWWcl_150_250ptv_shape_PwPy8_mCC",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysWWcl_150_250ptv_shape_PwPy8_mCC->SetParameter(0, 4.35475);
  m_f_SysWWcl_150_250ptv_shape_PwPy8_mCC->SetParameter(1, -0.134789);
  m_f_SysWWcl_150_250ptv_shape_PwPy8_mCC->SetParameter(2, 0.00184298);
  m_f_SysWWcl_150_250ptv_shape_PwPy8_mCC->SetParameter(3, -1.01734e-05);
  m_f_SysWWcl_150_250ptv_shape_PwPy8_mCC->SetParameter(4, 1.96218e-08);
  Utils::SaveHist(m_f_SysWWcl_150_250ptv_shape_PwPy8_mCC, m_allHists);

  m_f_SysWWcl_250ptv_shape_PwPy8_mCC = std::make_shared<TF1>(
      "SysWWcl_250ptv_shape_PwPy8_mCC",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysWWcl_250ptv_shape_PwPy8_mCC->SetParameter(0, 3.73416);
  m_f_SysWWcl_250ptv_shape_PwPy8_mCC->SetParameter(1, -0.120469);
  m_f_SysWWcl_250ptv_shape_PwPy8_mCC->SetParameter(2, 0.00174084);
  m_f_SysWWcl_250ptv_shape_PwPy8_mCC->SetParameter(3, -1.002e-05);
  m_f_SysWWcl_250ptv_shape_PwPy8_mCC->SetParameter(4, 2.00891e-08);
  Utils::SaveHist(m_f_SysWWcl_250ptv_shape_PwPy8_mCC, m_allHists);

  m_f_SysWWbkg_150_250ptv_shape_PwPy8_mCC = std::make_shared<TF1>(
      "SysWWbkg_150_250ptv_shape_PwPy8_mCC",
      "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4)-1 : [0] + [1]*140 +[2]*( (140)^2) + [3]*( (140)^3) + [4]*( "
      "(140)^4)-1",
      0, 500e3);
  m_f_SysWWbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(0, 3.35084);
  m_f_SysWWbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(1, -0.0900272);
  m_f_SysWWbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(2, 0.00117008);
  m_f_SysWWbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(3, -6.14094e-06);
  m_f_SysWWbkg_150_250ptv_shape_PwPy8_mCC->SetParameter(4, 1.12256e-08);
  Utils::SaveHist(m_f_SysWWbkg_150_250ptv_shape_PwPy8_mCC, m_allHists);

  m_f_SysWWbkg_250ptv_shape_PwPy8_mCC =
      std::make_shared<TF1>("SysWWbkg_250ptv_shape_PwPy8_mCC",
                            "x<140e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) -1 : "
                            "[0] + [1]*140 +[2]*( (140)^2) -1",
                            0, 500e3);
  m_f_SysWWbkg_250ptv_shape_PwPy8_mCC->SetParameter(0, 0.755481);
  m_f_SysWWbkg_250ptv_shape_PwPy8_mCC->SetParameter(1, 0.0028964);
  m_f_SysWWbkg_250ptv_shape_PwPy8_mCC->SetParameter(2, -4.4456e-06);
  Utils::SaveHist(m_f_SysWWbkg_250ptv_shape_PwPy8_mCC, m_allHists);

  //================== Signal: VHcc  ==============================//

  // VHcc signal mironova @ 13/06/2020

  m_f_SysqqZHcc_150_250ptv_shape_PwHwg7_mCC = std::make_shared<TF1>(
      "SysqqZHcc_150_250ptv_shape_PwHwg7_mCC",
      "x<155e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4) + [5]*( (x/1e3)^5) -1 : [0] + [1]*155 +[2]*( (155)^2) + "
      "[3]*( (155)^3) + [4]*( "
      "(155)^4)+[5]*( (155)^5) -1",
      0, 500e3);
  m_f_SysqqZHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(0, 21.616);
  m_f_SysqqZHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(1, -1.1178);
  m_f_SysqqZHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(2, 0.0232764);
  m_f_SysqqZHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(3, -0.000233583);
  m_f_SysqqZHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(4, 1.13284e-06);
  m_f_SysqqZHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(5, -2.12858e-09);
  Utils::SaveHist(m_f_SysqqZHcc_150_250ptv_shape_PwHwg7_mCC, m_allHists);

  m_f_SysqqZHcc_250ptv_shape_PwHwg7_mCC = std::make_shared<TF1>(
      "SysqqZHcc_250ptv_shape_PwHwg7_mCC",
      "x<155e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4) + [5]*( (x/1e3)^5) -1 : [0] + [1]*155 +[2]*( (155)^2) + "
      "[3]*( (155)^3) + [4]*( "
      "(155)^4)+[5]*( (155)^5) -1",
      0, 500e3);
  m_f_SysqqZHcc_250ptv_shape_PwHwg7_mCC->SetParameter(0, 31.618);
  m_f_SysqqZHcc_250ptv_shape_PwHwg7_mCC->SetParameter(1, -1.61832);
  m_f_SysqqZHcc_250ptv_shape_PwHwg7_mCC->SetParameter(2, 0.032901);
  m_f_SysqqZHcc_250ptv_shape_PwHwg7_mCC->SetParameter(3, -0.000323342);
  m_f_SysqqZHcc_250ptv_shape_PwHwg7_mCC->SetParameter(4, 1.54087e-06);
  m_f_SysqqZHcc_250ptv_shape_PwHwg7_mCC->SetParameter(5, -2.85388e-09);
  Utils::SaveHist(m_f_SysqqZHcc_250ptv_shape_PwHwg7_mCC, m_allHists);

  m_f_SysggZHcc_150_250ptv_shape_PwHwg7_mCC = std::make_shared<TF1>(
      "SysggZHcc_150_250ptv_shape_PwHwg7_mCC",
      "x<155e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4) + [5]*( (x/1e3)^5) -1 : [0] + [1]*155 +[2]*( (155)^2) + "
      "[3]*( (155)^3) + [4]*( "
      "(155)^4)+[5]*( (155)^5) -1",
      0, 500e3);
  m_f_SysggZHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(0, 15.9395);
  m_f_SysggZHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(1, -0.816191);
  m_f_SysggZHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(2, 0.0170527);
  m_f_SysggZHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(3, -0.000171175);
  m_f_SysggZHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(4, 8.27345e-07);
  m_f_SysggZHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(5, -1.54155e-09);
  Utils::SaveHist(m_f_SysggZHcc_150_250ptv_shape_PwHwg7_mCC, m_allHists);

  m_f_SysggZHcc_250ptv_shape_PwHwg7_mCC = std::make_shared<TF1>(
      "SysggZHcc_250ptv_shape_PwHwg7_mCC",
      "x<155e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4) + [5]*( (x/1e3)^5) -1 : [0] + [1]*155 +[2]*( (155)^2) + "
      "[3]*( (155)^3) + [4]*( "
      "(155)^4)+[5]*( (155)^5) -1",
      0, 500e3);
  m_f_SysggZHcc_250ptv_shape_PwHwg7_mCC->SetParameter(0, 23.5294);
  m_f_SysggZHcc_250ptv_shape_PwHwg7_mCC->SetParameter(1, -1.438);
  m_f_SysggZHcc_250ptv_shape_PwHwg7_mCC->SetParameter(2, 0.0339514);
  m_f_SysggZHcc_250ptv_shape_PwHwg7_mCC->SetParameter(3, -0.000376042);
  m_f_SysggZHcc_250ptv_shape_PwHwg7_mCC->SetParameter(4, 1.97378e-06);
  m_f_SysggZHcc_250ptv_shape_PwHwg7_mCC->SetParameter(5, -3.95697e-09);
  Utils::SaveHist(m_f_SysggZHcc_250ptv_shape_PwHwg7_mCC, m_allHists);

  m_f_SysqqWHcc_150_250ptv_shape_PwHwg7_mCC = std::make_shared<TF1>(
      "SysqqWHcc_150_250ptv_shape_PwHwg7_mCC",
      "x<155e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4) + [5]*( (x/1e3)^5) -1 : [0] + [1]*155 +[2]*( (155)^2) + "
      "[3]*( (155)^3) + [4]*( "
      "(155)^4)+[5]*( (155)^5) -1",
      0, 500e3);
  m_f_SysqqWHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(0, 20.4486);
  m_f_SysqqWHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(1, -1.07907);
  m_f_SysqqWHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(2, 0.0228534);
  m_f_SysqqWHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(3, -0.000232539);
  m_f_SysqqWHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(4, 1.14156e-06);
  m_f_SysqqWHcc_150_250ptv_shape_PwHwg7_mCC->SetParameter(5, -2.16863e-09);
  Utils::SaveHist(m_f_SysqqWHcc_150_250ptv_shape_PwHwg7_mCC, m_allHists);

  m_f_SysqqWHcc_250ptv_shape_PwHwg7_mCC = std::make_shared<TF1>(
      "SysqqWHcc_250ptv_shape_PwHwg7_mCC",
      "x<155e3 ? [0] + [1]*x/1e3 +[2]*( (x/1e3)^2) + [3]*( (x/1e3)^3) + [4]*( "
      "(x/1e3)^4) + [5]*( (x/1e3)^5) -1 : [0] + [1]*155 +[2]*( (155)^2) + "
      "[3]*( (155)^3) + [4]*( "
      "(155)^4)+[5]*( (155)^5) -1",
      0, 500e3);
  m_f_SysqqWHcc_250ptv_shape_PwHwg7_mCC->SetParameter(0, 26.1489);
  m_f_SysqqWHcc_250ptv_shape_PwHwg7_mCC->SetParameter(1, -1.33724);
  m_f_SysqqWHcc_250ptv_shape_PwHwg7_mCC->SetParameter(2, 0.0271786);
  m_f_SysqqWHcc_250ptv_shape_PwHwg7_mCC->SetParameter(3, -0.000266254);
  m_f_SysqqWHcc_250ptv_shape_PwHwg7_mCC->SetParameter(4, 1.26306e-06);
  m_f_SysqqWHcc_250ptv_shape_PwHwg7_mCC->SetParameter(5, -2.32698e-09);
  Utils::SaveHist(m_f_SysqqWHcc_250ptv_shape_PwHwg7_mCC, m_allHists);

  //================== VHbb Analysis:  ===============================//

  //================== Signal: UEPSMbb  ==============================//

  // NM 17-06-15
  // Histogram variations for VHUEPSMbb for EPS 2017

  std::string FileLocationVHSysts =
      m_WorkDir_DIR + "/data/CorrsAndSysts/SystVH/";
  TFile *file_VH_mBB = TFile::Open(
      (TString)(FileLocationVHSysts + "Herwig_shapes_2019.root"), "READ");

  // all systs are ~equal. Just pick one with decent stats. VD to refnie if
  // needed
  if (file_VH_mBB && file_VH_mBB->IsOpen()) {
    m_h_SysVHUEPSMbb = std::shared_ptr<TH1F>(
        reinterpret_cast<TH1F *>(file_VH_mBB->Get("ratio_mBB_1leptons_2jet")));
    m_h_SysVHUEPSMbb->SetDirectory(0);
    file_VH_mBB->Close();
  }
  delete file_VH_mBB;

  //===================== Diboson: Reso  ==============================//
  // Diboson ptV and mbb shape systematics defined by Paolo Francavilla and
  // Carlo Pandini:
  // https://indico.cern.ch/event/540849/contributions/2245805/attachments/1310155/1960010/Hbb_SMVH_DibosonShapes_2016_07_15.pdf
  // https://indico.cern.ch/event/540849/contributions/2245423/attachments/1309922/1959563/DibosonShape.pdf
  // Added by Paolo
  //================ MatrixElement mBB  ==============================//
  m_f_SysVVMbbME_WZlvqq_2j =
      std::make_shared<TF1>("mBB__WZlvqq_PS_Sherpa_fit",
                            "[0]+[1]*tanh( (x/1e3-[2])/[3] )", 25e3, 500e3);
  m_f_SysVVMbbME_WZlvqq_2j->SetParameter(0, 0.28196);
  m_f_SysVVMbbME_WZlvqq_2j->SetParameter(1, 4.30023e-01);
  m_f_SysVVMbbME_WZlvqq_2j->SetParameter(2, 1.01083e+02);
  m_f_SysVVMbbME_WZlvqq_2j->SetParameter(3, 1.87708e+01);
  Utils::SaveHist(m_f_SysVVMbbME_WZlvqq_2j, m_allHists);

  m_f_SysVVMbbME_ZZvvqq_2j =
      std::make_shared<TF1>("mBB__ZZqqvv_PS_Sherpa_fit",
                            "[0]+[1]*tanh( (x/1e3-[2])/[3] )", 25e3, 500e3);
  m_f_SysVVMbbME_ZZvvqq_2j->SetParameter(0, 0.24918);
  m_f_SysVVMbbME_ZZvvqq_2j->SetParameter(1, 4.07409e-01);
  m_f_SysVVMbbME_ZZvvqq_2j->SetParameter(2, 9.51575e+01);
  m_f_SysVVMbbME_ZZvvqq_2j->SetParameter(3, 1.46196e+01);
  Utils::SaveHist(m_f_SysVVMbbME_ZZvvqq_2j, m_allHists);

  m_f_SysVVMbbME_ZZllqq_2j =
      std::make_shared<TF1>("mBB__ZZqqll_PS_Sherpa_fit",
                            "[0]+[1]*tanh( (x/1e3-[2])/[3] )", 25e3, 500e3);
  m_f_SysVVMbbME_ZZllqq_2j->SetParameter(0, 0.18724);
  m_f_SysVVMbbME_ZZllqq_2j->SetParameter(1, 3.20219e-01);
  m_f_SysVVMbbME_ZZllqq_2j->SetParameter(2, 9.38599e+01);
  m_f_SysVVMbbME_ZZllqq_2j->SetParameter(3, 1.45278e+01);
  Utils::SaveHist(m_f_SysVVMbbME_ZZllqq_2j, m_allHists);

  m_f_SysVVMbbME_WZlvqq_3j = std::make_shared<TF1>(
      "mBB__WZlvqq_PS_Sherpa_fit_3J",
      " x<250e3 ? [0]+[1]*x/1e3+[2]*x*x/1e6+[3]*x*x*x/1e9 "
      ":[0]+[1]*250+[2]*250*250+[3]*250*250*250",
      25e3, 500e3);
  m_f_SysVVMbbME_WZlvqq_3j->SetParameter(0, -1.29300e-01 - 1);
  m_f_SysVVMbbME_WZlvqq_3j->SetParameter(1, 2.92621e-02);
  m_f_SysVVMbbME_WZlvqq_3j->SetParameter(2, -2.35003e-04);
  m_f_SysVVMbbME_WZlvqq_3j->SetParameter(3, 5.60350e-07);
  Utils::SaveHist(m_f_SysVVMbbME_WZlvqq_3j, m_allHists);

  m_f_SysVVMbbME_ZZvvqq_3j = std::make_shared<TF1>(
      "mBB__ZZqqvv_PS_Sherpa_fit_3J",
      " x<250e3 ? [0]+[1]*x/1e3+[2]*x*x/1e6+[3]*x*x*x/1e9 "
      ":[0]+[1]*250+[2]*250*250+[3]*250*250*250",
      25e3, 500e3);
  m_f_SysVVMbbME_ZZvvqq_3j->SetParameter(0, 1.65481e-01 - 1);
  m_f_SysVVMbbME_ZZvvqq_3j->SetParameter(1, 1.63341e-02);
  m_f_SysVVMbbME_ZZvvqq_3j->SetParameter(2, -8.01718e-05);
  m_f_SysVVMbbME_ZZvvqq_3j->SetParameter(3, 1.00608e-07);
  Utils::SaveHist(m_f_SysVVMbbME_ZZvvqq_3j, m_allHists);

  m_f_SysVVMbbME_ZZllqq_3j = std::make_shared<TF1>(
      "mBB__ZZqqll_PS_Sherpa_fit_3J",
      " x<250e3 ? [0]+[1]*x/1e3+[2]*x*x/1e6+[3]*x*x*x/1e9 "
      ":[0]+[1]*250+[2]*250*250+[3]*250*250*250",
      25e3, 500e3);
  m_f_SysVVMbbME_ZZllqq_3j->SetParameter(0, 2.42069e-01 - 1);
  m_f_SysVVMbbME_ZZllqq_3j->SetParameter(1, 1.99211e-02);
  m_f_SysVVMbbME_ZZllqq_3j->SetParameter(2, -1.50742e-04);
  m_f_SysVVMbbME_ZZllqq_3j->SetParameter(3, 3.18587e-07);
  Utils::SaveHist(m_f_SysVVMbbME_ZZllqq_3j, m_allHists);

  //================ VVMbbPSUE: Reso  ==============================//
  // NM 17-06-15
  // Histogram variations for VVMbbPSUE for EPS 2017

  std::string FileLocationVVSysts =
      m_WorkDir_DIR + "/data/CorrsAndSysts/SystVV/";
  TFile *file_VV_mBB = TFile::Open(
      (TString)(FileLocationVVSysts + "specialDiboson.root"), "READ");

  // all systs are ~equal. Just pick one with decent stats. VD to refnie if
  // needed
  if (file_VV_mBB && file_VV_mBB->IsOpen()) {
    m_h_SysVVMbbPSUE_WZlvqq_2j = std::shared_ptr<TH1>(reinterpret_cast<TH1 *>(
        file_VV_mBB->Get("WZ_lvqq_1lep_0ptag2jet_SUM__PSunc")));
    m_h_SysVVMbbPSUE_ZZvvqq_2j = std::shared_ptr<TH1>(reinterpret_cast<TH1 *>(
        file_VV_mBB->Get("ZZ_qqvv_0lep_0ptag2jet_SR__PSunc")));
    m_h_SysVVMbbPSUE_ZZllqq_2j = std::shared_ptr<TH1>(reinterpret_cast<TH1 *>(
        file_VV_mBB->Get("ZZ_qqll_2lep_0ptag2jet_HL_SR__PSunc")));
    m_h_SysVVMbbPSUE_WZlvqq_3j = std::shared_ptr<TH1>(reinterpret_cast<TH1 *>(
        file_VV_mBB->Get("WZ_lvqq_1lep_0ptag3jet_SUM__PSunc")));
    m_h_SysVVMbbPSUE_ZZvvqq_3j = std::shared_ptr<TH1>(reinterpret_cast<TH1 *>(
        file_VV_mBB->Get("ZZ_qqvv_0lep_0ptag3jet_SR__PSunc")));
    m_h_SysVVMbbPSUE_ZZllqq_3j = std::shared_ptr<TH1>(reinterpret_cast<TH1 *>(
        file_VV_mBB->Get("ZZ_qqll_2lep_0ptag3pjet_HL_SR__PSunc")));

    m_h_SysVVMbbPSUE_WZlvqq_2j->SetDirectory(0);
    m_h_SysVVMbbPSUE_ZZvvqq_2j->SetDirectory(0);
    m_h_SysVVMbbPSUE_ZZllqq_2j->SetDirectory(0);
    m_h_SysVVMbbPSUE_WZlvqq_3j->SetDirectory(0);
    m_h_SysVVMbbPSUE_ZZvvqq_3j->SetDirectory(0);
    m_h_SysVVMbbPSUE_ZZllqq_3j->SetDirectory(0);

    file_VV_mBB->Close();
  }
  delete file_VV_mBB;

  //=============Diboson: Matrix element 2j ================================//

  m_f_SysVVPTVME_WZlvqq_2j = std::make_shared<TF1>(
      "pTV__WZlvqq_PS__2jet_Sherpa_fit", "[0]+[1]/((x/1e3)+[3])+[2]*(x/1e3) -1",
      150e3, 500e3);
  m_f_SysVVPTVME_WZlvqq_2j->SetParameter(0, 0.460423);
  m_f_SysVVPTVME_WZlvqq_2j->SetParameter(1, 6537.89);
  m_f_SysVVPTVME_WZlvqq_2j->SetParameter(2, -0.000754577);
  m_f_SysVVPTVME_WZlvqq_2j->SetParameter(3, 9471.99);
  Utils::SaveHist(m_f_SysVVPTVME_WZlvqq_2j, m_allHists);

  m_f_SysVVPTVME_ZZvvqq_2j = std::make_shared<TF1>(
      "pTV__ZZqqvv_PS__2jet_Sherpa_fit", "[0]+[1]/((x/1e3)+[3])+[2]*(x/1e3) -1",
      150e3, 500e3);
  m_f_SysVVPTVME_ZZvvqq_2j->SetParameter(0, 0.48311);
  m_f_SysVVPTVME_ZZvvqq_2j->SetParameter(1, 2467.86);
  m_f_SysVVPTVME_ZZvvqq_2j->SetParameter(2, -0.000459634);
  m_f_SysVVPTVME_ZZvvqq_2j->SetParameter(3, 3816.75);
  Utils::SaveHist(m_f_SysVVPTVME_ZZvvqq_2j, m_allHists);

  m_f_SysVVPTVME_ZZllqq_2j = std::make_shared<TF1>(
      "pTV__ZZqqll_PS__2jet_Sherpa_fit", "[0]+[1]/((x/1e3)+[3])+[2]*(x/1e3) -1",
      25e3, 500e3);
  m_f_SysVVPTVME_ZZllqq_2j->SetParameter(0, -0.219101);
  m_f_SysVVPTVME_ZZllqq_2j->SetParameter(1, 862.682);
  m_f_SysVVPTVME_ZZllqq_2j->SetParameter(2, 0.00100774);
  m_f_SysVVPTVME_ZZllqq_2j->SetParameter(3, 684.347);
  Utils::SaveHist(m_f_SysVVPTVME_ZZllqq_2j, m_allHists);

  //=============Diboson: PS/UE 2j ================================//

  m_f_SysVVPTVPSUE_WZlvqq_2j = std::make_shared<TF1>(
      "pTV__WZlvqq_PS__2jet_PS_fit", "[0]+[1]/((x/1e3)+[3])+[2]*(x/1e3) -1",
      150e3, 500e3);
  m_f_SysVVPTVPSUE_WZlvqq_2j->SetParameter(0, 0.416151);
  m_f_SysVVPTVPSUE_WZlvqq_2j->SetParameter(1, 5988.8);
  m_f_SysVVPTVPSUE_WZlvqq_2j->SetParameter(2, -0.000164153);
  m_f_SysVVPTVPSUE_WZlvqq_2j->SetParameter(3, 9540.26);
  Utils::SaveHist(m_f_SysVVPTVPSUE_WZlvqq_2j, m_allHists);

  m_f_SysVVPTVPSUE_ZZvvqq_2j = std::make_shared<TF1>(
      "pTV__ZZqqvv_PS__2jet_PS_fit", "[0]+[1]/((x/1e3)+[3])+[2]*(x/1e3) -1",
      150e3, 500e3);
  // VD on 13-06-2017: putting the same as the 3J one
  m_f_SysVVPTVPSUE_ZZvvqq_2j->SetParameter(0, 1.0046);
  m_f_SysVVPTVPSUE_ZZvvqq_2j->SetParameter(1, 255.117);
  m_f_SysVVPTVPSUE_ZZvvqq_2j->SetParameter(2, -0.000162205);
  m_f_SysVVPTVPSUE_ZZvvqq_2j->SetParameter(3, 8322.63);
  Utils::SaveHist(m_f_SysVVPTVPSUE_ZZvvqq_2j, m_allHists);

  m_f_SysVVPTVPSUE_ZZllqq_2j = std::make_shared<TF1>(
      "pTV__ZZqqll_PS__2jet_PS_fit", "[0]+[1]/((x/1e3)+[3])+[2]*(x/1e3) -1",
      25e3, 500e3);
  m_f_SysVVPTVPSUE_ZZllqq_2j->SetParameter(0, 0.912408);
  m_f_SysVVPTVPSUE_ZZllqq_2j->SetParameter(1, 3.96622);
  m_f_SysVVPTVPSUE_ZZllqq_2j->SetParameter(2, 0.000266947);
  m_f_SysVVPTVPSUE_ZZllqq_2j->SetParameter(3, 10);
  Utils::SaveHist(m_f_SysVVPTVPSUE_ZZllqq_2j, m_allHists);

  //=============Diboson: Matrix element 3j ================================//

  m_f_SysVVPTVME_WZlvqq_3j = std::make_shared<TF1>(
      "pTV__WZlvqq_PS__3jet_Sherpa_fit", "[0]+[1]/((x/1e3)+[3])+[2]*(x/1e3) -1",
      150e3, 500e3);
  m_f_SysVVPTVME_WZlvqq_3j->SetParameter(0, 0.684114);
  m_f_SysVVPTVME_WZlvqq_3j->SetParameter(1, 4432.74);
  m_f_SysVVPTVME_WZlvqq_3j->SetParameter(2, -0.00064093);
  m_f_SysVVPTVME_WZlvqq_3j->SetParameter(3, 9979.11);
  Utils::SaveHist(m_f_SysVVPTVME_WZlvqq_3j, m_allHists);

  m_f_SysVVPTVME_ZZvvqq_3j = std::make_shared<TF1>(
      "pTV__ZZqqvv_PS__3jet_Sherpa_fit", "[0]+[1]/((x/1e3)+[3])+[2]*(x/1e3) -1",
      150e3, 500e3);

  // VD on 13-06-2017: putting the same as the 2J one
  m_f_SysVVPTVME_ZZvvqq_3j->SetParameter(0, 0.48311);
  m_f_SysVVPTVME_ZZvvqq_3j->SetParameter(1, 2467.86);
  m_f_SysVVPTVME_ZZvvqq_3j->SetParameter(2, -0.000459634);
  m_f_SysVVPTVME_ZZvvqq_3j->SetParameter(3, 3816.75);
  Utils::SaveHist(m_f_SysVVPTVME_ZZvvqq_3j, m_allHists);

  m_f_SysVVPTVME_ZZllqq_3j = std::make_shared<TF1>(
      "pTV__ZZqqll_PS__3jet_Sherpa_fit", "[0]+[1]/((x/1e3)+[3])+[2]*(x/1e3) -1",
      25e3, 500e3);
  m_f_SysVVPTVME_ZZllqq_3j->SetParameter(0, 0.0611405);
  m_f_SysVVPTVME_ZZllqq_3j->SetParameter(1, 792.836);
  m_f_SysVVPTVME_ZZllqq_3j->SetParameter(2, 0.000446438);
  m_f_SysVVPTVME_ZZllqq_3j->SetParameter(3, 795.418);
  Utils::SaveHist(m_f_SysVVPTVME_ZZllqq_3j, m_allHists);

  //=============Diboson: PS/UE 3j ================================//

  m_f_SysVVPTVPSUE_WZlvqq_3j = std::make_shared<TF1>(
      "pTV__WZlvqq_PS__3jet_PS_fit", "[0]+[1]/((x/1e3)+[3])+[2]*(x/1e3) -1",
      150e3, 500e3);
  m_f_SysVVPTVPSUE_WZlvqq_3j->SetParameter(0, 0.660208);
  m_f_SysVVPTVPSUE_WZlvqq_3j->SetParameter(1, 3994.23);
  m_f_SysVVPTVPSUE_WZlvqq_3j->SetParameter(2, -0.000273587);
  m_f_SysVVPTVPSUE_WZlvqq_3j->SetParameter(3, 9945.67);
  Utils::SaveHist(m_f_SysVVPTVPSUE_WZlvqq_3j, m_allHists);

  m_f_SysVVPTVPSUE_ZZvvqq_3j = std::make_shared<TF1>(
      "pTV__ZZqqvv_PS__3jet_PS_fit", "[0]+[1]/((x/1e3)+[3])+[2]*(x/1e3) -1",
      150e3, 500e3);
  m_f_SysVVPTVPSUE_ZZvvqq_3j->SetParameter(0, 1.0046);
  m_f_SysVVPTVPSUE_ZZvvqq_3j->SetParameter(1, 255.117);
  m_f_SysVVPTVPSUE_ZZvvqq_3j->SetParameter(2, -0.000162205);
  m_f_SysVVPTVPSUE_ZZvvqq_3j->SetParameter(3, 8322.63);
  Utils::SaveHist(m_f_SysVVPTVPSUE_ZZvvqq_3j, m_allHists);

  m_f_SysVVPTVPSUE_ZZllqq_3j = std::make_shared<TF1>(
      "pTV__ZZqqll_PS__3jet_PS_fit", "[0]+[1]/((x/1e3)+[3])+[2]*(x/1e3) -1",
      25e3, 500e3);
  m_f_SysVVPTVPSUE_ZZllqq_3j->SetParameter(0, 0.938234);
  m_f_SysVVPTVPSUE_ZZllqq_3j->SetParameter(1, 1.93089);
  m_f_SysVVPTVPSUE_ZZllqq_3j->SetParameter(2, 0.000351818);
  m_f_SysVVPTVPSUE_ZZllqq_3j->SetParameter(3, 10);
  Utils::SaveHist(m_f_SysVVPTVPSUE_ZZllqq_3j, m_allHists);

  //================ Single top Resolved  ==============================

  // stop pTV mbb shape systs defined from Elisabeth Schopf:
  // https://indico.cern.ch/event/540849/contributions/2238370/attachments/1306487/1955101/16_07_08_stop_update.pdf
  // added by yama 12.07.16

  // stop pTV mbb shape systs updated by Aidan Robson for summer 2017 paper:
  // https://indico.cern.ch/event/630642/contributions/2552625/attachments/1444032/2248873/hbb_singletopThUncert_170412_updateAftertalk.pdf
  // added by eschopf 10.05.17

  //============= VHbb reso: stop-t ================================//

  // stop_t pTV shape systs - updated for summer 2017
  m_f_SysStoptPTV = std::make_shared<TF1>(
      "f_SysStoptPTV", "x<500e3 ? [0] + [1] * x - 1 : [0] + [1] * 500e3 - 1", 0,
      500e3);
  m_f_SysStoptPTV->SetParameter(0, 1.17);
  m_f_SysStoptPTV->SetParameter(1, -0.001e-3); // in 1/MeV
  Utils::SaveHist(m_f_SysStoptPTV, m_allHists);

  /// summer 2017
  m_f_SysStoptMBB = std::make_shared<TF1>(
      "f_SysStoptMBB", "x<500e3 ? [0] + [1] * x - 1 : [0] + [1] * 500e3 - 1", 0,
      500e3);
  m_f_SysStoptMBB->SetParameter(0, 1.12);
  m_f_SysStoptMBB->SetParameter(1, -0.0008e-3); // in 1/MeV
  Utils::SaveHist(m_f_SysStoptMBB, m_allHists);

  //============= VHbb reso: stop-Wt ================================//

  // stop_Wt PTV shape systs
  m_f_SysStopWtbbPTV = std::make_shared<TF1>(
      "f_SysStopWtbbPTV", "x<400e3 ? [0] + [1] * x - 1 : [0] + [1] * 400e3 - 1",
      0, 500e3);
  m_f_SysStopWtbbPTV->SetParameter(0, 1.86676e+00);
  m_f_SysStopWtbbPTV->SetParameter(1, -3.99643e-06); // in 1/MeV
  Utils::SaveHist(m_f_SysStopWtbbPTV, m_allHists);

  m_f_SysStopWtothPTV = std::make_shared<TF1>(
      "f_SysStopWtothPTV",
      "x<400e3 ? [0] + [1] * x - 1 : [0] + [1] * 400e3 - 1", 0, 500e3);
  m_f_SysStopWtothPTV->SetParameter(0, 1.23126e+00);
  m_f_SysStopWtothPTV->SetParameter(1, -1.08791e-06); // in 1/MeV
  Utils::SaveHist(m_f_SysStopWtothPTV, m_allHists);

  // stop_Wt MBB shape systs
  m_f_SysStopWtbbMBB = std::make_shared<TF1>(
      "f_SysStopWtbbMBB", "x<275e3 ? [0] + [1] * x - 1 : [0] + [1] * 275e3 - 1",
      0, 500e3);
  m_f_SysStopWtbbMBB->SetParameter(0, 1.24982e+00);
  m_f_SysStopWtbbMBB->SetParameter(1, -1.16573e-06); // in 1/MeV
  Utils::SaveHist(m_f_SysStopWtbbMBB, m_allHists);

  m_f_SysStopWtothMBB = std::make_shared<TF1>(
      "f_SysStopWtothMBB",
      "x<200e3 ? [0] + [1] * x - 1 : [0] + [1] * 200e3 - 1", 0, 500e3);
  m_f_SysStopWtothMBB->SetParameter(0, 1.18685e+00);
  m_f_SysStopWtothMBB->SetParameter(1, -1.53239e-06); // in 1/MeV
  Utils::SaveHist(m_f_SysStopWtothMBB, m_allHists);

  //================ Single top Boosted  ==============================

  // StopWt sys used in the boosted analysis. In the analysis there is a
  // variation in each analysis region.
  // The values are extracted from the shape comparison between the nominal and
  // the DS sample (using the truth tagging)

  m_h_SysStopWtDSDR_0L_Mptv_HPSR =
      std::make_shared<TH1F>("h_SysStopWtDSDR_0L_Mptv_HPSR",
                             "h_SysStopWtDSDR_0L_Mptv_HPSR", 20, 0, 500e3);
  m_h_SysStopWtDSDR_0L_Mptv_HPSR->SetDirectory(0);
  m_h_SysStopWtDSDR_1L_Mptv_HPSR =
      std::make_shared<TH1F>("h_SysStopWtDSDR_1L_Mptv_HPSR",
                             "h_SysStopWtDSDR_1L_Mptv_HPSR", 20, 0, 500e3);
  m_h_SysStopWtDSDR_1L_Mptv_HPSR->SetDirectory(0);
  m_h_SysStopWtDSDR_0L_Mptv_LPSR =
      std::make_shared<TH1F>("h_SysStopWtDSDR_0L_Mptv_LPSR",
                             "h_SysStopWtDSDR_0L_Mptv_LPSR", 20, 0, 500e3);
  m_h_SysStopWtDSDR_0L_Mptv_LPSR->SetDirectory(0);
  m_h_SysStopWtDSDR_1L_Mptv_LPSR =
      std::make_shared<TH1F>("h_SysStopWtDSDR_1L_Mptv_LPSR",
                             "h_SysStopWtDSDR_1L_Mptv_LPSR", 20, 0, 500e3);
  m_h_SysStopWtDSDR_1L_Mptv_LPSR->SetDirectory(0);
  m_h_SysStopWtDSDR_0L_Mptv_CR0J =
      std::make_shared<TH1F>("h_SysStopWtDSDR_0L_Mptv_CR0J",
                             "h_SysStopWtDSDR_0L_Mptv_CR0J", 20, 0, 500e3);
  m_h_SysStopWtDSDR_0L_Mptv_CR0J->SetDirectory(0);
  m_h_SysStopWtDSDR_0L_Mptv_CR1J =
      std::make_shared<TH1F>("h_SysStopWtDSDR_1L_Mptv_CR1J",
                             "h_SysStopWtDSDR_1L_Mptv_CR1J", 20, 0, 500e3);
  m_h_SysStopWtDSDR_0L_Mptv_CR1J->SetDirectory(0);

  m_h_SysStopWtDSDR_1L_Mptv_CR = std::make_shared<TH1F>(
      "h_SysStopWtDSDR_1L_Mptv_CR", "h_SysStopWtDSDR_1L_Mptv_CR", 20, 0, 500e3);
  m_h_SysStopWtDSDR_1L_Mptv_CR->SetDirectory(0);

  Float_t a_stopwtmj_m[20] = {
      // This array describes the DS/DR shape in the medium ptv region. The same
      // shape is used in the 0L and 1L.
      0.860114077094, 0.860114077094, 0.860114077094, 0.875856923038,
      0.99107319244,  1.10141203694,  1.19854210127,  0.951640693412,
      0.706271621772, 0.731736123956, 0.37716985124,  2.05581776213,
      2.05581776213,  2.05581776213,  2.05581776213,  2.05581776213,
      2.05581776213,  2.05581776213,  2.05581776213,  2.05581776213};

  Float_t a_stopwtmj_0L_Mptv_HPSR[20];
  Float_t a_stopwtmj_1L_Mptv_HPSR[20];
  Float_t a_stopwtmj_0L_Mptv_LPSR[20];
  Float_t a_stopwtmj_1L_Mptv_LPSR[20];
  Float_t a_stopwtmj_0L_Mptv_CR0J[20];
  Float_t a_stopwtmj_0L_Mptv_CR1J[20];
  Float_t a_stopwtmj_1L_Mptv_CR[20];

  for (int i = 0; i < 20; i++) {
    a_stopwtmj_0L_Mptv_HPSR[i] =
        0.84 * (1. / 1.03713) * a_stopwtmj_m[i] -
        1; // The first number represents the yield ratio DS/DR, the second one
           // removes the residual norm effect from shape. These numbers are
           // extracted for each analysis region in each lepton channel.
    a_stopwtmj_0L_Mptv_LPSR[i] = 0.43 * (1. / 0.997544) * a_stopwtmj_m[i] - 1;
    a_stopwtmj_0L_Mptv_CR0J[i] = 0.53 * 0.950087 * a_stopwtmj_m[i] - 1;
    a_stopwtmj_0L_Mptv_CR1J[i] = 0.53 * 1.0178 * a_stopwtmj_m[i] - 1;

    a_stopwtmj_1L_Mptv_HPSR[i] = 0.64 * (1. / 1.01464) * a_stopwtmj_m[i] - 1;
    a_stopwtmj_1L_Mptv_LPSR[i] = 0.36 * (1. / 0.99602) * a_stopwtmj_m[i] - 1;
    a_stopwtmj_1L_Mptv_CR[i] = 0.60 * (1. / 1.00257) * a_stopwtmj_m[i] - 1;
  }
  Utils::FillTH1(20, a_stopwtmj_0L_Mptv_HPSR, m_h_SysStopWtDSDR_0L_Mptv_HPSR,
                 m_allHists);
  Utils::FillTH1(20, a_stopwtmj_0L_Mptv_LPSR, m_h_SysStopWtDSDR_0L_Mptv_LPSR,
                 m_allHists);
  Utils::FillTH1(20, a_stopwtmj_0L_Mptv_CR0J, m_h_SysStopWtDSDR_0L_Mptv_CR0J,
                 m_allHists);
  Utils::FillTH1(20, a_stopwtmj_0L_Mptv_CR1J, m_h_SysStopWtDSDR_0L_Mptv_CR1J,
                 m_allHists);
  Utils::FillTH1(20, a_stopwtmj_1L_Mptv_HPSR, m_h_SysStopWtDSDR_1L_Mptv_HPSR,
                 m_allHists);
  Utils::FillTH1(20, a_stopwtmj_1L_Mptv_LPSR, m_h_SysStopWtDSDR_1L_Mptv_LPSR,
                 m_allHists);
  Utils::FillTH1(20, a_stopwtmj_1L_Mptv_CR, m_h_SysStopWtDSDR_1L_Mptv_CR,
                 m_allHists);

  m_h_SysStopWtDSDR_0L_Hptv_HPSR =
      std::make_shared<TH1F>("h_SysStopWtDSDR_0L_Hptv_HPSR",
                             "h_SysStopWtDSDR_0L_Hptv_HPSR", 20, 0, 500e3);
  m_h_SysStopWtDSDR_0L_Hptv_HPSR->SetDirectory(0);
  m_h_SysStopWtDSDR_1L_Hptv_HPSR =
      std::make_shared<TH1F>("h_SysStopWtDSDR_1L_Hptv_HPSR",
                             "h_SysStopWtDSDR_1L_Hptv_HPSR", 20, 0, 500e3);
  m_h_SysStopWtDSDR_1L_Hptv_HPSR->SetDirectory(0);
  m_h_SysStopWtDSDR_0L_Hptv_LPSR =
      std::make_shared<TH1F>("h_SysStopWtDSDR_0L_Hptv_LPSR",
                             "h_SysStopWtDSDR_0L_Hptv_LPSR", 20, 0, 500e3);
  m_h_SysStopWtDSDR_0L_Hptv_LPSR->SetDirectory(0);
  m_h_SysStopWtDSDR_1L_Hptv_LPSR =
      std::make_shared<TH1F>("h_SysStopWtDSDR_1L_Hptv_LPSR",
                             "h_SysStopWtDSDR_1L_Hptv_LPSR", 20, 0, 500e3);
  m_h_SysStopWtDSDR_1L_Hptv_LPSR->SetDirectory(0);
  m_h_SysStopWtDSDR_0L_Hptv_CR0J =
      std::make_shared<TH1F>("h_SysStopWtDSDR_0L_Hptv_CR0J",
                             "h_SysStopWtDSDR_0L_Hptv_CR0J", 20, 0, 500e3);
  m_h_SysStopWtDSDR_0L_Hptv_CR0J->SetDirectory(0);
  m_h_SysStopWtDSDR_0L_Hptv_CR1J =
      std::make_shared<TH1F>("h_SysStopWtDSDR_0L_Hptv_CR1J",
                             "h_SysStopWtDSDR_0L_Hptv_CR1J", 20, 0, 500e3);
  m_h_SysStopWtDSDR_0L_Hptv_CR1J->SetDirectory(0);
  m_h_SysStopWtDSDR_1L_Hptv_CR =
      std::make_shared<TH1F>("h_SysStopWtDSDR_1L_Hptv_CR0J",
                             "h_SysStopWtDSDR_1L_Hptv_CR0J", 20, 0, 500e3);
  m_h_SysStopWtDSDR_1L_Hptv_CR->SetDirectory(0);

  Float_t a_stopwtmj_h[20] = {
      // This array describes the DS/DR shape in the high ptv region. The same
      // shape is used in 0L and 1L.
      0.767471500538, 0.767471500538, 0.767471500538, 0.841395357565,
      0.984401566758, 1.3177335005,   1.75159962603,  1.06541229995,
      0.47960805945,  0.278185918666, 0.257883382585, 0.107811510541,
      0.107811510541, 0.107811510541, 0.107811510541, 0.107811510541,
      0.107811510541, 0.107811510541, 0.107811510541, 0.107811510541};

  Float_t a_stopwtmj_0L_Hptv_HPSR[20];
  Float_t a_stopwtmj_1L_Hptv_HPSR[20];
  Float_t a_stopwtmj_0L_Hptv_LPSR[20];
  Float_t a_stopwtmj_1L_Hptv_LPSR[20];
  Float_t a_stopwtmj_0L_Hptv_CR0J[20];
  Float_t a_stopwtmj_0L_Hptv_CR1J[20];
  Float_t a_stopwtmj_1L_Hptv_CR[20];

  for (int i = 0; i < 20; i++) {
    a_stopwtmj_0L_Hptv_HPSR[i] =
        0.38 * (1. / 0.916722) * a_stopwtmj_h[i] -
        1; // The first number represents the yield ratio DS/DR, the second one
           // removes the residual norm effect from shape. These numbers are
           // extracted for each analysis region in each lepton channel.
    a_stopwtmj_0L_Hptv_LPSR[i] = 0.22 * (1. / 0.918623) * a_stopwtmj_h[i] - 1;
    a_stopwtmj_0L_Hptv_CR0J[i] = 0.29 * 1.07272 * a_stopwtmj_h[i] - 1;
    a_stopwtmj_0L_Hptv_CR1J[i] = 0.29 * 0.928019 * a_stopwtmj_h[i] - 1;

    a_stopwtmj_1L_Hptv_HPSR[i] = 0.37 * (1. / 1.07412) * a_stopwtmj_h[i] - 1;
    a_stopwtmj_1L_Hptv_LPSR[i] = 0.19 * (1. / 0.997927) * a_stopwtmj_h[i] - 1;
    a_stopwtmj_1L_Hptv_CR[i] = 0.23 * (1. / 0.972403) * a_stopwtmj_h[i] - 1;
  }

  Utils::FillTH1(20, a_stopwtmj_0L_Hptv_HPSR, m_h_SysStopWtDSDR_0L_Hptv_HPSR,
                 m_allHists);
  Utils::FillTH1(20, a_stopwtmj_0L_Hptv_LPSR, m_h_SysStopWtDSDR_0L_Hptv_LPSR,
                 m_allHists);
  Utils::FillTH1(20, a_stopwtmj_0L_Hptv_CR0J, m_h_SysStopWtDSDR_0L_Hptv_CR0J,
                 m_allHists);
  Utils::FillTH1(20, a_stopwtmj_0L_Hptv_CR1J, m_h_SysStopWtDSDR_0L_Hptv_CR1J,
                 m_allHists);
  Utils::FillTH1(20, a_stopwtmj_1L_Hptv_HPSR, m_h_SysStopWtDSDR_1L_Hptv_HPSR,
                 m_allHists);
  Utils::FillTH1(20, a_stopwtmj_1L_Hptv_LPSR, m_h_SysStopWtDSDR_1L_Hptv_LPSR,
                 m_allHists);
  Utils::FillTH1(20, a_stopwtmj_1L_Hptv_CR, m_h_SysStopWtDSDR_1L_Hptv_CR,
                 m_allHists);
  // end of stopWt sys used in the boosted analysis.

  //================ TTbar Boosted  ==============================

  // mfoti
  m_f_SysTTbarMJISR_M = std::make_shared<TF1>(
      "f_SysTTbarMJISR_M", "[0] + [1]*x + [2]*x*x -1", 0, 500e3);
  m_f_SysTTbarMJISR_M->SetParameter(0, 0.882392456029);
  m_f_SysTTbarMJISR_M->SetParameter(1, 0.00183429726144e-3);
  m_f_SysTTbarMJISR_M->SetParameter(2, -6.57920263491e-12);
  Utils::SaveHist(m_f_SysTTbarMJISR_M, m_allHists);

  m_f_SysTTbarMJISR_H = std::make_shared<TF1>(
      "f_SysTTbarMJISR_H", "[0] + [1]*x + [2]*x*x -1", 0, 500e3);
  m_f_SysTTbarMJISR_H->SetParameter(0, 0.778289012272);
  m_f_SysTTbarMJISR_H->SetParameter(1, 0.00293311622985e-3);
  m_f_SysTTbarMJISR_H->SetParameter(2, -8.98299052949e-12);
  Utils::SaveHist(m_f_SysTTbarMJISR_H, m_allHists);

  m_h_SysTTbarMJPS = std::make_shared<TH1F>("h_SysTTbarMJPS", "h_SysTTbarMJPS",
                                            10, 50.0e3, 300e3);
  m_h_SysTTbarMJPS->SetDirectory(0);
  Float_t a_ttbarmjps[10] = {0.894909399947 - 1, 0.97661545519 - 1,
                             1.05419674144 - 1,  1.08271614388 - 1,
                             0.970446542709 - 1, 0.922786799197 - 1,
                             1.04505543553 - 1,  0.953300315517 - 1,
                             0.882220516226 - 1, 1.01973505182 - 1};
  Utils::FillTH1(10, a_ttbarmjps, m_h_SysTTbarMJPS, m_allHists);
  // mfoti

  //================ Diboson JMR Boosted  ==============================

  // JMR VV uncertainties for the boosted VHbb analysis (bmoser)
  // 0L and 1L:
  m_h_SysVVMJJMR_01L_M = std::make_shared<TH1F>(
      "h_SysVVMJJMR_01L_M", "h_SysVVMJJMR_01L_M", 20, 50e3, 150e3);
  m_h_SysVVMJJMR_01L_M->SetDirectory(0);
  Float_t a_vvmjjmr_01l_m[20] = {
      1.01213 - 1.0,  1.06831 - 1.0, 1.09208 - 1.0,  1.18973 - 1.0,
      1.05046 - 1.0,  1.09693 - 1.0, 0.854599 - 1.0, 0.870626 - 1.0,
      0.915896 - 1.0, 1.02599 - 1.0, 1.06313 - 1.0,  1.2618 - 1.0,
      1.30362 - 1.0,  1.27 - 1.0,    1.25428 - 1.0,  1.18 - 1.0,
      1.10 - 1.0,     1.05 - 1.0,    1.0001 - 1.0,   1.0001 - 1.0,
  };
  Utils::FillTH1(20, a_vvmjjmr_01l_m, m_h_SysVVMJJMR_01L_M, m_allHists);

  m_h_SysVVMJJMR_01L_H = std::make_shared<TH1F>(
      "h_SysVVMJJMR_01L_H", "h_SysVVMJJMR_01L_H", 20, 50e3, 150e3);
  m_h_SysVVMJJMR_01L_H->SetDirectory(0);
  Float_t a_vvmjjmr_01l_h[20] = {
      1.0001 - 1.0,   1.1 - 1.0,      1.25 - 1.0,     1.38844 - 1.0,
      1.2322 - 1.0,   0.963082 - 1.0, 0.931486 - 1.0, 0.880754 - 1.0,
      0.863709 - 1.0, 0.858185 - 1.0, 1.23816 - 1.0,  1.40696 - 1.0,
      1.36597 - 1.0,  1.17792 - 1.0,  1.13 - 1.0,     1.09614 - 1.0,
      1.05 - 1.0,     1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
  };
  Utils::FillTH1(20, a_vvmjjmr_01l_h, m_h_SysVVMJJMR_01L_H, m_allHists);
  // 2L:
  m_h_SysVVMJJMR_2L_M = std::make_shared<TH1F>(
      "h_SysVVMJJMR_2L_M", "h_SysVVMJJMR_2L_M", 20, 50e3, 150e3);
  m_h_SysVVMJJMR_2L_M->SetDirectory(0);
  Float_t a_vvmjjmr_2l_m[20] = {
      1.0001 - 1.0,   1.0001 - 1.0,   1.17728 - 1.0, 1.15089 - 1.0,
      1.42143 - 1.0,  1.11141 - 1.0,  0.87491 - 1.0, 0.752471 - 1.0,
      0.821382 - 1.0, 0.783624 - 1.0, 1.47401 - 1.0, 1.60257 - 1.0,
      1.65995 - 1.0,  1.38834 - 1.0,  1.2 - 1.0,     1.15 - 1.0,
      1.1 - 1.0,      1.05 - 1.0,     1.0001 - 1.0,  1.0001 - 1.0,
  };
  Utils::FillTH1(20, a_vvmjjmr_2l_m, m_h_SysVVMJJMR_2L_M, m_allHists);

  m_h_SysVVMJJMR_2L_H = std::make_shared<TH1F>(
      "h_SysVVMJJMR_2L_H", "h_SysVVMJJMR_2L_H", 20, 50e3, 150e3);
  m_h_SysVVMJJMR_2L_H->SetDirectory(0);
  Float_t a_vvmjjmr_2l_h[20] = {
      1.0001 - 1.0,   1.05666 - 1.0,  1.23084 - 1.0,  1.18965 - 1.0,
      1.02172 - 1.0,  1.22227 - 1.0,  0.986648 - 1.0, 0.869981 - 1.0,
      0.629216 - 1.0, 0.871045 - 1.0, 1.13969 - 1.0,  1.36373 - 1.0,
      1.76614 - 1.0,  1.8 - 1.0,      1.63538 - 1.0,  1.2 - 1.0,
      0.968119 - 1.0, 1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
  };
  Utils::FillTH1(20, a_vvmjjmr_2l_h, m_h_SysVVMJJMR_2L_H, m_allHists);
  // end of SysVVMJJMR

  //================ Signal JMR Boosted  ==============================

  // JMR VH uncertainties for the boosted VHbb analysis (bmoser)
  // 0L and 1L:
  m_h_SysVHMJJMR_01L_M = std::make_shared<TH1F>(
      "h_SysVHMJJMR_01L_M", "h_SysVHMJJMR_01L_M", 40, 50e3, 250e3);
  m_h_SysVHMJJMR_01L_M->SetDirectory(0);
  Float_t a_vhmjjmr_01l_m[40] = {
      1.09001 - 1.0,  1.05406 - 1.0,  1.13227 - 1.0,  1.16812 - 1.0,
      1.19541 - 1.0,  1.20643 - 1.0,  1.16626 - 1.0,  1.19635 - 1.0,
      1.15379 - 1.0,  1.13403 - 1.0,  1.07974 - 1.0,  0.986524 - 1.0,
      0.901622 - 1.0, 0.835322 - 1.0, 0.820661 - 1.0, 0.908627 - 1.0,
      1.07707 - 1.0,  1.39442 - 1.0,  1.63572 - 1.0,  1.66425 - 1.0,
      1.65574 - 1.0,  1.43861 - 1.0,  1.27096 - 1.0,  1.26795 - 1.0,
      1.10666 - 1.0,  1.00884 - 1.0,  1.06242 - 1.0,  1.05526 - 1.0,
      1.09112 - 1.0,  1.11 - 1.0,     1.13477 - 1.0,  1.16 - 1.0,
      1.20 - 1.0,     1.25 - 1.0,     1.29364 - 1.0,  1.3 - 1.0,
      1.3 - 1.0,      1.3 - 1.0,      1.3 - 1.0,      1.3 - 1.0,
  };
  Utils::FillTH1(40, a_vhmjjmr_01l_m, m_h_SysVHMJJMR_01L_M, m_allHists);

  m_h_SysVHMJJMR_01L_H = std::make_shared<TH1F>(
      "h_SysVHMJJMR_01L_H", "h_SysVHMJJMR_01L_H", 40, 50e3, 250e3);
  m_h_SysVHMJJMR_01L_H->SetDirectory(0);
  Float_t a_vhmjjmr_01l_h[40] = {
      1.01051 - 1.0,  1.05 - 1.0,     1.10 - 1.0,     1.15 - 1.0,
      1.20 - 1.0,     1.25 - 1.0,     1.31874 - 1.0,  1.21876 - 1.0,
      1.21712 - 1.0,  1.24229 - 1.0,  1.1738 - 1.0,   1.0866 - 1.0,
      0.961184 - 1.0, 0.855509 - 1.0, 0.782169 - 1.0, 0.798667 - 1.0,
      0.964736 - 1.0, 1.36929 - 1.0,  1.89125 - 1.0,  2.0453 - 1.0,
      1.90675 - 1.0,  1.59267 - 1.0,  1.24554 - 1.0,  1.11874 - 1.0,
      1.05512 - 1.0,  1.04228 - 1.0,  1.03 - 1.0,     1.02 - 1.0,
      1.01 - 1.0,     1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
  };
  Utils::FillTH1(40, a_vhmjjmr_01l_h, m_h_SysVHMJJMR_01L_H, m_allHists);
  // 2L:
  m_h_SysVHMJJMR_2L_M = std::make_shared<TH1F>(
      "h_SysVHMJJMR_2L_M", "h_SysVHMJJMR_2L_M", 40, 50e3, 250e3);
  m_h_SysVHMJJMR_2L_M->SetDirectory(0);
  Float_t a_vhmjjmr_2l_m[40] = {
      1.0001 - 1.0,   1.12227 - 1.0,  1.14 - 1.0,     1.18168 - 1.0,
      1.33413 - 1.0,  1.41589 - 1.0,  1.52746 - 1.0,  1.58542 - 1.0,
      1.47226 - 1.0,  1.43851 - 1.0,  1.31026 - 1.0,  1.08557 - 1.0,
      0.885218 - 1.0, 0.703766 - 1.0, 0.667372 - 1.0, 0.769597 - 1.0,
      1.08747 - 1.0,  1.6514 - 1.0,   2.37668 - 1.0,  2.8253 - 1.0,
      2.39889 - 1.0,  2.26405 - 1.0,  1.86845 - 1.0,  1.69507 - 1.0,
      1.90474 - 1.0,  1.67949 - 1.0,  1.37009 - 1.0,  1.35 - 1.0,
      1.30 - 1.0,     1.26763 - 1.0,  1.2 - 1.0,      1.15 - 1.0,
      1.1 - 1.0,      1.05 - 1.0,     1.0001 - 1.0,   1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
  };
  Utils::FillTH1(40, a_vhmjjmr_2l_m, m_h_SysVHMJJMR_2L_M, m_allHists);

  m_h_SysVHMJJMR_2L_H = std::make_shared<TH1F>(
      "h_SysVHMJJMR_2L_H", "h_SysVHMJJMR_2L_H", 40, 50e3, 250e3);
  m_h_SysVHMJJMR_2L_H->SetDirectory(0);
  Float_t a_vhmjjmr_2l_h[40] = {
      1.0001 - 1.0,   1.05 - 1.0,     1.1 - 1.0,      1.15 - 1.0,
      1.35736 - 1.0,  1.5 - 1.0,      1.67251 - 1.0,  1.82681 - 1.0,
      2.02617 - 1.0,  1.80851 - 1.0,  1.50853 - 1.0,  1.35866 - 1.0,
      0.992295 - 1.0, 0.705485 - 1.0, 0.619493 - 1.0, 0.62419 - 1.0,
      0.978495 - 1.0, 1.51553 - 1.0,  2.08122 - 1.0,  2.48193 - 1.0,
      3.04949 - 1.0,  2.44445 - 1.0,  2.1942 - 1.0,   1.83662 - 1.0,
      1.56201 - 1.0,  1.3 - 1.0,      1.25 - 1.0,     1.2 - 1.0,
      1.15 - 1.0,     1.05 - 1.0,     1.03 - 1.0,     1.02 - 1.0,
      1.01 - 1.0,     1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
  };
  Utils::FillTH1(40, a_vhmjjmr_2l_h, m_h_SysVHMJJMR_2L_H, m_allHists);
  // end of SysVHMJJMR

  //================ Signal: VHbb Boosted  ==============================//

  // VH PH7 MJ shape uncertainties for the boosted VHbb analysis (bmoser)
  // Merge 0L and 1L for HPSR, keep them separated for LPSR (based on chi2 test
  // with variations)

  //=========================== 0-lep ==============================//
  // LP SR:
  m_h_SysMJVHPH7_0L_LP_M = std::make_shared<TH1F>(
      "h_SysMJVHPH7_0L_LP_M", "h_SysMJVHPH7_0L_LP_M", 20, 50e3, 250e3);
  m_h_SysMJVHPH7_0L_LP_M->SetDirectory(0);
  Float_t a_vhmjph7_0l_lp_m[20] = {
      0.670494 - 1.0, 0.993502 - 1.0, 0.964274 - 1.0, 1.07877 - 1.0,
      0.915951 - 1.0, 0.939847 - 1.0, 0.996085 - 1.0, 1.01658 - 1.0,
      1.0979 - 1.0,   1.16527 - 1.0,  1.44881 - 1.0,  1.39244 - 1.0,
      0.767616 - 1.0, 0.920236 - 1.0, 1.06927 - 1.0,  1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0};
  Utils::FillTH1(20, a_vhmjph7_0l_lp_m, m_h_SysMJVHPH7_0L_LP_M, m_allHists);

  m_h_SysMJVHPH7_0L_LP_H = std::make_shared<TH1F>(
      "h_SysMJVHPH7_0L_LP_H", "h_SysMJVHPH7_0L_LP_H", 20, 50e3, 250e3);
  m_h_SysMJVHPH7_0L_LP_H->SetDirectory(0);
  Float_t a_vhmjph7_0l_lp_h[20] = {
      0.906246 - 1.0, 0.75157 - 1.0,  0.729127 - 1.0, 1.14257 - 1.0,
      0.949488 - 1.0, 0.961864 - 1.0, 0.964752 - 1.0, 0.966762 - 1.0,
      1.1294 - 1.0,   1.05755 - 1.0,  1.63807 - 1.0,  2.02145 - 1.0,
      1.78668 - 1.0,  1.28148 - 1.0,  1.0001 - 1.0,   1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0};
  Utils::FillTH1(20, a_vhmjph7_0l_lp_h, m_h_SysMJVHPH7_0L_LP_H, m_allHists);

  //=========================== 1-lep ==============================//
  // LP SR:
  m_h_SysMJVHPH7_1L_LP_M = std::make_shared<TH1F>(
      "h_SysMJVHPH7_1L_LP_M", "h_SysMJVHPH7_1L_LP_M", 20, 50e3, 250e3);
  m_h_SysMJVHPH7_1L_LP_M->SetDirectory(0);
  Float_t a_vhmjph7_1l_lp_m[20] = {
      0.786147 - 1.0, 0.789548 - 1.0, 0.959236 - 1.0, 0.964331 - 1.0,
      0.922932 - 1.0, 0.932157 - 1.0, 1.00173 - 1.0,  1.06396 - 1.0,
      1.0368 - 1.0,   1.25224 - 1.0,  1.29 - 1.0,     1.32769 - 1.0,
      1.14391 - 1.0,  1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0};
  Utils::FillTH1(20, a_vhmjph7_1l_lp_m, m_h_SysMJVHPH7_1L_LP_M, m_allHists);

  m_h_SysMJVHPH7_1L_LP_H = std::make_shared<TH1F>(
      "h_SysMJVHPH7_1L_LP_H", "h_SysMJVHPH7_1L_LP_H", 20, 50e3, 250e3);
  m_h_SysMJVHPH7_1L_LP_H->SetDirectory(0);
  Float_t a_vhmjph7_1l_lp_h[20] = {
      0.678637 - 1.0, 0.722447 - 1.0, 1.12836 - 1.0, 1.05355 - 1.0,
      1.07264 - 1.0,  0.949175 - 1.0, 0.96764 - 1.0, 0.970103 - 1.0,
      1.11397 - 1.0,  1.19327 - 1.0,  1.25 - 1.0,    1.32636 - 1.0,
      1.24916 - 1.0,  1.0495 - 1.0,   1.0001 - 1.0,  1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,  1.0001 - 1.0,
  };
  Utils::FillTH1(20, a_vhmjph7_1l_lp_h, m_h_SysMJVHPH7_1L_LP_H, m_allHists);

  // HP SR:
  // --> combined 0L and 1L
  m_h_SysMJVHPH7_01L_HP_M = std::make_shared<TH1F>(
      "h_SysMJVHPH7_01L_HP_M", "h_SysMJVHPH7_01L_HP_M", 20, 50e3, 250e3);
  m_h_SysMJVHPH7_01L_HP_M->SetDirectory(0);
  Float_t a_vhmjph7_01l_hp_m[20] = {
      0.721629 - 1.0, 0.837649 - 1.0, 0.944306 - 1.0, 0.875916 - 1.0,
      0.973692 - 1.0, 0.9682 - 1.0,   0.997944 - 1.0, 1.02803 - 1.0,
      1.13373 - 1.0,  1.16124 - 1.0,  1.27772 - 1.0,  1.0027 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0};
  Utils::FillTH1(20, a_vhmjph7_01l_hp_m, m_h_SysMJVHPH7_01L_HP_M, m_allHists);

  m_h_SysMJVHPH7_01L_HP_H = std::make_shared<TH1F>(
      "h_SysMJVHPH7_01L_HP_H", "h_SysMJVHPH7_01L_HP_H", 20, 50e3, 250e3);
  m_h_SysMJVHPH7_01L_HP_H->SetDirectory(0);
  Float_t a_vhmjph7_01l_hp_h[20] = {
      1.04424 - 1.0,  0.863473 - 1.0, 1.02152 - 1.0,  1.07523 - 1.0,
      0.983151 - 1.0, 0.958348 - 1.0, 0.981634 - 1.0, 1.00532 - 1.0,
      1.06189 - 1.0,  1.04503 - 1.0,  1.02679 - 1.0,  1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
  };
  Utils::FillTH1(20, a_vhmjph7_01l_hp_h, m_h_SysMJVHPH7_01L_HP_H, m_allHists);

  //=========================== 2-lep ==============================//

  m_h_SysMJVHPH7_2L_M = std::make_shared<TH1F>(
      "h_SysMJVHPH7_2L_M", "h_SysMJVHPH7_2L_M", 20, 50e3, 250e3);
  m_h_SysMJVHPH7_2L_M->SetDirectory(0);
  Float_t a_vhmjph7_2l_m[20] = {
      0.750255 - 1.0, 0.764489 - 1.0, 0.863257 - 1.0, 0.947708 - 1.0,
      0.943083 - 1.0, 0.949237 - 1.0, 0.978097 - 1.0, 1.02345 - 1.0,
      1.11378 - 1.0,  1.23595 - 1.0,  1.1 - 1.0,      1.05628 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
  };
  Utils::FillTH1(20, a_vhmjph7_2l_m, m_h_SysMJVHPH7_2L_M, m_allHists);

  m_h_SysMJVHPH7_2L_H = std::make_shared<TH1F>(
      "h_SysMJVHPH7_2L_H", "h_SysMJVHPH7_2L_H", 20, 50e3, 250e3);
  m_h_SysMJVHPH7_2L_H->SetDirectory(0);
  Float_t a_vhmjph7_2l_h[20] = {
      0.661871 - 1.0, 1.10657 - 1.0,  1.02965 - 1.0,  1.05667 - 1.0,
      1.02153 - 1.0,  0.840459 - 1.0, 0.927458 - 1.0, 1.00614 - 1.0,
      1.19424 - 1.0,  1.13707 - 1.0,  1.4 - 1.0,      1.2 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
      1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,   1.0001 - 1.0,
  };
  Utils::FillTH1(20, a_vhmjph7_2l_h, m_h_SysMJVHPH7_2L_H, m_allHists);
  // end of SysMJVHPH7

  //================ W+jet Resolved: BDTr  ==============================

  // updated Z+jets shape systematics from Thomas Charman (top data driven
  // subtraction) 07/08/19

  //=========== Different WPtV shapes in 2j and 3j and according to flavour
  // composition - used to reweight nominal W+jets Sherpa in the context of the
  // pTV-factorized BDTr approach. sgargiul 24.10.2019
  //===========//

  //=========================== 2-jet ==============================//

  m_f_SysWPtV_BDTr_2j_bb = std::make_shared<TF1>(
      "f_SysWPtV_BDTr_2j_bb",
      "x<500e3 ? [0] + ([1] * x)-1   : [0] + ([1] * 500e3) - 1", 0, 600e3);
  m_f_SysWPtV_BDTr_2j_bb->SetParameter(0, 8.94532e-1);
  m_f_SysWPtV_BDTr_2j_bb->SetParameter(1, 8.95169e-4 / 1e3);
  Utils::SaveHist(m_f_SysWPtV_BDTr_2j_bb, m_allHists);

  m_f_SysWPtV_BDTr_2j_bc = std::make_shared<TF1>(
      "f_SysWPtV_BDTr_2j_bc",
      "x<500e3 ? [0] + ([1] * x)-1   : [0] + ([1] * 500e3) - 1", 0, 600e3);
  m_f_SysWPtV_BDTr_2j_bc->SetParameter(0, 9.82703e-1);
  m_f_SysWPtV_BDTr_2j_bc->SetParameter(1, 9.23207e-5 / 1e3);
  Utils::SaveHist(m_f_SysWPtV_BDTr_2j_bc, m_allHists);

  m_f_SysWPtV_BDTr_2j_bl = std::make_shared<TF1>(
      "f_SysWPtV_BDTr_2j_bl",
      "x<500e3 ? [0] + ([1] * x)-1   : [0] + ([1] * 500e3) - 1", 0, 600e3);
  m_f_SysWPtV_BDTr_2j_bl->SetParameter(0, 6.74155e-1);
  m_f_SysWPtV_BDTr_2j_bl->SetParameter(1, 2.49819e-3 / 1e3);
  Utils::SaveHist(m_f_SysWPtV_BDTr_2j_bl, m_allHists);

  m_f_SysWPtV_BDTr_2j_cc = std::make_shared<TF1>(
      "f_SysWPtV_BDTr_2j_cc",
      "x<500e3 ? [0] + ([1] * x)-1   : [0] + ([1] * 500e3) - 1", 0, 600e3);
  m_f_SysWPtV_BDTr_2j_cc->SetParameter(0, 9.57816e-1);
  m_f_SysWPtV_BDTr_2j_cc->SetParameter(1, 3.69981e-4 / 1e3);
  Utils::SaveHist(m_f_SysWPtV_BDTr_2j_cc, m_allHists);

  //=========================== 3-jet ==============================//

  m_f_SysWPtV_BDTr_3j_bb = std::make_shared<TF1>(
      "f_SysWPtV_BDTr_3j_bb",
      "x<500e3 ? [0] + ([1] * x)-1   : [0] + ([1] * 500e3) - 1", 0, 600e3);
  m_f_SysWPtV_BDTr_3j_bb->SetParameter(0, 8.52079e-1);
  m_f_SysWPtV_BDTr_3j_bb->SetParameter(1, 1.03908e-3 / 1e3);
  Utils::SaveHist(m_f_SysWPtV_BDTr_3j_bb, m_allHists);

  m_f_SysWPtV_BDTr_3j_bc = std::make_shared<TF1>(
      "f_SysWPtV_BDTr_3j_bc",
      "x<500e3 ? [0] + ([1] * x)-1   : [0] + ([1] * 500e3) - 1", 0, 600e3);
  m_f_SysWPtV_BDTr_3j_bc->SetParameter(0, 9.21754e-1);
  m_f_SysWPtV_BDTr_3j_bc->SetParameter(1, 5.71938e-4 / 1e3);
  Utils::SaveHist(m_f_SysWPtV_BDTr_3j_bc, m_allHists);

  m_f_SysWPtV_BDTr_3j_bl = std::make_shared<TF1>(
      "f_SysWPtV_BDTr_3j_bl",
      "x<500e3 ? [0] + ([1] * x)-1   : [0] + ([1] * 500e3) - 1", 0, 600e3);
  m_f_SysWPtV_BDTr_3j_bl->SetParameter(0, 7.41833e-1);
  m_f_SysWPtV_BDTr_3j_bl->SetParameter(1, 1.81989e-3 / 1e3);
  Utils::SaveHist(m_f_SysWPtV_BDTr_3j_bl, m_allHists);

  m_f_SysWPtV_BDTr_3j_cc = std::make_shared<TF1>(
      "f_SysWPtV_BDTr_3j_cc",
      "x<500e3 ? [0] + ([1] * x)-1   : [0] + ([1] * 500e3) - 1", 0, 600e3);
  m_f_SysWPtV_BDTr_3j_cc->SetParameter(0, 8.96505e-1);
  m_f_SysWPtV_BDTr_3j_cc->SetParameter(1, 7.34927e-4 / 1e3);
  Utils::SaveHist(m_f_SysWPtV_BDTr_3j_cc, m_allHists);

  //================ TTbar Resolved: BDTr  ==============================//

  //=========== Different TTbarPtV shapes in 2j and 3j and in 0- and 1- channel
  // used to reweight nominal TTbar PowhegPythia in the context of the
  // pTV-factorized BDTr approach from comparison to aMCATNLoPythia
  //===========//      //kalkhour // 14.11.2019

  //=========================== 0-lep 2-jet ==============================//

  m_f_SysTTbarPtV_BDTr_0l_2j =
      std::make_shared<TF1>("f_SysTTbarPtV_BDTr_0l_2j",
                            " x< 500e03 ? [0] + ([1] * x) + ([2] * x * x) -1 : "
                            "[0] + ([1] * 500e03) + ([2] * 500e03 * 500e03) -1",
                            0, 600e3);
  m_f_SysTTbarPtV_BDTr_0l_2j->SetParameter(0, 1.49148);
  m_f_SysTTbarPtV_BDTr_0l_2j->SetParameter(1, -0.00396242 / 1e03);
  m_f_SysTTbarPtV_BDTr_0l_2j->SetParameter(2, 6.64736e-06 / 1e06);
  Utils::SaveHist(m_f_SysTTbarPtV_BDTr_0l_2j, m_allHists);

  //=========================== 0-lep 3-jet ==============================//

  m_f_SysTTbarPtV_BDTr_0l_3j =
      std::make_shared<TF1>("f_SysTTbarPtV_BDTr_0l_3j",
                            "x< 500e03 ? [0] + ([1] * x) + ([2] * x * x) -1: "
                            "[0] + ([1] * 500e03) + ([2] * 500e03 * 500e03) -1",
                            0, 600e3);
  m_f_SysTTbarPtV_BDTr_0l_3j->SetParameter(0, 1.215);
  m_f_SysTTbarPtV_BDTr_0l_3j->SetParameter(1, -0.00134936 / 1e3);
  m_f_SysTTbarPtV_BDTr_0l_3j->SetParameter(2, 9.57408e-07 / 1e6);
  Utils::SaveHist(m_f_SysTTbarPtV_BDTr_0l_3j, m_allHists);

  //=========================== 1-lep 2-jet ==============================//

  m_f_SysTTbarPtV_BDTr_1l_2j =
      std::make_shared<TF1>("f_SysTTbarPtV_BDTr_1l_2j",
                            " x< 500e03 ? [0] + ([1] * x) + ([2] * x * x) -1 : "
                            "[0] + ([1] * 500e03) + ([2] * 500e03 * 500e03) -1",
                            0, 600e3);
  m_f_SysTTbarPtV_BDTr_1l_2j->SetParameter(0, 1.12756);
  m_f_SysTTbarPtV_BDTr_1l_2j->SetParameter(1, -0.00133424 / 1e3);
  m_f_SysTTbarPtV_BDTr_1l_2j->SetParameter(2, 1.84207e-06 / 1e6);
  Utils::SaveHist(m_f_SysTTbarPtV_BDTr_1l_2j, m_allHists);

  //=========================== 1-lep 3-jet ==============================//

  m_f_SysTTbarPtV_BDTr_1l_3j =
      std::make_shared<TF1>("f_SysTTbarPtV_BDTr_1l_3j",
                            " x< 500e03 ? [0] + ([1] * x) + ([2] * x * x) -1 : "
                            "[0] + ([1] * 500e03) + ([2] * 500e03 * 500e03) -1",
                            0, 600e3);
  m_f_SysTTbarPtV_BDTr_1l_3j->SetParameter(0, 1.15361);
  m_f_SysTTbarPtV_BDTr_1l_3j->SetParameter(1, -0.00148613 / 1e3);
  m_f_SysTTbarPtV_BDTr_1l_3j->SetParameter(2, 1.97925e-06 / 1e6);
  Utils::SaveHist(m_f_SysTTbarPtV_BDTr_1l_3j, m_allHists);

  //====================================================================//
  //====================================================================//

  //===================  Z+jet Resolved:  ==============================//
  // tcharman

  m_f_SysZPtV = std::make_shared<TF1>(
      "f_SysZPtV", "x>10e3 ? [0]*log10(x/50e3)-[1] : [0]*log10(10e3/50e3)-[1]",
      0, 400e3);
  m_f_SysZPtV->SetParameter(0, -1.97e-1);
  // This parameter helps avoid an overall normalisation shift for ptV>150 GeV
  // required for pt>75 GeV analyses
  m_f_SysZPtV->SetParameter(1, -1.06);
  Utils::SaveHist(m_f_SysZPtV, m_allHists);

  //=========================================================//

  m_f_SysZMbb = std::make_shared<TF1>(
      "f_SysZMbb", "x > [2] ? ([0] * [2]) - [1] : ([0] * x) - [1]", 0, 400e3);
  m_f_SysZMbb->SetParameter(0, 4.81e-7);
  m_f_SysZMbb->SetParameter(1, -9.36e-1);
  m_f_SysZMbb->SetParameter(2, 300e3); // constant above
  Utils::SaveHist(m_f_SysZMbb, m_allHists);

  //============ Z+jet: Boosted =============================//
  // !!!!!!!!!Unit MeV!!!!!!!!!! //

  //============ Z+jets: 0-lep ==============================//
  m_f_SysZMbbBoosted_Zvv_HP_250_400 = std::make_shared<TF1>(
      "f_SysZMbbBoosted_Zvv_HP_250_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysZMbbBoosted_Zvv_HP_250_400->SetParameter(0, -1.53639);
  m_f_SysZMbbBoosted_Zvv_HP_250_400->SetParameter(1, -0.012519);
  m_f_SysZMbbBoosted_Zvv_HP_250_400->SetParameter(2, -0.0854672);
  Utils::SaveHist(m_f_SysZMbbBoosted_Zvv_HP_250_400, m_allHists);

  m_f_SysZMbbBoosted_Zvv_HP_400 = std::make_shared<TF1>(
      "f_SysZMbbBoosted_Zvv_HP_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysZMbbBoosted_Zvv_HP_400->SetParameter(0, -1.47839);
  m_f_SysZMbbBoosted_Zvv_HP_400->SetParameter(1, -0.0127538);
  m_f_SysZMbbBoosted_Zvv_HP_400->SetParameter(2, -0.0721286);
  Utils::SaveHist(m_f_SysZMbbBoosted_Zvv_HP_400, m_allHists);

  //========================================================//

  m_f_SysZMbbBoosted_Zvv_LP_250_400 = std::make_shared<TF1>(
      "f_SysZMbbBoosted_Zvv_LP_250_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysZMbbBoosted_Zvv_LP_250_400->SetParameter(0, -1.61041);
  m_f_SysZMbbBoosted_Zvv_LP_250_400->SetParameter(1, -0.0175978);
  m_f_SysZMbbBoosted_Zvv_LP_250_400->SetParameter(2, -0.0533993);
  Utils::SaveHist(m_f_SysZMbbBoosted_Zvv_LP_250_400, m_allHists);

  m_f_SysZMbbBoosted_Zvv_LP_400 = std::make_shared<TF1>(
      "f_SysZMbbBoosted_Zvv_LP_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysZMbbBoosted_Zvv_LP_400->SetParameter(0, -1.57788);
  m_f_SysZMbbBoosted_Zvv_LP_400->SetParameter(1, -0.0226178);
  m_f_SysZMbbBoosted_Zvv_LP_400->SetParameter(2, -0.032578);
  Utils::SaveHist(m_f_SysZMbbBoosted_Zvv_LP_400, m_allHists);

  //=============== Z+jets: MUR 2-lep =========================//

  m_f_SysZMbbBoosted_Zll_250_400 = std::make_shared<TF1>(
      "f_SysZMbbBoosted_Zll_250_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysZMbbBoosted_Zll_250_400->SetParameter(0, -1.2689);
  m_f_SysZMbbBoosted_Zll_250_400->SetParameter(1, -0.00518446);
  m_f_SysZMbbBoosted_Zll_250_400->SetParameter(2, -0.181333);
  Utils::SaveHist(m_f_SysZMbbBoosted_Zll_250_400, m_allHists);

  m_f_SysZMbbBoosted_Zll_400 = std::make_shared<TF1>(
      "f_SysZMbbBoosted_Zll_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysZMbbBoosted_Zll_400->SetParameter(0, -1.80583);
  m_f_SysZMbbBoosted_Zll_400->SetParameter(1, -0.00616441);
  m_f_SysZMbbBoosted_Zll_400->SetParameter(2, -0.0879948);
  Utils::SaveHist(m_f_SysZMbbBoosted_Zll_400, m_allHists);

  //============ W+jet: Boosted =============================//

  //=============W+jets MUR 0Lep HP =========================//
  m_f_SysWMbbBoosted_0L_HP_250_400 = std::make_shared<TF1>(
      "f_SysWMbbBoosted_0L_HP_250_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysWMbbBoosted_0L_HP_250_400->SetParameter(0, -1.26742);
  m_f_SysWMbbBoosted_0L_HP_250_400->SetParameter(1, -0.0045132);
  m_f_SysWMbbBoosted_0L_HP_250_400->SetParameter(2, -0.19825);
  Utils::SaveHist(m_f_SysWMbbBoosted_0L_HP_250_400, m_allHists);

  m_f_SysWMbbBoosted_0L_HP_400 = std::make_shared<TF1>(
      "f_SysWMbbBoosted_0L_HP_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysWMbbBoosted_0L_HP_400->SetParameter(0, -1.53939);
  m_f_SysWMbbBoosted_0L_HP_400->SetParameter(1, -0.0130373);
  m_f_SysWMbbBoosted_0L_HP_400->SetParameter(2, -0.0654451);
  Utils::SaveHist(m_f_SysWMbbBoosted_0L_HP_400, m_allHists);

  //=============W+jets MUR 0Lep LP =========================//

  m_f_SysWMbbBoosted_0L_LP_250_400 = std::make_shared<TF1>(
      "f_SysWMbbBoosted_0L_LP_250_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysWMbbBoosted_0L_LP_250_400->SetParameter(0, -1.45311);
  m_f_SysWMbbBoosted_0L_LP_250_400->SetParameter(1, -0.0227019);
  m_f_SysWMbbBoosted_0L_LP_250_400->SetParameter(2, -0.0462185);
  Utils::SaveHist(m_f_SysWMbbBoosted_0L_LP_250_400, m_allHists);

  m_f_SysWMbbBoosted_0L_LP_400 = std::make_shared<TF1>(
      "f_SysWMbbBoosted_0L_LP_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysWMbbBoosted_0L_LP_400->SetParameter(0, -0.0100003);
  m_f_SysWMbbBoosted_0L_LP_400->SetParameter(1, -0.0816199);
  m_f_SysWMbbBoosted_0L_LP_400->SetParameter(2, -0.0107762);
  Utils::SaveHist(m_f_SysWMbbBoosted_0L_LP_400, m_allHists);

  //=============W+jets MUR 1Lep HP =========================//

  m_f_SysWMbbBoosted_1L_HP_250_400 = std::make_shared<TF1>(
      "f_SysWMbbBoosted_1L_HP_250_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysWMbbBoosted_1L_HP_250_400->SetParameter(0, -1.64359);
  m_f_SysWMbbBoosted_1L_HP_250_400->SetParameter(1, -0.00717458);
  m_f_SysWMbbBoosted_1L_HP_250_400->SetParameter(2, -0.113973);
  Utils::SaveHist(m_f_SysWMbbBoosted_1L_HP_250_400, m_allHists);

  m_f_SysWMbbBoosted_1L_HP_400 = std::make_shared<TF1>(
      "f_SysWMbbBoosted_1L_HP_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysWMbbBoosted_1L_HP_400->SetParameter(0, -1.43702);
  m_f_SysWMbbBoosted_1L_HP_400->SetParameter(1, -0.0151103);
  m_f_SysWMbbBoosted_1L_HP_400->SetParameter(2, -0.0637308);
  Utils::SaveHist(m_f_SysWMbbBoosted_1L_HP_400, m_allHists);

  //=============W+jets MUR 1Lep LP =========================//

  m_f_SysWMbbBoosted_1L_LP_250_400 = std::make_shared<TF1>(
      "f_SysWMbbBoosted_1L_LP_250_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysWMbbBoosted_1L_LP_250_400->SetParameter(0, -1.70977);
  m_f_SysWMbbBoosted_1L_LP_250_400->SetParameter(1, -0.0168838);
  m_f_SysWMbbBoosted_1L_LP_250_400->SetParameter(2, -0.0505894);
  Utils::SaveHist(m_f_SysWMbbBoosted_1L_LP_250_400, m_allHists);

  m_f_SysWMbbBoosted_1L_LP_400 = std::make_shared<TF1>(
      "f_SysWMbbBoosted_1L_LP_400", " exp([0]+[1]*x*1e-3)+[2]", 0, 300e3);
  m_f_SysWMbbBoosted_1L_LP_400->SetParameter(0, -1.85013);
  m_f_SysWMbbBoosted_1L_LP_400->SetParameter(1, -0.0193449);
  m_f_SysWMbbBoosted_1L_LP_400->SetParameter(2, -0.0309488);
  Utils::SaveHist(m_f_SysWMbbBoosted_1L_LP_400, m_allHists);

  //============ Diboson: Boosted =============================//

  //=================== Diboson: MUR ==========================//
  m_f_SysVVMURMJBoosted = std::make_shared<TF1>(
      "m_f_SysVVMURMJBoosted", "[0]+[1]*tanh((x*1e-3-[2])/[3])", 0, 500e3);
  m_f_SysVVMURMJBoosted->SetParameter(0, 0.956398 - 1);
  m_f_SysVVMURMJBoosted->SetParameter(1, -0.0631497);
  m_f_SysVVMURMJBoosted->SetParameter(2, 116.326);
  m_f_SysVVMURMJBoosted->SetParameter(3, 16.5256);
  Utils::SaveHist(m_f_SysVVMURMJBoosted, m_allHists);

  //================= Diboson PWPy8 vs. Sherpa HP ================//
  m_f_SysVVPP8MJBoosted_HP =
      std::make_shared<TF1>("m_f_SysVVPP8MJBoosted_HP",
                            "x*1e-3 < (-sqrt([2]*[2]-3*[1]*[3])-[2])/(3*[3]) "
                            "? [0]+[1]*x*1e-3+[2]*x*x*1e-6+[3]*x*x*x*1e-9"
                            ": [0]+ "
                            "(2*[2]*[2]*[2]-9*[1]*[2]*[3]+(2*[2]*[2]-6*[3]*[1])"
                            "*sqrt([2]*[2]-3*[1]*[3]))/(27*[3]*[3])",
                            0, 500e3);
  m_f_SysVVPP8MJBoosted_HP->SetParameter(0, 1.33257 - 1);
  m_f_SysVVPP8MJBoosted_HP->SetParameter(1, -0.0205943);
  m_f_SysVVPP8MJBoosted_HP->SetParameter(2, 0.000260714);
  m_f_SysVVPP8MJBoosted_HP->SetParameter(3, -8.0449e-07);
  Utils::SaveHist(m_f_SysVVPP8MJBoosted_HP, m_allHists);

  //================= Diboson PWPy8 vs. Sherpa LP ================//

  m_f_SysVVPP8MJBoosted_LP =
      std::make_shared<TF1>("m_f_SysVVPP8MJBoosted_LP",
                            "x*1e-3 < (sqrt([2]*[2]-3*[1]*[3])-[2])/(3*[3]) "
                            "? [0]+[1]*x*1e-3+[2]*x*x*1e-6+[3]*x*x*x*1e-9"
                            ": [0]+ "
                            "(2*[2]*[2]*[2]-9*[1]*[2]*[3]-(2*[2]*[2]-6*[3]*[1])"
                            "*sqrt([2]*[2]-3*[1]*[3]))/(27*[3]*[3])",
                            0, 500e3);
  m_f_SysVVPP8MJBoosted_LP->SetParameter(0, -1.20674 - 1);
  m_f_SysVVPP8MJBoosted_LP->SetParameter(1, 0.0608031);
  m_f_SysVVPP8MJBoosted_LP->SetParameter(2, -0.00050379);
  m_f_SysVVPP8MJBoosted_LP->SetParameter(3, 1.21692e-06);
  Utils::SaveHist(m_f_SysVVPP8MJBoosted_LP, m_allHists);

  //================= Draw options: ================//

  if (m_draw) {
    TString fname("CorrsAndSysts_");
    if (m_zero) {
      fname.Append("Zero");
    }
    if (m_one) {
      fname.Append("One");
    }
    if (m_two) {
      fname.Append("Two");
    }
    fname.Append("Lep_13TeV.root");
    WriteHistsToFile(fname);
  }

} // Initialize

CAS::EventType CorrsAndSysts::GetEventType(TString name) {

  // often the name needs to be adapted to a different convention
  if (m_typeNames.find(name.Data()) == m_typeNames.end()) {
    return CAS::NONAME;
  }
  return m_typeNames[name.Data()];
} // GetEventType

CAS::DetailEventType CorrsAndSysts::GetDetailEventType(TString name) {
  if (m_detailtypeNames.find(name.Data()) == m_detailtypeNames.end()) {
    return CAS::NODETAILNAME;
  }
  return m_detailtypeNames[name.Data()];
} // GetEventType

/*********************************************
 *
 *  CORRECTIONS continued - functions below
 *
 *********************************************/

float CorrsAndSysts::Get_HiggsNLOEWKCorrection(CAS::EventType type, float VpT) {
  float scale = 1;
  float tmpVpT = VpT;
  if (VpT < 25e3)
    tmpVpT = 25e3;
  if (VpT > 500e3)
    tmpVpT = 500e3;
  switch (type) {
  case CAS::WHlvbb:
    scale = 1. + Utils::GetScale(tmpVpT, m_h_WHlvbbNLOEWKCorrection);
    break;
  case CAS::qqZHllbb:
    scale = 1. + Utils::GetScale(tmpVpT, m_h_ZHllbbNLOEWKCorrection);
    break;
  case CAS::qqZHvvbb:
    scale = 1. + Utils::GetScale(tmpVpT, m_h_ZHvvbbNLOEWKCorrection);
    break;
    /* BM 15.07.20 decided to not apply the EW corrs to the gg-initiated samples
  until we have dedicated corrs case CAS::ggZHllbb: scale = 1. +
  Utils::GetScale(tmpVpT, m_h_ZHllbbNLOEWKCorrection); break; case
  CAS::ggZHvvbb: scale = 1. + Utils::GetScale(tmpVpT,
  m_h_ZHvvbbNLOEWKCorrection); break;*/
  case CAS::WHlvcc:
    scale = 1. + Utils::GetScale(tmpVpT, m_h_WHlvbbNLOEWKCorrection);
    break;
  case CAS::qqZHllcc:
    scale = 1. + Utils::GetScale(tmpVpT, m_h_ZHllbbNLOEWKCorrection);
    break;
  case CAS::qqZHvvcc:
    scale = 1. + Utils::GetScale(tmpVpT, m_h_ZHvvbbNLOEWKCorrection);
    break;
    /*
  case CAS::ggZHllcc:
    scale = 1. + Utils::GetScale(tmpVpT, m_h_ZHllbbNLOEWKCorrection);
    break;
  case CAS::ggZHvvcc:
    scale = 1. + Utils::GetScale(tmpVpT, m_h_ZHvvbbNLOEWKCorrection);
    break;*/
  default:
    scale = 1;
    break;
  }
  return scale;
}

/*********************************************
 *
 *  SYSTEMATICS continued - functions below
 *
 *********************************************/

float CorrsAndSysts::Get_SystematicWeight(CAS::EventType type, float VpT,
                                          float Mbb, float truthPt, int njet,
                                          int ntag, CAS::Systematic sys,
                                          CAS::SysVar var, bool print) {

  // no systematics
  if (sys == CAS::Nominal)
    return 1;

  // Recycling pre-existing function arguments (Horrific hack but time is
  // precious, so forgive me).
  bool doSherpaSyst = false;
  if (truthPt == 2) {
    doSherpaSyst = true;
  }

  // Local scope variables that retain the returning scale factor 'scale' and
  // the sign of the scale factor
  float scale = 0;
  int sgn = !doSherpaSyst ? 2 * var - 1 : 1;
  TH1 *tmpHist = nullptr;

  if (print)
    std::cout
        << "<CorrsAndSysts::Get_SystematicWeight()> Searching for CA:: enum "
        << std::endl;
  switch (sys) {
    float tmpMbb;
    float tmpVpT;
  case CAS::SysVHNLOEWK:
    tmpVpT = VpT;
    if (VpT < 25e3)
      tmpVpT = 25e3;
    if (VpT > 500e3)
      tmpVpT = 500e3;
    if (!((type == CAS::WHlvbb) || (type == CAS::qqZHllbb) ||
          (type == CAS::qqZHvvbb) || (type == CAS::WHlvcc) ||
          (type == CAS::qqZHllcc) || (type == CAS::qqZHvvcc)))
      return 1;
    if (type == CAS::WHlvbb || type == CAS::WHlvcc) {
      float tmpvar = Utils::GetScale(tmpVpT, m_h_WHlvbbNLOEWKCorrection);
      scale = 0.015;
      if (tmpvar * tmpvar > scale)
        scale = tmpvar * tmpvar;
    }
    if (type == CAS::qqZHllbb || type == CAS::qqZHllcc) {
      float tmpvar = Utils::GetScale(tmpVpT, m_h_ZHllbbNLOEWKCorrection);
      scale = 0.01;
      if (tmpvar * tmpvar > scale)
        scale = tmpvar * tmpvar;
    }
    if (type == CAS::qqZHvvbb || type == CAS::qqZHvvcc) {
      float tmpvar = Utils::GetScale(tmpVpT, m_h_ZHvvbbNLOEWKCorrection);
      scale = 0.01;
      if (tmpvar * tmpvar > scale)
        scale = tmpvar * tmpvar;
    }
    break;
  case CAS::SysVHQCDscalePTV:
    tmpVpT = VpT;
    if (VpT > 300e3)
      tmpVpT = 300e3;
    if (!((type == CAS::WHlvbb) || (type == CAS::qqZHllbb) ||
          (type == CAS::qqZHvvbb)))
      return 1;
    if (njet == 2) {
      if (type == CAS::WHlvbb)
        scale = m_f_SysVHQCDscalePTV_WlvH_2j->Eval(tmpVpT);
      if (type == CAS::qqZHvvbb)
        scale = m_f_SysVHQCDscalePTV_ZvvH_2j->Eval(tmpVpT);
      if (type == CAS::qqZHllbb)
        scale = m_f_SysVHQCDscalePTV_ZllH_2j->Eval(tmpVpT);
    } else {
      if (type == CAS::WHlvbb)
        scale = m_f_SysVHQCDscalePTV_WlvH_3j->Eval(tmpVpT);
      if (type == CAS::qqZHvvbb)
        scale = m_f_SysVHQCDscalePTV_ZvvH_3j->Eval(tmpVpT);
      if (type == CAS::qqZHllbb)
        scale = m_f_SysVHQCDscalePTV_ZllH_3j->Eval(tmpVpT);
    }
    break;
  case CAS::SysVHQCDscaleMbb:
    tmpMbb = Mbb;
    if (Mbb < 50e3)
      tmpMbb = 50e3;
    if (Mbb > 300e3)
      tmpMbb = 300e3;
    if (!((type == CAS::WHlvbb) || (type == CAS::qqZHllbb) ||
          (type == CAS::qqZHvvbb)))
      return 1;
    if (type == CAS::WHlvbb)
      scale = m_f_SysVHQCDscaleMbb_WlvH->Eval(tmpMbb);
    if (type == CAS::qqZHvvbb)
      scale = m_f_SysVHQCDscaleMbb_ZvvH->Eval(tmpMbb);
    if (type == CAS::qqZHllbb)
      scale = m_f_SysVHQCDscaleMbb_ZllH->Eval(tmpMbb);
    break;
  case CAS::SysVHPDFPTV:
    tmpVpT = VpT;
    if (VpT > 300e3)
      tmpVpT = 300e3;
    if (!((type == CAS::WHlvbb) || (type == CAS::qqZHllbb) ||
          (type == CAS::qqZHvvbb)))
      return 1;
    if (njet == 2) {
      if (type == CAS::WHlvbb)
        scale = m_f_SysVHPDFPTV_WlvH_2j->Eval(tmpVpT);
      if (type == CAS::qqZHvvbb)
        scale = m_f_SysVHPDFPTV_ZvvH_2j->Eval(tmpVpT);
      if (type == CAS::qqZHllbb)
        scale = m_f_SysVHPDFPTV_ZllH_2j->Eval(tmpVpT);
    } else {
      if (type == CAS::WHlvbb)
        scale = m_f_SysVHPDFPTV_WlvH_3j->Eval(tmpVpT);
      if (type == CAS::qqZHvvbb)
        scale = m_f_SysVHPDFPTV_ZvvH_3j->Eval(tmpVpT);
      if (type == CAS::qqZHllbb)
        scale = m_f_SysVHPDFPTV_ZllH_3j->Eval(tmpVpT);
    }
    break;
  case CAS::SysVHQCDscalePTV_ggZH:
    tmpVpT = VpT;
    if (VpT > 300e3)
      tmpVpT = 300e3;
    if (!((type == CAS::ggZHvvbb) || (type == CAS::ggZHllbb)))
      return 1;
    if (njet == 2) {
      if (type == CAS::ggZHvvbb)
        scale = m_f_SysVHQCDscalePTV_ZvvH_2j->Eval(tmpVpT);
      if (type == CAS::ggZHllbb)
        scale = m_f_SysVHQCDscalePTV_ZllH_2j->Eval(tmpVpT);
    } else {
      if (type == CAS::ggZHvvbb)
        scale = m_f_SysVHQCDscalePTV_ZvvH_3j->Eval(tmpVpT);
      if (type == CAS::ggZHllbb)
        scale = m_f_SysVHQCDscalePTV_ZllH_3j->Eval(tmpVpT);
    }
    break;
  case CAS::SysVHQCDscaleMbb_ggZH:
    tmpMbb = Mbb;
    if (Mbb < 50e3)
      tmpMbb = 50e3;
    if (Mbb > 300e3)
      tmpMbb = 300e3;
    if (!((type == CAS::ggZHvvbb) || (type == CAS::ggZHllbb)))
      return 1;
    if (type == CAS::ggZHvvbb)
      scale = m_f_SysVHQCDscaleMbb_ZvvH->Eval(tmpMbb);
    if (type == CAS::ggZHllbb)
      scale = m_f_SysVHQCDscaleMbb_ZllH->Eval(tmpMbb);
    break;
  case CAS::SysVHPDFPTV_ggZH:
    tmpVpT = VpT;
    if (VpT > 300e3)
      tmpVpT = 300e3;
    if (!((type == CAS::ggZHvvbb) || (type == CAS::ggZHllbb)))
      return 1;
    if (njet == 2) {
      if (type == CAS::ggZHvvbb)
        scale = m_f_SysVHPDFPTV_ZvvH_2j->Eval(tmpVpT);
      if (type == CAS::ggZHllbb)
        scale = m_f_SysVHPDFPTV_ZllH_2j->Eval(tmpVpT);
    } else {
      if (type == CAS::ggZHvvbb)
        scale = m_f_SysVHPDFPTV_ZvvH_3j->Eval(tmpVpT);
      if (type == CAS::ggZHllbb)
        scale = m_f_SysVHPDFPTV_ZllH_3j->Eval(tmpVpT);
    }
    break;
  case CAS::SysVHUEPSPTV:
    tmpVpT = VpT;
    if (VpT > 300e3)
      tmpVpT = 300e3;
    if (!((type == CAS::WHlvbb) || (type == CAS::qqZHllbb) ||
          (type == CAS::qqZHvvbb) || (type == CAS::ggZHvvbb) ||
          (type == CAS::ggZHllbb)))
      return 1;
    if (njet == 2) {
      if (type == CAS::WHlvbb)
        scale = m_f_SysVHUEPSPTV_WlvH_2j->Eval(tmpVpT);
      if (type == CAS::qqZHvvbb || type == CAS::ggZHvvbb)
        scale = m_f_SysVHUEPSPTV_ZvvH_2j->Eval(tmpVpT);
      if (type == CAS::qqZHllbb || type == CAS::ggZHllbb)
        scale = m_f_SysVHUEPSPTV_ZllH_2j->Eval(tmpVpT);
    } else {
      if (type == CAS::WHlvbb)
        scale = m_f_SysVHUEPSPTV_WlvH_3j->Eval(tmpVpT);
      if (type == CAS::qqZHvvbb || type == CAS::ggZHvvbb)
        scale = m_f_SysVHUEPSPTV_ZvvH_3j->Eval(tmpVpT);
      if (type == CAS::qqZHllbb || type == CAS::ggZHllbb)
        scale = m_f_SysVHUEPSPTV_ZllH_3j->Eval(tmpVpT);
    }
    break;
  case CAS::SysVHUEPSMbb:
    if (!((type == CAS::WHlvbb) || (type == CAS::qqZHllbb) ||
          (type == CAS::qqZHvvbb) || (type == CAS::ggZHvvbb) ||
          (type == CAS::ggZHllbb)))
      return 1;
    tmpMbb = Mbb;
    if (Mbb < 50e3)
      tmpMbb = 50001;
    if (Mbb > 200e3)
      tmpMbb = 199999;
    // pay attention to MeV/GeV
    scale = m_h_SysVHUEPSMbb->GetBinContent(
        m_h_SysVHUEPSMbb->GetXaxis()->FindBin(tmpMbb / 1.e3));
    // /!\ the scale is supposed to be centered at 0
    scale -= 1;
    break;

  // added by francav 15.07.16
  case CAS::SysVVMbbME:
    if (!(type == CAS::WZ || (type == CAS::ZZ && m_zero) ||
          (type == CAS::ZZ && m_two)) ||
        (njet < 2))
      return 1;
    tmpMbb = Mbb;
    if (Mbb < 50e3)
      tmpMbb = 50e3;
    if (Mbb > 300e3)
      tmpMbb = 300e3;
    if (m_zero && type == CAS::ZZ) {
      if (njet == 2)
        scale = m_f_SysVVMbbME_ZZvvqq_2j->Eval(tmpMbb);
      else
        scale = m_f_SysVVMbbME_ZZvvqq_3j->Eval(tmpMbb);
    }
    if (type == CAS::WZ) {
      if (njet == 2)
        scale = m_f_SysVVMbbME_WZlvqq_2j->Eval(tmpMbb);
      else
        scale = m_f_SysVVMbbME_WZlvqq_3j->Eval(tmpMbb);
    }
    if (m_two && type == CAS::ZZ) {
      if (njet == 2)
        scale = m_f_SysVVMbbME_ZZllqq_2j->Eval(tmpMbb);
      else
        scale = m_f_SysVVMbbME_ZZllqq_3j->Eval(tmpMbb);
    }
    break;

  case CAS::SysVVPTVME:
    if (!(type == CAS::WZ || (type == CAS::ZZ && m_zero) ||
          (type == CAS::ZZ && m_two)) ||
        (njet < 2))
      return 1;
    tmpVpT = VpT;
    if (VpT > 300e3)
      tmpVpT = 300e3;
    if (m_zero && type == CAS::ZZ) {
      if (VpT < 150e3)
        tmpVpT = 150e3;
      if (njet == 2)
        return 1.0; // not used since the fit is mostly fluctuations
                    // m_f_SysVVPTVME_ZZvvqq_2j->Eval(tmpVpT);
      else
        scale = m_f_SysVVPTVME_ZZvvqq_3j->Eval(tmpVpT);
    }
    if (type == CAS::WZ) {
      if (VpT < 150e3)
        tmpVpT = 150e3;
      if (njet == 2)
        scale = m_f_SysVVPTVME_WZlvqq_2j->Eval(tmpVpT);
      else
        scale = m_f_SysVVPTVME_WZlvqq_3j->Eval(tmpVpT);
    }
    if (m_two && type == CAS::ZZ) {
      if (njet == 2)
        scale = m_f_SysVVPTVME_ZZllqq_2j->Eval(tmpVpT);
      else
        scale = m_f_SysVVPTVME_ZZllqq_3j->Eval(tmpVpT);
    }
    break;

  case CAS::SysVVMbbPSUE:
    if (!(type == CAS::WZ || (type == CAS::ZZ && m_zero) ||
          (type == CAS::ZZ && m_two)) ||
        (njet < 2))
      return 1;
    tmpMbb = Mbb;
    if (Mbb < 30e3)
      tmpMbb = 30001;
    if (Mbb > 250e3)
      tmpMbb = 249999;
    if (m_zero && type == CAS::ZZ) {
      if (njet == 2)
        tmpHist = m_h_SysVVMbbPSUE_ZZvvqq_2j.get();
      else
        tmpHist = m_h_SysVVMbbPSUE_ZZvvqq_3j.get();
    }
    if (type == CAS::WZ) {
      if (njet == 2)
        tmpHist = m_h_SysVVMbbPSUE_WZlvqq_2j.get();
      else
        tmpHist = m_h_SysVVMbbPSUE_WZlvqq_3j.get();
    }
    if (m_two && type == CAS::ZZ) {
      if (njet == 2)
        tmpHist = m_h_SysVVMbbPSUE_ZZllqq_2j.get();
      else
        tmpHist = m_h_SysVVMbbPSUE_ZZllqq_3j.get();
    }
    // pay attention to MeV/GeV
    scale = tmpHist->GetBinContent(tmpHist->GetXaxis()->FindBin(tmpMbb / 1.e3));
    // /!\ the scale is supposed to be centered at 0
    scale -= 1;
    break;

  case CAS::SysVVPTVPSUE:
    if (!(type == CAS::WZ || (type == CAS::ZZ && m_zero) ||
          (type == CAS::ZZ && m_two)) ||
        (njet < 2))
      return 1;
    tmpVpT = VpT;
    if (VpT > 300e3)
      tmpVpT = 300e3;
    if (m_zero && type == CAS::ZZ) {
      if (VpT < 150e3)
        tmpVpT = 150e3;
      if (njet == 2)
        return 1.0; // not used since the fit is mostly fluctuations
                    // m_f_SysVVPTVPSUE_ZZvvqq_2j->Eval(tmpVpT);
      else
        scale = m_f_SysVVPTVPSUE_ZZvvqq_3j->Eval(tmpVpT);
    }
    if (type == CAS::WZ) {
      if (VpT < 150e3)
        tmpVpT = 150e3;
      if (njet == 2)
        scale = m_f_SysVVPTVPSUE_WZlvqq_2j->Eval(tmpVpT);
      else
        scale = m_f_SysVVPTVPSUE_WZlvqq_3j->Eval(tmpVpT);
    }
    if (m_two && type == CAS::ZZ) {
      if (njet == 2)
        scale = m_f_SysVVPTVPSUE_ZZllqq_2j->Eval(tmpVpT);
      else
        scale = m_f_SysVVPTVPSUE_ZZllqq_3j->Eval(tmpVpT);
    }
    break;

    // added by yama 12.07.16
    // modified by eschopf 10.05.17
  case CAS::SysStoptPTV:
    if (type != CAS::stop_t)
      return 1;
    scale = m_f_SysStoptPTV->Eval(VpT);
    break;

  // ntag: the number of additional b-tagged VR track jets
  // njet: the number of additional small-R calo jets
  case CAS::SysStopWtDSDR: // added by gdigrego 19.11.19
    if (type != CAS::stop_Wt)
      return 1;
    if (m_zero) {            // 0L
      if (ntag == 0) {       // SR
        if (njet == 0) {     // HP SR
          if (VpT < 400e3) { // medium ptv
            scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_0L_Mptv_HPSR);
          } else { // high pTV
            scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_0L_Hptv_HPSR);
          }
        } else if (njet >= 1) { // LP SR
          if (VpT < 400e3) {    // medium ptv
            scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_0L_Mptv_LPSR);
          } else { // high pTV
            scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_0L_Hptv_LPSR);
          }
        }                   // end of LP SR
      }                     // end of SR
      else if (ntag >= 1) { // CR
        if (VpT < 400e3) {  // medium ptv
          if (njet == 0)    // 0 add jets
            scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_0L_Mptv_CR0J);
          else if (njet >= 1) // 1+ add jets
            scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_0L_Mptv_CR1J);
        } else { // high ptv
          if (njet == 0)
            scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_0L_Hptv_CR0J);
          else if (njet >= 1)
            scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_0L_Hptv_CR1J);
        }
      }                      // end of CR
    }                        // end of 0L
    else if (m_one) {        // 1L
      if (ntag == 0) {       // SR
        if (njet == 0) {     // HP SR
          if (VpT < 400e3) { // medium ptv
            scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_1L_Mptv_HPSR);
          } else { // high pTV
            scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_1L_Hptv_HPSR);
          }
        } else if (njet >= 1) { // LP SR
          if (VpT < 400e3) {    // medium ptv
            scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_1L_Mptv_LPSR);
          } else { // high ptv
            scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_1L_Hptv_LPSR);
          }
        }                   // end of LP SR
      }                     // end of SR
      else if (ntag >= 1) { // CR
        if (VpT < 400e3) {  // medium ptv
          scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_1L_Mptv_CR);
        } else { // high ptv
          scale = Utils::GetScale(Mbb, m_h_SysStopWtDSDR_1L_Hptv_CR);
        }
      } // end of CR
    }
    break;

  case CAS::SysStoptMBB:
    if (type != CAS::stop_t)
      return 1;
    scale = m_f_SysStoptMBB->Eval(Mbb);
    break;

  case CAS::SysStopWtPTV:
    if (type == CAS::stop_Wtbb)
      scale = m_f_SysStopWtbbPTV->Eval(VpT);
    else if (type == CAS::stop_Wtoth)
      scale = m_f_SysStopWtothPTV->Eval(VpT);
    else
      return 1;
    break;

  case CAS::SysStopWtMBB:
    if (type == CAS::stop_Wtbb)
      scale = m_f_SysStopWtbbMBB->Eval(Mbb);
    else if (type == CAS::stop_Wtoth)
      scale = m_f_SysStopWtothMBB->Eval(Mbb);
    else
      return 1;
    break;

  case CAS::SysStopWtbbACC:
    if (type == CAS::stop_Wtbb) {
      if (njet == 2)
        scale = 0.549;
      else if (njet >= 3)
        scale = 0.507;
    } else
      return 1;
    break;

  case CAS::SysStopWtothACC:
    if (type == CAS::stop_Wtoth) {
      if (njet == 2)
        scale = 0.241;
      else if (njet >= 3)
        scale = 0.212;
    } else
      return 1;
    break;

    // sjiggins - 17/01/20
  case CAS::SysTTbarbcMEACC:
    if (type == CAS::ttbarbc) {
      if (njet == 2)
        if (m_zero) {
          scale = 1.082 - 1;
        } else if (m_one) {
          scale = 1.013 - 1;
        } else {
          return 1.0;
        }
      else if (njet >= 3) {
        if (m_zero) {
          scale = 1.076 - 1;
        } else if (m_one) {
          scale = 1.038 - 1;
        } else {
          return 1.0;
        }
      } else {
        return 1.0;
      }
    } else {
      return 1.0;
    }
    break;

    // sjiggins - 17/01/20
  case CAS::SysTTbarbcPSACC:
    if (type == CAS::ttbarbc) {
      if (njet == 2)
        if (m_zero) {
          scale = 0.979 - 1;
        } else if (m_one) {
          scale = 1.071 - 1;
        } else {
          return 1.0;
        }

      else if (njet >= 3) {
        if (m_zero) {
          scale = 0.968 - 1;
        } else if (m_one) {
          scale = 1.015 - 1;
        } else {
          return 1.0;
        }
      } else {
        return 1.0;
      }
    } else {
      return 1.0;
    }
    break;

    // sjiggins - 17.01.20
  case CAS::SysTTbarOthPSACC:
    if (type == CAS::ttbaroth) {
      if (njet == 2)
        if (m_zero) {
          scale = 0.868 - 1;
        } else if (m_one) {
          scale = 1.003 - 1;
        } else {
          return 1.0;
        }
      else if (njet >= 3) {
        if (m_zero) {
          scale = 0.944 - 1;
        } else if (m_one) {
          scale = 1.021 - 1;
        } else {
          return 1.0;
        }
      } else {
        return 1.0;
      }
    } else {
      return 1.0;
    }
    break;

  // sjiggins - 17.01.20
  case CAS::SysTTbarOthMEACC:
    if (type == CAS::ttbaroth) {
      if (njet == 2)
        if (m_zero) {
          scale = 0.9364 - 1;
        } else if (m_one) {
          scale = 1.033 - 1;
        } else {
          return 1.0;
          ;
        }
      else if (njet >= 3) {
        if (m_zero) {
          scale = 0.972 - 1;
        } else if (m_one) {
          scale = 1.057 - 1;
        } else {
          return 1.0;
        }
      } else {
        return 1.0;
      }
    } else {
      return 1.0;
    }
    break;

  // mfoti 20/21.11/2019
  case CAS::SysTTbarMJISR:
    if (type != CAS::ttbar)
      return 1;
    if (VpT < 400e3)
      scale = m_f_SysTTbarMJISR_M->Eval(Mbb);
    else
      scale = m_f_SysTTbarMJISR_H->Eval(Mbb);
    break;

  case CAS::SysTTbarMJPS:
    if (type != CAS::ttbar)
      return 1;
    scale = Utils::GetScale(Mbb, m_h_SysTTbarMJPS);
    break;
    // mfoti

    //=========== Different TTbarPtV shapes in 2j and 3j and 0- and 1-lepton
    //===========//      //
    // kalkhour 14.11.19
  case CAS::SysTTbarPtV_BDTr:
    if (type != CAS::ttbar)
      return 1;
    // 0-lepton
    if (m_zero) {
      if (njet == 2)
        scale = m_f_SysTTbarPtV_BDTr_0l_2j->Eval(VpT);
      else if (njet == 3)
        scale = m_f_SysTTbarPtV_BDTr_0l_3j->Eval(VpT);
    }
    // 1-lep
    else if (m_one) {
      if (njet == 2)
        scale = m_f_SysTTbarPtV_BDTr_1l_2j->Eval(VpT);
      else if (njet == 3)
        scale = m_f_SysTTbarPtV_BDTr_1l_3j->Eval(VpT);
    }
    break;

    // Amendment - sjiggins (06.05.19): SysWPtV originally utilised truthPt
    // variable
    //      which stored the transverse momentum of the Sherpa 2.2.1 vector
    //      boson within the event record. During migration to Truth3 xAOD
    //      derivation truth format, original xAOD::TruthParticleContainer is
    //      was empty in v32 CxAODs. Consequently migration to using Reco.
    //      transverse momentum was made to allow for a viable estimation of the
    //      SysWPtV systematic. Migration to using dR matched Reco-to-Truth
    //      scheme will be made for 2019 VHbb publication.
    //=========== Different WPtV shapes in 2j and 3j ===========//      //
    // sgargiul 15.04.19

  case CAS::SysWPtV_BDTr:
    if (type != CAS::W && type != CAS::Wbb && type != CAS::Wbc &&
        type != CAS::Wbl && type != CAS::Wcc)
      return 1;
    if (njet == 2) {
      if (type == CAS::Wbb)
        scale = m_f_SysWPtV_BDTr_2j_bb->Eval(VpT);
      if (type == CAS::Wbc)
        scale = m_f_SysWPtV_BDTr_2j_bc->Eval(VpT);
      if (type == CAS::Wbl)
        scale = m_f_SysWPtV_BDTr_2j_bl->Eval(VpT);
      if (type == CAS::Wcc)
        scale = m_f_SysWPtV_BDTr_2j_cc->Eval(VpT);
    } else if (njet == 3) {
      if (type == CAS::Wbb)
        scale = m_f_SysWPtV_BDTr_3j_bb->Eval(VpT);
      if (type == CAS::Wbc)
        scale = m_f_SysWPtV_BDTr_3j_bc->Eval(VpT);
      if (type == CAS::Wbl)
        scale = m_f_SysWPtV_BDTr_3j_bl->Eval(VpT);
      if (type == CAS::Wcc)
        scale = m_f_SysWPtV_BDTr_3j_cc->Eval(VpT);
    }
    break;

  // Amendment - sjiggins (06.05.19): SysZPtV originally utilised truthPt
  // variable
  //      which stored the transverse momentum of the Sherpa 2.2.1 vector
  //      boson within the event record. During migration to Truth3 xAOD
  //      derivation truth format, original xAOD::TruthParticleContainer is
  //      was empty in v32 CxAODs. Consequently migration to using Reco.
  //      transverse momentum was made to allow for a viable estimation of the
  //      SysZPtV systematic. Migration to using dR matched Reco-to-Truth
  //      scheme will be made for 2019 VHbb publication.
  case CAS::SysZPtV:
    if (type != CAS::Z && type != CAS::Zl && type != CAS::Zcl &&
        type != CAS::Zcc && type != CAS::Zbb && type != CAS::Zbl &&
        type != CAS::Zbc)
      return 1;
    scale = m_f_SysZPtV->Eval(VpT); // Originally truthPt
    scale -= 1;
    break;

  case CAS::SysZMbb:
    if (type != CAS::Z && type != CAS::Zl && type != CAS::Zcl &&
        type != CAS::Zcc && type != CAS::Zbb && type != CAS::Zbl &&
        type != CAS::Zbc)
      return 1;
    scale = m_f_SysZMbb->Eval(Mbb);
    scale -= 1;

    break;

  //========== Boosted VHbb modelling ============

  // njet: the number of additional small-R calo jets
  case CAS::SysZMbbBoosted: // Z+jets MUR
    if (type != CAS::Z && type != CAS::Zl && type != CAS::Zcl &&
        type != CAS::Zcc && type != CAS::Zbb && type != CAS::Zbl &&
        type != CAS::Zbc)
      return 1;
    if (m_zero) {
      if (njet == 0) {     // high purity
        if (VpT < 400e3) { // medium pTV
          scale = m_f_SysZMbbBoosted_Zvv_HP_250_400->Eval(Mbb);
        } else { // high pTV
          scale = m_f_SysZMbbBoosted_Zvv_HP_400->Eval(Mbb);
        }
      } else if (njet >= 1) { // low purity
        if (VpT < 400e3) {    // medium pTV
          scale = m_f_SysZMbbBoosted_Zvv_LP_250_400->Eval(Mbb);
        } else { // high pTV
          scale = m_f_SysZMbbBoosted_Zvv_LP_400->Eval(Mbb);
        }
      } else {
        std::cout << "Warning: 0L number of additional calo jet:  ";
        std::cout << njet << " < 0 , set SysZMbbBoosted to 0 " << std::endl;
        scale = 0;
      }
    } // end of (m_zero)
    else if (m_two) {
      if (VpT < 400e3) { // medium pTV
        scale = m_f_SysZMbbBoosted_Zll_250_400->Eval(Mbb);
      } else { // high pTV
        scale = m_f_SysZMbbBoosted_Zll_400->Eval(Mbb);
      }
    } // end of (m_two)
    else
      return 1;
    break;

  case CAS::SysWMbbBoosted: // W+jets MUR
    if (type != CAS::W && type != CAS::Wl && type != CAS::Wcl &&
        type != CAS::Wcc && type != CAS::Wbb && type != CAS::Wbc &&
        type != CAS::Wbl)
      return 1;
    if (m_zero) {
      if (njet == 0) {     // high purity
        if (VpT < 400e3) { // medium pTV
          scale = m_f_SysWMbbBoosted_0L_HP_250_400->Eval(Mbb);
        } else { // high pTV
          scale = m_f_SysWMbbBoosted_0L_HP_400->Eval(Mbb);
        }
      } else if (njet >= 1) { // low purity
        if (VpT < 400e3) {    // medium pTV
          scale = m_f_SysWMbbBoosted_0L_LP_250_400->Eval(Mbb);
        } else { // high pTV
          scale = m_f_SysWMbbBoosted_0L_LP_400->Eval(Mbb);
        }
      } else {
        std::cout << "Warning: 0L number of additional calo jet:  ";
        std::cout << njet << " < 0 , set SysWMbbBoosted to 0 " << std::endl;
        scale = 0;
      }
    } // end of (m_zero)
    else if (m_one) {
      if (njet == 0) {     // high purity
        if (VpT < 400e3) { // medium pTV
          scale = m_f_SysWMbbBoosted_1L_HP_250_400->Eval(Mbb);
        } else { // high pTV
          scale = m_f_SysWMbbBoosted_1L_HP_400->Eval(Mbb);
        }
      } else if (njet >= 1) { // low purity
        if (VpT < 400e3) {    // medium pTV
          scale = m_f_SysWMbbBoosted_1L_LP_250_400->Eval(Mbb);
        } else { // high pTV
          scale = m_f_SysWMbbBoosted_1L_LP_400->Eval(Mbb);
        }
      } else {
        std::cout << "Warning: 1L number of additional calo jet:  ";
        std::cout << njet << " < 0 , set SysWMbbBoosted to 0 " << std::endl;
        scale = 0;
      }
    } // end of (m_one)
    else
      return 1;
    break;

    // boosted VHbb VV JMR
  case CAS::SysMJJMR: // added by bmoser 04.12.19
    tmpMbb = Mbb;
    if ((type == CAS::WW || type == CAS::WZ || type == CAS::ZZ)) {
      if (m_one || m_zero) {
        if (VpT < 400e3) {
          if (tmpMbb < 50e3)
            tmpMbb = 50e3;
          else if (tmpMbb > 150e3)
            tmpMbb = 149.9e3;
          scale = Utils::GetScale(tmpMbb, m_h_SysVVMJJMR_01L_M);
        } else {
          if (tmpMbb < 50e3)
            tmpMbb = 50e3;
          else if (tmpMbb > 150e3)
            tmpMbb = 149.9e3;
          scale = Utils::GetScale(tmpMbb, m_h_SysVVMJJMR_01L_H);
        }
      } else if (m_two) {
        if (VpT < 400e3) {
          if (tmpMbb < 50e3)
            tmpMbb = 50e3;
          else if (tmpMbb > 150e3)
            tmpMbb = 149.9e3;
          scale = Utils::GetScale(tmpMbb, m_h_SysVVMJJMR_2L_M);
        } else {
          if (tmpMbb < 50e3)
            tmpMbb = 50e3;
          else if (tmpMbb > 150e3)
            tmpMbb = 149.9e3;
          scale = Utils::GetScale(tmpMbb, m_h_SysVVMJJMR_2L_H);
        }
      }
    } else if (type == CAS::WHlvbb || type == CAS::qqZHllbb ||
               type == CAS::qqZHvvbb || type == CAS::ggZHllbb ||
               type == CAS::ggZHvvbb) {
      if (m_one || m_zero) {
        if (VpT < 400e3) {
          if (tmpMbb < 50e3)
            tmpMbb = 50e3;
          else if (tmpMbb > 250e3)
            tmpMbb = 249.9e3;
          scale = Utils::GetScale(tmpMbb, m_h_SysVHMJJMR_01L_M);
        } else {
          if (tmpMbb < 50e3)
            tmpMbb = 50e3;
          else if (tmpMbb > 250e3)
            tmpMbb = 249.9e3;
          scale = Utils::GetScale(tmpMbb, m_h_SysVHMJJMR_01L_H);
        }
      } else if (m_two) {
        if (VpT < 400e3) {
          if (tmpMbb < 50e3)
            tmpMbb = 50e3;
          else if (tmpMbb > 250e3)
            tmpMbb = 249.9e3;
          scale = Utils::GetScale(tmpMbb, m_h_SysVHMJJMR_2L_M);
        } else {
          if (tmpMbb < 50e3)
            tmpMbb = 50e3;
          else if (tmpMbb > 250e3)
            tmpMbb = 249.9e3;
          scale = Utils::GetScale(tmpMbb, m_h_SysVHMJJMR_2L_H);
        }
      }
    } else {
      return 1;
    }

    break;

    // boosted VHbb VH PH7 mJ shape
  case CAS::SysMJVHPH7: // added by bmoser 03.01.20
    tmpMbb = Mbb;
    if (type == CAS::WHlvbb || type == CAS::qqZHllbb || type == CAS::qqZHvvbb ||
        type == CAS::ggZHllbb || type == CAS::ggZHvvbb) {
      if (m_one || m_zero) {
        if (njet == 0) { // HP, i.e. 0L and 1L merged
          if (VpT < 400e3) {
            if (tmpMbb < 50e3)
              tmpMbb = 50e3;
            else if (tmpMbb > 250e3)
              tmpMbb = 249.9e3;
            scale = Utils::GetScale(tmpMbb, m_h_SysMJVHPH7_01L_HP_M);
          } else {
            if (tmpMbb < 50e3)
              tmpMbb = 50e3;
            else if (tmpMbb > 250e3)
              tmpMbb = 249.9e3;
            scale = Utils::GetScale(tmpMbb, m_h_SysMJVHPH7_01L_HP_H);
          }
        } else if (njet >= 1) {
          if (VpT < 400e3) {
            if (tmpMbb < 50e3)
              tmpMbb = 50e3;
            else if (tmpMbb > 250e3)
              tmpMbb = 249.9e3;
            if (m_zero) {
              scale = Utils::GetScale(tmpMbb, m_h_SysMJVHPH7_0L_LP_M);
            } else if (m_one) {
              scale = Utils::GetScale(tmpMbb, m_h_SysMJVHPH7_1L_LP_M);
            }
          } else {
            if (tmpMbb < 50e3)
              tmpMbb = 50e3;
            else if (tmpMbb > 250e3)
              tmpMbb = 249.9e3;
            if (m_zero) {
              scale = Utils::GetScale(tmpMbb, m_h_SysMJVHPH7_0L_LP_H);
            } else if (m_one) {
              scale = Utils::GetScale(tmpMbb, m_h_SysMJVHPH7_1L_LP_H);
            }
          }
        }
      } else if (m_two) {
        if (VpT < 400e3) {
          if (tmpMbb < 50e3)
            tmpMbb = 50e3;
          else if (tmpMbb > 250e3)
            tmpMbb = 249.9e3;
          scale = Utils::GetScale(tmpMbb, m_h_SysMJVHPH7_2L_M);
        } else {
          if (tmpMbb < 50e3)
            tmpMbb = 50e3;
          else if (tmpMbb > 250e3)
            tmpMbb = 249.9e3;
          scale = Utils::GetScale(tmpMbb, m_h_SysMJVHPH7_2L_H);
        }
      }
    } else {
      return 1;
    }

    break;

  case CAS::SysVVMURMJBoosted: // diboson MUR
    if (type != CAS::WZ && type != CAS::ZZ)
      return 1;
    scale = m_f_SysVVMURMJBoosted->Eval(Mbb);
    break;

  case CAS::SysVVPP8MJBoosted: // diboson PwPy8 vs. Sherpa
    if (type != CAS::WZ && type != CAS::ZZ)
      return 1;
    if (m_two) {
      return 1;
    } else {           // only applied in 0 and 1L
      if (njet == 0) { // high purity
        scale = m_f_SysVVPP8MJBoosted_HP->Eval(Mbb);
      } else if (njet >= 1) { // low purity
        scale = m_f_SysVVPP8MJBoosted_LP->Eval(Mbb);
      } else {
        std::cout << "Warning: Number of additional calo jet:  ";
        std::cout << njet << " < 0 , set SysVVPP8MJBoosted to 0 " << std::endl;
        scale = 0;
      }
    }
    break;

    //==================================VHcc===========================================================
    // amendesj @ 20/04/20

  case CAS::SysZhf_shape_ShMGPy8_mCC:
    if (type == CAS::Zcc || type == CAS::Zbb) {
      if (VpT < 150e3) {
        scale = m_f_SysZhf_75_150ptv_shape_ShMGPy8_mCC->Eval(Mbb);
      } else if (VpT > 150e3) {
        scale = m_f_SysZhf_150ptv_shape_ShMGPy8_mCC->Eval(Mbb);
      }
    } else {
      return 1;
    }
    break;

  case CAS::SysZmf_shape_ShMGPy8_mCC:
    if (type == CAS::Zcl || type == CAS::Zbc || type == CAS::Zbl) {
      if (VpT < 150e3) {
        scale = m_f_SysZmf_75_150ptv_shape_ShMGPy8_mCC->Eval(Mbb);
      } else if (VpT > 150e3) {
        scale = m_f_SysZmf_150ptv_shape_ShMGPy8_mCC->Eval(Mbb);
      }
    } else {
      return 1;
    }
    break;

  case CAS::SysZl_shape_ShMGPy8_mCC:
    if (type != CAS::Zl)
      return 1;
    if (njet == 2 && VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_mCC->Eval(Mbb);
    } else if (njet == 2 && VpT > 250e3) {
      scale = m_f_SysZl_2jet_250ptv_shape_ShMGPy8_mCC->Eval(Mbb);
    } else if (njet == 2 && VpT < 150e3) {
      scale = m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_mCC->Eval(Mbb);
    } else if (njet > 2 && VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_mCC->Eval(Mbb);
    } else if (njet > 2 && VpT > 250e3) {
      scale = m_f_SysZl_3jet_250ptv_shape_ShMGPy8_mCC->Eval(Mbb);
    } else if (njet > 2 && VpT < 150e3) {
      scale = m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysWhf_shape_ShMGPy8_mCC:
    if (type == CAS::Wcc || type == CAS::Wbb) {
      scale = m_f_SysWhf_shape_ShMGPy8_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysWmf_shape_ShMGPy8_mCC:
    if (type == CAS::Wcl || type == CAS::Wbc || type == CAS::Wbl) {
      scale = m_f_SysWmf_shape_ShMGPy8_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysWl_shape_ShMGPy8_mCC:
    if (type != CAS::Wl)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysWl_150_250ptv_shape_ShMGPy8_mCC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysWl_250ptv_shape_ShMGPy8_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

    // SR systs - mironova
  case CAS::SysZhf_shape_ShMGPy8_SR_mCC:
    if (type == CAS::Zcc || type == CAS::Zbb) {
      if (VpT < 150e3) {
        scale = m_f_SysZhf_75_150ptv_shape_ShMGPy8_SR_mCC->Eval(Mbb);
      } else if (VpT > 150e3) {
        scale = m_f_SysZhf_150ptv_shape_ShMGPy8_SR_mCC->Eval(Mbb);
      }
    } else {
      return 1;
    }
    break;

  case CAS::SysZmf_shape_ShMGPy8_SR_mCC:
    if (type == CAS::Zcl || type == CAS::Zbc || type == CAS::Zbl) {
      if (VpT < 150e3) {
        scale = m_f_SysZmf_75_150ptv_shape_ShMGPy8_SR_mCC->Eval(Mbb);
      } else if (VpT > 150e3) {
        scale = m_f_SysZmf_150ptv_shape_ShMGPy8_SR_mCC->Eval(Mbb);
      }
    } else {
      return 1;
    }
    break;

  case CAS::SysZl_shape_ShMGPy8_SR_mCC:
    if (type != CAS::Zl)
      return 1;
    if (njet == 2 && VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysZl_2jet_150_250ptv_shape_ShMGPy8_SR_mCC->Eval(Mbb);
    } else if (njet == 2 && VpT > 250e3) {
      scale = m_f_SysZl_2jet_250ptv_shape_ShMGPy8_SR_mCC->Eval(Mbb);
    } else if (njet == 2 && VpT < 150e3) {
      scale = m_f_SysZl_2jet_75_150ptv_shape_ShMGPy8_SR_mCC->Eval(Mbb);
    } else if (njet > 2 && VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysZl_3jet_150_250ptv_shape_ShMGPy8_SR_mCC->Eval(Mbb);
    } else if (njet > 2 && VpT > 250e3) {
      scale = m_f_SysZl_3jet_250ptv_shape_ShMGPy8_SR_mCC->Eval(Mbb);
    } else if (njet > 2 && VpT < 150e3) {
      scale = m_f_SysZl_3jet_75_150ptv_shape_ShMGPy8_SR_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysWhf_shape_ShMGPy8_SR_mCC:
    if (type == CAS::Wcc || type == CAS::Wbb) {
      scale = m_f_SysWhf_shape_ShMGPy8_SR_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysWmf_shape_ShMGPy8_SR_mCC:
    if (type == CAS::Wcl || type == CAS::Wbc || type == CAS::Wbl) {
      scale = m_f_SysWmf_shape_ShMGPy8_SR_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysWl_shape_ShMGPy8_SR_mCC:
    if (type != CAS::Wl)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysWl_150_250ptv_shape_ShMGPy8_SR_mCC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysWl_250ptv_shape_ShMGPy8_SR_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  // VHcc ttbarbq
  case CAS::SysTTbarbq_shape_mCC_Her_Pyt:
    if (type != CAS::ttbarbq && type != CAS::ttbarqq)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysTTbarbq_150_250ptv_shape_mCC_Her_Pyt->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysTTbarbq_250ptv_shape_mCC_Her_Pyt->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysTTbarbq_shape_mCC_Pow_aMC:
    if (type != CAS::ttbarbq && type != CAS::ttbarqq)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysTTbarbq_150_250ptv_shape_mCC_Pow_aMC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysTTbarbq_250ptv_shape_mCC_Pow_aMC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysTTbarbq_shape_mCC_rHi:
    if (type != CAS::ttbarbq && type != CAS::ttbarqq)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysTTbarbq_150_250ptv_shape_mCC_rHi->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysTTbarbq_250ptv_shape_mCC_rHi->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  // VHcc ttbarlq
  case CAS::SysTTbarlq_shape_mCC_Her_Pyt:
    if (type != CAS::ttbarlq)
      return 1;
    if (VpT > 150e3) {
      scale = m_f_SysTTbarlq_shape_mCC_Her_Pyt->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysTTbarlq_shape_mCC_Pow_aMC:
    if (type != CAS::ttbarlq)
      return 1;
    if (VpT > 150e3) {
      scale = m_f_SysTTbarlq_shape_mCC_Pow_aMC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysTTbarlq_shape_mCC_rHi:
    if (type != CAS::ttbarlq)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysTTbarlq_150_250ptv_shape_mCC_rHi->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysTTbarlq_250ptv_shape_mCC_rHi->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  // VHcc stopWtbq
  case CAS::SysStopWtbq_shape_mCC_Her_Pyt:
    if (type != CAS::stop_Wtbq)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysStopWtbq_150_250ptv_shape_mCC_Her_Pyt->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysStopWtbq_250ptv_shape_mCC_Her_Pyt->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysStopWtbq_shape_mCC_Pow_aMC:
    if (type != CAS::stop_Wtbq)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysStopWtbq_150_250ptv_shape_mCC_Pow_aMC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysStopWtbq_250ptv_shape_mCC_Pow_aMC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

    // VHcc stopWtlq

  case CAS::SysStopWtlq_shape_mCC_Her_Pyt:
    if (type != CAS::stop_Wtlq)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysStopWtlq_150_250ptv_shape_mCC_Her_Pyt->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysStopWtlq_250ptv_shape_mCC_Her_Pyt->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysStopWtlq_shape_mCC_Pow_aMC:
    if (type != CAS::stop_Wtlq)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysStopWtlq_150_250ptv_shape_mCC_Pow_aMC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysStopWtlq_250ptv_shape_mCC_Pow_aMC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

    // VHcc Diboson mironova @ 06/05/2020

  case CAS::SysZZcc_shape_PwPy8_mCC:
    if (type != CAS::ZZcc)
      return 1;
    if (VpT > 75e3 && VpT <= 150e3) {
      scale = m_f_SysZZcc_75_150ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysZZcc_150_250ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysZZcc_250ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysZZbkg_shape_PwPy8_mCC:
    if (type != CAS::ZZbkg)
      return 1;
    if (VpT > 75e3 && VpT <= 150e3) {
      scale = m_f_SysZZbkg_75_150ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysZZbkg_150_250ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysZZbkg_250ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysWZhadlepcl_shape_PwPy8_mCC:
    if (type != CAS::WZhadlepcl)
      return 1;
    if (m_two) {
      if (VpT > 75e3 && VpT <= 150e3) {
        scale = m_f_SysWZhadlepcl_75_150ptv_shape_PwPy8_mCC_2lep->Eval(Mbb);
      } else if (VpT > 150e3 && VpT <= 250e3) {
        scale = m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_2lep->Eval(Mbb);
      } else if (VpT > 250e3) {
        scale = m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_2lep->Eval(Mbb);
      } else {
        return 1;
      }
    } else {
      if (VpT > 150e3 && VpT <= 250e3) {
        scale = m_f_SysWZhadlepcl_150_250ptv_shape_PwPy8_mCC_0lep->Eval(Mbb);
      } else if (VpT > 250e3) {
        scale = m_f_SysWZhadlepcl_250ptv_shape_PwPy8_mCC_0lep->Eval(Mbb);
      } else {
        return 1;
      }
    }
    break;

  case CAS::SysWZhadlepbkg_shape_PwPy8_mCC:
    if (type != CAS::WZhadlepbkg)
      return 1;
    if (m_two) {
      if (VpT > 75e3 && VpT <= 150e3) {
        scale = m_f_SysWZhadlepbkg_75_150ptv_shape_PwPy8_mCC_2lep->Eval(Mbb);
      } else if (VpT > 150e3 && VpT <= 250e3) {
        scale = m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_2lep->Eval(Mbb);
      } else if (VpT > 250e3) {
        scale = m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_2lep->Eval(Mbb);
      } else {
        return 1;
      }
    } else {
      if (VpT > 150e3 && VpT <= 250e3) {
        scale = m_f_SysWZhadlepbkg_150_250ptv_shape_PwPy8_mCC_0lep->Eval(Mbb);
      } else if (VpT > 250e3) {
        scale = m_f_SysWZhadlepbkg_250ptv_shape_PwPy8_mCC_0lep->Eval(Mbb);
      } else {
        return 1;
      }
    }
    break;

  case CAS::SysWZlephadcc_shape_PwPy8_mCC:
    if (type != CAS::WZlephadcc)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysWZlephadcc_150_250ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysWZlephadcc_250ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysWZlephadbkg_shape_PwPy8_mCC:
    if (type != CAS::WZlephadbkg)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysWZlephadbkg_150_250ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysWZlephadbkg_250ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysWWcl_shape_PwPy8_mCC:
    if (type != CAS::WWcl)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysWWcl_150_250ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysWWcl_250ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysWWbkg_shape_PwPy8_mCC:
    if (type != CAS::WWbkg)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysWWbkg_150_250ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysWWbkg_250ptv_shape_PwPy8_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

    // VHcc signal mironova @ 13/05/2020
  case CAS::SysqqZHcc_shape_PwHwg7_mCC:
    if ((type != CAS::qqZHllcc) && (type != CAS::qqZHvvcc))
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysqqZHcc_150_250ptv_shape_PwHwg7_mCC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysqqZHcc_250ptv_shape_PwHwg7_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysggZHcc_shape_PwHwg7_mCC:
    if ((type != CAS::ggZHllcc) && (type != CAS::ggZHvvcc))
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysggZHcc_150_250ptv_shape_PwHwg7_mCC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysggZHcc_250ptv_shape_PwHwg7_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  case CAS::SysqqWHcc_shape_PwHwg7_mCC:
    if (type != CAS::WHlvcc)
      return 1;
    if (VpT > 150e3 && VpT <= 250e3) {
      scale = m_f_SysqqWHcc_150_250ptv_shape_PwHwg7_mCC->Eval(Mbb);
    } else if (VpT > 250e3) {
      scale = m_f_SysqqWHcc_250ptv_shape_PwHwg7_mCC->Eval(Mbb);
    } else {
      return 1;
    }
    break;

  //==================================VHcc===========================================================
  default:
    return 1;
    break;
  }

  // generic case: symmetric uncertainties
  scale *= sgn;

  if (print)
    std::cout << "<CorrsAndSysts::Get_SystematicWeight()>     ===>>> Scale = "
              << scale << std::endl;
  return 1 + scale;

} // Get_SystematicWeight

// if using this function
//  need to have Up or Do as the last two characters of the syst name
void CorrsAndSysts::GetSystFromName(TString name, CAS::Systematic &sys,
                                    CAS::SysVar &var) {
  if (name.EndsWith("Up") || name.EndsWith("Do")) {
    var = m_varFromNames[name(name.Length() - 2, 2).Data()];
    name.Remove(name.Length() - 2);
  }
  sys = m_systFromNames[name.Data()];
}

// faster method
void CorrsAndSysts::GetSystFromName(const TString &name, CAS::Systematic &sys) {
  sys = m_systFromNames[name.Data()];
}

void CorrsAndSysts::WriteHistsToFile(TString fname) {
  TFile *file = new TFile(fname, "RECREATE");
  for (auto hists = m_allHists.begin(); hists != m_allHists.end(); hists++) {
    hists->second->Write();
  }
  file->Close();
  delete file;
} // WriteHistsToFile

/*
 *
 *  UTILITY FUNCTIONS
 *
 */

namespace Utils {

std::shared_ptr<TH1F>
BuildTH1F(std::vector<Double_t> contents, TString hname, float min, float max,
          std::map<TString, std::shared_ptr<TObject>> &hists) {
  std::shared_ptr<TH1F> tmp =
      std::make_shared<TH1F>(hname, hname, contents.size(), min, max);
  for (unsigned int i = 1; i < contents.size() + 1; i++) {
    tmp->SetBinContent(i, contents[i - 1]);
  }
  if (tmp->GetBinContent(tmp->GetNbinsX() + 1) == 0) {
    tmp->SetBinContent(tmp->GetNbinsX() + 1,
                       tmp->GetBinContent(tmp->GetNbinsX()));
  }
  SaveHist(tmp, hists);
  return tmp;
}

void FillTH1(std::vector<Float_t> contents, std::shared_ptr<TH1> h,
             std::map<TString, std::shared_ptr<TObject>> &hists) {
  if (contents.size() != static_cast<unsigned int>(h->GetNbinsX())) {
    std::cout << "CorrsAndSysts: filling the histogram " << h->GetName()
              << " with a wrong number of bins" << std::endl;
    exit(-1);
  }
  for (int i = 0; i < h->GetNbinsX(); i++) {
    h->SetBinContent(i + 1, contents[i]);
  }
  if (h->GetBinContent(h->GetNbinsX() + 1) == 0) {
    h->SetBinContent(h->GetNbinsX() + 1, h->GetBinContent(h->GetNbinsX()));
  }
  SaveHist(h, hists);
}

void FillTH1(int len, Float_t *contents, std::shared_ptr<TH1> h,
             std::map<TString, std::shared_ptr<TObject>> &hists) {
  if (len != h->GetNbinsX()) {
    std::cout << "CorrsAndSysts: filling the histogram " << h->GetName()
              << " with a wrong number of bins" << std::endl;
    exit(-1);
  }
  for (int i = 0; i < h->GetNbinsX(); i++) {
    h->SetBinContent(i + 1, contents[i]);
  }
  if (h->GetBinContent(h->GetNbinsX() + 1) == 0) {
    h->SetBinContent(h->GetNbinsX() + 1, h->GetBinContent(h->GetNbinsX()));
  }
  SaveHist(h, hists);
}

inline float GetScale(float value, std::shared_ptr<TH1> h) {
  return h->GetBinContent(h->FindBin(value));
}

void ArraySubstractOne(float *array, unsigned int length) {
  for (unsigned int i = 0; i < length; i++) {
    array[i] -= 1;
  }
}

} // namespace Utils
