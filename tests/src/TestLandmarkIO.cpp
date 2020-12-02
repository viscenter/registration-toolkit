#include <gtest/gtest.h>

#include <random>
#include <string>

#include "rt/LandmarkRegistrationBase.hpp"
#include "rt/io/LandmarkIO.hpp"

using namespace rt;

static LandmarkContainer RandomLandmarks(size_t num)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution randReal(
        0.0, std::numeric_limits<double>::max());

    // Random landmark coordinates
    LandmarkContainer lc;
    for (size_t i = 0; i < num; i++) {
        Landmark l;
        l[0] = randReal(gen);
        l[1] = randReal(gen);
        lc.emplace_back(l);
    }
    return lc;
}

TEST(LandmarkIO, RoundTrip)
{
    // Get random LandmarkContainer
    auto orig = RandomLandmarks(100);

    // Round trip
    std::string path = "TestLandmarkIO_RoundTrip.lc";
    EXPECT_NO_THROW(WriteLandmarkContainer(path, orig));

    LandmarkContainer result;
    EXPECT_NO_THROW(result = ReadLandmarkContainer(path));

    // Compare Landmarks
    EXPECT_EQ(result, orig);
}