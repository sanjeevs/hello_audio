# hello_audio
check out sound api in linux and windows.
By default the sound card is configured for
1. 8 bit PCM Samples, Mono Samples
2. 8Khz sampling rate.

## Windows
* Create win_audio_in.exe that saves audio samples from the microphone, for say 3 sec, to a file.
User specifies the output file name and the time duration of the recording.
```
win_microphone.exe  out_filename [time]
```

* Create a win_speaker.exe that takes the raw pcm file and plays it on the default output device.
```
win_speaker.exe in_filename
```

## Linux
* Create lnx_audio_in that saves microphone data, for say 3 sec, as raw pcm samples to a file.
```
lnx_microphone out_filename [time]
```

* Create lnx_audio_out that plays the raw sample file on the speaker.
```
lnx_speaker in_filename
```
