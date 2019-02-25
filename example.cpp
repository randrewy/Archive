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
    char buffer[buffer_size];
    size_t read_pos = 0;
    size_t write_pos = 0;

    size_t write(const char* data, size_t size) {
        assert(write_pos + size < buffer_size);
        memcpy(&buffer[write_pos], data, size);
        write_pos += size;
        return size;
    }

    void read(char* data, size_t size) {
        assert(read_pos + size < buffer_size);
        memcpy(data, &buffer[read_pos], size);
        read_pos += size;
    }
};

void test_arch() {
    archive::BinaryArchive<DummyStorage<1024>> archive;

    int i = 1233124;
    double d = 123.1243;
    char c = 't';
    Enum e = E2;
    Enumc ec = Enumc::E2;
    std::vector<int> vec {1, 5, 7, 9};
    std::string str = "string";
    std::pair<int, std::string> p {333, "second"};
    std::map<int, std::string> map {
        {101, "one"},
        {202, "two"},
    };
    TestPack tp { 777 };
    std::tuple<TestPack, double> tup { {12}, 0.404};
    std::array<int, 4> sarr {{1, 2, 3, 4}};
    int arr[3] = {1,2,3};
    std::optional<int> opt = std::make_optional<int>(222);
    std::optional<int> opte;

    archive.serialize(i);
    archive.serialize(d);
    archive.serialize(c);
    archive.serialize(e);
    archive.serialize(ec);
    archive.serialize(str);
    archive.serialize(p);
    archive.serialize(vec);
    archive.serialize(map);
    archive.serialize(tp);
    archive.serialize(tup);
    archive.serialize(sarr);
    archive.serialize(arr);
    archive.serialize(opt);
    archive.serialize(opte);


    int i1 = 0;
    double d1 = 0;
    char c1 = '0';
    Enum e1;
    Enumc ec1;
    std::string str1;
    std::pair<int, std::string> p1;
    std::vector<int> vec1;
    std::map<int, std::string> map1;
    TestPack tp1;
    std::tuple<TestPack, double> tup1;
    std::array<int, 4> sarr1 {};
    int arr1[3];
    std::optional<int> opt1;
    std::optional<int> opte1 = std::make_optional<int>(999);

    archive.deserialize(i1);
    archive.deserialize(d1);
    archive.deserialize(c1);
    archive.deserialize(e1);
    archive.deserialize(ec1);
    archive.deserialize(str1);
    archive.deserialize(p1);
    archive.deserialize(vec1);
    archive.deserialize(map1);
    archive.deserialize(tp1);
    archive.deserialize(tup1);
    archive.deserialize(sarr1);
    archive.deserialize(arr1);
    archive.deserialize(opt1);
    archive.deserialize(opte1);

    assert(i == i1);
    assert(d >= d1 && d <=d1); // silent float comparisong warning
    assert(c == c1);
    assert(e == e1);
    assert(ec == ec1);
    assert(str == str1);
    assert(p == p1);
    assert(vec == vec1);
    assert(map == map1);
    assert(tp == tp1);
    assert(tup == tup1);
    assert(sarr == sarr1);
    assert(opt == opt1);
    assert(opte == opte1);

    assert(arr[0] == arr1[0]);
    assert(arr[1] == arr1[1]);
    assert(arr[2] == arr1[2]);
}


int main() {
    test_arch();
}
