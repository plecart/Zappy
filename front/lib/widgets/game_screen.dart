import 'package:flutter/material.dart';
import 'package:zappy/constants/constants.dart';
import 'package:zappy/models/camera.dart';
import 'package:zappy/models/egg.dart';
import 'package:zappy/models/ghost_player.dart';
import 'package:zappy/models/inventory.dart';
import 'package:zappy/models/message.dart';
import 'package:zappy/models/player.dart';
import 'package:zappy/models/team.dart';
import 'package:zappy/models/tile.dart';
import 'package:zappy/tcp_client.dart';
import 'package:zappy/widgets/control_buttons.dart';
import 'package:zappy/widgets/game.dart';
import 'package:zappy/widgets/message_recap.dart';
import 'package:zappy/widgets/team_recap.dart';
import 'package:zappy/widgets/tile_recap.dart';

class GameScreen extends StatefulWidget {
  const GameScreen({super.key});

  @override
  GameScreenState createState() => GameScreenState();
}

class GameScreenState extends State<GameScreen> {
  List<List<Tile>>? mapData;
  int gridWidth = 0;
  int gridHeight = 0;
  double zoomLevel = 1.0;
  double cellSize = 50.0;
  List<Team> teams = [];
  List<Egg> eggs = [];
  List<GhostPlayer> ghostPlayers = [];
  Camera camera = Camera();
  bool isTileSelected = false;
  Tile? selectedTile;
  Team? winner;
  double get adjustedCellSize => cellSize * zoomLevel;
  List<Message> listMessage = [];
  bool hideMessages = false;
  bool isLost = false;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.blueGrey[900],
      body: Row(
        children: [
          SizedBox(
            width: 200,
            child: Padding(
              padding: const EdgeInsets.all(4),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  ControlButtons(
                    onZoomIn: () => setState(
                        () => zoomLevel = (zoomLevel * 1.05).clamp(0.01, 2.0)),
                    onZoomOut: () => setState(
                        () => zoomLevel = (zoomLevel / 1.05).clamp(0.01, 2.0)),
                    onHideMessages: () =>
                        setState(() => hideMessages = !hideMessages),
                    onPlayTap: () => connectWebSocket(handleServerMessage),
                  ),
                  const SizedBox(height: 20),
                  TeamRecap(
                      teams: teams,
                      onPlayerTap: setCameraFollowing,
                      onCancelTap: setCameraFree),
                ],
              ),
            ),
          ),
          Expanded(
            child: Game(
              mapData: mapData,
              camera: camera,
              gridWidth: gridWidth,
              gridHeight: gridHeight,
              ghostPlayers: ghostPlayers,
              teams: teams,
              eggs: eggs,
              onTileTap: onTileTap,
              winner: winner,
              adjustedCellSize: adjustedCellSize,
              isLost: isLost,
            ),
          ),
          SizedBox(
            width: 200,
            child: Column(
              children: [
                if (isTileSelected)
                  Flexible(
                    child: TileRecap(
                        selectedTile: selectedTile, teams: teams, eggs: eggs),
                  ),
                const SizedBox(height: 20),
                if (!hideMessages)
                  Flexible(child: MessageRecap(messages: listMessage)),
              ],
            ),
          )
        ],
      ),
    );
  }

  void handleServerMessage(String fullMessage) {
    List<String> lines = fullMessage.trim().split("\n");

    for (String line in lines) {
      List<String> parts = line.split(" ");
      if (parts.isEmpty) continue;

      if (parts[0] == "msz") {
        gridWidth = int.parse(parts[1]);
        gridHeight = int.parse(parts[2]);

        mapData = List.generate(
          gridWidth,
          (x) => List.generate(
            gridHeight,
            (y) => Tile(x, y, [0, 0, 0, 0, 0, 0, 0], 0),
          ),
        );
      } else if (parts[0] == "bct" && mapData != null) {
        int x = int.parse(parts[1]);
        int y = int.parse(parts[2]);
        List<int> resources = parts.sublist(3).map(int.parse).toList();
        if (mapData?[x][y] != null) {
          setState(() {
            mapData?[x][y].resources = resources;
          });
        } else {
          mapData?[x][y] = Tile(x, y, resources, 0);
        }
      } else if (parts[0] == "tna") {
        teams.add(Team(parts[1], [], Colors.primaries[teams.length]));
      } else if (parts[0] == "pnw") {
        int id = int.parse(parts[1].substring(1));
        int x = int.parse(parts[2]);
        int y = int.parse(parts[3]);
        int direction = int.parse(parts[4]);
        int level = int.parse(parts[5]);
        String teamName = parts[6];
        Player newPlayer = Player(x, y, id, level, direction, Inventory([]));

        for (Team team in teams) {
          if (team.teamName == teamName) {
            team.players?.add(newPlayer);
            break;
          }
        }
      } else if (parts[0] == "ppo") {
        int playerId = int.parse(parts[1].substring(1));
        int newX = int.parse(parts[2]);
        int newY = int.parse(parts[3]);
        int newDirection = int.parse(parts[4]);

        setState(() {
          for (Team team in teams) {
            for (Player player in team.players ?? []) {
              if (player.id == playerId) {
                player.x = newX;
                player.y = newY;
                player.direction = newDirection;
                return;
              }
            }
          }
        });
      } else if (parts[0] == "pgt") {
        int playerId = int.parse(parts[1].substring(1));
        int resourceType = int.parse(parts[2]);

        setState(() {
          for (Team team in teams) {
            for (Player player in team.players ?? []) {
              if (player.id == playerId) {
                if (mapData?[player.x][player.y].resources[resourceType] == 0) {
                  return;
                }
                mapData?[player.x][player.y].resources[resourceType] -= 1;
                return;
              }
            }
          }
        });
      } else if (parts[0] == "pdi") {
        int playerId = int.parse(parts[1].substring(1));

        setState(() {
          for (Team team in teams) {
            team.players?.removeWhere((player) => player.id == playerId);
          }
        });
      } else if (parts[0] == "pdr") {
        int playerId = int.parse(parts[1].substring(1));
        int resourceType = int.parse(parts[2]);

        setState(() {
          for (Team team in teams) {
            for (Player player in team.players ?? []) {
              if (player.id == playerId) {
                mapData?[player.x][player.y].resources[resourceType] += 1;
                return;
              }
            }
          }
        });
      } else if (parts[0] == "pbc") {
        int playerId = int.parse(parts[1].substring(1));
        String message = parts.sublist(2).join(" ");

        setState(() {
          for (Team team in teams) {
            for (Player player in team.players ?? []) {
              if (player.id == playerId) {
                listMessage.add(Message(playerId, player.x, player.y, message));
              }
            }
          }
        });
      } else if (parts[0] == "seg") {
        if (parts.length < 2) {
          setState(() {
            isLost = true;
          });
          return;
        }
        String teamName = parts[1];
        for (Team team in teams) {
          if (team.teamName == teamName) {
            setState(() {
              winner = team;
            });
            return;
          }
        }
      } else if (parts[0] == "smg") {
        String message = parts[1];
        print("Server message: $message");
      } else if (parts[0] == "pfk") {
        int playerId = int.parse(parts[1].substring(1));
        setState(() {
          for (Team team in teams) {
            for (Player player in team.players ?? []) {
              if (player.id == playerId) {
                listMessage.add(Message(playerId, player.x, player.y, "🥚"));
                return;
              }
            }
          }
        });
      } else if (parts[0] == "enw") {
        int eggId = int.parse(parts[1].substring(1));
        int playerId = int.parse(parts[2].substring(1));
        int x = int.parse(parts[3]);
        int y = int.parse(parts[4]);
        eggs.add(Egg(eggId, x, y));
        setState(() {
          for (Team team in teams) {
            for (Player player in team.players ?? []) {
              if (player.id == playerId) {
                listMessage
                    .add(Message(playerId, player.x, player.y, "🥚 Done"));
                return;
              }
            }
          }
        });
      } else if (parts[0] == "eht") {
        int eggId = int.parse(parts[1].substring(1));
        Egg egg = eggs.firstWhere((egg) => egg.id == eggId);
        eggs.removeWhere((egg) => egg.id == eggId);
        ghostPlayers.add(GhostPlayer(egg.x, egg.y, egg.id));
      } else if (parts[0] == "edi") {
        int eggId = int.parse(parts[1].substring(1));
        eggs.removeWhere((egg) => egg.id == eggId);
      } else if (parts[0] == "ebo") {
        int eggId = int.parse(parts[1].substring(1));
        ghostPlayers.removeWhere((ghostPlayer) => ghostPlayer.id == eggId);
      } else if (parts[0] == "pin") {
        int playerId = int.parse(parts[1].substring(1));
        int resources = int.parse(parts[4]);
        int linemate = int.parse(parts[5]);
        int deraumere = int.parse(parts[6]);
        int sibur = int.parse(parts[7]);
        int mendiane = int.parse(parts[8]);
        int phiras = int.parse(parts[9]);
        int thystame = int.parse(parts[10]);

        setState(() {
          for (Team team in teams) {
            for (Player player in team.players ?? []) {
              if (player.id == playerId) {
                player.inventory = Inventory([
                  resources,
                  linemate,
                  deraumere,
                  sibur,
                  mendiane,
                  phiras,
                  thystame,
                ]);
                return;
              }
            }
          }
        });
      } else if (parts[0] == "pic") {
        int x = int.parse(parts[1]);
        int y = int.parse(parts[2]);
        int level = int.parse(parts[3]);
        List<int> playerIds = parts
            .sublist(4)
            .map((part) => int.parse(part.substring(1)))
            .toList();

        setState(() {
          mapData?[x][y].nbIncantation += 1;
          for (Team team in teams) {
            for (Player player in team.players ?? []) {
              if (player.x == x && player.y == y) {
                listMessage.add(Message(player.id, player.x, player.y,
                    "Incantation $level, with ${playerIds.length} other players"));
              }
            }
          }
        });
      } else if (parts[0] == "pie") {
        int x = int.parse(parts[1]);
        int y = int.parse(parts[2]);
        int result = int.parse(parts[3]);

        mapData?[x][y].nbIncantation -= 1;
        if (result == 1) {
          listMessage.add(Message(0, x, y, "Incantation success"));
        } else {
          listMessage.add(Message(0, x, y, "Incantation failed"));
        }
      } else if (parts[0] == "plv") {
        int playerId = int.parse(parts[1].substring(1));
        int level = int.parse(parts[2]);

        setState(() {
          for (Team team in teams) {
            for (Player player in team.players ?? []) {
              if (player.id == playerId) {
                player.level = level;
                return;
              }
            }
          }
        });
      }
    }
  }

  void simulateServerMessages() async {
    for (String message in messages) {
      await Future.delayed(const Duration(milliseconds: 30));
      handleServerMessage(message);
    }
  }

  void setCameraFollowing(Player? player) {
    setState(() {
      camera.isFollowingPlayer = true;
      camera.player = player;
    });
  }

  void setCameraFree() {
    setState(() {
      camera.isFollowingPlayer = false;
    });
  }

  void onTileTap(Tile tile) {
    setState(() {
      isTileSelected = selectedTile != tile;
      selectedTile = selectedTile != tile ? tile : null;
    });
  }
}
