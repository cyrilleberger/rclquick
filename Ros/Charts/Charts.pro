TEMPLATE=lib

uri      = Ros.Charts

OTHER_FILES += qmldir

qmldir.files = qmldir LinePlot.qml

unix {
    isEmpty(PREFIX) {
      installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    }
    else {
      installPath = $$PREFIX/lib/qt5/qml/$$replace(uri, \\., /)
    }

    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}


