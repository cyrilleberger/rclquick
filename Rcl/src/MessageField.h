#ifndef _RCLQML_MESSAGE_FIELD_H_
#define _RCLQML_MESSAGE_FIELD_H_

#include <QObject>

namespace ros
{
  namespace serialization
  {
    class IStream;
    class OStream;
    class LStream;
  } // namespace serialization
} // namespace ros

class MessageField : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(Type type READ type CONSTANT)
public:
  enum class Type
  {
    Bool,
    Int8,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float32,
    Float64,
    String,
    Time,
    Duration,
    Message,
    ByteArray
  };
  Q_ENUM(Type)
public:
  MessageField(const QString& _name, Type _type, bool _array, std::size_t _index);
  QString name() const { return m_name; }
  Type type() const { return m_type; }
  bool isArray() const { return m_is_array; }
  std::size_t index() const;
protected:
  virtual void elementInitialize(quint8* _data) const = 0;
  virtual void elementFinalize(quint8* _data) const = 0;
  virtual QVariant elementReadValue(const quint8* _data) const = 0;
  virtual void elementWriteValue(quint8* _data, const QVariant& _value) const = 0;
  virtual std::size_t elementSize() const = 0;
  virtual std::size_t elementAlignment() const = 0;
public:
  void fieldInitialize(quint8* _data) const;
  void fieldFinalize(quint8* _data) const;
  QVariant fieldReadValue(const quint8* _data) const;
  void fieldWriteValue(quint8* _data, const QVariant& _value) const;
  std::size_t fieldSize() const;
  std::size_t alignment() const;
private:
  QString m_name;
  Type m_type;
  bool m_is_array;
  std::size_t m_index;
};

#endif
