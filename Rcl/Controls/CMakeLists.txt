########################
## rcl plugin

set(RCL_CONTROLS_PLUGIN_SRCS
  src/Image.cpp
  src/ImageViewItem.cpp
  src/RclControlsQmlPlugin.cpp
  )

add_library(rclcontrolsqmlplugin MODULE ${RCL_CONTROLS_PLUGIN_SRCS})
# ament_target_dependencies(rclcontrolsqmlplugin rcl ament_index_cpp)
target_link_libraries(rclcontrolsqmlplugin Qt5::Quick Qt5::Concurrent)

INSTALL(TARGETS rclcontrolsqmlplugin
  RUNTIME DESTINATION ${INSTALL_QML_DIR}/Rcl/Controls
  LIBRARY DESTINATION ${INSTALL_QML_DIR}/Rcl/Controls
)

install(DIRECTORY qml/Rcl
        DESTINATION ${INSTALL_QML_DIR}/)
