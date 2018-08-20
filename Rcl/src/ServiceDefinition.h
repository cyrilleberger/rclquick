#include <QObject>

#include <rosidl_generator_c/service_type_support_struct.h>

class MessageDefinition;

class ServiceDefinition : public QObject
{
public:
  ServiceDefinition(const QString& _type_name);
  ~ServiceDefinition();
  static ServiceDefinition* get(const QString& _type_name);
  bool isValid() const;
  MessageDefinition* requestDefinition() const { return m_requestDefinition; }
  MessageDefinition* answerDefinition() const  { return m_answerDefinition;  }
  const rosidl_service_type_support_t* typeSupport() { return m_type_support; }
private:
  bool m_is_valid = false;
  QString m_type_name;
  const rosidl_service_type_support_t* m_type_support = nullptr;
  MessageDefinition* m_requestDefinition = nullptr;
  MessageDefinition* m_answerDefinition = nullptr;
};
