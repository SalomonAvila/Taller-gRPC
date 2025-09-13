#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "protos/biblio.grpc.pb.h"

using biblio::Biblioteca;
using biblio::ConsultaResponse;
using biblio::DevolucionResponse;
using biblio::IsbnRequest;
using biblio::PrestamoResponse;
using biblio::TituloRequest;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class BiliotecaCliente
{
public:
  BiliotecaCliente(std::shared_ptr<Channel> channel)
      : stub_(Biblioteca::NewStub(channel)) {}

  std::string PrestamoISBN(const std::int32_t ISBN)
  {
    IsbnRequest request;
    request.set_isbn(ISBN);

    PrestamoResponse response;
    ClientContext context;

    Status status = stub_->PrestamoISBN(&context, request, &response);
    if (status.ok())
    {
      if (response.estado())
      {
        return "Prestado y con fecha de devolucion el " + response.fecha();
      }
      else
      {
        return "No se pudo prestar";
      }
    }
    else
    {
      return "Error en la conexion con server";
    }
  }

  std::string PrestamoTitulo(const std::string tituloLibro)
  {
    TituloRequest request;
    request.set_titulo(tituloLibro);
    PrestamoResponse response;
    ClientContext context;

    Status status = stub_->PrestamoTitulo(&context, request, &response);
    if (status.ok())
    {
      if (response.estado())
      {
        return "Prestado y con fecha de devolucion el " + response.fecha();
      }
      else
      {
        return "No se pudo prestar";
      }
    }
    else
    {
      return "Error en la conexion con server";
    }
  }

  std::string ConsultaISBN(const std::int32_t ISBN)
  {
    IsbnRequest request;
    request.set_isbn(ISBN);

    ConsultaResponse response;
    ClientContext context;

    Status status = stub_->ConsultaISBN(&context, request, &response);
    if (status.ok())
    { 
      if (response.estado() && (response.disponibles() > 0))
      {
        std::string mensaje = "Existe con ";
        mensaje += std::to_string(response.disponibles());
        mensaje += " ejemplares disponibles";
        return mensaje;
      }
      else
      {
        return "No se pudo prestar";
      }
    }
    else
    {
      return "Error en la conexion con server";
    }
  }

  std::string DevolucionISBN(const std::int32_t ISBN)
  {
    IsbnRequest request;
    request.set_isbn(ISBN);

    DevolucionResponse response;
    ClientContext context;

    Status status = stub_->DevolucionISBN(&context, request, &response);
    if (status.ok())
    {
      if (response.estado())
      {
        return "Devuelto con exito";
      }
      else
      {
        return "No se pudo devolver, revisar ISBN y volver a intentar...";
      }
    }
    else
    {
      return "Error en la conexion con server";
    }
  }

private:
  std::unique_ptr<biblio::Biblioteca::Stub> stub_;
};

void menu()
{
  std::cout << "\n\n-------MENU--------------------------------\n\n";
  std::cout << "|1. Pedir libro prestado por isbn         |\n";
  std::cout << "|2. Pedir libro prestado por titulo       |\n";
  std::cout << "|3. Consultar existencia de un libro isbn |\n";
  std::cout << "|4. Devolver un libro por isbn            |\n";
  std::cout << "|0. Salir                                 |\n";
  std::cout << "-------------------------------------------\n";
  std::cout << "Ingrese la opcion deseada: ";
}

int main()
{
  BiliotecaCliente biblioteca(
      grpc::CreateChannel("10.43.101.228:50051", grpc::InsecureChannelCredentials()));
  int opc = -1;
  std::cout << "Bienvenido al servicio de biblioteca\n";
  while (true)
  {
    menu();
    std::cin >> opc;
    std::string reply;
    std::string titulo;
    int32_t isbn;

    switch (opc)
    {
    case 1:
      std::cout << "Ingrese el isbn del libro que quiere pedir prestado: ";
      std::cin >> isbn;
      reply = biblioteca.PrestamoISBN(isbn);
      std::cout << reply << std::endl;
      break;
    case 2:
      std::cout << "Ingrese el titulo del libro que quiere pedir prestado: ";
      std::cin >> titulo;
      reply = biblioteca.PrestamoTitulo(titulo);
      std::cout << reply << std::endl;
      break;
    case 3:
      std::cout << "Ingrese el isbn del libro que quiere consultar: ";
      std::cin >> isbn;
      reply = biblioteca.ConsultaISBN(isbn);
      std::cout << reply << std::endl;
      break;
    case 4:
      std::cout << "Ingrese el isbn del libro que quiere devolver: ";
      std::cin >> isbn;
      reply = biblioteca.DevolucionISBN(isbn);
      std::cout << reply << std::endl;
      break;
    case 0:
      std::cout << "¡Gracias por usar el sistema de libros" << std::endl;
      exit(0);
      break;
    }
  }
  return 0;
}
