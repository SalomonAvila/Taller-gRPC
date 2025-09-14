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

// Estructura para representar un libro en la base de datos
// Formato en db.txt: Titulo ISBN Existencia Prestados
struct Libro
{
  std::string nombre;
  std::int32_t isbn;
  std::int32_t existencia;
  std::int32_t prestados;
};

// Estructura auxiliar: Devuelve resultados de búsqueda por nombre
struct RespuestaNombre{
  bool estado;
  std::int32_t disponibles;
  std::int32_t indice;
};
// Implementación del servicio gRPC 'Biblioteca'
class BibliotecaImplementada final : public Biblioteca::Service
{
  // RPC: PrestamoISBN
  // Intenta prestar un libro por ISBN: consulta disponibilidad y actualiza el archivo
  Status PrestamoISBN(ServerContext *context, const IsbnRequest *request,
                      PrestamoResponse *response) override
  {
    // Llama a ConsultaISBN para obtener disponibilidad e índice
    ConsultaResponse consulta;
    ConsultaISBN(context, request, &consulta);

    // Si existe y hay disponibles, se actualiza la base de datos
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
      // Índice inválido o no hay disponibles
      response->set_estado(false);
      response->set_fecha(fechaSiguiente()); // aunque falle, retorna fecha
    }

    return Status::OK;
  }

// RPC: PrestamoTitulo
// Intenta prestar un libro por título: consulta disponibilidad y actualiza el archivo
  Status PrestamoTitulo(ServerContext *context, const TituloRequest *request,
                        PrestamoResponse *response) override
  {
    // Buscar libro por título usando la función auxiliar
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

  // RPC: ConsultaISBN
  // Recorre db.txt buscando el ISBN y devuelve si existe, cuántos disponibles y su índice
  Status ConsultaISBN(ServerContext *context, const IsbnRequest *request,
                      ConsultaResponse *response) override
  {
    // Ruta del archivo
    std::ifstream file("/home/estudiante/Documents/app/db.txt");
    std::string line;
    std::int32_t isbnBuscado = request->isbn();
    Libro libroEncontrado = {"", -1, -1, -1};
    bool encontrado = false;

    Libro libro;
    int indice = -1, contador = 0;
    // Recorre el archivo buscando el ISBN
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
      // Libro encontrado: calcula disponibles y devuelve índice
      response->set_estado(true);
      response->set_disponibles(libroEncontrado.existencia - libroEncontrado.prestados);
      response->set_indice(indice);
    }
    else
    {
      // Libro no encontrado: se devuelve estado falso e índice -1
      response->set_estado(false);
      response->set_disponibles(libroEncontrado.existencia - libroEncontrado.prestados);
      response->set_indice(indice);
    }
    return Status::OK;
  }

  // RPC: DevolucionISBN
  // Intenta devolver un libro por ISBN: consulta existencia y actualiza el archivo
  Status DevolucionISBN(ServerContext *context, const IsbnRequest *request,
                        DevolucionResponse *response) override
  {
    ConsultaResponse consulta;
    ConsultaISBN(context, request, &consulta);
    if (consulta.estado()){
      // Solo se puede devolver si hay prestados > 0
      std::vector<Libro> biblioteca = cargarBiblioteca();
      if(biblioteca[consulta.indice()].existencia > consulta.disponibles()){
        biblioteca[consulta.indice()].prestados--;
        escribirBiblioteca(biblioteca);
        response->set_estado(true);
      }else{
        // No hay ejemplares prestados a devolver
        response->set_estado(false);
      }
    }else{
      response->set_estado(false);
    }
    return Status::OK;
  }

  // Busca un libro por nombre y devuelve estado, disponibles e índice
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
      // Libro encontrado: calcular disponibles y devolver índice
      respuesta.estado = true;
      respuesta.disponibles = libroEncontrado.existencia - libroEncontrado.prestados;
      respuesta.indice = indice;
    }
    else
    {
      // Libro no encontrado: devolver estado falso
      respuesta.estado = false;
      respuesta.disponibles = libroEncontrado.existencia - libroEncontrado.prestados;
      respuesta.indice = indice;
    }

    return respuesta;
  }

  // Carga la base de datos desde db.txt en un vector de libros
  std::vector<Libro> cargarBiblioteca()
  {
    std::vector<Libro> biblioteca;
    Libro libro;
    std::ifstream file("/home/estudiante/Documents/app/db.txt");
    // Leer línea por línea el archivo e ir guardando en el vector
    while (file >> libro.nombre >> libro.isbn >> libro.existencia >> libro.prestados)
    {
      biblioteca.push_back(libro);
    }
    file.close();
    return biblioteca;
  }

  // Reescribe el archivo db.txt con la información del vector recibido
  void escribirBiblioteca(std::vector<Libro> biblioteca)
  {
    std::ofstream file("/home/estudiante/Documents/app/db.txt");
    for (int i = 0; i < biblioteca.size(); i++)
    {
       // Sobreescribe todo el archivo
      file << biblioteca[i].nombre << " " << biblioteca[i].isbn << " " << biblioteca[i].existencia << " " << biblioteca[i].prestados << std::endl;
    }
  }

  // Calcula la fecha de devolución (7 días a partir de hoy) y la devuelve como string
  std::string fechaSiguiente(){
    time_t ahora = time(nullptr);
    time_t futuro = ahora + 7 * 24 * 60 * 60;
    tm* fecha = localtime(&futuro);
    std::string fechaFinal = "dia " + std::to_string(fecha->tm_mday) + " del mes " + std::to_string(fecha->tm_mon + 1);
    return fechaFinal;
  } 
};

// Configura y ejecuta el servidor gRPC
void RunServer()
{
  // Dirección por defecto. Cambiar si es necesario
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
