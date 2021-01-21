### format range:
It takes a range and a format (in this case using boost::fmt until std::format is more widely available) it spits another ranges of strings formatted.

### Interleave (actually not a range)
It works kinda like a zip_iterator, it takes several ranges and it outputs a
single range in which each increment to the iterator gives the next value of the
next range and then loops back. In other words it flattens several ranges into
one and interleaves the values.

Right now I'm trying to implement this with same length iterators, but i'll probably skip the ones that have reached their ends.


### Dependencies:

- boost format 1.74 or higher (lower versions of boost::fmt have a bug with gcc 10)
- gcc 10 or higher

### Compile instructions

```g++-10 format.cpp -I./boost_root -std=c++2a -o format```
