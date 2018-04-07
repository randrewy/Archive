# Archive
Archive provides a simple and straightforward interface to serialize/deserialize objects
as binary data via user-defined Storage class.

By default Archive works with:
  - primitive types
  - arrays ans std::array
  - enums and enum classes
  - stl-like containers
  - strings
  - tuples
  - optional-like types

If any other type is needed external `serialize_object/deserialize_object` can be provided by user
and woud be automatically used by Archive internals.
  
Archive has no schema, so no extra information is stored except that is needed to deserialize a type.
Type safety should be provided by user. Mixing serialize source and deserialize destination is allowed
yet almost always disastrous.


### Example
```c++
// extremely simple user-provided class (look example.cpp for some details)
struct DummyStorage {
    size_t write(const char* data, size_t size);
    void read(char* data, size_t size);
};

void example() {
    // by default storage will be placed inside Achive class
    // look storage_policy if you need other behaviour
    archive::BinaryArchive<DummyStorage> archive;

    int i = 42;
    double d = 13.37;
    std::vector<int> vec {1, 5, 7, 9};
    std::string str = "string";

    archive.serialize(i);
    archive.serialize(d);
    archive.serialize(vec);
    archive.serialize(str);
    
    
    int i1 = 0;
    double d1 = 0;
    std::vector<int> vec1;
    std::string str1;
    std::pair<int, std::string> p1;

    archive.deserialize(i1);   // 42
    archive.deserialize(d1);   // 13.37
    archive.deserialize(vec1); // {1, 5, 7, 9}
    archive.deserialize(str1); // "string"
}

```
