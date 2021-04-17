#include "music_visual_app.h"

namespace musicvisual {

MusicVisualApp::MusicVisualApp() {
}

void MusicVisualApp::setup() {
  audio::SourceFileRef sourceFile = audio::load( app::loadAsset( "01 Ballade No. 1 in G Minor, Op. 23.m4a" ) );
  mVoice = audio::Voice::create( sourceFile );

  // Start playing audio from the voice:
  mVoice->start();
}

void MusicVisualApp::draw() {
  AppBase::draw();
}

void MusicVisualApp::update() {
  AppBase::update();
}

}