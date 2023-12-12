#pragma once

#include <QQmlExtensionPlugin>

class RclTopicToolsQmlPlugin : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
  RclTopicToolsQmlPlugin();
  void registerTypes(const char *uri);
};
