import 'package:flutter/material.dart';
import 'package:zappy/widgets/audio/music.dart';

class ControlButtons extends StatelessWidget {
  final VoidCallback onZoomIn;
  final VoidCallback onZoomOut;
  final VoidCallback onHideMessages;

  const ControlButtons({
    required this.onZoomIn,
    required this.onZoomOut,
    required this.onHideMessages,
    super.key,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        FloatingActionButton(
            mini: true,
            onPressed: onZoomIn,
            backgroundColor: Colors.green,
            child: const Icon(Icons.zoom_in, size: 12)),
        const SizedBox(height: 20),
        FloatingActionButton(
            mini: true,
            onPressed: onZoomOut,
            backgroundColor: Colors.red,
            child: const Icon(Icons.zoom_out, size: 12)),
        const SizedBox(height: 20),
        FloatingActionButton(
            mini: true,
            onPressed: onHideMessages,
            child: const Icon(Icons.message, size: 12)),
        const SizedBox(height: 20),
        const BackgroundMusicScreen(),
      ],
    );
  }
}
