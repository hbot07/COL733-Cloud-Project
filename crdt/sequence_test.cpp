#include <catch2/catch_test_macros.hpp>
#include <sequence.h>
#include <iostream>

TEST_CASE("Sequence runs without errors", "[sequence]") {
    Sequence seq;
    seq.append("item1");
    seq.append("item2");
    seq.insert(1, "item3");
    seq.remove(1);
    REQUIRE(seq.get().size() == 2);
    REQUIRE(seq.get()[0] == "item1");
    REQUIRE(seq.get()[1] == "item2");
}

TEST_CASE("Sequence appends items correctly", "[sequence]") {
    Sequence seq;
    seq.append("item1");
    seq.append("item2");
    seq.append("item3");
    REQUIRE(seq.get().size() == 3);
    REQUIRE(seq.get()[0] == "item1");
    REQUIRE(seq.get()[1] == "item2");
    REQUIRE(seq.get()[2] == "item3");
}

TEST_CASE("Sequence inserts items correctly", "[sequence]") {
    Sequence seq;
    seq.append("item1");
    seq.append("item2");
    seq.append("item3");
    seq.insert(1, "item4");
    REQUIRE(seq.get().size() == 4);
    REQUIRE(seq.get()[0] == "item1");
    REQUIRE(seq.get()[1] == "item4");
    REQUIRE(seq.get()[2] == "item2");
    REQUIRE(seq.get()[3] == "item3");
}

TEST_CASE("Sequence removes items correctly", "[sequence]") {
    Sequence seq;
    seq.append("item1");
    seq.append("item2");
    seq.append("item3");
    seq.remove(1);
    REQUIRE(seq.get().size() == 2);
    REQUIRE(seq.get()[0] == "item1");
    REQUIRE(seq.get()[1] == "item3");
}

TEST_CASE("Sequence merges correctly", "[sequence]") {
    Sequence seq1, seq2;
    seq1.append("item1");
    seq1.append("item2");
    seq2.append("item3");
    seq2.append("item4");
    seq1.merge(seq2);
    REQUIRE(seq1.get().size() == 4);
    REQUIRE(seq1.get()[0] == "item1");
    REQUIRE(seq1.get()[1] == "item2");
    REQUIRE(seq1.get()[2] == "item3");
    REQUIRE(seq1.get()[3] == "item4");
}

TEST_CASE("Sequence handles conflicts correctly", "[sequence]") {
    Sequence seq1, seq2;
    seq1.append("item1");
    seq1.append("item2");
    seq2.append("item3");
    seq2.insert(0, "item4");
    seq1.merge(seq2);
    REQUIRE(seq1.get().size() == 4);
    REQUIRE(seq1.get()[0] == "item4");
    REQUIRE(seq1.get()[1] == "item1");
    REQUIRE(seq1.get()[2] == "item2");
    REQUIRE(seq1.get()[3] == "item3");
}

TEST_CASE("Sequence handles tombstones correctly", "[sequence]") {
    Sequence seq;
    seq.append("item1");
    seq.append("item2");
    seq.append("item3");
    seq.remove(1);
    REQUIRE(seq.get().size() == 2);
    REQUIRE(seq.get()[0] == "item1");
    REQUIRE(seq.get()[1] == "item3");
}

TEST_CASE("Sequence handles nested sequences correctly", "[sequence]") {
    Sequence outer1, outer2;
    Sequence inner1, inner2;

    inner1.append("inner1-item1");
    inner1.append("inner1-item2");
    inner2.append("inner2-item1");
    inner2.append("inner2-item2");

    outer1.append(inner1);
    outer1.append("outer1-item1");
    outer2.append(inner2);
    outer2.append("outer2-item1");

    outer1.merge(outer2);

    REQUIRE(outer1.get().size() == 4);
    REQUIRE(outer1.get()[0].get()[0] == "inner1-item1");
    REQUIRE(outer1.get()[0].get()[1] == "inner1-item2");
    REQUIRE(outer1.get()[1] == "outer1-item1");
    REQUIRE(outer1.get()[2].get()[0] == "inner2-item1");
    REQUIRE(outer1.get()[2].get()[1] == "inner2-item2");
    REQUIRE(outer1.get()[3] == "outer2-item1");
}
