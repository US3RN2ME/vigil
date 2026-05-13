
#include <vigil/EventCollector.hpp>

namespace vigil {
   EventCollector::EventCollector(std::unique_ptr<ProcessInfoReader> reader)
      : processInfoReader_(std::move(reader)) {}
}