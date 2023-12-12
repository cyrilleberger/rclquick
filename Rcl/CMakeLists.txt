########################
## subdirectories

add_subdirectory(Controls/)
add_subdirectory(ImageTransport/)
add_subdirectory(TopicTools/)

########################
## rcl plugin

set(RCL_PLUGIN_SRCS
  src/MessageData.cpp
  src/MessageDefinition.cpp
  src/MessageField.cpp
  src/MessageMessageField.cpp
  src/Publisher.cpp
  src/RosObject.cpp
  src/RosQmlPlugin.cpp
  src/RosThread.cpp
  src/RosWrapper.cpp
  src/ServiceClient.cpp
  src/ServiceDefinition.cpp
  src/Subscriber.cpp
  src/TypeSupport.cpp
  )

add_library(rclqmlplugin MODULE ${RCL_PLUGIN_SRCS})
ament_target_dependencies(rclqmlplugin rcl ament_index_cpp)
target_link_libraries(rclqmlplugin Qt5::Quick Qt5::Concurrent)

INSTALL(TARGETS rclqmlplugin
  RUNTIME DESTINATION ${INSTALL_QML_DIR}/Rcl
  LIBRARY DESTINATION ${INSTALL_QML_DIR}/Rcl
)

install(DIRECTORY qml/Rcl
        DESTINATION ${INSTALL_QML_DIR}/)
