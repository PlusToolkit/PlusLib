# _____________________________________________________________________________
#
#     BKM Data File Library
# _____________________________________________________________________________
#

# -----------------------------------------------------------------------------
#     Header Files
# -----------------------------------------------------------------------------
set( HDR
     BkmDataFile.h
     BkmDataFileFormat.h
     BkmDataFileFrameUtil.h )


# -----------------------------------------------------------------------------
#    CPP Files
# -----------------------------------------------------------------------------

set( CPP 
     BkmDataFile.cpp
     BkmDataFileUtil.cpp )


# -----------------------------------------------------------------------------
#   Visual Studio Options
# -----------------------------------------------------------------------------

add_definitions( -W4 )       # Warning level 4

if (MSVC)
   add_definitions( -WX )    # Treat warnings as errors
endif(MSVC)


# -----------------------------------------------------------------------------
#   Add Library
# -----------------------------------------------------------------------------

add_library(BkmDataFile ${HDR} ${CPP})



# -----------------------------------------------------------------------------
#   Installation instructions
# -----------------------------------------------------------------------------

install ( TARGETS BkmDataFile
		  ARCHIVE
          DESTINATION lib
          COMPONENT Libraries )

install ( FILES ${HDR}
          DESTINATION include
          COMPONENT Libraries )


# ----------------------------------------------------------------------------
# Doxygen documentation
# ----------------------------------------------------------------------------
if (BUILD_DOXYGEN_DOCS)
	add_doxygen_target(BkmDataFileLib_Doc)
endif(BUILD_DOXYGEN_DOCS)
