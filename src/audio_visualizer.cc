#include "audio_visualizer.h"

namespace musicvisual {

AudioVisualizer::AudioVisualizer() = default;

void AudioVisualizer::Load(const audio::Buffer& buffer, const Rectf& bounds, const size_t &sample_rate, const float &display_rate_time_domain) {
  buffer_ = buffer;
  bounds_ = bounds;
  sample_rate_ = sample_rate;
  display_rate_time_domain_ = display_rate_time_domain;
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

PolyLine2f AudioVisualizer::CalculateInstantGraphInTimeDomain(const float *data, const size_t& frame) const {
  PolyLine2f waveform = PolyLine2f();

  // Default wave height of this graph
  const float wave_height = bounds_.getHeight() / 5;
  const float x_scale = bounds_.getWidth() / (static_cast<float>(sample_rate_) /
                                              display_rate_time_domain_);

  float y_offset = bounds_.y2;
  float x = bounds_.x1;

  for (size_t f = frame; f < frame + sample_rate_ / static_cast<size_t>(display_rate_time_domain_); f++) {
    float y;

    if (frame >= buffer_.getNumFrames()) {
      y = y_offset - wave_height / 5  ;
    } else {
      y = y_offset - ConvertMagnitudeToDisplayableRatio(data[f]) * wave_height;
    }

    waveform.push_back(vec2(x, y));
    x += x_scale;
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

float AudioVisualizer::ConvertMagnitudeToDisplayableRatio(const float &magnitude) const {
  return (1.0f - (magnitude * 0.5f + 0.5f));
}

}  // namespace musicvisual