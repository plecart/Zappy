import 'package:flutter/material.dart';

class VolumeSlider extends StatefulWidget {
  const VolumeSlider(
      {required this.volume, required this.onChanged, super.key});

  final double volume;
  final ValueChanged<double> onChanged;

  @override
  VolumeSliderState createState() => VolumeSliderState();
}

class VolumeSliderState extends State<VolumeSlider> {
  double _volume = 1.0;

  @override
  void initState() {
    super.initState();
    _volume = widget.volume;
  }

  @override
  Widget build(BuildContext context) {
    return Slider(
      value: _volume,
      onChanged: (value) {
        widget.onChanged(value);
        setState(() {
          _volume = value;
        });
      },
    );
  }
}
