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

  int extractNumber(const std::filesystem::path& p) {
    std::string stem = p.stem().string();
    try {
      return std::stoi(stem);
    } catch(...) {
      return 0;
    }
  }

  std::vector<std::filesystem::path> getAnimationFiles(const std::filesystem::path &folderPath) {
    if(!std::filesystem::is_directory(folderPath)) {
      Log::Error("Kunne ikke finde en mappe på filstien: {}", folderPath.string());
      return {};
    }
  
    std::vector<std::filesystem::path> files;
    for(auto const& dir_entry : std::filesystem::directory_iterator(folderPath)) {
      if(dir_entry.is_regular_file()) {
        files.push_back(dir_entry.path());
      }
    }

    std::sort(files.begin(), files.end(), [](auto const& a, auto const& b) {
      return Utils::extractNumber(a) < Utils::extractNumber(b);
    });

    return files;
  }
}