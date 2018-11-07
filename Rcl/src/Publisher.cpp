#include "Publisher.h"

#include <QDebug>
#include <QVariant>

#include "MessageData.h"
#include "MessageDefinition.h"
#include "RosThread.h"

Publisher::Publisher(QObject* _parent) : RosObject(_parent), m_publisher(rcl_get_zero_initialized_publisher())
{
}

Publisher::~Publisher()
{
  if(rcl_publisher_fini(&m_publisher, RosThread::instance()->rclNode()) != RCL_RET_OK)
  {
    qWarning() << "Failed to finalize publisher: " << m_topic_name;
    rcutils_reset_error();
  }
}


void Publisher::setDataType(const QString& _topicName)
{
  m_data_type = _topicName;
  emit(dataTypeChanged());
  start_publisher();
}

void Publisher::setTopicName(const QString& _topicName)
{
  m_topic_name = _topicName;
  emit(topicNameChanged());
  start_publisher();
}

void Publisher::start_publisher()
{
  if(rcl_publisher_fini(&m_publisher, RosThread::instance()->rclNode()) != RCL_RET_OK)
  {
    qWarning() << "Failed to finalize publisher: " << m_topic_name;
    rcutils_reset_error();
  }
  if(not m_data_type.isEmpty() and not m_topic_name.isEmpty())
  {
    m_message_definition = MessageDefinition::get(m_data_type);
    emit(messageDefinitionChanged());
    rcl_publisher_options_t publisher_ops = rcl_publisher_get_default_options(); // TODO support for QoS
    if(rcl_publisher_init(&m_publisher, RosThread::instance()->rclNode(), m_message_definition->typeSupport(), qPrintable(m_topic_name), &publisher_ops) != RCL_RET_OK)
    {
      qWarning() << "Failed to initialize publisher: " << m_topic_name;
      rcutils_reset_error();
    }
  }
}

void Publisher::publish(const QVariant& _message)
{
  if(m_message_definition)
  {
    QVariantMap message = m_message_definition->variantToMap(_message);
    MessageData message_data = m_message_definition->serializeMessage(message);
    
    if(rcl_publish(&m_publisher, message_data.data()) != RCL_RET_OK)
    {
      qWarning() << "Failed to publish: " << m_topic_name;
      rcutils_reset_error();
    }
  }
}
