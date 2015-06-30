
g++ -c simulated_device.cpp &&
g++ -c main.cpp &&
g++ -c block_device_manager.cpp &&
g++ -c mbr.cpp &&
g++ -o test main.o simulated_device.o block_device_manager.o mbr.o