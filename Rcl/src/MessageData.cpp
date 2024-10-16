#include "MessageData.h"

#include "MessageDefinition.h"

MessageData::MessageData(const MessageDefinition* _definition) : d(new Data)
{
  d->definition = _definition;
  d->data = _definition->allocateZeroInitialised();
}

MessageData::MessageData(const MessageDefinition* _definition, quint8* _data) : d(new Data)
{
  d->definition = _definition;
  d->data = _data;
}

MessageData::MessageData(const MessageData& _rhs) : d(_rhs.d) {}

MessageData& MessageData::operator=(const MessageData& _rhs)
{
  d = _rhs.d;
  return *this;
}

MessageData::~MessageData() {}

MessageData::Data::~Data() { definition->disallocate(data); }
