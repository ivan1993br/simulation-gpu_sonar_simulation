#include "MultibeamSonar.hpp"
#include <iostream>

using namespace gpu_sonar_simulation;

namespace gpu_sonar_simulation {

// Simulate a base::samples::SonarBeam data and update the sonar head position
base::samples::Sonar MultibeamSonar::simulateMultibeamSonar(const std::vector<float>& data) {

    base::samples::Sonar sonar;

    sonar.time = base::Time::now();
    sonar.speed_of_sound = _speed_of_sound;
    sonar.bin_count = _number_of_bins;
    sonar.beam_count = _number_of_beams;
    sonar.beam_width = base::Angle::fromDeg(_beamwidth_horizontal);
    sonar.beam_height = base::Angle::fromDeg(_beamwidth_vertical);
    sonar.bins = data;

    base::Angle angular_resolution = base::Angle::fromRad(sonar.beam_width.rad / sonar.beam_count);
    sonar.setRegularBeamBearings(base::Angle::fromRad(-sonar.beam_width.rad / 2), angular_resolution);

    sonar.validate();
    return sonar;
}

// Split image in beam parts. The shader is not radially spaced equally
// over the FOV-X degree sector, so it is necessary to identify which column
// is contained on each beam.
std::vector<uint8_t> MultibeamSonar::codeSonarData(const cv::Mat3f& cv_image) {

	std::vector<cv::Mat> shader;
	cv::split(cv_image, shader);

	// associates shader columns with their respective beam
	std::vector<uint8_t> sonar_data(_number_of_beams * _number_of_bins, 0);

	int middle_img = shader[0].cols * 0.5;
	double a = _number_of_beams * 1.0 / 2;

	for (int i = 0; i < shader[0].cols; i++) {

		// Checks normal values in each column
		if (cv::countNonZero(shader[0].col(i))) {

			int col_start = i;
			int col_end = -1;
			int id_beam;

			// gets the maximum angle in that column
			double max, max2;
			cv::minMaxIdx(shader[2].col(i), NULL, &max);

			i < middle_img ? max = -max : max += 0;
			id_beam = a * (max + 1);

			while (col_end == -1) {
				cv::minMaxIdx(shader[2].col(++i), NULL, &max2);
				i < middle_img ? max2 = -max2 : max2 += 0;
				int id_curr = a * (max2 + 1);
				if (id_curr != id_beam)
					col_end = --i;
			}

			// gets the ROI (beam) of shader image
			cv::Mat cv_roi = cv_image.colRange(col_start, col_end + 1);

			// processes shader informations
			std::vector<double> raw_intensity = decodeShaderImage(cv_roi);
			std::vector<uint8_t> roi_data = getPingData(raw_intensity);
			for (int i = 0; i < _number_of_bins; ++i)
				sonar_data[_number_of_bins * id_beam + i] = roi_data[i];
		}
	}

	return sonar_data;
}


}
