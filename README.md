# Audio Stream Direction of Arrival (DOA)

This repository is designed to stream raw audio data, Voice Activity Detection (VAD) status, and decibel (dB) volume levels from an ESP32S3 microphone to a TCP socket.

## Prerequisites

- ESP-IDF version 5.0.0

## Configuration

Set `CONFIG_STA_WIFI_SSID`, `CONFIG_STA_WIFI_PASS`, and `CONFIG_TCP_SERVER_IP` in `sdkconfig.defaults` to connect the device to your network and define the TCP server's IP address.

## File Descriptions

### `main.c`

- Sends VAD and dB volume data for both microphones to port 5000.
- Streams `mic_left` data to port 5000.
- Streams `mic_right` data to port 5001.

### `main_vad_vol.c`

- Streams only VAD and dB volume data to port 5000.

### `tcp_server_mult.py`

- Located on the PC side.
- Sets up TCP sockets to receive the audio, VAD, and volume data.
- Records the audio data into files.
- Outputs volume and VAD data to the console.

### `tcp_server.py`

- Located on the PC side.
- Sets up a TCP socket to receive and print incoming VAD and volume data.
