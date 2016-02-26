#------------------------------------
# UI Graphical
# Operations
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} GUI/operations/BaseOperation.cpp GUI/operations/FileOperations.cpp
        GUI/operations/EditOperations.cpp GUI/operations/ViewOperations.cpp
        GUI/operations/InitializationOperation.cpp GUI/operations/MouseOperations.cpp
        GUI/operations/SelectionOperations.cpp)
# mouse handlers
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} GUI/MouseHandlers.cpp)
# OpenGL viewer
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} GUI/Model.cpp GUI/Viewer2D.cpp GUI/GridLayer.cpp GUI/SceneLayer.cpp
        GUI/InteractiveLayer.cpp GUI/IconLayer.cpp)
# UI files
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} GUI/MainWindow.cpp GUI/DomainProperties.cpp GUI/DocumentSettings.cpp)
# Resource files
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} GUI/Resources.qrc)
# Remaining
set(SOURCE_FILES_GUI ${SOURCE_FILES_GUI} GUI/Model.cpp GUI/Snapping.cpp GUI/PstdAlgorithm.cpp GUI/Edges.cpp)
add_executable(OpenPSTD-gui ${SOURCE_FILES_GUI} GUI/main.cpp)

# create extra Qt5 code
set_target_properties(OpenPSTD-gui
        PROPERTIES AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON)

target_include_directories(OpenPSTD-gui PUBLIC ${Qt5_INCLUDE_DIRS})
target_include_directories(OpenPSTD-gui PUBLIC unqlite)
target_include_directories(OpenPSTD-gui PUBLIC ${Boost_INCLUDE_DIR})
target_include_directories(OpenPSTD-gui PUBLIC ${OPENGL_INCLUDE_DIR})

target_link_libraries(OpenPSTD-gui OpenPSTD)
target_link_libraries(OpenPSTD-gui OpenPSTD-shared)
target_link_libraries(OpenPSTD-gui unqlite)
target_link_libraries(OpenPSTD-gui ${Boost_LIBRARIES})
target_link_libraries(OpenPSTD-gui ${Qt5_LIBRARIES})
target_link_libraries(OpenPSTD-gui ${OPENGL_LIBRARIES})