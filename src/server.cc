#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include "suma.grpc.pb.h"
#include <iostream>
#include <memory>
#include <string>
#include <thread>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using suma::Suma;
using suma::SumaReply;
using suma::SumaRequest;

class SumaService final : public Suma::Service
{

  Status CalcularSuma(ServerContext *context,
                      const SumaRequest *request,
                      SumaReply *reply){
    std::int32_t res = request->a()+request->b();
    reply->set_resultado(res);
    return Status::OK;
  }

  Status CalcularSumaStream(ServerContext *context,
                        const SumaRequest *request,
                        grpc::ServerWriter<SumaReply> *writer){
      SumaReply reply;
      std::int32_t res = request->a()+request->b();
      reply.set_resultado(res);
      writer->Write(reply);
      std::cout << res<< std::endl;
    return Status::OK;
  }
};

void RunServer()
{
  std::string server_address("0.0.0.0:50051");
    
  // Crear instancias de los servicios
  SumaService suma_service;
  ServerBuilder builder;
    

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    
    builder.RegisterService(&suma_service);
    
    std::unique_ptr<Server> server(builder.BuildAndStart());
    
    if (!server) {
        std::cerr << "Failed to start server!" << std::endl;
        return;
    }
    
    std::cout << "Server listening on " << server_address << std::endl;
    std::cout << "Servicios disponibles: Suma y Greeter" << std::endl;
    
    server->Wait();
}

int main()
{
  std::cout << "Starting gRPC Server..." << std::endl;
  RunServer();
  return 0;
}