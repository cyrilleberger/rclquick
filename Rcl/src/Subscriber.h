#pragma once

#include "RosObject.h"

#include <QMutex>
#include <QVariantMap>

#include <rcl/subscription.h>

class MessageDefinition;
class RosThread;

class Subscriber : public RosObject
{
  friend class RosThread;
  Q_OBJECT
  Q_PROPERTY(QString dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)
  Q_PROPERTY(QString topicName READ topicName WRITE setTopicName NOTIFY topicNameChanged)
  Q_PROPERTY(int skip READ skip WRITE setSkip NOTIFY skipChanged)
  Q_PROPERTY(QVariant lastMessage READ lastMessage NOTIFY messageReceived)
  Q_PROPERTY(
    MessageDefinition* messageDefinition READ messageDefinition NOTIFY messageDefinitionChanged)
public:
  Subscriber(QObject* _parent = nullptr);
  ~Subscriber();
  QVariant lastMessage() const { return m_lastMessage; }
  QString topicName() const { return m_topic_name; }
  void setTopicName(const QString& _topicName);
  int skip() const { return m_skip; }
  void setSkip(int skip)
  {
    m_skip = skip;
    m_skipCount = m_skip;
    emit(skipChanged());
  }
  QString dataType() const { return m_data_type; }
  void setDataType(const QString& _topicName);
  MessageDefinition* messageDefinition() const { return m_message_definition; }
private:
  void subscribe();
  void tryHandleMessage();
signals:
  void topicNameChanged();
  void dataTypeChanged();
  void skipChanged();
  void messageReceived(const QVariant& message, quint64 timestamp, const QString& publisher);
  void messageDefinitionChanged();
private:
  QMutex m_mutex;
  rcl_subscription_t m_subscription;
  QString m_topic_name, m_data_type;
  int m_skip, m_skipCount;
  QVariantMap m_lastMessage;
  MessageDefinition* m_message_definition = nullptr;
};
