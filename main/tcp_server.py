import socket
import struct

def audio_desc_program():
    host = socket.gethostname()
    port = 5000

    server_socket = socket.socket()
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(('', port))
    ip, actual_port = server_socket.getsockname()
    print("Server running on IP: " + str(ip) + " at port " + str(actual_port))
    
    server_socket.listen(1) # configure how many client the server can listen simultaneously
    conn, address = server_socket.accept()
    print("Connection from: " + str(address))

    PACKET_FORMAT = '?if?if'
    
    while(True):
        data = conn.recv(struct.calcsize(PACKET_FORMAT))
        if data:
            mic_0_vad, mic_0_loudness, mic_0_vol_db, mic_1_vad, mic_1_loudness, mic_1_vol_db = struct.unpack(PACKET_FORMAT, data)
            print(f"Mic 0: VAD - {mic_0_vad}, Vol(dB): {mic_0_vol_db:.2f}")
            print(f"Mic 1: VAD - {mic_1_vad}, Vol(dB): {mic_1_vol_db:.2f}")

if __name__ == '__main__':
    audio_desc_program()