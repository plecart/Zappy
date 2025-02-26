import 'package:flutter/material.dart';
import 'package:zappy/models/team.dart';

class EndWidget extends StatelessWidget {
  const EndWidget({
    required this.winner,
    required this.isLost,
    super.key,
  });

  final Team? winner;
  final bool isLost;

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Container(
        padding: const EdgeInsets.all(20),
        decoration: BoxDecoration(
          color: Colors.black.withOpacity(0.5),
          borderRadius: BorderRadius.circular(10),
        ),
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            if (isLost)
              const Text("You lost!",
                  style: TextStyle(
                    color: Colors.white,
                    fontWeight: FontWeight.bold,
                    fontSize: 20,
                  )),
            if (winner != null)
              Text("Team ${winner!.teamName} wins!",
                  style: const TextStyle(
                    color: Colors.white,
                    fontWeight: FontWeight.bold,
                    fontSize: 20,
                  )),
            if (winner != null) const SizedBox(height: 10),
            if (winner != null)
              const Text("Congratulations!",
                  style: TextStyle(
                    color: Colors.white,
                    fontSize: 16,
                  )),
          ],
        ),
      ),
    );
  }
}
