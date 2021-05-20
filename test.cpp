#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

using json = nlohmann::json;

struct rectangle_info 
{
    int left_top[2];
    int right_bottom[2];
};

struct box_info 
{
    std::string name;
    rectangle_info rectangle;
};

struct bbox 
{
    int left_top_x;
    int left_top_y;
    int right_bottom_x;
    int right_bottom_y;
};

void from_json(const json &j, rectangle_info &r)
{
    for(int i = 0; i < j["left_top"].size(); i++) {
        r.left_top[i] = j["left_top"][i];
    }
    for(int i = 0; i < j["right_bottom"].size(); i++) {
        r.right_bottom[i] = j["right_bottom"][i];
    }
}

void from_json(const json &j, box_info &b)
{
    j.at("name").get_to(b.name);
    j.at("rectangle").get_to(b.rectangle);
}

bbox get_bbox(std::string json_file)
{
    bbox b;
    json j;
    std::ifstream jfile(json_file);
    jfile >> j;
    int box_length = j["boxes"].size();
    box_info bi[box_length];
    for (int i = 0; i < box_length; i++) 
    {
        bi[i] = j["boxes"][i];
        if (bi[i].name == "box_b")
        {
            b.left_top_x = bi[i].rectangle.left_top[0];
            b.left_top_y = bi[i].rectangle.left_top[1];
            b.right_bottom_x = bi[i].rectangle.right_bottom[0];
            b.right_bottom_y = bi[i].rectangle.right_bottom[1];
            // std::cout << "left_top: [" << bi[i].rectangle.left_top[0] << ", " << bi[i].rectangle.left_top[1]
            // << "]; right_bottom: [" << bi[i].rectangle.right_bottom[0] << ", " << bi[i].rectangle.right_bottom[1]<< "]" << std::endl;
        }
    }
    return b;
}

void fill(std::string src_img, std::string dst_img, const bbox &b)
{
    cv::Mat src_mat = cv::imread(src_img);
    cv::Mat dst_mat = cv::imread(dst_img);

    int height = b.right_bottom_y - b.left_top_y;
    int width = b.right_bottom_x - b.left_top_x;

    cv::Mat rect = dst_mat(cv::Rect(b.left_top_x, b.left_top_y, width, height));

    cv::Mat src_mat_fill = cv::Mat(cv::Size(width, height), src_mat.type());
    cv::resize(src_mat, src_mat_fill, src_mat_fill.size());

    src_mat_fill.copyTo(rect);
}

int main()
{
    bbox x = get_bbox("boxes.json");
    std::cout << x.left_top_x << std::endl;
    return 0;
}
