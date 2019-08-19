#include "archive.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <type_traits>
#include <assert.h>
#include <string.h>
#include <tuple>
#include <array>
#include <optional>

enum Enum {E1, E2};
enum class Enumc {E1, E2};

struct TestPack {
    int value;
    bool operator == (const TestPack& other) const { return value == other.value; }
};

template<typename Archive>
archive::usize serialize_object(const TestPack& t, Archive& a) {
    return a.serialize(t.value);
}

template<typename Archive>
void deserialize_object(TestPack& t, Archive& a) {
    a.deserialize(t.value);
}

template<size_t buffer_size>
struct DummyStorage {
    unsigned char buffer[buffer_size];
    size_t read_pos = 0;
    size_t write_pos = 0;

    size_t write(const unsigned char* data, size_t size) {
        assert(write_pos + size < buffer_size);
        memcpy(&buffer[write_pos], data, size);
        write_pos += size;
        return size;
    }

    void read(unsigned char* data, size_t size) {
        assert(read_pos + size < buffer_size);
        memcpy(data, &buffer[read_pos], size);
        read_pos += size;
    }
};

struct TestObject {
    int i {};
    double d {};
    char c {};
    Enum e {};
    Enumc ec {};
    std::vector<int> vec {};
    std::string str {};
    std::pair<int, std::string> p{};
    std::map<int, std::string> map {};
    TestPack tp {};
    std::tuple<TestPack, double> tup {};
    std::array<int, 4> sarr {};
    int arr[3] = {};
    std::optional<int> opt {};
    std::optional<int> opte {};
};

TestObject makeTestObject() {
    TestObject test;

    test.i = 1233124;
    test.d = 123.1243;
    test.c = 't';
    test.e = E2;
    test.ec = Enumc::E2;
    test.vec = {1, 5, 7, 9};
    test.str = "string";
    test.p = {333, "second"};
    test.map = {
        {101, "one"},
        {202, "two"},
    };
    test.tp = { 777 };
    test.tup = { {12}, 0.404};
    test.sarr = {{1, 2, 3, 4}};
    test.arr[0] = 1;
    test.arr[1] = 2;
    test.arr[2] = 3;
    test.opt = std::make_optional<int>(222);

    return test;
}

void assert_equal(const TestObject& test, const TestObject& result) {
    assert(test.i == result.i);
    assert(test.d >= result.d && test.d <= result.d); // silent float comparisong warning
    assert(test.c == result.c);
    assert(test.e == result.e);
    assert(test.ec == result.ec);
    assert(test.str == result.str);
    assert(test.p == result.p);
    assert(test.vec == result.vec);
    assert(test.map == result.map);
    assert(test.tp == result.tp);
    assert(test.tup == result.tup);
    assert(test.sarr == result.sarr);
    assert(test.opt == result.opt);
    assert(test.opte == result.opte);

    assert(test.arr[0] == result.arr[0]);
    assert(test.arr[1] == result.arr[1]);
    assert(test.arr[2] == result.arr[2]);
}

void test_arch() {
    archive::BinaryArchive<DummyStorage<1024>> archive;
    const TestObject test = makeTestObject();

    archive.serialize(test.i);
    archive.serialize(test.d);
    archive.serialize(test.c);
    archive.serialize(test.e);
    archive.serialize(test.ec);
    archive.serialize(test.str);
    archive.serialize(test.p);
    archive.serialize(test.vec);
    archive.serialize(test.map);
    archive.serialize(test.tp);
    archive.serialize(test.tup);
    archive.serialize(test.sarr);
    archive.serialize(test.arr);
    archive.serialize(test.opt);
    archive.serialize(test.opte);

    TestObject result;

    archive.deserialize(result.i);
    archive.deserialize(result.d);
    archive.deserialize(result.c);
    archive.deserialize(result.e);
    archive.deserialize(result.ec);
    archive.deserialize(result.str);
    archive.deserialize(result.p);
    archive.deserialize(result.vec);
    archive.deserialize(result.map);
    archive.deserialize(result.tp);
    archive.deserialize(result.tup);
    archive.deserialize(result.sarr);
    archive.deserialize(result.arr);
    archive.deserialize(result.opt);
    archive.deserialize(result.opte);

    assert_equal(test, makeTestObject());
    assert_equal(test, result);
}

// both signatures are ok
// ```
//  template<typename Archive, archive::Direction policy>
//  void stream_serialization (archive::ArchiveStream<Archive, policy>& stream, archive::ArgumentRef<TestObject, policy>& t)
// ```
// or
template<typename Stream>
void stream_serialization (Stream& stream, archive::ArgumentRef<TestObject, Stream::get_policy()>& t) {
    stream & t.i & t.d & t.c & t.e
           & t.ec & t.vec & t.str & t.p
           & t.map & t.tp & t.tup & t.sarr
           & t.arr & t.opt & t.opte;
}

void test_stream() {
    using StorageType = DummyStorage<1024>;
    using ArchiveType = archive::BinaryArchive<StorageType, archive::storage_policy::NotOwningPointer>;
    StorageType storage;


    archive::stream::Writer<ArchiveType> writer(&storage);

    const TestObject test = makeTestObject();
    writer & test;


    archive::stream::Reader<ArchiveType> reader(&storage);

    TestObject result;
    reader & result;

    assert_equal(test, makeTestObject());
    assert_equal(test, result);
}

void test_directional() {
    archive::ArchiveStream<archive::BinaryArchive<DummyStorage<1024>>, archive::Direction::Bidirectional> archive;

    const TestObject test = makeTestObject();
    TestObject result;

    archive << test;
    archive >> result;

    assert_equal(test, makeTestObject());
    assert_equal(test, result);
}



void test_empty() {
    struct Empty {};
    archive::BinaryArchive<DummyStorage<1024>> archive;

    Empty e;
    std::tuple<> t;

    archive.serialize(e);
    archive.serialize(t);
    archive.deserialize(e);
    archive.deserialize(t);
    // compilation test
}

int main() {
    test_arch();
    test_stream();
    test_directional();
    test_empty();
    std::cout << "OK\n";
}
