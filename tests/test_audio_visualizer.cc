#include <catch2/catch.hpp>

#include "audio_visualizer.h"

using namespace ci;

TEST_CASE("Test CalculateInstantGraphInTimeDomain") {
  visualmusic::AudioVisualizer visualizer;
  visualizer.Load(audio::Buffer(2, 2), Rectf(vec2(0, 0), vec2(10, 10)), 3, 1,
                  1);

  // Create an array (representing audio buffer)
  auto *arr_ptr = new float[2];
  arr_ptr[0] = 0.1f;
  arr_ptr[1] = 0.5f;

  visualizer.SetMaxMagnitude(0.5f);

  // Use the calculation method to transform the array
  std::vector<vec2> graph_data =
      visualizer.CalculateInstantGraphInTimeDomain(arr_ptr, 0).getPoints();

  SECTION("Buffer index in bound") {
    // Testing the transformation of buffer in the data
    REQUIRE(Approx(graph_data[0].x).epsilon(0.001) == 0.0f);
    REQUIRE(Approx(graph_data[0].y).epsilon(0.001) == 8.8000001907f);
    REQUIRE(Approx(graph_data[1].x).epsilon(0.001) == 3.3333f);
    REQUIRE(Approx(graph_data[1].y).epsilon(0.001) == 9.199999f);
  }

  SECTION("Buffer index out of bound") {
    // Testing the transformation of out-of-index buffer
    REQUIRE(Approx(graph_data[2].x).epsilon(0.001) == 6.6666f);
    REQUIRE(Approx(graph_data[2].y).epsilon(0.001) == 8.6999f);
  }

  delete[] arr_ptr;
}