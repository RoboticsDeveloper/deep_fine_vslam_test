#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <System.h>
#include <glim_extension_test/util/config_ext.hpp>

class OrbSLAMTest {
public:
  OrbSLAMTest() {
    const std::string voc_path = "/root/ros2_ws/src/glim_extension_test/thirdparty/ORB_SLAM3/Vocabulary/ORBvoc.txt";
    const std::string settings_path = "/root/ros2_ws/src/glim_extension_test/thirdparty/ORB_SLAM3/Examples/Monocular/EuRoC.yaml";
    system.reset(new ORB_SLAM3::System(voc_path, settings_path, ORB_SLAM3::System::MONOCULAR, true));
  }

public:
  std::unique_ptr<ORB_SLAM3::System> system;
};

int main(int argc, char** argv) {
  const std::string euroc_path = "/root/ros2_ws/src/glim_extension_test/data/orb_slam/EuRoC/MH01/mav0";

  std::string line;
  std::vector<std::pair<double, std::string>> images;

  std::ifstream cam0_ifs(euroc_path + "/cam0/data.csv");
  while (!cam0_ifs.eof() && std::getline(cam0_ifs, line) && !line.empty()) {
    std::stringstream sst(line);
    if (line[0] == '#') {
      continue;
    }

    long timestamp;
    char comma;
    std::string filename;

    sst >> timestamp >> comma >> filename;
    images.push_back(std::make_pair(timestamp / 1e9, euroc_path + "/cam0/data/" + filename));
  }

  OrbSLAMTest orb_slam;

  cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(3.0, cv::Size(8, 8));

  for (int i = 0; i < images.size(); i++) {
    cv::Mat image = cv::imread(images[i].second, cv::IMREAD_UNCHANGED);

    if (images[i].second.empty()) {
      std::cerr << std::endl << "Failed to load image at: " << images[i].first << std::endl;
      return 1;
    }

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    orb_slam.system->TrackMonocular(image, images[i].first);
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

    double ttrack = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();

    double T = 0;
    if (i < images.size() - 1)
      T = images[i + 1].first - images[i].first;
    else if (i > 0)
      T = images[i].first - images[i - 1].first;

    if (ttrack < T) usleep((T - ttrack) * 1e6);
  }
  orb_slam.system->Shutdown();
  std::cin.ignore(1);

  return 0;
}