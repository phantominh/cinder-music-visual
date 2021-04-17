#include "music_visual_app.h"

namespace musicvisual {

MusicVisualApp::MusicVisualApp() {
}

void MusicVisualApp::setup() {
  auto ctx = audio::Context::master();

  // Create a SourceFile and set its output sample rate to match the context
  audio::SourceFileRef source_file =
      audio::load(app::loadAsset("01 Ballade No. 1 in G Minor, Op. 23.m4a"),
                  ctx->getSampleRate());

  // Initialize the buffer player node
  buffer_player_node_ = ctx->makeNode(new audio::BufferPlayerNode());
  buffer_player_node_->loadBuffer(source_file);

  // Connect nodes and context
  buffer_player_node_ >> ctx->getOutput();

  buffer_player_node_->enable();
  ctx->enable();
}

void MusicVisualApp::draw() {
  gl::clear();
  gl::enableAlphaBlending();

  AppBase::draw();
}

void MusicVisualApp::update() {
  AppBase::update();
}

void MusicVisualApp::keyDown(KeyEvent event) {
  if (event.getCode() == KeyEvent::KEY_SPACE) {
    if (buffer_player_node_->isEnabled())
      buffer_player_node_->stop();
    else
      buffer_player_node_->start();
  }
}

void MusicVisualApp::mouseDown(MouseEvent event) {
  buffer_player_node_->start();
}

}  // namespace musicvisual