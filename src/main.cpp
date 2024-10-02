#include <Arduino.h>
#include <i2c_device.h>
#include "max98389.h"
#include <Audio.h>

const unsigned long amp_config_poll_time_ms = 500;

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=188,240
AudioEffectEnvelope      envelope1;      //xy=371,237

AudioInputI2SQuad        i2s_quad1;      //xy=301,598
AudioOutputI2S           i2s2;           //xy=565,241
AudioInputUSB            usb1;           //xy=157,294
AudioOutputUSB           usb2;           //xy=405,293

AudioConnection          patchCord1(waveform1, envelope1);
AudioConnection          patchCord2(usb1, 0, i2s2, 0);
AudioConnection          patchCord3(usb1, 1, i2s2, 1);
AudioConnection          patchCord4(i2s_quad1, 2, usb2, 0);
AudioConnection          patchCord5(i2s_quad1, 3, usb2, 1);

bool configured = false;
unsigned long configuration_time_counter = 0;

max98389* p_max;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    // Enable the serial port for debugging
    Serial.begin(9600);
    Serial.println("Started");
    
    static max98389 max;
    p_max = &max;
    max.master.set_internal_pullups(InternalPullup::disabled);
    max.begin(400 * 1000U);
    // Check that we can see the sensor and configure it.
    configured = max.configure();
    if (configured) {
        Serial.println("Configured");
    } else {
        Serial.println("Not configured");
    }

    AudioMemory(128);

    //waveform1.pulseWidth(0.5);
    waveform1.begin(0.1, 306, WAVEFORM_SINE);

    envelope1.attack(0);
    envelope1.decay(0);
    envelope1.release(0);

    envelope1.noteOn();
}



void loop() {

    unsigned long current_millis = millis();
    if (current_millis > (configuration_time_counter + amp_config_poll_time_ms)){
        configuration_time_counter = current_millis;
        bool amp_present_and_correct_revision = p_max->isAvailable();
        if (amp_present_and_correct_revision && !configured){ //If amp found and currently not configured
            Serial.println("Amp found on i2c bus. Configuring...");
            configured = p_max->configure();
        }
        else if ((!amp_present_and_correct_revision) && configured){ //If amp lost and currently configured
            configured = false; //Assume amp has lost power and is no longer configured
            Serial.println("Amp has been lost on the i2c bus");
        }

        p_max->globalEnableStatus();
    }
}