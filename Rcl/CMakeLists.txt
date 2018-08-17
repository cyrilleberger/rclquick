########################
## subdirectories

add_subdirectory(Controls/)
add_subdirectory(ImageTransport/)

########################
## rcl plugin

set(RCL_PLUGIN_SRCS
#   src/MessageDefinition.cpp
#   src/MessageField.cpp
#   src/MessageMessageField.cpp
#   src/Publisher.cpp
  src/RosObject.cpp
#   src/RosQmlPlugin.cpp
  src/RosThread.cpp
#   src/RosWrapper.cpp
#   src/ServiceClient.cpp
#   src/ServiceDefinition.cpp
#   src/Subscriber.cpp
  )

add_library(rclqmlplugin MODULE ${RCL_PLUGIN_SRCS})
ament_target_dependencies(rclqmlplugin rcl)
target_link_libraries(rclqmlplugin Qt5::Quick Qt5::Concurrent)

INSTALL(TARGETS rclqmlplugin
  RUNTIME DESTINATION ${INSTALL_QML_DIR}/Rcl
  LIBRARY DESTINATION ${INSTALL_QML_DIR}/Rcl
)

install(FILES qml/Time.js qmldir
        DESTINATION ${INSTALL_QML_DIR}/Rcl/)
