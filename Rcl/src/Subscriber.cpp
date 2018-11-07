#include "Subscriber.h"

#include <rcl/error_handling.h>

#include <QDebug>

#include "MessageData.h"
#include "MessageDefinition.h"
#include "RosThread.h"

Subscriber::Subscriber(QObject* _parent) : RosObject(_parent), m_subscription(rcl_get_zero_initialized_subscription()), m_skip(0), m_skipCount(0)
{
  RosThread::instance()->registerSubscriber(this);
}

Subscriber::~Subscriber()
{
  QMutexLocker l(&m_mutex);
  RosThread::instance()->unregisterSubscriber(this);
  RosThread::instance()->finalize(m_subscription);
}

void Subscriber::setTopicName(const QString& _topicName)
{
  m_topic_name = _topicName;
  emit(topicNameChanged());
  subscribe();
}

void Subscriber::setDataType(const QString& _topicName)
{
  m_data_type = _topicName;
  emit(dataTypeChanged());
  subscribe();
}

void Subscriber::tryHandleMessage()
{
  QMutexLocker l(&m_mutex);
  
  if(m_message_definition)
  {
    MessageData data(m_message_definition);

    rmw_message_info_t message_info;
    message_info.from_intra_process = false;
    
    
    rcl_ret_t status = rcl_take(&m_subscription, data.data(), &message_info);

    switch(status)
    {
      case RCL_RET_OK:
      {
        if(m_skipCount < m_skip)
        {
          ++m_skipCount;
        } else {
          m_skipCount = 0;
          
          m_lastMessage = m_message_definition->deserializeMessage(data);
          emit(messageReceived(m_lastMessage, RosThread::instance()->now(), QByteArray((const char*)message_info.publisher_gid.data).toHex()));
        }
        break;
      }
      case RCL_RET_SUBSCRIPTION_TAKE_FAILED:
        // I am guessing answer is not available yet we get this error message
        break;
      default:
        qWarning() << "Failed to get subscribtion message" << m_topic_name << rcl_get_error_string_safe();
        rcl_reset_error();
        break;
    }
  }
}


void Subscriber::subscribe()
{
  QMutexLocker l(&m_mutex);
  RosThread::instance()->finalize(m_subscription);
  m_subscription = rcl_get_zero_initialized_subscription();

  qDebug() << m_message_definition << m_data_type << m_topic_name;
  if(not m_data_type.isEmpty() and not m_topic_name.isEmpty())
  {
    m_message_definition = MessageDefinition::get(m_data_type);
  qDebug() << m_message_definition << m_data_type;
    emit(messageDefinitionChanged());
    rcl_subscription_options_t subscription_ops = rcl_subscription_get_default_options();
    if(rcl_subscription_init(&m_subscription, RosThread::instance()->rclNode(), m_message_definition->typeSupport(), qPrintable(m_topic_name), &subscription_ops) != RCL_RET_OK)
    {
      qWarning() << "Failed to initialize subscriber: " << m_topic_name;
      rcutils_reset_error();
    }

  }
}

