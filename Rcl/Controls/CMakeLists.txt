add_subdirectory(Charts)
add_subdirectory(Maps/)

install(FILES Event.qml Events.qml EventsLogger.qml Joystick.qml MessageView.qml qmldir
        DESTINATION ${INSTALL_QML_DIR}/Rcl/Controls)
