#include "TypeSupport.h"

#include <QDebug>
#include <QLibrary>

namespace TypeSupport
{

  typedef const rosidl_message_type_support_t* (*GetMessageTypeSupportFunction)();
  typedef const rosidl_service_type_support_t* (*GetServiceTypeSupportFunction)();

#if defined(Q_OS_MACOS)
  const char* lib_prefix = "lib";
  const char* lib_ext = ".dylib";
#elif defined(Q_OS_LINUX)
  const char* lib_prefix = "lib";
  const char* lib_ext = ".so";
#elif defined(Q_OS_WINDOWS)
  const char* lib_prefix = "";
  const char* lib_ext = ".dll";
#endif


  const rosidl_message_type_support_t* getMessageTypeSupport(const QString& _package_name, const QString& _msg_name)
  {
    QFunctionPointer function = QLibrary::resolve(lib_prefix + _package_name + "__rosidl_typesupport_c" + lib_ext,
        qPrintable("rosidl_typesupport_c__get_message_type_support_handle__" + _package_name + "__msg__" + _msg_name));
    
    if (function)
    {
      return reinterpret_cast<GetMessageTypeSupportFunction>(function)();
    }
    else
    {
      qWarning() << "Failed to get type support for message: " << _package_name << "/" << _msg_name;
      return nullptr;
    }

  }

  const rosidl_service_type_support_t* getServiceTypeSupport(const QString& _package_name, const QString& _srvname)
  {
    QFunctionPointer function = QLibrary::resolve(lib_prefix + _package_name + "__rosidl_typesupport_c" + lib_ext,
        qPrintable("rosidl_typesupport_c__get_message_type_support_handle__" + _package_name + "__srv__" + _srvname));
    
    if (function)
    {
      return reinterpret_cast<GetServiceTypeSupportFunction>(function)();
    }
    else
    {
      qWarning() << "Failed to get type support for service: " << _package_name << "/" << _srvname;
      return nullptr;
    }

  }

}
