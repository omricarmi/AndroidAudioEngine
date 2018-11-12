Android audio history
===
** work in progress **

A list of important audio features, bugs, fixes and workarounds for various Android versions. 

### 9.0 Pie - API 28 (August 6, 2018)
- AAudio adds support for setUsage(), setSessionId(), setContentType(), setInputPreset() for builders.
- Regression bug: [AAudio] Headphone disconnect event not fired for MMAP streams. https://github.com/google/oboe/issues/252

### 8.1 Oreo MR1 - API 27 
- Oboe uses AAudio by default.
- AAudio MMAP data path enabled on Pixel devices. PerformanceMode::Exclusive supported.
- Fixed: [AAudio] RefBase issue

### 8.0 Oreo - API 26 (August 21, 2017)
- [AAudio API introduced](https://developer.android.com/ndk/guides/audio/aaudio/aaudio)
- Bug: RefBase issue causes crash after stream closed. This why AAudio is not recommended for 8.0.
  https://github.com/google/oboe/issues/40

### 7.1 Nougat MR1 - API 25
- OpenSL adds supports for setting and querying of PerformanceMode.

### 7.0 Nougat - API 24

### 6.1 Marshmallow MR1 - API 24 (Don?)
- OpenSL method `acquireJavaProxy` added, which allows the Java AudioTrack object associated with playback to be obtained (which allows underrun count).

### 6.0 Marshmallow - API 23 (October 5, 2015)
- Floating point recording supported.
- [MIDI API introduced](https://developer.android.com/reference/android/media/midi/package-summary)

### 5.0 Lollipop - API 21 (November 12, 2014)
- Floating point playback supported.




