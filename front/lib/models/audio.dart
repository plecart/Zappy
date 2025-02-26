import 'package:just_audio/just_audio.dart';

class AudioSound {
  final String name;
  final String path;
  final AudioPlayer player = AudioPlayer();
  double _lastVolume;
  bool isLooping;

  AudioSound(this.name, this.path,
      {double lastVolume = 1.0, this.isLooping = false})
      : _lastVolume = lastVolume {
    _init();
  }

  Future<void> _init() async {
    await player.setAsset(path);
    await player.setLoopMode(isLooping ? LoopMode.one : LoopMode.off);
    await player.setVolume(_lastVolume);
  }

  set lastVolume(double value) {
    _lastVolume = value;
  }

  void toggle() {
    if (player.playing) {
      player.pause();
    } else {
      player.play();
    }
  }

  void mute() {
    _lastVolume = player.volume;
    player.setVolume(0);
  }

  void unmute() {
    player.setVolume(_lastVolume);
  }
}

class AllAudioSounds {
  final List<AudioSound> allSounds;
  bool isPlaying;

  AllAudioSounds(this.allSounds, {this.isPlaying = false});

  void toggle() {
    isPlaying = !isPlaying;
    for (var sound in allSounds) {
      sound.toggle();
    }
  }

  void mute() {
    for (var sound in allSounds) {
      sound.mute();
    }
  }

  void unmute() {
    for (var sound in allSounds) {
      sound.unmute();
    }
  }
}
