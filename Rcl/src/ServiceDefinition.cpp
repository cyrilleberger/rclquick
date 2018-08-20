#include "ServiceDefinition.h"

#include <ament_index_cpp/get_package_share_directory.hpp>


#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QHash>


#include "MessageDefinition.h"
#include "TypeSupport.h"

ServiceDefinition::ServiceDefinition(const QString& _type_name) : m_type_name(_type_name), m_requestDefinition(new MessageDefinition(this)), m_answerDefinition(new MessageDefinition(this))
{
  qDebug() << "ServiceDefinition for " << m_type_name;
  QStringList splited = m_type_name.split('/');
  if(splited.size() != 2)
  {
    qWarning() << "Invalid type: " << _type_name;
    return;
  }
  const QString packagename = splited[0];
  const QString servicename = splited[1];
  QFile file(QString::fromStdString(ament_index_cpp::get_package_share_directory(packagename.toStdString())) + "/srv/" + servicename + ".srv");

  if(file.open(QIODevice::ReadOnly))
  {
    QTextStream stream(&file);
    QString data = file.readAll();
    
    QStringList splited = data.split("---\n");
    if(splited.size() != 2)
    {
      qWarning() << "Invalid service definition: " << _type_name;
      return;
    }
    
    QTextStream request_definition(&splited[0]);
    m_requestDefinition->parseDefinition(packagename, request_definition);
    QTextStream answer_definition(&splited[1]);
    m_answerDefinition->parseDefinition(packagename, answer_definition);
    
    m_is_valid = m_requestDefinition->isValid() and m_answerDefinition->isValid();
    m_type_support = TypeSupport::getServiceTypeSupport(packagename, servicename);
    m_is_valid = m_is_valid and m_type_support;
    
  } else {
    qWarning() << "Failed to open: " << file.fileName();
  }
  
}

ServiceDefinition::~ServiceDefinition()
{
}

ServiceDefinition* ServiceDefinition::get(const QString& _type_name)
{
  static QHash<QString, ServiceDefinition*> definitions;
  ServiceDefinition* md = definitions[_type_name];
  if(not md)
  {
    md = new ServiceDefinition(_type_name);
    definitions[_type_name] = md;
  }
  return md;
}
