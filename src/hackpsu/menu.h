#pragma once

namespace hackPSU {
  class Box;
  struct MenuItem{
    String heading;
    void (*loop)(Box*);
  };
}
