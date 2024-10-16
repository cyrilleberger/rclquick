#include "ServiceClient.h"

#include <rcl/error_handling.h>

#include <QtConcurrent/QtConcurrent>

#include <rcl/graph.h>

#include "MessageData.h"
#include "MessageDefinition.h"
#include "RosThread.h"
#include "ServiceDefinition.h"

ServiceClient::ServiceClient(QObject* _parent)
    : RosObject(_parent), m_client(rcl_get_zero_initialized_client())
{
  RosThread::instance()->registerClient(this);
}

ServiceClient::~ServiceClient()
{
  QMutexLocker l(&m_mutex);
  RosThread::instance()->unregisterClient(this);
}

void ServiceClient::setDataType(const QString& _dataType)
{
  m_data_type = _dataType;
  emit(dataTypeChanged());
  RosThread::instance()->registerAction(this, &ServiceClient::start_client);
}

void ServiceClient::setServiceName(const QString& _serviceName)
{
  m_service_name = _serviceName;
  emit(serviceNameChanged());
  RosThread::instance()->registerAction(this, &ServiceClient::start_client);
}

void ServiceClient::setShouldWaitForAvailable(bool _v)
{
  m_shouldWaitForAvailable = _v;
  emit(shouldWaitForAvailable());
}

bool ServiceClient::isAvailable() const
{
  bool is_ready;
  rcl_ret_t ret
    = rcl_service_server_is_available(RosThread::instance()->rclNode(), &m_client, &is_ready);

  if(ret != RCL_RET_OK)
  {
    qWarning() << "Failed to check for service server availability: " << rcl_get_error_string().str;
    rcl_reset_error();
    return false;
  }
  return is_ready;
}

bool ServiceClient::call(const QVariant& _message)
{
  QMutexLocker l(&m_mutex);
  if(not m_service_definition)
  {
    qWarning() << "Service definition not set or not found";
    return false;
  }
  if(m_called)
  {
    qWarning() << "Service call in progress";
    return false;
  }
  if(not m_client.impl)
  {
    qWarning() << "Client was not created for " << m_service_name << " of type " << m_data_type;
    return false;
  }
  m_called = true;
  emit(callInProgressChanged());

  if(not isAvailable())
  {
    qWarning() << "Service '" << m_service_name << "' not available...";
    return false;
  }

  QVariantMap message = m_service_definition->requestDefinition()->variantToMap(_message);

  MessageData message_data = m_service_definition->requestDefinition()->serializeMessage(message);

  if(rcl_send_request(&m_client, message_data.data(), &m_sequence_number) != RCL_RET_OK)
  {
    qWarning() << "Failed to send request on service: " << m_service_name
               << rcl_get_error_string().str;
    rcl_reset_error();
    emit(callFailed());
    m_called = false;
    emit(callInProgressChanged());
    return false;
  }
  else
  {
    return true;
  }
}

void ServiceClient::tryHandleAnswer()
{
  QMutexLocker l(&m_mutex);
  rmw_request_id_t request_header;
  request_header.sequence_number = m_sequence_number;

  if(m_service_definition)
  {

    MessageData answerData(m_service_definition->answerDefinition());

    rcl_ret_t status = rcl_take_response(&m_client, &request_header, answerData.data());

    switch(status)
    {
    case RCL_RET_OK:
    {
      QVariantMap h = m_service_definition->answerDefinition()->deserializeMessage(answerData);
      emit(answerReceived(h));
      m_called = false;
      emit(callInProgressChanged());
      break;
    }
    case RCL_RET_CLIENT_TAKE_FAILED:
      // I am guessing answer is not available yet we get this error message
      break;
    default:
      emit(callFailed());
      qWarning() << "Failed to get answer service: " << m_service_name
                 << rcl_get_error_string().str;
      rcl_reset_error();
      m_called = false;
      emit(callInProgressChanged());
      break;
    }
  }
}

void ServiceClient::start_client()
{
  QMutexLocker l(&m_mutex);
  if(rcl_client_fini(&m_client, RosThread::instance()->rclNode()) != RCL_RET_OK)
  {
    qWarning() << "Failed to finalize client!" << rcl_get_error_string().str;
    rcl_reset_error();
  }
  m_client = rcl_get_zero_initialized_client();

  if(not m_data_type.isEmpty() and not m_service_name.isEmpty())
  {
    m_service_definition = ServiceDefinition::get(m_data_type);
    emit(serviceDefinitionChanged());

    if(m_service_definition and m_service_definition->isValid())
    {
      rcl_client_options_t client_ops = rcl_client_get_default_options();
      if(rcl_client_init(&m_client, RosThread::instance()->rclNode(),
                         m_service_definition->typeSupport(), qPrintable(m_service_name),
                         &client_ops)
         != RCL_RET_OK)
      {
        qWarning() << "Failed to initialize client: " << m_service_name
                   << rcl_get_error_string().str;
        rcl_reset_error();
      }
    }
    else
    {
      qWarning() << "Not a valid service definition for " << m_data_type;
    }
  }
}

#include "moc_ServiceClient.cpp"
