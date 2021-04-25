#include "audio_visualizer.h"

namespace musicvisual {

AudioVisualizer::AudioVisualizer() = default;

void AudioVisualizer::Load(const audio::Buffer& buffer, const Rectf& bounds,
                           const size_t& sample_rate,
                           const size_t& instant_display_rate_time_domain,
                           const size_t& general_display_rate_time_domain) {
  buffer_ = buffer;
  bounds_ = bounds;
  sample_rate_ = sample_rate;

  // Play rate
  instant_time_domain_display_rate_ = instant_display_rate_time_domain;
  general_time_domain_display_rate_ = general_display_rate_time_domain;

  // Boundaries
  general_time_domain_graph_bounds_ = Rectf(
      vec2(bounds_.getX1(), bounds_.getY1() + bounds_.getHeight() * 6 / 10),
      vec2(bounds_.getX2(), bounds_.getY1() + bounds_.getHeight() * 8 / 10));
  instant_time_domain_graph_bounds_ = Rectf(
      vec2(bounds_.getX1(), bounds_.getY1() + bounds_.getHeight() * 8.2 / 10),
      vec2(bounds_.getX2(), bounds_.getY1() + bounds_.getHeight() * 9.2 / 10));

  ConstructCompressedBuffer();
  max_magnitude_ = FindMaximumMagnitude(buffer_);
  max_magnitude_compressed_ = FindMaximumMagnitude(compressed_buffer_);
}

void AudioVisualizer::Display(const size_t& frame) const {
  DisplayInstantMagnitudeInTimeDomain(frame);
  DisplayGeneralMagnitudeInTimeDomain(frame);
}

void AudioVisualizer::DisplayInstantMagnitudeInTimeDomain(
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
  for (size_t f = frame; f < frame + sample_rate_ / static_cast<size_t>(instant_time_domain_display_rate_); f++) {
    float y;

    // Handle edge case: The final frames
    if (f >= buffer_.getNumFrames()) {
      y = instant_time_domain_graph_bounds_.y2 -
          ConvertMagnitudeToDisplayableRatio(0.0f, max_magnitude_) *
              wave_height;
    } else {
      y = instant_time_domain_graph_bounds_.y2 -
          ConvertMagnitudeToDisplayableRatio(data[f], max_magnitude_) *
              wave_height;
    }

    waveform.push_back(vec2(x, y));
    x += x_scale;
  }

  return waveform;
}

void AudioVisualizer::DisplayGeneralMagnitudeInTimeDomain(
    const size_t& frame) const {
  // Color only has effect in this scope
  gl::ScopedGlslProg glslScope(getStockShader(gl::ShaderDef().color()));
  gl::color(Color("white"));

  // Display border
  gl::drawStrokedRect(general_time_domain_graph_bounds_);

  // Init the graph
  PolyLine2f waveform = PolyLine2f();
  const float wave_height = general_time_domain_graph_bounds_.getHeight();
  const float x_scale = general_time_domain_graph_bounds_.getWidth() /
                        (static_cast<float>(compressed_buffer_.size()));
  float x = general_time_domain_graph_bounds_.x1;

  // Construct the graph
  for (size_t f = 0; f < (frame / (sample_rate_ / general_time_domain_display_rate_)); f++) {
    float y;

    y = general_time_domain_graph_bounds_.y2 -
        ConvertMagnitudeToDisplayableRatio(compressed_buffer_[f],
                                           max_magnitude_compressed_) *
            wave_height;

    waveform.push_back(vec2(x, y));
    x += x_scale;
  }

  if (!waveform.getPoints().empty()) {
    gl::draw(waveform);
  }

  // Display current playtime
  gl::color(Color("red"));
  gl::drawStrokedRect(Rectf(x, general_time_domain_graph_bounds_.getY1(),
                            x + x_scale,
                            general_time_domain_graph_bounds_.getY2()));
}

auto AudioVisualizer::ConvertMagnitudeToDisplayableRatio(
    const float& magnitude, const float& max_magnitude) const -> float {
  return 0.5f * (1 - magnitude / max_magnitude);
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

void AudioVisualizer::SetMaxMagnitude(const float& magnitude) {
  max_magnitude_ = magnitude;
}

}  // namespace musicvisual