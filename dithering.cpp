#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

// Function to convert hex color to BGR
cv::Vec3b hexToBGR(const std::string& hex) {
    int r, g, b;
    std::sscanf(hex.c_str(), "%02x%02x%02x", &r, &g, &b);
    return cv::Vec3b(b, g, r);
}

void invertColors(cv::Mat& image) {
    image = 255 - image;
}

// Function to find the closest color in the color set
cv::Vec3b findClosestColor(const cv::Vec3b& color, const std::vector<cv::Vec3b>& colorSet) {
    cv::Vec3b closestColor = colorSet[0];
    int minDistance = std::numeric_limits<int>::max();

    for (const auto& c : colorSet) {
        int distance = cv::norm(color - c);
        if (distance < minDistance) {
            minDistance = distance;
            closestColor = c;
        }
    }

    return closestColor;
}

// Floyd-Steinberg dithering algorithm
void floydSteinbergDithering(cv::Mat& image, const std::vector<cv::Vec3b>& colorSet) {
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            cv::Vec3b oldColor = image.at<cv::Vec3b>(y, x);
            cv::Vec3b newColor = findClosestColor(oldColor, colorSet);
            image.at<cv::Vec3b>(y, x) = newColor;

            cv::Vec3f error = cv::Vec3f(oldColor) - cv::Vec3f(newColor);

            if (x + 1 < image.cols) {
                cv::Vec3b& pixel = image.at<cv::Vec3b>(y, x + 1);
                pixel = cv::Vec3b(cv::saturate_cast<uchar>(pixel[0] + error[0] * 7 / 16),
                                  cv::saturate_cast<uchar>(pixel[1] + error[1] * 7 / 16),
                                  cv::saturate_cast<uchar>(pixel[2] + error[2] * 7 / 16));
            }

            if (y + 1 < image.rows) {
                if (x > 0) {
                    cv::Vec3b& pixel = image.at<cv::Vec3b>(y + 1, x - 1);
                    pixel = cv::Vec3b(cv::saturate_cast<uchar>(pixel[0] + error[0] * 3 / 16),
                                      cv::saturate_cast<uchar>(pixel[1] + error[1] * 3 / 16),
                                      cv::saturate_cast<uchar>(pixel[2] + error[2] * 3 / 16));
                }

                cv::Vec3b& pixel = image.at<cv::Vec3b>(y + 1, x);
                pixel = cv::Vec3b(cv::saturate_cast<uchar>(pixel[0] + error[0] * 5 / 16),
                                  cv::saturate_cast<uchar>(pixel[1] + error[1] * 5 / 16),
                                  cv::saturate_cast<uchar>(pixel[2] + error[2] * 5 / 16));

                if (x + 1 < image.cols) {
                    cv::Vec3b& pixel = image.at<cv::Vec3b>(y + 1, x + 1);
                    pixel = cv::Vec3b(cv::saturate_cast<uchar>(pixel[0] + error[0] * 1 / 16),
                                      cv::saturate_cast<uchar>(pixel[1] + error[1] * 1 / 16),
                                      cv::saturate_cast<uchar>(pixel[2] + error[2] * 1 / 16));
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_image> [--output <output_image>] [--colors <color1,color2,...>]" << std::endl;
        return -1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = "output.png";
    std::string colorSetStr = "000000,FFFFFF";
    bool invertFlag = false;

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--output" && i + 1 < argc) {
            outputPath = argv[++i];
        } else if (arg == "--colors" && i + 1 < argc) {
            colorSetStr = argv[++i];
        } else if (arg == "--invert") {
            invertFlag = true;
        }
    }

    cv::Mat image = cv::imread(inputPath);
    if (image.empty()) {
        std::cerr << "Error: Could not read the image." << std::endl;
        return -1;
    }


    std::vector<cv::Vec3b> colorSet;
    std::istringstream iss(colorSetStr);
    std::string hexColor;
    while (std::getline(iss, hexColor, ',')) {
        colorSet.push_back(hexToBGR(hexColor));
    }

    floydSteinbergDithering(image, colorSet);
    if (invertFlag) {
        invertColors(image);
    };

    cv::imwrite(outputPath, image);
    std::cout << "Dithered image saved to: " << outputPath << std::endl;

    return 0;
}
