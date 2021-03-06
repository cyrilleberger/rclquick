#include "RosWrapper.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QJSValue>
#include <QUuid>
#include <QVariant>

#include "RosThread.h"

RosWrapper::RosWrapper(QObject* _parent) : RosObject(_parent)
{
}

RosWrapper::~RosWrapper()
{
}

quint64 RosWrapper::startTime() const
{
  return RosThread::instance()->startTime();
}

quint64 RosWrapper::now() const
{
  return RosThread::instance()->now();
}

QByteArray RosWrapper::toByteArray(const QVariant& _list) const
{
  QVariantList l = _list.toList();
  QByteArray ba(l.size(), 0);
  for(int i = 0; i < l.size(); ++i)
  {
    ba[i] = l[i].value<char>();
  }
  return ba;
}

QString RosWrapper::toHex(const QByteArray& _array) const
{
  return QString::fromLatin1(_array.toHex());
}

bool RosWrapper::sameAs(const QVariant& _a, const QVariant& _b) const
{
  QVariant a = _a;
  QVariant b = _b;
  if(a.userType() == qMetaTypeId<QJSValue>())
  {
    a = a.value<QJSValue>().toVariant();
  }
  if(b.userType() == qMetaTypeId<QJSValue>())
  {
    b = b.value<QJSValue>().toVariant();
  }
  return a == b;
}

QString RosWrapper::toUuid(const QVariant& _list) const
{
  QByteArray arr = toByteArray(_list);
  QUuid uuid(arr);
  return uuid.toString();
}

QByteArray RosWrapper::sha3_512(const QVariant& _value) const
{
  QCryptographicHash hash(QCryptographicHash::Sha3_512);
  switch(_value.type())
  {
    case QVariant::ByteArray:
      hash.addData(_value.toByteArray());
      break;
    case QVariant::String:
      hash.addData(_value.toString().toUtf8());
      break;
    default:
      if(_value.canConvert<QByteArray>())
      {
        hash.addData(_value.toByteArray());
      } else if(_value.canConvert<QString>()) {
        hash.addData(_value.toString().toUtf8());
      } else {
        qWarning() << "Cannot compute sha3_512 for value:" << _value;
        return QByteArray();
      }
  }
  return hash.result();
}

#if 0

namespace
{
  QVariant xmlRpcToVariant(XmlRpc::XmlRpcValue _value)
  {
    switch(_value.getType())
    {
      default:
      case XmlRpc::XmlRpcValue::TypeInvalid:
      {
        return QVariant();
      }
      case XmlRpc::XmlRpcValue::TypeBoolean:
      {
        return QVariant((bool)_value);
      }
      case XmlRpc::XmlRpcValue::TypeInt:
      {
        return QVariant((int)_value);
      }
      case XmlRpc::XmlRpcValue::TypeDouble:
      {
        return QVariant((double)_value);
      }
      case XmlRpc::XmlRpcValue::TypeString:
      {
        return QVariant(QString::fromStdString(_value));
      }
      case XmlRpc::XmlRpcValue::TypeDateTime:
      {
        tm t = _value;
        return QVariant(QDateTime::fromTime_t(std::mktime(&t)));
      }
      case XmlRpc::XmlRpcValue::TypeBase64:
      {
        XmlRpc::XmlRpcValue::BinaryData bd = _value;
        return QByteArray(reinterpret_cast<const char*>(bd.data()), bd.size());
      }
      case XmlRpc::XmlRpcValue::TypeArray:
      {
        QVariantList var;
        for(int i = 0; i < _value.size(); ++i)
        {
          var.append(xmlRpcToVariant(_value[i]));
        }
        return var;
      }
      case XmlRpc::XmlRpcValue::TypeStruct:
      {
        QVariantMap map;
        for(XmlRpc::XmlRpcValue::iterator it = _value.begin(); it != _value.end(); ++it)
        {
          map[QString::fromStdString(it->first)] = xmlRpcToVariant(it->second);
        }
        return map;
      }
    }
  }
}

QVariant RosWrapper::getParam(const QString& _name) const
{
  if(_name.startsWith('~'))
  {
    ros::NodeHandle handle("~");
    std::string str;
    XmlRpc::XmlRpcValue value;
    if(handle.getParam(_name.right(_name.length() - 1).toStdString(), value))
    {
      return xmlRpcToVariant(value);
    } else {
      qWarning() << "No parameter called: " << _name;
      return QVariant();
    }
  } else {
    XmlRpc::XmlRpcValue value;
    if(m_handle.getParam(_name.toStdString(), value))
    {
      return xmlRpcToVariant(value);
    } else {
      qWarning() << "No parameter called: " << _name;
      return QVariant();
    }
  }
}

#endif

#include "moc_RosWrapper.cpp"
