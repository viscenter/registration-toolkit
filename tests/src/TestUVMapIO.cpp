#include <gtest/gtest.h>

#include <random>

#include "rt/io/UVMapIO.hpp"
#include "rt/types/UVMap.hpp"

using namespace rt;

static auto RandomUVMap(size_t numUVs, size_t numFaces) -> UVMap
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution randReal(
        0.0, std::nextafter(1.0, std::numeric_limits<double>::max()));
    std::uniform_int_distribution<size_t> randInt(0, numUVs - 1);

    // UV Map
    UVMap uv;

    // Random UV coordinates
    for (size_t i = 0; i < numUVs; i++) {
        uv.addUV({randReal(gen), randReal(gen)});
    }

    // Random Faces
    for (size_t i = 0; i < numFaces; i++) {
        auto a = randInt(gen);
        auto b = randInt(gen);
        while (b == a) {
            b = randInt(gen);
        }
        auto c = randInt(gen);
        while (c == a or c == b) {
            c = randInt(gen);
        }
        uv.addFace(a, b, c);
    }

    return uv;
}

TEST(UVMapIO, RoundTrip)
{
    // Get random UV Map
    auto orig = RandomUVMap(100, 75);

    // Round trip
    EXPECT_NO_THROW(WriteUVMap("TestUVMapIO_RoundTrip.uvm", orig));

    UVMap result;
    EXPECT_NO_THROW(result = ReadUVMap("TestUVMapIO_RoundTrip.uvm"));

    // Compare sizes
    EXPECT_EQ(result.size(), orig.size());
    EXPECT_EQ(result.size_faces(), orig.size_faces());

    // Compare UVs
    EXPECT_EQ(result.uvs_as_vector(), orig.uvs_as_vector());

    // Compare faces
    // cv::Vec_<size_t> doesn't define equality operator so manually compare
    for (const auto& [idx, resFace] : result.faces_as_map()) {
        auto origFace = orig.getFace(idx);
        EXPECT_EQ(resFace[0], origFace[0]);
        EXPECT_EQ(resFace[1], origFace[1]);
        EXPECT_EQ(resFace[2], origFace[2]);
    }
}