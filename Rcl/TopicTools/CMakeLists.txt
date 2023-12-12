########################
## rcl plugin

set(RCL_TOPICTOOLS_PLUGIN_SRCS
  src/TimeSynchroniser.cpp
  src/RclTopicToolsQmlPlugin.cpp
  )

add_library(rcltopictoolsqmlplugin MODULE ${RCL_TOPICTOOLS_PLUGIN_SRCS})
# ament_target_dependencies(rcltopictoolsqmlplugin rcl ament_index_cpp)
target_link_libraries(rcltopictoolsqmlplugin Qt5::Quick Qt5::Concurrent)

INSTALL(TARGETS rcltopictoolsqmlplugin
  RUNTIME DESTINATION ${INSTALL_QML_DIR}/Rcl/TopicTools
  LIBRARY DESTINATION ${INSTALL_QML_DIR}/Rcl/TopicTools
)

install(DIRECTORY qml/Rcl
        DESTINATION ${INSTALL_QML_DIR}/)
