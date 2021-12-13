#include "mytools.h"
//#include "tools/tools.h"

#include <EGrabber.h>
#include <FormatConverter.h>

using namespace Euresys;

namespace {

class MyGrabber: public EGrabber<CallbackOnDemand>{//public EGrabber<CallbackOnDemand> { EGrabberCallbackSingleThread, 
    public:

        MyGrabber(EGenTL &gentl, int interfaceIndex, int deviceIndex)//EGrabber<CallbackOnDemand>(gentl, interfaceIndex, deviceIndex)//EGrabberCallbackSingleThread(gentl, interfaceIndex, deviceIndex)
        : EGrabber<CallbackOnDemand>(gentl, interfaceIndex, deviceIndex){
            reallocBuffers(10);
            enableEvent<DataStreamData>();
        }
        ~MyGrabber() {
            disableEvent<DataStreamData>();
            shutdown();
        }
        void go() {
            std::cout<<"Grabbing..."<<std::endl;
            start();
            try {
                for (int i = 0; i < 100; ++i) {
                    // get 100 events
                    // there will be new buffers and data stream events
                    processEvent< OneOf<NewBufferData, DataStreamData> >(10000);
                }
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

    private:
        //FormatConverter converter;
        //size_t frame;

        virtual void onNewBufferEvent(const NewBufferData& data) {
            
            /*ScopedBuffer buffer(*this, data); // wait and get a buffer
            // Note: ScopedBuffer pushes the buffer back to the input queue automatically
            uint8_t *imagePointer = buffer.getInfo<uint8_t *>(gc::BUFFER_INFO_BASE);
            // get the raw buffer image pointer and pass it to a BGR8 converter
            FormatConverter::Auto bgr(converter, FormatConverter::OutputFormat("BGR8"), imagePointer,
                buffer.getInfo<uint64_t>(gc::BUFFER_INFO_PIXELFORMAT),
                buffer.getInfo<size_t>(gc::BUFFER_INFO_WIDTH),
                buffer.getInfo<size_t>(gc::BUFFER_INFO_DELIVERED_IMAGEHEIGHT));
                */

            ScopedBuffer buffer(*this, data); // re-queues buffer automatically


            cv::Mat testframe(buffer.getInfo<size_t>(GenTL::BUFFER_INFO_HEIGHT),buffer.getInfo<size_t>(GenTL::BUFFER_INFO_WIDTH),CV_8U,buffer.getInfo<void*>(GenTL::BUFFER_INFO_BASE));
            cv::Mat resized_frame;
            resize(testframe, resized_frame, cv::Size(buffer.getInfo<size_t>(GenTL::BUFFER_INFO_WIDTH)/10, buffer.getInfo<size_t>(GenTL::BUFFER_INFO_HEIGHT)/10), cv::INTER_LINEAR);
            cv::imshow("Test Frame", resized_frame);
            cv::waitKey(1000);

            // output the converted buffer
            //bgr.saveToDisk(Tools::getEnv("sample-output-path") + "/frame.NNN.jpeg", frame++);
        }

        /*
        virtual void onDataStreamEvent(const DataStreamData &data) {
            std::cout<<"onDataStreamEvent"<<std::endl;
            switch (data.numid) {
                case ge::EVENT_DATA_NUMID_DATASTREAM_START_OF_CAMERA_READOUT:
                    std::cout<<"StartOfCameraReadout"<<std::endl;
                    break;
                case ge::EVENT_DATA_NUMID_DATASTREAM_END_OF_CAMERA_READOUT:
                    std::cout<<"EndOfCameraReadout"<<std::endl;
                    break;
                case ge::EVENT_DATA_NUMID_DATASTREAM_START_OF_SCAN:
                    std::cout<<"StartOfScan"<<std::endl;
                    break;
                case ge::EVENT_DATA_NUMID_DATASTREAM_END_OF_SCAN:
                    std::cout<<"EndOfScan"<<std::endl;
                    break;
                case ge::EVENT_DATA_NUMID_DATASTREAM_REJECTED_FRAME:
                    std::cout<<"RejectedFrame"<<std::endl;
                    break;
                case ge::EVENT_DATA_NUMID_DATASTREAM_REJECTED_SCAN:
                    std::cout<<"RejectedScan"<<std::endl;
                    break;
                default:
                    break;
            }
        }*/
};

class MyGrabberST: public EGrabberCallbackSingleThread {
    public:
        MyGrabberST(EGenTL &gentl, int interfaceIndex, int deviceIndex)
            : EGrabberCallbackSingleThread(gentl, interfaceIndex, deviceIndex) {
            reallocBuffers(10);
        }
        ~MyGrabberST() {
            shutdown();
        }
    private:
        virtual void onNewBufferEvent(const NewBufferData& data) {
            ScopedBuffer buffer(*this, data); // re-queues buffer automatically

            cv::Mat testframe(buffer.getInfo<size_t>(GenTL::BUFFER_INFO_HEIGHT),buffer.getInfo<size_t>(GenTL::BUFFER_INFO_WIDTH),CV_8U,buffer.getInfo<void*>(GenTL::BUFFER_INFO_BASE));
            cv::Mat resized_frame;
            resize(testframe, resized_frame, cv::Size(buffer.getInfo<size_t>(GenTL::BUFFER_INFO_WIDTH)/10, buffer.getInfo<size_t>(GenTL::BUFFER_INFO_HEIGHT)/10), cv::INTER_LINEAR);
            
            cv::imshow("Test Frame", resized_frame);
            cv::waitKey(1000);
            std::cout<<"got a buffer"<<std::endl;
/*
            std::cout<<"BUFFER_INFO_WIDTH - "<<buffer.getInfo<size_t>(GenTL::BUFFER_INFO_WIDTH)<<std::endl;
            std::cout<<"BUFFER_INFO_HEIGHT - "<<buffer.getInfo<size_t>(GenTL::BUFFER_INFO_HEIGHT)<<std::endl;
            std::cout<<"BUFFER_INFO_FRAMEID - "<<buffer.getInfo<size_t>(GenTL::BUFFER_INFO_FRAMEID)<<std::endl;
            std::cout<<"BUFFER_INFO_TIMESTAMP - "<<buffer.getInfo<size_t>(GenTL::BUFFER_INFO_TIMESTAMP)<<std::endl;

            std::cout<<"------------------------------------------------------"<<std::endl;*/
            //Tools::log("got a buffer from: " + interfaceId + " <" + deviceId +">\n");
        }
};

}

static void sample() {
    int FPS = 30;
    EGenTL genTL;
    MyGrabberST grabber(genTL,0,0);

    std::cout<<"Start!"<<std::endl;

    // camera configuration
    //grabber.runScript("config.js");

    grabber.setString<Euresys::RemoteModule>("TriggerMode", "Off");
    //std::cout<<"TriggerMode - "<<grabber.getString<RemoteModule>("TriggerMode")<<std::endl;
    
    //grabber.setString<Euresys::RemoteModule>("TriggerSource", "CXPin");
    //std::cout<<"TriggerSource - "<<grabber.getString<RemoteModule>("TriggerSource")<<std::endl;

    //std::cout<<"TriggerSource - "<<grabber.getString<RemoteModule>("TriggerSource")<<std::endl;

    grabber.setString<Euresys::InterfaceModule>("LineSelector", "TTLIO12");
    std::cout<<"LineSelector - "<<grabber.getString<InterfaceModule>("LineSelector")<<std::endl;

    grabber.setString<Euresys::InterfaceModule>("LineMode", "Input");    
    std::cout<<"LineMode - "<<grabber.getString<InterfaceModule>("LineMode")<<std::endl;

    grabber.setString<Euresys::InterfaceModule>("LineInputToolSelector", "LIN1"); // on grabber EuresysPattl/PattlInterface:0 (1-camera)
    std::cout<<"LineInputToolSelector - "<<grabber.getString<InterfaceModule>("LineInputToolSelector")<<std::endl;

    grabber.setString<Euresys::InterfaceModule>("LineInputToolSource", "TTLIO12"); // on grabber EuresysPattl/PattlInterface:0 (1-camera)
    std::cout<<"LineInputToolSource - "<<grabber.getString<InterfaceModule>("LineInputToolSource")<<std::endl;

    grabber.setString<Euresys::InterfaceModule>("LineInputToolActivation", "RisingEdge"); // on grabber EuresysPattl/PattlInterface:0 (1-camera)
    std::cout<<"LineInputToolActivation - "<<grabber.getString<InterfaceModule>("LineInputToolActivation")<<std::endl;

    //grabber.setString<Euresys::RemoteModule>("TriggerSource", "LIN1");                 // 2
    //grabber.setString<Euresys::RemoteModule>("ExposureMode", "TriggerWidth");           // 3
    // frame grabber configuration

    //std::cout<<"TriggerMode - "<<grabber.getString<RemoteModule>("TriggerMode")<<std::endl;
    //std::cout<<"TriggerSource - "<<grabber.getString<RemoteModule>("TriggerSource")<<std::endl;
    //std::cout<<"LineSelector - "<<grabber.getString<RemoteModule>("LineSelector")<<std::endl;
    //std::cout<<"LineStatus - "<<grabber.getString<RemoteModule>("LineStatus")<<std::endl;

    //grabber.setString<InterfaceModule>("LineSelector","IOUT12");
    //grabber.setString<InterfaceModule>("LineSelector","TTLIO11");
	//std::cout<<"LineInverter: True"<<std::endl;
	//grabber.setString<InterfaceModule>("LineInverter","True");

    //std::string DeviceIDstr = "Device1Strobe";
	//grabber.setString<Euresys::InterfaceModule>("LineSource",DeviceIDstr);

    int64_t counter=0;
    counter = grabber.getInteger<DeviceModule>("EventCount[CameraTriggerRisingEdge]");

    int d_width = grabber.getInteger<RemoteModule>("Width");
	int d_height = grabber.getInteger<RemoteModule>("Height");

	std::cout<<"Width: "<<d_width<<std::endl;
	std::cout<<"Height: "<<d_height<<std::endl;

    grabber.setString<Euresys::DeviceModule>("CameraControlMethod", "RC");              // 4
    grabber.setString<Euresys::DeviceModule>("CycleTriggerSource", "LIN1");
    std::cout<<"CycleTriggerSource - "<<grabber.getString<Euresys::DeviceModule>("CycleTriggerSource")<<std::endl;

    //grabber.setString<Euresys::DeviceModule>("CycleTriggerSource", "Immediate");
    grabber.setFloat<Euresys::DeviceModule>("CycleTargetPeriod", 1e6 / FPS);          // 6
    
    //std::cout<<"ExposureReadoutOverlap - True"<<std::endl;
    //grabber.setString<Euresys::DeviceModule>("ExposureReadoutOverlap","True");

    //std::cout<<"CycleMinimumPeriod - "<<1e6/(2*FPS)<<std::endl;
	//grabber.setInteger<Euresys::DeviceModule>("CycleMinimumPeriod",1e6/(2*FPS));

    //std::cout<<"CxpTriggerMessageFormat - Toggle"<<std::endl;
	//grabber.setString<Euresys::DeviceModule>("CxpTriggerMessageFormat","Toggle");

    std::cout<<"ExposureTime - "<<6000<<std::endl;
	grabber.setInteger<Euresys::DeviceModule>("ExposureTime",6000);

	grabber.setInteger<DeviceModule>("StrobeDuration",1500);

	//grabber.setInteger<DeviceModule>("StrobeDelay",0);

    //grabber.start(20);
    //usleep(5000000);
    
    grabber.start();
}

int main(int argc, char **argv)
{
    sample();
    std::cout<<"Finish!"<<std::endl;
}
