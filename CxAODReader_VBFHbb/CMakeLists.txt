################################################################################
# Build configuration for CxAODReader_VBFHbb
################################################################################

# Declare the name of the package:
atlas_subdir( CxAODReader_VBFHbb )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC 
   CxAODReader
   CxAODTools_VBFHbb
   KinematicFit
   Reconstruction/Jet/JetUncertainties
#   Reconstruction/Jet/JetCPInterfaces
   )

find_package( ROOT COMPONENTS Core Tree MathCore Hist Physics TMVA XMLParser XMLIO)

atlas_add_executable( vbfhbbframeworkReadCxAOD
   util/vbfhbbframeworkReadCxAOD.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODBase xAODRootAccess xAODEventInfo PATInterfaces
   EventLoop EventLoopGrid CxAODReader CxAODReader_VBFHbb CxAODTools
   )

# Build a dictionary for the library:
atlas_add_root_dictionary( CxAODReader_VBFHbb _dictionarySource
   ROOT_HEADERS Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Build a shared library:
atlas_add_library( CxAODReader_VBFHbb
   CxAODReader_VBFHbb/*.h Root/*.h Root/*.cxx ${_dictionarySource}
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PUBLIC_HEADERS CxAODReader_VBFHbb
#   LINK_LIBRARIES ${ROOT_LIBRARIES} CxAODReader CxAODTools_VBFHbb KinematicFit JetUncertaintiesLib JetCPInterfacesLib)
LINK_LIBRARIES ${ROOT_LIBRARIES} CxAODReader CxAODTools_VBFHbb KinematicFit JetUncertaintiesLib)

atlas_install_generic( data/*
   DESTINATION data PKGNAME_SUBDIR )
