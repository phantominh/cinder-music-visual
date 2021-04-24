#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/audio/Voice.h"
#include "cinder/audio/audio.h"
#include "cinder/gl/gl.h"

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
   * @param instant_display_rate_time_domain
   * @param general_display_rate_time_domain
   */
  void Load(const audio::Buffer &buffer, const Rectf &bounds,
            const size_t &sample_rate,
            const size_t &instant_display_rate_time_domain = 20,
            const size_t &general_display_rate_time_domain = 100);

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
   * Returns a graph that represent the instant data at current frame (time
   * domain).
   * @param data
   * @param frame
   * @return PolyLine2f
   */
  auto CalculateInstantGraphInTimeDomain(const float *data,
                                         const size_t &frame) const
      -> PolyLine2f;

  /**
   * Compress the buffer to data per second
   */
  void ConstructCompressedBuffer();

  auto FindMaximumMagnitude(const audio::Buffer &buffer) const -> float;

  auto FindMaximumMagnitude(const std::vector<float> &buffer) const -> float;

 private:
  audio::Buffer buffer_;
  Rectf bounds_;

  size_t sample_rate_;                       // Number of frames per second
  size_t instant_time_domain_display_rate_;  // Rate of instant display (time
                                             // domain)
  size_t general_time_domain_display_rate_;  // Rate of general display (time
                                             // domain)

  std::vector<float> compressed_buffer_;  // The compressed version of buffer

  const float kMargin = 50;

  // Graph boundaries
  Rectf instant_time_domain_graph_bounds_;
  Rectf general_time_domain_graph_bounds_;

  // Maximum magnitudes
  float max_magnitude_;
  float max_magnitude_compressed_;

  /**
   * Convert the magnitude to a displayable ratio. Magnitude range: [-1, 1]
   * @param magnitude
   * @return
   */
  auto ConvertMagnitudeToDisplayableRatio(const float &magnitude,
                                          const float &max_magnitude) const
      -> float;
};

}  // namespace musicvisual