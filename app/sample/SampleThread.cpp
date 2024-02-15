#include "SampleThread.h"
#include "Spec.h"
#include "SetupN.h"
#include "AlarmInCategoryN.h"
#include "AlarmOutCategoryN.h"
#include "EmailCategoryN.h"
#include "MotionCategoryN.h"

SampleThread::SampleThread()
: AbstractThread(300)
{
    SetupN::instance()->addObserver(_systemCate.index(), this);
}
SampleThread::~SampleThread()
{
}
void SampleThread::worker()
{
    //int sleep = 10;
    //std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
    static int cnt = 0;
    if((cnt % 10) == 0) {
        if(cnt == 10) {
            SetupN* setup = SetupN::instance();
            SystemCategoryN systemCategory;
            setup->getCategory(&systemCategory);
            systemCategory._cameraName = "aaaaa";
            systemCategory._language = 0;
            setup->setCategory(systemCategory);

            for(int i=0 ; i < Spec::instance()->numberOfVideos(); i++) {
                VideoCategoryN videoCategory(i);
                setup->getCategory(&videoCategory);
                videoCategory._bitrate = 4000;
                videoCategory._frameRate = 30;
                videoCategory._interval = 30;
                videoCategory._videoCodec = VideoCodec::H264_MAIN;
                videoCategory._videoEncodeMode = VideoEncodeMode::CBR;
                if(0) {
                //if(i == 0) {
                    videoCategory._width = 1920;
                    videoCategory._height = 1080;
                    videoCategory._frameRate = 5;
                }
                setup->setCategory(videoCategory);
            } 
            for(int i=0 ; i < Spec::instance()->numberOfAlarmIns(); i++) {
                AlarmInCategoryN alarmInCategory(i);
                setup->getCategory(&alarmInCategory);
                alarmInCategory._enable = true;
                if(i == 1)
                    alarmInCategory._enable = false;
                alarmInCategory._mask |= static_cast<uint32_t>(EEventExecuterMask::EmailMask);
                alarmInCategory._mask |= static_cast<uint32_t>(EEventExecuterMask::AlarmMask);
                alarmInCategory._duration = 3 * 1000;
                //alarmInCategory._thursday = 0;
                alarmInCategory._scheduleCate._thursday = 0;
                alarmInCategory._scheduleCate._thursday |= 0x01 << 4;
                //alarmInCategory._scheduleCate._thursday = 0xffffffff;
                setup->setCategory(alarmInCategory);
            }
            
            for(int i=0 ; i < Spec::instance()->numberOfAlarmOuts(); i++) {
                AlarmOutCategoryN alarmOutCategory(i);
                setup->getCategory(&alarmOutCategory);
                alarmOutCategory._enable = true;
                if(i == 1)
                    alarmOutCategory._enable = false;
                alarmOutCategory._nOpen = true;
                setup->setCategory(alarmOutCategory);
            }
            
            for(int i=0 ; i < Spec::instance()->numberOfMotions(); i++) {
                MotionCategoryN motionCategory(i);
                setup->getCategory(&motionCategory);
                motionCategory._enable = false;
                if(i == 0) {
                    motionCategory._enable = true;
                    motionCategory._duration = 1 * 1000;
                }
                motionCategory._mask |= static_cast<uint32_t>(EEventExecuterMask::EmailMask);
                motionCategory._mask |= static_cast<uint32_t>(EEventExecuterMask::AlarmMask);
                setup->setCategory(motionCategory);
            }
            
            EmailCategoryN emailCategory;
            setup->getCategory(&emailCategory);
            emailCategory._serverAddress = "www.google.com";
            setup->setCategory(emailCategory);
        }
        if(0) {
        //if(cnt % 100 == 0) {
            static int rate = 30;
            SetupN* setup = SetupN::instance();
            VideoCategoryN videoCategory(0);
            setup->getCategory(&videoCategory);
            videoCategory._width = 1280;
            videoCategory._height = 720;
            if(rate > 2) {
                rate -= 2;
                videoCategory._frameRate = rate;
            }
            setup->setCategory(videoCategory);
        }
    }
    cnt++;
}   
void SampleThread::update(AbstractSetupCategoryN* cate)
{
    if(cate->index() == _systemCate.index()){
        cate->get(&_systemCate);
        printf("%s %s %d,_cameraName %s!!\n", __FILE__, __func__, __LINE__, _systemCate._cameraName.c_str());
    }
}
