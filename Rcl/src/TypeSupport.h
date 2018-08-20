#include <rosidl_generator_c/message_type_support_struct.h>
#include <rosidl_generator_c/service_type_support_struct.h>

#include <QString>

namespace TypeSupport
{
  const rosidl_message_type_support_t* getMessageTypeSupport(const QString& _package_name, const QString& _msg_name);
  const rosidl_service_type_support_t* getServiceTypeSupport(const QString& _package_name, const QString& _srvname);
}
