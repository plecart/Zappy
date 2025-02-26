import asyncio
import websockets
import socket

TCP_HOST = "127.0.0.1"
TCP_PORT = 4458
WS_HOST = "0.0.0.0"
WS_PORT = 8080

ws_clients = set()

def create_tcp_connection():
    """Crée une connexion persistante au serveur TCP."""
    tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_socket.connect((TCP_HOST, TCP_PORT))
    tcp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
    print(f"✅ Connecté en permanence au serveur TCP {TCP_HOST}:{TCP_PORT}")
    return tcp_socket

async def handle_tcp(tcp_socket):
    """Écoute les messages du serveur TCP et les diffuse aux WebSockets."""
    loop = asyncio.get_running_loop()
    while True:
        try:
            data = await loop.run_in_executor(None, tcp_socket.recv, 18000)
            if not data:
                break
            message = data.decode("utf-8")
            print(f"📩 TCP received: {message}")
            await asyncio.gather(*(ws.send(message) for ws in list(ws_clients)))
        except Exception as e:
            print(f"❌ Erreur TCP: {e}")
            break

async def handle_websocket(websocket, tcp_socket):
    print("🚀 Nouveau client WebSocket connecté")
    ws_clients.add(websocket)
    try:
        await websocket.send("Connected !")
        async for message in websocket:
            print(f"💬 WebSocket received: {message}")
            try:
                tcp_socket.sendall(message.encode("utf-8"))
            except Exception as e:
                print(f"❌ Erreur d'envoi TCP: {e}")
    except websockets.exceptions.ConnectionClosed:
        print(f"❌ WebSocket client déconnecté")
    finally:
        ws_clients.discard(websocket)

async def main():
    tcp_socket = create_tcp_connection()
    loop = asyncio.get_event_loop()
    loop.create_task(handle_tcp(tcp_socket))
    start_server = await websockets.serve(lambda ws: handle_websocket(ws, tcp_socket), WS_HOST, WS_PORT)
    print(f"✅ Serveur WebSocket actif sur ws://{WS_HOST}:{WS_PORT}")
    await start_server.wait_closed()

if __name__ == "__main__":
    asyncio.run(main())
