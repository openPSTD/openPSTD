//
// Created by michiel on 24-1-2016.
//

#include "KernelInterface.h"

namespace OpenPSTD {
    namespace Kernel {

        const char *PSTDKernelNotConfiguredException::what() const noexcept {
            return "Kernel is not yet configured";
        }


        float PSTDSettings::GetGridSpacing() {
            return this->gridSpacing;
        }

        void PSTDSettings::SetGridSpacing(float value) {
            this->gridSpacing = value;
            //TODO edit max frequency here as well
        }

        float PSTDSettings::GetPatchError() {
            return this->patcherror;
        }

        void PSTDSettings::SetPatchError(float value) {
            this->patcherror = value;
        }

        float PSTDSettings::GetRenderTime() {
            return this->calctime;
        }

        void PSTDSettings::SetRenderTime(float value) {
            this->calctime = value;
        }

        int PSTDSettings::GetPMLCells() {
            return this->PMLCells;
        }

        void PSTDSettings::SetPMLCells(int value) {
            this->PMLCells = value;
        }

        float PSTDSettings::GetAttenuationOfPMLCells() {
            return this->ampMax;
        }

        void PSTDSettings::SetAttenuationOfPMLCells(float value) {
            this->ampMax = value;
        }

        float PSTDSettings::GetDensityOfAir() {
            return this->rho;
        }

        void PSTDSettings::SetDensityOfAir(float value) {
            this->rho = value;
        }

/**
 * TODO: add dynamic behavior to setters of freqMax and grid spacing
 */
        float PSTDSettings::GetMaxFrequency() {
            return this->freqMax;
        }

        void PSTDSettings::SetMaxFrequency(float value) {
            this->freqMax = value;
            //TODO edit grid spacing as well
        }

        float PSTDSettings::GetSoundSpeed() {
            return this->c1;
        }

        void PSTDSettings::SetSoundSpeed(float value) {
            this->c1 = value;
        }

        float PSTDSettings::GetFactRK() {
            return this->tfactRK;
        }

        void PSTDSettings::SetFactRK(float value) {
            this->tfactRK = value;
        }

        int PSTDSettings::GetSaveNth() {
            return this->SaveNth;
        }

        void PSTDSettings::SetSaveNth(int value) {
            this->SaveNth = value;
        }

        float PSTDSettings::GetBandWidth() {
            return this->band_width;
        }

        void PSTDSettings::SetBandWidth(float value) {
            this->band_width = value;
        }

        float PSTDSettings::GetWaveLength() {
            return this->wave_length;
        }

        void PSTDSettings::SetWaveLength(float value) {
            this->wave_length = value;
        }

        bool PSTDSettings::GetSpectralInterpolation() {
            return this->spectral_interpolation;
        }

        void PSTDSettings::SetSpectralInterpolation(bool value) {
            this->spectral_interpolation = value;
        }

        bool PSTDSettings::GetGPUAccel() {
            return this->gpu;
        }

        void PSTDSettings::SetGPUAccel(bool value) {
            this->gpu = value;
        }

        bool PSTDSettings::GetMultiThread() {
            return this->multithread;
        }

        void PSTDSettings::SetMultiThread(bool value) {
            this->multithread = value;
        }

        void PSTDSettings::SetWindow(Eigen::ArrayXf A) {
            this->window = A;
        }

        Eigen::ArrayXf PSTDSettings::GetWindow() {
            return this->window;
        }

        float PSTDSettings::GetTimeStep() {
            return this->tfactRK * this->gridSpacing / this->c1;
        }

        int PSTDSettings::GetWindowSize() {
            //directly translated from original Python implementation
            int tmp = (int) std::round((this->patcherror * 0.7 - 17) / 2.);
            tmp = (int) std::round(tmp * 2);
            return tmp;
        }

        std::vector<float> PSTDSettings::GetRKCoefficients() {
            //return this->rk_coefficients;
            return std::vector<float>();
        }

        void PSTDSettings::SetRKCoefficients(std::vector<float> coef) {
            this->rk_coefficients = coef; //Todo: Set somewhere.
        }

        float DomainConf::GetAbsorption(PSTD_DOMAIN_SIDE side) {
            switch (side) {
                case PSTD_DOMAIN_SIDE_TOP:
                    return T.Absorption;
                case PSTD_DOMAIN_SIDE_BOTTOM:
                    return B.Absorption;
                case PSTD_DOMAIN_SIDE_LEFT:
                    return L.Absorption;
                case PSTD_DOMAIN_SIDE_RIGHT:
                    return R.Absorption;
                default:
                    return 0;
            }
        }

        bool DomainConf::GetLR(PSTD_DOMAIN_SIDE side) {
            switch (side) {
                case PSTD_DOMAIN_SIDE_TOP:
                    return T.LR;
                case PSTD_DOMAIN_SIDE_BOTTOM:
                    return B.LR;
                case PSTD_DOMAIN_SIDE_LEFT:
                    return L.LR;
                case PSTD_DOMAIN_SIDE_RIGHT:
                    return R.LR;
                default:
                    return false;
            }
        }

        void DomainConf::SetLR(PSTD_DOMAIN_SIDE sides, bool LR) {
            if (sides & PSTD_DOMAIN_SIDE_TOP != 0)
                T.LR = LR;
            if (sides & PSTD_DOMAIN_SIDE_BOTTOM != 0)
                B.LR = LR;
            if (sides & PSTD_DOMAIN_SIDE_LEFT != 0)
                L.LR = LR;
            if (sides & PSTD_DOMAIN_SIDE_RIGHT != 0)
                R.LR = LR;
        }

        void DomainConf::SetAbsorption(PSTD_DOMAIN_SIDE sides, float absorption) {
            if (sides & PSTD_DOMAIN_SIDE_TOP != 0)
                T.Absorption = absorption;
            if (sides & PSTD_DOMAIN_SIDE_BOTTOM != 0)
                B.Absorption = absorption;
            if (sides & PSTD_DOMAIN_SIDE_LEFT != 0)
                L.Absorption = absorption;
            if (sides & PSTD_DOMAIN_SIDE_RIGHT != 0)
                R.Absorption = absorption;
        }

        std::shared_ptr<PSTDConfiguration> PSTDConfiguration::CreateDefaultConf() {
            std::shared_ptr<PSTDConfiguration> conf = std::make_shared<PSTDConfiguration>();
            conf->Settings.SetRenderTime(1.0f);
            float sound_speed = 340;
            float grid_spacing = 0.2;
            float patch_error = 70;
            conf->Settings.SetSoundSpeed(sound_speed);
            conf->Settings.SetAttenuationOfPMLCells(20000.0f);
            conf->Settings.SetDensityOfAir(1.2f);
            conf->Settings.SetPatchError(70.0f);
            conf->Settings.SetFactRK(0.5f);
            conf->Settings.SetGridSpacing(grid_spacing);
            conf->Settings.SetPMLCells(50);
            conf->Settings.SetSaveNth(1);

            float band_width = (float) (3 * exp(-6) * (sound_speed / grid_spacing) * (sound_speed / grid_spacing));
            conf->Settings.SetBandWidth(band_width);
            conf->Settings.SetRKCoefficients(std::vector<float>(
                    {8.91421261e-4f, 7555704391e-3f, 4.0919732041e-2f, 1.65919771368e-1f, 5e-1, 1.f}));
            //Todo: What is this window_alpha?
            float window_alpha = (patch_error - 40) / 20 + 1;
            int wsize = conf->Settings.GetWindowSize();
            Eigen::ArrayXf window_coefficients = (Eigen::ArrayXf::LinSpaced(2 * wsize + 1, -wsize, wsize) /
                                                  wsize).square().cube().exp(); // Need to go to power 6

            conf->Settings.SetWindow(window_coefficients);
            conf->Settings.SetSpectralInterpolation(true);

            conf->Speakers.push_back(QVector3D(4, 5, 0));
            conf->Receivers.push_back(QVector3D(6, 5, 0));

            DomainConf d;
            d.TopLeft = QVector2D(0, 0);
            d.Size = QVector2D(10, 10);
            d.T.Absorption = 0;
            d.B.Absorption = 0;
            d.L.Absorption = 0;
            d.R.Absorption = 0;

            d.T.LR = false;
            d.B.LR = false;
            d.L.LR = false;
            d.R.LR = false;

            conf->Domains.push_back(d);

            return conf;
        }
    }
}
