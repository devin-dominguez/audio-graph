clang  -Wall -D__MACOSX_CORE__ -I./include src/*.c  libportaudio_static.a -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices -framework Carbon -o app
