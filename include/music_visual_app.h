#pragma once

#include "audio_visualizer.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/audio/Voice.h"
#include "cinder/audio/audio.h"
#include "cinder/gl/gl.h"

namespace visualmusic {

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
  void keyDown(KeyEvent event) override;

  /**
   * Handle mouse input
   * @param event
   */
  void mouseDrag(MouseEvent event) override;

  /**
   * Resize the app
   */
  void resize() override;

 private:
  // Node for sample audio playback
  audio::BufferPlayerNodeRef buffer_player_node_;
  size_t last_saved_frame_;

  // Modify this if necessary
  const float kMargin = 50;

  // Visualizer that handle and draw audio buffers
  AudioVisualizer visualizer_;

  /**
   * Display the info board, including: time, frame, fps, etc
   */
  void DisplayInfoBoard();

  /**
   * Display Guidance
   */
  void DisplayGuidance();
};

}  // namespace visualmusic