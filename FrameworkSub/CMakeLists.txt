################################################################################
# Build configuration for FrameworkSub
################################################################################

# Declare the name of the package:
atlas_subdir( FrameworkSub )

# Declare the package's dependencies:
atlas_depends_on_subdirs()

# Build a shared library:
atlas_add_library( FrameworkSub
   ${_dictionarySource}
   LINK_LIBRARIES )

atlas_install_generic( data/*
   DESTINATION data PKGNAME_SUBDIR )
