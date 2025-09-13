#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include "protos/biblio.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using biblio::Biblioteca;
using biblio::IsbnRequest;
using biblio::TituloRequest;
using biblio::PrestamoResponse;
using biblio::ConsultaResponse;
using biblio::DevolucionResponse;

struct Libro{
  std::string nombre;
  std::int32_t isbn;
  std::int32_t existencia;
  std::int32_t prestados;
};

class BibliotecaImplementada final : public Biblioteca::Service{
  Status PrestamoISBN(ServerContext* context, const IsbnRequest* request,
                      PrestamoResponse* response) override {
    std::ifstream file("db.txt"); 
    std::string line;
    std::int32_t isbnBuscado = request->isbn();         
    Libro libroEncontrado;
    bool encontrado = false;

    while (std::getline(file, line)) {
        if (line.empty()) continue; 

        Libro libro;

        libro.nombre = line.substr(line.find(":") + 1);
        std::getline(file, line);  
        libro.isbn = std::stoi(line.substr(line.find(":") + 1));
        std::getline(file, line);  
        libro.existencia = std::stoi(line.substr(line.find(":") + 1));
        std::getline(file, line); 
        libro.prestados = std::stoi(line.substr(line.find(":") + 1));

        if (libro.isbn == isbnBuscado) {
            libroEncontrado = libro;
            encontrado = true;
            break;
        }
    }
    if(encontrado){
      response->set_estado(true);
      response->set_fecha("hoy");
    }else{
      response->set_estado(false);
      response->set_fecha("NA");
    }
    return Status::OK;

  }

  Status PrestamoTitulo(ServerContext* context, const TituloRequest* request,
                        PrestamoResponse* response) override{
    std::ifstream file("db.txt"); 
    std::string line;
    std::string titulo = request->titulo();         
    Libro libroEncontrado;
    bool encontrado = false;

    while (std::getline(file, line)) {
        if (line.empty()) continue; 

        Libro libro;

        libro.nombre = line.substr(line.find(":") + 1);
        std::getline(file, line);  
        libro.isbn = std::stoi(line.substr(line.find(":") + 1));
        std::getline(file, line);  
        libro.existencia = std::stoi(line.substr(line.find(":") + 1));
        std::getline(file, line); 
        libro.prestados = std::stoi(line.substr(line.find(":") + 1));

        if (libro.nombre == titulo) {
            libroEncontrado = libro;
            encontrado = true;
            break;
        }
    }
    if(encontrado){
      response->set_estado(true);
      response->set_fecha("hoy");
      return Status::OK;
    }else{
      response->set_estado(false);
      response->set_fecha("NA");
      return Status::CANCELLED;
    }
    return Status::OK;
  }

  Status ConsultaISBN(ServerContext* context, const IsbnRequest* request,
                      ConsultaResponse* response) override {
    response->set_estado(true);
    response->set_disponibles(4);
    return Status::OK;
  }

  Status DevolucionISBN(ServerContext* context, const IsbnRequest* request,
                        DevolucionResponse* response) override {
    response->set_estado(true); 
    return Status::OK;
  }
  
};

void RunServer() {
  std::string server_address("10.43.101.228:50051");
  BibliotecaImplementada service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
}

int main() {
  RunServer();
  return 0;
}
