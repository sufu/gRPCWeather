/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/weather.grpc.pb.h"
#else
#include "weather.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using weather::City;
using weather::Zip;
using weather::Weather;
using weather::WeatherService;

class WeatherClient {
 public:
  WeatherClient(std::shared_ptr<Channel> channel)
      : stub_(WeatherService::NewStub(channel)) {}

  std::string GetWeatherByCity(const std::string& city) {
    City request;
    request.set_name(city);

    Weather reply;
    ClientContext context;
    Status status = stub_->GetWeatherByCity(&context, request, &reply);

    if (status.ok()) {
      std::string weather = reply.city() + ": " + std::to_string(reply.temp()) + " degree";
      return weather;
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

  std::string GetWeatherByZip(const std::string& zip) {
    Zip request;
    request.set_code(zip);

    Weather reply;
    ClientContext context;
    Status status = stub_->GetWeatherByZip(&context, request, &reply);

    if (status.ok()) {
      std::string weather = reply.city() + ": " + std::to_string(reply.temp()) + " degree";
      return weather;
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<WeatherService::Stub> stub_;
};

int main(int argc, char** argv) {
  WeatherClient client(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  if (argc == 1) {
    std::string city("Fremont");
    std::string reply = client.GetWeatherByCity(city);
    std::cout << "WeatherService received: " << reply << std::endl;

    std::string zip("95010");
    reply = client.GetWeatherByZip(zip);
    std::cout << "WeatherService received: " << reply << std::endl;
  } else if (argc == 3) {
    std::string type = std::string(argv[1]);
    std::string value = std::string(argv[2]);
    if (type != "city" && type != "zip") {
      std::cout << "Query type must be city or zip." << std::endl;
      return 0;
    }

    std::string reply;
    if (type == "city") {
      reply = client.GetWeatherByCity(value);
    } else if (type == "zip") {
      reply = client.GetWeatherByZip(value);
    }
    std::cout << "WeatherService received: " << reply << std::endl;
  }
  return 0;
}
