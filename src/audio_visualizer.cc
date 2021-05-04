#include "audio_visualizer.h"

namespace visualmusic {

AudioVisualizer::AudioVisualizer() = default;

void AudioVisualizer::Load(const audio::Buffer& buffer, const Rectf& bounds,
                           const size_t& sample_rate,
                           const size_t& instant_display_rate_time_domain,
                           const size_t& general_display_rate_time_domain,
                           const size_t& three_dimension_display_rate) {
  buffer_ = buffer;
  bounds_ = bounds;
  sample_rate_ = sample_rate;

  // Play rate
  instant_time_domain_display_rate_ = instant_display_rate_time_domain;
  general_time_domain_display_rate_ = general_display_rate_time_domain;
  three_dimension_display_rate_ = three_dimension_display_rate;

  ConstructBoundaries();

  ConstructCompressedBuffer();
  max_magnitude_general_ = FindMaximumMagnitude(buffer_);
  max_magnitude_compressed_ = FindMaximumMagnitude(compressed_buffer_);

  ConstructBufferSpectralArray(kFrequencyRange);
}

void AudioVisualizer::Resize(Rectf bounds) {
  bounds_ = bounds;
  ConstructBoundaries();
}

void AudioVisualizer::ConstructBoundaries() {
  // Boundaries
  general_time_domain_graph_bounds_ = Rectf(
      vec2(bounds_.getX1(), bounds_.getY1() + bounds_.getHeight() * 5.5 / 10),
      vec2(bounds_.getX2(), bounds_.getY1() + bounds_.getHeight() * 8 / 10));
  instant_time_domain_graph_bounds_ = Rectf(
      vec2(bounds_.getX1(), bounds_.getY1() + bounds_.getHeight() * 8.2 / 10),
      vec2(bounds_.getX2(), bounds_.getY1() + bounds_.getHeight() * 9.2 / 10));
  three_dimension_graph_bounds_ = Rectf(
      vec2(bounds_.getX1(), bounds_.getY1() + bounds_.getHeight() * 0.5 / 10),
      vec2(bounds_.getX2(), bounds_.getY1() + bounds_.getHeight() * 5.3 / 10));
}

void AudioVisualizer::Display(const size_t& frame) const {
  DisplayInstantGraphInTimeDomain(frame);
  DisplayGeneralGraphInTimeDomain(frame);
  Display3DGraph(frame);
}

void AudioVisualizer::DisplayInstantGraphInTimeDomain(
    const size_t& frame) const {
  // Color only has effect in this scope
  gl::ScopedGlslProg glslScope(getStockShader(gl::ShaderDef().color()));

  // Display Border
  gl::color(Color("white"));
  gl::drawStrokedRect(instant_time_domain_graph_bounds_);

  // Display Graph
  gl::color(Color("red"));
  for (size_t channel = 0; channel < buffer_.getNumChannels(); channel++) {
    PolyLine2f waveform =
        CalculateInstantGraphInTimeDomain(buffer_.getChannel(channel), frame);

    if (!waveform.getPoints().empty()) {
      gl::draw(waveform);
    }
  }
}

auto AudioVisualizer::CalculateInstantGraphInTimeDomain(
    const float* data, const size_t& frame) const -> PolyLine2f {
  PolyLine2f waveform = PolyLine2f();

  // Default wave height of this graph
  const float wave_height = instant_time_domain_graph_bounds_.getHeight();
  const float x_scale = instant_time_domain_graph_bounds_.getWidth() /
                        (static_cast<float>(sample_rate_) /
                         static_cast<float>(instant_time_domain_display_rate_));

  float x = instant_time_domain_graph_bounds_.x1;

  // Construct the graph out of the buffers
  for (size_t f = frame;
       f < frame + sample_rate_ /
                       static_cast<size_t>(instant_time_domain_display_rate_);
       f++) {
    float y;

    // Handle edge case: The final frames
    if (f >= buffer_.getNumFrames()) {
      y = instant_time_domain_graph_bounds_.y2 -
          ConvertMagnitudeToDisplayableRatio(0.0f, max_magnitude_general_) *
              wave_height;
    } else {
      y = instant_time_domain_graph_bounds_.y2 -
          ConvertMagnitudeToDisplayableRatio(data[f], max_magnitude_general_) *
              wave_height;
    }

    waveform.push_back(vec2(x, y));
    x += x_scale;
  }

  return waveform;
}

void AudioVisualizer::DisplayGeneralGraphInTimeDomain(
    const size_t& frame) const {
  // Color only has effect in this scope
  gl::ScopedGlslProg glslScope(getStockShader(gl::ShaderDef().color()));
  gl::color(Color("white"));

  // Display border
  gl::drawStrokedRect(general_time_domain_graph_bounds_);

  // Init the graph
  PolyLine2f waveform = CalculateGeneralGraphInTimeDomain(frame);

  if (!waveform.getPoints().empty()) {
    gl::draw(waveform);
  }

  // Display current playtime
  if (waveform.size() != 0) {
    gl::color(Color("red"));

    // Get last buffer
    auto last_iter = --waveform.end();
    const float x_scale = general_time_domain_graph_bounds_.getWidth() /
                          (static_cast<float>(compressed_buffer_.size()));
    gl::drawStrokedRect(Rectf(
        last_iter->x, general_time_domain_graph_bounds_.getY1(),
        last_iter->x + x_scale, general_time_domain_graph_bounds_.getY2()));
  }
}

auto AudioVisualizer::CalculateGeneralGraphInTimeDomain(
    const size_t& frame) const -> PolyLine2f {
  // Init the graph
  PolyLine2f waveform = PolyLine2f();
  const float wave_height = general_time_domain_graph_bounds_.getHeight();
  const float x_scale = general_time_domain_graph_bounds_.getWidth() /
                        (static_cast<float>(compressed_buffer_.size()));
  float x = general_time_domain_graph_bounds_.x1;

  // Construct the graph
  for (size_t f = 0;
       f < (frame / (sample_rate_ / general_time_domain_display_rate_)); f++) {
    float y;

    y = general_time_domain_graph_bounds_.y2 -
        ConvertMagnitudeToDisplayableRatio(compressed_buffer_[f],
                                           max_magnitude_compressed_) *
            wave_height;

    waveform.push_back(vec2(x, y));
    x += x_scale;
  }

  return waveform;
}

void AudioVisualizer::ConstructCompressedBuffer() {
  float magnitude_per_range = 0;

  for (size_t frame = 0; frame < buffer_.getNumFrames(); frame++) {
    // Add value to range
    for (size_t channel = 0; channel < buffer_.getNumChannels(); channel++) {
      magnitude_per_range += buffer_.getChannel(channel)[frame];
    }

    // If end range, input data to compress_buffer, and reset variable
    if ((frame + 1) % (sample_rate_ / general_time_domain_display_rate_) == 0) {
      compressed_buffer_.push_back(
          magnitude_per_range *
          static_cast<float>(general_time_domain_display_rate_) *
          static_cast<float>(sample_rate_ * buffer_.getNumChannels()));
      magnitude_per_range = 0;
    }

    // Else if ending frame
    else if (frame == buffer_.getNumFrames() - 1) {
      compressed_buffer_.push_back(
          magnitude_per_range /
          static_cast<float>(
              (buffer_.getSize() %
               (sample_rate_ / general_time_domain_display_rate_)) *
              buffer_.getNumChannels()));
    }
  }
}

void AudioVisualizer::Display3DGraph(const size_t& frame) const {
  // Color only has effect in this scope
  gl::ScopedGlslProg glslScope(getStockShader(gl::ShaderDef().color()));
  gl::color(Color("white"));

  // Display border
  gl::drawStrokedRect(three_dimension_graph_bounds_);

  // Display multiple frequency domain graph
  for (size_t i = 0; i < three_dimension_display_rate_; i++) {
    // Avoid out of script

    if (frame < i * kFrequencyRange) {
      break;
    }

    vec2 top_left_corner = vec2(
        three_dimension_graph_bounds_.getX1() +
            static_cast<float>(i) * three_dimension_graph_bounds_.getWidth() /
                static_cast<float>(three_dimension_display_rate_),
        three_dimension_graph_bounds_.getY2() -
            three_dimension_graph_bounds_.getHeight() / 2 -
            static_cast<float>(i) *
                (three_dimension_graph_bounds_.getHeight() / 2) /
                static_cast<float>(three_dimension_display_rate_));

    vec2 bottom_right_corner =
        vec2(three_dimension_graph_bounds_.getX2(),
             three_dimension_graph_bounds_.getY2() -
                 static_cast<float>(i) *
                     (three_dimension_graph_bounds_.getHeight() / 2) /
                     static_cast<float>(three_dimension_display_rate_));

    Rectf graph_bounds = Rectf(top_left_corner, bottom_right_corner);

    PolyLine2f waveform = CalculateInstantGraphInFrequencyDomain(
        (frame - i * kFrequencyRange), graph_bounds);

    if (!waveform.getPoints().empty()) {
      float color_indicator =
          1.0f - static_cast<float>(i) /
                     static_cast<float>(three_dimension_display_rate_);

      // Different state of white
      gl::color(Color(color_indicator, color_indicator, color_indicator));
      gl::draw(waveform);
    }
  }
}

auto AudioVisualizer::CalculateInstantGraphInFrequencyDomain(
    const size_t& frame, const Rectf& bounds) const -> PolyLine2f {
  // Init the graph
  PolyLine2f waveform = PolyLine2f();
  const float wave_height = bounds.getHeight();
  const float x_scale = bounds.getWidth() / static_cast<float>(kFrequencyRange);
  float x = bounds.x1;

  float* buffer = buffer_spectral_arr_[frame / kFrequencyRange]->getData();

  // Construct the graph
  for (size_t f = 0; f < kFrequencyRange; f++) {
    float y;

    y = bounds.y2 - ConvertMagnitudeToDisplayableRatio(
                        buffer[f], static_cast<float>(max_magnitude_fft_)) *
                        wave_height;

    waveform.push_back(vec2(x, y));
    x += x_scale;
  }

  return waveform;
}

void AudioVisualizer::ConstructBufferSpectralArray(const size_t& fft_size) {
  if (!isPowerOf2(fft_size)) {
    throw std::invalid_argument("Range must be a power of 2");
  }

  max_magnitude_fft_ = 0.0f;

  std::vector<audio::Buffer*> buffer_arr = GenerateBufferPerRange(fft_size);
  audio::dsp::Fft fft = audio::dsp::Fft(fft_size);
  audio::BufferSpectral* temp;

  for (auto& buffer : buffer_arr) {
    temp = new audio::BufferSpectral(fft_size);
    fft.forward(buffer, temp);
    buffer_spectral_arr_.push_back(temp);

    float max_temp = FindMaximumMagnitude(temp);
    if (max_magnitude_fft_ < max_temp) {
      max_magnitude_fft_ = max_temp;
    }
  }
}

auto AudioVisualizer::ConvertMagnitudeToDisplayableRatio(
    const float& magnitude, const float& max_magnitude) const -> float {
  return 0.5f * (1 - magnitude / max_magnitude);
}

auto AudioVisualizer::FindMaximumMagnitude(const audio::Buffer& buffer) const
    -> float {
  float max_magnitude = 0;

  for (size_t i = 0; i < buffer.getSize(); i++) {
    max_magnitude = std::fmaxf(max_magnitude, abs(buffer.getData()[i]));
  }

  return max_magnitude;
}

auto AudioVisualizer::FindMaximumMagnitude(
    const std::vector<float>& buffer) const -> float {
  float max_magnitude = 0;

  for (float i : buffer) {
    max_magnitude = std::fmaxf(max_magnitude, abs(i));
  }

  return max_magnitude;
}

auto AudioVisualizer::FindMaximumMagnitude(
    const audio::BufferSpectral* buffer) const -> float {
  float max_magnitude = 0;

  for (size_t i = 0; i < buffer->getSize(); i++) {
    max_magnitude = std::fmaxf(max_magnitude, abs(buffer->getData()[i]));
  }

  return max_magnitude;
}

void AudioVisualizer::SetMaxMagnitude(const float& magnitude) {
  max_magnitude_general_ = magnitude;
}

auto AudioVisualizer::GenerateBufferPerRange(const size_t& range_size) const
    -> std::vector<audio::Buffer*> {
  std::vector<audio::Buffer*> buffer_vector;

  for (size_t frame = 0; frame + range_size < buffer_.getNumFrames();
       frame += range_size) {
    auto* temp = new audio::Buffer(range_size, buffer_.getNumChannels());

    // Copy a portion from the original buffer
    temp->copyOffset(buffer_, range_size, 0, frame);
    buffer_vector.push_back(temp);
  }

  // Handle the last frames
  if (buffer_.getNumFrames() % range_size != 0) {
    auto* temp = new audio::Buffer(range_size, buffer_.getNumChannels());
    size_t current_frame =
        buffer_.getNumFrames() - 1 - (buffer_.getNumFrames() % range_size);

    temp->copyOffset(buffer_, (buffer_.getNumFrames() % range_size), 0,
                     current_frame);
    buffer_vector.push_back(temp);
  }

  return buffer_vector;
}

}  // namespace visualmusic