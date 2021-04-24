#include "music_visual_app.h"

using musicvisual::MusicVisualApp;

void prepareSettings(MusicVisualApp::Settings* settings) {
  settings->setFullScreen();
}

// This line is a macro that expands into an "int main()" function.
CINDER_APP(MusicVisualApp, ci::app::RendererGl, prepareSettings);

