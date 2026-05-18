#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>

struct DataPoint {
    double timestamp;
    int x;
    int y;
    int z;
    int centered_y;
};

int compute_sma(const std::vector<int>& data, int index, int window_size) {
    int sum = 0;
    int count = 0;
    for (int i = index; i > index - window_size && i >= 0; i--) {
        sum += data[i];
        count++;
    }
    return (count == 0) ? 0 : sum / count;
}

int main() {
    std::string filename = "MY_DATA.CSV";
    std::ifstream file(filename);

    std::cout << "==================================================\n";
    std::cout << "          JUMPSTRIDE UX - PERFORMANCE APP         \n";
    std::cout << "==================================================\n";

    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << ".\n";
        std::cerr << "Ensure your micro:bit data is exported to this directory.\n";
        return 1;
    }

    std::string line;
    std::vector<DataPoint> points;
    std::vector<int> y_values;

    if (std::getline(file, line)) {
        if (line.find("Time") == std::string::npos && line.find("X") == std::string::npos) {
            // Line doesn't look like text fields, reset to top
            file.seekg(0);
        }
    }

    while (std::getline(file, line)) {
        if(line.empty()) continue;
        std::stringstream ss(line);
        std::string t_str, x_str, y_str, z_str;

        if (std::getline(ss, t_str, ',') &&
            std::getline(ss, x_str, ',') &&
            std::getline(ss, y_str, ',') &&
            std::getline(ss, z_str, ',')) {

            try {
                DataPoint p;
                p.timestamp = std::stod(t_str);
                p.x = std::stoi(x_str);
                p.y = std::stoi(y_str);
                p.z = std::stoi(z_str);
                points.push_back(p);
                y_values.push_back(p.y);
            } catch (...) {
                continue;
            }
        }
    }
    file.close();

    if (points.empty()) {
        std::cout << "No data records processed from the CSV file.\n";
        return 0;
    }

    long long total_y = 0;
    for (int y : y_values) total_y += y;
    int calculated_mean = total_y / y_values.size();

    std::vector<int> centered_sma_y;
    for (size_t i = 0; i < points.size(); ++i) {
        points[i].centered_y = points[i].y - calculated_mean;
    }

    std::vector<int> raw_centered;
    for(auto p : points) raw_centered.push_back(p.centered_y);
    for(size_t i=0; i < points.size(); ++i) {
        centered_sma_y.push_back(compute_sma(raw_centered, i, 5));
    }

    int jumps = 0;
    int prev_val = 0;
    int local_ext = 0;
    int theta = 450;

    std::vector<double> jump_timestamps;

    for (size_t i = 0; i < centered_sma_y.size(); ++i) {
        int curr_val = centered_sma_y[i];

        if (curr_val > local_ext && curr_val > 0) local_ext = curr_val;
        else if (curr_val < local_ext && curr_val < 0) local_ext = curr_val;

        if (prev_val <= 0 && curr_val > 0) {
            if (std::abs(local_ext - curr_val) > theta) {
                jumps++;
                jump_timestamps.push_back(points[i].timestamp);
            }
            local_ext = curr_val;
        }
        else if (prev_val >= 0 && curr_val < 0) {
            local_ext = curr_val;
        }
        prev_val = curr_val;
    }

    // Metric 1: Max Intensity Exertion Window Calculation
    int max_delta = 0;
    double exertion_timestamp = 0;
    for(size_t i = 1; i < points.size(); ++i) {
        int delta = std::abs(points[i].y - points[i-1].y);
        if(delta > max_delta) {
            max_delta = delta;
            exertion_timestamp = points[i].timestamp;
        }
    }

    // Metric 2: Peak Cadence (Highest consecutive burst rate)
    int max_consecutive_burst = 0;
    if (jump_timestamps.size() > 1) {
        int sequential_count = 1;
        for (size_t i = 1; i < jump_timestamps.size(); ++i) {
            if ((jump_timestamps[i] - jump_timestamps[i-1]) <= 1200.0) {
                sequential_count++;
            } else {
                if (sequential_count > max_consecutive_burst) {
                    max_consecutive_burst = sequential_count;
                }
                sequential_count = 1;
            }
        }
        if (sequential_count > max_consecutive_burst) max_consecutive_burst = sequential_count;
    } else if (!jump_timestamps.empty()) {
        max_consecutive_burst = 1;
    }

    std::cout << ">> Total Samples Parsed   : " << points.size() << "\n";
    std::cout << ">> Calibrated System Mean : " << calculated_mean << " milli-g\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << " PERFORMANCE REPORT:\n";
    std::cout << "  * Estimated Total Jumps  : " << jumps << " jumps\n";
    std::cout << "  * Max Consecutive Burst  : " << max_consecutive_burst << " continuous jumps\n";
    std::cout << "  * Peak Exertion Moment   : " << exertion_timestamp / 1000.0 << " seconds "
              << "(G-Force Shift: " << max_delta << " milli-g)\n";
    std::cout << "==================================================\n";

    return 0;
}
