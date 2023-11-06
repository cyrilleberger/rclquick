#include "MessageField.h"

#include <QVariant>

namespace
{
  struct GenericRosArray
  {
    quint8* data;
    size_t size;
    size_t capacity;
  };
}

MessageField::MessageField(const QString& _name, Type _type, bool _array, std::size_t _index) : m_name(_name), m_type(_type), m_is_array(_array), m_index(_index)
{
}

std::size_t MessageField::index() const
{
  std::size_t a = alignment();
  if((m_index % a) > 0)
  {
    return m_index + a - (m_index % a);
  } else {
    return m_index;
  }
}

void MessageField::fieldInitialize(quint8* _data) const
{
  if(m_is_array)
  {
    GenericRosArray* array = reinterpret_cast<GenericRosArray*>(_data);
    array->data = nullptr;
    array->size = 0;
    array->capacity = 0;
  } else {
    return elementInitialize(_data);
  }
}

void MessageField::fieldFinalize(quint8* _data) const
{
  if(m_is_array)
  {
    GenericRosArray* array = reinterpret_cast<GenericRosArray*>(_data);
    const std::size_t elt_size = elementSize();
    for(std::size_t i = 0; i < array->size; ++i)
    {
      elementFinalize(array->data + i * elt_size);
    }
    free(array->data);
    array->data = nullptr;
    array->size = 0;
    array->capacity = 0;
  } else {
    return elementFinalize(_data);
  }
}

QVariant MessageField::fieldReadValue(const quint8* _data) const
{
  if(m_is_array)
  {
    const GenericRosArray* array = reinterpret_cast<const GenericRosArray*>(_data);
    QVariantList list;
    const std::size_t elt_size = elementSize();
    for(std::size_t i = 0; i < array->size; ++i)
    {
      list.append(elementReadValue(array->data + i * elt_size));
    }
    return list;
  } else {
    return elementReadValue(_data);
  }
}

void MessageField::fieldWriteValue(quint8* _data, const QVariant& _value) const
{
  if(m_is_array)
  {
    fieldFinalize(_data);
    GenericRosArray* array = reinterpret_cast<GenericRosArray*>(_data);
    
    QVariantList list;
    if(_value.canConvert<QVariantList>())
    {
      list = _value.toList();
    } else {
      list.append(_value);
    }
    
    array->size = list.size();
    array->capacity = array->size;
    array->data = reinterpret_cast<quint8*>(malloc(array->size * elementSize()));
    const std::size_t elt_size = elementSize();
    
    for(std::size_t i = 0; i < array->size; ++i)
    {
      quint8* element = array->data + elt_size * i;
      elementInitialize(element);
      elementWriteValue(element, list[i]);
    }
    
  } else {
    elementWriteValue(_data, _value);
  }
}

std::size_t MessageField::fieldSize() const
{
  if(m_is_array)
  {
    return sizeof(GenericRosArray);
  } else {
    return elementSize();
  }
}

std::size_t MessageField::alignment() const
{
  if(m_is_array)
  {
    return alignof(GenericRosArray);
  } else {
    return elementAlignment();
  }
}

#include "moc_MessageField.cpp"
