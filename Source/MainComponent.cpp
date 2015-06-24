/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "MainComponent.h"


#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include <iostream>

#define NUM_SECONDS   (2.5)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)

#define NUM_CH (2)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

using namespace std;

#define TABLE_SIZE   (200)
typedef struct
{
    float sine[TABLE_SIZE];
    int left_phase;
    int right_phase;
    char message[20];
    float pan, panL, panR, masterGain;
}
paTestData;

static int patestCallback3( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
//    paTestData *data = (paTestData*)userData;
//    float *out = (float*)outputBuffer;
//    unsigned long i;
//    
//    (void) timeInfo;
//    (void) statusFlags;
//    (void) inputBuffer;
//    
//    for( i=0; i<framesPerBuffer; i++ )
//    {
//        *out++ = (data->masterGain)*(data->panL)*data->sine[data->left_phase];  /* left */
//        *out++ = (data->masterGain)*(data->panR)*data->sine[data->right_phase];  /* right */
//        data->left_phase += 3;
//        if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
//        data->right_phase += 5; /* higher pitch so we can distinguish left and right. */
//        if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
//    }
//    return paContinue;;
    
    
    paTestData *data = (paTestData*)userData;
    float *out = (float*)outputBuffer;
    unsigned long i;
    
    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;
    
    for( i=0; i<framesPerBuffer; i++ )
    {
//        *out++ = data->sine[data->left_phase];  /* left */
//        *out++ = data->sine[data->right_phase];  /* right */
        *out++ = (data->masterGain)*(data->panL)*data->sine[data->left_phase];  /* left */
        *out++ = (data->masterGain)*(data->panR)*data->sine[data->right_phase];  /* right */
        data->left_phase += 1;
        if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
        data->right_phase += 3; /* higher pitch so we can distinguish left and right. */
        if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
    }
    
    return paContinue;
}

static void StreamFinished( void* userData )
{
	paTestData *data = (paTestData *) userData;
	printf( "Stream Completed: %s\n", data->message );
}

PaStreamParameters outputParameters;
PaStream *stream; //audio stream
PaError err; //error code
paTestData data; // holds the paTestData structure whose sine table we are initializing

// custom function for PA init
static void my_PA_Setup(){
    
    printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);
    
    int i;
    /* initialise sinusoidal wavetable */
    for( i=0; i<TABLE_SIZE; i++ )
    {
        data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. ); // cast to double for precision
    }
	
    data.left_phase = data.right_phase = 0;//initialize phase to zero
	data.panL = 0.5;
	data.panR = 0.5;
    data.masterGain = 0.5;
    
    err = Pa_Initialize();
	
    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL; //portaudio talks to native api for cross-platform, that's why host api is there, ex JACK or CoreAudio
    
}



//==============================================================================
MainContentComponent::MainContentComponent()
{
    setSize (600, 400);
    my_PA_Setup();
    
//    button1 = new TextButton ("Tone in L ch");
//    button2 = new TextButton ("Tone in R ch");
    startButton = new TextButton ("To start a tone");
    stopButton = new TextButton ("To stop a tone");
    
    slider = new Slider ("Slider");
    
    // add to component
//    addAndMakeVisible (button1);
//    addAndMakeVisible (button2);
    
    addAndMakeVisible (startButton);
    addAndMakeVisible (stopButton);
    addAndMakeVisible (slider);
    
//    button1->setBounds (100, 300, 100, 60);
//    button2->setBounds (400, 300, 100, 60);
    
    startButton->setBounds (100, 100, 100, 60);
    stopButton->setBounds (400, 100, 100, 60);
    
    slider->setBounds (150, 250, 260, 20);
    
    // to register
//    button1->addListener(this);
//    button2->addListener(this);
    startButton->addListener(this);
    stopButton->addListener(this);
    
    slider->setSliderStyle (Slider::LinearHorizontal);
    slider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    slider->setRange(-1.0, 1.0);
    slider->addListener (this);
}

MainContentComponent::~MainContentComponent()
{
//    button1 = nullptr;
//    button2 = nullptr;
    startButton = nullptr;
    stopButton = nullptr;
    slider = nullptr;
    
    //Pa_CloseStream( stream );
	Pa_Terminate();
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);
    g.setFont (Font (24.0f));
    g.setColour (Colours::yellowgreen);
    g.drawText ("Simple JUCE Stereo Panning GUI", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}


void MainContentComponent::buttonClicked (Button* button)
{
//    if (button == button1)
//    {
//        std::cout<< "button1 clicked" << std::endl;
//        
//        
//        Pa_Initialize();
//        Pa_OpenStream(
//                            &stream, // address to output stream
//                            NULL, /* no input (stream) */
//                            &outputParameters,
//                            SAMPLE_RATE,
//                            FRAMES_PER_BUFFER,
//                            paClipOff,      /* we won't output out of range samples so don't bother clipping them (paClipOff:automatically clipping) */
//                            patestCallback3,
//                            &data ); // pass the address of sine wave table
//        
//		Pa_SetStreamFinishedCallback( stream, &StreamFinished );
//		Pa_StartStream( stream );
//        
//    }
//    else if (button == button2)
//    {
//        std::cout<< "button1 clicked" << std::endl;
//        
//        
//        Pa_Initialize();
//        Pa_OpenStream(
//                            &stream, // address to output stream
//                            NULL, /* no input (stream) */
//                            &outputParameters,
//                            SAMPLE_RATE,
//                            FRAMES_PER_BUFFER,
//                            paClipOff,      /* we won't output out of range samples so don't bother clipping them (paClipOff:automatically clipping) */
//                            patestCallback3,
//                            &data ); // pass the address of sine wave table
//        
//        Pa_SetStreamFinishedCallback( stream, &StreamFinished );
//		Pa_StartStream( stream );
//        
//    }
    if (button == startButton){
        std::cout<< "startButton clicked" << std::endl;
        
        data.masterGain = 1.0;
        data.pan = 0.5;
        
        
        Pa_Initialize();
        Pa_OpenStream(
                            &stream, // address to output stream
                            NULL, /* no input (stream) */
                            &outputParameters,
                            SAMPLE_RATE,
                            FRAMES_PER_BUFFER,
                            paClipOff,      /* we won't output out of range samples so don't bother clipping them (paClipOff:automatically clipping) */
                            patestCallback3,
                            &data ); // pass the address of sine wave table
        
        Pa_SetStreamFinishedCallback( stream, &StreamFinished );
		Pa_StartStream( stream );

    }
    
    else if (button == stopButton){
        std::cout<< "stopButton clicked" << std::endl;
        
        Pa_StopStream( stream );
		Pa_CloseStream( stream );
    }
}

void MainContentComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    if (sliderThatWasMoved == slider) {
        std::cout<< "slider dragged, value = " << slider->getValue() << std::endl;
        
        //data.masterGain = slider->getValue();
        data.pan = slider->getValue();
        
        data.panL = (float)(-1.0*((data.pan - 1.0)/2.0));
        data.panR = (float)((data.pan + 1.0)/2.0);
    }
    
    //
}

