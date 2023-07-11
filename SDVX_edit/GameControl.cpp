#include "gameControl.h"

GameControl::GameControl()
{
	struct shm_remove
	{
		shm_remove() { boost::interprocess::shared_memory_object::remove("MySharedMemory"); }
		~shm_remove() { boost::interprocess::shared_memory_object::remove("MySharedMemory"); }
	} remover;
	//Create a managed shared memory segment
	memSegment = boost::interprocess::managed_shared_memory(boost::interprocess::create_only, "MySharedMemory", 1000);

	controlPtr = static_cast<GameControl*>(memSegment.allocate(sizeof(GameControl)));
	controlPtr->speed = 1.0;
	controlPtr->seekPos = 50;

	//An handle from the base address can identify any byte of the shared
	//memory segment even if it is mapped in different base addresses
	boost::interprocess::managed_shared_memory::handle_t handle = memSegment.get_handle_from_address(controlPtr);

	std::stringstream s;
	s << "start C:/Users/niayo/source/repos/unnamed-sdvx-clone/bin/usc-game_Debug.exe" << " " << handle <<
		" C:/Users/niayo/source/repos/SDVX_edit/SDVX_edit/exh.ksh" << " -autoplay";
	//Launch child process
	//std::system(s.str().c_str());
}
