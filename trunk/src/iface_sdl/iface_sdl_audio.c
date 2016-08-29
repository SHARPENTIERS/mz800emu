/* 
 * File:   iface_sdl_audio.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. července 2015, 18:29
 */

#include "mz800emu_cfg.h"

#include <stdio.h>
#include <stdint.h>

#include <string.h>
#include <SDL.h>

#include "iface_sdl_audio.h"
#include "mz800.h"
#include "audio.h"


#ifdef LINUX
#define USE_LINUX_AUDIO_BUGFIX
/* V Linuxu nejsou volani do audio_sdl_cb() periodicky presna.
 * Nektera volani pro plneni bufferu jdou prilis rychle po sobe.
 * Tohle je nejvice viditelne napr. po ALT+P, kdy nekolik snimku probehne zrychlene.
 * Proto se pokusime alespon o pribliznou synchronizaci pomoci usleep() 
 */
#define USE_USLEEP_SYNC 
#endif

#ifdef USE_USLEEP_SYNC
#include <time.h>
#include <unistd.h>
static struct timespec g_cycle_start;
//#define MIN_NS_PER_CYCLE     20000000    /* realna delka je 20ms, tzn. 20 000 000 ns */
#define MIN_NS_PER_CYCLE     17000000    /* realna delka je 20ms, tzn. 20 000 000 ns */
#endif

typedef enum en_AUBUFSTATE {
    AUBUFSTATE_INPROGRES = 0,
    AUBUFSTATE_DONE
} en_AUBUFSTATE;

static volatile en_AUBUFSTATE g_aubufstate;

typedef enum en_AUCBSTATE {
    AUCBSTATE_PLAYING = 0,
    AUCBSTATE_DONE
} en_AUCBSTATE;

static volatile en_AUCBSTATE g_aucbstate;

SDL_AudioDeviceID dev;

void audio_sdl_cb ( void *userdata, Uint8 *stream, int len ) {

    if ( g_mz800.use_max_emulation_speed == 0 ) {
        while ( ! ( ( g_aubufstate == AUBUFSTATE_DONE ) && ( g_aucbstate == AUCBSTATE_PLAYING ) ) ) {
        };
    };

    memcpy ( stream, g_audio.buffer, len );

    g_aucbstate = AUCBSTATE_DONE;
}

void iface_sdl_audio_init ( void ) {

#ifdef MZ800EMU_CFG_AUDIO_DISABLED
    return;
#endif

    if ( SDL_InitSubSystem ( SDL_INIT_AUDIO ) ) {
        fprintf ( stderr, "%s():%d - Can not initialise audio subsystem: %s\n", __func__, __LINE__, SDL_GetError ( ) );
    };

#if 1
    unsigned i;
    for ( i = 0; i < SDL_GetNumAudioDrivers ( ); i++ ) {
        const char *driver_name = SDL_GetAudioDriver ( i );

        if ( SDL_AudioInit ( driver_name ) ) {
            fprintf ( stderr, "%s():%d - Audio driver failed to initialize '%s': %s\n", __func__, __LINE__, driver_name, SDL_GetError ( ) );
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

#ifdef USE_LINUX_AUDIO_BUGFIX
    int use_bugfix = 0;
#endif

    if ( dev == 0 ) {
        fprintf ( stderr, "%s():%d - Failed to open audio: %s\n", __func__, __LINE__, SDL_GetError ( ) );
    } else {

#ifdef USE_LINUX_AUDIO_BUGFIX
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
            fprintf ( stderr, "%s():%d - Failed to open audio: %s\n", __func__, __LINE__, SDL_GetError ( ) );
        } else {
#ifdef USE_LINUX_AUDIO_BUGFIX
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

#ifdef MZ800EMU_CFG_AUDIO_DISABLED
    return;
#endif
    
    /* pokus o korektni ukonceni CB */
    g_aubufstate = AUBUFSTATE_DONE;
    g_aucbstate = AUCBSTATE_PLAYING;
    SDL_PauseAudioDevice ( dev, 1 );
}

void audio_sdl_start_cycle ( void ) {
    g_aubufstate = AUBUFSTATE_INPROGRES;
    g_aucbstate = AUCBSTATE_PLAYING;
    SDL_PauseAudioDevice ( dev, 0 );
#ifdef USE_USLEEP_SYNC
    clock_gettime ( CLOCK_MONOTONIC, &g_cycle_start );
#endif

}

void audio_sdl_wait_to_cycle_done ( void ) {

    g_aubufstate = AUBUFSTATE_DONE;

    if ( g_mz800.use_max_emulation_speed == 0 ) {
        while ( g_aucbstate == AUCBSTATE_PLAYING ) {
        };

#ifdef USE_USLEEP_SYNC
        struct timespec t;
        long int dist_ns = 0;

        clock_gettime ( CLOCK_MONOTONIC, &t );
        int sec_dist = t.tv_sec - g_cycle_start.tv_sec;
        if ( sec_dist <= 1 ) {
            if ( sec_dist == 1 ) {
                dist_ns = ( 1000000000 - g_cycle_start.tv_nsec ) + t.tv_nsec;
            } else if ( sec_dist == 0 ) {
                dist_ns = t.tv_nsec - g_cycle_start.tv_nsec;
            };
        };

        //printf ( "DIST: %d", dist_ns );
        if ( dist_ns < MIN_NS_PER_CYCLE ) {
            unsigned int usecs = ( MIN_NS_PER_CYCLE - dist_ns ) / 1000;
            usleep ( usecs );
            //printf ( "\tUSLEEP: %d", usecs );
        };
        //printf ( "\n" );
        clock_gettime ( CLOCK_MONOTONIC, &g_cycle_start );
#endif
    };

    g_aubufstate = AUBUFSTATE_INPROGRES;
    g_aucbstate = AUCBSTATE_PLAYING;

}
