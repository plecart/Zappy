import 'package:flutter/material.dart';
import 'package:zappy/constants/constants.dart';
import 'package:zappy/widgets/audio/volume_slider.dart';

class BackgroundMusicScreen extends StatefulWidget {
  const BackgroundMusicScreen({super.key});

  @override
  State<BackgroundMusicScreen> createState() => _BackgroundMusicScreenState();
}

class _BackgroundMusicScreenState extends State<BackgroundMusicScreen> {
  bool _isPlaying = false;

  @override
  void initState() {
    super.initState();
    _isPlaying = allAudioSounds.isPlaying;
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        FloatingActionButton(
          onPressed: () {
            setState(() {
              _isPlaying = !_isPlaying;
            });
            backgroundMusic.toggle();
            _isPlaying ? allAudioSounds.unmute() : allAudioSounds.mute();
          },
          mini: true,
          child: Icon(_isPlaying ? Icons.music_off_sharp : Icons.music_note,
              size: 12),
        ),
        const SizedBox(height: 20),
        FloatingActionButton(
            onPressed: () => showDialog(
                  context: context,
                  builder: (context) => AlertDialog(
                    title: const Text('Volume'),
                    content: Column(
                      mainAxisSize: MainAxisSize.min,
                      children: [
                        VolumeSlider(
                            volume: backgroundMusic.player.volume,
                            onChanged: (value) {
                              backgroundMusic.player.setVolume(value);
                            }),
                        ElevatedButton(
                          onPressed: () {
                            Navigator.of(context).pop();
                          },
                          child: const Text('OK'),
                        ),
                      ],
                    ),
                  ),
                ),
            mini: true,
            child: const Icon(Icons.menu, size: 12)),
      ],
    );
  }
}
