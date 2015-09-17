/* 
 * File:   iface_sdl_audio.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. července 2015, 18:29
 */

#include <stdio.h>
#include <stdint.h>

#include <string.h>
#include <SDL.h>

#include "iface_sdl_audio.h"
#include "mz800.h"
#include "audio.h"



SDL_AudioDeviceID dev;
static volatile unsigned g_callbacks_counter;


void audio_sdl_cb ( void *userdata, Uint8 *stream, int len ) {

    memcpy ( stream, g_audio.buffer, len );

    if ( g_mz800.emulation_speed == 0 ) {
        SDL_PauseAudioDevice ( dev, 1 );
    };
    g_callbacks_counter = 1;
}


void iface_sdl_audio_init ( void ) {

    if ( SDL_InitSubSystem ( SDL_INIT_AUDIO ) ) {
        fprintf ( stderr, "%s():%d - Can not initialise audio subsystem: %s\n", __FUNCTION__, __LINE__, SDL_GetError ( ) );
    };

#if 1
    unsigned i;
    for ( i = 0; i < SDL_GetNumAudioDrivers ( ); i++ ) {
        const char *driver_name = SDL_GetAudioDriver ( i );

        if ( SDL_AudioInit ( driver_name ) ) {
            fprintf ( stderr, "%s():%d - Audio driver failed to initialize '%s': %s\n", __FUNCTION__, __LINE__, driver_name, SDL_GetError ( ) );
        } else {
            fprintf ( stdout, "Audio driver OK: %s\n", driver_name );
#if 0
            count_devices = SDL_GetNumAudioDevices ( 0 );
            printf ( "\tNumAudioDevices: %d\n", count_devices );
            for ( j = 0; j < count_devices; ++j ) {
                /*SDL_Log ( "Audio device %d: %s\n", j, SDL_GetAudioDeviceName ( j, 0 ) );*/
                printf ( "\t\tAudio device %d: %s\n", j, SDL_GetAudioDeviceName ( j, 0 ) );
            };
#endif
            break;
        };
        SDL_AudioQuit ( );
    };
#endif

    SDL_AudioSpec want, have;

    SDL_zero ( want );
    want.freq = IFACE_AUDIO_SAMPLE_RATE;
    want.format = AUDIO_U16SYS;
    want.channels = IFACE_AUDIO_CHANNELS;
    want.samples = IFACE_AUDIO_20MS_SAMPLES;
    want.callback = audio_sdl_cb;

    //    dev = SDL_OpenAudioDevice ( NULL, 0, &want, &have, SDL_AUDIO_ALLOW_ANY_CHANGE );
    dev = SDL_OpenAudioDevice ( NULL, 0, &want, &have, 0 );

#ifdef LINUX
    int use_bugfix = 0;
#endif

    if ( dev == 0 ) {
        fprintf ( stderr, "%s():%d - Failed to open audio: %s\n", __FUNCTION__, __LINE__, SDL_GetError ( ) );
    } else {

#ifdef LINUX
        /* BUGFIX: zatim nevim proc, ale v linuxu mam vzdy jen 1/2 samples z toho co jsem pozadoval */
        if ( have.samples == ( want.samples / 2 ) ) {
            use_bugfix = 1;
            printf ( "Audio: try double samples bugfix...\n" );
            SDL_CloseAudioDevice ( dev );
            want.samples = IFACE_AUDIO_20MS_SAMPLES * 2;
            dev = SDL_OpenAudioDevice ( NULL, 0, &want, &have, 0 );
        };
#endif

        if ( dev == 0 ) {
            fprintf ( stderr, "%s():%d - Failed to open audio: %s\n", __FUNCTION__, __LINE__, SDL_GetError ( ) );
        } else {
#ifdef LINUX
            if ( use_bugfix == 1 ) {
                fprintf ( stdout, "Bugfix result:\n" );
                fprintf ( stdout, "Want - Frq: %d Hz, format: %d, channels: %d, silence: %d, samples: %d, size: %d\n", want.freq, want.format, want.channels, want.silence, want.samples, want.size );
                fprintf ( stdout, "Have - Frq: %d Hz, format: %d, channels: %d, silence: %d, samples: %d, size: %d\n", have.freq, have.format, have.channels, have.silence, have.samples, have.size );
            };
#endif
            /*SDL_PauseAudioDevice ( dev, 0 );*/
            audio_sdl_start_cycle ( );
        };
    };
}


void iface_sdl_audio_quit ( void ) {
    SDL_PauseAudioDevice ( dev, 1 );
}


void audio_sdl_start_cycle ( void ) {
    g_callbacks_counter = 0;
    SDL_PauseAudioDevice ( dev, 0 );
}


void audio_sdl_wait_to_cycle_done ( void ) {
    if ( g_mz800.emulation_speed == 0 ) {
        while ( 0 == g_callbacks_counter ) {
        };
        SDL_PauseAudioDevice ( dev, 0 );
        g_callbacks_counter = 0;
    };
}

