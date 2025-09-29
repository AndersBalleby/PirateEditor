#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <algorithm>

#include "logging/Logger.hpp" 

namespace Utils {
  using Layout = std::vector<std::vector<int>>;

  /* Indlæser CSV layout til scene for en tile group*/
  Layout LoadCSVFile(const std::string& path);

  /* Indlæser animation filer ud fra korrekt rækkefølge */
  int extractNumber(const std::filesystem::path& p);
  std::vector<std::filesystem::path> getAnimationFiles(const std::filesystem::path& folderPath);
}
 