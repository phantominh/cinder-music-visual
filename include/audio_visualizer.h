#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/audio/audio.h"
#include "cinder/audio/Voice.h"

namespace musicvisual {

using namespace ci;
using namespace ci::app;

/**
 * This class visualizes the audio buffer
 */
class AudioVisualizer {
 public:
  /**
   * Initialize the visualizer
   *
   */
  AudioVisualizer();

  /**
   * Load audio buffer and bounds of the visualizer
   * @param buffer
   * @param bounds
   * @param sample_rate
   */
  void Load(const audio::Buffer &buffer, const Rectf &bounds, const size_t &sample_rate);

  /**
   * Display the general audio magnitude in time domain at a specific frame
   * @param frame
   */
  void DisplayGeneralMagnitudeInTimeDomain(const size_t &frame) const;

  /**
   * Display current play position
   * @param frame
   */
  void DisplayPosition(const size_t &frame) const;

  /**
   * Returns a graph that represent the instant data at current frame (time domain).
   * @param data
   * @param frame
   * @return PolyLine2f
   */
  auto CalculateInstantGraphInTimeDomain(const float *data, const size_t& frame) const -> PolyLine2f;

 private:
  audio::Buffer buffer_;
  Rectf bounds_;

  // Number of frames per second
  size_t sample_rate_;

  float display_rate_time_domain = 50;
};

}  // namespace musicvisual