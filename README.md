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

Archive has no schema, so no extra information is stored except that is needed to deserialize a type.
Type safety, when deserializing binary data should be ensured by user. Endianness is currently considered
same for serialization/deserialization. 

Archive supports user-defined types. Look [User-Defined types](#user-defined-types)

## API
Archive has two different API

#### 1. Pair of serialize / deserialize
Use class `Archive<Storage, StoragePolicy>` for this API.
Use `serialize(T&)` / `deserialize(const T&)` methods explicitly


#### 2. Operators API
Use class `ArchiveStream<Archive, Direction>` for this API.
`ArchiveStream` type is defined by it's `Direction`:
* `Deserialize`
* `Serialize`
* `Bidirectional`

`Deserialize/Serialize` types can be used with `operator &`, as the operation is defined by the type itself
```c++
ArchiveStream<..., Serialize> serializeStream;
ArchiveStream<..., Deserialize> deserializeStream;

serializeStream   & a & b & c & d;
deserializeStream & a & b & c & d;
```
`Bidirectional` can only be used with explicit operators `>>` and `<<` so that operation is defined by the operator
```c++
ArchiveStream<..., Bidirectional> stream;

stream >> a >> b >> c >> d;
stream << a << b << c << d;
```

Note that operators `>>` and `<<` can also be used with corresponding directional-typed stream.


## User-defined types
For APIv1 provide two standalone functions:
```c++
archive::usize serialize_object(const CustomType& t, Archive& a);
void deserialize_object(CustomType& t, Archive& a);
```
For APIv2 you can also use theese functions but you can define a single one to do both operations:
```c++
void stream_serialization (ArchiveStream& stream, CustomType& t);
```
If you want to maintain const-correctness make it a template, for example:
```c++
template<typename Archive, archive::Direction policy>
void stream_serialization (ArchiveStream<Archive, policy>& stream, ArgumentRef<CustomType, policy>& t);
// OR
template<typename Stream>
void stream_serialization (Stream& stream, archive::ArgumentRef<CustomType, Stream::get_policy()>& t);
``` 
`archive::ArgumentRef<T, Direction>` will resolve into `non-const T&` for a `Deserialize` type (and only for it).
More specialized versions of `stream_serialization` template are OK.


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

### Example APIv2
```c++
struct SomeStruct {
    int i;
    double d;

    template<typename Stream>
    friend void stream_serialization (Stream& stream, archive::ArgumentRef<SomeStruct, Stream::get_policy()>& t) {
        stream & t.i & t.d;
    }
}

template<typename ArchiveStream>
void example_v2(ArchiveStream& stream) {
    SomeStruct s;
    stream & s;
}
```