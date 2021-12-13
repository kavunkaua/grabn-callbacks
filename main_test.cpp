#include "mytools.h"


using namespace Euresys;

static void sample() {
    EGenTL genTL; // load GenTL producer
    EGrabber<CallbackOnDemand> grabber(genTL,0, 0); // create grabber
    
    //grabber.runScript("config.js");

    grabber.setString<Euresys::RemoteModule>("TriggerMode", "On");

    //grabber.setString<Euresys::RemoteModule>("TriggerSource", "CXPin");

    grabber.setString<Euresys::InterfaceModule>("LineSelector", "TTLIO11");
    std::cout<<"LineSelector - "<<grabber.getString<InterfaceModule>("LineSelector")<<std::endl;

    grabber.setString<Euresys::InterfaceModule>("LineMode", "Input");    
    std::cout<<"LineMode - "<<grabber.getString<InterfaceModule>("LineMode")<<std::endl;

    grabber.setString<Euresys::InterfaceModule>("LineInputToolSelector", "LIN1"); // on grabber EuresysPattl/PattlInterface:0 (1-camera)
    std::cout<<"LineInputToolSelector - "<<grabber.getString<InterfaceModule>("LineInputToolSelector")<<std::endl;

    grabber.setString<Euresys::InterfaceModule>("LineInputToolSource", "TTLIO11"); // on grabber EuresysPattl/PattlInterface:0 (1-camera)
    std::cout<<"LineInputToolSource - "<<grabber.getString<InterfaceModule>("LineInputToolSource")<<std::endl;

    grabber.setString<Euresys::InterfaceModule>("LineInputToolActivation", "RisingEdge"); // on grabber EuresysPattl/PattlInterface:0 (1-camera)
    std::cout<<"LineInputToolActivation - "<<grabber.getString<InterfaceModule>("LineInputToolActivation")<<std::endl;

    grabber.setString<Euresys::DeviceModule>("CameraControlMethod", "RC");
    grabber.setString<Euresys::DeviceModule>("CycleTriggerSource", "LIN1");

    std::cout<<"CycleTriggerSource - "<<grabber.getString<DeviceModule>("CycleTriggerSource")<<std::endl;

    std::cout<<"ExposureTime - "<<6000<<std::endl;
	grabber.setInteger<Euresys::DeviceModule>("ExposureTime",6000);
    
    std::cout<<"Start"<<std::endl;
   
    grabber.reallocBuffers(20); // prepare 20 buffers
    
    for (size_t frame = 0; frame < 20; ++frame) {
        grabber.start(1);
        std::cout<<"frame - "<<frame<<std::endl;
        int64_t counterEventCount = grabber.getInteger<DeviceModule>("EventCount[CameraTriggerRisingEdge]");
        std::cout<<"EventCount[CameraTriggerRisingEdge] - "<<counterEventCount<<std::endl;
        int64_t counterCycleLostTriggerCount = grabber.getInteger<DeviceModule>("CycleLostTriggerCount");
        std::cout<<"CycleLostTriggerCount - "<<counterCycleLostTriggerCount<<std::endl;
        //usleep(1000000);
    
    /*
        Buffer buffer(grabber.pop());
        std::cout<<"buffer - OK!"<<std::endl;
        uint8_t *imagePointer = buffer.getInfo<uint8_t *>(grabber, gc::BUFFER_INFO_BASE);
        buffer.getInfo<size_t>(grabber, gc::BUFFER_INFO_WIDTH);
        cv::Mat testframe(buffer.getInfo<size_t>(grabber,gc::BUFFER_INFO_HEIGHT),buffer.getInfo<size_t>(grabber,gc::BUFFER_INFO_WIDTH),CV_8U,imagePointer);
        cv::Mat resized_frame;
        resize(testframe, resized_frame, cv::Size(buffer.getInfo<size_t>(grabber,gc::BUFFER_INFO_WIDTH)/10, buffer.getInfo<size_t>(grabber,gc::BUFFER_INFO_HEIGHT)/10), cv::INTER_LINEAR);
        cv::imshow("Test Frame", resized_frame);
        cv::waitKey(1000);
    */    
    try {
            ScopedBuffer buffer(grabber,1000); // re-queues buffer automatically
            std::cout<<"buffer - OK!"<<std::endl;
            cv::Mat testframe(buffer.getInfo<size_t>(GenTL::BUFFER_INFO_HEIGHT),buffer.getInfo<size_t>(GenTL::BUFFER_INFO_WIDTH),CV_8U,buffer.getInfo<void*>(GenTL::BUFFER_INFO_BASE));
            cv::Mat resized_frame;
            resize(testframe, resized_frame, cv::Size(buffer.getInfo<size_t>(GenTL::BUFFER_INFO_WIDTH)/10, buffer.getInfo<size_t>(GenTL::BUFFER_INFO_HEIGHT)/10), cv::INTER_LINEAR);
            cv::imshow("Test Frame", resized_frame);
            cv::waitKey(1000);
        }
        catch (const gentl_error &err) {
                if (err.gc_err == gc::GC_ERR_TIMEOUT) {
                    std::cout<<"Timeout -> event loop stopped"<<std::endl;
                } else {
                    std::cout<<"GenTL exception: "<<err.what()<<std::endl;
                    throw;
                }
            }
    }
}

int main(int argc, char **argv) {
    sample();
    std::cout<<"Finish!"<<std::endl;
}