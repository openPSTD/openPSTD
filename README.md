openPSTD
========

openPSTD is an open source initiative to provide an intuitive visual interface for the Pseudo-Spectral Time-Domain method to simulate the propagation of sound. Documentation on openPSTD can be found here, please use the Navigation menu on the left.
License

License
=======

openPSTD is licensed under the GPL open source license, because we think it is important for our users to have the freedom to use and adapt openPSTD freely in their academic and commercial projects. Furthermore, we believe in a collaboratively development model and we welcome contributions from other experts. Note that conditions apply if you wish to redistribute a modified version of openPSTD. 

Installation
============

Download the most recent release of openPSTD from [this link](http://openpstd.org/Download%20openPSTD.html) and install the add-on by clicking on the 'Install from File...' button in the User Preferences > Addons window in Blender. Blender 2.70 or newer is required to run the add-on.

Enabling GPU Acceleration (Optional)
====================================

GPU acceleration is available for computers that have a GPU which supports either CUDA or OpenCL. The GPU acceleration in openPSTD v1.1 requires either PyOpenCL or PyCUDA and the PyFFT library. PyFFT is incompatible with Python 3, which is shipped with Blender. In order to make use of GPU acceleration, a Python 2 installation has to be present.

Please go through the following steps to be able to make use of GPU acceleration with openPSTD:

1. Install either a CUDA or an OpenCL driver.
  * Check developer.nvidia.com/cuda-gpus if CUDA is available for your card. If not, OpenCL drivers are generally available from the GPU vendor.
  * Install Python 2 if it is not yet on your system. Install numpy, matplotlib, PyFFT and PyOpenCL/PyCUDA. If your default Python is set to a Python 3 installation, make sure you install the modules in your Python 2 installation.
2. Import the openPSTD plugin as described above if you have not done it yet.
3. Locate *openPSTD.txt* in your Blender config folder. This can be found by entering *bpy.utils.user_resource('CONFIG')* in a Blender Python console. The default locations are:
  * Linux: ~/.config/blender/[version]/config/
  * OSX: ~/Library/Application Support/Blender/2.75/config/
  * Windows: C:\Documents and Settings\{username}\AppData\Roaming\Blender Foundation\Blender\2.75\config\
4. Change the path behind *python_path* = '' to the full path of your Python 2 installation. On Linux and OSX, this is usually */usr/bin/python*. On Windows, this is usually *C:\Python27\python.exe*.
5. Restart Blender. If everything is installed correctly, the **Enable GPU Acceleration** option now works.

__Warning__: openPSTD fails when **Enable GPU Acceleration** is checked but not all modules are installed correctly. If no output is produced, check if all needed modules are available and the Python 2 path is correctly entered in the config file.

Contents of this repository
=============================
This repository contains code that implements the PSTD model for the propagation of sound and an interface for Blender to create and visualize simulation results. Both are written in the Python programming language. The simulation code can use GPU acceleration using either OpenCL or CUDA.

Simulation kernel
-----------------

* **./kernel/pstd.py**

  A stand-alone executable and the main entry point for the simulation code. This file contains code for parsing the scene description, initialization, the main loop and can address both the CPU and GPU variants of the implementation of the propagation model.

* **./kernel/util.py**

  A stand-alone executable that can be used to convert and resample the intermediate receiver output files.

* **./kernel/core/classes.py**

  This module contains the cardinal classes to describe the geometry, semantics and logic of a boundary geometry decomposed into rectangular subdomains.

* **./kernel/core/derived_config.py**

  A module that is mixed into the static configuration and scene description and calculates and completes certain derived settings. For example, it calculates the PML attenuation factor from the width of the PML domains. 

* **./kernel/core/functions.py**

  Several low level procedures that perform the core functionality pertaining to the PSTD model.

* **./kernel/core/kcalc.py**

  Code related to wave number discretization

* **./kernel/core/pstd.cfg**

  Default configuration settings that can be overridden in the scene descriptor file.

* **./kernel/cuda_kernels.cu**

  CUDA kernels containing functions necessary for the GPU acceleration of the spatial derivatives (64 bit versions).

* **./kernel/cuda_kernels32.cu**

  CUDA kernels containing functions necessary for the GPU acceleration of the spatial derivatives (32 bit versions).

* **./kernel/ocl_kernels.cl**

  OpenCL kernels containing functions necessary for the GPU acceleration of the spatial derivatives (64 bit versions).

* **./kernel/ocl_kernels32.cl**

  OpenCL kernels containing functions necessary for the GPU acceleration of the spatial derivatives (32 bit versions).

Blender add-on
--------------

* **./__init__.py**

  Main entry point for the openPSTD Blender add-on. Defines and registers the Blender callback functions and UI elements and contains logic to invoke the simulation kernel in a separate subprocess.

* **./pstd_bmesh.py**

  Convenience functions to facilitate working with Blender bmesh layers, which are used to describe the absorption at edges of the domain and whether they are reacting only locally, i.e only in the direction orthogonal to the edge vector.

* **./pstd_data.py**

  Classes to transfer settings and results to and from the simulation subprocess.

* **./pstd_enums.py**

  Enumerations to categorize enriched behaviour of standard Blender objects.

* **./pstd_i18n.py**

  Internationalized error messages, currently only implemented in English.

* **./pstd_mesh.py**

  Unified interface to both traditional Blender mesh and bmesh to validate the constraints for the current simulation kernel, which currently only operates on a **2d**, **orthogonal** and **quadrangular** meshes. 

* **./pstd_ops.py**

  Defines the entry points for Blender UI buttons, for example, to start the simulation, and timer ticks to visualize the progress of the simulation currently running in the Blender Timeline area.

* **./pstd_props.py**

  Defines additional properties for the configuration of the PSTD scene. Data types of the Blender panel elements are registered here and persisted into the .blend file.

* **./pstd_store.py**

  A singleton to transfer data between the several Python modules if necessary.

* **./pstd_ui.py**

  Defines and renders the panels and menu handlers for the add-on.

* **./pstd_visualisation.py**

  Contains the various elements that enrich the Blender UI for the purpose of visualizing the simulation data and results. This includes 1) an additional visualisation on top of the Blender mesh to print the discretized edges 2) a visualization of the pressure levels emitted from the simulation kernel based on the current frame in world coordinates 3) a color scale that maps the colors used in 2. to sound pressure levels 4) a visualisation of the impulse response in the Blender Timeline area for the currently selected Listener object.

Resources
=========

Website: http://openpstd.org/

Documentation: http://www.openpstd.org/wiki/index.php/OpenPSTD
