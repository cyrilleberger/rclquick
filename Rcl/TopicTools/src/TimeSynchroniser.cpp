#include "TimeSynchroniser.h"

TimeSynchroniser::TimeSynchroniser(QObject* _parent) : QObject(_parent) {}

TimeSynchroniser::~TimeSynchroniser() {}

void TimeSynchroniser::appendSubscriber(QQmlListProperty<QObject>* _property, QObject* _subscriber)
{
  TimeSynchroniser* self = qobject_cast<TimeSynchroniser*>(_property->object);
  QMutexLocker l(&self->m_mutex);
  _subscriber->setParent(self);
  connect(_subscriber, SIGNAL(messageReceived(const QVariant&, quint64, const QString&)), self,
          SLOT(handleMessage(const QVariant&)));
  self->m_subscribers.append(_subscriber);
}

qsizetype TimeSynchroniser::subscriberCount(QQmlListProperty<QObject>* _property)
{
  TimeSynchroniser* self = qobject_cast<TimeSynchroniser*>(_property->object);
  QMutexLocker l(&self->m_mutex);
  return self->m_subscribers.size();
}

QObject* TimeSynchroniser::subscriber(QQmlListProperty<QObject>* _property, qsizetype _index)
{
  TimeSynchroniser* self = qobject_cast<TimeSynchroniser*>(_property->object);
  QMutexLocker l(&self->m_mutex);
  return self->m_subscribers.at(_index);
}

void TimeSynchroniser::clearSubscribers(QQmlListProperty<QObject>* _property)
{
  TimeSynchroniser* self = qobject_cast<TimeSynchroniser*>(_property->object);
  QMutexLocker l(&self->m_mutex);
  for(QObject* s : self->m_subscribers)
  {
    s->setParent(nullptr);
    disconnect(s, SIGNAL(messageReceived(const QVariant&, quint64, const QString&)), self,
               SLOT(handleMessage(const QVariant&)));
  }
  self->m_subscribers.clear();
}

QQmlListProperty<QObject> TimeSynchroniser::subscribers()
{
  return QQmlListProperty<QObject>(
    this, this, &TimeSynchroniser::appendSubscriber, &TimeSynchroniser::subscriberCount,
    &TimeSynchroniser::subscriber, &TimeSynchroniser::clearSubscribers);
}

struct ros_time
{
  ros_time() : sec(0), nanosec(0) {}
  ros_time(const QVariant& _var)
  {
    QVariantMap map = _var.toMap();
    sec = map["sec"].value<qint32>();
    nanosec = map["nanosec"].value<quint32>();
  }
  qint32 sec;
  quint32 nanosec;
  bool operator<(const ros_time& _rhs)
  {
    return sec < _rhs.sec or (sec == _rhs.sec and nanosec < _rhs.nanosec);
  }
  bool operator!=(const ros_time& _rhs) { return sec != _rhs.sec or nanosec != _rhs.nanosec; }
  bool operator==(const ros_time& _rhs) { return sec == _rhs.sec and nanosec == _rhs.nanosec; }
  static ros_time fromMsg(const QVariant& _message) { return ros_time(_message.toMap()["header"]); }
  static ros_time infinite()
  {
    ros_time rt;
    rt.sec = std::numeric_limits<qint32>::max();
    rt.nanosec = std::numeric_limits<quint32>::max();
    return rt;
  }
};

void TimeSynchroniser::handleMessage(const QVariant& _message)
{
  QMutexLocker l(&m_mutex);
  QObject* sender_ptr = sender();
  m_messages[sender_ptr].append(_message);

  while(true)
  {
    ros_time earliest_time = ros_time::infinite();
    for(QObject* s : m_subscribers)
    {
      if(m_messages[s].empty())
        return; // <- no message received yet
      ros_time ct = ros_time::fromMsg(m_messages[s].first());
      if(ct < earliest_time)
        earliest_time = ct;
    }
    bool all_earliest = true;
    for(QObject* s : m_subscribers)
    {
      ros_time ct = ros_time::fromMsg(m_messages[s].first());
      if(ct != earliest_time)
      {
        all_earliest = false;
      }
    }
    if(all_earliest)
    {
      QVariantList messages;
      for(QObject* s : m_subscribers)
      {
        messages.append(m_messages[s].takeFirst());
      }
      emit(messagesReceived(messages));
    }
    else
    {
      for(QObject* s : m_subscribers)
      {
        ros_time ct = ros_time::fromMsg(m_messages[s].first());
        if(ct == earliest_time)
        {
          m_messages[s].removeFirst();
        }
      }
    }
  }
}

#include "moc_TimeSynchroniser.cpp"
