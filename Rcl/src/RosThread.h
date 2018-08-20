#ifndef ROSTHREAD_H
#define ROSTHREAD_H

#include <QMutex>
#include <QThread>
#include <QThreadPool>

#include <rcl/client.h>
#include <rcl/node.h>
#include <rcl/subscription.h>

class Subscriber;
class ServiceClient;

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
  void finalize(rcl_subscription_t _subscription);
  void finalize(rcl_client_t _client);
protected:
  void run();
private:
  rcl_node_t m_rcl_node;
  quint64 m_startTime;
  QThreadPool m_threadPool;
  QMutex m_mutex;
  QList<Subscriber*>    m_subscribers;
  QList<ServiceClient*> m_clients;
  QMutex m_mutex_finalize;
  QList<rcl_subscription_t> m_subscriptionsToFinalize;
  QList<rcl_client_t> m_clientsToFinalize;
};

#endif // ROSTHREAD_H
