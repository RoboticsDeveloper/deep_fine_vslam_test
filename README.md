# 실행 방법
 
```bash
git clone https://github.com/RoboticsDeveloper/deep_fine_vslam_test.git

cd deep_fine_vslam_test

make build // docker image build

make // docker container run

cd data/orb_slam
./download_dataset_sample.sh // test dataset download

cd ~/ros2_ws
source install/setup.bash // ros2 pkg update

ros2 run glim_extension_test mono_slam_test  
ros2 run glim_extension_test stereo_slam_test 
```

# 주요 수정 사항을 확인할 수 있는 파일 경로

- .gitmoudles 
- CMakeLists.txt
- package.xml
- Makefile
- docker-compose.yaml
- src/glim_extension_test/mono_slam_test.cpp
- src/glim_extension_test/stereo_slam_test.cpp

# 프로젝트 보고서

## 1. 개요
### 1.1 프로젝트 제목
- **프로젝트명**: Visual SLAM 개발자 사전과제

### 1.2 목표
- **프로젝트/과제 목표**: GLIM 레포를 활용하여 CPU only mono/stereo SLAM 시스템 구현
- **기대 성과**:  모노/스테레오 형식의 입력 영상에 대해 pose/map 이 업데이트되는 상태를 viewer 로 확인 가능

---

## 2. 논문 리뷰
### 2.1 논문 정보
- **논문 제목**:  GLIM:3Drange-inertiallocalization andmappingwithGPU-acceleratedscan matchingfactors

### 2.2 논문 요약
- **핵심 개념**: Tightly Coupled LiDAR + IMU SLAM, Keyframe Based Matching Contraint, GPU parallel processing for real time 
- **주요 기법/방법론**:
  - 라이다 데이터 처리를 위해 GPU 가속 스캔 매칭 factor를 활용하여 높은 연산 효율을 구현
  - odometry/sub mapping/global mapping 세 가지 모듈을 비동기화하여 SLAM 시스템 구현
  - tightly coupled 기반의 visual feature projector factor 를 factor 로 추가하면 상태 추정 안정도를 높일 수 있도록 설계
- **결론 및 의의**: GPU 병렬 연산을 통해 그래프 기반 최적화 기법을 수행하여 실시간 SLAM system 을 구현한 것에 의의가 있다. 

### 2.3 리뷰
- **장점**:  LiDAR, IMU 외의 pose/landmark 추정 제약 조건을 제공할 수 있는 다른 종류의 센서 모듈을 추가할 수 있도록 확장하여 다양한 환경에서 강인성을 보일 수 있도록 시스템 설계한 점
- **한계점**: 실시간 시스템 구현 비용이 높다, 장시간 라이다 센서 데이터가 악화된 환경에서 상태 보정 기능이 현저히 떨어진다. 
- **적용 가능성**: 다양한 유형의 센서에 따른 optimization factor 를 추가하여 강인한 SLAM 시스템을 구현할 수 있음. 

---

## 3. 코드 리뷰
### 3.1 코드 개요
- **프로젝트/레포지토리 이름**:  https://koide3.github.io/glim/
- **주요 파일 및 구조**:  odometry/sub mapping/global mapping 모듈로 나뉘어 state estimation 을 진행할 수 있도록 설계되어 있다.
- **주요 라이브러리**: Eigen, OpenCV, GTSAM 등

---

## 4. 과제 해결 과정
### 4.1 문제 정의
- **주어진 문제**:  GLIM 레포를 기반으로 카메라 정보만을 활용하여 SLAM 시스템 구현
- **제약 조건**:  CPU 만을 사용, 카메라 만을 사용

### 4.2 해결 방법
- **사용된 기법/알고리즘**:  ORB SLAM3 의 covisibility graph, Bundle Adjustment, Pose Graph optimization 기법을 활용
- **참고 자료**:  glim_ext/orb_slam test module

### 4.3 GLIM vs ORB SLAM3 시스템 비교
1. 공통점 : odometry / submap / global map 세 스레드로 구성하여 실시간 시스템으로 구현한 것은 비슷함
2. 차이점 : data association 과정에서 sliding 윈도우를 도구로 활용한 것과 covisibility graph, Place recognizaiton 데이터베이스를 활용한 것이 핵심적으로 구별되는 특징
  - shortem data association 방식에서는 지도 상에서 유사한 연관성(matching fator)를 찾는 과정의 차이가 없을 수 있음
  - midterm/long term data association 방식에서는 covisibility 데이터베이스 구조를 활용하여 연산 효율 측면에서도 매우 뛰어남
3. covisiblity graph 를 global map 에 대해서 구현한다면 GPU 없이도 CPU 연산량으로 지도 간의 연결 관계를 표현할 수 있다는 것이 이번 과제를 하면서 핵심으로 생각한 점

---

## 5. 과제 결과물
### 5.1 최종 결과물
- **결과물 요약**: ORB SLAM3 shared library 를 glim 라이브러리 환경에서 활용할 수 있도록 docker container 화 작업
 

### 5.2 결과 분석
- ****:  
- **한계 및 향후 과제**: 일시적/중기적/장기적 지도 간 연간 관계를 형성할 수 있으며 CPU 연산 성능만으로 연관 관계 데이터베이스를 갱신할 수 있는 자료구조 및 알고리즘 구현이 visual SLAM system 을 구현할 수 있는지 여부를 결정할 것으로 판단함

---

## 6. 구현 과정에서의 어려움
### 6.1 주요 문제점
- **발생한 문제**:
  - glim 레포의 각 상태추정 모듈을 visual only slam system refactoring 으로 구현하기에 주어진 시간이 너무 짧음   
  - ROS2 humble 과정에서 ORB SLAM3 라이브러를 빌드하는 과정에서 쓰인 라이브러리 호환 문제 해결

### 6.2 해결 방법
- **시도한 접근법**:
  - glim 의 확장 모듈은 factor 만을 구성하여 GTSAM 최적화에 활용할 수 있도록 설계되어 이미 factor graph 로 최적화된 모듈을 between pose 나 projection factor 로 대체하는 것은 상태 추정 과정에서 큰 의미가 없어 보임
  - GLIM_ext 도커 이미지를 기반으로 ORB SLAM3 를 설치하고 ros2 glim extension package 환경에서 ORB SLAM3 시스템을 구동 가능하도록 구현 
- **최종 해결책**: glim_ext 레포를 참고하여 orb slam test 모듈을 실행 할 수 있도록 코드 변형 

