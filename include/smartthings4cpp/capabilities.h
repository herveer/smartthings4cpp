#pragma once

/**
 * @file capabilities.h
 * @brief Convenience header that includes every typed capability class
 */

#include "capability.h"
#include "capabilities/switch.h"
#include "capabilities/switch_level.h"
#include "capabilities/audio_volume.h"
#include "capabilities/audio_mute.h"
#include "capabilities/contact_sensor.h"
#include "capabilities/temperature_measurement.h"
#include "capabilities/thermostat_cooling_setpoint.h"
#include "capabilities/refresh.h"
#include "capabilities/media_playback.h"
#include "capabilities/media_track_control.h"
#include "capabilities/media_input_source.h"
#include "capabilities/tv_channel.h"
#include "capabilities/power_consumption_report.h"
#include "capabilities/remote_control_status.h"
#include "capabilities/demand_response_load_control.h"
#include "capabilities/washer_operating_state.h"
#include "capabilities/refrigeration.h"
#include "capabilities/execute.h"
#include "capabilities/ocf.h"

// Samsung-proprietary capabilities (generated; grouped by namespace prefix)
#include "capabilities/samsungce.h"
#include "capabilities/samsungvd.h"
#include "capabilities/custom.h"
#include "capabilities/sec.h"
#include "capabilities/hca.h"
#include "capabilities/samsungim.h"

#include "capabilities/unknown_capability.h"
