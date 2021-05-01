#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/audio/Voice.h"
#include "cinder/audio/audio.h"
#include "cinder/gl/gl.h"

namespace visualmusic {

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
            const size_t &general_display_rate_time_domain = 100,
            const size_t &three_dimension_display_rate = 10);

  /**
   * Display everything inside the visualizer at a specific frame
   * @param frame
   */
  void Display(const size_t &frame) const;

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
   * Returns a graph that represent the instant data at current frame (time
   * domain).
   * @param frame
   * @return PolyLine2f
   */
  auto CalculateGeneralGraphInTimeDomain(const size_t &frame) const
      -> PolyLine2f;

  /**
   * Returns a frequency graph at index (frame / frequency_range)
   * @param frame
   * @return
   */
  auto CalculateInstantGraphInFrequencyDomain(const size_t &frame,
                                              const Rectf &bounds) const
      -> PolyLine2f;

  /**
   * Compress the buffer to data per range
   */
  // TODO: Find a way to test this function
  void ConstructCompressedBuffer();

  /**
   * Construct an array of buffer spectral (Frequency Domain)
   * @param fft_size Size of Fft for transformation from time domain to
   * frequency domain
   */
  void ConstructBufferSpectralArray(const size_t &fft_size);

  /**
   * Set a custom maximum magnitude
   * @param magnitude
   */
  void SetMaxMagnitude(const float &magnitude);

 private:
  audio::Buffer buffer_;
  std::vector<audio::BufferSpectral *> buffer_spectral_arr_;
  Rectf bounds_;

  size_t sample_rate_;                       // Number of frames per second
  size_t instant_time_domain_display_rate_;  // Rate of instant display (time
                                             // domain)
  size_t general_time_domain_display_rate_;  // Rate of general display (time
                                             // domain)
  size_t three_dimension_display_rate_;  // Rate of general display (time
  // domain)


  std::vector<float> compressed_buffer_;  // The compressed version of buffer

  // Graph boundaries
  Rectf instant_time_domain_graph_bounds_;
  Rectf general_time_domain_graph_bounds_;
  Rectf three_dimension_graph_bounds_;

  // Maximum magnitudes
  float max_magnitude_general_;
  float max_magnitude_compressed_;
  float max_magnitude_fft_;

  // Frequency range
  const size_t kFrequencyRange = static_cast<size_t>(pow(2, 10));

  /**
   * Display the instant audio magnitude in time domain at a specific frame
   * @param frame
   */
  void DisplayInstantGraphInTimeDomain(const size_t &frame) const;

  /**
   * Display the general magnitude in time domain at a specific frame
   * @param frame
   */
  void DisplayGeneralGraphInTimeDomain(const size_t &frame) const;

  /**
   * Display the 3d audio graph - frequency & time domain
   * @param frame
   */
  void Display3DGraph(const size_t &frame) const;

  /**
   * Convert the magnitude to a displayable ratio. Magnitude range: [-1, 1]
   * @param magnitude
   * @return a variable proportional to the display screen
   */
  auto ConvertMagnitudeToDisplayableRatio(const float &magnitude,
                                          const float &max_magnitude) const
      -> float;

  /**
   * Find the maximum magnitude
   * @param buffer
   * @return max magnitude
   */
  auto FindMaximumMagnitude(const audio::Buffer &buffer) const -> float;

  /**
   * Find the maximum magnitude
   * @param buffer
   * @return max magnitude
   */
  auto FindMaximumMagnitude(const std::vector<float> &buffer) const -> float;

  /**
   * Find the maximum magnitude
   * @param buffer
   * @return max magnitude
   */
  auto FindMaximumMagnitude(const audio::BufferSpectral *buffer) const -> float;

  /**
   * Divide buffer data into smaller buffers (per second)
   * @param range_size
   * @return list of smaller buffers
   */
  auto GenerateBufferPerRange(const size_t &range_size) const
      -> std::vector<audio::Buffer *>;
};

}  // namespace visualmusic