from http.server import BaseHTTPRequestHandler, HTTPServer
import asyncio
import threading
import websockets

class RequestHandler(BaseHTTPRequestHandler):
    def _send_response(self, message):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(bytes(message, "utf8"))

    def do_GET(self):
        self._send_response("Hello, world!")

async def handle_websocket(websocket, path):
    while True:
        await websocket.send(f"Hello my friend")
        message = await websocket.recv()
        print(f"Received message: {message}")
        await websocket.send(f"Echo: {message}")

httpd = HTTPServer(('', 8000), RequestHandler)
print("Starting HTTP server on port 8000...")
httpd_thread = threading.Thread(target=httpd.serve_forever)
httpd_thread.start()

start_server = websockets.serve(handle_websocket, "localhost", 8001)
print("Starting WebSocket server on port 8001...")
asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()


