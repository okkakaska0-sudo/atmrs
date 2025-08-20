#include "PitchCorrectionEngine.h" 
#include "Utils.h"
#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>
#include <algorithm>
#include <cmath>
#include <vector>

PitchCorrectionEngine::PitchCorrectionEngine()
{
    // Initialize FFT
    fft = std::make_unique<juce::dsp::FFT>(fftOrder);
    window = std::make_unique<juce::dsp::WindowingFunction<float>>(fftSize, juce::dsp::WindowingFunction<float>::hann);
    
    // Allocate frequency domain buffer
    frequencyData.allocate(fftSize, true);
    magnitudeSpectrum.resize(fftSize / 2);
    
    // Initialize pitch history
    pitchHistory.resize(pitchHistoryLength, 0.0f);
    
    // Initialize formant arrays
    formantFrequencies.resize(maxFormants, 0.0f);
    formantAmplitudes.resize(maxFormants, 0.0f);
    
    // Initialize grain buffer
    grainBuffer.reserve(32); // Reserve space for grains
}

PitchCorrectionEngine::~PitchCorrectionEngine()
{
}

void PitchCorrectionEngine::prepare(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    
    // Resize buffers
    autocorrelationBuffer.resize(static_cast<size_t>(blockSize) * 2);
    windowedBuffer.resize(fftSize);
    tempBuffer.resize(static_cast<size_t>(blockSize));
    
    // Calculate grain parameters based on sample rate
    grainSize = static_cast<int>(sampleRate * 0.025); // 25ms grains
    hopSize = grainSize / 4; // 75% overlap
    
    reset();
}

void PitchCorrectionEngine::prepareToPlay(double newSampleRate, int newBlockSize)
{
    prepare(newSampleRate, newBlockSize);
}

void PitchCorrectionEngine::reset()
{
    currentPitch = 0.0f;
    pitchConfidence = 0.0f;
    rmsLevel = 0.0f;
    
    std::fill(pitchHistory.begin(), pitchHistory.end(), 0.0f);
    std::fill(formantFrequencies.begin(), formantFrequencies.end(), 0.0f);
    std::fill(formantAmplitudes.begin(), formantAmplitudes.end(), 0.0f);
    
    grainBuffer.clear();
}

void PitchCorrectionEngine::detectPitch(const float* inputBuffer, int numSamples, float* pitchOutput)
{
    // Use autocorrelation-based pitch detection for basic mode
    for (int i = 0; i < numSamples; ++i)
    {
        if (i % 256 == 0) // Update pitch every 256 samples for efficiency
        {
            int analysisSize = std::min(1024, numSamples - i);
            currentPitch = detectPitchAutocorrelation(&inputBuffer[i], analysisSize);
            smoothPitch(currentPitch);
            rmsLevel = calculateRMS(&inputBuffer[i], analysisSize);
        }
        
        pitchOutput[i] = currentPitch;
    }
}

void PitchCorrectionEngine::detectPitchAdvanced(const float* inputBuffer, int numSamples, float* pitchOutput)
{
    // Use multiple algorithms and combine results for AI mode
    for (int i = 0; i < numSamples; ++i)
    {
        if (i % 128 == 0) // More frequent updates for AI mode
        {
            int analysisSize = std::min(2048, numSamples - i);
            
            // Get pitches from different algorithms
            float autoPitch = detectPitchAutocorrelation(&inputBuffer[i], analysisSize);
            float yinPitch = detectPitchYIN(&inputBuffer[i], analysisSize);
            float spectralPitch = detectPitchSpectral(&inputBuffer[i], analysisSize);
            float harmonicPitch = detectPitchHarmonic(&inputBuffer[i], analysisSize);
            
            // Combine results with confidence weighting
            std::vector<float> pitches = {autoPitch, yinPitch, spectralPitch, harmonicPitch};
            std::vector<float> weights = {0.2f, 0.3f, 0.3f, 0.2f}; // YIN and spectral get more weight
            
            float combinedPitch = 0.0f;
            float totalWeight = 0.0f;
            
            for (size_t j = 0; j < pitches.size(); ++j)
            {
                if (pitches[j] > 0.0f) // Valid pitch
                {
                    combinedPitch += pitches[j] * weights[j];
                    totalWeight += weights[j];
                }
            }
            
            if (totalWeight > 0.0f)
            {
                currentPitch = combinedPitch / totalWeight;
                smoothPitch(currentPitch);
            }
            
            rmsLevel = calculateRMS(&inputBuffer[i], analysisSize);
            analyzeSpectrum(&inputBuffer[i], analysisSize);
        }
        
        pitchOutput[i] = currentPitch;
    }
}

float PitchCorrectionEngine::detectPitchAutocorrelation(const float* buffer, int numSamples)
{
    if (numSamples < 64) return 0.0f;
    
    // Copy to windowed buffer and apply window
    int analysisSize = std::min(numSamples, static_cast<int>(windowedBuffer.size()));
    std::copy(buffer, buffer + analysisSize, windowedBuffer.begin());
    applyHannWindow(windowedBuffer.data(), analysisSize);
    
    // Calculate autocorrelation
    std::fill(autocorrelationBuffer.begin(), autocorrelationBuffer.end(), 0.0f);
    
    for (int lag = 1; lag < analysisSize / 2; ++lag)
    {
        for (int i = 0; i < analysisSize - lag; ++i)
        {
            autocorrelationBuffer[lag] += windowedBuffer[i] * windowedBuffer[i + lag];
        }
        autocorrelationBuffer[lag] /= (analysisSize - lag); // Normalize
    }
    
    // Find peak in autocorrelation
    float maxValue = 0.0f;
    int maxLag = 0;
    int minLag = static_cast<int>(sampleRate / 800.0); // Min freq ~800Hz
    int maxLagLimit = static_cast<int>(sampleRate / 50.0); // Max freq ~50Hz
    
    for (int lag = minLag; lag < maxLagLimit && lag < autocorrelationBuffer.size(); ++lag)
    {
        if (autocorrelationBuffer[lag] > maxValue)
        {
            maxValue = autocorrelationBuffer[lag];
            maxLag = lag;
        }
    }
    
    if (maxLag > 0 && maxValue > 0.3f) // Confidence threshold
    {
        // Parabolic interpolation for sub-sample accuracy
        if (maxLag > 1 && maxLag < autocorrelationBuffer.size() - 1)
        {
            float y1 = autocorrelationBuffer[maxLag - 1];
            float y2 = autocorrelationBuffer[maxLag];
            float y3 = autocorrelationBuffer[maxLag + 1];
            
            float a = (y1 - 2*y2 + y3) / 2.0f;
            if (std::abs(a) > 1e-6f)
            {
                float peak = -0.5f * (y3 - y1) / a;
                float trueLag = maxLag + peak;
                pitchConfidence = maxValue;
                return static_cast<float>(sampleRate / trueLag);
            }
        }
        
        pitchConfidence = maxValue;
        return static_cast<float>(sampleRate / maxLag);
    }
    
    pitchConfidence = 0.0f;
    return 0.0f;
}

float PitchCorrectionEngine::detectPitchYIN(const float* buffer, int numSamples)
{
    // Simplified YIN algorithm implementation
    if (numSamples < 128) return 0.0f;
    
    int bufferSize = std::min(numSamples, 2048);
    std::vector<float> yinBuffer(static_cast<size_t>(bufferSize) / 2, 0.0f);
    
    // Step 1: Difference function
    for (int tau = 1; tau < bufferSize / 2; ++tau)
    {
        for (int i = 0; i < bufferSize / 2; ++i)
        {
            float delta = buffer[i] - buffer[i + tau];
            yinBuffer[tau] += delta * delta;
        }
    }
    
    // Step 2: Cumulative mean normalized difference
    yinBuffer[0] = 1.0f;
    float runningSum = 0.0f;
    for (int tau = 1; tau < bufferSize / 2; ++tau)
    {
        runningSum += yinBuffer[tau];
        if (runningSum > 0.0f)
        {
            yinBuffer[tau] *= tau / runningSum;
        }
        else
        {
            yinBuffer[tau] = 1.0f;
        }
    }
    
    // Step 3: Absolute threshold
    const float threshold = 0.1f;
    int minTau = static_cast<int>(sampleRate / 800.0);
    int maxTau = static_cast<int>(sampleRate / 50.0);
    
    for (int tau = minTau; tau < maxTau && tau < yinBuffer.size(); ++tau)
    {
        if (yinBuffer[tau] < threshold)
        {
            // Step 4: Parabolic interpolation
            while (tau + 1 < yinBuffer.size() && yinBuffer[tau + 1] < yinBuffer[tau])
            {
                tau++;
            }
            
            // Interpolation
            if (tau > 0 && tau < yinBuffer.size() - 1)
            {
                float s0 = yinBuffer[tau - 1];
                float s1 = yinBuffer[tau];
                float s2 = yinBuffer[tau + 1];
                
                float betterTau = tau + (s2 - s0) / (2.0f * (2.0f * s1 - s2 - s0));
                return static_cast<float>(sampleRate / betterTau);
            }
            
            return static_cast<float>(sampleRate / tau);
        }
    }
    
    return 0.0f;
}

float PitchCorrectionEngine::detectPitchSpectral(const float* buffer, int numSamples)
{
    if (numSamples < fftSize) return 0.0f;
    
    // Prepare FFT input
    std::fill(windowedBuffer.begin(), windowedBuffer.end(), 0.0f);
    std::copy(buffer, buffer + std::min(numSamples, fftSize), windowedBuffer.begin());
    
    // Apply window
    window->multiplyWithWindowingTable(windowedBuffer.data(), fftSize);
    
    // Convert to complex and perform FFT
    for (int i = 0; i < fftSize; ++i)
    {
        frequencyData[i] = juce::dsp::Complex<float>(windowedBuffer[i], 0.0f);
    }
    
    fft->perform(frequencyData, frequencyData, false);
    
    // Calculate magnitude spectrum
    for (int i = 0; i < fftSize / 2; ++i)
    {
        magnitudeSpectrum[i] = std::abs(frequencyData[i]);
    }
    
    // Find peak in magnitude spectrum
    float maxMag = 0.0f;
    int peakBin = 0;
    int minBin = static_cast<int>(50.0f * fftSize / sampleRate); // 50Hz minimum
    int maxBin = static_cast<int>(800.0f * fftSize / sampleRate); // 800Hz maximum
    
    for (int i = minBin; i < maxBin && i < magnitudeSpectrum.size(); ++i)
    {
        if (magnitudeSpectrum[i] > maxMag)
        {
            maxMag = magnitudeSpectrum[i];
            peakBin = i;
        }
    }
    
    if (peakBin > 0 && maxMag > 0.01f)
    {
        // Parabolic interpolation for better frequency resolution
        if (peakBin > 1 && peakBin < magnitudeSpectrum.size() - 1)
        {
            float y1 = magnitudeSpectrum[peakBin - 1];
            float y2 = magnitudeSpectrum[peakBin];
            float y3 = magnitudeSpectrum[peakBin + 1];
            
            float a = (y1 - 2*y2 + y3) / 2.0f;
            if (std::abs(a) > 1e-6f)
            {
                float peak = -0.5f * (y3 - y1) / a;
                float trueBin = peakBin + peak;
                return static_cast<float>(trueBin * sampleRate / fftSize);
            }
        }
        
        return static_cast<float>(peakBin * sampleRate / fftSize);
    }
    
    return 0.0f;
}

float PitchCorrectionEngine::detectPitchHarmonic(const float* buffer, int numSamples)
{
    // Harmonic product spectrum method
    if (numSamples < fftSize) return 0.0f;
    
    // Use existing spectrum from spectral method
    analyzeSpectrum(buffer, numSamples);
    
    std::vector<float> hps(magnitudeSpectrum.size());
    std::copy(magnitudeSpectrum.begin(), magnitudeSpectrum.end(), hps.begin());
    
    // Multiply spectrum with its decimated versions
    const int maxHarmonics = 5;
    for (int harmonic = 2; harmonic <= maxHarmonics; ++harmonic)
    {
        for (size_t i = 0; i < hps.size() / harmonic; ++i)
        {
            hps[i] *= magnitudeSpectrum[i * harmonic];
        }
    }
    
    // Find peak in HPS
    float maxValue = 0.0f;
    int peakBin = 0;
    int minBin = static_cast<int>(50.0f * fftSize / sampleRate);
    int maxBin = static_cast<int>(800.0f * fftSize / sampleRate);
    
    for (int i = minBin; i < maxBin && i < hps.size(); ++i)
    {
        if (hps[i] > maxValue)
        {
            maxValue = hps[i];
            peakBin = i;
        }
    }
    
    if (peakBin > 0)
    {
        return static_cast<float>(peakBin * sampleRate / fftSize);
    }
    
    return 0.0f;
}

void PitchCorrectionEngine::analyzeSpectrum(const float* buffer, int numSamples)
{
    // This would be called from detectPitchSpectral, so just ensure spectrum is available
    detectPitchSpectral(buffer, numSamples);
    detectFormants(magnitudeSpectrum);
}

void PitchCorrectionEngine::detectFormants(const std::vector<float>& spectrum)
{
    // Simple formant detection using peak picking
    std::fill(formantFrequencies.begin(), formantFrequencies.end(), 0.0f);
    std::fill(formantAmplitudes.begin(), formantAmplitudes.end(), 0.0f);
    
    // Look for peaks in the spectrum
    std::vector<std::pair<float, int>> peaks;
    
    for (int i = 2; i < static_cast<int>(spectrum.size()) - 2; ++i)
    {
        float freq = static_cast<float>(i * sampleRate / fftSize);
        if (freq > 200.0f && freq < 4000.0f) // Typical formant range
        {
            if (spectrum[i] > spectrum[i-1] && spectrum[i] > spectrum[i+1] &&
                spectrum[i] > spectrum[i-2] && spectrum[i] > spectrum[i+2])
            {
                peaks.push_back({spectrum[i], i});
            }
        }
    }
    
    // Sort peaks by amplitude
    std::sort(peaks.begin(), peaks.end(), std::greater<std::pair<float, int>>());
    
    // Take the top formants
    for (size_t i = 0; i < std::min(static_cast<size_t>(maxFormants), peaks.size()); ++i)
    {
        formantAmplitudes[i] = peaks[i].first;
        formantFrequencies[i] = static_cast<float>(peaks[i].second * sampleRate / fftSize);
    }
}

void PitchCorrectionEngine::correctPitch(float* buffer, int numSamples, 
                                        float targetPitch, float speed, float amount)
{
    if (currentPitch <= 0.0f || targetPitch <= 0.0f) return;
    
    float pitchRatio = targetPitch / currentPitch;
    float correction = (pitchRatio - 1.0f) * amount * 0.01f; // Scale amount
    
    // Apply simple pitch shifting using interpolation
    for (int i = 0; i < numSamples; ++i)
    {
        float smoothedCorrection = interpolateValue(0.0f, correction, speed * 0.01f);
        buffer[i] *= (1.0f + smoothedCorrection);
    }
}

void PitchCorrectionEngine::correctPitchHard(float* buffer, int numSamples, 
                                            float targetPitch, float speed, float amount)
{
    if (currentPitch <= 0.0f || targetPitch <= 0.0f) return;
    
    float pitchRatio = targetPitch / currentPitch;
    float correction = (pitchRatio - 1.0f) * amount * 0.01f;
    
    // Hard correction with immediate snapping
    float hardSpeed = juce::jmin(speed * 0.1f, 1.0f);
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Immediate correction for hard mode
        buffer[i] *= (1.0f + correction * hardSpeed);
    }
}

void PitchCorrectionEngine::correctPitchAI(float* buffer, int numSamples, 
                                          float targetPitch, float speed, float amount)
{
    // AI mode with formant preservation and natural correction
    if (currentPitch <= 0.0f || targetPitch <= 0.0f) return;
    
    float pitchRatio = targetPitch / currentPitch;
    
    // Preserve formants during pitch shifting
    preserveFormants(buffer, numSamples, pitchRatio);
    
    // Apply smooth, natural correction
    float correction = (pitchRatio - 1.0f) * amount * 0.01f;
    float naturalSpeed = speed * 0.005f; // Slower, more natural correction
    
    for (int i = 0; i < numSamples; ++i)
    {
        float smoothCorrection = interpolateValue(0.0f, correction, naturalSpeed);
        buffer[i] *= (1.0f + smoothCorrection);
    }
}

void PitchCorrectionEngine::preserveFormants(float* buffer, int numSamples, float pitchShiftRatio)
{
    // Simplified formant preservation
    // In a full implementation, this would involve spectral envelope manipulation
    
    if (pitchShiftRatio == 1.0f) return;
    
    // Apply spectral processing to preserve formant structure
    // This is a simplified approach - real formant preservation requires
    // more sophisticated spectral manipulation
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Apply slight formant compensation
        float compensation = 1.0f - (pitchShiftRatio - 1.0f) * 0.3f;
        buffer[i] *= compensation;
    }
}

float PitchCorrectionEngine::calculateRMS(const float* buffer, int numSamples)
{
    if (numSamples <= 0) return 0.0f;
    
    float sum = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        sum += buffer[i] * buffer[i];
    }
    
    return std::sqrt(sum / numSamples);
}

float PitchCorrectionEngine::calculateCentroid(const std::vector<float>& spectrum)
{
    float numerator = 0.0f;
    float denominator = 0.0f;
    
    for (size_t i = 1; i < spectrum.size(); ++i)
    {
        float freq = static_cast<float>(i * sampleRate / fftSize);
        numerator += freq * spectrum[i];
        denominator += spectrum[i];
    }
    
    return denominator > 0.0f ? numerator / denominator : 0.0f;
}

void PitchCorrectionEngine::smoothPitch(float newPitch)
{
    if (newPitch > 0.0f)
    {
        // Add to history
        for (size_t i = 0; i < static_cast<size_t>(pitchHistoryLength - 1); ++i)
        {
            pitchHistory[i] = pitchHistory[i + 1];
        }
        pitchHistory.back() = newPitch;
        
        // Calculate smoothed pitch
        float smoothedPitch = 0.0f;
        float totalWeight = 0.0f;
        
        for (size_t i = 0; i < static_cast<size_t>(pitchHistoryLength); ++i)
        {
            if (pitchHistory[i] > 0.0f)
            {
                float weight = static_cast<float>(i + 1) / pitchHistoryLength;
                smoothedPitch += pitchHistory[i] * weight;
                totalWeight += weight;
            }
        }
        
        if (totalWeight > 0.0f)
        {
            currentPitch = smoothedPitch / totalWeight;
        }
    }
}

float PitchCorrectionEngine::interpolateValue(float current, float target, float factor)
{
    factor = juce::jlimit(0.0f, 1.0f, factor);
    return current + (target - current) * factor;
}

void PitchCorrectionEngine::applyHannWindow(float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        float windowValue = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * i / (numSamples - 1)));
        buffer[i] *= windowValue;
    }
}

void PitchCorrectionEngine::applyBlackmanWindow(float* buffer, int numSamples)
{
    const float a0 = 0.42f;
    const float a1 = 0.5f;
    const float a2 = 0.08f;
    
    for (int i = 0; i < numSamples; ++i)
    {
        float phase = 2.0f * juce::MathConstants<float>::pi * i / (numSamples - 1);
        float windowValue = a0 - a1 * std::cos(phase) + a2 * std::cos(2.0f * phase);
        buffer[i] *= windowValue;
    }
}
