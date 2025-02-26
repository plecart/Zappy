import 'package:flutter/material.dart';
import 'package:zappy/models/ghost_player.dart';

class GhostPlayerWidget extends StatelessWidget {
  final double cellSize;
  final GhostPlayer ghostPlayer;

  const GhostPlayerWidget({
    required this.cellSize,
    required this.ghostPlayer,
    super.key,
  });

  @override
  Widget build(BuildContext context) {
    return Positioned(
      left: ghostPlayer.x * cellSize + cellSize / 4,
      top: ghostPlayer.y * cellSize + cellSize / 4,
      child: Container(
        width: cellSize / 2,
        height: cellSize / 2,
        decoration: BoxDecoration(
          color: Colors.grey,
          shape: BoxShape.circle,
          border: Border.all(color: Colors.black, width: 1),
        ),
        child: Center(
          child: Text("👻", style: TextStyle(fontSize: cellSize / 5)),
        ),
      ),
    );
  }
}
