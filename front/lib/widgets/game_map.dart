import 'package:flutter/material.dart';
import 'package:zappy/models/egg.dart';
import 'package:zappy/models/tile.dart';
import 'package:zappy/utils.dart';

class GameMap extends StatelessWidget {
  final List<List<Tile>> mapData;
  final double cellSize;
  final void Function(Tile) onTileTap;
  final List<Egg> eggs;
  final bool isRessourceInt;

  const GameMap({
    required this.mapData,
    required this.cellSize,
    required this.onTileTap,
    required this.eggs,
    required this.isRessourceInt,
    super.key,
  });

  int getEggsNumber(int x, int y) {
    return eggs.where((egg) => egg.x == x && egg.y == y).length;
  }

  @override
  Widget build(BuildContext context) {
    int gridWidth = mapData.length;
    int gridHeight = mapData[0].length;

    return SizedBox(
      width: gridWidth * cellSize,
      height: gridHeight * cellSize,
      child: GridView.builder(
        physics: const NeverScrollableScrollPhysics(),
        gridDelegate: SliverGridDelegateWithFixedCrossAxisCount(
          crossAxisCount: gridWidth,
        ),
        itemCount: gridWidth * gridHeight,
        itemBuilder: (context, index) {
          int x = index % gridWidth;
          int y = index ~/ gridWidth;
          Tile tile = mapData[x][y];

          return InkWell(
            onTap: () => onTileTap(tile),
            child: Container(
              decoration: BoxDecoration(
                border: Border.all(color: Colors.white30),
                color: (x + y) % 2 == 0 ? Colors.grey[700] : Colors.grey[800],
              ),
              child: Stack(
                children: [
                  Wrap(
                    runSpacing: cellSize / 6,
                    spacing: cellSize / 6,
                    children: [
                      for (int i = 0; i < tile.resources.length; i++)
                        if (tile.resources[i] > 0 && !isRessourceInt)
                          Icon(
                            getResourceIcon(i),
                            color: getResourceColor(i),
                            size: cellSize / 6,
                          )
                        else if (tile.resources[i] > 0 && isRessourceInt)
                          Text(
                            tile.resources[i].toString(),
                            style: TextStyle(
                              color: getResourceColor(i),
                              fontSize: cellSize / 6,
                            ),
                          ),
                      if (getEggsNumber(x, y) > 0)
                        Icon(
                          Icons.egg,
                          color: Colors.white,
                          size: cellSize / 6,
                        ),
                    ],
                  ),
                  if (tile.nbIncantation > 0)
                    Positioned(
                      top: 5,
                      child: Wrap(
                        children: [
                          for (int i = 0; i < tile.nbIncantation; i++)
                            i > 3
                                ? const SizedBox()
                                : Column(
                                    children: [
                                      Icon(
                                        Icons.thunderstorm_outlined,
                                        color: Colors.amberAccent,
                                        size: cellSize / 3,
                                      ),
                                      const SizedBox(height: 4),
                                      Container(
                                        width: 8,
                                        height:
                                            tile.nbIncantation * (cellSize / 3),
                                        decoration: BoxDecoration(
                                          color: Colors.orangeAccent,
                                          borderRadius:
                                              BorderRadius.circular(4),
                                        ),
                                      ),
                                    ],
                                  ),
                        ],
                      ),
                    ),
                ],
              ),
            ),
          );
        },
      ),
    );
  }
}
