#include "response.hpp"

namespace hackPSU{
    Response::Response(String payload, int code){
        this->payload = payload;
        this->code = code;
    }

    Response::operator bool() const{
      return bool(code);
    }
}