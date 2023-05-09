#include "MessageDefinition.h"

#include <cstring>

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <ament_index_cpp/get_package_prefix.hpp>
#include <builtin_interfaces/msg/time.h>
#include <rosidl_runtime_c/string.h>

#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QJSValue>
#include <QHash>
#include <QTextStream>

#include "MessageData.h"
#include "MessageMessageField.h"
#include "TypeSupport.h"

namespace {
  template<typename _T_>
  inline void do_initialize(_T_* _v)
  {
    Q_UNUSED(_v);
  }
  template<>
  inline void do_initialize<rosidl_runtime_c__String>(rosidl_runtime_c__String* _v)
  {
    _v->data      = nullptr;
    _v->size      = 0;
    _v->capacity  = 0;
  }
  template<typename _T_>
  inline void do_finalize(_T_* _v)
  {
    Q_UNUSED(_v);
  }
  template<>
  inline void do_finalize<rosidl_runtime_c__String>(rosidl_runtime_c__String* _v)
  {
    free(_v->data);
    _v->data      = nullptr;
    _v->size      = 0;
    _v->capacity  = 0;
  }
  
  template<typename _T_>
  inline QVariant element_read_value(const _T_* _v)
  {
    return QVariant::fromValue(*_v);
  }
  template<>
  inline QVariant element_read_value<rosidl_runtime_c__String>(const rosidl_runtime_c__String* _v)
  {
    return QVariant::fromValue(QString::fromUtf8(_v->data, _v->size));
  }
  template<>
  inline QVariant element_read_value<builtin_interfaces__msg__Time>(const builtin_interfaces__msg__Time* _v)
  {
    QVariantMap map;
    map["sec"] = _v->sec;
    map["nanosec"] = _v->nanosec;
    return map;
  }
  template<typename _T_>
  inline void element_write_value(_T_* _data, const QVariant& _v)
  {
    *_data = _v.value<_T_>();
  }
  template<>
  inline void element_write_value<rosidl_runtime_c__String>(rosidl_runtime_c__String* _data, const QVariant& _v)
  {
    do_finalize(_data);
    QByteArray utf8str = _v.value<QString>().toUtf8();
    std::size_t str_size = utf8str.size();
    std::size_t data_size = str_size + 1;
    _data->size = str_size;
    _data->capacity = data_size;
    _data->data = reinterpret_cast<char*>(malloc(data_size));
    std::memcpy(_data->data, utf8str.data(), data_size);
    _data->data[str_size] = 0;
  }
  template<>
  inline void element_write_value<builtin_interfaces__msg__Time>(builtin_interfaces__msg__Time* _data, const QVariant& _v)
  {
    QVariantMap v = _v.toMap();
    *_data = builtin_interfaces__msg__Time{v["sec"].value<int>(), v["nanosec"].value<uint32_t>()};
  }
}

template<typename _T_>
class BaseTypeMessageField : public MessageField
{
public:
  BaseTypeMessageField(const QString _name, Type _type, bool _array, std::size_t _index) : MessageField(_name, _type, _array, _index)
  {
  }
  void elementInitialize(quint8* _data) const override
  {
    do_initialize<_T_>(reinterpret_cast<_T_*>(_data));
  }
  void elementFinalize(quint8* _data) const override
  {
    do_finalize<_T_>(reinterpret_cast<_T_*>(_data));
  }
  QVariant elementReadValue(const quint8* _data) const override
  {
    return element_read_value<_T_>(reinterpret_cast<const _T_*>(_data));
  }
  void elementWriteValue(quint8* _data, const QVariant& _value) const override
  {
    element_write_value<_T_>(reinterpret_cast<_T_*>(_data), _value);
  }
  std::size_t elementSize() const override
  {
    return sizeof(_T_);
  }
};

MessageDefinition::MessageDefinition(QObject* _parent) : QObject(_parent)
{
}

MessageDefinition::MessageDefinition(const QString& _type_name) : m_type_name(_type_name)
{
  Q_ASSERT(not _type_name.endsWith("[]"));
  qDebug() << "MessageDefinition for " << m_type_name;
  if(m_type_name == "Header")
  {
    m_type_name = "std_msgs/Header";
  }

  QStringList splited = m_type_name.split('/');
  if(splited.size() != 2)
  {
    qWarning() << "Invalid type: " << _type_name;
    return;
  }
  const QString packagename = splited[0];
  const QString messagename = splited[1];
  try
  {
    QFile file(QString::fromStdString(ament_index_cpp::get_package_share_directory(packagename.toStdString())) + "/msg/" + messagename + ".msg");
    if(file.open(QIODevice::ReadOnly))
    {
      QTextStream stream(&file);
      parseDefinition(packagename, stream);
      m_typesupport = TypeSupport::getMessageTypeSupport(packagename, messagename);
      m_valid = m_valid and m_typesupport;
    } else {
      qWarning() << "Failed to open: " << file.fileName();
    }
  } catch(const ament_index_cpp::PackageNotFoundError& e)
  {
    qWarning() << "Cannot find package: " << packagename << " with error: " << e.what();
  }
}

void MessageDefinition::parseDefinition(const QString& _packagename, QTextStream& _stream)
{
  m_valid = true;
  int current_index = 0;
  while(not _stream.atEnd())
  {
    QString line = _stream.readLine();
    int comment_char = line.indexOf('#');
    QStringRef ref = (comment_char >= 0) ? line.leftRef(comment_char) : QStringRef(&line);
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    QVector<QStringRef>  l = ref.split(' ', QString::SkipEmptyParts);
#else
    QVector<QStringRef>  l = ref.split(' ', Qt::SkipEmptyParts);
#endif
    if(l.size() == 2)
    {
      QString type = l[0].toString();
      QString name = l[1].toString();
      bool is_array = false;
      QString baseType;
      if(type.endsWith("[]"))
      {
        is_array = true;
        baseType = type.left(type.length() - 2);
      } else if(type.endsWith(']'))
      {
        QRegExp r("(.*)\\[(.*)\\]");
        r.exactMatch(type);
        baseType = r.cap(1);
        is_array = true;
      } else {
        baseType = type;
      }
      if(baseType == "string")
      {
        m_fields.append(new BaseTypeMessageField<rosidl_runtime_c__String>(name, MessageField::Type::String, is_array, current_index));
      } else if(baseType == "float32")
      {
        m_fields.append(new BaseTypeMessageField<float>(name, MessageField::Type::Float32, is_array, current_index));
      } else if(baseType == "float64")
      {
        m_fields.append(new BaseTypeMessageField<double>(name, MessageField::Type::Float64, is_array, current_index));
      } else if(baseType == "uint8")
      {
        m_fields.append(new BaseTypeMessageField<quint8>(name, MessageField::Type::UInt8, is_array, current_index));
      } else if(baseType == "int8")
      {
        m_fields.append(new BaseTypeMessageField<qint8>(name, MessageField::Type::Int8, is_array, current_index));
      } else if(baseType == "uint16")
      {
        m_fields.append(new BaseTypeMessageField<quint16>(name, MessageField::Type::UInt16, is_array, current_index));
      } else if(baseType == "int16")
      {
        m_fields.append(new BaseTypeMessageField<qint16>(name, MessageField::Type::Int16, is_array, current_index));
      } else if(baseType == "uint32")
      {
        m_fields.append(new BaseTypeMessageField<quint32>(name, MessageField::Type::UInt32, is_array, current_index));
      } else if(baseType == "int32")
      {
        m_fields.append(new BaseTypeMessageField<qint32>(name, MessageField::Type::Int32, is_array, current_index));
      } else if(baseType == "uint64")
      {
        m_fields.append(new BaseTypeMessageField<uint64_t>(name, MessageField::Type::UInt64, is_array, current_index));
      } else if(baseType == "int64")
      {
        m_fields.append(new BaseTypeMessageField<int64_t>(name, MessageField::Type::Int64, is_array, current_index));
      } else if(baseType == "bool")
      {
        m_fields.append(new BaseTypeMessageField<bool>(name, MessageField::Type::Bool, is_array, current_index));
      } else if(baseType == "time")
      {
        m_fields.append(new BaseTypeMessageField<builtin_interfaces__msg__Time>(name, MessageField::Type::Time, is_array, current_index));
      } else {
        if(baseType == "Header")
        {
          baseType = "std_msgs/Header";
          type = "std_msgs/" + type;
        }
        qDebug() << baseType; 
        if(not baseType.contains("/"))
        {
          type = _packagename + "/" + type;
          baseType = _packagename + "/" + baseType;
        }
        MessageDefinition* md = MessageDefinition::get(baseType);
        qDebug() << md << type << md->isValid();
        if(md->isValid())
        {
          m_fields.append(new MessageMessageField(name, md, is_array, current_index));
        } else {
          qWarning() << "Unsupported field type: " << type << name;
          m_valid = false;
          return;
        }
      }
      MessageField* last_field = m_fields.last();
      current_index += last_field->fieldSize();
    } else if(l.size() == 4) {
      if(l[2] == "=")
      {
        // Constant, ignored for now
      } else {
        qWarning() << "Invalid line: " << line;
        m_valid = false;
          return;
      }
    } else if(l.size() != 0) {
      qWarning() << "Invalid line: " << line;
      m_valid = false;
      return;
    }
  }
}

MessageDefinition::~MessageDefinition()
{
}

MessageDefinition* MessageDefinition::get(const QString& _type_name)
{
  static QHash<QString, MessageDefinition*> definitions;
  MessageDefinition* md = definitions[_type_name];
  if(not md)
  {
    md = new MessageDefinition(_type_name);
    definitions[_type_name] = md;
    if(_type_name == "Header")
    {
      definitions["std_msgs/Header"] = md;
    } else if(_type_name == "std_msgs/Header")
    {
      definitions["Header"] = md;
    }
  }
  return md;
}

QVariantMap MessageDefinition::variantToMap(const QVariant& _variant) const
{
  if(_variant.canConvert<QJSValue>())
  {
    QJSValue value = _variant.value<QJSValue>();
    if(value.isArray())
    {
      QVariantList list;
      const int length = value.property("length").toInt();
      for(int i = 0; i < length; ++i)
      {
        list.append(value.property(i).toVariant());
      }
      return variantToMap(list);
    } else if(value.isObject()) {
      return value.toVariant().toMap();
    } else {
      QVariantList list;
      list.append(value.toVariant());
      return variantToMap(list);
    }
  } else if(_variant.canConvert<QVariantMap>())
  {
    return  _variant.toMap();
  } else if(_variant.canConvert<QVariantList>())
  {
    QVariantList list = _variant.toList();
    QVariantMap r;
    int i = 0;
    for(; i < std::min(list.size(), m_fields.size()); ++i)
    {
      const MessageField* mf = m_fields[i];
      QVariant value = list[i];
      if(mf->type() == MessageField::Type::Message)
      {
        const MessageMessageField* mmf = qobject_cast<const MessageMessageField*>(mf);
        r[mf->name()] = mmf->messageDefinition()->variantToMap(value);
      } else {
        r[mf->name()] = value;
      }
    }
    return r;
  } else if(m_fields.size() == 1) {
    QVariantMap r;
    const MessageField* mf = m_fields.first();
    if(mf->type() == MessageField::Type::Message)
    {
      const MessageMessageField* mmf = qobject_cast<const MessageMessageField*>(mf);
      r[mf->name()] = mmf->messageDefinition()->variantToMap(_variant);
    } else {
      r[mf->name()] = _variant;
    }
    return r;
  } else {
    qWarning() << "Invalid message: " << _variant;
    return QVariantMap();
  }
}

QVariantMap MessageDefinition::deserializeMessage(const MessageData& _buffer) const
{
  return deserializeMessage(_buffer.data());
}

QVariantMap MessageDefinition::deserializeMessage(const quint8* _buffer) const
{
  QVariantMap v;
  for(const MessageField* mf : m_fields)
  {
    v[mf->name()] = mf->fieldReadValue(_buffer + mf->index());
  }
  return v;
}

MessageData MessageDefinition::serializeMessage(const QVariantMap& _hash) const
{
  MessageData data(this);
  serializeMessage(_hash, data.data());
  return data;
}

void MessageDefinition::serializeMessage(const QVariantMap& _hash, quint8* _buffer) const
{
  for(const MessageField* mf : m_fields)
  {
    mf->fieldWriteValue(_buffer + mf->index(), _hash[mf->name()]);
  }
}

std::size_t MessageDefinition::serializedLength() const
{
  std::size_t length = 0;
  for(const MessageField* mf : m_fields)
  {
    length += mf->fieldSize();
  }
  return length;
}

quint8* MessageDefinition::allocateZeroInitialised() const
{
  quint8* data = reinterpret_cast<quint8*>(malloc(serializedLength()));
  for(const MessageField* mf : m_fields)
  {
    mf->fieldInitialize(data + mf->index());
  }
  return data;
}

void MessageDefinition::disallocate(quint8* data) const
{
  for(const MessageField* mf : m_fields)
  {
    mf->fieldFinalize(data + mf->index());
  }
  free(data);
}


QList<QObject*> MessageDefinition::fieldsLO() const
{
  return *reinterpret_cast<const QList<QObject*>*>(&m_fields);
}

#include "moc_MessageDefinition.cpp"
