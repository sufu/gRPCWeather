# Copyright 2015 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""The Python implementation of the GRPC weather.Greeter server."""

from concurrent import futures
import time
import grpc
import weather_pb2
import weather_pb2_grpc
import pprint
import requests


_ONE_DAY_IN_SECONDS = 60 * 60 * 24

api_id = open('./api_id.txt', 'r').read().strip()

def generateURL(type, info):
    url = "http://api.openweathermap.org/data/2.5/weather?"
    if type == "city":
        url += ("q=" + info + ",us")
    elif type == "zip":
        url += ("zip=" + info + ",us")
    else:
        return ""
    url += "&units=metric"
    url += api_id
    return url

def getWeather(type, info):
    weather = weather_pb2.Weather()
    url = generateURL(type, info)
    if len(url) == 0:
        return weather
    pprint.pprint(url)
    weather_request = requests.get(url)
    weather_json = weather_request.json()
    pprint.pprint(weather_json)
    weather.city = weather_json['name']
    weather.temp = int(weather_json['main']['temp'])
    return weather



class WeatherService(weather_pb2_grpc.WeatherServiceServicer):

    def GetWeatherByCity(self, request, context):
        pprint.pprint(request)
        weather = getWeather("city", request.name)
        pprint.pprint(weather)
        return weather

    def GetWeatherByZip(self, request, context):
        pprint.pprint(request)
        weather = getWeather("zip", request.code)
        pprint.pprint(weather)
        return weather


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    weather_pb2_grpc.add_WeatherServiceServicer_to_server(WeatherService(), server)
    server.add_insecure_port('localhost:50051')
    server.start()
    try:
        while True:
            time.sleep(_ONE_DAY_IN_SECONDS)
    except KeyboardInterrupt:
        server.stop(0)


if __name__ == '__main__':
    serve()
