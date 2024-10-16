#include <QSharedData>

class MessageDefinition;

class MessageData
{
public:
  MessageData(const MessageDefinition* _definition);
  MessageData(const MessageDefinition* _definition, quint8* _data);
  MessageData(const MessageData& _rhs);
  ~MessageData();
  MessageData& operator=(const MessageData& _rhs);
  quint8* data() { return d->data; }
  const quint8* data() const { return d->data; }
  const MessageDefinition* definition() const { return d->definition; }
private:
  struct Data : public QSharedData
  {
    ~Data();
    quint8* data;
    const MessageDefinition* definition;
  };
  QExplicitlySharedDataPointer<Data> d;
};
