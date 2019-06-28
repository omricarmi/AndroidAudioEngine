/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.google.sample.oboe.liveEffect;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.os.Build;
import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import com.google.sample.audio_device.AudioDeviceListEntry;
import com.google.sample.audio_device.AudioDeviceSpinner;

import java.util.ArrayList;
import java.util.List;

/**
 * TODO: Update README.md and go through and comment sample
 */
public class MainActivity extends Activity
        implements ActivityCompat.OnRequestPermissionsResultCallback {

    private static final String TAG = MainActivity.class.getName();
    private static final int AUDIO_EFFECT_REQUEST = 0;
    private static final int OBOE_API_AAUDIO = 0;
    private static final int OBOE_API_OPENSL_ES=1;

    private TextView statusText;
    private Button toggleEffectButton;
    private AudioDeviceSpinner recordingDeviceSpinner;
    private AudioDeviceSpinner playbackDeviceSpinner;
    private boolean isPlaying = false;

    private int apiSelection = OBOE_API_AAUDIO;
    private boolean aaudioSupported = true;
    private Spinner sampleRatesSpinner;
    private Switch switchBlockData;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Sample Rate - START
        List<Integer> sampleRatesList = getSupportedSampleRates();
        sampleRatesSpinner = findViewById(R.id.spinner_sample_rates);
        // Create an ArrayAdapter using the string array and a default spinner layout
        ArrayAdapter<Integer> adapter = new ArrayAdapter<Integer>(this,android.R.layout.simple_spinner_item,sampleRatesList);
        // Specify the layout to use when the list of choices appears
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        // Apply the adapter to the spinner
        sampleRatesSpinner.setAdapter(adapter);
        // Sample Rate - END

        // Toggle Block data - START
        switchBlockData = findViewById(R.id.switch_block_data);
        // Toggle Block data - END

        // Toggle Block data - START
        findViewById(R.id.btn_debug).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                LiveEffectEngine.debug();
            }
        });
        // Toggle Block data - END

        statusText = findViewById(R.id.status_view_text);
        toggleEffectButton = findViewById(R.id.button_toggle_effect);
        toggleEffectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                toggleEffect();
            }
        });
        toggleEffectButton.setText(getString(R.string.start_effect));

        recordingDeviceSpinner = findViewById(R.id.recording_devices_spinner);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            recordingDeviceSpinner.setDirectionType(AudioManager.GET_DEVICES_INPUTS);
            recordingDeviceSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                    LiveEffectEngine.setRecordingDeviceId(getRecordingDeviceId());
                }

                @Override
                public void onNothingSelected(AdapterView<?> adapterView) {
                    // Do nothing
                }
            });
        }

        playbackDeviceSpinner = findViewById(R.id.playback_devices_spinner);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            playbackDeviceSpinner.setDirectionType(AudioManager.GET_DEVICES_OUTPUTS);
            playbackDeviceSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                    LiveEffectEngine.setPlaybackDeviceId(getPlaybackDeviceId());
                }

                @Override
                public void onNothingSelected(AdapterView<?> adapterView) {
                    // Do nothing
                }
            });
        }

        ((RadioGroup)findViewById(R.id.apiSelectionGroup)).check(R.id.aaudioButton);
        findViewById(R.id.aaudioButton).setOnClickListener(new RadioButton.OnClickListener(){
            @Override
            public void onClick(View v) {
                if (((RadioButton)v).isChecked()) {
                    apiSelection = OBOE_API_AAUDIO;
                }
            }
        });
        findViewById(R.id.slesButton).setOnClickListener(new RadioButton.OnClickListener(){
            @Override
            public void onClick(View v) {
                if (((RadioButton)v).isChecked()) {
                    apiSelection = OBOE_API_OPENSL_ES;
                }
            }
        });

        LiveEffectEngine.create();
        aaudioSupported = LiveEffectEngine.isAAudioSupported();
        EnableAudioApiUI(true);
        LiveEffectEngine.setAPI(apiSelection);
    }

    private void EnableAudioApiUI(boolean enable) {
        if(apiSelection == OBOE_API_AAUDIO && !aaudioSupported)
        {
            apiSelection = OBOE_API_OPENSL_ES;
        }
        findViewById(R.id.slesButton).setEnabled(enable);
        if(!aaudioSupported) {
          findViewById(R.id.aaudioButton).setEnabled(false);
        } else {
            findViewById(R.id.aaudioButton).setEnabled(enable);
        }

        ((RadioGroup)findViewById(R.id.apiSelectionGroup))
          .check(apiSelection == OBOE_API_AAUDIO ? R.id.aaudioButton : R.id.slesButton);
    }
    @Override
    protected void onStart() {
        super.onStart();
        setVolumeControlStream(AudioManager.STREAM_MUSIC);
    }

    @Override
    protected void onDestroy() {
        LiveEffectEngine.delete();
        super.onDestroy();
    }

    public void toggleEffect() {
        if (isPlaying) {
            stopEffect();
            EnableAudioApiUI(true);
        } else {
            EnableAudioApiUI(false);
            LiveEffectEngine.setAPI(apiSelection);
            setSamplingRate();
            setBlockDataSate();
            startEffect();
            refreshSamplingRate();
        }
    }

    private void setBlockDataSate() {
        boolean isBlockDataOn = switchBlockData.isChecked();
        LiveEffectEngine.setBlockDataOn(isBlockDataOn);
    }

    private void setSamplingRate() {
            int samplingRate = (int) sampleRatesSpinner.getSelectedItem();
            LiveEffectEngine.setSamplingRate(samplingRate);
    }

    private void refreshSamplingRate() {
        int samplingRate = LiveEffectEngine.getSamplingRate();
        List<Integer> sampleRates = getSupportedSampleRates();
        for (int i = 0; i < sampleRates.size(); i++) {
            if(sampleRates.get(i) == samplingRate){
                sampleRatesSpinner.setSelection(i);
                return;
            }
        }
    }

    private void startEffect() {
        Log.d(TAG, "Attempting to start");

        if (!isRecordPermissionGranted()){
            requestRecordPermission();
            return;
        }

        setConfigurationEnabled(false);
        LiveEffectEngine.setEffectOn(true);
        statusText.setText(R.string.status_playing);
        toggleEffectButton.setText(R.string.stop_effect);
        isPlaying = true;
    }

    private void stopEffect() {
        Log.d(TAG, "Playing, attempting to stop");
        LiveEffectEngine.setEffectOn(false);
        resetStatusView();
        toggleEffectButton.setText(R.string.start_effect);
        isPlaying = false;
        setConfigurationEnabled(true);
    }

    private void setConfigurationEnabled(boolean isEnabled){
        recordingDeviceSpinner.setEnabled(isEnabled);
        playbackDeviceSpinner.setEnabled(isEnabled);
        sampleRatesSpinner.setEnabled(isEnabled);
        switchBlockData.setEnabled(isEnabled);
    }

    private int getRecordingDeviceId(){
        return ((AudioDeviceListEntry)recordingDeviceSpinner.getSelectedItem()).getId();
    }

    private int getPlaybackDeviceId(){
        return ((AudioDeviceListEntry)playbackDeviceSpinner.getSelectedItem()).getId();
    }

    private boolean isRecordPermissionGranted() {
        return (ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) ==
                PackageManager.PERMISSION_GRANTED);
    }

    private void requestRecordPermission(){
        ActivityCompat.requestPermissions(
                this,
                new String[]{Manifest.permission.RECORD_AUDIO},
                AUDIO_EFFECT_REQUEST);
    }
    private void resetStatusView() {
        statusText.setText(R.string.status_warning);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {

        if (AUDIO_EFFECT_REQUEST != requestCode) {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
            return;
        }

        if (grantResults.length != 1 ||
                grantResults[0] != PackageManager.PERMISSION_GRANTED) {

            // User denied the permission, without this we cannot record audio
            // Show a toast and update the status accordingly
            statusText.setText(R.string.status_record_audio_denied);
            Toast.makeText(getApplicationContext(),
                    getString(R.string.need_record_audio_permission),
                    Toast.LENGTH_SHORT)
                    .show();
        } else {
            // Permission was granted, start live effect
            toggleEffect();
        }
    }

    private List<Integer> getSupportedSampleRates() {

        final int possibleValidSampleRates[] = new int[] {8000, 11025, 16000, 22050,
                32000, 37800, 44056, 44100, 47250, 48000, 50000, 50400, 88200,
                96000, 176400, 192000, 352800, 2822400, 5644800 };
        List<Integer> validSampleRates = new ArrayList<Integer>();
        for (int i = 0; i < possibleValidSampleRates.length; i++) {
            int result = AudioRecord.getMinBufferSize(possibleValidSampleRates[i],
                    AudioFormat.CHANNEL_OUT_STEREO,
                    AudioFormat.ENCODING_PCM_FLOAT);
            if (result != AudioRecord.ERROR
                    && result != AudioRecord.ERROR_BAD_VALUE && result > 0) {
                // got here if valid sample rate
                validSampleRates.add(possibleValidSampleRates[i]);
            }
        }
        // return the list of supported sample rates could be empty
        return validSampleRates;
    }
}
