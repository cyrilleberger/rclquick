#ifndef ROSTHREAD_H
#define ROSTHREAD_H

#include <functional>

#include <QMutex>
#include <QMultiHash>
#include <QThread>
#include <QThreadPool>

#include <rcl/client.h>
#include <rcl/guard_condition.h>
#include <rcl/node.h>
#include <rcl/subscription.h>

class Subscriber;
class ServiceClient;
class RosObject;

class RosThread : public QThread
{
  RosThread();
public:
  static RosThread* instance();
  quint64 now() const;
  quint64 startTime() const { return m_startTime; }
  rcl_node_t* rclNode() { return &m_rcl_node; }
  void registerSubscriber(Subscriber* _subscriber);
  void unregisterSubscriber(Subscriber* _subscriber);
  void registerClient(ServiceClient* _client);
  void unregisterClient(ServiceClient* _client);
  void registerAction(RosObject* _object, const std::function<void()>& _action);
  template<typename _T_>
  void registerAction(_T_* _object, void (_T_::*member)())
  {
    registerAction(_object, std::bind(member, _object));
  }
protected:
  void run();
  void wakeUpLoop();
private:
  rcl_node_t m_rcl_node;
  quint64 m_startTime;
  QThreadPool m_threadPool;
  QMutex m_mutex;
  QMultiHash<RosObject*, std::function<void()>> m_actions;
  QList<Subscriber*>    m_subscribers;
  QList<ServiceClient*> m_clients;
  rcl_guard_condition_t m_wake_up_loop;
  bool m_running;
};

#endif // ROSTHREAD_H
