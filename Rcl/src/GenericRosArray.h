
struct GenericRosArray
{
  quint8* data;
  size_t size;
  size_t capacity;
};

struct GenericRosArrayInterface
{
  static void fieldInitialize(quint8* _data)
  {
    GenericRosArray* array = reinterpret_cast<GenericRosArray*>(_data);
    array->data = nullptr;
    array->size = 0;
    array->capacity = 0;
  }
  static void fieldFinalize(quint8* _data)
  {
    GenericRosArray* array = reinterpret_cast<GenericRosArray*>(_data);
    free(array->data);
    array->data = nullptr;
    array->size = 0;
    array->capacity = 0;
  }

};
