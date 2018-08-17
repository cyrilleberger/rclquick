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
  QByteArray md5() const { return m_md5; }
  MessageDefinition* requestDefinition() const { return m_requestDefinition; }
  MessageDefinition* answerDefinition() const  { return m_answerDefinition;  }
  const rosidl_service_type_support_t* typeSupport();
private:
  bool m_is_valid = false;
  QString m_type_name;
  MessageDefinition* m_requestDefinition = nullptr;
  MessageDefinition* m_answerDefinition = nullptr;
  QByteArray m_md5;
};
