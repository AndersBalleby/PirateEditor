#include "utils.hpp"

namespace Utils {
  std::vector<std::vector<int>> LoadCSVFile(const std::string& path) {
    std::ifstream file(path);
    if(!file.is_open()) {
      Log::Error("Kunne ikke åbne filen: {}", path);
      return {};
    }

    std::vector<std::vector<int>> map;
    std::string line;
    
    while(std::getline(file, line)) {
      std::stringstream ss(line);
      std::string value;
      std::vector<int> row;

      while(std::getline(ss, value, ',')) {
        row.push_back(std::stoi(value));
      }

      map.push_back(row);
    }

    file.close();

    Log::Info("Indlæste indhold fra csv fil: {}", path);
    return map;
  }
}