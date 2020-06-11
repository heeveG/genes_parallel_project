#include <CL/sycl.hpp>
#include <CL/sycl/intel/fpga_extensions.hpp>
#include "../headers/device_selector.h"
#include "../headers/oneapi_parallel.h"

class dummy;

void oneapi_find_markers(concurrent_que<std::string> *qFasta, std::vector <std::vector<size_t>> &matrix,
                         aho_corasick::AhoCorasick *a) {

    auto exception_handler = [](cl::sycl::exception_list exceptionList) {
        for (std::exception_ptr const &e : exceptionList) {
            try {
                std::rethrow_exception(e);
            } catch (cl::sycl::exception const &e) {
                std::terminate();
            }
        }
    };


    MyDeviceSelector sel;


    try {
        auto propList =
                cl::sycl::property_list{cl::sycl::property::queue::enable_profiling()};

        cl::sycl::queue q(sel, exception_handler, propList);


        std::cout << "Running on "

                  << q.get_device().get_info<cl::sycl::info::device::name>()

                  << "\n";

        cl::sycl::buffer<aho_corasick::Node, 1> aho_buf(a->nodes, cl::sycl::range<1>(a->nodeNum));

        std::string tempFasta;
        int len;
        std::vector <size_t> vec;
        while (qFasta->back() != " ") {

            vec.clear();
            vec.resize(a->numMarkers);
            std::fill(vec.begin(), vec.end(), 0);
            tempFasta = qFasta->pop();
            len = tempFasta.size();
            cl::sycl::buffer<char, 1> fasta_buf(tempFasta.data(), cl::sycl::range<1>(len));
            cl::sycl::buffer<size_t, 1> vec_buf(vec.data(), cl::sycl::range<1>(a->numMarkers));


            std::cout << "submitting lambda kernel..." << std::endl;


            q.submit([&](cl::sycl::handler &cgh) {
//            cl::sycl::stream out(1024, 256, cgh);
                auto string_acc = fasta_buf.get_access<cl::sycl::access::mode::read>(cgh);
                auto vec_acc = vec_buf.get_access<cl::sycl::access::mode::write>(cgh);
                auto aho_acc = aho_buf.get_access<cl::sycl::access::mode::read>(cgh);

                cgh.parallel_for<class dummy>(cl::sycl::range<1>(len), [=](cl::sycl::item<1> i) {

                    int num = i.get_linear_id();

                    aho_corasick::Node node;
                    node = aho_acc[0];
                    while (num < len) {
                        char letter = string_acc[num];
                        if (letter == 'N') {
                            break;
                        }
                        int nodesIndex = node.children[abs((letter - 65) / 2 % 5 - 1)];
                        if (nodesIndex == 0) {
                            break;
                        }
                        node = aho_acc[nodesIndex];

//                    out << letter << " " << node.markerHash << cl::sycl::endl;
                        if (node.markerHash) {
//                        out << node.markerId << " " << node.markerHash << sycl::endl;
                            vec_acc[node.markerId] = node.markerHash;
                        }
                        ++num;
                    }

                });
            });
            q.wait_and_throw();
            matrix.emplace_back(std::move(vec));
        }

    } catch (cl::sycl::exception
             e) {

        std::cout << "SYCL exception caught: " << e.what() << std::endl;
        exit(1);
    }

    int count = 0;
    for (auto &v: matrix) {
        for (auto &i: v) {
            if (i != 0) {
                for (auto &j: a->markerIdMap[i]) {
                    v[j] = 1;
                    ++count;
                }
            }
        }
        std::cout << count << std::endl;
        count = 0;
    }


}
