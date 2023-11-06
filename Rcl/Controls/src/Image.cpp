#include "Image.h"

#include <QDebug>
#include <QImage>

void Image::setImageData(const Image::Data& _imageData)
{
  m_imageData = _imageData;
  emit(imageDataChanged());
}

namespace
{
  constexpr quint8 scale(quint8 _t) { return _t; }
  constexpr quint8 scale(qint8 _t) { return qint16(_t) + 0x7f; }
  constexpr quint8 scale(quint16 _t) { return _t / 0xFF; }
  constexpr quint8 scale(qint16 _t) { return (qint16(_t) + 0x7fff) / 0xFF; }
  constexpr quint8 scale(quint32 _t) { return _t / 0xFFFF; }
  constexpr quint8 scale(qint32 _t) { return (qint32(_t) + 0x7fffffff) / 0xFFFF; }
  constexpr quint8 scale(quint64 _t) { return _t / 0xFFFFFFFF; }
  constexpr quint8 scale(qint64 _t) { return (quint64(_t) - 0x7fffffffffffffff) / 0xFFFFFFFF; }
  constexpr quint8 scale(float _t) { return _t * 0xFF; }
  constexpr quint8 scale(double _t) { return _t * 0xFF; }

  template<typename _T_>
  struct ChannelConverter
  {
    constexpr static quint8 convert(const char* data)
    {
      return scale(*reinterpret_cast<const _T_*>(data));
    }
    constexpr static std::size_t size()
    {
      return sizeof(_T_);
    }
  };
  
  template<int _red_channel_, int _green_channel_, int _blue_channel_, typename _channel_t_>
  struct ToRgb32PixelGenericConverter
  {
    typedef ::ChannelConverter<_channel_t_> ChannelConverter;
    ToRgb32PixelGenericConverter()
    {
      const quint32 size = ChannelConverter::size();
      red_offset    = _red_channel_ * size;
      green_offset  = _green_channel_ * size;
      blue_offset   = _blue_channel_ * size;
    }
    constexpr void convert(const char* _indata, quint8* _outdata)
    {
      _outdata[0] = ChannelConverter::convert(_indata + red_offset);
      _outdata[1] = ChannelConverter::convert(_indata + green_offset);
      _outdata[2] = ChannelConverter::convert(_indata + blue_offset);
      _outdata[3] = 0xff; // alpha
    }
    constexpr QImage::Format format()
    {
      return QImage::Format_RGB32;
    }
    constexpr std::size_t imgPixelSize()
    {
      return 4;
    }
    constexpr std::size_t dataPixelSize()
    {
      return 3 * ChannelConverter::size();
    }
    quint32 red_offset, green_offset, blue_offset;
  };
  
  template<typename _channel_t_>
  using RgbToRgb32PixelConverter = ToRgb32PixelGenericConverter<2, 1, 0, _channel_t_>;
  template<typename _channel_t_>
  using BgrToRgb32PixelConverter = ToRgb32PixelGenericConverter<0, 1, 2, _channel_t_>;
  
  template<template<typename> class _PixelConverter_>
  struct Converter
  {
    static QImage convert(const Image::Data& _source)
    {
      switch(_source.depth)
      {
        case Image::Depth::UInt8:
          return do_convert<quint8>(_source);
          break;
        case Image::Depth::Int8:
          return do_convert<qint8>(_source);
          break;
        case Image::Depth::UInt16:
          return do_convert<quint16>(_source);
          break;
        case Image::Depth::Int16:
          return do_convert<qint16>(_source);
          break;
        case Image::Depth::UInt32:
          return do_convert<quint32>(_source);
          break;
        case Image::Depth::Int32:
          return do_convert<qint32>(_source);
          break;
        case Image::Depth::UInt64:
          return do_convert<quint64>(_source);
          break;
        case Image::Depth::Int64:
          return do_convert<qint64>(_source);
          break;
        case Image::Depth::Float32:
          return do_convert<float>(_source);
          break;
        case Image::Depth::Float64:
          return do_convert<double>(_source);
          break;
      }
      qFatal("Unsupported depth");
    }
  private:
    template<typename _T_>
    static QImage do_convert(const Image::Data& _source)
    {
      _PixelConverter_<_T_> pc;
      QImage img(_source.width, _source.height, pc.format());
      
      int pos_img = 0;
      int pos_data = 0;
      
      std::size_t data_stride = pc.dataPixelSize();
      std::size_t img_stride = pc.imgPixelSize();
      
      for(std::size_t i = 0; i < _source.width * _source.height; ++i)
      {
        pc.convert(_source.data.data() + pos_data, img.bits() + pos_img);
        pos_data += data_stride;
        pos_img  += img_stride;
        
      }
      return img;
    }
  };
}

QImage Image::toQImage() const
{
  if(m_imageData.isEmpty()) return QImage();
  
  switch(m_imageData.colorspace)
  {
    case Image::Colorspace::RGB:
      return Converter<RgbToRgb32PixelConverter>::convert(m_imageData);
    case Image::Colorspace::BGR:
      return Converter<BgrToRgb32PixelConverter>::convert(m_imageData);
    default:
      qWarning() << "Unsupported colorspace: " << int(m_imageData.colorspace);
      return QImage();
  }
}

QVariant Image::message() const
{
  return m_message;
}

namespace image_encodings
{
  const char RGB8[] = "rgb8";
  const char RGBA8[] = "rgba8";
  const char RGB16[] = "rgb16";
  const char RGBA16[] = "rgba16";
  const char BGR8[] = "bgr8";
  const char BGRA8[] = "bgra8";
  const char BGR16[] = "bgr16";
  const char BGRA16[] = "bgra16";
  const char MONO8[] = "mono8";
  const char MONO16[] = "mono16";

  // OpenCV CvMat types
  const char TYPE_8UC1[] = "8UC1";
  const char TYPE_8UC2[] = "8UC2";
  const char TYPE_8UC3[] = "8UC3";
  const char TYPE_8UC4[] = "8UC4";
  const char TYPE_8SC1[] = "8SC1";
  const char TYPE_8SC2[] = "8SC2";
  const char TYPE_8SC3[] = "8SC3";
  const char TYPE_8SC4[] = "8SC4";
  const char TYPE_16UC1[] = "16UC1";
  const char TYPE_16UC2[] = "16UC2";
  const char TYPE_16UC3[] = "16UC3";
  const char TYPE_16UC4[] = "16UC4";
  const char TYPE_16SC1[] = "16SC1";
  const char TYPE_16SC2[] = "16SC2";
  const char TYPE_16SC3[] = "16SC3";
  const char TYPE_16SC4[] = "16SC4";
  const char TYPE_32SC1[] = "32SC1";
  const char TYPE_32SC2[] = "32SC2";
  const char TYPE_32SC3[] = "32SC3";
  const char TYPE_32SC4[] = "32SC4";
  const char TYPE_32FC1[] = "32FC1";
  const char TYPE_32FC2[] = "32FC2";
  const char TYPE_32FC3[] = "32FC3";
  const char TYPE_32FC4[] = "32FC4";
  const char TYPE_64FC1[] = "64FC1";
  const char TYPE_64FC2[] = "64FC2";
  const char TYPE_64FC3[] = "64FC3";
  const char TYPE_64FC4[] = "64FC4";

  // Bayer encodings
  const char BAYER_RGGB8[] = "bayer_rggb8";
  const char BAYER_BGGR8[] = "bayer_bggr8";
  const char BAYER_GBRG8[] = "bayer_gbrg8";
  const char BAYER_GRBG8[] = "bayer_grbg8";
  const char BAYER_RGGB16[] = "bayer_rggb16";
  const char BAYER_BGGR16[] = "bayer_bggr16";
  const char BAYER_GBRG16[] = "bayer_gbrg16";
  const char BAYER_GRBG16[] = "bayer_grbg16";
}

void Image::setMessage(const QVariant& _message)
{
  struct ChannelsDescription
  {
    Image::Depth depth;
    quint8 channels;
    Image::Colorspace colorspace;
  };

  static QHash<QString, ChannelsDescription> ros_to_channels_description;

  if(ros_to_channels_description.isEmpty())
  {
#define RRCD(__name__, __depth__, __channels__, __colorspace__) \
      ros_to_channels_description[QString::fromStdString(image_encodings::__name__)] = ChannelsDescription{Image::Depth::__depth__, __channels__, Image::Colorspace::__colorspace__};
    RRCD(RGB8, UInt8, 3, RGB);
    RRCD(RGBA8, UInt8, 4, RGBA);
    RRCD(RGB16, UInt16, 3, RGB);
    RRCD(RGBA16, UInt16, 4, RGBA);
    RRCD(BGR8, UInt8, 3, BGR);
    RRCD(BGRA8, UInt8, 4, BGRA);
    RRCD(BGR16, UInt16, 3, BGR);
    RRCD(BGRA16, UInt16, 4, BGRA);
    RRCD(MONO8, UInt8, 1, Mono);
    RRCD(MONO16, UInt16, 1, Mono);

    // OpenCV CvMat types
    RRCD(TYPE_8UC1, UInt8, 1, Unknown);
    RRCD(TYPE_8UC2, UInt8, 2, Unknown);
    RRCD(TYPE_8UC3, UInt8, 3, Unknown);
    RRCD(TYPE_8UC4, UInt8, 4, Unknown);
    RRCD(TYPE_8SC1, Int8, 1, Unknown);
    RRCD(TYPE_8SC2, Int8, 2, Unknown);
    RRCD(TYPE_8SC3, Int8, 3, Unknown);
    RRCD(TYPE_8SC4, Int8, 4, Unknown);
    RRCD(TYPE_16UC1, UInt16, 1, Unknown);
    RRCD(TYPE_16UC2, UInt16, 2, Unknown);
    RRCD(TYPE_16UC3, UInt16, 3, Unknown);
    RRCD(TYPE_16UC4, UInt16, 4, Unknown);
    RRCD(TYPE_16SC1, Int16, 1, Unknown);
    RRCD(TYPE_16SC2, Int16, 2, Unknown);
    RRCD(TYPE_16SC3, Int16, 3, Unknown);
    RRCD(TYPE_16SC4, Int16, 4, Unknown);
    RRCD(TYPE_32SC1, Int32, 1, Unknown);
    RRCD(TYPE_32SC2, Int32, 2, Unknown);
    RRCD(TYPE_32SC3, Int32, 3, Unknown);
    RRCD(TYPE_32SC4, Int32, 4, Unknown);
    RRCD(TYPE_32FC1, Float32, 1, Unknown);
    RRCD(TYPE_32FC2, Float32, 2, Unknown);
    RRCD(TYPE_32FC3, Float32, 3, Unknown);
    RRCD(TYPE_32FC4, Float32, 4, Unknown);
    RRCD(TYPE_64FC1, Float64, 1, Unknown);
    RRCD(TYPE_64FC2, Float64, 2, Unknown);
    RRCD(TYPE_64FC3, Float64, 3, Unknown);
    RRCD(TYPE_64FC4, Float64, 4, Unknown);

    // Bayer encodings
    RRCD(BAYER_RGGB8, UInt8, 1, BayerRGGB);
    RRCD(BAYER_BGGR8, UInt8, 1, BayerBGGR);
    RRCD(BAYER_GBRG8, UInt8, 1, BayerGBRG);
    RRCD(BAYER_GRBG8, UInt8, 1, BayerGRBG);
    RRCD(BAYER_RGGB16, UInt16, 1, BayerRGGB);
    RRCD(BAYER_BGGR16, UInt16, 1, BayerBGGR);
    RRCD(BAYER_GBRG16, UInt16, 1, BayerGBRG);
    RRCD(BAYER_GRBG16, UInt16, 1, BayerGRBG);
  }

  m_message = _message;
  emit(messageChanged());

  // Build data
  Data data;
  QVariantMap map_message = _message.toMap();

  data.data = map_message["data"].toByteArray();

  data.width = map_message["width"].toUInt();
  data.height = map_message["height"].toUInt();
  data.step = map_message["step"].toUInt();

  auto it = ros_to_channels_description.find(map_message["encoding"].toString());
  if(it == ros_to_channels_description.end())
  {
    qWarning() << "Unknown image encoding '" << map_message["encoding"].toString() << "'.";
    return;
  }
  const ChannelsDescription& cd = it.value();
  data.depth = cd.depth;
  data.channels = cd.channels;
  data.colorspace = cd.colorspace;
  setImageData(data);
}






