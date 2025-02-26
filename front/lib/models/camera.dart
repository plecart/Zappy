import 'package:zappy/models/player.dart';

class Camera {
  Player? player;
  bool isFollowingPlayer;

  Camera({this.player}) : isFollowingPlayer = false;
}
