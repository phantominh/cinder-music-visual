#pragma once

#include "audio_visualizer.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/audio/Voice.h"
#include "cinder/audio/audio.h"
#include "cinder/gl/gl.h"

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
   * @param event
   */
  void mouseDrag( MouseEvent event ) override;

 private:
  // Node for sample audio playback
  audio::BufferPlayerNodeRef buffer_player_node_;
  size_t last_saved_frame_;

  AudioVisualizer visualizer_;

  void DisplayInfoBoard();
};

}  // namespace musicvisual