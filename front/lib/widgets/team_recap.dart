import 'package:flutter/material.dart';
import 'package:zappy/models/player.dart';
import 'package:zappy/models/team.dart';

class TeamRecap extends StatelessWidget {
  final List<Team> teams;
  final void Function(Player? player) onPlayerTap;
  final VoidCallback onCancelTap;

  const TeamRecap({
    required this.teams,
    required this.onPlayerTap,
    required this.onCancelTap,
    super.key,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        for (Team team in teams)
          Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Row(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Container(
                    width: 20,
                    height: 20,
                    color: team.teamColor,
                  ),
                  const SizedBox(width: 10),
                  Text(
                    team.teamName,
                    style: const TextStyle(color: Colors.white),
                  ),
                ],
              ),
              const SizedBox(height: 10),
              for (int i = 0; i < team.players!.length; i++)
                InkWell(
                  onTap: () => onPlayerTap(team.players![i]),
                  child: Row(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      Container(
                        width: 10,
                        height: 10,
                        color: team.teamColor,
                      ),
                      const SizedBox(width: 10),
                      Text(
                        "${team.players![i].id.toString()} lvl${team.players![i].level}",
                        style: const TextStyle(color: Colors.white),
                      ),
                    ],
                  ),
                ),
            ],
          ),
        const SizedBox(height: 20),
        FloatingActionButton(
          mini: true,
          onPressed: onCancelTap,
          backgroundColor: Colors.red,
          child: const Icon(Icons.flip_camera_android_rounded, size: 12),
        ),
      ],
    );
  }
}
