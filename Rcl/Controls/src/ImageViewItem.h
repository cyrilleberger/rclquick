#ifndef _IMAGEVIEWITEM_H_
#define _IMAGEVIEWITEM_H_

#include <QImage>
#include <QQuickItem>
#include <QThreadPool>

class Image;
class QSGTexture;

class ImageViewItem : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(Image* image READ image WRITE setImage NOTIFY imageChanged)
  Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
public:
  enum class FillMode
  {
    NoFill,
    Fit
  };
  Q_ENUM(FillMode)
public:
  ImageViewItem(QQuickItem* _parent = 0);
  virtual ~ImageViewItem();
public:
  Image* image() const;
  void setImage(Image* _image);
  FillMode fillMode() const;
  void setFillMode(FillMode _fillMode);
signals:
  void imageChanged();
  void fillModeChanged();
protected:
  virtual QSGNode* updatePaintNode(QSGNode* _oldNode, UpdatePaintNodeData* _upnd);
private slots:
  void updateImage();
private:
  Image* m_image;
  QImage m_img;
  QSGTexture* m_texture = nullptr;
  QThreadPool m_pool;
  FillMode m_fillMode = FillMode::Fit;
};

#endif
