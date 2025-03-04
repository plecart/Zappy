import 'package:flutter/material.dart';
import 'package:zappy/widgets/audio/music.dart';

class ControlButtons extends StatefulWidget {
  final VoidCallback onZoomIn;
  final VoidCallback onZoomOut;
  final VoidCallback onHideMessages;
  final VoidCallback onPlayTap;

  const ControlButtons({
    required this.onZoomIn,
    required this.onZoomOut,
    required this.onHideMessages,
    required this.onPlayTap,
    super.key,
  });

  @override
  State<ControlButtons> createState() => _ControlButtonsState();
}

class _ControlButtonsState extends State<ControlButtons> {
  bool _isPlaying = false;

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        FloatingActionButton(
            mini: true,
            onPressed: widget.onZoomIn,
            backgroundColor: Colors.green,
            child: const Icon(Icons.zoom_in, size: 12)),
        const SizedBox(height: 20),
        FloatingActionButton(
            mini: true,
            onPressed: widget.onZoomOut,
            backgroundColor: Colors.red,
            child: const Icon(Icons.zoom_out, size: 12)),
        const SizedBox(height: 20),
        FloatingActionButton(
            mini: true,
            onPressed: widget.onHideMessages,
            child: const Icon(Icons.message, size: 12)),
        const SizedBox(height: 20),
        const BackgroundMusicScreen(),
        const SizedBox(height: 20),
        if (!_isPlaying)
          FloatingActionButton(
              onPressed: () {
                setState(() {
                  _isPlaying = true;
                });
                widget.onPlayTap();
              },
              child: const Icon(Icons.pause, size: 30))
      ],
    );
  }
}
