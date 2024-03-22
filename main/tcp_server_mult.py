import socket
import wave
import struct
import threading
import os
import select

MIC_0_VOL_CAL_SUB = 0.18

def mic_data_desc_print(port):
    #host = socket.gethostname()

    server_socket = socket.socket()
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(('', port))
    ip, actual_port = server_socket.getsockname()
    print("Server running on IP: " + str(ip) + " at port " + str(actual_port))
    
    server_socket.listen(1) # configure how many client the server can listen simultaneously
    conn, address = server_socket.accept()
    print("Connection from: " + str(address))

    PACKET_FORMAT = '?if?if'
    PACKET_SIZE = struct.calcsize(PACKET_FORMAT)
    
    #diff_total = 0
    #iter = 1

    while(True):
        data = conn.recv(PACKET_SIZE)
        if data:
            mic_0_vad, mic_0_loudness, mic_0_vol_db, mic_1_vad, mic_1_loudness, mic_1_vol_db = struct.unpack(PACKET_FORMAT, data)
            #print(f"Mic 0: VAD - {mic_0_vad}, Loudness: {mic_0_loudness}, Vol(dB): {mic_0_vol_db:.2f}")
            #print(f"Mic 1: VAD - {mic_1_vad}, Loudness: {mic_1_loudness}, Vol(dB): {mic_1_vol_db:.2f}")

            #diff_total += (mic_0_vol_db - mic_1_vol_db)
            #print(f"diff: {diff_total/iter:.2f}")
            #iter += 1
            
            #mic_0_vol_db -= MIC_0_VOL_CAL_SUB
            print(f"Mic 0: VAD - {mic_0_vad}, Vol(dB): {mic_0_vol_db:.2f}")
            print(f"Mic 1: VAD - {mic_1_vad}, Vol(dB): {mic_1_vol_db:.2f}")

def audio_file_record(port, file_prefix):
    EOT_MARKER = b'EndOfTransmission'
    
    server_socket = socket.socket()
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(('', port))
    ip, actual_port = server_socket.getsockname()
    print(f"Server running on IP: {ip} at port {actual_port}")
    
    server_socket.listen(1)
    conn, address = server_socket.accept()
    print(f"Connection from: {address}")

    file_counter = 1
    while True:
        if file_counter > 5:
            file_counter = 0

        file_name = f"{file_prefix}_{file_counter}.wav"
        with wave.open(file_name, 'wb') as wave_file:
            wave_file.setnchannels(1)  # Mono
            wave_file.setsampwidth(2)  # Sample width in bytes
            wave_file.setframerate(16000)  # Sample rate in Hz
            total_written = 0
            print(f"{file_name} created")

            while True:
                data = conn.recv(512)
                if not data or data == EOT_MARKER:
                    break
                
                wave_file.writeframes(data)
                total_written = wave_file.tell()
            
            print(f"{file_name} written, total bytes: {total_written}")
        
        file_counter += 1
        print("Waiting for new audio transmission...")

        # Wait for data to be available on the socket using select
        while True:
            ready, _, _ = select.select([conn], [], [])
            if ready:
                break
    
    conn.close()

def start_record_server(port, file_name):
    threading.Thread(target=audio_file_record, args=(port, file_name)).start()

if __name__ == '__main__':
    threading.Thread(target=mic_data_desc_print, args=(5000,)).start()
    start_record_server(5001, 'test/mic0_')
    start_record_server(5002, 'test/mic1_')