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
   */
  void Load(const audio::Buffer &buffer, const Rectf &bounds);

  /**
   * Display all data at a specific frame
   * @param frame
   */
  void DisplayAllAtFrame(const size_t &frame) const;

  /**
   * Draw current play position
   */
  void DrawPlayPosition(const size_t &frame) const;

 private:
  audio::Buffer buffer_;
  Rectf bounds_;
};

}