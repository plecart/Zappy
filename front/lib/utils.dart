import 'package:flutter/material.dart';

IconData getResourceIcon(int type) {
  switch (type) {
    case 0:
      return Icons.local_dining;
    case 1:
      return Icons.star;
    case 2:
      return Icons.diamond;
    case 3:
      return Icons.hexagon;
    case 4:
      return Icons.opacity;
    case 5:
      return Icons.local_fire_department;
    case 6:
      return Icons.emoji_objects;
    default:
      return Icons.help;
  }
}

Color getResourceColor(int type) {
  switch (type) {
    case 0:
      return Colors.green;
    case 1:
      return Colors.blue;
    case 2:
      return Colors.orange;
    case 3:
      return Colors.purple;
    case 4:
      return Colors.brown;
    case 5:
      return Colors.red;
    case 6:
      return Colors.yellow;
    default:
      return Colors.white;
  }
}
