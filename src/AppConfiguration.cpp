#include "AppConfiguration.h"
#include "config.h"
#include "RTTTL.h"

AppConfiguration* AppConfiguration::instance = 0;

AppConfiguration* AppConfiguration::getInstance() {
    if (instance == 0){
        instance = new AppConfiguration();
    }

    return instance;
}
void AppConfiguration::readPreferences() {
    preferences.begin("rESCue", true);
    String json = preferences.getString("config", "");
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, json);
    Logger::verbose(LOG_TAG_APPCONFIGURATION, "readPreferences: ");
    Serial.println("readPreferences: " + json);
    config.otaUpdateActive = doc["otaUpdateActive"] | false;
    config.isNotificationEnabled = doc["isNotificationEnabled"] | false;
    config.minBatteryVoltage = doc["minBatteryVoltage"] | MIN_BATTARY_VOLTAGE;
    config.maxBatteryVoltage = doc["maxBatteryVoltage"] | MAX_BATTARY_VOLTAGE;
    config.startSoundIndex = doc["startSoundIndex"] | (int)RTTTL_MELODIES::NO_TONE;
    config.startLightIndex = doc["startLightIndex"] | 1;
    config.batteryWarningSoundIndex = doc["batteryWarningSoundIndex"] | 406;
    config.batteryAlarmSoundIndex = doc["batteryAlarmSoundIndex"] | 402;
    config.startLightDuration = doc["startLightDuration"] | 1000;
    config.lightColorPrimary = doc["lightColorPrimary"] | 0xFFFFFF;
    config.lightColorSecondary = doc["lightColorSecondary"] | 0xFF0000;
    config.idleLightIndex = doc["idleLightIndex"] | 0;
    config.lightFadingDuration = doc["lightFadingDuration"] | 220;
    config.lightMaxBrightness = doc["lightMaxBrightness"] | MAX_BRIGHTNESS;
    config.brakeLightEnabled = doc["brakeLightEnabled"] | true;
    config.brakeLightMinAmp = doc["brakeLightMinAmp"] | 4;
    config.authToken = doc["authToken"] | BLYNK_AUTH_TOKEN;
    config.vescId = doc["vescId"] | VESC_CAN_ID;
    config.numberPixelLight = doc["numberPixelLight"] | NUMPIXELS;
    config.numberPixelBatMon = doc["numberPixelBatMon"] | LIGHT_BAR_NUMPIXELS;
    // calculate RGB values for primary and secondary color
    config.lightColorPrimaryRed = (config.lightColorPrimary >> 16) & 0x0ff;
    config.lightColorPrimaryGreen = (config.lightColorPrimary >> 8) & 0x0ff;
    config.lightColorPrimaryBlue =  config.lightColorPrimary & 0x0ff;
    config.lightColorSecondaryRed = (config.lightColorSecondary >> 16) & 0x0ff;
    config.lightColorSecondaryGreen = (config.lightColorSecondary >> 8) & 0x0ff;
    config.lightColorSecondaryBlue = config.lightColorSecondary & 0x0ff;
    config.logLevel = doc["logLevel"] | Logger::NOTICE;
    preferences.end();
}

void AppConfiguration::savePreferences() {
    preferences.begin("rESCue", false);
    StaticJsonDocument<1024> doc;
    doc["otaUpdateActive"] = config.otaUpdateActive;
    doc["isNotificationEnabled"] = config.isNotificationEnabled;
    doc["minBatteryVoltage"] = config.minBatteryVoltage;
    doc["maxBatteryVoltage"] = config.maxBatteryVoltage;
    doc["startSoundIndex"] = config.startSoundIndex;
    doc["startLightIndex"] = config.startLightIndex;
    doc["batteryWarningSoundIndex"] = config.batteryWarningSoundIndex;
    doc["batteryAlarmSoundIndex"] = config.batteryAlarmSoundIndex;
    doc["startLightDuration"] = config.startLightDuration;
    doc["lightColorPrimary"] = config.lightColorPrimary;
    doc["lightColorSecondary"] = config.lightColorSecondary;
    doc["idleLightIndex"] = config.idleLightIndex;
    doc["lightFadingDuration"] = config.lightFadingDuration;
    doc["lightMaxBrightness"] = config.lightMaxBrightness;
    doc["brakeLightEnabled"] = config.brakeLightEnabled;
    doc["brakeLightMinAmp"] = config.brakeLightMinAmp;
    doc["authToken"] = config.authToken;
    doc["vescId"] = config.vescId;
    doc["numberPixelLight"] = config.numberPixelLight;
    doc["numberPixelBatMon"] = config.numberPixelBatMon;
    doc["logLevel"] = config.logLevel;
    String json = "";
    serializeJson(doc, json);
    Logger::verbose(LOG_TAG_APPCONFIGURATION, "savePreferences: ");
    Serial.println("savePreferences: " + json);
    preferences.putString("config", json);
    preferences.end();
}

