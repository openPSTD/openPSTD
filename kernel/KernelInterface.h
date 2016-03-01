//
// Created by michiel on 24-1-2016.
//

#ifndef OPENPSTD_KERNELINTERFACE_H
#define OPENPSTD_KERNELINTERFACE_H

#include <string>
#include "GeneralTypes.h"
#include <QVector2D>
#include <QVector3D>
#include <Eigen/Core>


namespace OpenPSTD {
    namespace Kernel {
/**
 * The status of the kernel when the callback is called.
 */
        enum class CALLBACKSTATUS {
            STARTING,
            RUNNING,
            FINISHED
        };

        enum PSTD_DOMAIN_SIDE {
            PSTD_DOMAIN_SIDE_TOP = 1,
            PSTD_DOMAIN_SIDE_BOTTOM = 2,
            PSTD_DOMAIN_SIDE_LEFT = 4,
            PSTD_DOMAIN_SIDE_RIGHT = 8,
            PSTD_DOMAIN_SIDE_ALL = 8 + 4 + 2 + 1,
            PSTD_DOMAIN_SIDE_NONE = 0
        };

        class PSTDSettings {
        private:
            float calctime;
            float c1;
            float ampMax;
            float freqMax;
            float rho;
            float patcherror;
            float tfactRK;
            float gridSpacing;
            float band_width;
            float wave_length;
            std::vector<float> rk_coefficients;
            bool spectral_interpolation;
            int PMLCells;
            int SaveNth;
            bool gpu;
            bool multithread;
            Eigen::ArrayXf window;
        public:
            float GetGridSpacing();

            void SetGridSpacing(float value);

            float GetPatchError();

            void SetPatchError(float value);

            int GetWindowSize();

            float GetRenderTime();

            void SetRenderTime(float value);

            int GetPMLCells();

            void SetPMLCells(int value);

            float GetAttenuationOfPMLCells();

            void SetAttenuationOfPMLCells(float value);

            float GetDensityOfAir();

            void SetDensityOfAir(float value);

            float GetMaxFrequency();

            void SetMaxFrequency(float value);

            float GetSoundSpeed();

            void SetSoundSpeed(float value);

            float GetFactRK();

            void SetFactRK(float value);

            int GetSaveNth();

            void SetSaveNth(int value);

            float GetBandWidth();

            void SetBandWidth(float value);

            bool GetSpectralInterpolation();

            void SetSpectralInterpolation(bool value);

            float GetWaveLength();

            void SetWaveLength(float value);

            float GetTimeStep();

            bool GetGPUAccel();

            void SetGPUAccel(bool value);

            bool GetMultiThread();

            void SetMultiThread(bool value);

            std::vector<float> GetRKCoefficients();

            void SetRKCoefficients(std::vector<float> coef);
        };

        class DomainConfEdge {
        public:
            float Absorption;
            bool LR;
        };

        class DomainConf {
        public:
            //todo replace QVector2D
            QVector2D TopLeft;
            QVector2D Size;
            DomainConfEdge T, L, B, R;

            float GetAbsorption(PSTD_DOMAIN_SIDE side);

            void SetAbsorption(PSTD_DOMAIN_SIDE sides, float absorption);

            bool GetLR(PSTD_DOMAIN_SIDE side);

            void SetLR(PSTD_DOMAIN_SIDE sides, bool LR);


        };

        class PSTDConfiguration {
        public:
            PSTDSettings Settings;
            //todo replace QVector3D
            std::vector<QVector3D> Speakers;
            std::vector<QVector3D> Receivers;
            std::vector<DomainConf> Domains;

            /**
             * Creates a default configuration
             */
            static std::shared_ptr<PSTDConfiguration> CreateDefaultConf();
        };

        class KernelCallback {
        public:
            /**
             * This callback will be called with information how far the kernel is progressed.
             * @param status: CALLBACKSTATUS enum, either one of starting/running/finishing/error.
             * @param message: Message to pass to callback handler
             * @param frame: either positive integer corresponding to time step of data or -1 when kernel is not running.
             */
            virtual void Callback(CALLBACKSTATUS status, std::string message, int frame) = 0;

            /**
             * Return pressure data of scene to callback handler.
             * @param frame: Positive integer corresponding to time step of data.
             * @param domain: an identifier that identifies the domain
             * @param data: 1D row-major vector of pressure data.
             */
            virtual void WriteFrame(int frame, int domain, PSTD_FRAME_PTR data) = 0;

            /**
             * Return receiver data of scene to callback handler.
             * @param startSample: Positive integer corresponding to time step of the first data point.
             * @param receiver: an identifier that identifies the receiver
             * @param data: a set of data points
             */
            virtual void WriteSample(int startSample, int receiver, std::vector<float> data) = 0;
        };

        class SimulationMetadata {
        public:
            /**
             * The discretization of the domain sizes, in order they were passed to the kernel.
             * A vector in which domain n is represented by a vector at the nth position.
             * In the "inner" vectors, v[0],v[1],v[2] correspond to size x,y,z.
             */
            std::vector<std::vector<int>> DomainMetadata;
            /**
             * Number of frames generated by the kernel
             */
            int Framecount;
            /**
             * The discretization of the domain positions, in order they were passed to the kernel.
             * A vector in which domain n is represented by a vector at the nth position.
             * In the "inner" vectors, v[0],v[1],v[2] correspond to size x,y,z.
             */
            std::vector<std::vector<int>> DomainPositions;
        };

/**
 * The kernel API. Contains one method for initialization and one for running the simulation.
 */
        class KernelInterface {
        public:
            /**
             * Sets the configuration.
             */
            virtual void initialize_kernel(std::shared_ptr<PSTDConfiguration> config) = 0;

            /**
             * Runs the kernel. The callback has a single function that informs the rest of the
             * application of the progress of the kernel.
             * Must first be configured, else a PSTDKernelNotConfiguredException is thrown.
             */
            virtual void run(KernelCallback *callback) = 0;

            /**
             * Query the kernel for metadata about the simulation that is configured.
             */
            virtual SimulationMetadata get_metadata() = 0;
        };

        class PSTDKernelNotConfiguredException : public std::exception {
        public:
            const char *what() const noexcept override;
        };
    }
}


#endif //OPENPSTD_KERNELINTERFACE_H
