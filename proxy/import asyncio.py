import asyncio
import websockets

async def test_ws():
    try:
        async with websockets.connect("ws://localhost:8080") as ws:
            print("✅ Connecté au WebSocket")
            await ws.send("Test")
            print("📤 Message envoyé")
            response = await ws.recv()
            print("📩 Réponse reçue:", response)
    except Exception as e:
        print("❌ Erreur:", e)

asyncio.run(test_ws())