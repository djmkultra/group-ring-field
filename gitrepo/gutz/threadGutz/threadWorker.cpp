///////////////////////////////////////////////////////////////////////////
//              _____________  ______________________    ^    ----  _
//             /  ________  |  |   ___   ________   /   / \  /    \ |
//            |  |       |  |_ |  |_ |  |       /  /   /   \|       |
//            |  |  ___  |  || |  || |  |      /  /   / --- \   --- |
//            |  | |   \ |  || |  || |  |     /  /   /       \____/ |_____|
//            |  | |_@  ||  || |  || |  |    /  /          
//            |  |___/  ||  ||_|  || |  |   /  /_____________________
//             \_______/  \______/ | |__|  /___________________________
//                        |  |__|  |
//                         \______/
//                 University of New Mexico       
//                           2010
///////////////////////////////////////////////////////////////////////////


#include <threadWorker.h>

namespace gutz
{
   ThreadWorker::~ThreadWorker()
   {
      _die = true;
      std::cerr  << "Thread destructor" << std::endl;
   }
   
   
   // Do it! Run the thread till we die.
   void ThreadWorker::run(void)
   {
      while(!_die || !_commandBuffer.empty())
      {
         if(!_commandBuffer.empty())
         {
            Command* cmd = _commandBuffer.front();
            cmd->exec();
            _commandBuffer.pop_front();
//            msleep(10);
         }
      }
      
      std::cerr << "Child thread outta here!" << std::endl;
   }
   
   void ThreadWorker::addCommand(Command* cmd)
   {
      _commandBuffer.push_back(cmd);
   }
   
#ifdef USE_QTHREAD
#if 0
   void sleep(unsigned int mseconds)
   {
      clock_t goal = mseconds + clock();
      //	std::cout << "goal: " << goal << " clock: " << clock() << std::endl;
      while (goal > clock());
   }
#endif
#endif
}
