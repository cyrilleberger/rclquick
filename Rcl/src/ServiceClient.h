#pragma once

#include "RosObject.h"

#include <QMutex>

#include <rcl/client.h>

class ServiceDefinition;
class RosThread;

class ServiceClient : public RosObject
{
  friend class RosThread;
  Q_OBJECT
  Q_PROPERTY(bool shouldWaitForAvailable READ shouldWaitForAvailable WRITE setShouldWaitForAvailable
               NOTIFY shouldWaitForAvailableChanged)
  Q_PROPERTY(bool callInProgress READ callInProgress NOTIFY callInProgressChanged)
  Q_PROPERTY(QString dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)
  Q_PROPERTY(QString serviceName READ serviceName WRITE setServiceName NOTIFY serviceNameChanged)
  Q_PROPERTY(
    ServiceDefinition* serviceDefinition READ serviceDefinition NOTIFY serviceDefinitionChanged)
public:
  ServiceClient(QObject* _parent = nullptr);
  ~ServiceClient();
  QString serviceName() const { return m_service_name; }
  void setServiceName(const QString& _serviceName);
  QString dataType() const { return m_data_type; }
  void setDataType(const QString& _serviceName);
  bool callInProgress() const { return m_called; }
  bool shouldWaitForAvailable() const { return m_shouldWaitForAvailable; }
  void setShouldWaitForAvailable(bool _v);
  Q_INVOKABLE bool call(const QVariant& _message);
  ServiceDefinition* serviceDefinition() const { return m_service_definition; }
private:
  void tryHandleAnswer();
  void start_client();
  bool isAvailable() const;
signals:
  void dataTypeChanged();
  void serviceNameChanged();
  void serviceDefinitionChanged();
  void callInProgressChanged();
  void shouldWaitForAvailableChanged();
  void answerReceived(const QVariant& answer);
  void callFailed();
private:
  QMutex m_mutex;
  QString m_service_name, m_data_type;
  rcl_client_t m_client;
  ServiceDefinition* m_service_definition = nullptr;
  bool m_called = false;
  int64_t m_sequence_number;
  bool m_shouldWaitForAvailable = false;
};
