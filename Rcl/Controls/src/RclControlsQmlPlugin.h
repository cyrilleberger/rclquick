#pragma once

#include <QQmlExtensionPlugin>

class RclControlsQmlPlugin : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
  RclControlsQmlPlugin();
  void registerTypes(const char *uri);
};
