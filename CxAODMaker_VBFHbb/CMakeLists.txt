################################################################################
# Build configuration for CxAODMaker_VBFHbb
################################################################################

# Declare the name of the package:
atlas_subdir( CxAODMaker_VBFHbb )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC 
   CxAODMaker
   CxAODMaker_VHbb
   CxAODTools_VBFHbb
   PhysicsAnalysis/D3PDTools/EventLoop 
   Trigger/TriggerSimulation/TrigBtagEmulationTool
   TruthWeightTools
   )

find_package( ROOT COMPONENTS Core RIO Hist Tree )

atlas_add_executable( vbfhbbframework
   util/vbfhbbframework.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODBase xAODRootAccess xAODEventInfo PATInterfaces
   EventLoop EventLoopGrid CxAODMaker CxAODMaker_VBFHbb CxAODTools
   )

# Build a dictionary for the library:
atlas_add_root_dictionary( CxAODMaker_VBFHbb _dictionarySource
   ROOT_HEADERS Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Build a shared library:
atlas_add_library( CxAODMaker_VBFHbb
   CxAODMaker_VBFHbb/*.h Root/*.h Root/*.cxx ${_dictionarySource}
   PUBLIC_HEADERS CxAODMaker_VBFHbb
   LINK_LIBRARIES EventLoop CxAODMaker CxAODMaker_VHbb CxAODTools_VBFHbb TrigBtagEmulationToolLib TruthWeightToolsLib)

atlas_install_generic( data/*
   DESTINATION data PKGNAME_SUBDIR )
