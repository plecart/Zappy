import 'dart:html';

void connectWebSocket(void Function(String) onMessage) {
  WebSocket socket =
      WebSocket('ws://127.0.0.1:8080'); // Connexion à la passerelle
  socket.onOpen.listen((event) {
    print('✅ Connecté à la passerelle WebSocket');
    socket.send('GRAPHIC'); // Envoyer un message de test
  });

  socket.onMessage.listen((event) {
    print('📩 Réponse du serveur C via passerelle: ${event.data}');
    onMessage(event.data);
  });

  socket.onClose.listen((event) {
    print('❌ Connexion WebSocket fermée');
  });
}
