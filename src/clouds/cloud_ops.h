#pragma once
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include "utils_pcl.h"
#include <pcl/Vertices.h>
#include <pcl/ModelCoefficients.h>
#include <opencv2/core/core.hpp>
#include <cv.h>

ColorPoint makeNanPoint();
std::vector< std::vector<int> > findClusters(ColorCloudPtr cloud, float tol=.02, int minSize=100);
ColorCloudPtr downsampleCloud(const ColorCloudPtr in, float sz);
ColorCloudPtr removeOutliers(const ColorCloudPtr in, float thresh=1, int k=15);
ColorCloudPtr removeRadiusOutliers(const ColorCloudPtr in, float radius, int minK);
ColorCloudPtr smoothSurface(const ColorCloudPtr in);
ColorCloudPtr findConcaveHull(ColorCloudPtr in, float alpha, std::vector<pcl::Vertices>& polygons);
ColorCloudPtr findConvexHull(ColorCloudPtr in, std::vector<pcl::Vertices>& polygons);
ColorCloudPtr cropToHull(const ColorCloudPtr in, ColorCloudPtr hull, std::vector<pcl::Vertices>& polygons, bool organized=false);
ColorCloudPtr projectOntoPlane(const ColorCloudPtr in, pcl::ModelCoefficients::Ptr coefficients);
ColorCloudPtr filterZ(ColorCloudPtr in, float low, float high);
ColorCloudPtr filterY(ColorCloudPtr in, float low, float high);
ColorCloudPtr filterX(ColorCloudPtr in, float low, float high);
ColorCloudPtr filterPlane(ColorCloudPtr in, float dist_thresh, pcl::ModelCoefficients::Ptr coefficients);
Eigen::VectorXf getCircle(ColorCloudPtr cloud);
Eigen::VectorXf getEnclosingCircle(ColorCloudPtr cloud);
ColorCloudPtr getBiggestCluster(ColorCloudPtr in, float tol);
ColorCloudPtr clusterFilter(ColorCloudPtr in, float tol, int minSize);
ColorCloudPtr maskCloud(const ColorCloudPtr in, cv::Mat mask, bool organized=false, bool negative=false);
ColorCloudPtr maskCloud(const ColorCloudPtr in, const VectorXb& mask, bool organized=false, bool negative=false);
ColorCloudPtr removeZRange(const ColorCloudPtr in, float minZ, float maxZ);
void labelCloud(ColorCloudPtr in, const cv::Mat& labels);
ColorCloudPtr hueFilter(ColorCloudPtr in, uint8_t minHue, uint8_t maxHue, uint8_t minSat=0, uint8_t maxSat=255, uint8_t minVal=0, uint8_t maxVal=255, bool organized=false, bool negative=false);
ColorCloudPtr colorSpaceFilter(const ColorCloudPtr in, uint8_t minx, uint8_t maxx, uint8_t miny, uint8_t maxy, uint8_t minz, uint8_t maxz, int code, bool organized=false, bool negative=false);
ColorCloudPtr orientedBoxFilter(ColorCloudPtr cloud_in, const Eigen::Matrix3f& ori, const Eigen::Vector3f& mins, const Eigen::Vector3f& maxes, bool organized=false, bool negative=false);
ColorCloudPtr boxFilter(ColorCloudPtr cloud_in, const Eigen::Vector3f& mins, const Eigen::Vector3f& maxes);
ColorCloudPtr chessBoardCorners(const ColorCloudPtr in, int width_cb, int height_cb);
int getChessBoardPose(const ColorCloudPtr cloud_in, int width_cb, int height_cb, double square_size, Eigen::Matrix4f& transform);
ColorPoint getCorrespondingPoint(ColorCloudPtr cloud, cv::Point2i pixel);
ColorCloudPtr skinFilter(ColorCloudPtr cloud_dense);
ColorCloudPtr filterNeighbors(ColorCloudPtr cloud_in, ColorCloudPtr cloud_neighbor, float radius_search, int color_squared_dist, bool negative=false);
pcl::PointIndices::Ptr neighborIndices(ColorCloudPtr cloud_in, ColorCloudPtr cloud_neighbor, float radius_search, int color_squared_dist);
ColorCloudPtr extractBorder(ColorCloudPtr cloud_in, ColorCloudPtr cloud_veil = new ColorCloudPtr(), ColorCloudPtr cloud_shadow = new ColorCloudPtr());
ColorCloudPtr projectPointsOntoPlane(ColorCloudPtr in, std::vector<float> abcd);