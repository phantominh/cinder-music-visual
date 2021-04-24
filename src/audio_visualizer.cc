#include "audio_visualizer.h"

namespace musicvisual {

AudioVisualizer::AudioVisualizer() = default;

void AudioVisualizer::Load(const audio::Buffer& buffer, const Rectf& bounds, const size_t &sample_rate) {
  buffer_ = buffer;
  bounds_ = bounds;
  sample_rate_ = sample_rate;
}

void AudioVisualizer::DisplayGeneralMagnitudeInTimeDomain(const size_t& frame) const {
  // Color only has effect in this scope
  gl::ScopedGlslProg glslScope( getStockShader( gl::ShaderDef().color() ) );
  gl::color(Color("red"));

  for (size_t channel = 0; channel < buffer_.getNumChannels(); channel++) {
    PolyLine2f waveform =
        CalculateInstantGraphInTimeDomain(buffer_.getChannel(channel), frame);

    if (!waveform.getPoints().empty()) {
      gl::draw(waveform);
    }
  }
}

auto AudioVisualizer::CalculateInstantGraphInTimeDomain(const float *data, const size_t& frame) const -> PolyLine2f {
  PolyLine2f waveform;

  // Default wave height of this graph
  const float wave_height = bounds_.getHeight() / 5;
  const float x_scale = bounds_.getWidth() / (static_cast<float>(sample_rate_) / display_rate_time_domain);

  float y_offset = bounds_.y2;
  float x = bounds_.x1;

  for (size_t f = frame; f < frame + sample_rate_ / static_cast<size_t>(display_rate_time_domain); f++) {
    x += x_scale;
    float y = y_offset - (1 - (data[f] * 0.5f + 0.5f)) * wave_height;

    waveform.push_back(vec2(x, y));
  }

  return waveform;
}

void AudioVisualizer::DisplayPosition(const size_t& frame) const {
  // Color only has effect in this scope
  gl::ScopedGlslProg glslScope( getStockShader( gl::ShaderDef().color() ) );
  gl::color(Color("white"));

  // Draw current play position
  float position = static_cast<float>(bounds_.getWidth()) *
                   static_cast<float>(frame) /
                   static_cast<float>(buffer_.getNumFrames());

  gl::drawSolidCircle(vec2(position, bounds_.getHeight() / 2), 5);
}

}  // namespace musicvisual