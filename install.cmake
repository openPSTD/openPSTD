#The MXE boost refers to the static interface libraries and not directly to the dll, so this function solves that problem
macro(WinInstallBoost BoostModule)
    install(FILES ${Boost_LIBRARY_DIRS}/../bin/libboost_${BoostModule}-mt.dll DESTINATION .)
endmacro(WinInstallBoost)

#------------------------------------
# INSTALL
if(OPENPSTD_SYSTEM_NAME STREQUAL "osx")
    message(STATUS "Package for osx")

    install(TARGETS OpenPSTD-gui DESTINATION .)
    #install(TARGETS OpenPSTD-cli DESTINATION .)

    # paths to executables
    set(APPS "${CMAKE_SOURCE_DIR}/_CPack_Packages/${OPENPSTD_SYSTEM_NAME}/Bundle/OpenPSTD-${OPENPSTD_VERSION_MAJOR}.${OPENPSTD_VERSION_MINOR}.${OPENPSTD_VERSION_PATCH}-${OPENPSTD_SYSTEM_NAME}/OpenPSTD.app/Contents/Resources/OpenPSTD-gui")

    message(STATUS "Mac OS X app: ${APPS}")
    message(STATUS "Mac OS X dependencies: ${OPENPSTD_DEPENDENCIES_SEARCH_PATH}")

    INSTALL(CODE "
	   include(BundleUtilities)
	   fixup_bundle(\"${APPS}\"   \"\"   \"${OPENPSTD_DEPENDENCIES_SEARCH_PATH}\")
	   " COMPONENT Runtime)

elseif(OPENPSTD_SYSTEM_NAME STREQUAL "win64")
    message(STATUS "Package for win64")

    install(TARGETS OpenPSTD DESTINATION .)
    install(TARGETS OpenPSTD-gui DESTINATION .)
    install(TARGETS OpenPSTD-cli DESTINATION .)
    install(TARGETS unqlite DESTINATION .)

    install(FILES ${Boost_LIBRARIES} DESTINATION .)
    #special function for windows builds (error in MXE)
    WinInstallBoost(program_options)
    WinInstallBoost(unit_test_framework)
    WinInstallBoost(chrono)
    WinInstallBoost(system)
    WinInstallBoost(timer)
    WinInstallBoost(regex)
    install(FILES ${QtCore_location} DESTINATION .)
    install(FILES ${QtWidgets_location} DESTINATION .)
    install(FILES ${QtOpenGL_location} DESTINATION .)
    install(FILES ${Qt5_LIBRARIES_LOCATIONS} DESTINATION .)
    install(FILES ${FFTWF_LIBRARY} DESTINATION .)
    install(FILES ${HDF5_LIBRARY} DESTINATION .)
    install(FILES ${HDF5_HL_LIBRARY} DESTINATION .)

elseif(OPENPSTD_SYSTEM_NAME STREQUAL "linux")
    message(STATUS "Package for linux")

    install(TARGETS OpenPSTD DESTINATION lib)
    install(TARGETS OpenPSTD-gui DESTINATION bin)
    install(TARGETS OpenPSTD-cli DESTINATION bin)
    install(TARGETS unqlite DESTINATION lib)

    install(FILES ${Boost_LIBRARIES} DESTINATION lib)
    install(FILES ${QtCore_location} DESTINATION lib)
    install(FILES ${QtWidgets_location} DESTINATION lib)
    install(FILES ${QtOpenGL_location} DESTINATION lib)
    install(FILES ${Qt5_LIBRARIES_LOCATIONS} DESTINATION lib)
    install(FILES ${FFTWF_SHARED_OBJECT} DESTINATION lib)
    install(FILES ${HDF5_LIBRARY} DESTINATION lib)
    install(FILES ${HDF5_HL_LIBRARY} DESTINATION lib)

    message(STATUS "Package for linux")
endif()

set(CPACK_BUNDLE_NAME OpenPSTD)
set(CPACK_BUNDLE_ICON GUI/OpenPSTD.icns)
set(CPACK_BUNDLE_PLIST GUI/Info.plist)
set(CPACK_BUNDLE_STARTUP_COMMAND OpenPSTD-gui)

set(CPACK_PACKAGE_VERSION_MAJOR ${OPENPSTD_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${OPENPSTD_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${OPENPSTD_VERSION_PATCH})
set(CPACK_SYSTEM_NAME ${OPENPSTD_SYSTEM_NAME})

include(CPack)