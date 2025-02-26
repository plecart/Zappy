import 'package:flutter/material.dart';
import 'package:zappy/widgets/game_screen.dart';

void main() {
  runApp(const ZappyApp());
}

class ZappyApp extends StatelessWidget {
  const ZappyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Zappy',
      theme: ThemeData(
        primarySwatch: Colors.blue,
        scaffoldBackgroundColor: Colors.blueGrey[900],
      ),
      home: const GameScreen(),
    );
  }
}
