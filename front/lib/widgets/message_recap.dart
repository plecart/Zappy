import 'package:flutter/material.dart';
import 'package:zappy/models/message.dart';

class MessageRecap extends StatelessWidget {
  const MessageRecap({
    required this.messages,
    super.key,
  });

  final List<Message> messages;

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(10),
      decoration: BoxDecoration(
        color: Colors.white,
        border: Border.all(color: Colors.black),
      ),
      child: SingleChildScrollView(
        reverse: true,
        child: Column(
          children: [
            for (int i = 0; i < messages.length; i++)
              Column(
                children: [
                  Text(
                      "${messages[i].playerId} [${messages[i].x}X,${messages[i].y}Y] : ${messages[i].message}",
                      style: const TextStyle(color: Colors.black)),
                ],
              ),
          ],
        ),
      ),
    );
  }
}
