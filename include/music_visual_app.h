#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/audio/audio.h"
#include "cinder/audio/Voice.h"

#include "audio_buffer_visualizer.h"

namespace musicvisual {

using namespace ci;
using namespace ci::app;

class MusicVisualApp : public App {
 public:
  /**
   * Initialize the Music Visual Application
   */
  MusicVisualApp();

  /**
   * Setup the Application
   */
  void setup() override;

  /**
   * Draw the Application
   */
  void draw() override;

  /**
   * Update per frame
   */
  void update() override;

  /**
   * Handle user's input key
   * @param event
   */
  void keyDown( KeyEvent event ) override;

  /**
   * Handle mouse input
   */
  void mouseDrag( MouseEvent event ) override;

 private:
  // Node for sample audio playback
  audio::BufferPlayerNodeRef buffer_player_node_;
  AudioBufferVisualizer visualizer_;

  /**
   * Draw current play position
   */
   void DrawPlayPosition();
};

}  // namespace musicvisual