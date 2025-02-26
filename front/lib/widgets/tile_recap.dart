import 'package:flutter/material.dart';
import 'package:zappy/models/egg.dart';
import 'package:zappy/models/team.dart';
import 'package:zappy/models/tile.dart';
import 'package:zappy/utils.dart';

class TileRecap extends StatelessWidget {
  const TileRecap({
    required this.selectedTile,
    required this.teams,
    required this.eggs,
    super.key,
  });

  final Tile? selectedTile;
  final List<Team> teams;
  final List<Egg> eggs;

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(10),
      decoration: BoxDecoration(
        color: Colors.white,
        border: Border.all(color: Colors.black),
      ),
      child: SingleChildScrollView(
        child: Column(
          children: [
            Text("Tile (${selectedTile!.x}, ${selectedTile!.y})",
                style: const TextStyle(
                  color: Colors.black,
                  fontWeight: FontWeight.bold,
                )),
            const SizedBox(height: 10),
            for (int i = 0; i < selectedTile!.resources.length; i++)
              Column(
                children: [
                  Icon(
                    getResourceIcon(i),
                    color: getResourceColor(i),
                  ),
                  Text("${selectedTile!.resources[i]}",
                      style: const TextStyle(color: Colors.black)),
                ],
              ),
            if (selectedTile?.nbIncantation != null)
              Column(
                children: [
                  const Icon(Icons.thunderstorm_outlined, color: Colors.black),
                  Text("${selectedTile?.nbIncantation}",
                      style: const TextStyle(color: Colors.black)),
                ],
              ),
            for (Team team in teams)
              for (int i = 0; i < team.players!.length; i++)
                if (team.players![i].x == selectedTile?.x &&
                    team.players![i].y == selectedTile?.y)
                  Padding(
                    padding: const EdgeInsets.only(top: 10),
                    child: Text("Player ${team.players![i].id}",
                        style: const TextStyle(color: Colors.black)),
                  ),
            const SizedBox(height: 10),
            for (Egg egg in eggs)
              if (egg.x == selectedTile?.x && egg.y == selectedTile?.y)
                Padding(
                  padding: const EdgeInsets.only(top: 10),
                  child: Text("Egg 🥚 ${egg.id}",
                      style: const TextStyle(color: Colors.black)),
                ),
          ],
        ),
      ),
    );
  }
}
