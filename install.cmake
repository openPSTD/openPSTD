#------------------------------------
# INSTALL
if(APPLE)
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
elseif(WIN32)
    install(TARGETS OpenPSTD DESTINATION OpenPSTD)
    install(TARGETS OpenPSTD-gui DESTINATION OpenPSTD)
    #install(TARGETS OpenPSTD-cli DESTINATION OpenPSTD)
    install(TARGETS unqlite DESTINATION OpenPSTD)

    #install(FILES ${Boost_LIBRARIES} DESTINATION OpenPSTD)
    #install(FILES ${QtCore_location} DESTINATION OpenPSTD)
    #install(FILES ${QtWidgets_location} DESTINATION OpenPSTD)
    #install(FILES ${QtOpenGL_location} DESTINATION OpenPSTD)
    #install(FILES ${Qt5_LIBRARIES_LOCATIONS} DESTINATION OpenPSTD)
    #install(FILES ${FFTWF_SHARED_OBJECT} DESTINATION OpenPSTD)

elseif(UNIX)
    install(TARGETS OpenPSTD DESTINATION lib)
    install(TARGETS OpenPSTD-gui DESTINATION bin)
    #install(TARGETS OpenPSTD-cli DESTINATION bin)
    install(TARGETS unqlite DESTINATION lib)

    install(FILES ${Boost_LIBRARIES} DESTINATION lib)
    install(FILES ${QtCore_location} DESTINATION lib)
    install(FILES ${QtWidgets_location} DESTINATION lib)
    install(FILES ${QtOpenGL_location} DESTINATION lib)
    install(FILES ${Qt5_LIBRARIES_LOCATIONS} DESTINATION lib)
    install(FILES ${FFTWF_SHARED_OBJECT} DESTINATION OpenPSTD)
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