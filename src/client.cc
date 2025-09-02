#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include "suma.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using suma::Suma;           
using suma::SumaRequest;    
using suma::SumaReply;     

class SumaClient {
 public:
  SumaClient(std::shared_ptr<Channel> channel)
      : stub_(Suma::NewStub(channel)) {} 

  int32_t CalcularSuma(int32_t a, int32_t b) {
    SumaRequest request;
    request.set_a(a);
    request.set_b(b);

    SumaReply reply;
    ClientContext context;

    Status status = stub_->CalcularSuma(&context, request, &reply);

    if (status.ok()) {
      return reply.resultado();
    } else {
      std::cout << "RPC failed: " << status.error_message() << std::endl;
      return -1;
    }
  }

  void CalcularSumaStream(int32_t a, int32_t b) {
    SumaRequest request;
    request.set_a(a);
    request.set_b(b);

    ClientContext context;
    
    std::unique_ptr<grpc::ClientReader<SumaReply>> reader(
        stub_->CalcularSumaStream(&context, request));

    SumaReply reply;
    std::cout << "Recibiendo stream del servidor:" << std::endl;
    
    while (reader->Read(&reply)) {
      std::cout << "Resultado recibido: " << reply.resultado() << std::endl;
    }

    Status status = reader->Finish();
    if (!status.ok()) {
      std::cout << "Stream falló: " << status.error_message() << std::endl;
    }
  }

 private:
  std::unique_ptr<Suma::Stub> stub_;
};

int main(int argc, char** argv) {
  std::string target_str = "localhost:50051";
  
  auto channel = grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials());
  SumaClient client(channel);

  // Probar suma simple
  std::cout << "=== Suma Simple ===" << std::endl;
  int resultado = client.CalcularSuma(5, 3);
  std::cout << "5 + 3 = " << resultado << std::endl;

  // Probar suma con streaming
  std::cout << "\n=== Suma con Streaming ===" << std::endl;
  client.CalcularSumaStream(10, 20);

  return 0;
}