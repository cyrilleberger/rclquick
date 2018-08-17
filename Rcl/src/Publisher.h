#include "RosObject.h"

#include <rcl/publisher.h>

class MessageDefinition;

class Publisher : public RosObject
{
  Q_OBJECT
  Q_PROPERTY(QString dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)
  Q_PROPERTY(QString topicName READ topicName WRITE setTopicName NOTIFY topicNameChanged)
  Q_PROPERTY(MessageDefinition* messageDefinition READ messageDefinition NOTIFY messageDefinitionChanged)
public:
  Publisher(QObject* _parent = nullptr);
  ~Publisher();
  QString topicName() const { return m_topic_name; }
  void setTopicName(const QString& _topicName);
  QString dataType() const { return m_data_type; }
  void setDataType(const QString& _topicName);
  Q_INVOKABLE void publish(const QVariant& _message);
  MessageDefinition* messageDefinition() const { return m_message_definition; }
signals:
  void dataTypeChanged();
  void topicNameChanged();
  void messageDefinitionChanged();
private:
  void start_publisher();
  QString m_topic_name, m_data_type;
  rcl_publisher_t m_publisher;
  MessageDefinition* m_message_definition = nullptr;
};
