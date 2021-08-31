################################################################################
# Build configuration for FrameworkExe_VBFHbb
################################################################################

# Declare the name of the package:
atlas_subdir( FrameworkExe_VBFHbb )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC 
   CxAODMaker
   CxAODMaker_VBFHbb
   CxAODReader
   CxAODReader_VBFHbb
   CxAODTools
   PhysicsAnalysis/D3PDTools/EventLoop
   PhysicsAnalysis/D3PDTools/EventLoopGrid
   Event/xAOD/xAODBase
   Event/xAOD/xAODEventInfo
   Control/xAODRootAccess
   PhysicsAnalysis/AnalysisCommon/PATInterfaces/PATInterfaces
   )

find_package( ROOT COMPONENTS Core Tree)

# Install files from the package:
atlas_add_executable( vbfhbbframework
   util/vbfhbbframework.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODBase xAODRootAccess xAODEventInfo PATInterfaces
   EventLoop EventLoopGrid CxAODMaker CxAODMaker_VBFHbb CxAODTools
   )

atlas_add_executable( vbfhbbframeworkReadCxAOD
   util/vbfhbbframeworkReadCxAOD.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODBase xAODRootAccess xAODEventInfo PATInterfaces
   EventLoop EventLoopGrid CxAODReader CxAODReader_VBFHbb CxAODTools
   )

atlas_install_generic( data/*
   DESTINATION data PKGNAME_SUBDIR )
