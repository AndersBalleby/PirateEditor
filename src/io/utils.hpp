#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "logging/Logger.hpp" 

namespace Utils {
  using Layout = std::vector<std::vector<int>>;

  Layout LoadCSVFile(const std::string& path);
}
 