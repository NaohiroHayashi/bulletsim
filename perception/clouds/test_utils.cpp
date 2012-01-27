#include "utils_pcl.h"
#include "testing.h"
#include <cmath>
using namespace Eigen;
using namespace pcl;

void test_toEigenMatrix() {
ColorCloudPtr cloud = readPCD("/home/joschu/cpp/clouds/test.pcd");
 MatrixXf xyz = toEigenMatrix(cloud);
 BOOST_VERIFY(xyz.rows()==cloud->size());
 BOOST_VERIFY(xyz.cols()==3);
 for (int i=0; i<cloud->size(); i++) {
    PointXYZRGB& pt = cloud->at(i);
    BOOST_VERIFY(pt.x == xyz(i,0));
    BOOST_VERIFY(pt.y == xyz(i,1));
    BOOST_VERIFY(pt.z == xyz(i,2));

 }
}

void test_toBGR() {
  ColorCloudPtr cloud = readPCD("/home/joschu/cpp/clouds/test.pcd");
  MatrixXb bgr = toBGR(cloud);
  BOOST_VERIFY(bgr.rows() == cloud->size());
  BOOST_VERIFY(bgr.cols() == 3);
  for (int i=0; i<cloud->size(); i++) {
    PointXYZRGB& pt = cloud->at(i);
    BOOST_VERIFY(pt.b == bgr(i,0));
    BOOST_VERIFY(pt.g == bgr(i,1));
    BOOST_VERIFY(pt.r == bgr(i,2));
  }
}

void test_getDepthImage() {
  ColorCloudPtr cloud = readPCD("/home/joschu/cpp/clouds/test.pcd");
  MatrixXf depths = getDepthImage(cloud);
  depths.resize(depths.rows()*depths.cols(),1);
  for (int i=0; i<cloud->size(); i++){
      PointXYZRGB& pt = cloud->at(i);
      float d = sqrtf(pow(pt.x,2)+pow(pt.y,2)+pow(pt.z,2));
      BOOST_VERIFY(d == depths(i));
  }
}

void test_xyz2uv() {
  // todo
  // for now, just here to avoid linker errors
  MatrixXf x;
  xyz2uv(x);
}

int main() {
  TEST_FUNC(test_toEigenMatrix);
  TEST_FUNC(test_toBGR);
  TEST_FUNC(test_getDepthImage);
}
