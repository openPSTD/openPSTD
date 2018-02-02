/**
 * \page kernelstructure
 * # Structure of the kernel
 * This page describes the interaction between the classes that are responsible for the scientific computations in openPSTD.
 *
 * ## Component list
 * A list with all the module in the kernel is present [here](@ref OpenPSTD::Kernel).
 *
 * ## Composition structure
 * The kernel is roughly divided in two subsets: 
 *
 * 1. The core components that represent the simulation environment 
 * ([Domain](@ref OpenPSTD::Kernel::Domain), [Point](@ref OpenPSTD::Kernel::Point), [Scene](@ref OpenPSTD::Kernel::Scene), [Receiver](@ref OpenPSTD::Kernel::Receiver), etc)
 * 2. The interface components that facilitate the communication of data to the application ([PSTDKernel](@ref OpenPSTD::Kernel::PSTDKernel), [KernelInterface](@ref OpenPSTD::Kernel::KernelInterface), [KernelCallback](@ref OpenPSTD::Kernel::KernelCallback), etc)
 *
 * Almost all components of any subset are agnostic to the components of the other. The exception is the PSTDKernel and the scene, which have a aggregation relationship.
 * If tools permit, a UML diagram will be posted in this section.
 *
 * ## Initialization steps
 * When running a simulation, the application (possessing a [configuration](@ref OpenPSTD::Kernel::PSTDConfiguration) object) creates an instance of PSTDKernel.
 * Upon creating this instance, the scene is created, all domains are converted to their kernel representations and the PML domains are constructed.
 * ### Detailed initialization
 * First the domains, speakers and receivers from the PSTDFile are converted to their kernel representation. After that, the domains are connected and integrated into the scene, and the PML domains are created wherever necessary.
 * Then, the attenuation arrays of these PML domains are computed and stored for later use.
 * The initial sound data from the speakers is added to the domains, and the preprocessing phase is complete.
 *
 * ## Running steps
 * After initialization of the PSTDKernel, the simulation is start with a call to [run](@ref OpenPSTD::Kernel::PSTDKernel::run()).
 * This creates an [Solver](@ref OpenPSTD::Kernel::Solver) which takes a pointer to a fully initialized scene as an argument.
 * This class performs the spatial computations of the state variables using the PSTD method and the time integration using a six stage Runge Kutta scheme.
 * Each time step the values of the scene are updated, and if requested they are returned to the calling application using the [callback](@ref OpenPSTD::Kernel::KernelCallback) class.
 * ### Detailed propagation
 * For each time step, 3 individual steps take place:
 * 1. Checking the need for an update.
 * 2. Computing the state variables for the next Runge-Kutta substep
 * 3. Adding these to the total pressure
 *
 */
