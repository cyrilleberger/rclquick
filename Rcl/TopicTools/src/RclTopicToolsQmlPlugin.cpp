#include "RclTopicToolsQmlPlugin.h"

#include <QtQml>

#include "TimeSynchroniser.h"

RclTopicToolsQmlPlugin::RclTopicToolsQmlPlugin()
{
}

void RclTopicToolsQmlPlugin::registerTypes(const char *uri)
{
  qmlRegisterType<TimeSynchroniser>(uri, 1, 0, "TimeSynchroniser");
}
