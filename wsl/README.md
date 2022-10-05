# WSL Driver Related
Here I present how to configure various drivers related to WSL.

Window Subsystem for Linux (WSL) enables running Linux binaries in Windows without creating a virtual machine. It implements a compatibility layer that translates Linux system calls to Windows system calls. It allows for Linux development in a Windows environment while preventing the overhead of virtual machines.

Since not all system calls have been implemented, not all Linux programs will run flawlessly under WSL. Also, not all hardware resources are available in WSL. However, this is gradually improving and some hardware access is available in WSL:

## CUDA Development on WSL
WSL allows for CUDA usage in the Linux environment for machine learning, intense mathematics or related uses. In order to activate CUDA inside WSL, Windows 11 or Windows 10 with version 21H2 support is required. [WSL2](#5-wsl2) is required along with the 5.10 kernel which is available through `wsl --update` command.
 - [CUDA drivers and toolkit](https://developer.nvidia.com/cuda-toolkit) are required to be installed on Windows. 11.7 is recommended.
 - [Section 3](https://docs.nvidia.com/cuda/wsl-user-guide/index.html) will instruct how to install the CUDA requirements inside WSL. **Do not install a driver inside WSL.** The Windows driver will be used through the 5.10 kernel.
  
You can test the install after following the above steps through PyTorch (`torch.cuda.is_available()`) or similar tools. Another option is using the [NVIDIA CUDA Samples](https://github.com/NVIDIA/cuda-samples).

This process requires a CUDA enabled GPU device.

### CUDA from Docker
Microsoft provides a tutorial for setting up CUDA use in Linux using [Docker containers](https://learn.microsoft.com/en-us/windows/wsl/tutorials/gpu-compute)

## GUI / Graphics
In Windows 11, you can install gui programs and they will run out of the box. 
    - [GUI Apps](https://learn.microsoft.com/en-us/windows/wsl/tutorials/gui-apps)
For Windows 10, Make sure you add `export DISPLAY=:0.0` to your `.bashrc` file. You also need an xserver on Windows.
    -  [VcXsrv](https://github.com/ArcticaProject/vcxsrv)
    -  [Xming](https://sourceforge.net/projects/xming/)
    -  [Cygwin/X](https://x.cygwin.com/)

## Sound
Sound can be achieved using PulseAudio with the instructions available [here](https://research.wmz.ninja/articles/2017/11/setting-up-wsl-with-graphics-and-audio.html).
