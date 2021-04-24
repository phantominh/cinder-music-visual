#include "music_visual_app.h"

namespace musicvisual {

MusicVisualApp::MusicVisualApp() = default;

void MusicVisualApp::setup() {
  auto ctx = audio::Context::master();

  // Create a source file and set its output sample rate to match the context
  audio::SourceFileRef source_file =
      audio::load(app::loadAsset("01 Ballade No. 1 in G Minor, Op. 23.m4a"),
                  ctx->getSampleRate());

  // Initialize the buffer player node
  buffer_player_node_ = ctx->makeNode(new audio::BufferPlayerNode());
  buffer_player_node_->loadBuffer(source_file);

  // Connect nodes & context and enable them
  buffer_player_node_ >> ctx->getOutput();
  buffer_player_node_->enable();
  ctx->enable();

  visualizer_.Load(*buffer_player_node_->getBuffer(), getWindowBounds(), ctx->getSampleRate());
}

void MusicVisualApp::draw() {
  gl::clear();
  gl::enableAlphaBlending();

  visualizer_.DisplayGeneralMagnitudeInTimeDomain(last_saved_frame_);
  visualizer_.DisplayPosition(last_saved_frame_);

  DisplayInfoBoard();
}

void MusicVisualApp::update() {
  if (buffer_player_node_->isEnabled()) {
    last_saved_frame_ = buffer_player_node_->getReadPosition();
  }
}

void MusicVisualApp::keyDown(KeyEvent event) {
  if (event.getCode() == KeyEvent::KEY_SPACE) {
    if (buffer_player_node_->isEnabled()) {
      buffer_player_node_->stop();
    } else {
      buffer_player_node_->start();
      buffer_player_node_->seek(last_saved_frame_);
    }
  }
}

void MusicVisualApp::mouseDrag(MouseEvent event) {
  if (!buffer_player_node_->isEnabled()) {
    buffer_player_node_->start();
  }
  buffer_player_node_->seekToTime(buffer_player_node_->getNumSeconds() *
                                  static_cast<double>(event.getX()) /
                                  static_cast<double>(getWindowWidth()));
}

void MusicVisualApp::DisplayInfoBoard() {
  std::string output = "play_time: " + std::to_string(static_cast<float>(last_saved_frame_) / buffer_player_node_->getSampleRate()) + "/" + std::to_string(buffer_player_node_->getNumSeconds());
  gl::drawStringRight(output, vec2(getWindowBounds().getX2() - 20, getWindowBounds().getY2() - 40),
                      Color("white"));
  gl::drawStringRight("sample_rate: " + std::to_string(buffer_player_node_->getSampleRate()) + " fps", vec2(getWindowBounds().getX2() - 20, getWindowBounds().getY2() - 60),
                      Color("white"));

  std::string state = "playing";
  if (!buffer_player_node_->isEnabled()) {
    state = "paused";
  }
  gl::drawStringRight(state, vec2(getWindowBounds().getX2() - 20, getWindowBounds().getY1() + 10),
                      Color("white"));
}

}  // namespace musicvisual