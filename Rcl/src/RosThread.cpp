#include "RosThread.h"

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rcl/time.h>

#include <QCoreApplication>
#include <QDebug>
#include <QProcessEnvironment>

#include "ServiceClient.h"
#include "Subscriber.h"



RosThread::RosThread() : m_rcl_node(rcl_get_zero_initialized_node()), m_wake_up_loop(rcl_get_zero_initialized_guard_condition())
{
  
}

RosThread* RosThread::instance()
{
  static RosThread* rt = nullptr;
  static QMutex mutex;
  static rcl_init_options_t rcl_init_options;
  
  QMutexLocker l(&mutex);
  if(not rt)
  {
    rt = new RosThread();
    
    QStringList ros_arguments = QProcessEnvironment::systemEnvironment().value("ROS_ARGUMENTS").split(' ');
    QList<QByteArray> ros_argv_buffers;
    char** ros_argv = new char*[ros_arguments.size()];
    
    for(int i = 0; i < ros_arguments.size(); ++i)
    {
      QByteArray buffer = ros_arguments[i].toUtf8();
      ros_argv[i] = buffer.data();
      ros_argv_buffers.append(buffer);
    }
    
    rt->m_rcl_context = rcl_get_zero_initialized_context();
    rcl_init_options = rcl_get_zero_initialized_init_options();
    if(rcl_init_options_init(&rcl_init_options, rcl_get_default_allocator()) != RCL_RET_OK)
    {
      qFatal("Failed to initialize initialise options: %s", rcl_get_error_string().str);
    }
    
    if(rcl_init(ros_arguments.size(), ros_argv, &rcl_init_options, &rt->m_rcl_context) != RCL_RET_OK)
    {
      qFatal("Failed to initialize rmw implementation: %s", rcl_get_error_string().str);
    }
    
    delete[] ros_argv;
    
    QString ros_name = QProcessEnvironment::systemEnvironment().value("ROS_NAME");
    if(ros_name.isEmpty())
    {
      ros_name = QString("qmlapp_%0").arg(QCoreApplication::applicationPid());
    }

    QString ros_namespace = QProcessEnvironment::systemEnvironment().value("ROS_NAMESPACE");

    rcl_node_options_t node_options = rcl_node_get_default_options();
    if(rcl_node_init(&rt->m_rcl_node, qPrintable(ros_name), qPrintable(ros_namespace), &rt->m_rcl_context, &node_options))
    {
      qFatal("Failed to initialize node: %s", rcl_get_error_string().str);
    }
    
  }
  return rt;
}

void RosThread::registerClient(ServiceClient* _client)
{
  Q_ASSERT(_client);
  QMutexLocker l(&m_mutex);
  m_clients.append(_client);
  wakeUpLoop();
}

void RosThread::unregisterClient(ServiceClient* _client)
{
  Q_ASSERT(_client);
  QMutexLocker l(&m_mutex);
  rcl_client_t client = _client->m_client;
  m_actions.insert(nullptr, [client, this]() mutable { 
    if(rcl_client_fini(&client, &m_rcl_node) != RCL_RET_OK)
    {
      qWarning() << "Failed to finalize client!" << rcl_get_error_string().str;
      rcl_reset_error();
    }
  });
  m_clients.removeAll(_client);
  m_actions.remove(_client);
  wakeUpLoop();
}

void RosThread::registerSubscriber(Subscriber* _subscriber)
{
  Q_ASSERT(_subscriber);
  QMutexLocker l(&m_mutex);
  m_subscribers.append(_subscriber);
  wakeUpLoop();
}

void RosThread::unregisterSubscriber(Subscriber* _subscriber)
{
  Q_ASSERT(_subscriber);
  QMutexLocker l(&m_mutex);
  rcl_subscription_t subscribtion = _subscriber->m_subscription;
  m_actions.insert(nullptr, [subscribtion, this]() mutable { 
    if(rcl_subscription_fini(&subscribtion, &m_rcl_node) != RCL_RET_OK)
    {
      qWarning() << "Failed to finalize subscription!" << rcl_get_error_string().str;
      rcl_reset_error();
    }
  });
  m_subscribers.removeAll(_subscriber);
  m_actions.remove(_subscriber);
  wakeUpLoop();
}

void RosThread::registerAction(RosObject* _object, const std::function<void()>& _action)
{
  Q_ASSERT(_object);
  QMutexLocker l(&m_mutex);
  m_actions.insert(_object, _action);
  wakeUpLoop();
}

void RosThread::run()
{
  m_running = true;
  m_startTime = now();
  if(rcl_guard_condition_init(&m_wake_up_loop, &m_rcl_context, rcl_guard_condition_get_default_options()) != RCL_RET_OK)
  {
    qFatal("Failed to initialize wake up loop");
  }
  connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, [this]()
  {
    m_running = false;
    wakeUpLoop();
  });
  while(m_running)
  {
    {
      QMutexLocker l(&m_mutex);
      for(const std::function<void()>& act : m_actions)
      {
        act();
      }
      m_actions.clear();
    }
    {
      QMutexLocker l(&m_mutex);
      // Handle subscription
      for(Subscriber* sub : m_subscribers)
      {
        sub->tryHandleMessage();
      }
    }
    {
      QMutexLocker l(&m_mutex);
      // Handle client
      for(ServiceClient* cl : m_clients)
      {
        cl->tryHandleAnswer();
      }
    }
    
    rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
    {
      QMutexLocker l(&m_mutex);
      if(rcl_wait_set_init(&wait_set, m_subscribers.size(), 1, 0, m_clients.size(), 0, 0, &m_rcl_context, rcl_get_default_allocator()) != RCL_RET_OK)
      {
        qFatal("Failed to initialize wait_set");
      }
      
      if(rcl_wait_set_add_guard_condition(&wait_set, &m_wake_up_loop, NULL) != RCL_RET_OK)
      {
        qFatal("Error when adding guard condition to wait_set %s", rcl_get_error_string().str);
      }
      
      for(int i = 0; i < m_subscribers.size(); ++i)
      {
        QMutexLocker l2(&m_subscribers[i]->m_mutex);
        if(rcl_subscription_is_valid(&m_subscribers[i]->m_subscription))
        {
          if(rcl_wait_set_add_subscription(&wait_set, &m_subscribers[i]->m_subscription, NULL) != RCL_RET_OK)
          {
            qFatal("Error when adding subscription to wait_set %s", rcl_get_error_string().str);
          }
        } else {
          rcutils_reset_error();
        }
      }
      for(int i = 0; i < m_clients.size(); ++i)
      {
        QMutexLocker l2(&m_clients[i]->m_mutex);
        if(rcl_client_is_valid(&m_clients[i]->m_client))
        {
          if(rcl_wait_set_add_client(&wait_set, &m_clients[i]->m_client, NULL) != RCL_RET_OK)
          {
            qFatal("Error when adding client to wait_set %s", rcl_get_error_string().str);
          }
        } else {
          rcutils_reset_error();
        }
      }
    }
    rcl_ret_t ret_wait = rcl_wait(&wait_set, -1);
    if(ret_wait == RCL_RET_ERROR or ret_wait == RCL_RET_INVALID_ARGUMENT)
    {
      qFatal("Failed to wait: %s", rcl_get_error_string().str);
    }
    rcl_reset_error();
    if(rcl_wait_set_fini(&wait_set) != RCL_RET_OK)
    {
      qFatal("Failed to finalize wait_set %s", rcl_get_error_string().str);
    }
  }
  if(rcl_node_fini(&m_rcl_node) != RCL_RET_OK)
  {
    qFatal("Failed to finalize node: %s", rcl_get_error_string().str);
  }
  if(rcl_shutdown(&m_rcl_context) != RCL_RET_OK)
  {
    qFatal("Failed to shutdown: %s", rcl_get_error_string().str);
  }
  
  qDebug() << "done execution";
}


void RosThread::wakeUpLoop()
{
  if(rcl_trigger_guard_condition(&m_wake_up_loop) != RCL_RET_OK)
  {
    qWarning() << "Failed to wake up loop: " << rcl_get_error_string().str;
    rcl_reset_error();
  }
}

quint64 RosThread::now() const
{
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_clock_t clock;
  if(rcl_clock_init(RCL_SYSTEM_TIME, &clock, &allocator) != RCL_RET_OK)
  {
    qFatal("Failed to initialize time point.");
  }
  rcl_time_point_value_t ns;
  if(rcl_clock_get_now(&clock, &ns) != RCL_RET_OK)
  {
    qFatal("Failed to get current time.");
  }
  if(rcl_clock_fini(&clock) != RCL_RET_OK)
  {
    qFatal("Failed to finalize clock.");
  }
  return ns;
}
