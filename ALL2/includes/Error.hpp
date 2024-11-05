// ERROR.hpp
#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>
#include "Server.hpp"
#include "HttpResponse.hpp"

HttpResponse handleError(int statusCode, const Server* server);

#endif // ERROR_HPP