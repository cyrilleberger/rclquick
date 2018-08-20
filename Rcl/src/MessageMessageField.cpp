#include "MessageMessageField.h"

#include <QVariant>

#include "MessageDefinition.h"

MessageMessageField::~MessageMessageField()
{
}

void MessageMessageField::elementInitialize(quint8* _data) const
{
  for(MessageField* mf : m_md->fields())
  {
    mf->fieldInitialize(_data + mf->index());
  }
}

void MessageMessageField::elementFinalize(quint8* _data) const
{
  for(MessageField* mf : m_md->fields())
  {
    mf->fieldFinalize(_data + mf->index());
  }
}

QVariant MessageMessageField::elementReadValue(const quint8* _data) const
{
  return m_md->deserializeMessage(_data);
}

void MessageMessageField::elementWriteValue(quint8* _data, const QVariant& _value) const
{
  m_md->serializeMessage(_value.toMap(), _data);
}

std::size_t MessageMessageField::elementSize() const
{
  return m_md->serializedLength();
}

#include "moc_MessageMessageField.cpp"
