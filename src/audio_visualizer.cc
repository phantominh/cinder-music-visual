#include "audio_visualizer.h"

namespace musicvisual {

AudioVisualizer::AudioVisualizer() = default;

void AudioVisualizer::Load(const audio::Buffer& buffer, const Rectf& bounds) {
  buffer_ = buffer;
  bounds_ = bounds;
}

// FIXME: This is a terrible way to display frame. Over 2.5M frames per small
// audio file.
// Looping through every single frame per Cinder update would break the
// program. Alternative: Possibly Calculate Average? For example: Split audio
// all frames to 1000 parts?
void AudioVisualizer::DisplayAllAtFrame(const size_t& frame) const {
  gl::ScopedGlslProg glslScope(getStockShader(gl::ShaderDef().color()));
  gl::color(Color("white"));

  const float wave_height = bounds_.getHeight();
  const float x_scale =
      bounds_.getWidth() / static_cast<float>(buffer_.getNumFrames());
  float y_offset = bounds_.y1;

  for (size_t channel = 0; channel < 1; channel++) {
    PolyLine2f waveform;
    const float* channel_data = buffer_.getChannel(channel);
    float x = bounds_.x1;

    for (size_t f = 0; f < frame; f++) {
      x += x_scale;
      float y = (1 - (channel_data[f] * 0.5f + 0.5f)) * wave_height + y_offset;
      waveform.push_back(vec2(x, y));
    }

    if (!waveform.getPoints().empty()) {
      gl::draw(waveform);
    }
  }
}

void AudioVisualizer::DrawPlayPosition(const size_t& frame) const {
  // Draw current play position
  float position = static_cast<float>(bounds_.getWidth()) *
                   static_cast<float>(frame) /
                   static_cast<float>(buffer_.getNumFrames());

  gl::color(Color("red"));
  gl::drawSolidCircle(vec2(position, bounds_.getHeight() / 2), 5);
  std::string output = "Frame: " + std::to_string(frame) + "/" +
                       std::to_string(buffer_.getNumFrames());
  gl::drawStringRight(output, vec2(bounds_.getX2() - 20, bounds_.getY2() - 20),
                      Color("white"));
}

}  // namespace musicvisual