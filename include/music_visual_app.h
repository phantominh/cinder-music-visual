#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include <cinder/audio/Voice.h>

namespace musicvisual {

using namespace ci;
using namespace ci::app;

class MusicVisualApp : public App {
 public:
  MusicVisualApp();

  void setup() override;

  void draw() override;

  void update() override;

 private:
  audio::VoiceRef mVoice;
};

}  // namespace musicvisual