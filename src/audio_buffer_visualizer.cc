#include "audio_buffer_visualizer.h"

namespace musicvisual {

AudioBufferVisualizer::AudioBufferVisualizer() = default;

void AudioBufferVisualizer::Load(const audio::Buffer &buffer, const Rectf& bounds) {
  buffer_ = buffer;
  bounds_ = bounds;
}

void AudioBufferVisualizer::DisplayAtFrame(const size_t& frame) const {
  gl::ScopedGlslProg glslScope( getStockShader( gl::ShaderDef().color() ) );
  gl::color( Color("white") );

  const float wave_height = bounds_.getHeight();
  const float x_scale = bounds_.getWidth() / static_cast<float>(buffer_.getNumFrames());
  float y_offset = bounds_.y1;

  for ( size_t channel = 0; channel < buffer_.getNumChannels(); channel++) {
    const float *channel_data = buffer_.getChannel(channel);
    float x = bounds_.x1 + static_cast<float>(frame) * x_scale;
    float y = ( 1 - ( channel_data[frame] * 0.5f + 0.5f ) ) * wave_height + y_offset;

    gl::drawSolidRect(Rectf(vec2(x,y), vec2(x + 10, y)));
  }
}

void AudioBufferVisualizer::DisplayAllAtFrame(const size_t&  frame) const {
  gl::ScopedGlslProg glslScope( getStockShader( gl::ShaderDef().color() ) );
  gl::color( Color("white") );

  const float wave_height = bounds_.getHeight();
  const float x_scale = bounds_.getWidth() / static_cast<float>(buffer_.getNumFrames());
  float y_offset = bounds_.y1;

  for ( size_t channel = 0; channel < 1; channel++) {
    PolyLine2f waveform;
    const float *channel_data = buffer_.getChannel(channel);
    float x = bounds_.x1;

    for (size_t f = 0; f < frame; f++) {
      x += x_scale;
      float y = ( 1 - ( channel_data[f] * 0.5f + 0.5f ) ) * wave_height + y_offset;
      waveform.push_back( vec2(x, y) );
    }

    if ( ! waveform.getPoints().empty() ) {
      gl::draw( waveform );
    }
  }
}

}  // namespace musicvisual