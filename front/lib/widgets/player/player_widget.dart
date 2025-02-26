import 'package:flutter/material.dart';
import 'package:zappy/models/player.dart';
import 'package:zappy/models/team.dart';

class PlayerWidget extends StatelessWidget {
  final double cellSize;
  final Player player;
  final Team team;
  final void Function(Player? player) onPlayerTap;

  const PlayerWidget({
    required this.cellSize,
    required this.player,
    required this.team,
    required this.onPlayerTap,
    super.key,
  });

  @override
  Widget build(BuildContext context) {
    return AnimatedPositioned(
      duration: const Duration(milliseconds: 200),
      left: player.x * cellSize,
      top: player.y * cellSize,
      child: SizedBox(
        width: cellSize,
        height: cellSize,
        child: Column(
          children: [
            Container(
              decoration: BoxDecoration(
                color: Colors.black.withOpacity(0.5),
              ),
              child: Text(
                player.id.toString(),
                style: TextStyle(
                  color: Colors.white,
                  fontSize: cellSize / 6,
                ),
                overflow: TextOverflow.ellipsis,
              ),
            ),
            InkWell(
              onTap: () => onPlayerTap(player),
              child: RotatedBox(
                quarterTurns: player.direction,
                child: ClipPath(
                  clipper: PlayerClipper(),
                  child: Container(
                    width: cellSize / 2,
                    height: cellSize / 2,
                    decoration: BoxDecoration(
                      color: team.teamColor,
                      shape: BoxShape.circle,
                      border: Border.all(color: Colors.black, width: 1),
                    ),
                    child: Center(
                      child:
                          Text("👾", style: TextStyle(fontSize: cellSize / 5)),
                    ),
                  ),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class PlayerClipper extends CustomClipper<Path> {
  @override
  Path getClip(Size size) {
    Path path = Path();
    path.moveTo(size.width / 2, 0);
    path.lineTo(size.width, size.height);
    path.lineTo(0, size.height);
    path.close();
    return path;
  }

  @override
  bool shouldReclip(covariant CustomClipper<Path> oldClipper) {
    return false;
  }
}
