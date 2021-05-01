#include "music_visual_app.h"

namespace visualmusic {

MusicVisualApp::MusicVisualApp() = default;

void MusicVisualApp::setup() {
  auto ctx = audio::Context::master();

  // Create a source file and set its output sample rate to match the context
  audio::SourceFileRef source_file =
      audio::load(app::loadAsset("12 Liebesleid (Love's Sorrow).m4a"),
                  ctx->getSampleRate());

  // Initialize the buffer player node
  buffer_player_node_ = ctx->makeNode(new audio::BufferPlayerNode());
  buffer_player_node_->loadBuffer(source_file);

  // Connect nodes & context and enable them
  buffer_player_node_ >> ctx->getOutput();
  buffer_player_node_->enable();
  ctx->enable();

  visualizer_.Load(*buffer_player_node_->getBuffer(),
                   Rectf(static_cast<float>(getWindowBounds().x1) +
                             static_cast<float>(kMargin),
                         static_cast<float>(getWindowBounds().y1) +
                             static_cast<float>(kMargin),
                         static_cast<float>(getWindowBounds().x2) -
                             static_cast<float>(kMargin),
                         static_cast<float>(getWindowBounds().y2) -
                             static_cast<float>(kMargin)),
                   ctx->getSampleRate());
}

void MusicVisualApp::draw() {
  gl::clear();
  gl::enableAlphaBlending();

  visualizer_.Display(last_saved_frame_);

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
  buffer_player_node_->seekToTime(
      buffer_player_node_->getNumSeconds() *
      (static_cast<double>(event.getX()) - static_cast<double>(kMargin)) /
      (static_cast<double>(getWindowWidth()) -
       2 * static_cast<double>(kMargin)));
}

void MusicVisualApp::DisplayInfoBoard() {
  // Display time
  std::string output =
      "time: " +
      std::to_string(static_cast<float>(last_saved_frame_) /
                     static_cast<float>(buffer_player_node_->getSampleRate())) +
      "/" + std::to_string(buffer_player_node_->getNumSeconds());
  gl::drawStringRight(
      output,
      vec2(getWindowBounds().getX2() - 20, getWindowBounds().getY2() - 40),
      Color("white"));

  // Sample rate
  gl::drawStringRight(
      "sample_rate: " + std::to_string(buffer_player_node_->getSampleRate()) +
          " fps",
      vec2(getWindowBounds().getX2() - 20, getWindowBounds().getY2() - 60),
      Color("white"));

  // Display frame
  output = "frame: " + std::to_string(buffer_player_node_->getReadPosition()) +
           "/" + std::to_string(buffer_player_node_->getNumFrames());
  gl::drawStringRight(
      output,
      vec2(getWindowBounds().getX2() - 20, getWindowBounds().getY2() - 20),
      Color("white"));

  // Display state
  std::string state = "playing";
  if (!buffer_player_node_->isEnabled()) {
    state = "paused";
  }
  gl::drawStringRight(
      state,
      vec2(getWindowBounds().getX2() - 20, getWindowBounds().getY1() + 10),
      Color("white"));
}

}  // namespace visualmusic