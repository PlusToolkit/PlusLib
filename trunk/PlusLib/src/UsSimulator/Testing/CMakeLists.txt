SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
#SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#INCLUDE_DIRECTORIES( ${UsSimulatorTest_INCLUDE_DIRS} )
ADD_EXECUTABLE(UsSimulatorTest vtkUsSimulatorAlgoTest.cxx )
TARGET_LINK_LIBRARIES(UsSimulatorTest UsSimulator vtkCommon )

INSTALL(TARGETS UsSimulatorTest
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)