use std::net::{TcpListener, TcpStream};
use std::sync::{Arc, Mutex};
use std::thread;
use std::env;
use tungstenite::{accept, Message};
use std::io::{Read, Write};
use std::time::Duration;

fn handle_tcp_connection(mut stream: TcpStream, ws_clients: Arc<Mutex<Vec<Arc<Mutex<tungstenite::WebSocket<TcpStream>>>>>>) {
    let mut buffer = [0; 512];
    while let Ok(bytes_read) = stream.read(&mut buffer) {
        if bytes_read == 0 {
            break;
        }
        let message = String::from_utf8_lossy(&buffer[..bytes_read]).to_string();
        println!("📩 TCP received: {}", message);

        let mut clients = ws_clients.lock().unwrap();
        println!("👀 Nombre de clients WebSocket: {}", clients.len());
        clients.retain(|ws| {
            if let Ok(mut ws) = ws.try_lock() {
                match ws.write_message(Message::Text(message.clone())) {
                    Ok(_) => {
                        println!("📤 Message envoyé à WebSocket: {}", message);
                        true
                    }
                    Err(e) => {
                        println!("❌ Erreur envoi WebSocket, suppression du client: {}", e);
                        false
                    }
                }
            } else {
                true
            }
        });
    }
}

fn handle_websocket_connection(stream: TcpStream, tcp_sender: Arc<Mutex<TcpStream>>, ws_clients: Arc<Mutex<Vec<Arc<Mutex<tungstenite::WebSocket<TcpStream>>>>>>) {
    let websocket = accept(stream).expect("Failed to accept WebSocket");
    println!("✅ WebSocket handshake completed!");
    let websocket = Arc::new(Mutex::new(websocket));
    {
        let mut clients = ws_clients.lock().unwrap();
        clients.push(websocket.clone());
    }
    let _ = websocket.lock().unwrap().write_message(Message::Text("Connected!".to_string()));
    
    loop {
        let message = {
            let mut ws = websocket.lock().unwrap();
            ws.read_message().ok()
        };
        if let Some(msg) = message {
            if msg.is_text() {
                let text = msg.to_text().unwrap();
                println!("💬 WebSocket received: {}", text);
                let mut tcp_stream = tcp_sender.lock().unwrap();
                if let Err(e) = tcp_stream.write_all(text.as_bytes()) {
                    println!("❌ Erreur d'envoi TCP: {}", e);
                }
            }
        } else {
            println!("❌ WebSocket closed");
            break;
        }
        thread::sleep(Duration::from_millis(10));
    }
    
    ws_clients.lock().unwrap().retain(|ws| !Arc::ptr_eq(ws, &websocket));
}

#[tokio::main]
async fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <tcp_server_port>", args[0]);
        return;
    }
    let tcp_port = &args[1];
    let tcp_address = format!("127.0.0.1:{}", tcp_port);

    let ws_listener = TcpListener::bind("127.0.0.1:8080").expect("Failed to bind WebSocket");
    let ws_clients: Arc<Mutex<Vec<Arc<Mutex<tungstenite::WebSocket<TcpStream>>>>>> = Arc::new(Mutex::new(Vec::new()));

    println!("🔗 Connecting to TCP server at {}...", tcp_address);
    let tcp_stream = TcpStream::connect(&tcp_address).expect("Failed to connect to TCP");
    println!("✅ Connected to TCP server at {}", tcp_address);
    let tcp_sender = Arc::new(Mutex::new(tcp_stream.try_clone().expect("Failed to clone TCP stream")));

    let ws_clients_clone = ws_clients.clone();
    let tcp_stream_clone = tcp_stream.try_clone().expect("Failed to clone TCP stream");
    
    thread::spawn(move || handle_tcp_connection(tcp_stream_clone, ws_clients_clone));

    let tcp_sender_clone = tcp_sender.clone();
    thread::spawn(move || {
        for stream in ws_listener.incoming() {
            if let Ok(stream) = stream {
                println!("🚀 New WebSocket connection!");
                let tcp_sender_clone = tcp_sender_clone.clone();
                let ws_clients_clone = ws_clients.clone();
                thread::spawn(move || handle_websocket_connection(stream, tcp_sender_clone, ws_clients_clone));
            }
        }
    });

    tokio::signal::ctrl_c().await.unwrap();
    println!("Shutting down.");
}