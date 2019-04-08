#pragma once

namespace hackPSU {

  struct MenuItem{
    String heading;
    void (*loop)();
  };
}
