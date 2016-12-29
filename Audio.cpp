#include "Audio.h"

void *playOnce(void *ptr);

AudioLoop::AudioLoop(const char* mp3file)
{
	audio_track = std::string(mp3file);
	AudioPlaying = 0;
}

void AudioLoop::playSoundOnce()
{

	pthread_create(&thread1, NULL, playOnce, (void *) &audio_track);
        AudioPlaying = 1;
}        


void AudioLoop::waitForCompletion()
{
        pthread_join(thread1,NULL);
        AudioPlaying = 0;
}

void *playOnce(void *ptr){
    mpg123_handle *mh;
    unsigned char *buffer;
    size_t buffer_size;
    size_t done;
    int err;

    int driver;
    ao_device *dev;

    ao_sample_format format;
    int channels, encoding;
    long rate;


    /* initializations */
    ao_initialize();
    driver = ao_default_driver_id();
    if (driver == -1)
    {
        std::cout << "No standard sound driver found, No sound will be heard"<< std::endl;
        return NULL;
        
    }
    mpg123_init();
    mh = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));
    std::string *filename;
    filename=(std::string *)ptr;
    /* open the file and get the decoding format */
    mpg123_open(mh, filename->c_str());
    mpg123_getformat(mh, &rate, &channels, &encoding);

    /* set the output format and open the output device */
    format.bits = mpg123_encsize(encoding) * BITS;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    dev = ao_open_live(driver, &format, NULL);
    std::cout << "Playing file: " << filename->c_str() << std::endl;
    /* decode and play */
    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK){
    	ao_play(dev, (char*)buffer, done);
    	//AudioPlaying = 1;
    }
     std::cout << "Completed playing file: " << filename->c_str() << std::endl;  

    /* clean up */
    free(buffer);
    ao_close(dev);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    ao_shutdown();
    //AudioPlaying = 0;
    return NULL;
}
