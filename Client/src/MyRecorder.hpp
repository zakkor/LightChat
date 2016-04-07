#include "SFML/Audio.hpp"

class MyRecorder : public sf::SoundRecorder
{
    virtual bool onStart() // optional
        {
            // initialize whatever has to be done before the capture starts
            ///...

            // return true to start the capture, or false to cancel it
            return true;
        }

    virtual bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount)
        {
            // do something useful with the new chunk of samples
            //...

            // return true to continue the capture, or false to stop it
            return true;
        }

    virtual void onStop() // optional
        {
            // clean up whatever has to be done after the capture is finished
            //...
        }
};
