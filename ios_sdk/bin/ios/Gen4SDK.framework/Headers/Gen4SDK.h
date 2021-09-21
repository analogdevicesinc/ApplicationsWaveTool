//
//  Gen4SDK.h
//  Gen4SDK
//
//  Created by UX TEAM on 03/01/18.
//  Copyright © 2018 UX TEAM. All rights reserved.
//

#import <UIKit/UIKit.h>
//! Project version number for Gen3SDK.
FOUNDATION_EXPORT double Gen3SDKVersionNumber;

//! Project version string for Gen3SDK.
FOUNDATION_EXPORT const unsigned char Gen3SDKVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import "Gen3SDK/PublicHeader.h"


// Include all the inc header file need to be compiled
#import "inc/bcm_application.hpp"
#import "inc/sqi_application.hpp"
#import "inc/display_application.hpp"
#import "inc/lowTouch_application.hpp"
#import "inc/adxl_application.hpp"
#import "inc/adpd4000_application.hpp"

#import "inc/blocking_queue.hpp"
#import "inc/byte_swap.hpp"
#import "inc/callback.hpp"
#import "inc/ecg_application.hpp"
#import "inc/eda_application.hpp"
#import "inc/fs_application.hpp"
#import "inc/git_version.hpp"
#import "inc/m2m2_application.hpp"
#import "inc/m2m2_data_stream.hpp"
#import "inc/ppg_application.hpp"
#import "inc/sdk_err.hpp"
#import "inc/sys_alert_enum.hpp"

#import "inc/syncppg_application.hpp"
#import "inc/temperature_application.hpp"
#import "inc/util.hpp"
#import "inc/pedometer_application.hpp"
#import "inc/pm_application.hpp"
#import "inc/watch.hpp"

// Include all the m2m2 interface header file need to be compiled
#import "inc/m2m2/cpp/bcm_application_interface.hpp"

#import "inc/m2m2/cpp/common_application_interface.hpp"
#import "inc/m2m2/cpp/common_sensor_interface.hpp"
#import "inc/m2m2/cpp/dcb_interface.hpp"

#import "inc/m2m2/cpp/debug_interface.hpp"
#import "inc/m2m2/cpp/display_interface.hpp"

#import "inc/m2m2/cpp/ecg_application_interface.hpp"
#import "inc/m2m2/cpp/eda_application_interface.hpp"
#import "inc/m2m2/cpp/file_system_interface.hpp"
#import "inc/m2m2/cpp/led_interface.hpp"

#import "inc/m2m2/cpp/m2m2_core.hpp"
#import "inc/m2m2/cpp/pedometer_application_interface.hpp"
#import "inc/m2m2/cpp/post_office_interface.hpp"
#import "inc/m2m2/cpp/ppg_application_interface.hpp"
#import "inc/m2m2/cpp/sqi_application_interface.hpp"

#import "inc/m2m2/cpp/sensor_adpd_application_interface.hpp"
#import "inc/m2m2/cpp/sensor_adxl_application_interface.hpp"
#import "inc/m2m2/cpp/sync_data_application_interface.hpp"
#import "inc/m2m2/cpp/temperature_application_interface.hpp"
#import "inc/m2m2/cpp/watchdog_interface.hpp"

#import "inc/m2m2/cpp/system_interface.hpp"
