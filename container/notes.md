- Linux Containers

1. Provides isolation from the host OS & other containers.
2. No VM overhead since it doesn't virtualize hardware; it runs on the host OS kernel.
3. Containers are built on **cgroups and namespaces**.
4. Namespaces define "What you can see"
5. New keywords to research: 
    - Unix Timesharing System
    - Process IDs
    - File System (Mount points)
    - Users
    - IPC 
    - Networking

-- Essentially I need to create a Linux Namespace (that has it's own filesystem)
Then, let it know how much resources it's allowed to use. Then be able to run processes inside this namespace.

-- Combining types of namespaces (Replacing - with _ )
    - PID Namespace ("CLONE-NEWPID")
    - Mount Namespace ("CLONE-NEWNS")
    - Network Namespace ("CLONE-NEWNET")
    - User Namespace ("CLONE-NEWUSER")

-- Container Setup
		- Namespaces
		- Capabilities
		- Cgroups
        - Seccomp

# Links 

- [Containers](https://www.infoq.com/articles/build-a-container-golang/)
- [Container in C](https://blog.lizzie.io/linux-containers-in-500-loc.html) 
- [RootFS with Busybox](https://www.learningaboutelectronics.com/Articles/How-to-create-a-root-file-system-linux-busybox.php)

