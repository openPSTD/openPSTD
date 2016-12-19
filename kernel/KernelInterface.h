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

        /**
         * Enums for the domain boundary representation in the interface
         */
        enum PSTD_DOMAIN_SIDE {
            PSTD_DOMAIN_SIDE_TOP = 1,
            PSTD_DOMAIN_SIDE_BOTTOM = 2,
            PSTD_DOMAIN_SIDE_LEFT = 4,
            PSTD_DOMAIN_SIDE_RIGHT = 8,
            PSTD_DOMAIN_SIDE_ALL = 8 + 4 + 2 + 1,
            PSTD_DOMAIN_SIDE_NONE = 0
        };

        /**
         * A collection of parameters and settings for the simulation
         *
         * PSTDSettings contains various parameters like simulation duration, time step size
         * window shape, etcetera. Most of these can freely be set, some depend on other parameters.
         */
        class PSTDSettings {
        private:
            /// Simulation length. Number of steps depends on this and dt
            float calctime;
            /// Speed of sound in air
            float c1;
            /// Maximum amplitude
            float ampMax;
            /// Maximum frequency
            float freqMax;
            /// Density of medium (default: air)
            float rho;
            /// Patch error
            // Todo: What is this?
            float patcherror;
            /// Time step coefficient used in computing the RK time step size
            float tfactRK;
            /// Size of grid cell in x and y (and z) direction
            float gridSpacing;
            /// Bandwidth
            float band_width;
            /// Wave length. Has to satisfy some minimum
            float wave_length;
            /// Coefficients of the six-stage RK method, according to <paper>
            std::vector<float> rk_coefficients;
            /// Flag indicating whether to interpolate pressure values with spectral interpolation.
            /// If false, interpolation occurs with a nearest neighbour approximation
            bool spectral_interpolation;
            /// Number of cells of the perfectly matched layer boundary
            int PMLCells;
            /// Subsampling size of the simulation results.
            int SaveNth;
            /// Window coefficients for attenuating the sound
            Eigen::ArrayXf window;

        public:

            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & calctime;
                ar & c1;
                ar & ampMax;
                ar & freqMax;
                ar & rho;
                ar & patcherror;
                ar & tfactRK;
                ar & gridSpacing;
                ar & band_width;
                ar & wave_length;
                ar & rk_coefficients;
                ar & spectral_interpolation;
                ar & PMLCells;
                ar & SaveNth;
            }

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

            std::vector<float> GetRKCoefficients();

            void SetRKCoefficients(std::vector<float> coef);
        };

        /**
         * Interface values for boundaries of domains
         */
        class DomainConfEdge {
        public:
            /// Wall absorption coefficient
            float Absorption;
            /// Locally reacting flag
            bool LR;

            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & Absorption;
                ar & LR;
            }
        };

        /**
         * Interface representation of the domain
         */
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

            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & TopLeft;
                ar & Size;
                ar & T;
                ar & L;
                ar & B;
                ar & R;
            }
        };

        /**
         * Representation of the scene configuration.
         *
         * This object contains the settings with the simulation parameters (see Settings) as well
         * as the domains, speaker and receivers with their respective positions.
         */
        class PSTDConfiguration {
        public:
            PSTDSettings Settings;
            //todo replace QVector3D
            std::vector<QVector3D> Speakers;
            std::vector<QVector3D> Receivers;
            std::vector<DomainConf> Domains;

            /**
             * Obtain a default configuration with decent values for a simulation run
             * @return: Pointer to a set configuration file
             *
             */
            static std::shared_ptr<PSTDConfiguration> CreateDefaultConf();
            /**
             * Creates an empty configuration
             */
            static std::shared_ptr<PSTDConfiguration> CreateEmptyConf();

            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & Settings;
                ar & Speakers;
                ar & Receivers;
                ar & Domains;
            }
        };

        /**
         * Callback interface for communication with the CLI or the GUI
         *
         * This callback is passed to the kernel to return simulation information to the requester.
         * Main use case is passing the observed pressure values, the warnings and the errors.
         */
        class KernelCallback {
        public:
            /**
             * This callback will be called with information how far the kernel is progressed.
             * @param status: CALLBACKSTATUS enum, either one of starting/running/finishing/error.
             * @param message: Message to pass to callback handler
             * @param frame: either positive integer corresponding to time step of data or -1 when kernel is not running.
             */
            __attribute__((deprecated)) virtual void Callback(CALLBACKSTATUS status, std::string message, int frame) = 0;

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

            /**
             * An fatel error where the Kernel can not continue. Remark, this should be a non-blocking method. So the
             * kernel will continue after this method.
             */
            virtual void Fatal(std::string message) = 0;
            /**
             * An simple error, the Kernel may not or may continue after this error. Remark, this should be a
             * non-blocking method. So the kernel will continue after this method.
             */
            virtual void Error(std::string message) = 0;
            /**
             * A warning, the long operation will continue after this, but the user should be aware of something that
             * probably is wrong. Remark, this should be a non-blocking method. So the kernel will continue after this
             * method.
             */
            virtual void Warning(std::string message) = 0;
            /**
             * Information that the user should be aware of.Remark, this should be a non-blocking method. So the
             * kernel will continue after this method.
             */
            virtual void Info(std::string message) = 0;
            /**
             * Debug information, this can be used extensivly, and is only shown to the user if the user has chosen it.
             * Remark, this should be a non-blocking method. So the kernel will continue after this method.
             */
            virtual void Debug(std::string message) = 0;
        };

        /**
         * Data not obtained in running openPSTD but necessary for representing the information.
         */
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
         * The kernel API.
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
