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
   */
  AudioVisualizer();

  /**
   * Load audio buffer and bounds of the visualizer
   * @param buffer
   * @param bounds
   * @param sample_rate
   * @param display_rate_time_domain
   */
  void Load(const audio::Buffer &buffer, const Rectf &bounds, const size_t &sample_rate, const float &display_rate_time_domain  = 20);

  /**
   * Display the instant audio magnitude in time domain at a specific frame
   * @param frame
   */
  void DisplayInstantMagnitudeInTimeDomain(const size_t &frame) const;

  /**
   * Display current play position
   * @param frame
   */
  void DisplayPosition(const size_t &frame) const;

  /**
   * Display the general magnitude in time domain at a specific frame
   * @param frame
   */
  void DisplayGeneralMagnitudeInTimeDomain(const size_t &frame) const;

  /**
   * Returns a graph that represent the instant data at current frame (time domain).
   * @param data
   * @param frame
   * @return PolyLine2f
   */
  PolyLine2f CalculateInstantGraphInTimeDomain(const float *data, const size_t& frame) const;

  /**
   * Compress the buffer to data per second
   */
   // FIXME: Make this more flexible in the future
  void ConstructCompressedBuffer();

 private:
  audio::Buffer buffer_;
  Rectf bounds_;

  size_t sample_rate_;   // Number of frames per second
  float display_rate_time_domain_; // Rate of instant display (time domain)

  std::vector<float> compressed_buffer_; // The compressed version of buffer

  const float kMargin = 100;

  // Graph boundaries
  Rectf general_time_domain_graph_bound_;
  Rectf instant_time_domain_graph_bound_;


  /**
   * Convert the magnitude to a displayable ratio. Magnitude range: [-1, 1]
   * @param magnitude
   * @return
   */
  auto ConvertMagnitudeToDisplayableRatio(const float &magnitude) const -> float;
};

}  // namespace musicvisual