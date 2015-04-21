#include <iostream>
#include <thread>
#include <csignal>

void run() 
{
    std::cout << "Sleeping" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(4));
    std::cout << "Awake" << std::endl;
}

int main(int argc, char** argv)
{
    std::signal(SIGINT,SIG_IGN);
    std::signal(SIGINT,SIG_PIPE);

    std::cout << "Starting" << std::endl;
    std::thread thread;
    std::cout << "Joinable " << thread.joinable() << std::endl;
    thread = std::thread(run);
    std::cout << "Joinable " << thread.joinable() << std::endl;
    thread.join();
}
