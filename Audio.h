#ifndef AUDIO_H
#define AUDIO_H

#include <iostream>
#include <ao/ao.h>
#include <mpg123.h>
#include <string>

#define BITS 8

class AudioLoop{
public:
	AudioLoop(const char* mp3file);
        int AudioPlaying;
        void playSoundOnce();
        void waitForCompletion();
private:
	std::string audio_track;
        pthread_t thread1;
};

#endif
