#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include <grpcpp/grpcpp.h>
#include "protos/biblio.grpc.pb.h"

using biblio::Biblioteca;
using biblio::ConsultaResponse;
using biblio::DevolucionResponse;
using biblio::IsbnRequest;
using biblio::PrestamoResponse;
using biblio::TituloRequest;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

struct Libro
{
  std::string nombre;
  std::int32_t isbn;
  std::int32_t existencia;
  std::int32_t prestados;
};

struct RespuestaNombre{
  bool estado;
  std::int32_t disponibles;
  std::int32_t indice;
};

class BibliotecaImplementada final : public Biblioteca::Service
{
  Status PrestamoISBN(ServerContext *context, const IsbnRequest *request,
                      PrestamoResponse *response) override
  {

    ConsultaResponse consulta;
    ConsultaISBN(context, request, &consulta);
    if (consulta.estado() && (consulta.disponibles() > 0))
    {
      std::vector<Libro> biblioteca = cargarBiblioteca();
      biblioteca[consulta.indice()].prestados++;
      escribirBiblioteca(biblioteca);
      response->set_estado(true);
      response->set_fecha(fechaSiguiente());
    }
    else
    {
      response->set_estado(false);
      response->set_fecha(fechaSiguiente());
    }

    return Status::OK;
  }

  Status PrestamoTitulo(ServerContext *context, const TituloRequest *request,
                        PrestamoResponse *response) override
  {
    RespuestaNombre respuesta = consultaNombre(request->titulo());
    if(respuesta.estado && (respuesta.disponibles>0)){
      std::vector<Libro> biblioteca = cargarBiblioteca();
      biblioteca[respuesta.indice].prestados++;
      escribirBiblioteca(biblioteca);
      response->set_estado(true);
      response->set_fecha(fechaSiguiente());
    }else{
      response->set_estado(false);
      response->set_fecha(fechaSiguiente());
    }
    return Status::OK;
  }

  Status ConsultaISBN(ServerContext *context, const IsbnRequest *request,
                      ConsultaResponse *response) override
  {
    std::ifstream file("/home/estudiante/Documents/app/db.txt");
    std::string line;
    std::int32_t isbnBuscado = request->isbn();
    Libro libroEncontrado = {"", -1, -1, -1};
    bool encontrado = false;

    Libro libro;
    int indice = -1, contador = 0;
    while (file >> libro.nombre >> libro.isbn >> libro.existencia >> libro.prestados)
    {
      if (libro.isbn == isbnBuscado)
      {
        indice = contador;
        encontrado = true;
        libroEncontrado = libro;
        break;
      }
      contador++;
    }
    file.close();
    if (encontrado)
    {
      response->set_estado(true);
      response->set_disponibles(libroEncontrado.existencia - libroEncontrado.prestados);
      response->set_indice(indice);
    }
    else
    {
      response->set_estado(false);
      response->set_disponibles(libroEncontrado.existencia - libroEncontrado.prestados);
      response->set_indice(indice);
    }
    return Status::OK;
  }

  Status DevolucionISBN(ServerContext *context, const IsbnRequest *request,
                        DevolucionResponse *response) override
  {
    ConsultaResponse consulta;
    ConsultaISBN(context, request, &consulta);
    if (consulta.estado()){
      std::vector<Libro> biblioteca = cargarBiblioteca();
      if(biblioteca[consulta.indice()].existencia > consulta.disponibles()){
        biblioteca[consulta.indice()].prestados--;
        escribirBiblioteca(biblioteca);
        response->set_estado(true);
      }else{
        response->set_estado(false);
      }
    }else{
      response->set_estado(false);
    }
    return Status::OK;
  }

  RespuestaNombre consultaNombre(std::string nombreBuscado)
  {

    Libro libro;
    Libro libroEncontrado = {"",-1,-1,-1};
    RespuestaNombre respuesta;
    bool encontrado = false;
    int indice = -1, contador = 0;
    std::ifstream file("/home/estudiante/Documents/app/db.txt");
    while (file >> libro.nombre >> libro.isbn >> libro.existencia >> libro.prestados)
    {
      if (libro.nombre == nombreBuscado)
      {
        indice = contador;
        encontrado = true;
        libroEncontrado = libro;
        break;
      }
      contador++;
    }
    file.close();
    if (encontrado)
    {
      respuesta.estado = true;
      respuesta.disponibles = libroEncontrado.existencia - libroEncontrado.prestados;
      respuesta.indice = indice;
    }
    else
    {
      respuesta.estado = false;
      respuesta.disponibles = libroEncontrado.existencia - libroEncontrado.prestados;
      respuesta.indice = indice;
    }

    return respuesta;
  }

  std::vector<Libro> cargarBiblioteca()
  {
    std::vector<Libro> biblioteca;
    Libro libro;
    std::ifstream file("/home/estudiante/Documents/app/db.txt");
    while (file >> libro.nombre >> libro.isbn >> libro.existencia >> libro.prestados)
    {
      biblioteca.push_back(libro);
    }
    file.close();
    return biblioteca;
  }

  void escribirBiblioteca(std::vector<Libro> biblioteca)
  {
    std::ofstream file("/home/estudiante/Documents/app/db.txt");
    for (int i = 0; i < biblioteca.size(); i++)
    {
      file << biblioteca[i].nombre << " " << biblioteca[i].isbn << " " << biblioteca[i].existencia << " " << biblioteca[i].prestados << std::endl;
    }
  }

  std::string fechaSiguiente(){
    time_t ahora = time(nullptr);
    time_t futuro = ahora + 7 * 24 * 60 * 60;
    tm* fecha = localtime(&futuro);
    std::string fechaFinal = "dia " + std::to_string(fecha->tm_mday) + " del mes " + std::to_string(fecha->tm_mon + 1);
    return fechaFinal;
  } 
};

void RunServer()
{
  std::string server_address("10.43.101.228:50051");
  BibliotecaImplementada service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Escuchando: " << server_address << std::endl;

  server->Wait();
}

int main()
{
  RunServer();
  return 0;
}
