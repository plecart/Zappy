import 'package:flutter/material.dart';
import 'package:zappy/models/player.dart';

class Team {
  final String teamName;
  final Color teamColor;
  final List<Player>? players;

  Team(this.teamName, this.players, this.teamColor);
}
