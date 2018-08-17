#include "Subscriber.h"

#include <QDebug>

#include "MessageDefinition.h"
#include "RosThread.h"

Subscriber::Subscriber(QObject* _parent) : RosObject(_parent), m_subscription(rcl_get_zero_initialized_subscription()), m_skip(0), m_skipCount(0)
{
}

Subscriber::~Subscriber()
{
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

#if 0
void Subscriber::callback(ros::MessageEvent<const topic_tools::ShapeShifter> _message)
{
  if(m_skipCount < m_skip)
  {
    ++m_skipCount;
    return;
  }
  m_skipCount = 0;
  MessageDefinition* md = MessageDefinition::get(QString::fromStdString(_message.getMessage()->getDataType()));
  if(md != m_message_definition)
  {
    m_message_definition = md;
    emit(messageDefinitionChanged());
  }
  
  QByteArray arr;
  arr.resize(_message.getMessage()->size());
  
  ros::serialization::OStream stream(reinterpret_cast<uint8_t*>(arr.data()), arr.size());
  _message.getMessage()->write(stream);
  
  QVariantMap h = md->deserializeMessage(arr);
  m_lastMessage = h;
  emit(messageReceived(h, _message.getReceiptTime().toNSec(), QString::fromStdString(_message.getPublisherName())));
}
#endif

void Subscriber::subscribe()
{
  if(rcl_subscription_fini(&m_subscription, RosThread::instance()->rclNode()) != RCL_RET_OK)
  {
    qWarning() << "Failed to finalize subscription: " << m_topic_name;
  }

  if(not m_data_type.isEmpty() and not m_topic_name.isEmpty())
  {
    m_message_definition = MessageDefinition::get(m_data_type);
    emit(messageDefinitionChanged());
    rcl_subscription_options_t subscription_ops = rcl_subscription_get_default_options();
    if(rcl_subscription_init(&m_subscription, RosThread::instance()->rclNode(), m_message_definition->typeSupport(), qPrintable(m_topic_name), &subscription_ops) != RCL_RET_OK)
    {
      qWarning() << "Failed to initialize publisher: " << m_topic_name;
    }

  }
}

