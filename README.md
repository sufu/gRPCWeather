# gRPCWeatherService

C++ Client --- (proto3) --- Python Server --- (JSON) --- OpenWeatherAPI

C++ Client:
make

Python Server:
python -m grpc_tools.protoc -I./ --python_out=. --grpc_python_out=. ./weather.proto
python weather_server.py
