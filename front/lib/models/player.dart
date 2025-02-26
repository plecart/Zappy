import 'package:zappy/models/inventory.dart';

class Player {
  int x, y, direction, id, level;
  Inventory inventory;

  Player(this.x, this.y, this.id, this.level, this.direction, this.inventory);
}
