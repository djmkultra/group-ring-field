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

#ifndef _THREAD_WORKER_H
#define _THREAD_WORKER_H

#include <thread.h>
#include <command.h>

namespace gutz
{
   //////////////////////////////////////////////////////////////////////////////////
   /// Special version of thread. The entry point is a loop that checks for commands
   /// that have been added to its buffer and executes them. Loops until die() is
   /// called.
   ///
   /// Example:
   ///
   /// \code
   /// #include <threadGutz.h>
   /// #include <iostream>
   /// 
   /// using namespace gutz;
   ///
   /// class Work
   /// {
   /// public:
   ///    void doStuff(int i) { std::cout << i << std::endl; }
   /// };
   ///
   /// int main(void)
   /// {
   ///    Work work;
   ///    ThreadWorker thread;
   ///    thread.start();
   ///
   ///    for(int i = 0; i < 30; ++i)
   ///    { 
   ///       thread.addCommand(new_command(&work, &Work::doStuff, i));
   ///    }
   ///
   ///    thread.die();
   ///    thread.wait();
   ///    return 0;
   /// }
   /// \endcode
   
   class ThreadWorker : public gutz::Thread
	{
	public:
      typedef gutz::SmartPtr<ThreadWorker> pointer;
      typedef std::list<pointer>           container;
      typedef std::list<pointer>::iterator iterator;
      
		/// Constructor
		ThreadWorker(void) : _die(false) {}
      
		/// Destructor. Probably needs to iterate over the command buffer
		/// and free some stuff. Unless SmartPtr is used, in which case, it's all good.
		~ThreadWorker();
		
		/// Tell the loop to quit
		void die(void) { _die = true; }
      
		/// Add a command to the thread's command queue
 		void addCommand(Command* cmd);
		
		/// Check to see if the buffer is empty
		bool bufferEmpty(void) const { return _commandBuffer.empty(); }
		
	protected:
		/// Do it! Run the thread till we die.
		void run(void);
      
	private:
		bool _die;
      gutz::Command::container _commandBuffer;
	};
}
#endif
