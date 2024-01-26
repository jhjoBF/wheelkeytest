#ifndef LOGCONST_H_
#define LOGCONST_H_

#include <cstdint>

enum class ELogId : uint16_t {
    SystemStart = 0,
    SystemShutDown,
    SystemReboot,
    SystemUpgradeStart,
    SystemUpgradeFinish,
    SystemUpgradeFail,
    SystemDefault,
    SystemResetSetup,
    SystemSetupBackup,
    SystemSetupResotre,

    LoginSuccess,
    LoginFailed,
    UserLogOut,

    SetupChangedSystem,
    SetupChangedUser,
    SetupChangedMotion,
    SetupChangedAlarmIn,
    SetupChangedAlarmOut,
    SetupChangedNetwork,
    SetupChangedVideoStream,
    SetupChangedAudioStream,
    SetupChangedDateTime,

    NetworkDown,
    NetworkUp,
    VideoLoss,
    LogSearch,

    ScanStart,
    ScanStop,
    CloudScanStart,
    ConnectClient,
    DisconnectClient,
    RapLog,

    None
};

#endif  //LOGCONST_H_
