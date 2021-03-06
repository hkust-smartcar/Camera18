if("${SCCC_CONFIG}" STREQUAL "INNO")
set(BIN_SUFFIX "${BIN_SUFFIX}-inno")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_INNO=1")
set(SCCC_MCU MK60DZ10)
message("Config = INNO")

elseif("${SCCC_CONFIG}" STREQUAL "2014_CAMERA")
set(BIN_SUFFIX "${BIN_SUFFIX}-2014-camera")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2014_CAMERA=1")
set(SCCC_MCU MK60DZ10)
message("Config = 2014_CAMERA")

elseif("${SCCC_CONFIG}" STREQUAL "2014_CCD")
set(BIN_SUFFIX "${BIN_SUFFIX}-2014-ccd")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2014_CCD=1")
set(SCCC_MCU MK60DZ10)
message("Config = 2014_CCD")

elseif("${SCCC_CONFIG}" STREQUAL "2014_INNO")
set(BIN_SUFFIX "${BIN_SUFFIX}-2014-inno")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2014_INNO=1")
set(SCCC_MCU MK60DZ10)
message("Config = 2014_INNO")

elseif("${SCCC_CONFIG}" STREQUAL "2014_INNO_FX")
set(BIN_SUFFIX "${BIN_SUFFIX}-2014-inno-fx")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2014_INNO=1")
set(SCCC_MCU MK60F15)
message("Config = 2014_INNO_FX")

elseif("${SCCC_CONFIG}" STREQUAL "2014_MAGNETIC")
set(BIN_SUFFIX "${BIN_SUFFIX}-2014-magnetic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2014_MAGNETIC=1")
set(SCCC_MCU MK60DZ10)
message("Config = 2014_MAGNETIC")

elseif("${SCCC_CONFIG}" STREQUAL "VCAN_FX15DEV")
set(BIN_SUFFIX "${BIN_SUFFIX}-vcan-fx15dev")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_VCAN_FX15DEV=1")
set(SCCC_MCU MK60F15)
message("Config = VCAN_FX15DEV")

elseif("${SCCC_CONFIG}" STREQUAL "2015_CAMERA")
set(BIN_SUFFIX "${BIN_SUFFIX}-2015-camera")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2015_CAMERA=1")
set(SCCC_MCU MK60F15)
message("Config = 2015_CAMERA")

elseif("${SCCC_CONFIG}" STREQUAL "2015_MAGNETIC")
set(BIN_SUFFIX "${BIN_SUFFIX}-2015-magnetic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2015_MAGNETIC=1")
set(SCCC_MCU MK60F15)
message("Config = 2015_MAGNETIC")

elseif("${SCCC_CONFIG}" STREQUAL "2015_CCD")
set(BIN_SUFFIX "${BIN_SUFFIX}-2015-ccd")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2015_CCD=1")
set(SCCC_MCU MK60DZ10)
message("Config = 2015_CCD")

elseif("${SCCC_CONFIG}" STREQUAL "VCAN_Z4DEV")
set(BIN_SUFFIX "${BIN_SUFFIX}-vcan-z4dev")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DKL26_VCAN_Z4DEV=1")
set(SCCC_MCU MKL26Z4)
message("Config = VCAN_Z4DEV")


elseif("${SCCC_CONFIG}" STREQUAL "2015_CCD2")
set(BIN_SUFFIX "${BIN_SUFFIX}-2015_ccd2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DKL26_2015_CCD2=1")
set(SCCC_MCU MKL26Z4)
message("Config = 2015_CCD2")

elseif("${SCCC_CONFIG}" STREQUAL "2015_M2")
set(BIN_SUFFIX "${BIN_SUFFIX}-2015_m2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DKL26_2015_M2=1")
set(SCCC_MCU MKL26Z4)
message("Config = 2015_M2")

elseif("${SCCC_CONFIG}" STREQUAL "2016_INNO")
set(BIN_SUFFIX "${BIN_SUFFIX}-2016-inno")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2016_INNO=1")
set(SCCC_MCU MK60F15)
message("Config = 2016_INNO")

elseif("${SCCC_CONFIG}" STREQUAL "2016_CAMERA")
set(BIN_SUFFIX "${BIN_SUFFIX}-2016-camera")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2016_CAMERA=1")
set(SCCC_MCU MK60F15)
message("Config = 2016_CAMERA")

elseif("${SCCC_CONFIG}" STREQUAL "2016_CCD")
set(BIN_SUFFIX "${BIN_SUFFIX}-2016-ccd")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2016_CCD=1")
set(SCCC_MCU MK60F15)
message("Config = 2016_CCD")

elseif("${SCCC_CONFIG}" STREQUAL "2016_CAMERA")
set(BIN_SUFFIX "${BIN_SUFFIX}-2016-camera")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2016_CAMERA=1")
set(SCCC_MCU MK60F15)
message("Config = 2016_CAMERA")

elseif("${SCCC_CONFIG}" STREQUAL "2016_RAIL")
set(BIN_SUFFIX "${BIN_SUFFIX}-2016-rail")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2016_RAIL=1")
set(SCCC_MCU MK60F15)
message("Config = 2016_RAIL")

elseif("${SCCC_CONFIG}" STREQUAL "2016_MAGNETIC")
set(BIN_SUFFIX "${BIN_SUFFIX}-2016-magnetic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2016_MAGNETIC=1")
set(SCCC_MCU MK60F15)
message("Config = 2016_MAGNETIC")

elseif("${SCCC_CONFIG}" STREQUAL "2016_MAGNETIC_YANG")
set(BIN_SUFFIX "${BIN_SUFFIX}-2016_magnetic_yang")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2016_MAGNETIC_YANG=1")
set(SCCC_MCU MK60F15)
message("Config = 2016_MAGNETIC_YANG")

elseif("${SCCC_CONFIG}" STREQUAL "2017_INNO")
set(BIN_SUFFIX "${BIN_SUFFIX}-2017-inno")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2017_INNO=1")
set(SCCC_MCU MK60F15)
message("Config = 2017_INNO")

elseif("${SCCC_CONFIG}" STREQUAL "2017_CREATIVE")
set(BIN_SUFFIX "${BIN_SUFFIX}-2017-creative")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2017_CREATIVE=1")
set(SCCC_MCU MK60F15)
message("Config = 2017_CREATIVE")

elseif("${SCCC_CONFIG}" STREQUAL "2018_CAMERA")
set(BIN_SUFFIX "${BIN_SUFFIX}-2018-camera")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DK60_2018_CAMERA=1")
set(SCCC_MCU MK60F15)
message("Config = 2018_CAMERA")

else()
message(FATAL_ERROR "Missing/Unknown config '${SCCC_CONFIG}' (set SCCC_CONFIG)")

endif()
