#pragma once

template <class T> class CircularBuffer{
  public:
    CircularBuffer(const size_t &size);
    void addValue(const T &value);
    T getAverage(void) const;
    T getMax(void) const;
  private:
    T* array;
    size_t arraySize;
    uint32_t index;
    size_t numElements;
};

template <class T> CircularBuffer <T>::CircularBuffer(const size_t &size)
{
  arraySize = size;
  array = new T[size];
}

template <class T> void CircularBuffer <T>::addValue(const T &value)
{

  if(++numElements > arraySize) numElements = arraySize;
  array[index++] = value;
  index %= arraySize;
}

template <class T> T CircularBuffer <T>::getAverage(void) const
{
  T sum = 0;
  for(unsigned int i = 0; i < numElements; i++)
  {
    sum += array[i];
  }
  return (sum / numElements);
}

template <class T> T CircularBuffer <T>::getMax(void) const
{
  T sum = 0;
  for(int i = 0; i < numElements; i++)
  {
    sum += array[i];
  }
  return (sum / numElements);
}
