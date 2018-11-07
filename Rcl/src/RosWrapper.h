#ifndef ROSWRAPPER_H_
#define ROSWRAPPER_H_

#include "RosObject.h"

class RosWrapper : public RosObject
{
  Q_OBJECT
  Q_PROPERTY(quint64 startTime READ startTime CONSTANT)
public:
  RosWrapper(QObject* _parent = nullptr);
  ~RosWrapper();
  quint64 startTime() const;
  Q_INVOKABLE quint64 now() const;
  Q_INVOKABLE QByteArray toByteArray(const QVariant& _list) const;
  Q_INVOKABLE QString toHex(const QByteArray& _array) const;
  Q_INVOKABLE bool sameAs(const QVariant& _a, const QVariant& _b) const;
  Q_INVOKABLE QString toUuid(const QVariant& _list) const;
//   Q_INVOKABLE QVariant getParam(const QString& _name) const;
protected:
};

#endif

