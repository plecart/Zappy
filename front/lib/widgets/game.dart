import 'package:flutter/material.dart';
import 'package:zappy/models/camera.dart';
import 'package:zappy/models/egg.dart';
import 'package:zappy/models/ghost_player.dart';
import 'package:zappy/models/player.dart';
import 'package:zappy/models/team.dart';
import 'package:zappy/models/tile.dart';
import 'package:zappy/utils.dart';
import 'package:zappy/widgets/end_widget.dart';
import 'package:zappy/widgets/game_map.dart';
import 'package:zappy/widgets/player/ghost_player_widget.dart';
import 'package:zappy/widgets/player/player_widget.dart';

class Game extends StatefulWidget {
  const Game({
    required this.mapData,
    required this.camera,
    required this.gridWidth,
    required this.gridHeight,
    required this.ghostPlayers,
    required this.teams,
    required this.eggs,
    required this.onTileTap,
    required this.winner,
    required this.adjustedCellSize,
    required this.isLost,
    required this.isRessourcesInt,
    super.key,
  });

  final List<List<Tile>>? mapData;
  final Camera camera;
  final int gridWidth;
  final int gridHeight;
  final List<GhostPlayer> ghostPlayers;
  final List<Team> teams;
  final List<Egg> eggs;
  final void Function(Tile) onTileTap;
  final Team? winner;
  final double adjustedCellSize;
  final bool isLost;
  final bool isRessourcesInt;

  @override
  State<Game> createState() => _GameState();
}

class _GameState extends State<Game> {
  double _panUpdateDx = 0;
  double _panUpdateDy = 0;
  bool seeInventory = false;
  Player? playerInventory;

  @override
  Widget build(BuildContext context) {
    double screenWidth = MediaQuery.of(context).size.width;
    double screenHeight = MediaQuery.of(context).size.height;
    double cameraX = widget.camera.isFollowingPlayer
        ? -((widget.camera.player?.x ?? 0) * widget.adjustedCellSize) +
            (screenWidth / 2 - widget.adjustedCellSize / 2) -
            200
        : -(widget.gridWidth * widget.adjustedCellSize) / 2 +
            screenWidth / 2 -
            200 +
            _panUpdateDx;
    double cameraY = widget.camera.isFollowingPlayer
        ? -((widget.camera.player?.y ?? 0) * widget.adjustedCellSize) +
            (screenHeight / 2 - widget.adjustedCellSize / 2)
        : -(widget.gridHeight * widget.adjustedCellSize) / 2 +
            screenHeight / 2 +
            _panUpdateDy;

    return Listener(
      onPointerMove: (details) {
        setState(() {
          _panUpdateDx += details.delta.dx;
          _panUpdateDy += details.delta.dy;
        });
      },
      child: Stack(
        children: [
          if (widget.mapData != null)
            Transform.translate(
              offset: Offset(cameraX, cameraY),
              child: Stack(
                children: [
                  const SizedBox(
                      width: double.infinity, height: double.infinity),
                  GameMap(
                    mapData: widget.mapData!,
                    cellSize: widget.adjustedCellSize,
                    onTileTap: widget.onTileTap,
                    eggs: widget.eggs,
                    isRessourceInt: widget.isRessourcesInt,
                  ),
                  for (GhostPlayer ghostPlayer in widget.ghostPlayers)
                    GhostPlayerWidget(
                      cellSize: widget.adjustedCellSize,
                      ghostPlayer: ghostPlayer,
                    ),
                  for (Team team in widget.teams)
                    for (Player player in team.players ?? [])
                      PlayerWidget(
                        team: team,
                        cellSize: widget.adjustedCellSize,
                        player: player,
                        onPlayerTap: (player) => setState(() {
                          seeInventory = true;
                          playerInventory = player;
                        }),
                      ),
                ],
              ),
            ),
          if (widget.winner != null || widget.isLost)
            EndWidget(winner: widget.winner, isLost: widget.isLost),
          if (seeInventory && playerInventory != null)
            Positioned(
              top: 0,
              left: 0,
              child: InkWell(
                onTap: () => setState(() {
                  seeInventory = false;
                  playerInventory = null;
                }),
                child: Container(
                  padding: const EdgeInsets.all(10),
                  decoration: BoxDecoration(
                    color: Colors.white,
                    border: Border.all(color: Colors.black),
                  ),
                  child: Column(
                    children: [
                      Text("Player ${playerInventory?.id}",
                          style: const TextStyle(
                            color: Colors.black,
                            fontWeight: FontWeight.bold,
                          )),
                      const SizedBox(height: 10),
                      for (int i = 0;
                          i <
                              (playerInventory?.inventory.ressources.length ??
                                  0);
                          i++)
                        Column(
                          children: [
                            Icon(
                              getResourceIcon(i),
                              color: getResourceColor(i),
                            ),
                            Text("${playerInventory?.inventory.ressources[i]}",
                                style: const TextStyle(color: Colors.black)),
                          ],
                        ),
                    ],
                  ),
                ),
              ),
            ),
        ],
      ),
    );
  }
}
