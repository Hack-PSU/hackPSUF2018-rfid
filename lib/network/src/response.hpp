#pragma once

#include <Arduino.h>

namespace hackPSU {
  class Response{
  public:
    String payload;
    int code;

    Response(String payload, int code);

    operator bool() const;

  };
}