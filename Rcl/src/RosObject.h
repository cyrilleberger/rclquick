#ifndef ROSOBJECT_H_
#define ROSOBJECT_H_

#include <QObject>

class RosObject : public QObject
{
public:
  RosObject(QObject* _parent = nullptr);
  ~RosObject();
protected:
};

#endif
