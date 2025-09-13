#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "protos/biblio.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using biblio::Biblioteca;
using biblio::IsbnRequest;
using biblio::TituloRequest;
using biblio::PrestamoResponse;
using biblio::ConsultaResponse;
using biblio::DevolucionResponse;

class BiliotecaCliente {
  public:
    BiliotecaCliente(std::shared_ptr<Channel>channel )
      : stub_(Biblioteca::NewStub(channel)) {}

    std::string PrestamoISBN(const std::int32_t ISBN){
      IsbnRequest request;
      request.set_isbn(ISBN);

      PrestamoResponse response;
      ClientContext context;

      Status status = stub_->PrestamoISBN(&context,request, &response);
      if (status.ok()) {
        if(response.estado()){
          return "Si existe el libro";
        }else{
          return "No existe el libro";
        }
      } else {
        return "RPC failed";
      }
    }

  private:
    std::unique_ptr<biblio::Biblioteca::Stub> stub_;
};

int main() {
  BiliotecaCliente biblioteca(
  grpc::CreateChannel("10.43.101.228:50051", grpc::InsecureChannelCredentials()));
  std::string reply = biblioteca.PrestamoISBN(123456);
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}
