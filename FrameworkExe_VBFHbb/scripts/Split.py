# author: Paolo Francavilla
# paolo.francavilla@cern.ch
# splitter for 0-lepton VH analysis
###############################
from ROOT import *
from os import *
from math import *

Analyser="Camilla"

# CHECK THIS BEFORE RUNNING !
usePUNorm=False # default: False (True used on mc15a where no PUw is applied)
useMadGraph=True
useTTbarSys=True

Luminosity=1.0
Luminosity2=1.0

samples={}
variables={}
regions={}
Rebin={}

CorrAndSys_Path="../../CorrsAndSysts/"

if Analyser=="Camilla":

    input_folder = "./inputs/"

    # SIGNAL HVT
    #########################################
    for mass in [ "5000", "600", "700", "800", "900", "1000", "1100", "1200", "1300", "1400","1500", "1700", "1800", "1900", "2200","2400","2600","2800","3000", "500", "1600", "4000", "3500","4500","2000"]:
        if float(mass) < 1000:
           samples["HVTZHvvqq"+mass+"_vvJ"]=["HVTZHvvqq"+mass,input_folder+"hist-HVT_0"+mass+".root"]
        else:
           samples["HVTZHvvqq"+mass+"_vvJ"]=["HVTZHvvqq"+mass,input_folder+"hist-HVT_"+mass+".root"]
 
    for mass in [ "5000", "600", "700", "800", "900", "1000", "1100", "1200", "1300", "1400","1500", "1700", "1800", "1900", "2200","2400","2600","2800","3000", "500", "1600", "4000", "3500","4500","2000"]:
        samples["HVTWHlvqq"+mass+"_vvJ"]=["HVTWHlvqq"+mass,input_folder+"hist-HVT_WH.root"]

    # OLD NAMING
     #   samples["HVTWHlvqq"+mass+"_vvJ"]=["HVTWHlvqq"+mass,"./hist-HVT_WH.root"]
        #if (int(mass)<1000):
            #samples["HVTZHvvqq"+mass+"_vvJ"]=["HVTZHvvqq"+mass,input_folder+"hist-HVT_0"+mass+".root"]
        #else:
            #samples["HVTZHvvqq"+mass+"_vvJ"]=["HVTZHvvqq"+mass,input_folder+"hist-HVT_"+mass+".root"]

    # SIGNAL A->Zh
    #########################################
    # OLD NAMING
    #for mass in ["1000", "1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800",  "1900",  "200", "2000", "280","300", "320","340", "360", "380", "400", "420", "440", "460", "480", "500", "550", "600", "650", "700", "750", "800", "850", "900", "950"]:
       # samples["ggAZHvvqq"+mass+"_vvJ"]=["ggA"+mass+"ZHvvbb",input_folder+"hist-ggA_"+mass+".root"]
       # samples["ggAZHllqq"+mass+"_vvJ"]=["ggA"+mass+"ZHllbb",input_folder+"hist-ggA_"+mass+".root"]

    # V+jets
    #########################################
    samples["Wb_vvJ"]=["Wb",input_folder+"hist-W.root"]
    samples["Wc_vvJ"]=["Wc",input_folder+"hist-W.root"]
    samples["Wl_vvJ"]=["Wl",input_folder+"hist-W.root"]
    samples["Zb_vvJ"]=["Zb",input_folder+"hist-Z.root"]
    samples["Zc_vvJ"]=["Zc",input_folder+"hist-Z.root"]
    samples["Zl_vvJ"]=["Zl",input_folder+"hist-Z.root"]
 
    # Dibosons
    #########################################
    samples["WW_vvJ"]=["WW",input_folder+"hist-WW.root"]
    samples["ZZ_vvJ"]=["ZZ",input_folder+"hist-ZZ.root"]
    samples["WZ_vvJ"]=["WZ",input_folder+"hist-WZ.root"]
 
    # Top
    #########################################
    samples["stop_Wt_vvJ"]=["stopWt",input_folder+"hist-singletop_Wt.root"]
    samples["stop_t_vvJ"]=["stopt",input_folder+"hist-singletop_t.root"]
    samples["stop_s_vvJ"]=["stops",input_folder+"hist-singletop_s.root"]
    samples["TTbar_vvJ"]=["ttbar",input_folder+"hist-ttbar.root"]
 
    # SM Higgs
    #########################################
    # qq
    samples["qqZvvH125_vvJ"]=["qqZvvH125",input_folder+"hist-ZHvv125.root"]
    samples["qqZllH125_vvJ"]=["qqZllH125",input_folder+"hist-ZHll125.root"]
    samples["qqWlvH125_vvJ"]=["qqWlvH125",input_folder+"hist-WH125.root"]
    # gg
    samples["ggZllH125_vvJ"]=["ggZllH125",input_folder+"hist-ZHll125.root"]
    samples["ggZvvH125_vvJ"]=["ggZvvH125",input_folder+"hist-ZHvv125.root"]
 
    # Data
    #########################################
    samples["data_vvJ"]=["data",input_folder+"hist-data.root"]


    # REGIONS
    #########################################
    regions["2tag_0addtag_SR"]="2tag0pjet_150ptv_0btrkjetunmatched_SR"
    regions["2tag_0addtag_lowMH"]="2tag0pjet_150ptv_0btrkjetunmatched_mbbCRlow"
    regions["2tag_0addtag_highMH"]="2tag0pjet_150ptv_0btrkjetunmatched_mbbCRhigh"

    regions["2tag_1paddtag_SR"]="2tag0pjet_150ptv_1pbtrkjetunmatched_SR"
    regions["2tag_1paddtag_lowMH"]="2tag0pjet_150ptv_1pbtrkjetunmatched_mbbCRlow"
    regions["2tag_1paddtag_highMH"]="2tag0pjet_150ptv_1pbtrkjetunmatched_mbbCRhigh"

    regions["1tag_0addtag_SR"]="1tag0pjet_150ptv_0btrkjetunmatched_SR"
    regions["1tag_0addtag_lowMH"]="1tag0pjet_150ptv_0btrkjetunmatched_mbbCRlow"
    regions["1tag_0addtag_highMH"]="1tag0pjet_150ptv_0btrkjetunmatched_mbbCRhigh"

    regions["1tag_1paddtag_SR"]="1tag0pjet_150ptv_1pbtrkjetunmatched_SR"
    regions["1tag_1paddtag_lowMH"]="1tag0pjet_150ptv_1pbtrkjetunmatched_mbbCRlow"
    regions["1tag_1paddtag_highMH"]="1tag0pjet_150ptv_1pbtrkjetunmatched_mbbCRhigh"

    # Not using 0tag by default
    #regions["0tag_0addtag_SR"]="0tag0pjet_150ptv_0btrkjetunmatched_SR"
    #regions["0tag_0addtag_lowMH"]="0tag0pjet_150ptv_0btrkjetunmatched_mbbCRlow"
    #regions["0tag_0addtag_highMH"]="0tag0pjet_150ptv_0btrkjetunmatched_mbbCRhigh"

    #regions["0tag_1paddtag_SR"]="0tag0pjet_150ptv_1pbtrkjetunmatched_SR"
    #regions["0tag_1paddtag_lowMH"]="0tag0pjet_150ptv_1pbtrkjetunmatched_mbbCRlow"
    #regions["0tag_1paddtag_highMH"]="0tag0pjet_150ptv_1pbtrkjetunmatched_mbbCRhigh"

    # DISTRIBUTIONS 
    #########################################
    variables["mVH"]="mVH"
    Rebin["mVH"]=5

    variables["MET"]="MET"
    Rebin["MET"]=2
    
    variables["pTJet"]="pTLeadFatJet"
    Rebin["pTJet"]=2
    
    variables["mJet"]="MLeadFatJet"
    Rebin["mJet"]=2
    
    variables["nBJet"]="NBTagMatchedTrackJetLeadFatJet"
    Rebin["nBJet"]=1

    #########################################


for region in regions.keys():
    for sample in samples.keys():
        for variable in variables.keys():
            print " "
            print "------------"
            print sample, region, variable
            f=TFile(samples[sample][1])
            if (f.Get(samples[sample][0]+"_"+regions[region]+"_"+variables[variable])):
                g=TFile(sample+"_"+region+"_"+variable+".root","recreate")

                PUnorm0tag=1
                PUnorm1tag=1
                PUnorm=1
                if (usePUNorm and sample!="data_vvJ"):
                    noPUwnorm_1ptag=0
                    PUwnorm_1ptag=0
                    if (f.Get(samples[sample][0]+"_1tag0pjet_150ptv_mbbCRhigh_PUw")):
                        noPUwnorm_1ptag      +=f.Get(samples[sample][0]+"_1tag0pjet_150ptv_mbbCRhigh_PUw").GetBinContent(1)
                        PUwnorm_1ptag        +=f.Get(samples[sample][0]+"_1tag0pjet_150ptv_mbbCRhigh_PUw").GetBinContent(2)
                    if (f.Get(samples[sample][0]+"_1tag0pjet_150ptv_mbbCRlow_PUw")):
                        noPUwnorm_1ptag      +=f.Get(samples[sample][0]+"_1tag0pjet_150ptv_mbbCRlow_PUw").GetBinContent(1)
                        PUwnorm_1ptag        +=f.Get(samples[sample][0]+"_1tag0pjet_150ptv_mbbCRlow_PUw").GetBinContent(2)
                    if (f.Get(samples[sample][0]+"_1tag0pjet_150ptv_SR_PUw")):
                        noPUwnorm_1ptag      +=f.Get(samples[sample][0]+"_1tag0pjet_150ptv_SR_PUw").GetBinContent(1)
                        PUwnorm_1ptag        +=f.Get(samples[sample][0]+"_1tag0pjet_150ptv_SR_PUw").GetBinContent(2)
                    
                    if (f.Get(samples[sample][0]+"_2tag0pjet_150ptv_mbbCRhigh_PUw")):
                        noPUwnorm_1ptag      +=f.Get(samples[sample][0]+"_2tag0pjet_150ptv_mbbCRhigh_PUw").GetBinContent(1)
                        PUwnorm_1ptag        +=f.Get(samples[sample][0]+"_2tag0pjet_150ptv_mbbCRhigh_PUw").GetBinContent(2)
                    if (f.Get(samples[sample][0]+"_2tag0pjet_150ptv_mbbCRlow_PUw")):
                        noPUwnorm_1ptag      +=f.Get(samples[sample][0]+"_2tag0pjet_150ptv_mbbCRlow_PUw").GetBinContent(1)
                        PUwnorm_1ptag        +=f.Get(samples[sample][0]+"_2tag0pjet_150ptv_mbbCRlow_PUw").GetBinContent(2)
                    if (f.Get(samples[sample][0]+"_2tag0pjet_150ptv_SR_PUw")):
                        noPUwnorm_1ptag      +=f.Get(samples[sample][0]+"_2tag0pjet_150ptv_SR_PUw").GetBinContent(1)
                        PUwnorm_1ptag        +=f.Get(samples[sample][0]+"_2tag0pjet_150ptv_SR_PUw").GetBinContent(2)


                    noPUwnorm_presel=0
                    PUwnorm_presel=0
                    if (f.Get(samples[sample][0]+"_0ptag0pjet_150ptv_mbbPresel_PUw")):
                        noPUwnorm_presel      =f.Get(samples[sample][0]+"_0ptag0pjet_150ptv_mbbPresel_PUw").GetBinContent(1)
                        PUwnorm_presel        =f.Get(samples[sample][0]+"_0ptag0pjet_150ptv_mbbPresel_PUw").GetBinContent(2)
                    
                    if  noPUwnorm_presel > 0 :
                        PUnorm0tag=PUwnorm_presel/noPUwnorm_presel
                    if  noPUwnorm_1ptag >0:
                        PUnorm1tag=PUwnorm_1ptag /noPUwnorm_1ptag
                            
                    print "\tPU normalization: presel", PUnorm0tag
                    print "\tPU normalization: 1p tag", PUnorm1tag
                    if "0tag" in regions[region]:
                        PUnorm=PUnorm0tag
                    else:
                        PUnorm=PUnorm1tag
                
                h=f.Get(samples[sample][0]+"_"+regions[region]+"_"+variables[variable])
                print "\t",h.GetName()
                h2=h.Clone()
                h2.SetName("nominal")
                h2.SetTitle(sample)
                h2.Rebin(Rebin[variable])
                if sample!="data_vvJ":
                    print "\tPU normalization Nominal=",PUnorm
                    h2.Scale(PUnorm)
                    print "\tLuminosity Scale Nominal=",Luminosity/Luminosity2
                    h2.Scale(Luminosity/Luminosity2)
                g.cd()
		print "\tNominal Integral= ",h2.Integral()
                h2.Write()
                
                
                if sample!="null":
                    for k in f.GetListOfKeys():
                        if "Sys" in k.GetName():
                            Syst=k.GetName()
                            #print Syst
                            Dir=k.ReadObj()
                            if (Dir.Get(samples[sample][0]+"_"+regions[region]+"_"+variables[variable]+"_"+Syst)):
                                hsys=Dir.Get(samples[sample][0]+"_"+regions[region]+"_"+variables[variable]+"_"+Syst)
                                h2sys=hsys.Clone()
                                h2sys.SetName(Syst.split("Sys")[1])
                                h2sys.SetTitle(sample)
                                h2sys.Rebin(Rebin[variable])
                                print "\tPU normalization "+Syst.split("Sys")[1]+"=",PUnorm
                                h2sys.Scale(PUnorm)
                                print "\tLuminosity Scale "+Syst.split("Sys")[1]+"=",Luminosity/Luminosity2
                                h2sys.Scale(Luminosity/Luminosity2)
                                g.cd()
                                h2sys.Write()
                            else:
                                print Syst.split("Sys")[1]," Missing: ",sample,region,variable
                
                if useTTbarSys:
                    if sample in ["TTbar_vvJ"]:
                        SystTTbar_0lep=TFile(CorrAndSys_Path+"/data/SystTTBar_shape_mVH/fit_VH_res_0lep.root")
                        if  ((variable=="mVH") and (("2tag_1paddtag" in region) or
                                                    ("1tag_1paddtag" in region) or
                                                    ("2tag_0addtag"  in region) or
                                                    ("1tag_0addtag"  in region))):

                            if "2tag_1paddtag" in region:
                                #fHerwig=TF1("MODEL_TTbar_Herwig__1up_2tag_CR", "0.162939 + (-1.09245 * log(x/1000.))", 0, 99999)
                                #fRad=TF1("MODEL_TTbar_rad__1up_2tag_CR", "0.936489 + (-0.101511 * log(x/1000.))", 0, 99999)
                                #faMcAtNlo=TF1("MODEL_TTbar_aMcAtNlo__1up_2tag_CR", "0.385881 + (-0.748813 * log(x/1000.))", 0, 99999)
                                faMcAtNlo = SystTTbar_0lep.Get("aMCAtNLO+Herwig++_Powheg+Herwig++_pretag_1paddtag_SR")
                                fHerwig   = SystTTbar_0lep.Get("Powheg+Pythia6_Powheg+Herwig++_pretag_1paddtag_SR")
                                frad      = SystTTbar_0lep.Get("RadHi_RadLo_pretag_1paddtag_SR")

    
                            if "1tag_1paddtag" in region:
                                #fHerwig=TF1("MODEL_TTbar_Herwig__1up_1tag_CR", "0.818941 + (-0.248451 * log(x/1000.))", 0, 99999)
                                #fRad=TF1("MODEL_TTbar_rad__1up_1tag_CR", "0.920924 + (-0.0860397 * log(x/1000.))", 0, 99999)
                                #faMcAtNlo=TF1("MODEL_TTbar_aMcAtNlo__1up_1tag_CR", "0.906739 + (-0.0991904 * log(x/1000.))", 0, 99999)
                                faMcAtNlo = SystTTbar_0lep.Get("aMCAtNLO+Herwig++_Powheg+Herwig++_pretag_1paddtag_SR")
                                fHerwig   = SystTTbar_0lep.Get("Powheg+Pythia6_Powheg+Herwig++_pretag_1paddtag_SR")
                                frad      = SystTTbar_0lep.Get("RadHi_RadLo_pretag_1paddtag_SR")
    
                         
                            if "2tag_0addtag" in region:
                                #fHerwig=TF1("MODEL_TTbar_Herwig__1up_2tag_SR", "1.04557 + (0.135654 * log(x/1000.))", 0, 99999)
                                #fRad=TF1("MODEL_TTbar_rad__1up_2tag_SR", "0.68769 + (-0.711336 * log(x/1000.))", 0, 99999)
                                #faMcAtNlo=TF1("MODEL_TTbar_aMcAtNlo__1up_2tag_SR", "1.6411 + (1.11052 * log(x/1000.))", 0, 99999)
                                faMcAtNlo = SystTTbar_0lep.Get("aMCAtNLO+Herwig++_Powheg+Herwig++_pretag_0addtag_SR")
                                fHerwig   = SystTTbar_0lep.Get("Powheg+Pythia6_Powheg+Herwig++_pretag_0addtag_SR")
                                frad      = SystTTbar_0lep.Get("RadHi_RadLo_pretag_0addtag_SR")
   
                            if "1tag_0addtag" in region:
                                #fHerwig=TF1("MODEL_TTbar_Herwig__1up_1tag_SR", "0.884461 + (-0.177822 * log(x/1000.))", 0, 99999)
                                #fRad=TF1("MODEL_TTbar_rad__1up_1tag_SR", "0.828781 + (-0.120811 * log(x/1000.))", 0, 99999)
                                #faMcAtNlo=TF1("MODEL_TTbar_aMcAtNlo__1up_1tag_SR", "1.23627 + (0.363092 * log(x/1000.))", 0, 99999)
                                faMcAtNlo = SystTTbar_0lep.Get("aMCAtNLO+Herwig++_Powheg+Herwig++_pretag_0addtag_SR")
                                fHerwig   = SystTTbar_0lep.Get("Powheg+Pythia6_Powheg+Herwig++_pretag_0addtag_SR")
                                frad      = SystTTbar_0lep.Get("RadHi_RadLo_pretag_0addtag_SR")

     
 
                            hNom=f.Get("ttbar_"+regions[region]+"_"+variables[variable])
                        
                            h2parMODEL_TTbar_Herwig=hNom.Clone()
                            h2parMODEL_TTbar_Herwig.SetName("MODEL_TTbar_Herwig__1up")
                            h2parMODEL_TTbar_Herwig.SetTitle(sample)
                            h2parMODEL_TTbar_Herwig.Rebin(Rebin[variable])
                            print "\tPU normalization SMODEL_TTbar_Herwig__1up=",PUnorm
                            h2parMODEL_TTbar_Herwig.Scale(PUnorm)
                            print "\tLuminosity Scale MODEL_TTbar_Herwig__1up=",Luminosity/Luminosity2
                            h2parMODEL_TTbar_Herwig.Scale(Luminosity/Luminosity2)
                            h2parMODEL_TTbar_Herwig.Multiply(fHerwig)
                            g.cd()
                            h2parMODEL_TTbar_Herwig.Write()
                        
                            h2parMODEL_TTbar_aMcAtNlo=hNom.Clone()
                            h2parMODEL_TTbar_aMcAtNlo.SetName("MODEL_TTbar_aMcAtNlo__1up")
                            h2parMODEL_TTbar_aMcAtNlo.SetTitle(sample)
                            h2parMODEL_TTbar_aMcAtNlo.Rebin(Rebin[variable])
                            print "\tPU normalization MODEL_TTbar_aMcAtNlo__1up=",PUnorm
                            h2parMODEL_TTbar_aMcAtNlo.Scale(PUnorm)
                            print "\tLuminosity Scale MODEL_TTbar_aMcAtNlo__1up=",Luminosity/Luminosity2
                            h2parMODEL_TTbar_aMcAtNlo.Scale(Luminosity/Luminosity2)
                            h2parMODEL_TTbar_aMcAtNlo.Multiply(faMcAtNlo)
                            g.cd()
                            h2parMODEL_TTbar_aMcAtNlo.Write()
                        
                            h2parMODEL_TTbar_rad__1up=hNom.Clone()
                            h2parMODEL_TTbar_rad__1up.SetName("MODEL_TTbar_rad__1up")
                            h2parMODEL_TTbar_rad__1up.SetTitle(sample)
                            h2parMODEL_TTbar_rad__1up.Rebin(Rebin[variable])
                            print "\tPU normalization MODEL_TTbar_rad__1up=",PUnorm
                            h2parMODEL_TTbar_rad__1up.Scale(PUnorm)
                            print "\tLuminosity Scale MODEL_TTbar_rad__1up=",Luminosity/Luminosity2
                            h2parMODEL_TTbar_rad__1up.Scale(Luminosity/Luminosity2)
                            for ibin in range(1, h2parMODEL_TTbar_rad__1up.GetNbinsX()+1):
                                w=frad.Eval(h2parMODEL_TTbar_rad__1up.GetBinCenter(ibin))
                                if w<0.1:
                                    w=0.1
                                w=1+(w-1)/(w+1)
                                h2parMODEL_TTbar_rad__1up.SetBinContent(ibin,h2parMODEL_TTbar_rad__1up.GetBinContent(ibin)*w)
                                h2parMODEL_TTbar_rad__1up.SetBinError(ibin,h2parMODEL_TTbar_rad__1up.GetBinError(ibin)*w)
                            #h2parMODEL_TTbar_rad__1up.Multiply(fRad)
                            g.cd()
                            h2parMODEL_TTbar_rad__1up.Write()
                    
                            h2parMODEL_TTbar_rad__1do=hNom.Clone()
                            h2parMODEL_TTbar_rad__1do.SetName("MODEL_TTbar_rad__1down")
                            h2parMODEL_TTbar_rad__1do.SetTitle(sample)
                            h2parMODEL_TTbar_rad__1do.Rebin(Rebin[variable])
                            print "\tPU normalization MODEL_TTbar_rad__1down=",PUnorm
                            h2parMODEL_TTbar_rad__1do.Scale(PUnorm)
                            print "\tLuminosity Scale MODEL_TTbar_rad__1down=",Luminosity/Luminosity2
                            h2parMODEL_TTbar_rad__1do.Scale(Luminosity/Luminosity2)
                            for ibin in range(1, h2parMODEL_TTbar_rad__1up.GetNbinsX()+1):
                                w=frad.Eval(h2parMODEL_TTbar_rad__1do.GetBinCenter(ibin))
                                if w<0.1:
                                    w=0.1
                                w=1-(w-1)/(w+1)
                                h2parMODEL_TTbar_rad__1do.SetBinContent(ibin,h2parMODEL_TTbar_rad__1do.GetBinContent(ibin)*w)
                                h2parMODEL_TTbar_rad__1do.SetBinError(ibin,h2parMODEL_TTbar_rad__1do.GetBinError(ibin)*w)
                            #h2parMODEL_TTbar_rad__1do.Multiply(fRadDo)
                            g.cd()
                            h2parMODEL_TTbar_rad__1do.Write()

                        normHerwig=1
                        normMcAtNloHW=1
                        normradhi=1
                        normradlo=1

                        PreNom=        f.Get(samples[sample][0]+"_0ptag0pjet_150ptv_mbbPresel_PUw").GetBinContent(1)
                        PreHerwig=0
                        if f.Get("ttbarPowHW_UEEE5_0ptag0pjet_150ptv_mbbPresel_PUw"):
                            PreHerwig=        f.Get("ttbarPowHW_UEEE5_0ptag0pjet_150ptv_mbbPresel_PUw").GetBinContent(1)
                        PreaMcAtNloHW=0
                        if f.Get("ttbaraMcAtNloHW_0ptag0pjet_150ptv_mbbPresel_PUw"):
                            PreaMcAtNloHW=     f.Get("ttbaraMcAtNloHW_0ptag0pjet_150ptv_mbbPresel_PUw").GetBinContent(1)
                        Preradhi=0
                        if f.Get("ttbar_radhi_0ptag0pjet_150ptv_mbbPresel_PUw"):
                            Preradhi=              f.Get("ttbar_radhi_0ptag0pjet_150ptv_mbbPresel_PUw").GetBinContent(1)
                        Preraddo=0
                        if f.Get("ttbar_radlo_0ptag0pjet_150ptv_mbbPresel_PUw"):
                            Preraddo=              f.Get("ttbar_radlo_0ptag0pjet_150ptv_mbbPresel_PUw").GetBinContent(1)

                        if PreHerwig>0:
                            normHerwig=PreNom/PreHerwig
                        if PreaMcAtNloHW>0:
                            normMcAtNloHW=PreNom/PreaMcAtNloHW
                        if Preradhi>0:
                            normradhi=PreNom/Preradhi
                        if Preraddo>0:
                            normradlo=PreNom/Preraddo
                        
                        if (f.Get("ttbarPowHW_UEEE5_"+regions[region]+"_"+variables[variable])):
                            hMODEL_TTbar_Herwig=f.Get("ttbarPowHW_UEEE5_"+regions[region]+"_"+variables[variable])
                            h2MODEL_TTbar_Herwig=hMODEL_TTbar_Herwig.Clone()
                            h2MODEL_TTbar_Herwig.SetName("mcMODEL_TTbar_Herwig__1up")
                            h2MODEL_TTbar_Herwig.SetTitle(sample)
                            h2MODEL_TTbar_Herwig.Rebin(Rebin[variable])
                            print "\tmcMODEL_TTbar_Herwig__1up pre-selection normalization= ",normHerwig
                            print "\tPU normalization mcMODEL_TTbar_Herwig__1up=",PUnorm
                            h2MODEL_TTbar_Herwig.Scale(PUnorm)
                            print "\tLuminosity Scale mcMODEL_TTbar_Herwig__1up=",Luminosity/Luminosity2
                            h2MODEL_TTbar_Herwig.Scale(Luminosity/Luminosity2)
                            g.cd()
                            h2MODEL_TTbar_Herwig.Write()
                        else:
                            print "mcMODEL_TTbar_Herwig__1up Missing: ",sample,region,variable
                        
                        if (f.Get("ttbaraMcAtNloHW_"+regions[region]+"_"+variables[variable])):
                            hMODEL_TTbar_aMcAtNlo=f.Get("ttbaraMcAtNloHW_"+regions[region]+"_"+variables[variable])
                            h2MODEL_TTbar_aMcAtNlo=hMODEL_TTbar_aMcAtNlo.Clone()
                            h2MODEL_TTbar_aMcAtNlo.SetName("mcMODEL_TTbar_aMcAtNlo__1up")
                            h2MODEL_TTbar_aMcAtNlo.SetTitle(sample)
                            h2MODEL_TTbar_aMcAtNlo.Rebin(Rebin[variable])
                            print "\tmcMODEL_TTbar_aMcAtNlo__1up pre-selection normalization= ",normMcAtNloHW
                            print "\tPU normalization mcMODEL_TTbar_aMcAtNlo__1up=",PUnorm
                            h2MODEL_TTbar_aMcAtNlo.Scale(PUnorm)
                            print "\tLuminosity Scale mcMODEL_TTbar_aMcAtNlo__1up=",Luminosity/Luminosity2
                            h2MODEL_TTbar_aMcAtNlo.Scale(Luminosity/Luminosity2)
                            g.cd()
                            h2MODEL_TTbar_aMcAtNlo.Write()
                        else:
                            print "mcMODEL_TTbar_aMcAtNlo__1up Missing: ",sample,region,variable
                        
                        if (f.Get("ttbar_radhi_"+regions[region]+"_"+variables[variable])):
                            hMODEL_TTbar_rad__1up=f.Get("ttbar_radhi_"+regions[region]+"_"+variables[variable])
                            h2MODEL_TTbar_rad__1up=hMODEL_TTbar_rad__1up.Clone()
                            h2MODEL_TTbar_rad__1up.SetName("mcMODEL_TTbar_rad__1up")
                            h2MODEL_TTbar_rad__1up.SetTitle(sample)
                            h2MODEL_TTbar_rad__1up.Rebin(Rebin[variable])
                            print "\tmcMODEL_TTbar_rad__1up pre-selection normalization= ",normradhi
                            print "\tPU normalization mcMODEL_TTbar_rad__1up=",PUnorm
                            h2MODEL_TTbar_rad__1up.Scale(PUnorm)
                            print "\tLuminosity Scale mcMODEL_TTbar_rad__1up=",Luminosity/Luminosity2
                            h2MODEL_TTbar_rad__1up.Scale(Luminosity/Luminosity2)
                            g.cd()
                            h2MODEL_TTbar_rad__1up.Write()
                        else:
                            print "mcMODEL_TTbar_rad__1up Missing: ",sample,region,variable
                        
                        if (f.Get("ttbar_radlo_"+regions[region]+"_"+variables[variable])):
                            hMODEL_TTbar_rad__1do=f.Get("ttbar_radlo_"+regions[region]+"_"+variables[variable])
                            h2MODEL_TTbar_rad__1do=hMODEL_TTbar_rad__1do.Clone()
                            h2MODEL_TTbar_rad__1do.SetName("mcMODEL_TTbar_rad__1down")
                            h2MODEL_TTbar_rad__1do.SetTitle(sample)
                            h2MODEL_TTbar_rad__1do.Rebin(Rebin[variable])
                            print "\tmcMODEL_TTbar_rad__1down pre-selection normalization= ",normradlo
                            print "\tPU normalization mcMODEL_TTbar_rad__1down=",PUnorm
                            h2MODEL_TTbar_rad__1do.Scale(PUnorm)
                            print "\tLuminosity Scale mcMODEL_TTbar_rad__1down=",Luminosity/Luminosity2
                            h2MODEL_TTbar_rad__1do.Scale(Luminosity/Luminosity2)
                            g.cd()
                            h2MODEL_TTbar_rad__1do.Write()
                        else:
                            print "mcMODEL_TTbar_rad__1down Missing: ",sample,region,variable



                if useMadGraph:
                    if sample in ["Wb_vvJ","Wc_vvJ","Wl_vvJ","Zb_vvJ","Zc_vvJ","Zl_vvJ"]:
                        SystVJets_0lep_lowMH_mVH=TFile(CorrAndSys_Path+"data/SystVJets_shape_mVH/SystVJets_0Lep_lowMH_mVH.root")
                        SystVJets_0lep_SR_mVH=TFile(CorrAndSys_Path+"data/SystVJets_shape_mVH/SystVJets_0Lep_SR_mVH.root")
                        SystVJets_0lep_highMH_mVH=TFile(CorrAndSys_Path+"data/SystVJets_shape_mVH/SystVJets_0Lep_highMH_mVH.root")
                        if  ((variable=="mVH") and (("2tag_1paddtag" in region) or
                                                    ("1tag_1paddtag" in region) or
                                                    ("2tag_0addtag"  in region) or
                                                    ("1tag_0addtag"  in region))):
                            #if sample in ["Wb_vvJ", "Zb_vvJ"]:
                            #    if region=="2tag_0addtag_SR" or region=="2tag_1paddtag_SR":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/SR/misc/bottom_vvJ_2tag_0addtag_SR_mVH_1-Order_PolyFit.root");
                            #    if region=="1tag_0addtag_SR" or region=="1tag_1paddtag_SR":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/SR/misc/bottom_vvJ_1tag_0addtag_SR_mVH_1-Order_PolyFit.root");
                            #    if region=="2tag_0addtag_highMH" or region=="2tag_1addtag_highMH":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/highMH/misc/bottom_vvJ_2tag_0addtag_highMH_mVH_1-Order_PolyFit.root");
                            #    if region=="1tag_0addtag_highMH" or region=="1tag_1addtag_highMH":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/highMH/misc/bottom_vvJ_1tag_0addtag_highMH_mVH_1-Order_PolyFit.root");
                            #    if region=="2tag_0addtag_lowMH" or region=="2tag_1addtag_lowMH":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/lowMH/misc/bottom_vvJ_2tag_0addtag_lowMH_mVH_1-Order_PolyFit.root");
                            #    if region=="1tag_0addtag_lowMH" or region=="1tag_1addtag_lowMH":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/lowMH/misc/bottom_vvJ_1tag_0addtag_lowMH_mVH_1-Order_PolyFit.root");
                            #if sample in ["Wc_vvJ", "Zc_vvJ"]:
                            #    if region=="2tag_0addtag_SR" or region=="2tag_1paddtag_SR":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/SR/misc/charm_vvJ_2tag_0addtag_SR_mVH_1-Order_PolyFit.root");
                            #    if region=="1tag_0addtag_SR" or region=="1tag_1paddtag_SR":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/SR/misc/charm_vvJ_1tag_0addtag_SR_mVH_1-Order_PolyFit.root");
                            #    if region=="2tag_0addtag_highMH" or region=="2tag_1addtag_highMH":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/highMH/misc/charm_vvJ_2tag_0addtag_highMH_mVH_1-Order_PolyFit.root");
                            #    if region=="1tag_0addtag_highMH" or region=="1tag_1addtag_highMH":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/highMH/misc/charm_vvJ_1tag_0addtag_highMH_mVH_1-Order_PolyFit.root");
                            #    if region=="2tag_0addtag_lowMH" or region=="2tag_1addtag_lowMH":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/lowMH/misc/charm_vvJ_2tag_0addtag_lowMH_mVH_1-Order_PolyFit.root");
                            #    if region=="1tag_0addtag_lowMH" or region=="1tag_1addtag_lowMH":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/lowMH/misc/charm_vvJ_1tag_0addtag_lowMH_mVH_1-Order_PolyFit.root");
                            #if sample in ["Wl_vvJ", "Zl_vvJ"]:
                            #    if region=="2tag_0addtag_SR" or region=="2tag_1paddtag_SR" or region=="1tag_0addtag_SR" or region=="1tag_1paddtag_SR" :
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/SR/misc/light_vvJ_1tag_0addtag_SR_mVH_1-Order_PolyFit.root");
                            #    if region=="2tag_0addtag_highMH" or region=="2tag_1addtag_highMH" or region=="1tag_0addtag_highMH" or region=="1tag_1addtag_highMH":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/highMH/misc/light_vvJ_1tag_0addtag_highMH_mVH_1-Order_PolyFit.root");
                            #    if region=="2tag_0addtag_lowMH" or region=="2tag_1addtag_lowMH" or region=="1tag_0addtag_lowMH" or region=="1tag_1addtag_lowMH":
                            #        m=TFile("Sherpa_MadGraph_1stOrderPoly_0Lep/lowMH/misc/light_vvJ_1tag_0addtag_lowMH_mVH_1-Order_PolyFit.root");
                            
                            #function=m.Get("MODEL_VJets__fit")
                            if "SR" in region:
                                function=SystVJets_0lep_SR_mVH.Get(sample+"_0ptag_0addtag_SR_mVH_logx_1-Order_PolyFit")
                            if "highMH" in region:
                                function=SystVJets_0lep_highMH_mVH.Get(sample+"_0ptag_0addtag_highMH_mVH_logx_1-Order_PolyFit")
                            if "lowMH" in region:
                                function=SystVJets_0lep_lowMH_mVH.Get(sample+"_0ptag_0addtag_lowMH_mVH_logx_1-Order_PolyFit")
                            
                            hNom=f.Get(samples[sample][0]+"_"+regions[region]+"_"+variables[variable])
                            h2parMODEL_Vjets_MadGraph__1up=hNom.Clone()
                            h2parMODEL_Vjets_MadGraph__1up.SetName("MODEL_Vjets_MadGraph__1up")
                            h2parMODEL_Vjets_MadGraph__1up.SetTitle(sample)
                            h2parMODEL_Vjets_MadGraph__1up.Rebin(Rebin[variable])
                            print "\tPU normalization MODEL_Vjets_MadGraph__1up=",PUnorm
                            h2parMODEL_Vjets_MadGraph__1up.Scale(PUnorm)
                            print "\tLuminosity Scale MODEL_Vjets_MadGraph__1up=",Luminosity/Luminosity2
                            h2parMODEL_Vjets_MadGraph__1up.Scale(Luminosity/Luminosity2)
                            h2parMODEL_Vjets_MadGraph__1up.Multiply(function)
                            g.cd()
                            h2parMODEL_Vjets_MadGraph__1up.Write()

                        if (f.Get("Mad"+samples[sample][0]+"_0ptag0pjet_150ptv_mbbPresel_PUw")):
                            Madnorm=1
                            PreSherpa=        f.Get(samples[sample][0]+"_0ptag0pjet_150ptv_mbbPresel_PUw").GetBinContent(1)
                            PreMadGraph=f.Get("Mad"+samples[sample][0]+"_0ptag0pjet_150ptv_mbbPresel_PUw").GetBinContent(1)
                            if PreMadGraph>0:
                                Madnorm=PreSherpa/(PreMadGraph)
                            if (f.Get("Mad"+samples[sample][0]+"_"+regions[region]+"_"+variables[variable])):
                                hMad=f.Get("Mad"+samples[sample][0]+"_"+regions[region]+"_"+variables[variable])
                                h2Mad=hMad.Clone()
                                h2Mad.SetName("mcMODEL_Vjets_MadGraph__1up")
                                h2Mad.SetTitle(sample)
                                h2Mad.Rebin(Rebin[variable])
                                print "\tmcMadGraph pre-selection normalization= ",Madnorm
                                h2Mad.Scale(Madnorm)
                                print "\tPU normalization mcMODEL_MadGraph__1up=",PUnorm
                                h2Mad.Scale(PUnorm)
                                print "\tLuminosity Scale mcMODEL_MadGraph__1up=",Luminosity/Luminosity2
                                h2Mad.Scale(Luminosity/Luminosity2)
                                g.cd()
                                h2Mad.Write()
                            else:
                                print "mcMODEL_MadGraph__1up Missing: ",sample,region,variable
                        else:
                            print "mcMODEL_MadGraph__1up Missing: ",sample,region,variable
                            
        
                g.Close()
            else:
                print "Region Missing: ",sample,region,variable
            f.Close()


# HADDS OF SMALL SAMPLES
#####################################
dirlist = listdir("./")
stop=[]
VV=[]
VH=[]
for i in dirlist:
    if "stop_Wt" in i:
        stop.append(i)
    if "ZZ" in i:
        VV.append(i)
    if "ggZvvH125" in i:
        VH.append(i)

for file in stop:
    system("hadd -k "
           +file.replace("stop_Wt","STop")+" "
           +file.replace("stop_Wt","stop_s")+" "
           +file.replace("stop_Wt","stop_t")+" "
           +file)
    system("rm "
           +file.replace("stop_Wt","stop_s")+" "
           +file.replace("stop_Wt","stop_t")+" "
           +file)

for file in VV:
    system("hadd -k "
           +file.replace("ZZ","Diboson")+" "
           +file.replace("ZZ","WW")+" "
           +file.replace("ZZ","WZ")+" "
           +file)
    system("rm "
           +file.replace("ZZ","WW")
           +" "+file.replace("ZZ","WZ")
           +" "+file)

for file in VH:
    system("hadd -k "
           +file.replace("ggZvvH125","VH")+" "
           +file.replace("ggZvvH125","ggZllH125")+" "
           +file.replace("ggZvvH125","qqZvvH125")+" "
           +file.replace("ggZvvH125","qqZllH125")+" "
           +file.replace("ggZvvH125","qqWlvH125")+" "
           +file)
    system("rm "
           +file.replace("ggZvvH125","ggZllH125")+" "
           +file.replace("ggZvvH125","qqZvvH125")+" "
           +file.replace("ggZvvH125","qqZllH125")+" "
           +file.replace("ggZvvH125","qqWlvH125")+" "
           +file)

#for file in stop:
#    system("hadd "+file.replace("stop_Wt","asymdata")+" "+file.replace("stop_Wt","*"))
#    system("mv "+file.replace("stop_Wt_vvJ","data")+" "+file.replace("stop_Wt_vvJ","data_vvJ"))

#for region in regions.keys():
#    for variable in variables.keys():
#        system("hadd tmp_asymov_vvJ_"+region+"_"+variable+".root VH_vvJ_"+region+"_"+variable+".root VV_vvJ_"+region+"_"+variable+".root Wl_vvJ_"+region+"_"+variable+".root Wc_vvJ_"+region+"_"+variable+".root Wb_vvJ_"+region+"_"+variable+".root Zl_vvJ_"+region+"_"+variable+".root Zc_vvJ_"+region+"_"+variable+".root Zb_vvJ_"+region+"_"+variable+".root stop_vvJ_"+region+"_"+variable+".root  ttbar_vvJ_"+region+"_"+variable+".root")
#        gasym=TFile("asymov_vvJ_"+region+"_"+variable+".root","recreate")
#        fasym=TFile("tmp_asymov_vvJ_"+region+"_"+variable+".root")
#        h=fasym.Get("nominal")
#        gasym.cd()
#        for i in range(0,h.GetNbinsX()):
#            h.SetBinError(i,sqrt(fabs(h.GetBinContent(i))))
#        h.Write()
#        gasym.Close()
#        fasym.Close()
#        system("rm tmp_asymov_vvJ_"+region+"_"+variable+".root")

