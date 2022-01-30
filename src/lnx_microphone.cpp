#include <stdio.h>
#include <stdlib.h>

#include <alsa/asoundlib.h>

snd_pcm_t* open_mic() {
    snd_pcm_t * mic_handle;
    int err;

    if((err = snd_pcm_open(&mic_handle, "default", SND_PCM_STREAM_CAPTURE, 0)) < 0 ) {
        fprintf(stderr, "Cannot open microphone. (%s)\n", snd_strerror(err));
        exit(1);
    }

    snd_pcm_hw_params_t *hw_params;

    // ALlocate structure
    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
                snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params allocated\n");
                 
    if ((err = snd_pcm_hw_params_any (mic_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
             snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params initialized\n");
    
   
    
    if ((err = snd_pcm_hw_params_set_format (mic_handle, hw_params, SND_PCM_FORMAT_U8)) < 0) {
        fprintf (stderr, "cannot set sample format (%s)\n",
             snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params format setted\n");
    
    if ((err = snd_pcm_hw_params_set_rate (mic_handle, hw_params, 8000, 0)) < 0) {
        fprintf (stderr, "cannot set sample rate (%s)\n",
             snd_strerror (err));
        exit (1);
    }
    
    fprintf(stdout, "hw_params rate setted\n");

    if ((err = snd_pcm_hw_params_set_channels (mic_handle, hw_params, 1)) < 0) {
        fprintf (stderr, "cannot set channel count (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    /* Set period size to 32 frames. */
    unsigned long frames = 32;
    int dir;

    snd_pcm_hw_params_set_period_size_near(mic_handle,
                              hw_params, &frames, &dir);

   
    fprintf(stdout, "hw_params channels setted\n");

    if ((err = snd_pcm_hw_params_set_access (mic_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf (stderr, "cannot set access type (%s)\n",
             snd_strerror (err));
        exit (1);
    }
    fprintf(stdout, "hw_params access setted\n");

    // Write the entire configuraion to the device.
    if ((err = snd_pcm_hw_params (mic_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot set parameters (%s)\n",
             snd_strerror (err));
    exit (1);
    }

    fprintf(stdout, "hw_params setted\n");

   
   
    
    // A period is the number of frames between each hardware interrupt.
    // A buffer is ring buffer of periods. This is usually 2 * period size or 8 * period size.
    // ALSA runtime will decide the actual buffer size and period size.
    printf("Size of the period in frames=%ld,Dir=%d\n", frames, dir);
    snd_pcm_hw_params_get_period_size(hw_params,
                                      &frames, &dir);

    unsigned int period_time;
    snd_pcm_hw_params_get_period_time(hw_params,
                                    &period_time, &dir);
    printf("PeriodTime=%d frames\n", period_time);

    printf("Number of periods in 5sec=%d\n", 5000000/period_time);

    snd_pcm_hw_params_free (hw_params);

    fprintf(stdout, "hw_params freed\n");
    
    return mic_handle;
}



int main(int argc, char* argv[]) {
    char *fname;
    char *endptr;
    int period;

    printf("argc=%d\n", argc);

    if(argc == 2) {
        fname = argv[1];
        period = 3;
    }
    else if(argc == 3) {
        fname = argv[1];
        period = strtol(argv[2], &endptr, 0);
    }
    else {
        printf("Invalid arguments specified.");
        printf("Usage: %s out_fname [time_period_sec]", argv[0]);
        return -1;
    }

    printf("\n>>Storing microphone audio to fname=%s for a period=%0d sec\n", 
           fname, period);


    snd_pcm_t * mic_handle = open_mic();
    

    int err;

    // Prepare the device for use.
    if ((err = snd_pcm_prepare (mic_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
                snd_strerror (err));
        exit (1);
    }


    unsigned long frames = 1;
    int size = 128;
    char * buffer = (char*) malloc(size);

    int rc;
    FILE* fptr = fopen(fname, "wb");
    if(fptr == NULL) {
        printf("Could not create the file %s. Error %s\n", fname, strerror(errno));
        exit(-1);
    }
    for(int i = 0; i < 312; i++) {
        rc = snd_pcm_readi(mic_handle, buffer, 128);
        if (rc == EPIPE) {
            printf("Overrun\n");
            snd_pcm_prepare(mic_handle);
        }
        else if(rc < 0) {
            fprintf(stderr,
                    "error from read: %s\n",
                    snd_strerror(rc));
    
        } else if (rc != 128) {
            fprintf(stderr, "short read, read %d frames\n", rc);
        }
    
        fwrite(buffer, rc, 1, fptr);
    }

    fclose(fptr);

    snd_pcm_drain(mic_handle);
    snd_pcm_close (mic_handle);
  
    free(buffer);

    fprintf(stdout, "audio interface closed\n");
    return 0;
}