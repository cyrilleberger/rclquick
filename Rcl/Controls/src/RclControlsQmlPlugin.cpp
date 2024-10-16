#include "RclControlsQmlPlugin.h"

#include <QtQml>

#include "Image.h"
#include "ImageViewItem.h"

RclControlsQmlPlugin::RclControlsQmlPlugin() {}

void RclControlsQmlPlugin::registerTypes(const char* uri)
{
  qmlRegisterType<Image>(uri, 1, 0, "Image");
  qmlRegisterType<ImageViewItem>(uri, 1, 0, "ImageView");
}
