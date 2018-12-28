#ifndef THEA_SIMPLE_ITERATOR_H
#define THEA_SIMPLE_ITERATOR_H

namespace thea {

template <class T> class SimpleIterator {
public:
  virtual ~SimpleIterator(){};
  virtual bool next();
  virtual T item();
  virtual bool end();
};

} // namespace thea

#endif
