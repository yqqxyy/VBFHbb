################################################################################
# Build configuration for CxAODMaker
################################################################################

# Declare the name of the package:
atlas_subdir( 
   CxAODMaker
   PhysicsAnalysis/JetTagging/FlavorTagDiscriminants )

find_package( ROOT COMPONENTS Core RIO Hist Tree )

# Build a dictionary for the library:
atlas_add_root_dictionary( CxAODMaker _dictionarySource
   ROOT_HEADERS Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Build a shared library:
atlas_add_library( CxAODMaker
   CxAODMaker/*.h Root/*.h Root/*.cxx ${_dictionarySource}
   PUBLIC_HEADERS CxAODMaker
   LINK_LIBRARIES FlavorTagDiscriminants EventLoop AssociationUtilsLib CxAODTools CxAODTools_VHbb   xAODBase    xAODRootAccess    xAODEventInfo xAODBTagging    PATInterfaces    PathResolver    GoodRunsListsLib    xAODJet    xAODMuon    xAODEgamma    xAODTracking    MuonEfficiencyCorrectionsLib    MuonMomentumCorrectionsLib    MuonSelectorToolsLib    JetCalibToolsLib    JetSelectorToolsLib    JetUncertaintiesLib    AthContainers    ElectronPhotonFourMomentumCorrectionLib    ElectronEfficiencyCorrectionLib    ElectronPhotonSelectorToolsLib    IsolationSelectionLib    METUtilitiesLib    METInterface    AsgTools    xAODMissingET    TauAnalysisToolsLib    xAODTau    PileupReweightingLib    TrigConfxAODLib    TrigDecisionToolLib    JetRecLib    JetEDM    PhotonEfficiencyCorrectionLib    xAODCutFlow    xAODTrigMissingET    xAODTrigEgamma    xAODTrigCalo    ElectronPhotonShowerShapeFudgeToolLib    xAODTrigMuon    JetMomentToolsLib    JetSubStructureUtils    TrigTauMatchingLib    IsolationCorrectionsLib    JetJvtEfficiencyLib    TriggerMatchingToolLib  JetTileCorrectionLib    BoostedJetTaggersLib   InDetTrackSystematicsToolsLib TrigBunchCrossingTool)

atlas_install_generic( data/*
   DESTINATION data PKGNAME_SUBDIR )
