/*
 * Copyright 2022 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common/xf_headers.hpp"
#include "xf_gammacorrection_tb_config.h"
#include "xcl2.hpp"

void compute_gamma(float r_g, float g_g, float b_g, uchar gamma_lut[256 * 3]) {
    float gamma_inv[256] = {
        0.000000, 0.003922, 0.007843, 0.011765, 0.015686, 0.019608, 0.023529, 0.027451, 0.031373, 0.035294, 0.039216,
        0.043137, 0.047059, 0.050980, 0.054902, 0.058824, 0.062745, 0.066667, 0.070588, 0.074510, 0.078431, 0.082353,
        0.086275, 0.090196, 0.094118, 0.098039, 0.101961, 0.105882, 0.109804, 0.113725, 0.117647, 0.121569, 0.125490,
        0.129412, 0.133333, 0.137255, 0.141176, 0.145098, 0.149020, 0.152941, 0.156863, 0.160784, 0.164706, 0.168627,
        0.172549, 0.176471, 0.180392, 0.184314, 0.188235, 0.192157, 0.196078, 0.200000, 0.203922, 0.207843, 0.211765,
        0.215686, 0.219608, 0.223529, 0.227451, 0.231373, 0.235294, 0.239216, 0.243137, 0.247059, 0.250980, 0.254902,
        0.258824, 0.262745, 0.266667, 0.270588, 0.274510, 0.278431, 0.282353, 0.286275, 0.290196, 0.294118, 0.298039,
        0.301961, 0.305882, 0.309804, 0.313725, 0.317647, 0.321569, 0.325490, 0.329412, 0.333333, 0.337255, 0.341176,
        0.345098, 0.349020, 0.352941, 0.356863, 0.360784, 0.364706, 0.368627, 0.372549, 0.376471, 0.380392, 0.384314,
        0.388235, 0.392157, 0.396078, 0.400000, 0.403922, 0.407843, 0.411765, 0.415686, 0.419608, 0.423529, 0.427451,
        0.431373, 0.435294, 0.439216, 0.443137, 0.447059, 0.450980, 0.454902, 0.458824, 0.462745, 0.466667, 0.470588,
        0.474510, 0.478431, 0.482353, 0.486275, 0.490196, 0.494118, 0.498039, 0.501961, 0.505882, 0.509804, 0.513725,
        0.517647, 0.521569, 0.525490, 0.529412, 0.533333, 0.537255, 0.541176, 0.545098, 0.549020, 0.552941, 0.556863,
        0.560784, 0.564706, 0.568627, 0.572549, 0.576471, 0.580392, 0.584314, 0.588235, 0.592157, 0.596078, 0.600000,
        0.603922, 0.607843, 0.611765, 0.615686, 0.619608, 0.623529, 0.627451, 0.631373, 0.635294, 0.639216, 0.643137,
        0.647059, 0.650980, 0.654902, 0.658824, 0.662745, 0.666667, 0.670588, 0.674510, 0.678431, 0.682353, 0.686275,
        0.690196, 0.694118, 0.698039, 0.701961, 0.705882, 0.709804, 0.713725, 0.717647, 0.721569, 0.725490, 0.729412,
        0.733333, 0.737255, 0.741176, 0.745098, 0.749020, 0.752941, 0.756863, 0.760784, 0.764706, 0.768627, 0.772549,
        0.776471, 0.780392, 0.784314, 0.788235, 0.792157, 0.796078, 0.800000, 0.803922, 0.807843, 0.811765, 0.815686,
        0.819608, 0.823529, 0.827451, 0.831373, 0.835294, 0.839216, 0.843137, 0.847059, 0.850980, 0.854902, 0.858824,
        0.862745, 0.866667, 0.870588, 0.874510, 0.878431, 0.882353, 0.886275, 0.890196, 0.894118, 0.898039, 0.901961,
        0.905882, 0.909804, 0.913725, 0.917647, 0.921569, 0.925490, 0.929412, 0.933333, 0.937255, 0.941176, 0.945098,
        0.949020, 0.952941, 0.956863, 0.960784, 0.964706, 0.968627, 0.972549, 0.976471, 0.980392, 0.984314, 0.988235,
        0.992157, 0.996078, 1.000000};

    unsigned char gam_r = 0, gam_g = 0, gam_b = 0;

    for (int i = 0; i < 256; ++i) {
        float r_inv = (float)1 / r_g;
        float g_inv = (float)1 / g_g;
        float b_inv = (float)1 / b_g;
        float powval_r = (float)std::pow(gamma_inv[i], r_inv);
        short tempgamma_r = (powval_r * 255.0);

        if (tempgamma_r > 255) {
            gam_r = 255;
        } else {
            gam_r = tempgamma_r;
        }

        float powval_g = (float)std::pow(gamma_inv[i], g_inv);
        short tempgamma_g = (powval_g * 255.0);

        if (tempgamma_g > 255) {
            gam_g = 255;
        } else {
            gam_g = tempgamma_g;
        }

        float powval_b = (float)std::pow(gamma_inv[i], b_inv);
        short tempgamma_b = (powval_b * 255.0);

        if (tempgamma_b > 255) {
            gam_b = 255;
        } else {
            gam_b = tempgamma_b;
        }
        gamma_lut[i] = gam_r;
        gamma_lut[i + 256] = gam_g;
        gamma_lut[i + 512] = gam_b;
    }
}

void gammacorrection_ref(
    cv::Mat in_gray, cv::Mat ocv_ref, uchar gam_r[256], uchar gam_g[256], uchar gam_b[256], int height, int width) {
    uchar in_r = 0, in_g = 0, in_b = 0;
    for (int x = 0; x < width; x++) {      // width = cols = x
        for (int y = 0; y < height; y++) { // height = rows = y
            in_r = in_gray.at<cv::Vec3b>(y, x)[0];
            in_g = in_gray.at<cv::Vec3b>(y, x)[1];
            in_b = in_gray.at<cv::Vec3b>(y, x)[2];
            ocv_ref.at<cv::Vec3b>(y, x)[0] = gam_r[in_r];
            ocv_ref.at<cv::Vec3b>(y, x)[1] = gam_g[in_g];
            ocv_ref.at<cv::Vec3b>(y, x)[2] = gam_b[in_b];
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid Number of Arguments!\nUsage:\n");
        fprintf(stderr, "<Executable Name> <input image path1> \n");
        return -1;
    }

    cv::Mat in_gray, in_gray1, ocv_ref, out_gray, diff, ocv_ref_in1, ocv_ref_in2, inout_gray1;
    in_gray = cv::imread(argv[1], 1); // read image
    if (in_gray.data == NULL) {
        fprintf(stderr, "Cannot open image %s\n", argv[1]);
        return -1;
    }

    ocv_ref.create(in_gray.rows, in_gray.cols, CV_8UC3);
    out_gray.create(in_gray.rows, in_gray.cols, CV_8UC3);
    diff.create(in_gray.rows, in_gray.cols, CV_8UC3);

    // float gamma_ = auto_gamma_value(in_gray);

    unsigned char gamma_lut[256 * 3];

    float gamma_val_r = 0.5f, gamma_val_g = 0.8f, gamma_val_b = 0.8f;

    compute_gamma(gamma_val_r, gamma_val_g, gamma_val_b, gamma_lut);

    cv::imwrite("in_hls.jpg", in_gray);

    int height = in_gray.rows;
    int width = in_gray.cols;
    size_t vec_in_size_bytes = 256 * 3 * sizeof(unsigned char);

    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];
    cl::Context context(device);
    std::cout << "Input image height : " << height << std::endl;
    std::cout << "Input image width  : " << width << std::endl;
    std::cout << "Input Image Bit Depth:" << XF_DTPIXELDEPTH(IN_TYPE, NPPCX) << std::endl;
    std::cout << "Input Image Channels:" << XF_CHANNELS(IN_TYPE, NPPCX) << std::endl;
    std::cout << "NPPC:" << NPPCX << std::endl;

    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);

    std::string device_name = device.getInfo<CL_DEVICE_NAME>();
    std::string binaryFile = xcl::find_binary_file(device_name, "krnl_gammacorrection");
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    cl::Program program(context, devices, bins);
    cl::Kernel krnl(program, "gammacorrection_accel");

    std::vector<cl::Memory> inBufVec, outBufVec;
    cl::Buffer imageToDevice(context, CL_MEM_READ_ONLY, (height * width * 3));
    cl::Buffer imageFromDevice(context, CL_MEM_WRITE_ONLY, (height * width * 3));
    cl::Buffer buffer_inVec(context, CL_MEM_READ_ONLY, vec_in_size_bytes);
    // Set the kernel arguments
    krnl.setArg(0, imageToDevice);
    krnl.setArg(1, imageFromDevice);
    krnl.setArg(2, buffer_inVec);
    krnl.setArg(3, height);
    krnl.setArg(4, width);

    q.enqueueWriteBuffer(imageToDevice, CL_TRUE, 0, (height * width * 3), in_gray.data);
    q.enqueueWriteBuffer(buffer_inVec, CL_TRUE, 0, vec_in_size_bytes, gamma_lut);

    // Profiling Objects
    cl_ulong start = 0;
    cl_ulong end = 0;
    double diff_prof = 0.0f;
    cl::Event event_sp;

    printf("\nbefore kernel\n");
    // Launch the kernel
    q.enqueueTask(krnl, NULL, &event_sp);
    clWaitForEvents(1, (const cl_event*)&event_sp);

    printf("\nafter kernel\n");

    event_sp.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
    event_sp.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
    diff_prof = end - start;
    std::cout << (diff_prof / 1000000) << "ms" << std::endl;

    // Copying Device result data to Host memory
    q.enqueueReadBuffer(imageFromDevice, CL_TRUE, 0, (height * width * 3), out_gray.data);

    q.finish();

    printf("\nafter cl flow\n");

    cv::imwrite("out_hls.jpg", out_gray);

    // ------------------------------------------------------
    // OpenCV function
    unsigned char gam_r[256], gam_g[256], gam_b[256];
    for (int i = 0; i < 256; i++) {
        gam_r[i] = gamma_lut[i];
        gam_g[i] = gamma_lut[i + 256];
        gam_b[i] = gamma_lut[i + 512];
    }

    gammacorrection_ref(in_gray, ocv_ref, gam_r, gam_g, gam_b, height, width);

    // Write OpenCV reference image
    cv::imwrite("out_ocv.jpg", ocv_ref);

    // Compute absolute difference image
    absdiff(out_gray, ocv_ref, diff);

    // Save the difference image
    imwrite("diff.png", diff);

    // Results verification:

    float err_per;
    static constexpr int ERROR_THRESHOLD = 1;
    xf::cv::analyzeDiff(diff, ERROR_THRESHOLD, err_per);

    if (err_per > 0.0f) {
        std::cout << "Testcase failed" << std::endl;
        return 1;
    }

    std::cout << "Testcase passed" << std::endl;
    return 0;
}
