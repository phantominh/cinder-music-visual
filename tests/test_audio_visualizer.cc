#include <catch2/catch.hpp>

#include "audio_visualizer.h"

using namespace ci;

TEST_CASE("Test CalculateInstantGraphInTimeDomain") {
  musicvisual::AudioVisualizer visualizer;
  visualizer.Load(audio::Buffer(2, 2), Rectf(vec2(0, 0), vec2(10, 10)), 2, 1);

  // Create an array (representing audio buffer)
  float *arr_ptr = new float[2];
  arr_ptr[0] = 0.1f;
  arr_ptr[1] = 0.5f;

  // Use the calculation method to transform the array
  std::vector<vec2> graph_data = visualizer.CalculateInstantGraphInTimeDomain(arr_ptr, 0).getPoints();

  REQUIRE(Approx(graph_data[0].x).epsilon(0.001) == 0);
  REQUIRE(Approx(graph_data[0].y).epsilon(0.001) ==  9.1);
  REQUIRE(Approx(graph_data[1].x).epsilon(0.001) == 5);
  REQUIRE(Approx(graph_data[1].y).epsilon(0.001) == (10 - 0.25f * 2));

  delete arr_ptr;
}