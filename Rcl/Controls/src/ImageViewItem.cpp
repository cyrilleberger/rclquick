#include "ImageViewItem.h"

#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <QtConcurrent/QtConcurrent>

#include "Image.h"

ImageViewItem::ImageViewItem(QQuickItem* _parent) : QQuickItem(_parent), m_image(new Image)
{
  setFlag(QQuickItem::ItemHasContents);
  m_pool.setMaxThreadCount(1);
}

ImageViewItem::~ImageViewItem()
{
}

Image* ImageViewItem::image() const
{
  return m_image;
}

void ImageViewItem::updateImage()
{
  m_pool.clear();
  Image::Data data = m_image->imageData(); // Make a copy of data to use it in a different thread
  QtConcurrent::run(&m_pool, [this, data](){
    m_img = Image(data).toQImage();
    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
  });
}

void ImageViewItem::setImage(Image* _image)
{
  delete m_image;
  m_image = _image;
  if(m_image)
  {
    m_image->setParent(this);
    updateImage();
    connect(m_image, SIGNAL(imageDataChanged()), SLOT(updateImage()));
  }
  emit(imageChanged());
}

QSGNode* ImageViewItem::updatePaintNode(QSGNode* _oldNode, UpdatePaintNodeData* _upnd)
{
  Q_UNUSED(_upnd);
  delete m_texture;
  m_texture = window()->createTextureFromImage(m_img);
  QSGSimpleTextureNode* textureNode = new QSGSimpleTextureNode;
  textureNode->setRect(0, 0, width(), height());
  textureNode->setTexture(m_texture);
  delete _oldNode;
  return textureNode;
}
