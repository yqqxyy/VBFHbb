################################################################################
# Build configuration for CxAODTools
################################################################################

# Declare the name of the package:
atlas_subdir( CxAODTools )

# External dependencies:
find_package( ROOT COMPONENTS TMVA XMLParser)

# Build a dictionary for the library:
atlas_add_root_dictionary( CxAODTools _dictionarySource
   ROOT_HEADERS Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT)
   
# Build a shared library:
atlas_add_library( CxAODTools
   CxAODTools/*.h Root/*.h Root/*.cxx ${_dictionarySource}
   PUBLIC_HEADERS CxAODTools
   LINK_LIBRARIES ${ROOT_LIBRARIES} EventLoop PathResolver xAODBase xAODRootAccess xAODEventInfo PATInterfaces xAODJet xAODMuon xAODEgamma xAODTracking AthContainers xAODBTagging xAODBTaggingEfficiencyLib AssociationUtilsLib xAODMissingET xAODLuminosity MuonEfficiencyCorrectionsLib PileupReweightingLib JetCalibToolsLib JetInterface )

atlas_add_dictionary( CxAODToolsDict
   CxAODTools/CxAODToolsDict.h
   CxAODTools/selection.xml
   LINK_LIBRARIES CxAODTools 
   EXTRA_FILES Root/dict/*.cxx )

# Install files from the package:
atlas_install_scripts( scripts/*.sh )

atlas_install_generic( data/*
   DESTINATION data PKGNAME_SUBDIR )
