/*
 * SonarUtils.hpp
 *
 *  Created on: Apr 29, 2015
 *      Author: romulogcerqueira
 */

#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <osg/Image>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <base/samples/SonarScan.hpp>
#include <base/Angle.hpp>
#include <iostream>
#include <sstream>

namespace gpu_sonar_simulation{
	cv::Mat convertShaderOSG2CV(osg::ref_ptr<osg::Image> osg_image);
	std::vector<uint8_t> applyDynamicRangeControl(std::vector<uint8_t> data, uint8_t ad_low, uint8_t ad_span);
	cv::Mat plotNormalHistogram(cv::Mat raw_image, int bins);
	cv::Mat plotDepthHistogram(cv::Mat raw_image, int bins);
	cv::Mat plotSonarData(base::samples::SonarScan sonar, float range, float gain);
	cv::Mat plotSonarData(base::samples::SonarBeam sonar, float range, float gain, cv::Mat cv_sonar, double step);
}
#endif