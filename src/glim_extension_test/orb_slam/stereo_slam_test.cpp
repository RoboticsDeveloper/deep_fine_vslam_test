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
    const std::string settings_path = "/root/ros2_ws/src/glim_extension_test/thirdparty/ORB_SLAM3/Examples/Stereo/EuRoC.yaml";
    system.reset(new ORB_SLAM3::System(voc_path, settings_path, ORB_SLAM3::System::STEREO, true));
  }

public:
  std::unique_ptr<ORB_SLAM3::System> system;
};

int main(int argc, char** argv) {
  const std::string euroc_path = "/root/ros2_ws/src/glim_extension_test/data/orb_slam/EuRoC/MH01/mav0";

  std::string line;
  std::vector<std::pair<double, std::string>> left_images, right_images;

  std::ifstream cam0_ifs(euroc_path + "/cam0/data.csv"), cam1_ifs(euroc_path + "/cam1/data.csv");
  while (!cam0_ifs.eof() && std::getline(cam0_ifs, line) && !line.empty()) {
    std::stringstream sst(line);
    if (line[0] == '#') {
      continue;
    }

    long timestamp;
    char comma;
    std::string filename;

    sst >> timestamp >> comma >> filename;
    left_images.push_back(std::make_pair(timestamp / 1e9, euroc_path + "/cam0/data/" + filename));
    right_images.push_back(std::make_pair(timestamp / 1e9, euroc_path + "/cam1/data/" + filename));
  }

  OrbSLAMTest orb_slam;

  for (int i = 0; i < left_images.size(); i++) {
    cv::Mat left_image = cv::imread(left_images[i].second, cv::IMREAD_UNCHANGED);
    cv::Mat right_image = cv::imread(right_images[i].second, cv::IMREAD_UNCHANGED);

    if (left_images[i].second.empty()) {
      std::cerr << std::endl << "Failed to load left image at: " << left_images[i].first << std::endl;
      return 1;
    }

    if (right_images[i].second.empty()) {
      std::cerr << std::endl << "Failed to load right image at: " << right_images[i].first << std::endl;
      return 1;
    }

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    orb_slam.system->TrackStereo(left_image, right_image, left_images[i].first);
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

    double ttrack = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();

    double T = 0;
    if (i < left_images.size() - 1)
      T = left_images[i + 1].first - left_images[i].first;
    else if (i > 0)
      T = left_images[i].first - left_images[i - 1].first;

    if (ttrack < T) usleep((T - ttrack) * 1e6);
  }
  orb_slam.system->Shutdown();
  std::cin.ignore(1);

  return 0;
}