SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
#SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#INCLUDE_DIRECTORIES( ${UsSimulatorTest_INCLUDE_DIRS} )
ADD_EXECUTABLE(UsSimulatorAlgoTest vtkUsSimulatorAlgoTest.cxx )
TARGET_LINK_LIBRARIES(UsSimulatorAlgoTest UsSimulatorAlgo vtkCommon )

INSTALL(TARGETS UsSimulatorAlgoTest
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)