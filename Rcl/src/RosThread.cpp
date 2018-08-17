#include "RosThread.h"

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rcl/time.h>

#include <QCoreApplication>
#include <QDebug>
#include <QProcessEnvironment>

RosThread::RosThread() : m_rcl_node(rcl_get_zero_initialized_node())
{
  
}

RosThread* RosThread::instance()
{
  static RosThread* rt = nullptr;
  if(not rt)
  {
    
    QStringList ros_arguments = QProcessEnvironment::systemEnvironment().value("ROS_ARGUMENTS").split(' ');
    QList<QByteArray> ros_argv_buffers;
    char** ros_argv = new char*[ros_arguments.size()];
    
    for(int i = 0; i < ros_arguments.size(); ++i)
    {
      QByteArray buffer = ros_arguments[i].toUtf8();
      ros_argv[i] = buffer.data();
      ros_argv_buffers.append(buffer);
    }
    
    delete[] ros_argv;
    
    if(rcl_init(ros_arguments.size(), ros_argv, rcl_get_default_allocator()) != RCL_RET_OK)
    {
      qFatal("Failed to initialize rmw implementation: %s", rcl_get_error_string_safe());
    }
    
    QString ros_name = QProcessEnvironment::systemEnvironment().value("ROS_NAME");
    if(ros_name.isEmpty())
    {
      ros_name = QString("qmlapp_%i").arg(QCoreApplication::applicationPid());
    }

    QString ros_namespace = QProcessEnvironment::systemEnvironment().value("ROS_NAMESPACE");

    rt = new RosThread();
    rcl_node_options_t node_options = rcl_node_get_default_options();
    if(rcl_node_init(&rt->m_rcl_node, qPrintable(ros_name), qPrintable(ros_namespace), &node_options))
    {
      qFatal("Failed to initialize node: %s", rcl_get_error_string_safe());
    }
    
  }
  return rt;
}

void RosThread::registerClient(ServiceClient* _client)
{
  QMutexLocker l(&m_mutex);
  m_clients.append(_client);
}

void RosThread::unregisterClient(ServiceClient* _client)
{
  QMutexLocker l(&m_mutex);
  m_clients.removeAll(_client);
}

void RosThread::registerSubscriber(Subscriber* _subscriber)
{
  QMutexLocker l(&m_mutex);
  m_subscribers.append(_subscriber);
}

void RosThread::unregisterSubscriber(Subscriber* _subscriber)
{
  QMutexLocker l(&m_mutex);
  m_subscribers.removeAll(_subscriber);
}


void RosThread::run()
{
  m_startTime = now();
  while(true)
  {
    {
      QMutexLocker l(&m_mutex);
      // Handle subscription
      for(Subscriber* sub : m_subscribers)
      {
        qFatal("unimplemented");
        Q_UNUSED(sub);
      }
    }
    {
      QMutexLocker l(&m_mutex);
      // Handle client
      for(ServiceClient* cl : m_clients)
      {
        qFatal("unimplemented");
        Q_UNUSED(cl);
      }
    }
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
  rcl_time_point_t ns;
  if(rcl_clock_get_now(&clock, &ns) != RCL_RET_OK)
  {
    qFatal("Failed to get current time.");
  }
  if(rcl_clock_fini(&clock) != RCL_RET_OK)
  {
    qFatal("Failed to finalize clock.");
  }
  return ns.nanoseconds;
}
