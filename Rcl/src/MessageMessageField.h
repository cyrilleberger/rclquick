#ifndef _MESSAGE_MESSAGE_FIELD_H_
#define _MESSAGE_MESSAGE_FIELD_H_

#include "MessageField.h"

class MessageDefinition;

class MessageMessageField : public MessageField
{
  Q_OBJECT
  Q_PROPERTY(MessageDefinition* messageDefinition READ messageDefinition CONSTANT)
public:
  MessageMessageField(const QString _name, MessageDefinition* _md, bool _array, std::size_t _index) : MessageField(_name, Type::Message, _array, _index), m_md(_md)
  {
  }
  ~MessageMessageField();
  void elementInitialize(quint8* _data) const override;
  void elementFinalize(quint8* _data) const override;
  QVariant elementReadValue(const quint8* _data) const override;
  void elementWriteValue(quint8* _data, const QVariant& _value) const override;
  std::size_t elementSize() const override;
  MessageDefinition* messageDefinition() const { return m_md; }
private:
  MessageDefinition* m_md;
};

#endif
